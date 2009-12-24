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
#include "game.h"
#include "map.h"

#include "graphics/camera.h"
#include "graphics/graphics.h"
#include "graphics/node.h"

#include "interface/interfacemanager.h"

#include "net/networkmanager.h"
#include "net/packet.h"
#include "net/protocol.h"

#include "irc/ircserver.h"
#include "irc/ircmessage.h"

#include "utilities/log.h"

#include <sstream>
#include <SDL.h>

namespace ST
{
    void submit_chat(AG_Event *event)
    {
        IRCServer *chatServer = static_cast<IRCServer*>(AG_PTR(1));

        AG_Textbox *input = static_cast<AG_Textbox*>(AG_PTR(2));
        AG_Console *output = static_cast<AG_Console*>(AG_PTR(3));

        if (input && output)
        {
            std::string chat = AG_TextboxDupString(input);
            if (!chat.empty())
            {
                if (chat.substr(0, 1) != "/")
                {
                    if (!chatServer->isConnected())
                        return;
                    // send message to IRC
                    IRCMessage *msg = new IRCMessage;
                    msg->setType(IRCMessage::CHAT);
                    msg->addString(chat);
                    chatServer->sendMessage(msg);

                    // add message to chat window
                    chat.insert(0, player->getSelectedCharacter()->getName() + ": ");
                    interfaceManager->sendToChat(chat);
                }
                else if (chat.substr(1, 3) == "me ")
                {
                    if (!chatServer->isConnected())
                        return;
                    IRCMessage *msg = new IRCMessage;
                    msg->setType(IRCMessage::EMOTE);
                    msg->addString(chat.substr(4));
                    chatServer->sendMessage(msg);

                    // add message to chat window
                    chat = chat.substr(4);
                    chat.insert(0, "* " + player->getSelectedCharacter()->getName() + " ");
                    interfaceManager->sendToChat(chat);
                }
                else if (chat.substr(1) == "cam")
                {
                    Point camPos = graphicsEngine->getCamera()->getPosition();
                    std::stringstream info;
                    info << "Cam Info: " << camPos.x << "," << camPos.y;
                    interfaceManager->sendToChat(info.str());
                }
                else if (chat.substr(1) == "pos")
                {
                    Point tilePos = player->getSelectedCharacter()->getTilePosition();
                    std::stringstream posStr;
                    posStr << "Player Pos: " << tilePos.x << "," << tilePos.y;
                    interfaceManager->sendToChat(posStr.str());
                }
            }
            // clear input textbox
            AG_TextboxClearString(input);
        }
    }

    void handle_mouse(Event *evt)
    {
        if (!mapEngine->mapLoaded())
            return;

        Point camPos = graphicsEngine->getCamera()->getPosition();
        Point pos;
        pos.x = evt->x + camPos.x;
        pos.y = evt->y + camPos.y + mapEngine->getTileHeight();

        if (evt->button == SDL_BUTTON_LEFT && evt->type == 0)
        {
            // check user clicked on map
            Node *node = graphicsEngine->getNode(pos.x, pos.y);
            if (node)
            {
                Being *being = beingManager->findBeing(node->getName());
                if (being)
                {
                    // toggle being name
                    being->toggleName();
                    return;
                }

                // save destination for later
                player->getSelectedCharacter()->saveDestination(pos);

                // send move message
                std::stringstream str;
                str << "sending move " << pos.x << "," << pos.y
                    << " from mouse click at " << evt->x << "," << evt->y;
                logger->logDebug(str.str());
                Packet *p = new Packet(PGMSG_PLAYER_MOVE);
                p->setInteger(pos.x);
                p->setInteger(pos.y);
                networkManager->sendPacket(p);
            }
        }

		if (evt->button == 0)
		{
			//TODO: Add moving the tile cursor
//			Node *node = graphicsEngine->getTile(pos.x, pos.y);
		}
    }

	TestState::TestState()
	{
        ms = 0;
        lastframe = SDL_GetTicks();
        mLoaded = false;
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

		std::string nick = player->getSelectedCharacter()->getName();
		std::string host = "london.uk.whatnet.org";
		chatServer->setNick(nick);

#ifndef WITHOUT_CHAT
        chatServer->connect(host);
#endif

        // add listener
        interfaceManager->addMouseListener(&handle_mouse);
	}

	void TestState::exit()
	{
		delete mCam;
		delete chatServer;
        interfaceManager->removeAllWindows();
        interfaceManager->removeMouseListeners();
	}

	bool TestState::update()
	{
	    // check if the map has been loaded then tell the game server we're ready
	    if (!mLoaded && mapEngine->mapLoaded())
	    {
            Packet *packet = new Packet(PGMSG_MAP_LOADED);
            networkManager->sendPacket(packet);
            mLoaded = true;
	    }

		// Check for input, if escape pressed, exit
		if (inputManager->getKey(SDLK_ESCAPE))
		{
            Packet *p = new Packet(PGMSG_DISCONNECT);
            networkManager->sendPacket(p);
            game->disconnect();
		    chatServer->quit();
			return false;
		}

        // number of milliseconds since last frame
        ms = SDL_GetTicks() - lastframe;
        lastframe = SDL_GetTicks();

        // pass the number of milliseconds to logic
        beingManager->logic(ms);
        player->getSelectedCharacter()->logic(ms);
        player->logic(ms);

		chatServer->process();

		SDL_Delay(0);

		return true;
	}
}
