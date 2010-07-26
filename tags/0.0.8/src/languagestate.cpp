/*********************************************
 *
 *	Author: David Athay
 *
 *	License: New BSD License
 *
 *	Copyright (c) 2010, CT Games
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
 *	Date of file creation: 10-07-11
 *
 *	$Id$
 *
 ********************************************/

#include "languagestate.h"
#include "input.h"
#include "game.h"
#include "connectstate.h"
#include "resourcemanager.h"

#include "graphics/graphics.h"

#include "interface/interfacemanager.h"

#include "net/networkmanager.h"

#include "utilities/log.h"
#include "utilities/stringutils.h"
#include "utilities/xml.h"

#include <agar/core.h>
#include <agar/gui.h>

namespace ST
{
    void submit_language(AG_Event *event)
    {
        std::string language;
        char *lang = AG_STRING(1);

        XMLFile file;

        if (file.load(resourceManager->getDataPath("townslife.cfg")))
        {
            language = lang;
            file.addString("language", "value", language);
            file.save();
        }

        free(lang);

        game->setLanguage(language);
        GameState *state = new ConnectState;
        game->changeState(state);
    }

	LanguageState::LanguageState()
	{
	}

	void LanguageState::enter()
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
        std::string name;
        std::string title;
        int w, h;

        if (file.load(resourceManager->getDataPath("language.xml")))
        {
            std::string lang[3];
            std::string value[3];
            std::string icon[3];

            file.setElement("window");
            name = file.readString("window", "name");
            title = file.readString("window", "title");
            w = file.readInt("window", "width");
            h = file.readInt("window", "height");
            file.setSubElement("button");
			lang[0] = file.readString("button", "text");
			value[0] = file.readString("button", "value");
			icon[0] = file.readString("button", "icon");
			file.nextSubElement("button");
			lang[1] = file.readString("button", "text");
			value[1] = file.readString("button", "value");
			icon[1] = file.readString("button", "icon");
			file.nextSubElement("button");
			lang[2] = file.readString("button", "text");
			value[2] = file.readString("button", "value");
			icon[2] = file.readString("button", "icon");
			file.close();

			AG_Window *test = AG_WindowNewNamed(AG_WINDOW_NOBUTTONS|AG_WINDOW_KEEPABOVE, name.c_str());
            AG_WindowSetCaption(test, title.c_str());
            AG_WindowSetSpacing(test, 12);
            AG_WindowSetGeometry(test, halfScreenWidth - (w / 2) , halfScreenHeight - (h / 2), w, h);

            AG_Button *button[3];

            AG_HBox *box = AG_HBoxNew(test, 0);
            for (int i = 0; i < 3; ++i)
            {
                button[i] = AG_ButtonNewFn(box, 0, lang[0].c_str(), submit_language, "%s", strdup(value[i].c_str()));
                AG_Surface *surface;
                SDL_Surface *s = graphicsEngine->loadSDLTexture(resourceManager->getDataPath(icon[i].c_str()));
                SDL_LockSurface(s);
                surface = AG_SurfaceFromPixelsRGBA(s->pixels, s->w, s->h, s->format->BitsPerPixel, s->format->Rmask, s->format->Bmask, s->format->Gmask, s->format->Amask);
                SDL_UnlockSurface(s);
                if (surface)
                    AG_ButtonSurface(button[i], surface);
            }

            AG_WindowShow(test);
            interfaceManager->addWindow(test);
        }
        else
        {
            // XML file wasnt found, load default
            AG_Window *test = AG_WindowNewNamed(AG_WINDOW_NOBUTTONS|AG_WINDOW_KEEPABOVE, "Language");
            AG_WindowSetCaption(test, "Choose Language");
            AG_WindowSetSpacing(test, 12);
            AG_WindowSetGeometry(test, halfScreenWidth - 125, halfScreenHeight - 45, 225, 135);

            AG_Button *button[3];

            button[0] = AG_ButtonNewFn(test, 0, "English", submit_language, "%s", "en");
            button[1] = AG_ButtonNewFn(test, 0, "Portugues", submit_language, "%s", "pt");
            button[2] = AG_ButtonNewFn(test, 0, "Espa\xc3\xb1ol", submit_language, "%s", "es");

            AG_WindowShow(test);
            interfaceManager->addWindow(test);
        }

    }

	void LanguageState::exit()
	{
		interfaceManager->removeAllWindows();
	}

	bool LanguageState::update()
	{
        // Check for input, if escape pressed, exit
		if (inputManager->getKey(AG_KEY_ESCAPE))
		{
			return false;
		}

		SDL_Delay(0);

		return true;
	}

}
