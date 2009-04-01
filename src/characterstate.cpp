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
        return 1;
    }

    void select_character(AG_Event *event)
    {
        AG_Socket *sock = static_cast<AG_Socket*>(AG_PTR(1));
        if (sock)
        {
            Packet *packet = new Packet(PAMSG_CHAR_CHOOSE);

            int slot = utils::toInt(AG_GetString(sock->icon, "character"));
            packet->setInteger(slot);

            networkManager->sendPacket(packet);
        }
    }

    void create_character(AG_Event *event)
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
        std::string tmpAvatar = AG_GetString(icon, "avatar");
        int avatar = utils::toInt(tmpAvatar);
        std::string name = AG_TextboxDupString(static_cast<AG_Textbox*>(AG_PTR(1)));
        if (!name.empty())
        {
            Packet *packet = new Packet(PAMSG_CHAR_CREATE);
            packet->setString(name);
            packet->setInteger(avatar);
            networkManager->sendPacket(packet);
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

		AG_Window *win = AG_WindowNew(AG_WINDOW_PLAIN);
		AG_WindowShow(win);
		AG_WindowMaximize(win);

		AG_Window *charSelect = AG_WindowNewNamed(AG_WINDOW_NOBUTTONS, "CharSelect");
		AG_WindowSetCaption(charSelect, "Select Character");
		AG_WindowSetSpacing(charSelect, 12);
		AG_WindowSetGeometry(charSelect, halfScreenWidth - 125, halfScreenHeight - 90, 225, 180);

		// load in the avatars to choose from
		const int avatarCount = 6;
		SDL_Surface *surface[avatarCount];

		surface[0] = graphicsEngine->loadSDLTexture("head1.png");
		surface[1] = graphicsEngine->loadSDLTexture("head2.png");
		surface[2] = graphicsEngine->loadSDLTexture("head3.png");
        surface[3] = graphicsEngine->loadSDLTexture("head4.png");
        surface[4] = graphicsEngine->loadSDLTexture("head5.png");
        surface[5] = graphicsEngine->loadSDLTexture("head6.png");

        AG_Surface *head[avatarCount];
        for (int i = 0; i < avatarCount; ++i)
        {
            head[i] = AG_SurfaceFromSDL(surface[i]);
            SDL_FreeSurface(surface[i]);
        }

        AG_HBox *hbox = AG_HBoxNew(charSelect, 0);

        // create number of characters based on number of characters a player has
        for (int i = 0; i < player->getNumChars(); ++i)
        {
            Character *c = player->getCharacter(i);
            if (c)
            {
                AG_Icon *icon = NULL;
                AG_Socket *avatars = AG_SocketNew(hbox, 0);
                for (int j = 0; j < avatarCount; ++j)
                {
                    if (head[j] && c->getHead() == j+1)
                    {
                        icon = AG_IconNew(avatars, 0);
                        AG_IconSetSurface(icon, head[j]);
                    }
                }
                if (icon)
                {
                    AG_SetString(icon, "character", utils::toString(i).c_str());
                    AG_SocketInsertIcon(avatars, icon);
                    AG_Label *level = AG_LabelNew(avatars, 0, "%s\n%d", c->getName().c_str(), c->getLevel());
                }

            }
        }

        AG_Socket *selected = AG_SocketNew(charSelect, 0);

		AG_Window *charNew = AG_WindowNewNamed(AG_WINDOW_NOBUTTONS, "CharNew");
		AG_WindowSetCaption(charNew, "Create new Character");
		AG_WindowSetSpacing(charNew, 12);
		AG_WindowSetGeometry(charNew, halfScreenWidth - 140, halfScreenHeight - 90, 280, 175);

        AG_VBox *new_box = AG_VBoxNew(charNew, 0);

        // create avatars to choose from
        AG_HBox *horizBox = AG_HBoxNew(new_box, 0);
        AG_Icon *avatarIcon[avatarCount];
        AG_Socket *avatarSocket[avatarCount];
        for (int i = 0; i < avatarCount; ++i)
        {
            avatarSocket[i] = AG_SocketNew(horizBox, 0);
            avatarIcon[i] = AG_IconNew(avatarSocket[i], 0);
            AG_IconSetSurface(avatarIcon[i], head[i]);
            AG_SetString(avatarIcon[i], "avatar", utils::toString(i+1).c_str());
            AG_SocketInsertIcon(avatarSocket[i], avatarIcon[i]);
        }

        AG_Socket *avatar = AG_SocketNew(new_box, 0);

		AG_Textbox *charNick = AG_TextboxNew(new_box, 0, "Nickname: ");

		AG_Button *new_button = AG_ButtonNewFn(new_box, 0, "Submit", submit_new, "%p%p", charNick, avatar);
		AG_ButtonJustify(new_button, AG_TEXT_CENTER);

		AG_HBox *box = AG_HBoxNew(charSelect, 0);
		AG_Button *button = AG_ButtonNewFn(box, 0, "Choose", select_character, "%p", selected);
		AG_ButtonJustify(button, AG_TEXT_CENTER);
		AG_Button *create_button = AG_ButtonNewFn(box, 0, "Create New",
                                                    create_character, "%p%p", charSelect, charNew);
		AG_ButtonJustify(create_button, AG_TEXT_CENTER);

		AG_WindowShow(charSelect);

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
		    networkManager->disconnect();
		    GameState *state = new ConnectState();
			game->changeState(state);
			return true;
		}

        SDL_Delay(0);

        return true;
    }
}
