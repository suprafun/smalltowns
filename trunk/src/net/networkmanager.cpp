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

#include "../interface/label.h"
#include "../interface/interfacemanager.h"

#include "../gamestate.h"
#include "../game.h"
#include "../loginstate.h"
#include "../teststate.h"

namespace ST
{
	NetworkManager::NetworkManager()
	{
		enet_initialize();
		atexit(enet_deinitialize);
        mHost = new Host();
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
                    GameState *state = new LoginState;
                    game->changeState(state);
                }
                else
                {
                    disconnect();
                    static_cast<Label*>(interfaceManager->getWindow("error"))->setText("Error connecting: Invalid client");
                }
            } break;

        case APMSG_REGISTER_RESPONSE:
            {
                if (packet->getByte() == ERR_NONE)
                {
                    GameState *state = new TestState;
                    game->changeState(state);
                }
                else if (packet->getByte() == ERR_TAKEN_NAME)
                {
                    static_cast<Label*>(interfaceManager->getWindow("error"))->setText("Username already taken");
                }
                else
                {
                    static_cast<Label*>(interfaceManager->getWindow("error"))->setText("Invalid username or password");
                }
            } break;

        case APMSG_LOGIN_RESPONSE:
            {
                if (packet->getByte() == ERR_NONE)
                {
                    GameState *state = new TestState;
                    game->changeState(state);
                }
                else
                {
                    static_cast<Label*>(interfaceManager->getWindow("error"))->setText("Error: Invalid username or password");
                }
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
}
