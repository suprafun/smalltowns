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
#include "graphics/texture.h"

#include "interface/interfacemanager.h"

#include "net/networkmanager.h"
#include "net/packet.h"
#include "net/protocol.h"

#include "utilities/stringutils.h"
#include "utilities/xml.h"

#include <SDL.h>

namespace ST
{
    void change_hair(AG_Event *event)
    {
        // get chosen body part
        Body *body = static_cast<Body*>(AG_PTR(1));

        // get state
        CharacterState *state = static_cast<CharacterState*>(AG_PTR(2));

        //state->mChosen[PART_HAIR] = body->file;
        state->updateAvatar(body);
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
			int hair = 0;

            Packet *packet = new Packet(PAMSG_CHAR_CREATE);
            packet->setString(name);
            packet->setInteger(hair);
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

    Choices::Choices(int numBodyParts)
    {
        for (int i = 0; i < numBodyParts; ++i)
        {
            PossibleChoices *pc = new PossibleChoices;
            pc->lastchoice = 0;
            pc->bodypart = i;
            mPossible.insert(std::pair<int, PossibleChoices*>(i, pc));
        }
    }

    Choices::~Choices()
    {
        PossibleItr itr = mPossible.begin(), itr_end = mPossible.end();
        while (itr != itr_end)
        {
            for (unsigned int i = 0; i < itr->second->choices.size(); ++i)
            {
                delete itr->second->choices[i];
            }
            delete itr->second;
            ++itr;
        }

        mPossible.clear();
    }

    Body* Choices::next(int bodypart)
    {
        PossibleItr itr = mPossible.find(bodypart);
        if (itr != mPossible.end())
        {
            PossibleChoices *pc = itr->second;
            ++(pc->lastchoice);
            if (pc->lastchoice == pc->choices.size())
            {
                pc->lastchoice = 0;
            }
            return pc->choices[pc->lastchoice];
        }

        return NULL;
    }

    void Choices::addPart(Body *part)
    {
        PossibleItr itr = mPossible.find(part->part);
        if (itr != mPossible.end())
        {
            itr->second->choices.push_back(part);
        }
    }

    Body* Choices::getPart(int partId, int type)
    {
        PossibleItr itr = mPossible.find(type);
        if (itr != mPossible.end())
        {
            PossibleChoices *pc = itr->second;
            for (unsigned int i = 0; i < pc->choices.size(); ++i)
            {
                if (pc->choices[i]->id == partId)
                {
                    return pc->choices[i];
                }
            }
        }

        return NULL;
    }

    int Choices::getCount(int bodypart)
    {
        PossibleItr itr = mPossible.find(bodypart);
        if (itr != mPossible.end())
        {
            return itr->second->choices.size();
        }

        return 0;
    }

    CharacterState::CharacterState()
    {
		mSelected = 0;
		mChoices = 0;
		mAvatar = 0;

		XMLFile file;
		if (file.load("body.cfg"))
		{
		    // read number of body parts available
		    mNumBodyParts = file.readInt("parts", "count");

            // set defaults
		    mDefaults.push_back(file.readString("default", "body"));
		    mDefaults.push_back(file.readString("default", "hair"));

            // add all available choices for body parts
            mChoices = new Choices(mNumBodyParts);

            do
            {
                Body *body = new Body;
                body->id = file.readInt("body", "id");
                body->file = file.readString("body", "file");
                body->icon = file.readString("body", "icon");
                body->part = file.readInt("body", "part");

                mChoices->addPart(body);
            } while (file.next("body"));
		}
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
        delete mChoices;
        mChoices = 0;
        delete mAvatar;
        mAvatar = 0;
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

    void CharacterState::createSelectionWindow()
    {
        AG_WindowSetCaption(mSelectWindow, "Select Character");
		AG_WindowSetSpacing(mSelectWindow, 5);
		AG_WindowSetGeometry(mSelectWindow, mHalfScreenWidth - 100, mHalfScreenHeight - 80, 200, 160);

		AG_Radio *selection = AG_RadioNew(mSelectWindow, 0, NULL);

		AG_HBox *layout = AG_HBoxNew(mSelectWindow, 0);
		AG_Fixed *position = AG_FixedNew(layout, 0);

		// create number of characters based on number of characters a player has
        for (int i = 0; i < player->getNumChars(); ++i)
        {
            Character *c = player->getCharacter(i);
            if (c)
            {
                AG_Pixmap *pixmap;

                // TODO: Add selectable body (for different genders)
				// load the texture
                Texture *tex = graphicsEngine->loadTexture(mDefaults[0]);

                // put the texture into the pixmap
                if (graphicsEngine->isOpenGL())
                {
                    pixmap = AG_PixmapFromTexture(0, 0, tex->getGLTexture(), 0);
                }
                else
                {
                    AG_Surface *s = AG_SurfaceFromSDL(tex->getSDLSurface());
                    pixmap = AG_PixmapFromSurface(0, AG_PIXMAP_RESCALE, s);
                }

                // put the pixmap on the screen
                AG_FixedPut(position, pixmap, 100, 10);

                pixmap = 0;
                tex = 0;

                // now add hair
                Body *hair = mChoices->getPart(c->look.hair, PART_HAIR);
                if (hair)
                {
                    // Load texture
                    tex = graphicsEngine->loadTexture(hair->file);

                    // put the texture into the pixmap
                    if (graphicsEngine->isOpenGL())
                    {
                        pixmap = AG_PixmapFromTexture(0, 0, tex->getGLTexture(), 0);
                    }
                    else
                    {
                        AG_Surface *s = AG_SurfaceFromSDL(tex->getSDLSurface());
                        pixmap = AG_PixmapFromSurface(0, AG_PIXMAP_RESCALE, s);
                    }

                    // put the pixmap on the screen
                    AG_FixedPut(position, pixmap, 100, 10);
                    break;
                }

                // Allow user to select the character
                AG_RadioAddItem(selection, "%s - Level %i", c->getName().c_str(), c->getLevel());
			}
		}

		AG_Expand(layout);
		AG_Expand(position);

		AG_HBox *box = AG_HBoxNew(mSelectWindow, 0);
		AG_Button *button = AG_ButtonNewFn(box, 0, "Choose", select_character, "%p", selection);
		AG_ButtonJustify(button, AG_TEXT_CENTER);
		AG_Button *create_button = AG_ButtonNewFn(box, 0, "Create New",
                                                  switch_char_window, "%p%p",
                                                  mSelectWindow, mCreateWindow);
		AG_ButtonJustify(create_button, AG_TEXT_CENTER);

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
        int hairCount = mChoices->getCount(PART_HAIR);
        for (int i = 0; i < hairCount; ++i)
        {
            // get the body
            Body *body = mChoices->next(PART_HAIR);
            // load the texture
            Texture *tex = graphicsEngine->loadTexture(body->icon);

            AG_Button *hair = AG_ButtonNewFn(charBox, 0, 0, change_hair, "%p%p", body, this);
            AG_ButtonJustify(hair, AG_TEXT_CENTER);
            AG_ButtonValign(hair, AG_TEXT_MIDDLE);
            AG_Surface *s = AG_SurfaceFromSDL(tex->getSDLSurface());
            AG_ButtonSurface(hair, s);
        }


		AG_Textbox *charNick = AG_TextboxNew(mCreateWindow, 0, "Nickname: ");
		AG_TextboxSizeHint(charNick, "XXXXXXXXXXXXXXXX");

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
        for (int i = 0; i < mNumBodyParts; ++i)
        {
            // load the texture
            Texture *tex = graphicsEngine->loadTexture(mDefaults[i]);

            // put the texture into the pixmap
            if (graphicsEngine->isOpenGL())
            {
                mAvatar->bodyparts.push_back(AG_PixmapFromTexture(NULL, 0, tex->getGLTexture(), 0));
            }
            else
            {
                AG_Surface *s = AG_SurfaceFromSDL(tex->getSDLSurface());
                mAvatar->bodyparts.push_back(AG_PixmapFromSurface(0, AG_PIXMAP_RESCALE, s));
            }
        }
    }

    void CharacterState::updateAvatar(Body *body)
    {
        // load the new graphic
        Texture *tex = graphicsEngine->loadTexture(body->file);

        AG_Pixmap *pixmap = mAvatar->bodyparts.at(body->part);

        AG_Surface *surface = 0;

        if (graphicsEngine->isOpenGL())
        {
            glBindTexture(GL_TEXTURE_2D, tex->getGLTexture());
            surface = AG_SurfaceRGBA(tex->getWidth(), tex->getHeight(), 32, 0,
#if AG_BYTEORDER == AG_BIG_ENDIAN
                                0xff000000,
                                0x00ff0000,
                                0x0000ff00,
                                0x000000ff
#else
                                0x000000ff,
                                0x0000ff00,
                                0x00ff0000,
                                0xff000000
#endif
                                );
            glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
            glBindTexture(GL_TEXTURE_2D, 0);

        }
        else
        {
            surface = AG_SurfaceFromSDL(tex->getSDLSurface());
        }

        AG_PixmapReplaceSurface(pixmap, 0, surface);
        AG_WindowUpdate(mCreateWindow);
    }
}
