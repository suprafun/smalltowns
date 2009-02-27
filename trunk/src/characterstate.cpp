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

#include "interface/button.h"
#include "interface/icon.h"
#include "interface/interfacemanager.h"
#include "interface/label.h"
#include "interface/textfield.h"
#include "interface/window.h"

#include "net/networkmanager.h"
#include "net/packet.h"
#include "net/protocol.h"

#include "utilities/stringutils.h"

#include <SDL.h>

namespace ST
{
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

        // get number of characters already created
        int numChars = player->getNumChars();

        Window *win = new Window("Character Selection");
        win->setPosition(0, screenHeight);
        win->setSize(screenWidth, screenHeight);
        interfaceManager->addWindow(win);

        Label *countLabel = new Label("Count Label");
        countLabel->setPosition(10, screenHeight - 50);
        countLabel->setText("Number of Characters: " + utils::toString(numChars));
        countLabel->setFontSize(20);
        interfaceManager->addSubWindow(win, countLabel);

        for (int i = 0; i < numChars; ++i)
        {
            // get the character for the slot
            Character *c = player->getCharacter(i);

            // create label for character name
            Label *label = new Label(utils::toString(i));
            label->setPosition(50 + 10 * i, 150);
            label->setText(c->getName());
            label->setFontSize(16);
            // create icon for character avatar
            Icon *icon = new Icon("Character" + utils::toString(i));
            icon->setPosition(50 + 10 * i, 200);
            icon->setSize(64, 64);
            // create button for selecting the character
            Button *button = new Button("Button" + utils::toString(i));
            button->setPosition(50 + 10 * i, 130);
            button->setSize(80, 24);
            button->setText("Select");
            button->setFontSize(18);

            interfaceManager->addSubWindow(win, label);
            interfaceManager->addSubWindow(win, icon);
            interfaceManager->addSubWindow(win, button);
        }

        Window *nameWindow = new Window("Create Character");
        nameWindow->setPosition(395, 600);
        nameWindow->setSize(260, 90);
        nameWindow->setTitleHeight(10);
        nameWindow->setTitleText("Create Character");
        interfaceManager->addWindow(nameWindow);

        Label *nameLabel = new Label("Name Label");
        nameLabel->setPosition(5, 50);
        nameLabel->setText("Character Name:");
        nameLabel->setFontSize(18);
        nameLabel->setVisible(false);
        interfaceManager->addSubWindow(nameWindow, nameLabel);

        TextField *nameField = new TextField("Character Name");
        nameField->setPosition(130, 70);
        nameField->setSize(120, 25);
        nameField->setFontSize(18);
        nameField->setVisible(false);
        interfaceManager->addSubWindow(nameWindow, nameField);

        Button *nameButton = new Button("Submit Character");
        nameButton->setPosition(5, 25);
        nameButton->setSize(80, 24);
        nameButton->setText("Submit");
        nameButton->setFontSize(18);
        nameButton->setVisible(false);
        interfaceManager->addSubWindow(nameWindow, nameButton);

        Button *newButton = new Button("New Character");
        newButton->setPosition(400, 490);
        newButton->setSize(180, 24);
        newButton->setText("Create New Character");
        newButton->setFontSize(18);
        interfaceManager->addSubWindow(win, newButton);

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
		else if (static_cast<Button*>(interfaceManager->getWindow("New Character"))->clicked())
		{
		    interfaceManager->getWindow("Name Label")->setVisible(true);
		    interfaceManager->getWindow("Character Name")->setVisible(true);
		    interfaceManager->getWindow("Submit Character")->setVisible(true);
		    interfaceManager->changeFocus(interfaceManager->getWindow("Character Name"));
		}
		else if (static_cast<Button*>(interfaceManager->getWindow("Submit Character"))->clicked())
		{
		    std::string username = static_cast<TextField*>(interfaceManager->getWindow("Character Name"))->getText();
		    if (!username.empty())
		    {
                Packet *packet = new Packet(PAMSG_CHAR_CREATE);
                packet->setString(username);
                networkManager->sendPacket(packet);
		    }
		}

        SDL_Delay(0);

        return true;
    }
}
