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
 *	Date of file creation: 10-04-17
 *
 *	$Id$
 *
 ********************************************/

#include "optionsstate.h"
#include "game.h"
#include "loginstate.h"
#include "input.h"
#include "resourcemanager.h"

#include "graphics/graphics.h"

#include "interface/interfacemanager.h"

#include "utilities/log.h"
#include "utilities/xml.h"

#include <sstream>

namespace ST
{
    int width, height;

    void selected_resolution(AG_Event *event)
    {
        AG_TlistItem *item = static_cast<AG_TlistItem*>(AG_PTR(1));
        width = static_cast<Point*>(item->p1)->x;
        height = static_cast<Point*>(item->p1)->y;
    }

    void apply_options(AG_Event *event)
    {
        int opengl = *static_cast<int*>(AG_PTR(1));
        int fs = *static_cast<int*>(AG_PTR(2));
        int lang = *static_cast<int*>(AG_PTR(3));

        std::stringstream str;
        str << "Fullscreen: " << fs;
        logger->logDebug(str.str());

        if (lang == -1)
            return;

        switch (lang)
        {
            case 0:
                game->setLanguage("en");
                break;

            case 1:
                game->setLanguage("pt");
                break;

            case 2:
                game->setLanguage("es");
                break;
        }

        interfaceManager->removeAllWindows();
        game->restart(opengl, fs, width, height);

        XMLFile file;
        if (file.load(resourceManager->getDataPath("townslife.cfg")))
        {
            file.setElement("graphics");

            file.changeInt("graphics", "opengl", opengl);
            if (fs)
                file.changeString("graphics", "fullscreen", "true");
            else
                file.changeString("graphics", "fullscreen", "false");
            file.changeInt("graphics", "width", width);
            file.changeInt("graphics", "height", height);

            file.setElement("language");
            file.changeString("language", "value", game->getLanguage());

            file.save();
        }

        GameState *state = new LoginState;
        game->changeState(state);
    }

    void cancel_options(AG_Event *event)
    {
        interfaceManager->removeAllWindows();
        GameState *state = new LoginState;
        game->changeState(state);
    }

    OptionsState::OptionsState()
    {
        mFullscreen = 0;
        mOpenGL = graphicsEngine->isOpenGL();
        std::string lang = game->getLanguage();
        if (lang == "en")
            mLangs = 0;
        else if (lang == "pt")
            mLangs = 1;
        else if (lang == "es")
            mLangs = 2;
        width = 1024;
        height = 768;
    }

    void OptionsState::enter()
    {
        int screenWidth = graphicsEngine->getScreenWidth();
		int screenHeight = graphicsEngine->getScreenHeight();
		int halfScreenWidth = screenWidth / 2;
		int halfScreenHeight = screenHeight / 2;

        AG_Window *win = AG_WindowNew(AG_WINDOW_PLAIN|AG_WINDOW_DENYFOCUS);
		AG_WindowShow(win);
		AG_WindowMaximize(win);

		interfaceManager->addWindow(win);

		// create options window
		AG_Window *optionWindow = AG_WindowNewNamed(AG_WINDOW_NOBUTTONS, "OptionWindow");
		AG_WindowSetCaption(optionWindow, "Options");
		AG_WindowSetGeometry(optionWindow, halfScreenWidth - 85, halfScreenHeight - 85, 170, 170);

		// alignment
		AG_VBox *box = AG_VBoxNew(optionWindow, 0);

		// create list of resolutions
		Point pt;
		pt.x = 1024;
		pt.y = 768;
		mRes.push_back(pt);
		pt.x = 800;
		pt.y = 600;
		mRes.push_back(pt);
		pt.x = 1280;
		pt.y = 1024;
		mRes.push_back(pt);
		pt.x = 1440;
		pt.y = 900;
		mRes.push_back(pt);

		// add selection box for resolution
        selectionBox = AG_UComboNew(box, 0);
        AG_ExpandHoriz(selectionBox);
        AG_UComboSizeHint(selectionBox, "Resolution", mRes.size());

        // loop through all the resolutions
        for (unsigned int i = 0; i < mRes.size(); ++i)
        {
            std::stringstream str;
            str << mRes[i].x << " x " << mRes[i].y;
            AG_TlistAddPtr(selectionBox->list, NULL, str.str().c_str(), &mRes[i]);
        }

        AG_TlistSelect(selectionBox->list, AG_TlistFirstItem(selectionBox->list));
        AG_SetEvent(selectionBox, "ucombo-selected", selected_resolution, NULL);

        std::stringstream fs;
        fs << "Fullscreen: " << mFullscreen;
        logger->logDebug(fs.str());

		// add checkbox for fullscreen
        AG_CheckboxNewInt(box, 0, "Fullscreen", &mFullscreen);

        // add checkbox for hardware acceleration
        AG_CheckboxNewInt(box, 0, "OpenGL", &mOpenGL);

        // add radio for languages
        const char *languages[] = { "English", "Portugues", "Espa\xc3\xb1ol", NULL};
        AG_RadioNewInt(box, 0, languages, &mLangs);

        AG_HBox *hbox = AG_HBoxNew(box, 0);
		// add button to apply
		AG_ButtonNewFn(hbox, 0, "Apply", apply_options, "%p%p%p", &mOpenGL, &mFullscreen, &mLangs);

		// add button to cancel
        AG_ButtonNewFn(hbox, 0, "Cancel", cancel_options, 0);

        AG_WindowShow(optionWindow);
        interfaceManager->addWindow(optionWindow);
    }

    void OptionsState::exit()
    {

    }

    bool OptionsState::update()
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
