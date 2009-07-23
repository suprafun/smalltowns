/*********************************************
 *
 *	Author: David Athay
 *
 *	License: New BSD License
 *
 *	Copyright (c) 2008, The Small Towns Dev Team
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
 *	Date of file creation: 08-09-21
 *
 *	$Id$
 *
 ********************************************/

#include "teststate.h"
#include "loginstate.h"
#include "input.h"
#include "player.h"
#include "beingmanager.h"
#include "character.h"

#include "graphics/camera.h"
#include "graphics/graphics.h"
#include "graphics/node.h"

#include "interface/interfacemanager.h"

#include "net/networkmanager.h"

#include "irc/ircserver.h"
#include "irc/ircmessage.h"

#include <sstream>
#include <SDL.h>

namespace ST
{
    void submit_chat(AG_Event *event)
    {
        IRCServer *chatServer = static_cast<IRCServer*>(AG_PTR(1));
        if (chatServer->isConnected())
		{
		    AG_Textbox *input = static_cast<AG_Textbox*>(AG_PTR(2));
		    AG_Console *output = static_cast<AG_Console*>(AG_PTR(3));

            if (input && output)
            {
                std::string chat = AG_TextboxDupString(input);
                if (!chat.empty())
                {
					if (chat.substr(0, 1) != "/")
					{
						// send message to IRC
						IRCMessage *msg = new IRCMessage;
						msg->setType(IRCMessage::CHAT);
						msg->addString(chat);
						chatServer->sendMessage(msg);

						// add message to chat window
						chat.insert(0, player->getName() + ": ");
						AG_ConsoleAppendLine(output, chat.c_str());
					}
					else if (chat.substr(1, 3) == "me ")
					{
						IRCMessage *msg = new IRCMessage;
						msg->setType(IRCMessage::EMOTE);
						msg->addString(chat.substr(4));
						chatServer->sendMessage(msg);

						// add message to chat window
						chat = chat.substr(4);
						chat.insert(0, "* " + player->getName() + " ");
						AG_ConsoleAppendLine(output, chat.c_str());


					}
                }
				// clear input textbox
				AG_TextboxClearString(input);
            }
        }
    }

    void handle_mouse(Event *evt)
    {
        if (evt->button == SDL_BUTTON_LEFT)
        {
            Node *node = graphicsEngine->getNode(evt->x, evt->y);
            if (node)
            {
/*                Being *being = beingManager->findBeing(node->getId());
                if (being)
                {
                    being->toggleName();
                }
*/            }
        }
    }

	TestState::TestState()
	{
        chatServer = new IRCServer;

		// create camera
		Rectangle rect;
		rect.height = 768;
		rect.width = 1024;
		rect.x = -512;
		rect.y = 0;
		mCam = new Camera("viewport", &rect);
		graphicsEngine->setCamera(mCam);
	}

	void TestState::enter()
	{
		int screenWidth = graphicsEngine->getScreenWidth();
		int screenHeight = graphicsEngine->getScreenHeight();

		AG_Window *chatWindow = AG_WindowNewNamed(AG_WINDOW_NOBUTTONS, "ChatWindow");
		AG_WindowSetCaption(chatWindow, "Chat");
		AG_WindowSetSpacing(chatWindow, 5);
		AG_WindowSetGeometry(chatWindow, 10, screenHeight - 185, 400, 175);
		AG_WindowShow(chatWindow);

        AG_Notebook *book = AG_NotebookNew(chatWindow, AG_NOTEBOOK_EXPAND);
        AG_ObjectSetName(book, "ChannelsFolder");
        AG_NotebookTab *nbTab = AG_NotebookAddTab(book, "Global Chat", AG_BOX_VERT);
        AG_ObjectSetName(nbTab, "GlobalChat");

        AG_Console *console = AG_ConsoleNew(nbTab, AG_CONSOLE_EXPAND);
        AG_ObjectSetName(console, "Chat");

		AG_Textbox *chatInput = AG_TextboxNew(chatWindow, AG_TEXTBOX_CATCH_TAB, NULL);
		AG_TextboxSizeHint(chatInput, "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
		AG_SetEvent(chatInput, "textbox-return", submit_chat, "%p%p%p", chatServer, chatInput, console);

		// add elements to interface manager
		interfaceManager->addWindow(chatWindow);

		std::string nick = player->getName();
		std::string host = "london.uk.whatnet.org";
		chatServer->setNick(nick);
//        chatServer->connect(host);
	}

	void TestState::exit()
	{
		delete mCam;
		delete chatServer;
        interfaceManager->removeAllWindows();
	}

	bool TestState::update()
	{
		// Check for input, if escape pressed, exit
		if (inputManager->getKey(SDLK_ESCAPE))
		{
		    chatServer->quit();
			return false;
		}

		chatServer->process();

		SDL_Delay(0);

		return true;
	}
}
