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

#include "../interface/interfacemanager.h"
#include "../interface/list.h"
#include "../interface/textbox.h"

#include <cppirclib.h>

namespace ST
{
    IRCServer::IRCServer():
    mHostname(""), mNick(""), mClient(0),
    mRegistering(false), mRegistered(false)
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
        static_cast<TextBox*>(interfaceManager->getWindow("chat"))->addRow("Connecting...");
    }

    void IRCServer::process()
    {
        if (!mClient->isConnected())
            return;

        if (!mRegistered && !mRegistering && mNick.size() > 1)
        {
            mClient->doRegistration("test", mNick,
                                    "st 0 * :" "Small Towns 0.0.1");
            mRegistering = true;
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
                conCommand->setParams("#smalltowns");
                mClient->sendCommand(conCommand);
                static_cast<TextBox*>(interfaceManager->getWindow("chat"))->addRow("Connected!");
            } break;

            case IRC::Command::IRC_SAY:
            {
                int i = 0;
                std::string text = command->getUserInfo() + ": ";
                while (command->getParam(i) != "" && i < 255)
                {
                    text.append(command->getParam(i));
                    ++i;
                }
                static_cast<TextBox*>(interfaceManager->getWindow("chat"))->addRow(text);
            } break;

            case IRC::Command::IRC_NAMES:
            {
                std::string name;
                for (int i = 0; i < command->numParams(); ++i)
                {
                    name = command->getParam(i);
                    static_cast<List*>(interfaceManager->getWindow("userlist"))->addLabel(name);
                }
            } break;

            case IRC::Command::IRC_JOIN:
            {
                std::string name = command->getUserInfo();
                std::string msg = name + " connected";
                static_cast<List*>(interfaceManager->getWindow("userlist"))->addLabel(name);
                static_cast<TextBox*>(interfaceManager->getWindow("chat"))->addRow(msg);
            } break;

            case IRC::Command::IRC_PART:
            {
                std::string name = command->getUserInfo();
                std::string msg = name + " parted";
                static_cast<List*>(interfaceManager->getWindow("userlist"))->removeLabel(name);
                static_cast<TextBox*>(interfaceManager->getWindow("chat"))->addRow(msg);
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
