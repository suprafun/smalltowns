/*********************************************
 *
 *	Author: David Athay
 *
 *	License: New BSD License
 *
 *	Copyright (c) 2009, CT Games
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
 *	- Neither the name of CT Games nor the names of its contributors
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

#include "../graphics/camera.h"
#include "../graphics/graphics.h"
#include "../graphics/texture.h"

#include "../interface/interfacemanager.h"

#include "../resources/bodypart.h"

#include "../utilities/log.h"
#include "../utilities/stringutils.h"

#include "../beingmanager.h"
#include "../character.h"
#include "../characterstate.h"
#include "../connectstate.h"
#include "../gamestate.h"
#include "../game.h"
#include "../loadstate.h"
#include "../loginstate.h"
#include "../map.h"
#include "../player.h"
#include "../resourcemanager.h"
#include "../teststate.h"
#include "../updatestate.h"

#include <curl/curl.h>
#include <sstream>

#ifdef WIN32
#include <time.h>
#endif

namespace ST
{
    size_t downloadData( void *buffer, size_t size, size_t nmemb, void *ptr)
    {
        int segsize = size * nmemb;
        char buf[65535];

        if (segsize >= 65535)
            return 0;

        /* Copy data to char */
        memcpy((void*)&buf[0], buffer, segsize);
        buf[segsize] = 0;

        /* Call function to output data to my file */
        static_cast<NetworkManager*>(ptr)->saveData(buf, segsize);

        /* Return the number of bytes received, indicating to curl that all is okay */
        return segsize;
    }

	NetworkManager::NetworkManager()
	{
		enet_initialize();
		atexit(enet_deinitialize);
        mHost = new Host();
        mTag = -1;
        mPing = 0;
        mNumDownloads = 0;
        mPos = 0;
	}

	NetworkManager::~NetworkManager()
	{
	    if (mHost->isConnected())
            disconnect();
        delete mHost;
	}

	void NetworkManager::connect()
	{
	    mHost->connect(mDefaultHost, mDefaultPort);

	    // log where they tried connecting
        std::stringstream msg;
        msg << "Connecting to server: " << mDefaultHost << ":" << mDefaultPort;
        logger->logDebug(msg.str());
	}

	void NetworkManager::connect(const std::string &hostname, int port)
	{
	    mHost->connect(hostname, port);

	    // log where they tried connecting
        std::stringstream msg;
        msg << "Connecting to server: " << hostname << ":" << port;
        logger->logDebug(msg.str());
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
					interfaceManager->setErrorMessage("Invalid client version, please update.");
                    interfaceManager->showErrorWindow(true);
                    logger->logError("Bad client");
                    disconnect();
                }
            } break;

        case APMSG_REGISTER_RESPONSE:
            {
                int result = packet->getByte();
                if (result == ERR_NONE)
                {
                    logger->logDebug("Registered new account");
                    GameState *state = new UpdateState;
                    game->changeState(state);
                }
                else if (result == ERR_TAKEN_NAME)
                {
					interfaceManager->setErrorMessage("Username taken.");
                    logger->logWarning("Username already registered");
                    interfaceManager->showErrorWindow(true);
                }
                else if (result == ERR_INVALID_PASS)
                {
                    interfaceManager->setErrorMessage("Password invalid.");
                    logger->logWarning("Problem with encryption? Invalid Password");
                    interfaceManager->showErrorWindow(true);
                }
                else
                {
					interfaceManager->setErrorMessage("Invalid username or password");
                    logger->logWarning("Invalid username or password used to register account");
                    interfaceManager->showErrorWindow(true);
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
					interfaceManager->setErrorMessage("Invalid username or password");
                    logger->logWarning("Invalid username or password used to login");
                    interfaceManager->showErrorWindow(true);
                }
            } break;

        case APMSG_CHANGE_PASS_RESPONSE:
            {
                if (packet->getByte() == ERR_NONE)
                {
                    logger->logDebug("Changed Password");
                }
                else
                {
					interfaceManager->setErrorMessage("Invalid password");
                    logger->logWarning("Invalid password used to change password");
                    interfaceManager->showErrorWindow(true);
                }
            } break;

        case APMSG_CHAR_LIST_RESPONSE:
            {
                // must create state before creating avatars
                // as the state loads the resources used
                GameState *state = new CharacterState;

                int count = packet->getInteger();
                for (int i = 0; i < count; ++i)
                {
                    std::map<int, int> Ids;
                    int id = packet->getInteger();
                    int slot = packet->getInteger();
                    std::string name = packet->getString();
                    Ids[PART_BODY] = packet->getInteger();
                    Ids[PART_HAIR] = packet->getInteger();
                    Ids[PART_CHEST] = packet->getInteger();
                    Ids[PART_LEGS] = packet->getInteger();
                    Ids[PART_FEET] = packet->getInteger();
                    Texture *avatar = graphicsEngine->createAvatar(slot, Ids, DIRECTION_SOUTHEAST);
                    Character *c = new Character(id, name, avatar);
                    c->look.body = Ids[PART_BODY];
                    c->look.hair = Ids[PART_HAIR];
                    c->setLevel(packet->getInteger());
                    c->setRights(packet->getInteger());
                    player->addCharacter(c, slot);
                }

                std::stringstream str;
                str << "Found " << count << " characters.";
                logger->logDebug(str.str());

                // can now change state
                game->changeState(state);

            } break;

        case APMSG_CHAR_CREATE_RESPONSE:
            {
                int error = packet->getByte();
                if (error == ERR_NONE)
                {
                    std::map<int, int> Ids;
					int id = packet->getInteger();
					player->setId(id);

                    int slot = packet->getInteger();
                    std::string name = packet->getString();
					int charId = packet->getInteger();
					Ids[PART_BODY] = packet->getInteger();
                    Ids[PART_HAIR] = packet->getInteger();
                    Ids[PART_CHEST] = packet->getInteger();
                    Ids[PART_LEGS] = packet->getInteger();
                    Ids[PART_FEET] = packet->getInteger();

					Texture *avatar = graphicsEngine->createAvatar(charId, Ids, DIRECTION_SOUTHEAST);
					Character *c = new Character(charId, name, avatar);
					c->look.body = Ids[PART_BODY];
                    c->look.hair = Ids[PART_HAIR];
                    c->look.chest = Ids[PART_CHEST];
                    c->look.legs = Ids[PART_LEGS];
                    c->look.feet = Ids[PART_FEET];
                    c->setLevel(packet->getInteger());
                    c->setRights(packet->getInteger());
                    player->addCharacter(c, slot);
                    player->setCharacter(slot);
                }
                else
                {
					interfaceManager->setErrorMessage("Invalid character");
                    logger->logWarning("Invalid character: Error " + utils::toString(error));
                    interfaceManager->showErrorWindow(true);
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
                    logger->logWarning("Invalid character chosen");
					interfaceManager->setErrorMessage("Invalid character chosen");
                    interfaceManager->showErrorWindow(true);
                    GameState *state = new CharacterState;
                    game->changeState(state);
                }
            } break;

        case APMSG_CHAR_DELETE_RESPONSE:
            {
                if (packet->getByte() == ERR_NONE)
                {
                    player->removeCharacters();
                    Packet *packet = new Packet(PAMSG_CHAR_LIST);
                    sendPacket(packet);
                }
                else
                {
                    logger->logWarning("Invalid character deleted");
					interfaceManager->setErrorMessage("Invalid character deleted");
                    interfaceManager->showErrorWindow(true);
                    GameState *state = new CharacterState;
                    game->changeState(state);
                }
            } break;

        case APMSG_GAME_SERVER:
            {
                std::string host = packet->getString();
                int port = packet->getInteger();
                mTag = packet->getInteger();

                game->disconnect();

				if (game->connect(host, port))
                {
                    GameState *state = new LoadState;
                    game->changeState(state);
                }
                else
                {
                    interfaceManager->setErrorMessage("Unable to connect to Game Server");
					interfaceManager->showErrorWindow(true);
                    logger->logWarning("Timed out connecting to game server.");
                    game->disconnect();
                    GameState *state = new ConnectState;
                    game->changeState(state);
                }
            } break;


        case GPMSG_CONNECT_RESPONSE:
            {
                int error = packet->getByte();
                if (error == ERR_NONE)
                {
					logger->logDebug("Connected to game server.");
                    GameState *state = new TestState;
                    game->changeState(state);
                }
                else if (error == ERR_INVALID_TAG)
                {
                    interfaceManager->setErrorMessage("Unable to connect to Game Server");
					interfaceManager->showErrorWindow(true);
                    logger->logWarning("Invalid token");
                    disconnect();
                    GameState *state = new ConnectState;
                    game->changeState(state);
                }
            } break;

        case GPMSG_LOAD_MAP:
            {
                std::string mapFile = packet->getString();

                if (mapEngine->loadMap(mapFile))
                    mapEngine->getLayer(mapEngine->getLayers() - 1)->addNode(player->getSelectedCharacter());
            } break;

        case GPMSG_WARPTO:
            {
                Point pt, pos;
                pt.x = packet->getInteger();
                pt.y = packet->getInteger();

                pos = mapEngine->convertTileToPixel(pt);
                Character *c = player->getSelectedCharacter();
                c->moveNode(&pos);

                graphicsEngine->warpCamera(pos);
                graphicsEngine->sort();
            } break;

        case GPMSG_PLAYER_MOVE:
            {
                Being *being = player->getSelectedCharacter();
                if (packet->getByte() == ERR_INVALID_POS)
                {
                    // TODO: Do something
                    being->setAnimation("");
                    being->setState(STATE_IDLE);
                    logger->logDebug("Server says Invalid move");
                }
                else
                {
                    //logger->logDebug("Got player move");
                    being->setState(STATE_MOVING);
                    being->calculateNextDestination();
                }
            } break;

        case GPMSG_PLAYER_MOVE_UPDATE:
            {
                // check if we know the player exists
                unsigned int id = packet->getInteger();
                Point pt, finish;
                finish.x = packet->getInteger();
                finish.y = packet->getInteger();
                int dir = packet->getInteger();
                Being *being = beingManager->findBeing(id);
                if (being)
                {
                    // found being, update their position
					//logger->logDebug("Being moving");
                    being->setState(STATE_MOVING);
                    being->calculateNextDestination(finish);
                }
                else if (player->getSelectedCharacter()->getId() == id)
                {
                    pt = mapEngine->convertTileToPixel(finish);
                    graphicsEngine->setCameraToShow(pt, 0);
                }
                else
                {
                    // get player info, save the data for later
                    Packet *p = new Packet(PGMSG_PLAYER_INFO);
                    p->setInteger(id);
                    sendPacket(p);
                    beingManager->saveBeingInfo(id, finish, dir);

                    std::stringstream str;
                    str << "New player with id " << id << " entered";
                    logger->logDebug(str.str());
                }
            } break;

        case GPMSG_PLAYER_INFO_RESPONSE:
            {
                // check if being already exists
                std::map<int, int> Ids;
                unsigned int id = packet->getInteger();
                std::string name = packet->getString();
                Ids[PART_BODY] = packet->getInteger();
                Ids[PART_HAIR] = packet->getInteger();
                Ids[PART_CHEST] = packet->getInteger();
                Ids[PART_LEGS] = packet->getInteger();
                Ids[PART_FEET] = packet->getInteger();
				int lvl = packet->getInteger();
				int rights = packet->getInteger();

                Being *being = beingManager->findBeing(id);
                if (being == NULL)
                {
                    // create new being based on info
                    Point pt = beingManager->getSavedDestination(id);
                    Point pos = mapEngine->convertTileToPixel(pt);
                    int dir = beingManager->getSavedDirection(id);

                    Texture *avatar = graphicsEngine->createAvatar(id, Ids, DIRECTION_SOUTHEAST);
                    Character *c = new Character(id, name, avatar);
                    c->look.body = Ids[PART_BODY];
                    c->look.hair = Ids[PART_HAIR];
                    c->look.chest = Ids[PART_CHEST];
                    c->look.legs = Ids[PART_LEGS];
                    c->look.feet = Ids[PART_FEET];
                    c->setLevel(lvl);
                    c->setRights(rights);
                    c->moveNode(&pos);
                    c->turnNode(dir);

                    beingManager->addBeing(c);
                    mapEngine->getLayer(mapEngine->getLayers() - 1)->addNode(c);
                    graphicsEngine->sort();

                    std::stringstream str;
                    str << "New player info from id " << id << " received";
                    logger->logDebug(str.str());
                }
                else
                {
                    std::stringstream str;
                    str << "Being already exists with id " << id;
                    logger->logDebug(str.str());
                }
            } break;

            case GPMSG_PLAYER_LEFT:
            {
                unsigned int id = packet->getInteger();
                Being *being = beingManager->findBeing(id);
                if (being)
                {
                    mapEngine->getLayer(mapEngine->getLayers() - 1)->removeNode(being);
                    beingManager->removeBeing(id);
                    graphicsEngine->sort();
                }
				logger->logDebug("Player Left");
            } break;

            case GPMSG_PING:
            {
                Packet *p = new Packet(PGMSG_PONG);
                sendPacket(p);
            } break;

            case GPMSG_PINGTIME:
            {
                mPing = packet->getInteger();
            } break;

            default:
            {
                std::stringstream str;
                str << "Received data with invalid id. Id was " << packet->getId()
                    << " and size was " << packet->getSize();
                logger->logWarning(str.str());
            } break;
	    }
	}

	bool NetworkManager::isConnected()
	{
	    return mHost->isConnected();
	}

	void NetworkManager::disconnect()
	{
	    if (isConnected())
	    {
            mHost->disconnect();
	    }

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
	    std::stringstream str;
	    str << "Client version: " << CLIENT_VERSION;
		logger->logDebug(str.str());
	    Packet *packet = new Packet(PAMSG_CONNECT);
        packet->setInteger(CLIENT_VERSION);
        sendPacket(packet);
	}

	bool NetworkManager::downloadFile(const std::string &hostname, const std::string &file)
	{
	    if (hostname.empty() || file.empty())
            return false;

	    CURLcode success;

	    // open file for writing to
	    FILE *outFile = NULL;

		std::string fullpath = resourceManager->getWritablePath() + file;

	    outFile = fopen(fullpath.c_str(), "wb+");
		if (outFile == NULL)
		{
			logger->logDebug("Failed to open file for downloading at " + fullpath);
			return false;
		}

	    // url to download from
	    std::string url = "http://" + hostname + "/";
	    url.append(file);

	    // initialise curl
        CURL *handle = curl_easy_init();

		if (handle != NULL)
		{

			// set curl options
			curl_easy_setopt(handle, CURLOPT_WRITEDATA, this);
			curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, downloadData);
			curl_easy_setopt(handle, CURLOPT_URL, url.c_str());

			// perform the download
			if ((success = curl_easy_perform(handle)) != 0)
			{
				// error! - log it and clean up
				logger->logError("Unable to get file: " + file);
				fclose(outFile);
				curl_easy_cleanup(handle);
				return false;
			}

			// cleanup
			curl_easy_cleanup(handle);
		}

		fwrite(mFileData, 1, mPos, outFile);

        fclose(outFile);

        free(mFileData);
        mFileData = NULL;
        mPos = 0;

        return true;
	}

	int NetworkManager::downloadContent(const std::string &hostname)
	{
	    // download list of updates
        if (!downloadFile(hostname, "updates.txt"))
        {
            logger->logError("Unable to download updates file.");
            return -1;
        }

	    // read list of updates
        std::string updateList = utils::readFile(resourceManager->getWritablePath() + "updates.txt");

        if (updateList.empty())
            return 0;

	    // check if we already have updates
	    // for now just check filename, in future, use MD5 hash
	    while (!updateList.empty())
	    {
	        size_t size = updateList.find_first_of("\n");
	        if (size == std::string::npos)
	        {
                size = updateList.size();
	        }
	        std::string str = updateList.substr(0, size);
	        // check if exists else download
	        if (resourceManager->doesExist(str))
	        {
	            logger->logDebug("File already exists: " + str);
	        }
	        else if (!downloadFile(hostname, str))
            {
                logger->logError("Unable to download: " + str);
                return -1;
            }

            resourceManager->addPath(resourceManager->getDataPath(str));
	        updateList = updateList.substr(size + 1);

	        ++mNumDownloads;
	    }

	    return mNumDownloads;
	}

	void NetworkManager::setDefault(const std::string &hostname, int port)
	{
	    mDefaultHost = hostname;
	    mDefaultPort = port;
	}

	void NetworkManager::sendPositionUpdate(const Point &pos)
	{
	    Packet *p = new Packet(PGMSG_PLAYER_MOVE_UPDATE);
	    p->setInteger(pos.x);
	    p->setInteger(pos.y);
	    sendPacket(p);
	}

	int NetworkManager::getTag() const
	{
	    return mTag;
	}

	int NetworkManager::getPing() const
	{
	    return mPing;
	}

	unsigned int NetworkManager::getTotalDownloads() const
	{
	    return mNumDownloads;
	}

	void NetworkManager::saveData(char *buffer, int size)
	{
	    if (mPos == 0)
            mFileData = (char*)malloc(size + 1);
        else
        {
            char *buf;
            buf = (char*)malloc(mPos);
            memcpy(buf, mFileData, mPos);
            mFileData = (char*)realloc(mFileData, mPos + size + 1);
            memcpy(mFileData, buf, mPos);
            free(buf);
        }
        memcpy(&mFileData[mPos], buffer, size);
        mPos += size;
	}
}
