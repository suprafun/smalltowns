/*********************************************
 *
 *	Author: David Athay
 *
 *	License: New BSD License
 *
 *	Copyright (c) 2008, CT Games
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
 *	Date of file creation: 08-09-21
 *
 *	$Id$
 *
 ********************************************/

#include "teststate.h"
#include "connectstate.h"
#include "input.h"
#include "player.h"
#include "beingmanager.h"
#include "character.h"
#include "game.h"
#include "map.h"
#include "resourcemanager.h"

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
#include "utilities/stringutils.h"
#include "utilities/types.h"

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
                else if (chat.substr(1) == "ping")
                {
                    interfaceManager->sendToChat(utils::toString(networkManager->getPing()));
                }
                else if (chat.substr(1) == "fps")
                {
                    std::stringstream fpsStr;
                    fpsStr << "Average FPS: " << graphicsEngine->getFPS();
                    interfaceManager->sendToChat(fpsStr.str());
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

        // left mouse button has finished being pressed
        if (evt->button == SDL_BUTTON_LEFT && evt->type == 1)
        {
            // check user clicked on map
            Node *node = graphicsEngine->getNode(pos.x, pos.y);
            if (node)
            {
                // show name if player/NPC is clicked
                Being *being = beingManager->findBeing(node->getName());
                if (being)
                {
                    // toggle being name
                    being->toggleName();
                    if (being->isNPC() && !being->isTalking())
                    {
                        Packet *p = new Packet(PGMSG_NPC_START_TALK);
                        p->setInteger(being->getId());
                        networkManager->sendPacket(p);
                    }
                    return;
                }

                Point pt = mapEngine->convertPixelToTile(pos.x, pos.y);

                if (mapEngine->blocked(pt))
                    return;

                // send move message
                Packet *p = new Packet(PGMSG_PLAYER_MOVE);
                p->setInteger(pt.x);
                p->setInteger(pt.y);
                networkManager->sendPacket(p);
                //logger->logDebug("Sending move request");

                // save destination for later
                player->getSelectedCharacter()->saveDestination(pt);
            }
        }

		if (evt->button == 0)
		{
		    if (!interfaceManager->getMouse()->cursor)
                return;

//            pos.x += mapEngine->getTileWidth() >> 1;

		    int mapWidth = mapEngine->getWidth() * mapEngine->getTileWidth();
		    int mapHeight = mapEngine->getHeight() * mapEngine->getTileHeight();

		    if (pos.x > mapWidth || -pos.x > mapWidth)
		        return;
		    if (pos.y > mapHeight || pos.y < 0)
		        return;

            Point tilePos = mapEngine->convertPixelToTile(pos.x, pos.y);

		    if (tilePos.x < 0 || tilePos.y < 0)
                return;

		    if (tilePos.x == interfaceManager->getMouse()->cursorPos.x && tilePos.y == interfaceManager->getMouse()->cursorPos.y)
                return;

            interfaceManager->getMouse()->cursorPos = tilePos;

		    Point screenPos;
		    screenPos.x = 0.5 * (tilePos.x - tilePos.y) * mapEngine->getTileWidth();
            screenPos.y = 0.5 * (tilePos.x + tilePos.y) * mapEngine->getTileHeight();
			interfaceManager->getMouse()->cursor->moveNode(&screenPos);
		}
    }

    void handle_logout(AG_Event *event)
    {
        IRCServer *chatServer = static_cast<IRCServer*>(AG_PTR(1));
        int *left = static_cast<int*>(AG_PTR(2));
        int logout = AG_INT(3);

        // disconnect from game server
        Packet *p = new Packet(PGMSG_DISCONNECT);
        networkManager->sendPacket(p);
        networkManager->process();
        game->disconnect();
        chatServer->quit();

        if (logout == 1)
        {
            // connect to account server
            GameState *gs = new ConnectState;
            game->changeState(gs);
        }
        else
        {
            *left = 1;
        }
    }

	TestState::TestState()
	{
        ms = 0;
        mTime = 0;
        lastframe = SDL_GetTicks();
        mLoaded = false;
        mLeft = 0;
        chatServer = new IRCServer;

        // load glowing tile
        resourceManager->loadGlowingTiles();

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

        AG_Console *console = AG_ConsoleNew(nbTab, AG_CONSOLE_EXPAND|AG_CONSOLE_AUTOSCROLL);
        AG_ObjectSetName(console, "Chat");

		AG_Textbox *chatInput = AG_TextboxNewS(chatWindow, AG_TEXTBOX_CATCH_TAB, "");
		AG_TextboxSizeHint(chatInput, "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
		AG_SetEvent(chatInput, "textbox-return", submit_chat, "%p%p%p", chatServer, chatInput, console);

		// add elements to interface manager
		interfaceManager->addWindow(chatWindow);

		popUp = AG_WindowNew(AG_WINDOW_NOBUTTONS);
		AG_WindowSetCaption(popUp, "Exit Game");
		AG_WindowSetGeometry(popUp, screenWidth / 2 - 75, screenHeight / 2 - 40, 150, 80);
		AG_WindowHide(popUp);

		AG_Button *logOut = AG_ButtonNewFn(popUp, 0, "Log Out", handle_logout, "%p%p%d", chatServer, &mLeft, 1);
		AG_ButtonJustify(logOut, AG_TEXT_CENTER);

		AG_Button *exitGame = AG_ButtonNewFn(popUp, 0, "Exit to Desktop", handle_logout, "%p%p%d", chatServer, &mLeft, 0);
		AG_ButtonJustify(exitGame, AG_TEXT_CENTER);

		interfaceManager->addWindow(popUp);

		std::string nick = player->getSelectedCharacter()->getName();
		std::string host = "neo.us.whatnet.org";
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
		mCam = NULL;
		delete chatServer;
		chatServer = NULL;
        interfaceManager->removeAllWindows();
        interfaceManager->removeMouseListeners();
        graphicsEngine->setCamera(NULL);
        mapEngine->removeNode(player->getSelectedCharacter());
        player->removeCharacters();
        mapEngine->unload();
	}

	bool TestState::update()
	{
	    if (mLeft)
            return false;

	    // check if the map has been loaded then tell the game server we're ready
	    if (!mLoaded && mapEngine->mapLoaded())
	    {
            Packet *packet = new Packet(PGMSG_MAP_LOADED);
            networkManager->sendPacket(packet);
            mLoaded = true;
            Texture *texture =
                graphicsEngine->getTexture(resourceManager->getDataPath("glowtile_red.png"));
            interfaceManager->getMouse()->cursor = new Node("Cursor", texture);
            mapEngine->getLayer(mapEngine->getLayers() - 1)->addNode(interfaceManager->getMouse()->cursor);
	    }

		// Check for input, if escape pressed, exit
		if (inputManager->getKey(AG_KEY_ESCAPE))
		{
            // bring up menu to choose whether to log off or quit
            if (AG_WindowIsVisible(popUp))
                AG_WindowHide(popUp);
            else
                AG_WindowShow(popUp);
		}

        // number of milliseconds since last frame
        ms = SDL_GetTicks() - lastframe;
        lastframe = SDL_GetTicks();
        mTime += ms;

        if (mTime > 1000)
        {
            graphicsEngine->saveFrames();
            mTime = 0;
        }

        if (networkManager->isConnected())
        {
            // pass the number of milliseconds to logic
            beingManager->logic(ms);
            player->getSelectedCharacter()->logic(ms);
            player->logic(ms);
            graphicsEngine->getCamera()->logic(ms);

            chatServer->process();
        }


		SDL_Delay(0);

		return true;
	}
}
