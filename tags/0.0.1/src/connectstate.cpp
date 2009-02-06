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
 *	Date of file creation: 09-01-30
 *
 *	$Id$
 *
 ********************************************/

#include "connectstate.h"
#include "input.h"
#include "game.h"
#include "loginstate.h"

#include "graphics/camera.h"
#include "graphics/graphics.h"

#include "interface/button.h"
#include "interface/interfacemanager.h"
#include "interface/label.h"
#include "interface/textfield.h"
#include "interface/window.h"

#include "net/networkmanager.h"

#include "utilities/stringutils.h"

#include <SDL.h>

namespace ST
{
	ConnectState::ConnectState()
	{

	}

	void ConnectState::enter()
	{
		// create window for entering username and password
		Window *win = new Window("Connect Window");
		win->setPosition(200, 400);
		win->setSize(375, 200);
		interfaceManager->addWindow(win);

		// create label for username
		Label *hostnameLabel = new Label("HostnameLabel");
		hostnameLabel->setPosition(260, 335);
		hostnameLabel->setText("Server: ");
		hostnameLabel->setFontSize(20);
		interfaceManager->addSubWindow(win, hostnameLabel);

		// create label for password
		Label *portLabel = new Label("PortLabel");
		portLabel->setPosition(260, 285);
		portLabel->setText("Port: ");
		portLabel->setFontSize(20);
		interfaceManager->addSubWindow(win, portLabel);

		// create textfield for entering server name and add to window
		TextField *hostname = new TextField("Host");
		hostname->setPosition(335, 350);
		hostname->setSize(180, 25);
		hostname->setFontSize(18);
		hostname->addBackground();
		interfaceManager->addSubWindow(win, hostname);

		// create textfield for entering port number and add to window
		TextField *port = new TextField("Port");
		port->setPosition(335, 300);
		port->setSize(120, 25);
		port->setFontSize(18);
		port->addBackground();
		interfaceManager->addSubWindow(win, port);

		// create button for submitting details
		Button *button = new Button("Submit");
		button->setPosition(475, 250);
        button->setSize(80,20);
        button->setText("Submit");
        button->setFontSize(18);
		button->addBackground();
        interfaceManager->addSubWindow(win, button);
	}

	void ConnectState::exit()
	{
		interfaceManager->removeAllWindows();
	}

	bool ConnectState::update()
	{

		// Check for input, if escape pressed, exit
		if (inputManager->getKey(SDLK_ESCAPE))
		{
			return false;
		}

		if (inputManager->getKey(SDLK_RETURN))
		{
		    submit();
		}

		if (static_cast<Button*>(interfaceManager->getWindow("Submit"))->clicked())
		{
		    submit();
		}

		if (networkManager->isConnected())
		{
		    GameState *state = new LoginState;
		    game->changeState(state);
		}

		SDL_Delay(0);

		return true;
	}

	void ConnectState::submit()
	{
	    std::string hostname = static_cast<TextField*>(interfaceManager->getWindow("Host"))->getText();
        std::string port = static_cast<TextField*>(interfaceManager->getWindow("Port"))->getText();
        if (hostname.size() > 0)
        {
            if (port == "")
                port = "0";
            networkManager->connect(hostname, utils::toInt(port));
        }
	}
}

