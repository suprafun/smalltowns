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

#include "interface/interfacemanager.h"

#include "net/networkmanager.h"

#include "utilities/log.h"
#include "utilities/stringutils.h"

#include <SDL.h>
#include <sstream>

namespace ST
{
    int timeout = 0;
    bool connecting = false;

    void submit_connect(AG_Event *event)
    {
        std::string hostname;
        int port = 0;
        // get the pointers to the input boxes
        AG_Textbox *one = static_cast<AG_Textbox*>(AG_PTR(1));
        AG_Textbox *two = static_cast<AG_Textbox*>(AG_PTR(2));
        AG_Label *error = static_cast<AG_Label*>(AG_PTR(3));

        // check they are valid, then assign their values
        if (one && two)
        {
            hostname = AG_TextboxDupString(one);
            port = AG_TextboxInt(two);
        }

        // check the input isnt blank and not already connecting
        if (!connecting && !hostname.empty() && port != 0)
        {
            // set when timeout starts, connect to server
            timeout = SDL_GetTicks();
            networkManager->connect(hostname, port);
            connecting = true;

            // reset any error messages
			if (error)
	            AG_LabelString(error, "");

            interfaceManager->showWindow("/Error", false);
        }
        else
        {
			if (!error)
				return;
            if (connecting)
            {
                AG_LabelString(error, "Already connecting, please be patient.");
            }

            else if (hostname.empty())
            {
                AG_LabelString(error, "Invalid hostname, please enter a server to connect to.");
            }

            else if (port == 0)
            {
                AG_LabelString(error, "Invalid port number, please enter the port to connect to.");
            }

            interfaceManager->showWindow("/Error", true);
        }
    }

	ConnectState::ConnectState()
	{
	}

	void ConnectState::enter()
	{
		int screenWidth = graphicsEngine->getScreenWidth();
		int screenHeight = graphicsEngine->getScreenHeight();
		float halfScreenWidth = screenWidth / 2.0f;
		float halfScreenHeight = screenHeight / 2.0f;

		// create window for entering username and password
		AG_Window *win = AG_WindowNew(AG_WINDOW_PLAIN|AG_WINDOW_KEEPBELOW);
		AG_WindowShow(win);
		AG_WindowMaximize(win);

		AG_Window *errorWindow = AG_WindowNewNamed(0, "Error");
		AG_WindowSetCaption(errorWindow, "Error");
		AG_WindowSetGeometry(errorWindow, halfScreenWidth - 300, 50, 300, 75);
		error = AG_LabelNewString(errorWindow, 0, "");
		AG_LabelSizeHint(error, 1, "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
		AG_LabelJustify(error, AG_TEXT_CENTER);

		AG_Window *test = AG_WindowNewNamed(AG_WINDOW_NOBUTTONS|AG_WINDOW_KEEPABOVE, "Connection");
		AG_WindowSetCaption(test, "Connect to server");
		AG_WindowSetSpacing(test, 12);
		AG_WindowSetGeometry(test, halfScreenWidth - 125, halfScreenHeight - 45, 225, 135);

		AG_Textbox *hostname = AG_TextboxNew(test, 0, "Server: ");
		AG_Textbox *port = AG_TextboxNew(test, AG_TEXTBOX_INT_ONLY, "Port: ");

		// set defaults
		AG_TextboxSetString(hostname, "server.casualgamer.co.uk");
		AG_TextboxSetString(port, "9601");

		AG_Button *button = AG_ButtonNewFn(test, 0, "Submit", submit_connect, "%p%p%p", hostname, port, error);
		AG_ButtonJustify(button, AG_TEXT_CENTER);

		AG_WindowHide(test);

		interfaceManager->addWindow(win);
		interfaceManager->addWindow(errorWindow);
		interfaceManager->addWindow(test);

		timeout = SDL_GetTicks();
        networkManager->connect();
        connecting = true;
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

        // when connected, send the version
		if (networkManager->isConnected() && connecting)
		{
		    connecting = false;
		    networkManager->sendVersion();
		}

        // check if its timedout
		if (timeout && (SDL_GetTicks() - timeout > 5000))
		{
		    // set error label, and stop connecting
		    AG_LabelString(error, "Error: Connection timed out");
		    interfaceManager->showWindow("/Error", true);

		    connecting = false;
		    networkManager->disconnect();

		    // reset timeout, and log the error
		    timeout = 0;
		    logger->logWarning("Connecting timed out");
		    interfaceManager->showWindow("/Connection", true);
		}

		SDL_Delay(0);

		return true;
	}

}

