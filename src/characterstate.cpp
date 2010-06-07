/*********************************************
 *
 *	Author: David Athay
 *
 *	License: New BSD License
 *
 *	Copyright (c) 2009, CT Games
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
#include "map.h"
#include "player.h"
#include "resourcemanager.h"

#include "graphics/graphics.h"
#include "graphics/texture.h"

#include "interface/interfacemanager.h"

#include "net/networkmanager.h"
#include "net/packet.h"
#include "net/protocol.h"

#include "resources/bodypart.h"

#include "utilities/log.h"
#include "utilities/stringutils.h"
#include "utilities/xml.h"

#include <SDL.h>

namespace ST
{
    void change_part(AG_Event *event)
    {
        // get chosen body part
        BodyPart *body = static_cast<BodyPart*>(AG_PTR(1));

        // get state
        CharacterState *state = static_cast<CharacterState*>(AG_PTR(2));

        state->mChosen[body->getType()] = body->getId();
        state->updateAvatar(body);
    }

    void change_sex(AG_Event *event)
    {
        // get selected sex
        AG_Radio *sex = static_cast<AG_Radio*>(AG_SELF());

        // get state
        CharacterState *state = static_cast<CharacterState*>(AG_PTR(1));

        // get the body
        BodyPart *body;
        if (sex->value == 0)
        {
            body = resourceManager->getDefaultBody(PART_BODY);
        }
        else
        {
            body = resourceManager->getFemaleBody();
        }

        state->mChosen[body->getType()] = body->getId();
        state->updateAvatar(body);
    }

    void radio_selected(AG_Event *event)
    {
        AG_Button *button = static_cast<AG_Button*>(AG_PTR(1));
        if (button)
            AG_WidgetEnable(button);
        AG_Button *del_button = static_cast<AG_Button*>(AG_PTR(2));
        if (del_button)
            AG_WidgetEnable(del_button);
    }

    void select_character(AG_Event *event)
    {
        AG_Radio *selected = static_cast<AG_Radio*>(AG_PTR(1));
        if (selected)
        {
            Packet *packet = new Packet(PAMSG_CHAR_CHOOSE);
            // send the slot of character selected
            int slot = selected->value;
            packet->setInteger(slot);
            player->setCharacter(slot);

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

    void del_character(AG_Event *event)
    {
        AG_Radio *selected = static_cast<AG_Radio*>(AG_PTR(1));
        if (selected)
        {
            // send the slot of character selected
            int slot = selected->value;

            Packet *packet = new Packet(PAMSG_CHAR_DELETE);
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
            int body = state->mChosen[PART_BODY];
			int hair = state->mChosen[PART_HAIR];
			int chest = 0;
			int legs = 0;
			int feet = 0;

            Packet *packet = new Packet(PAMSG_CHAR_CREATE);
            packet->setString(name);
            packet->setInteger(body);
            packet->setInteger(hair);
            packet->setInteger(chest);
            packet->setInteger(legs);
            packet->setInteger(feet);
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
		mAvatar = 0;
		#if SDL_BYTEORDER == SDL_BIG_ENDIAN
        rmask = 0xff000000;
        gmask = 0x00ff0000;
        bmask = 0x0000ff00;
        amask = 0x000000ff;
        #else
        rmask = 0x000000ff;
        gmask = 0x0000ff00;
        bmask = 0x00ff0000;
        amask = 0xff000000;
        #endif

#ifndef __APPLE__
		resourceManager->loadBodyParts("body.cfg");
        resourceManager->loadAnimations("animation.cfg");
#else
		resourceManager->loadBodyParts(resourceManager->getDataPath("body.cfg"));
        resourceManager->loadAnimations(resourceManager->getDataPath("animation.cfg"));
#endif

		BodyPart *body = resourceManager->getDefaultBody(PART_BODY);
		BodyPart *hair = resourceManager->getDefaultBody(PART_HAIR);
		if (body)
            mChosen.push_back(body->getId());
        if (hair)
           	mChosen.push_back(hair->getId());
    }

    void CharacterState::enter()
    {
        // When we enter character state
        // player should already contain a list
        // of existing characters from the game server
        int screenWidth = graphicsEngine->getScreenWidth();
		int screenHeight = graphicsEngine->getScreenHeight();
		mHalfScreenWidth = screenWidth / 2;
		mHalfScreenHeight = screenHeight / 2;

		AG_Window *win = AG_WindowNew(AG_WINDOW_PLAIN|AG_WINDOW_KEEPBELOW);
		AG_WindowShow(win);
		AG_WindowMaximize(win);
		interfaceManager->addWindow(win);

		mSelectWindow = AG_WindowNewNamed(AG_WINDOW_NOBUTTONS|AG_WINDOW_KEEPABOVE, "CharSelect");
		mCreateWindow = AG_WindowNewNamed(AG_WINDOW_NOBUTTONS|AG_WINDOW_KEEPABOVE, "CharNew");

		createSelectionWindow();
		createCreationWindow();
    }

    void CharacterState::exit()
    {
        interfaceManager->removeAllWindows();
        delete mAvatar;
        mAvatar = 0;
    }

    bool CharacterState::update()
    {
        // Check for input, if escape pressed, exit
		if (inputManager->getKey(AG_KEY_ESCAPE))
		{
			return false;
		}

        SDL_Delay(0);

        return true;
    }

    void CharacterState::createSelectionWindow()
    {
        AG_WindowSetCaption(mSelectWindow, "Select Character");
		AG_WindowSetSpacing(mSelectWindow, 5);
		AG_WindowSetGeometry(mSelectWindow, mHalfScreenWidth - 100, mHalfScreenHeight - 115, 200, 230);

        AG_VBox *selectBox = AG_VBoxNew(mSelectWindow, 0);
        AG_Radio *selection = AG_RadioNew(selectBox, 0, NULL);
		AG_Fixed *position = AG_FixedNew(selectBox, 0);

		// create number of characters based on number of characters a player has
        for (int i = 0; i < player->getNumChars(); ++i)
        {
            Character *c = player->getCharacter(i);
            if (c)
            {
                AG_Pixmap *pixmap = NULL;

				// load the texture
                Texture *tex = c->getTexture();

                if (tex)
                {
                    // put the texture into the pixmap
                    AG_Surface *surface = NULL;
                    if (graphicsEngine->isOpenGL())
                    {
						pixmap = AG_PixmapFromTexture(0, 0, tex->getGLTexture(), 0);
                    }
                    else
                    {
                        SDL_Surface *s = tex->getSDLSurface();
                        SDL_LockSurface(s);
                        surface = AG_SurfaceFromPixelsRGBA(s->pixels, s->w, s->h, s->format->BitsPerPixel, rmask, gmask, bmask, amask);
                        SDL_UnlockSurface(s);
                        pixmap = AG_PixmapFromSurface(0, 0, surface);
                    }

                    // put the pixmap on the screen
                    AG_FixedPut(position, pixmap, 2 + (64 * i), 0);
                }

                // Allow user to select the character
                AG_RadioAddItem(selection, "%s - Level %i", c->getName().c_str(), c->getLevel());
			}
		}
		AG_Expand(position);
		AG_Expand(selectBox);

		AG_HBox *box = AG_HBoxNew(mSelectWindow, 0);
		AG_ObjectSetName(box, "Container");
		AG_Button *button = AG_ButtonNewFn(box, 0, "Choose", select_character, "%p", selection);
		AG_ButtonJustify(button, AG_TEXT_CENTER);
		AG_Button *create_button = AG_ButtonNewFn(box, 0, "Create New",
                                                  switch_char_window, "%p%p",
                                                  mSelectWindow, mCreateWindow);
		AG_ButtonJustify(create_button, AG_TEXT_CENTER);
        AG_Button *delete_button = AG_ButtonNewFn(box, 0, "Delete", del_character, "%p", selection);
        AG_ButtonJustify(delete_button, AG_TEXT_CENTER);
		AG_WidgetDisable(button);
        AG_WidgetDisable(delete_button);

		AG_SetEvent(selection, "radio-changed", radio_selected, "%p%p", button, delete_button);

		AG_WindowShow(mSelectWindow);
		interfaceManager->addWindow(mSelectWindow);
    }

    void CharacterState::createCreationWindow()
    {
        // set window paramaters
        AG_WindowSetCaption(mCreateWindow, "Create new Character");
		AG_WindowSetSpacing(mCreateWindow, 12);
		AG_WindowSetGeometry(mCreateWindow, mHalfScreenWidth - 140, mHalfScreenHeight - 150, 280, 300);

        // create layout
		AG_HBox *charBox = AG_HBoxNew(mCreateWindow, 0);
		AG_Fixed *charPos = AG_FixedNew(charBox, 0);

        // build default avatar
        mAvatar = new Avatar;
        createAvatar();
        for (unsigned int i = 0; i < mAvatar->bodyparts.size(); ++i)
        {
            AG_FixedPut(charPos, mAvatar->bodyparts[i], 10, 10);
        }

        AG_Expand(charBox);
        AG_Expand(charPos);

        // list all the hair styles to choose from
        std::vector<BodyPart*> hairList = resourceManager->getBodyList(PART_HAIR);
        int hairCount = hairList.size();
        for (int i = 0; i < hairCount; ++i)
        {
            // get the body
            BodyPart *body = hairList[i];

            if (!body)
            {
                // error
                break;
            }
            // load the texture
            Texture *tex = body->getIcon();

            AG_Button *hair = AG_ButtonNewFn(charBox, 0, 0, change_part, "%p%p", body, this);
            AG_ButtonJustify(hair, AG_TEXT_CENTER);
            AG_ButtonValign(hair, AG_TEXT_MIDDLE);

            // make each haair style into a button with hair style icon
            SDL_Surface *s;
            AG_Surface *surface = NULL;
            if (graphicsEngine->isOpenGL())
            {
                s = graphicsEngine->createSurface(tex->getGLTexture(), tex->getWidth(), tex->getHeight());
                SDL_LockSurface(s);
                surface = AG_SurfaceFromPixelsRGBA(s->pixels, s->w, s->h, s->format->BitsPerPixel, rmask, gmask, bmask, amask);
                SDL_UnlockSurface(s);
            }
            else
            {
                s = tex->getSDLSurface();
                SDL_LockSurface(s);
                surface = AG_SurfaceFromPixelsRGBA(s->pixels, s->w, s->h, s->format->BitsPerPixel, rmask, gmask, bmask, amask);
                SDL_UnlockSurface(s);
            }
            AG_ButtonSurface(hair, surface);
        }

        AG_Radio *sex = AG_RadioNewFn(mCreateWindow, 0, NULL, change_sex, "%p", this);
        AG_RadioAddItem(sex, "male");
        AG_RadioAddItem(sex, "female");

		AG_Textbox *charNick = AG_TextboxNew(mCreateWindow, 0, "Nickname: ");
		AG_TextboxSizeHint(charNick, "XXXXXXXXXXXXXXXX");
		AG_ExpandHoriz(charNick);

		AG_HBox *hbox = AG_HBoxNew(mCreateWindow, 0);
		AG_Button *backButton = AG_ButtonNewFn(hbox, 0, "Cancel", switch_char_window, "%p%p",
                                                mCreateWindow, mSelectWindow);
		AG_ButtonJustify(backButton, AG_TEXT_CENTER);
		AG_Button *button = AG_ButtonNewFn(hbox, 0, "Create", submit_new, "%p%p", charNick, this);
		AG_ButtonJustify(button, AG_TEXT_CENTER);

		interfaceManager->addWindow(mCreateWindow);
    }

    void CharacterState::createAvatar()
    {
        for (unsigned int i = 0; i < resourceManager->numBodyParts(); ++i)
        {
            // load the texture
            BodyPart *body = resourceManager->getDefaultBody(i);
            if (!body)
                break;
            Texture *tex = body->getTexture(DIRECTION_SOUTHEAST);

            // put the texture into the pixmap
            AG_Surface *surface = NULL;
            SDL_Surface *s = NULL;
            if (graphicsEngine->isOpenGL())
            {
                s = graphicsEngine->createSurface(tex->getGLTexture(), resourceManager->getBodyWidth(), resourceManager->getBodyHeight());
                if (s)
                {
                    SDL_LockSurface(s);
                    surface = AG_SurfaceFromPixelsRGBA(s->pixels, s->w, s->h, s->format->BitsPerPixel, rmask, gmask, bmask, amask);
                    SDL_UnlockSurface(s);
                }
				if (surface)
				{
				    mAvatar->bodyparts.push_back(AG_PixmapFromSurface(0, 0, surface));
				}

            }
            else
            {
                s = tex->getSDLSurface();
                if (s)
                {
                    SDL_LockSurface(s);
                    surface = AG_SurfaceFromPixelsRGBA(s->pixels, s->w, s->h, s->format->BitsPerPixel, rmask, gmask, bmask, amask);
                    SDL_UnlockSurface(s);
                }
                if (surface)
                {
                    mAvatar->bodyparts.push_back(AG_PixmapFromSurface(0, 0, surface));
                }
            }
        }
    }

    void CharacterState::updateAvatar(BodyPart *body)
    {
        // load the new graphic
        Texture *tex = body->getTexture(DIRECTION_SOUTHEAST);

        AG_Pixmap *pixmap = mAvatar->bodyparts.at(body->getType());

        AG_Surface *surface = NULL;
        SDL_Surface *s = NULL;
        if (graphicsEngine->isOpenGL())
        {
            s = graphicsEngine->createSurface(tex->getGLTexture(), resourceManager->getBodyWidth(), resourceManager->getBodyHeight());
            SDL_LockSurface(s);
            surface = AG_SurfaceFromPixelsRGBA(s->pixels, s->w, s->h, s->format->BitsPerPixel, rmask, gmask, bmask, amask);
            SDL_UnlockSurface(s);
        }
        else
        {
            s = tex->getSDLSurface();
            SDL_LockSurface(s);
            surface = AG_SurfaceFromPixelsRGBA(s->pixels, s->w, s->h, s->format->BitsPerPixel, rmask, gmask, bmask, amask);
            SDL_UnlockSurface(s);
        }

        AG_PixmapReplaceCurrentSurface(pixmap, surface);
        AG_WindowUpdate(mCreateWindow);
    }
}
