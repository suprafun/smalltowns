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
	void checked(AG_Event *event)
	{
		CharacterState *state = static_cast<CharacterState*>(AG_PTR(1));

		AG_Checkbox *box = static_cast<AG_Checkbox*>(AG_SELF());
		if (box->state == 1)
		{
			state->mSelected = box;
			std::map<AG_Checkbox*, int>::iterator itr = state->mItems.begin(),
				itr_end = state->mItems.end();
			while(itr != itr_end)
			{
				if (itr->first != box && itr->first->state == 1)
				{
					itr->first->state = 0;
					break;
				}
				++itr;
			}
		}
		else
		{
			state->mSelected = 0;
		}
	}

    void select_character(AG_Event *event)
    {
        AG_Radio *selected = static_cast<AG_Radio*>(AG_PTR(1));
        if (selected)
        {
            Packet *packet = new Packet(PAMSG_CHAR_CHOOSE);
            int slot = -1;

			slot = selected->value;
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
        CharacterState *state = static_cast<CharacterState*>(AG_PTR(2));

        std::string name = AG_TextboxDupString(static_cast<AG_Textbox*>(AG_PTR(1)));
        if (!name.empty() && state)
        {
			int avatar = 0;
			std::map<AG_Checkbox*, int>::iterator itr = state->mItems.find(state->mSelected);

			if (itr != state->mItems.end())
			{
				avatar = itr->second;
			}

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
		mSelected = 0;
    }

    void CharacterState::enter()
    {
        // When we enter character state
        // player should already contain a list
        // of existing characters from the game server
        int screenWidth = graphicsEngine->getScreenWidth();
		int screenHeight = graphicsEngine->getScreenHeight();
		int halfScreenWidth = screenWidth / 2;
		int halfScreenHeight = screenHeight / 2;

		AG_Window *win = AG_WindowNew(AG_WINDOW_PLAIN|AG_WINDOW_KEEPBELOW);
		AG_WindowShow(win);
		AG_WindowMaximize(win);

		AG_Window *charSelect = AG_WindowNewNamed(AG_WINDOW_NOBUTTONS|AG_WINDOW_KEEPABOVE, "CharSelect");
		AG_WindowSetCaption(charSelect, "Select Character");
		AG_WindowSetSpacing(charSelect, 5);
		AG_WindowSetGeometry(charSelect, halfScreenWidth - 100, halfScreenHeight - 80, 200, 160);

		AG_Radio *selection = AG_RadioNew(charSelect, 0, NULL);

		// create number of characters based on number of characters a player has
        for (int i = 0; i < player->getNumChars(); ++i)
        {
            Character *c = player->getCharacter(i);
            if (c)
            {
				// Load in each sprite layer

                // Put them together to make up the character

                // Allow user to select the character
                AG_RadioAddItem(selection, "%s - Level %i", c->getName().c_str(), c->getLevel());
			}
		}

		AG_Window *charNew = AG_WindowNewNamed(AG_WINDOW_NOBUTTONS|AG_WINDOW_KEEPABOVE, "CharNew");
		AG_WindowSetCaption(charNew, "Create new Character");
		AG_WindowSetSpacing(charNew, 12);
		AG_WindowSetGeometry(charNew, halfScreenWidth - 200, halfScreenHeight - 95, 400, 190);

		AG_HBox *createBox = AG_HBoxNew(charNew, 0);

        //TODO: display different sprite layer choices

		AG_Textbox *charNick = AG_TextboxNew(charNew, 0, "Nickname: ");
		AG_TextboxSizeHint(charNick, "XXXXXXXXXXXXXXXX");

		AG_HBox *hbox = AG_HBoxNew(charNew, 0);
		AG_Button *new_button = AG_ButtonNewFn(hbox, 0, "Submit", submit_new, "%p%p", charNick, this);
		AG_ButtonJustify(new_button, AG_TEXT_CENTER);
		AG_Button *back_button = AG_ButtonNewFn(hbox, 0, "Back", switch_char_window, "%p%p", charNew, charSelect);
		AG_ButtonJustify(back_button, AG_TEXT_CENTER);

		AG_HBox *box = AG_HBoxNew(charSelect, 0);
		AG_Button *button = AG_ButtonNewFn(box, 0, "Choose", select_character, "%p", selection);
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
