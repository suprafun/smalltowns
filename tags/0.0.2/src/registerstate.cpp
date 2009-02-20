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
 *	Date of file creation: 09-02-09
 *
 *	$Id$
 *
 ********************************************/

#include "registerstate.h"
#include "loginstate.h"
#include "input.h"
#include "game.h"
#include "player.h"

#include "graphics/graphics.h"

#include "interface/button.h"
#include "interface/interfacemanager.h"
#include "interface/label.h"
#include "interface/textfield.h"
#include "interface/window.h"

#include "net/networkmanager.h"
#include "net/packet.h"
#include "net/protocol.h"

#include <SDL.h>

namespace ST
{
	RegisterState::RegisterState()
	{

	}

	void RegisterState::enter()
	{
		int screenWidth = graphicsEngine->getScreenWidth();
	    int screenHeight = graphicsEngine->getScreenHeight();

        // create window for entering username and password
		Window *win = new Window("Register Window");
		win->setPosition(50, screenHeight - 50);
		win->setSize(screenWidth - 100, screenHeight - 100);
		interfaceManager->addWindow(win);

		// create label for error messages
		Label *errorLabel = new Label("error");
		errorLabel->setPosition(150, win->getHeight() - 25);
		errorLabel->setText("");
		errorLabel->setFontSize(24);
		interfaceManager->addSubWindow(win, errorLabel);

		Label *usernameLabel = new Label("userLabel");
		usernameLabel->setPosition(100, screenHeight - 300);
		usernameLabel->setText("Username: ");
		usernameLabel->setFontSize(18);
		interfaceManager->addSubWindow(win, usernameLabel);

		Label *passwordLabel = new Label("passLabel");
		passwordLabel->setPosition(100, screenHeight - 350);
		passwordLabel->setText("Password: ");
		passwordLabel->setFontSize(18);
		interfaceManager->addSubWindow(win, passwordLabel);

        TextField *username = new TextField("username");
        username->setPosition(200, screenHeight - 285);
        username->setSize(180, 21);
        username->setFontSize(18);
        interfaceManager->addSubWindow(win, username);

        TextField *password = new TextField("password");
        password->setPosition(200, screenHeight - 335);
        password->setSize(180, 21);
        password->setFontSize(18);
        interfaceManager->addSubWindow(win, password);

        Button *button = new Button("Submit");
        button->setPosition(screenWidth - 200, 100);
        button->setSize(80, 24);
        button->setText("Submit");
        button->setFontSize(18);
        interfaceManager->addSubWindow(win, button);

        // set focus on first text field
        interfaceManager->changeFocus(username);
	}

	bool RegisterState::update()
	{
        // Check for input, if escape pressed, exit
		if (inputManager->getKey(SDLK_ESCAPE))
		{
		    GameState *state = new LoginState();
			game->changeState(state);
			return true;
		}
		else if (inputManager->getKey(SDLK_TAB))
		{
		    if (interfaceManager->getFocused()->getName() == "username")
                interfaceManager->changeFocus(interfaceManager->getWindow("password"));
            else
                interfaceManager->changeFocus(interfaceManager->getWindow("username"));
		}
		else if (inputManager->getKey(SDLK_RETURN) ||
			static_cast<Button*>(interfaceManager->getWindow("Submit"))->clicked())
		{
		    submit();
		}

		SDL_Delay(0);

		return true;
	}

	void RegisterState::exit()
    {
        interfaceManager->removeAllWindows();
    }

    void RegisterState::submit()
    {
        std::string username = static_cast<TextField*>(interfaceManager->getWindow("username"))->getText();
        std::string password = static_cast<TextField*>(interfaceManager->getWindow("password"))->getText();

        if (username.empty() || password.empty())
        {
            static_cast<Label*>(interfaceManager->getWindow("error"))->setText("Invalid username or password");
            return;
        }

        player->setName(username);
        Packet *packet = new Packet(PAMSG_REGISTER);
        packet->setString(username);
        packet->setString(password);
        networkManager->sendPacket(packet);
        static_cast<Label*>(interfaceManager->getWindow("error"))->setText("Sending...");
    }
}
