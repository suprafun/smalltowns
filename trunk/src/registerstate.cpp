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

#include <SDL.h>

namespace ST
{
	RegisterState::RegisterState()
	{

	}

	void RegisterState::enter()
	{
	    int screenHeight = graphicsEngine->getScreenHeight();

        // create window for entering username and password
		Window *win = new Window("Register Window");
		win->setPosition(50, screenHeight - 50);
		win->setSize(600, screenHeight - 100);
		interfaceManager->addWindow(win);

		Label *usernameLabel = new Label("userLabel");
		usernameLabel->setPosition(100, screenHeight - 200);
		usernameLabel->setText("Username: ");
		usernameLabel->setFontSize(18);
		interfaceManager->addSubWindow(win, usernameLabel);

		Label *passwordLabel = new Label("passLabel");
		passwordLabel->setPosition(100, screenHeight - 250);
		passwordLabel->setText("Password: ");
		passwordLabel->setFontSize(18);
		interfaceManager->addSubWindow(win, passwordLabel);

        TextField *username = new TextField("username");
        username->setPosition(180, screenHeight - 185);
        username->setSize(180, 31);
        username->setFontSize(18);
        username->addBackground();
        interfaceManager->addSubWindow(win, username);

        TextField *password = new TextField("password");
        password->setPosition(180, screenHeight - 235);
        password->setSize(180, 31);
        password->setFontSize(18);
        password->addBackground();
        interfaceManager->addSubWindow(win, password);

        Button *button = new Button("Submit");
        button->setPosition(475, 250);
        button->setSize(80, 24);
        button->setText("Submit");
        button->setFontSize(18);
		button->addBackground();
        interfaceManager->addSubWindow(win, button);
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
		else if (inputManager->getKey(SDLK_RETURN) ||
			static_cast<Button*>(interfaceManager->getWindow("Submit"))->clicked())
		{
		    //TODO: Send info to server
		}

		SDL_Delay(0);

		return true;
	}

	void RegisterState::exit()
    {
        interfaceManager->removeAllWindows();
    }
}
