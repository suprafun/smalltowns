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


#include "graphics/camera.h"
#include "graphics/graphics.h"
#include "graphics/node.h"
//#include "utilities/log.h"

#include "interface/icon.h"
#include "interface/interfacemanager.h"
#include "interface/list.h"
#include "interface/scrollbox.h"
#include "interface/textbox.h"
#include "interface/textfield.h"
#include "interface/window.h"

#include "net/networkmanager.h"

#include "irc/ircserver.h"
#include "irc/ircmessage.h"

#include <sstream>
#include <SDL.h>

/*
const int tilewidth = 42;
const int halftilewidth = 21;
const int tileheight = 22;
const int halftileheight = 11;
*/

namespace ST
{
	TestState::TestState()
	{
        chatServer = new IRCServer;
		// create camera
		Rectangle rect;
		rect.height = 768;
		rect.width = 1024;
		rect.x = 0;
		rect.y = 0;
		mCam = new Camera("viewport", &rect);
		graphicsEngine->setCamera(mCam);
	}

	void TestState::enter()
	{
	    /*
		mFrames = 0;
		mTime = 0;
		mLastTime = 0;

		// Create Viewport
		Rectangle rect;
		rect.x = 0;
		rect.y = 0;
		rect.width = 640;
		rect.height = 480;
		cam = new Camera("Default", &rect);

		// Set camera
		graphicsEngine->setCamera(cam);

		// Load in images
		if (!graphicsEngine->loadSpriteSheet("grass.png"))
		{
			// error
			logger->logError("Unable to load sprite sheet");
			return;
		}

		if (!graphicsEngine->loadSpriteSheet("base.png"))
		{
			// error
			logger->logError("Unable to load sprite sheet");
			return;
		}

		// Create Test Nodes
		Point p;
		p.x = 0;
		p.y = 0;

		int mapSize = ((float)rect.width / tilewidth) * ((float)rect.height / halftileheight);
		int row = 1;
		for (int i = 0; i < mapSize; ++i)
		{
			p.x += tilewidth;
			if (p.x > rect.width)
			{
				if (row % 2)
				{
					p.x = halftilewidth;
				}
				else
				{
					p.x = 0;
				}

				p.y += halftileheight;
				++row;
			}

			std::stringstream stream;
			stream << "Test" << i;
			Node *node = graphicsEngine->createNode(stream.str(), "grass.png", &p);
		}

		p.x = 0;
		p.y = 84;
		Node *node = graphicsEngine->createNode("player", "base.png", &p);
		*/
		int screenWidth = graphicsEngine->getScreenWidth();
		int screenHeight = graphicsEngine->getScreenHeight();

		// create window for chat
		Window *win = new Window("Chat Window");
		win->setPosition(75, screenHeight - 50);
		win->setSize(screenWidth - 150, screenHeight - 100);
		interfaceManager->addWindow(win);

		// create scroll box for chat box
		ScrollBox *scrollBox = new ScrollBox("scrollbox");
		scrollBox->setPosition(125, screenHeight - 100);
		scrollBox->setSize(win->getWidth() - 200, screenHeight - 200);
		interfaceManager->addSubWindow(win, scrollBox);

		// create textbox in non-edit mode for chat
		TextBox *chatBox = new TextBox("chat");
		chatBox->setPosition(125, screenHeight - 100);
		chatBox->setSize(win->getWidth() - 200, screenHeight - 200);
		chatBox->setRows(11);
		chatBox->setFontSize(18);
		interfaceManager->addSubWindow(win, chatBox);

		// create textfield for sending chat
		Icon *chatIcon = new Icon("chatbubble");
		chatIcon->setPosition(139, 85);
		chatIcon->setSize(27, 21);
		chatIcon->setBackground("icon.png");
		interfaceManager->addSubWindow(win, chatIcon);

		TextField *chatField = new TextField("sendchat");
		chatField->setPosition(174, 85);
		chatField->setSize(win->getWidth() - 249, 21);
		chatField->setFontSize(16);
		interfaceManager->addSubWindow(win, chatField);

		// create userlist
		List *list = new List("userlist");
		list->setPosition(screenWidth - 195, screenHeight - 100);
		list->setSize(85, 400);
		list->setFontSize(18);
		interfaceManager->addWindow(list);

		std::string nick = player->getName();
		std::string host = "london.uk.whatnet.org";
		chatServer->setNick(nick);
        chatServer->connect(host);
	}

	void TestState::exit()
	{
		delete mCam;
        interfaceManager->removeAllWindows();
	}

	bool TestState::update()
	{
	    /*
		// Add another frame
		// Get how much time has passed
		// Find the difference and if its a second or more
		// Or the time has travelled backwards (possible if time wraps)
		// Output the number of frames to log
		mFrames++;
		mTime = SDL_GetTicks();
		int diff = mTime - mLastTime;
		if (diff >= 1000 || diff < 0)
		{
			std::stringstream str;
			str << mFrames;
			logger->logDebug(str.str() + " frames per second");
			mFrames = 0;
			mLastTime = mTime;
		}
		*/

		if (inputManager->getKey(SDLK_RETURN))
		{
		    if (chatServer->isConnected())
		    {
                std::string chat = static_cast<TextField*>(interfaceManager->getWindow("sendchat"))->getText();
                if (chat.size() > 0)
                {
                    IRCMessage *msg = new IRCMessage;
                    msg->setType(IRCMessage::CHAT);
                    msg->addString(chat);
                    chatServer->sendMessage(msg);
                    chat.insert(0, player->getName() + ": ");
                    static_cast<TextBox*>(interfaceManager->getWindow("chat"))->addRow(chat);
                    static_cast<TextField*>(interfaceManager->getWindow("sendchat"))->setText("");
                }
		    }
		    else
		    {
		        static_cast<TextBox*>(interfaceManager->getWindow("chat"))->addRow("You aren't connected yet!");
		    }
		}

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
