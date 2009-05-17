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
 *	Date of file creation: 09-02-25
 *
 *	$Id$
 *
 ********************************************/

#include "characterstate.h"
#include "character.h"
#include "connectstate.h"
#include "input.h"
#include "game.h"
#include "player.h"

#include "graphics/graphics.h"

#include "interface/interfacemanager.h"

#include "net/networkmanager.h"
#include "net/packet.h"
#include "net/protocol.h"

#include "utilities/stringutils.h"

#include <SDL.h>

namespace ST
{
    int insert_avatar(AG_Socket *sock, AG_Icon *icon)
    {
        if (sock->count < 1)
            AG_SocketInsertIcon(sock, icon);
        return 1;
    }

    void select_character(AG_Event *event)
    {
        AG_Socket *sock = static_cast<AG_Socket*>(AG_PTR(1));
        if (sock)
        {
            Packet *packet = new Packet(PAMSG_CHAR_CHOOSE);
            int slot = -1;

#if AGAR_PATCHLEVEL > 3
            char name[255];
            AG_GetString(sock->icon, "character", name, 255);
            slot = utils::toInt(name);
#else
			slot = utils::toInt(AG_GetString(sock->icon, "character"));
#endif
            packet->setInteger(slot);

            networkManager->sendPacket(packet);

			interfaceManager->setErrorMessage("");
			interfaceManager->showErrorWindow(false);
        }
        else
        {
			interfaceManager->setErrorMessage("Invalid character chosen");
            interfaceManager->showErrorWindow(true);
        }
    }

    void switch_char_window(AG_Event *event)
    {
        AG_Window *one = static_cast<AG_Window*>(AG_PTR(1));
        if (one)
            AG_WindowHide(one);

        AG_Window *two = static_cast<AG_Window*>(AG_PTR(2));
        if (two)
            AG_WindowShow(two);
    }

    void submit_new(AG_Event *event)
    {
        AG_Socket *sock = static_cast<AG_Socket*>(AG_PTR(2));
        AG_Icon *icon = sock->icon;
#if AGAR_PATCHLEVEL > 3
        char tmpAvatar[255];
        AG_GetString(icon, "avatar", tmpAvatar, 255);
#else
		std::string tmpAvatar = AG_GetString(icon, "avatar");
#endif
        int avatar = utils::toInt(tmpAvatar);
        std::string name = AG_TextboxDupString(static_cast<AG_Textbox*>(AG_PTR(1)));
        if (!name.empty())
        {
            Packet *packet = new Packet(PAMSG_CHAR_CREATE);
            packet->setString(name);
            packet->setInteger(avatar);
            networkManager->sendPacket(packet);

            interfaceManager->setErrorMessage("");
			interfaceManager->showErrorWindow(false);
        }
        else
        {
            interfaceManager->setErrorMessage("Invalid character name");
            interfaceManager->showErrorWindow(true);
        }
    }

    CharacterState::CharacterState()
    {

    }

    void CharacterState::enter()
    {
        // When we enter character state
        // player should already contain a list
        // of existing characters from the game server
        int screenWidth = graphicsEngine->getScreenWidth();
		int screenHeight = graphicsEngine->getScreenHeight();
		float halfScreenWidth = screenWidth / 2.0f;
		float halfScreenHeight = screenHeight / 2.0f;

		AG_Window *win = AG_WindowNew(AG_WINDOW_PLAIN|AG_WINDOW_KEEPBELOW);
		AG_WindowShow(win);
		AG_WindowMaximize(win);

		AG_Window *charSelect = AG_WindowNewNamed(AG_WINDOW_NOBUTTONS|AG_WINDOW_KEEPABOVE, "CharSelect");
		AG_WindowSetCaption(charSelect, "Select Character");
		AG_WindowSetSpacing(charSelect, 5);
		AG_WindowSetGeometry(charSelect, halfScreenWidth - 125, halfScreenHeight - 125, 225, 225);

		// load in the avatars to choose from
		std::vector<SDL_Surface*> surfaces;

        surfaces.push_back(graphicsEngine->loadSDLTexture("head0.png"));
		surfaces.push_back(graphicsEngine->loadSDLTexture("head1.png"));
		surfaces.push_back(graphicsEngine->loadSDLTexture("head2.png"));
		surfaces.push_back(graphicsEngine->loadSDLTexture("head3.png"));
		surfaces.push_back(graphicsEngine->loadSDLTexture("head4.png"));
		surfaces.push_back(graphicsEngine->loadSDLTexture("head5.png"));
		surfaces.push_back(graphicsEngine->loadSDLTexture("head6.png"));

		int avatarCount = surfaces.size();

        std::vector<AG_Surface*> heads;
        for (int i = 0; i < avatarCount; ++i)
        {
            heads.push_back(AG_SurfaceFromSDL(surfaces[i]));
            SDL_FreeSurface(surfaces[i]);
        }

        AG_Fixed *fx = AG_FixedNew(charSelect, AG_FIXED_EXPAND);

        // create number of characters based on number of characters a player has
        for (int i = 0; i < player->getNumChars(); ++i)
        {
            Character *c = player->getCharacter(i);
            if (c)
            {
                AG_Icon *icon = NULL;
                AG_Socket *avatars = AG_SocketNew(fx, 0);
                AG_FixedMove(fx, avatars, 6 + i * 32, 6);
                AG_SocketInsertFn(avatars, insert_avatar);

                for (int j = 0; j < avatarCount; ++j)
                {
                    // find the correct avatar for this character
                    if (heads.size() > j && c->getHead() == j)
                    {
                        icon = AG_IconFromSurface(heads[j]);
                        break;
                    }
                }
                if (icon)
                {
                    AG_SetString(icon, "character", utils::toString(i).c_str());
                    AG_SocketInsertIcon(avatars, icon);
                    AG_Label *charInfo = AG_LabelNew(fx, 0, "%s\n%d", c->getName().c_str(), c->getLevel());
                    AG_FixedMove(fx, charInfo, 12 + i * 32, 45);
                }

            }
        }

        AG_Socket *selected = AG_SocketNew(fx, 0);
        AG_SocketInsertFn(selected, insert_avatar);
        AG_FixedMove(fx, selected, 6, 75);

		AG_Window *charNew = AG_WindowNewNamed(AG_WINDOW_NOBUTTONS|AG_WINDOW_KEEPABOVE, "CharNew");
		AG_WindowSetCaption(charNew, "Create new Character");
		AG_WindowSetSpacing(charNew, 12);
		AG_WindowSetGeometry(charNew, halfScreenWidth - 140, halfScreenHeight + 10, 280, 175);

        // create avatars to choose from
        AG_Fixed *fxNew = AG_FixedNew(charNew, AG_FIXED_EXPAND);
        std::vector<AG_Icon*> avatarIcons;
        std::vector<AG_Socket*> avatarSockets;

        // skip head0, its just for characters without avatars
        for (int i = 0; i < avatarCount - 1; ++i)
        {
            AG_Socket *avSock = AG_SocketNew(fxNew, 0);
            AG_FixedMove(fxNew, avSock, 6 + i * 32, 6);
            AG_SocketInsertFn(avSock, insert_avatar);
            avatarSockets.push_back(avSock);
            avatarIcons.push_back(AG_IconFromSurface(heads[i+1]));
            AG_SetString(avSock, "avatar", utils::toString(i).c_str());
            AG_SocketInsertIcon(avSock, avatarIcons[i]);
        }

        AG_Socket *avatar = AG_SocketNew(fxNew, 0);
        AG_FixedMove(fxNew, avatar, 6, 45);
        AG_SocketInsertFn(avatar, insert_avatar);

		AG_Textbox *charNick = AG_TextboxNew(fxNew, 0, "Nickname: ");
		AG_TextboxSizeHint(charNick, "XXXXXXXXXXXXXXXX");
		AG_FixedMove(fxNew, charNick, 6, 85);

		AG_HBox *hbox = AG_HBoxNew(charNew, 0);
		AG_Button *new_button = AG_ButtonNewFn(hbox, 0, "Submit", submit_new, "%p%p", charNick, avatar);
		AG_ButtonJustify(new_button, AG_TEXT_CENTER);
		AG_Button *back_button = AG_ButtonNewFn(hbox, 0, "Back", switch_char_window, "%p%p", charNew, charSelect);
		AG_ButtonJustify(back_button, AG_TEXT_CENTER);

		AG_HBox *box = AG_HBoxNew(charSelect, 0);
		AG_Button *button = AG_ButtonNewFn(box, 0, "Choose", select_character, "%p", selected);
		AG_ButtonJustify(button, AG_TEXT_CENTER);
		AG_Button *create_button = AG_ButtonNewFn(box, 0, "Create New",
                                                    switch_char_window, "%p%p", charSelect, charNew);
		AG_ButtonJustify(create_button, AG_TEXT_CENTER);

		AG_WindowShow(charSelect);
		AG_WindowHide(charNew);

		interfaceManager->addWindow(win);
		interfaceManager->addWindow(charSelect);
		interfaceManager->addWindow(charNew);
    }

    void CharacterState::exit()
    {
        interfaceManager->removeAllWindows();
    }

    bool CharacterState::update()
    {
        // Check for input, if escape pressed, exit
		if (inputManager->getKey(SDLK_ESCAPE))
		{
			return false;
		}

        SDL_Delay(0);

        return true;
    }
}
