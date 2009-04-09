/*********************************************
 *
 *	Author: David Athay
 *
 *	License: New BSD License
 *
 *	Copyright (c) 2009, The Small Towns Dev Team
 *	All rights reserved.
 *
 *	Redistribution and use in source and binary forms, with or without modification,
 *	are permitted provided that the following conditions are met:
 *
 *	- Redistributions of source code must retain the above copyright notice,
 *		this list of conditions and the following disclaimer.
 *	- Redistributions in binary form must reproduce the above copyright notice,
 *		this list of conditions and the following disclaimer in the documentation
 *		and/or other materials provided with the distribution.
 *	- Neither the name of the Small Towns Dev Team nor the names of its contributors
 *		may be used to endorse or promote products derived from this software without
 *		specific prior written permission.
 *
 *	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *	ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *	IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 *	INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *	BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 *	OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *	LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 *	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 *	THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 *	Date of file creation: 09-01-22
 *
 *	$Id$
 *
 ********************************************/

#include "networkmanager.h"
#include "host.h"
#include "packet.h"
#include "protocol.h"

#include "../interface/interfacemanager.h"

#include "../utilities/log.h"

#include "../character.h"
#include "../characterstate.h"
#include "../gamestate.h"
#include "../game.h"
#include "../loginstate.h"
#include "../map.h"
#include "../player.h"
#include "../teststate.h"
#include "../updatestate.h"

#include <curl/curl.h>
#include <sstream>

namespace ST
{
	NetworkManager::NetworkManager()
	{
		enet_initialize();
		atexit(enet_deinitialize);
        mHost = new Host();
	}

	NetworkManager::~NetworkManager()
	{
	    if (mHost->isConnected())
            disconnect();
	}

	void NetworkManager::connect(const std::string &hostname, unsigned int port)
	{
	    mHost->connect(hostname, port);
	}

	void NetworkManager::process()
	{
        mHost->process();

        Packet *packet = mHost->getPacket();
        if (packet)
        {
            processPacket(packet);
			delete packet;
			packet = NULL;
        }
	}

	void NetworkManager::processPacket(Packet *packet)
	{
	    switch(packet->getId())
	    {
        case APMSG_CONNECT_RESPONSE:
            {
                if (packet->getByte() == ERR_NONE)
                {
                    logger->logDebug("Connected to Account server");
                    GameState *state = new LoginState;
                    game->changeState(state);
                }
                else
                {
                    logger->logError("Bad client");
                    disconnect();
                }
            } break;

        case APMSG_REGISTER_RESPONSE:
            {
                if (packet->getByte() == ERR_NONE)
                {
                    logger->logDebug("Registered new account");
                    GameState *state = new UpdateState;
                    game->changeState(state);
                }
                else if (packet->getByte() == ERR_TAKEN_NAME)
                {
                    logger->logWarning("Username already registered");
                }
                else
                {
                    logger->logWarning("Invalid username or password used to register account");
                }
            } break;

        case APMSG_LOGIN_RESPONSE:
            {
                if (packet->getByte() == ERR_NONE)
                {
                    logger->logDebug("Logged in successfully");
                    UpdateState *state = new UpdateState;
                    game->changeState(state);
                }
                else
                {
                    logger->logWarning("Invalid username or password used to login");
                }
            } break;

        case APMSG_CHAR_LIST_RESPONSE:
            {
                int count = packet->getInteger();
                for (int i = 0; i < count; ++i)
                {
                    int id = packet->getInteger();
                    std::string name = packet->getString();
                    Character *c = new Character(id, name);
                    c->setHead(packet->getInteger());
                    c->setLevel(packet->getInteger());
                    c->setRights(packet->getInteger());
                    player->addCharacter(c);
                }

                std::stringstream str;
                str << "Found " << count << " character.";
                logger->logDebug(str.str());

                GameState *state = new CharacterState;
                game->changeState(state);

            } break;

        case APMSG_CHAR_CREATE_RESPONSE:
            {
                if (packet->getByte() == ERR_NONE)
                {
                    GameState *state = new TestState;
                    game->changeState(state);
                }
                else
                {
                    // TODO: Indicate error creating character
                }
            } break;

        case APMSG_CHAR_CHOOSE_RESPONSE:
            {
                if (packet->getByte() == ERR_NONE)
                {
                    int id = packet->getInteger();
                    player->setId(id);
                }
                else
                {
                    // TODO: Indicate error creating character
                }
            } break;

        case APMSG_GAME_SERVER:
            {
                std::string host = packet->getString();
                int port = packet->getInteger();
                int tag = packet->getInteger();

                disconnect();

                while (mHost->isConnected())
                    mHost->process();

                connect(host, port);

                while (!mHost->isConnected())
                    mHost->process();

                Packet *p = new Packet(PGMSG_CONNECT);
                p->setInteger(player->getId());
                p->setInteger(tag);
                sendPacket(p);
            } break;


        case GPMSG_CONNECT_RESPONSE:
            {
                if (packet->getByte() == ERR_NONE)
                {
                    GameState *state = new TestState;
                    game->changeState(state);
                }
                else
                {
                    // TODO: Indicate error connecting to game server
                }
            } break;

        case GPMSG_LOAD_MAP:
            {
                std::string mapFile = packet->getString();
                mapEngine->loadMap(mapFile);
            } break;
	    }
	}

	bool NetworkManager::isConnected()
	{
	    return mHost->isConnected();
	}

	void NetworkManager::disconnect()
	{
	    mHost->disconnect();
	    // try to disconnect right away
	    mHost->process();
	}

	void NetworkManager::sendPacket(Packet *packet)
	{
	    mHost->sendPacket(packet);
	    delete packet;
	}

	void NetworkManager::sendVersion()
	{
	    Packet *packet = new Packet(PAMSG_CONNECT);
        packet->setInteger(CLIENT_VERSION);
        sendPacket(packet);
	}

	bool NetworkManager::downloadUpdateFile(const std::string &file)
	{
	    CURLcode success;

	    // open file for writing to
	    FILE *outFile;
	    outFile = fopen(file.c_str(), "w");

	    // url to download from
	    std::string url = "http://www.casualgamer.co.uk/";
	    url.append(file);

	    // initialise curl
        CURL *handle = curl_easy_init();

		if (handle)
		{

			// set curl options
/*			curl_easy_setopt(handle, CURLOPT_WRITEDATA, outFile);
			curl_easy_setopt(handle, CURLOPT_URL, url.c_str());

			// perform the download
			if ((success = curl_easy_perform(handle)) != 0)
			{
				// error! - log it and clean up
				logger->logError("Unable to get update file");
				fclose(outFile);
				curl_easy_cleanup(handle);
				return false;
			}
*/
			// cleanup
			curl_easy_cleanup(handle);
		}

        fclose(outFile);

        return true;
	}
}
