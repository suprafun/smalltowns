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
 *	Date of file creation: 09-01-30
 *
 *	$Id$
 *
 ********************************************/

#include "connectstate.h"
#include "input.h"
#include "game.h"
#include "loginstate.h"
#include "resourcemanager.h"

#include "graphics/camera.h"
#include "graphics/graphics.h"

#include "interface/interfacemanager.h"

#include "net/networkmanager.h"

#include "utilities/log.h"
#include "utilities/stringutils.h"
#include "utilities/xml.h"

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
			interfaceManager->setErrorMessage("");
			interfaceManager->showErrorWindow(false);
        }
        else
        {
            if (connecting)
            {
                interfaceManager->setErrorMessage("Already connecting, please be patient.");
            }

            else if (hostname.empty())
            {
                interfaceManager->setErrorMessage("Invalid hostname, please enter a server to connect to.");
            }

            else if (port == 0)
            {
                interfaceManager->setErrorMessage("Invalid port number, please enter the port to connect to.");
            }

            interfaceManager->showErrorWindow(true);
        }
    }

	ConnectState::ConnectState()
	{
	}

	void ConnectState::enter()
	{
		int screenWidth = graphicsEngine->getScreenWidth();
		int screenHeight = graphicsEngine->getScreenHeight();
		int halfScreenWidth = screenWidth / 2;
		int halfScreenHeight = screenHeight / 2;

		// create window for entering username and password
		AG_Window *win = AG_WindowNew(AG_WINDOW_PLAIN|AG_WINDOW_KEEPBELOW);
		AG_WindowShow(win);
		AG_WindowMaximize(win);
        interfaceManager->addWindow(win);

        // Load windows from file
        XMLFile file;
        std::string filename;
        std::string name;
        std::string title;
        std::string hostText;
        std::string portText;
        std::string buttonText;
        int w;
        int h;

        filename = "connect.";
        filename.append(game->getLanguage());
        filename.append(".xml");

        if (file.load(resourceManager->getDataPath(filename)))
        {
            file.setElement("window");
            name = file.readString("window", "name");
            title = file.readString("window", "title");
            w = file.readInt("window", "width");
            h = file.readInt("window", "height");
            file.setSubElement("input");
			hostText = file.readString("input", "text");
			file.nextSubElement("input");
			portText = file.readString("input", "text");
			file.setSubElement("button");
			buttonText = file.readString("button", "text");
			file.close();

			AG_Window *test = AG_WindowNewNamed(AG_WINDOW_NOBUTTONS|AG_WINDOW_KEEPABOVE, name.c_str());
            AG_WindowSetCaption(test, title.c_str());
            AG_WindowSetSpacing(test, 12);
            AG_WindowSetGeometry(test, halfScreenWidth - (w / 2) , halfScreenHeight - (h / 2), w, h);

            AG_Textbox *hostname = AG_TextboxNew(test, 0, hostText.c_str());
            AG_Textbox *port = AG_TextboxNew(test, AG_TEXTBOX_INT_ONLY, portText.c_str());

            AG_ExpandHoriz(hostname);
            AG_ExpandHoriz(port);

            // set defaults
            AG_TextboxSetString(hostname, "server.casualgamer.co.uk");
            AG_TextboxSetString(port, "9910");

            AG_Button *button = AG_ButtonNewFn(test, 0, buttonText.c_str(), submit_connect, "%p%p", hostname, port);
            AG_ButtonJustify(button, AG_TEXT_CENTER);
            AG_WidgetFocus(button);

            AG_WindowHide(test);
            interfaceManager->addWindow(test);
        }
        else
        {
            logger->logDebug("XML file not found");

            // XML file wasnt found, load default
            AG_Window *test = AG_WindowNewNamed(AG_WINDOW_NOBUTTONS|AG_WINDOW_KEEPABOVE, "Connection");
            AG_WindowSetCaption(test, "Connect to server");
            AG_WindowSetSpacing(test, 12);
            AG_WindowSetGeometry(test, halfScreenWidth - 125, halfScreenHeight - 45, 225, 135);

            AG_Textbox *hostname = AG_TextboxNew(test, 0, "Server: ");
            AG_Textbox *port = AG_TextboxNew(test, AG_TEXTBOX_INT_ONLY, "Port: ");

            // set defaults
            AG_TextboxSetString(hostname, "server.casualgamer.co.uk");
            AG_TextboxSetString(port, "9910");

            AG_Button *button = AG_ButtonNewFn(test, 0, "Submit", submit_connect, "%p%p", hostname, port);
            AG_ButtonJustify(button, AG_TEXT_CENTER);
            AG_WidgetFocus(button);

            AG_WindowHide(test);
            interfaceManager->addWindow(test);
        }

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
		if (inputManager->getKey(AG_KEY_ESCAPE))
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
		    connecting = false;
		    networkManager->disconnect();

		    // reset timeout, and log the error
		    timeout = 0;
		    logger->logWarning("Connecting timed out");
		    interfaceManager->showWindow("/Connection", true);

			// set error label, and stop connecting
		    interfaceManager->setErrorMessage("Error: Connection timed out");
		    interfaceManager->showErrorWindow(true);
		}

		SDL_Delay(0);

		return true;
	}

}

