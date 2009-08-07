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
 *	Date of file creation: 09-02-03
 *
 *	$Id$
 *
 ********************************************/

#include "ircserver.h"
#include "ircmessage.h"

#include "../character.h"
#include "../game.h"
#include "../loginstate.h"
#include "../player.h"

#include "../interface/interfacemanager.h"

#include "../utilities/log.h"

#include <cppirclib.h>
#include <sstream>

namespace ST
{
    IRCServer::IRCServer():
    mClient(0),
    mRegistering(false), mRegistered(false), mAttempt(0)
    {
        mClient = new IRC::IRCClient();
        mClient->init();
    }

    IRCServer::~IRCServer()
    {
        delete mClient;
    }

    void IRCServer::connect(const std::string &hostname)
    {
        mHostname = hostname;
        mClient->connectTo(mHostname, 6667);
		interfaceManager->sendToChat("Connecting to chat server..");
    }

    void IRCServer::process()
    {
        if (!mClient->isConnected())
            return;

        if (!mRegistered && !mRegistering && mNick.size() > 1)
        {
			std::string pass = "test";
			std::string realname = "st 0 * :" "Towns Life 0.0.6";
            mClient->doRegistration(pass, mNick, realname);
            mRegistering = true;
            logger->logDebug("Registering with IRC server");
        }

        int packets = mClient->ping();

        if (packets > 0)
        {
            IRC::Command *command = mClient->getCommand();
            processMessage(command);
        }
    }

    void IRCServer::processMessage(IRC::Command *command)
    {
        switch (command->getCommand())
        {
            case IRC::Command::IRC_PING:
            {
                IRC::Command *pingCommand = new IRC::Command();
                pingCommand->setCommand(IRC::Command::IRC_PONG);
                std::string str = command->getParam(0);
                pingCommand->setParams(str);
                mClient->sendCommand(pingCommand);
            } break;

            case IRC::Command::IRC_CONNECT:
            {
                mRegistered = true;
                IRC::Command *conCommand = new IRC::Command;
                conCommand->setCommand(IRC::Command::IRC_JOIN);
                conCommand->setParams("#townslife");
                mClient->sendCommand(conCommand);

                interfaceManager->sendToChat("Connected!");

                logger->logDebug("Joining #townslife channel");
            } break;

            case IRC::Command::IRC_SAY:
            {
                std::string msg = command->getUserInfo() + ": ";
				msg.append(command->getMessage());

                interfaceManager->sendToChat(msg);
            } break;

            case IRC::Command::IRC_MSG:
            {
				std::string msg = command->getUserInfo() + ": ";
				msg.append(command->getMessage());

		 		interfaceManager->sendToChat(msg);
            } break;

            case IRC::Command::IRC_EMOTE:
            {
                std::string msg = "* ";
                msg.append(command->getUserInfo() + " ");
				msg.append(command->getMessage());

                interfaceManager->sendToChat(msg);
            } break;

            case IRC::Command::IRC_NOTICE:
            {
                std::string params = command->getMessage();
            } break;

            case IRC::Command::IRC_NAMES:
            {
                std::string name;
                for (int i = 0; i < command->numParams(); ++i)
                {
                    name = command->getParam(i);
//                    List *list = static_cast<List*>(interfaceManager->getWindow("userlist"));
//					if (list)
//						list->addLabel(name);
                }

                std::stringstream str;
                str << "There are " << command->numParams() << " users online.";
				interfaceManager->sendToChat(str.str());
            } break;

            case IRC::Command::IRC_JOIN:
            {
                std::string name = command->getUserInfo();
				if (name == mNick)
					return;
                std::string msg = name + " connected";

                interfaceManager->sendToChat(msg);
            } break;

            case IRC::Command::IRC_PART:
            case IRC::Command::IRC_QUIT:
            {
                std::string name = command->getUserInfo();
                std::string msg = name + " left the chat";

				interfaceManager->sendToChat(msg);
            } break;

			case IRC::Command::ERR_BADNICK:
			{
				GameState *state = new LoginState;
				game->changeState(state);

				interfaceManager->setErrorMessage("Invalid nick.");

                logger->logWarning("Invalid nickname used");
			} break;

			case IRC::Command::ERR_NICKINUSE:
			{
			    std::stringstream str;
			    str << player->getSelectedCharacter()->getName() << "[" << mAttempt << "]";
				mNick = str.str();
				mRegistering = false;
				mAttempt++;
				logger->logWarning("Nickname already used, using alternate nick");
			} break;
        }
    }

    void IRCServer::sendMessage(IRCMessage *msg)
	{
        IRC::Command *command = new IRC::Command;
        switch (msg->getType())
        {
            case IRCMessage::CHAT:
                command->setCommand(IRC::Command::IRC_SAY);
                break;

			case IRCMessage::EMOTE:
				command->setCommand(IRC::Command::IRC_EMOTE);
				break;
        }
        command->setParams(msg->getText());
        mClient->sendCommand(command);

        delete msg;
    }

    void IRCServer::setNick(const std::string &nick)
    {
        if (!mClient->isConnected())
            mNick = nick;

        // TODO: Send nick change to server
        if (mRegistered)
        {
            IRC::Command *command = new IRC::Command;
            command->setCommand(IRC::Command::IRC_NICK);
            command->setParams(nick);
            mClient->sendCommand(command);
        }
    }

    void IRCServer::quit()
    {
        if (!mClient->isConnected())
            return;
        IRC::Command *command = new IRC::Command;
        command->setCommand(IRC::Command::IRC_QUIT);
        mClient->sendCommand(command);
    }

    bool IRCServer::isConnected()
    {
        return mRegistered;
    }
}
