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
 *	Date of file creation: 09-01-22
 *
 *	$Id$
 *
 ********************************************/

#include "loginstate.h"
#include "connectstate.h"
#include "teststate.h"
#include "input.h"
#include "game.h"
#include "player.h"

#include "interface/button.h"
#include "interface/interfacemanager.h"
#include "interface/label.h"
#include "interface/textfield.h"
#include "interface/window.h"

#include <SDL.h>

namespace ST
{
	LoginState::LoginState()
	{

	}

	void LoginState::enter()
	{
		// create window for entering username and password
		Window *win = new Window("Login Window");
		win->setPosition(200, 400);
		win->setSize(375, 200);
		interfaceManager->addWindow(win);

		// create label for error messages
		Label *errorLabel = new Label("error");
		errorLabel->setPosition(240, 570);
		errorLabel->setText("");
		errorLabel->setFontSize(24);
		interfaceManager->addSubWindow(win, errorLabel);

		// create label for username
		Label *usernameLabel = new Label("0");
		usernameLabel->setPosition(240, 335);
		usernameLabel->setText("Username: ");
		usernameLabel->setFontSize(20);
		interfaceManager->addSubWindow(win, usernameLabel);

		// create label for password
//		Label *passwordLabel = new Label("1");
//		interfaceManager->addSubWindow(win, passwordLabel);

		// create textfield for entering username and add to window
		TextField *username = new TextField("Username");
		username->setPosition(335, 350);
		username->setSize(180, 25);
		username->setFontSize(18);
		username->addBackground();
		interfaceManager->addSubWindow(win, username);

		// create textfield for entering password and add to window
//		TextField *password = new TextField("Password");
//		interfaceManager->addSubWindow(win, password);

        Button *button = new Button("Submit");
        button->setPosition(475, 250);
        button->setSize(80,20);
        button->setText("Submit");
        button->setFontSize(18);
		button->addBackground();
        interfaceManager->addSubWindow(win, button);
	}

	void LoginState::exit()
	{
		interfaceManager->removeAllWindows();
	}

	bool LoginState::update()
	{

		// Check for input, if escape pressed, exit
		if (inputManager->getKey(SDLK_ESCAPE))
		{
		    GameState *state = new ConnectState();
			game->changeState(state);
		}

		if (inputManager->getKey(SDLK_RETURN) ||
			static_cast<Button*>(interfaceManager->getWindow("Submit"))->clicked())
		{
		    submit();
		}

		SDL_Delay(0);

		return true;
	}

	void LoginState::submit()
	{
	    std::string username = static_cast<TextField*>(interfaceManager->getWindow("Username"))->getText();
        if (username.size() > 1)
        {
            player->setName(username);
            GameState *state = new TestState;
            game->changeState(state);
        }
	}
}