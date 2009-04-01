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
#include "registerstate.h"
#include "teststate.h"
#include "input.h"
#include "game.h"
#include "player.h"

#include "net/networkmanager.h"
#include "net/packet.h"
#include "net/protocol.h"

#include "interface/button.h"
#include "interface/interfacemanager.h"
#include "interface/label.h"
#include "interface/textfield.h"
#include "interface/window.h"

#include "graphics/graphics.h"

#include <SDL.h>

namespace ST
{
    void submit_login(AG_Event *event)
    {
        std::string username;
	    std::string password;

	    AG_Textbox *one = static_cast<AG_Textbox*>(AG_PTR(1));
	    AG_Textbox *two = static_cast<AG_Textbox*>(AG_PTR(2));

	    if (one && two)
	    {
	        username = AG_TextboxDupString(one);
	        password = AG_TextboxDupString(two);
	    }

        if (!username.empty() && !password.empty())
        {
            player->setName(username);
            Packet *packet = new Packet(PAMSG_LOGIN);
            packet->setString(username);
            packet->setString(password);
            networkManager->sendPacket(packet);
        }
    }

    void submit_register(AG_Event *event)
    {
        std::string username;
	    std::string password;

	    AG_Textbox *one = static_cast<AG_Textbox*>(AG_PTR(1));
	    AG_Textbox *two = static_cast<AG_Textbox*>(AG_PTR(2));

	    if (one && two)
	    {
	        username = AG_TextboxDupString(one);
	        password = AG_TextboxDupString(two);
	    }

        if (!username.empty() && !password.empty())
        {
            player->setName(username);
            Packet *packet = new Packet(PAMSG_REGISTER);
            packet->setString(username);
            packet->setString(password);
            networkManager->sendPacket(packet);
        }
    }

    void create_register(AG_Event *event)
    {
        AG_Window *log = static_cast<AG_Window*>(AG_PTR(1));
        if (log)
            AG_WindowHide(log);
        AG_Window *reg = static_cast<AG_Window*>(AG_PTR(2));
        if (reg)
            AG_WindowShow(reg);
    }

	LoginState::LoginState()
	{

	}

	void LoginState::enter()
	{
		int screenWidth = graphicsEngine->getScreenWidth();
		int screenHeight = graphicsEngine->getScreenHeight();
		float halfScreenWidth = screenWidth / 2.0f;
		float halfScreenHeight = screenHeight / 2.0f;

		AG_Window *win = AG_WindowNew(AG_WINDOW_PLAIN);
		AG_WindowShow(win);
		AG_WindowMaximize(win);

		AG_Window *login = AG_WindowNewNamed(AG_WINDOW_NOBUTTONS, "Login");
		AG_WindowSetCaption(login, "Login");
		AG_WindowSetSpacing(login, 12);
		AG_WindowSetGeometry(login, halfScreenWidth - 125, halfScreenHeight - 45, 225, 135);

		AG_Textbox *username = AG_TextboxNew(login, 0, "Username: ");
		AG_Textbox *password = AG_TextboxNew(login, AG_TEXTBOX_PASSWORD, "Password: ");

		AG_Window *reg = AG_WindowNewNamed(AG_WINDOW_NOBUTTONS, "Register");
		AG_WindowSetCaption(reg, "Registration");
		AG_WindowSetSpacing(reg, 12);
		AG_WindowSetGeometry(reg, halfScreenWidth - 125, halfScreenHeight - 45, 225, 135);

		AG_Textbox *reg_user = AG_TextboxNew(reg, 0, "Username: ");
		AG_Textbox *reg_pass = AG_TextboxNew(reg, 0, "Password: ");
		AG_HBox *reg_box = AG_HBoxNew(reg, 0);
		AG_Button *reg_button = AG_ButtonNewFn(reg_box, 0, "Submit", submit_register, "%p%p", reg_user, reg_pass);
		AG_ButtonJustify(reg_button, AG_TEXT_CENTER);

		AG_HBox *box = AG_HBoxNew(login, 0);
		AG_Button *button = AG_ButtonNewFn(box, 0, "Submit", submit_login, "%p%p", username, password);
		AG_ButtonJustify(button, AG_TEXT_CENTER);
		AG_Button *register_button = AG_ButtonNewFn(box, 0, "Register",
                                                    create_register, "%p%p", login, reg);
		AG_ButtonJustify(register_button, AG_TEXT_CENTER);

		AG_WindowShow(login);

		interfaceManager->addWindow(win);
		interfaceManager->addWindow(login);
		interfaceManager->addWindow(reg);
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
			networkManager->disconnect();
			return true;
		}

		SDL_Delay(0);

		return true;
	}
}
