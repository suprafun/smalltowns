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

#include "updatestate.h"
#include "characterstate.h"
#include "input.h"
#include "game.h"
#include "resourcemanager.h"

#include "graphics/graphics.h"
#include "interface/interfacemanager.h"

#include "net/networkmanager.h"
#include "net/packet.h"
#include "net/protocol.h"

#include "utilities/log.h"
#include "utilities/xml.h"

#include <SDL.h>

namespace ST
{
    void clicked(AG_Event *event)
    {
        UpdateState *state = static_cast<UpdateState*>(AG_PTR(1));
        state->mAccepted = true;
    }

    std::string readFile(const std::string &filename)
    {
        std::string fileString;
        char buffer[256];
        int size = 0;
        FILE *file = fopen(filename.c_str(), "r");
		if (!file)
			return "";
        do
        {
            memset(buffer, 0, 256);
            size = fread(buffer, 255, 1, file);
            fileString.append(buffer);
        } while (size != 0 || size >= 255);

        return fileString;
    }

    UpdateState::UpdateState()
    {
        XMLFile file;

#ifndef __APPLE__
        if (file.load("townslife.cfg"))
#else
		if (file.load(resourceManager->getDataPath() + "townslife.cfg"))
#endif
        {
            file.setElement("newshost");
            hostname = file.readString("newshost", "host");
            filename = file.readString("newshost", "file");
        }
        else
        {
            logger->logWarning("Error loading configuration for newshost.");
        }
		file.close();

        mAccepted = false;
        mSuccess = false;
    }

    void UpdateState::enter()
    {
        AG_Window *win = AG_WindowNew(AG_WINDOW_PLAIN|AG_WINDOW_KEEPBELOW);
		AG_WindowShow(win);
		AG_WindowMaximize(win);
		interfaceManager->addWindow(win);

		int halfScreenWidth = graphicsEngine->getScreenWidth() / 2;
		int halfScreenHeight = graphicsEngine->getScreenHeight() / 2;

		AG_Window *newsWin = AG_WindowNew(AG_WINDOW_NOBUTTONS|AG_WINDOW_KEEPABOVE);
		AG_WindowSetCaption(newsWin, "News");
		AG_WindowSetGeometry(newsWin, halfScreenWidth - 200, halfScreenHeight - 125, 400, 250);
		AG_WindowSetSideBorders(newsWin, 3);
        AG_WindowShow(newsWin);
		interfaceManager->addWindow(newsWin);

		text = AG_LabelNew(newsWin, AG_LABEL_EXPAND, "Downloading Updates");
		//AG_Expand(text);

		button = AG_ButtonNewFn(newsWin, 0, "OK", clicked, "%p", this);
		AG_ButtonDisable(button);
    }

    void UpdateState::exit()
    {
        interfaceManager->removeAllWindows();
    }

    bool UpdateState::update()
    {
        if (!mSuccess)
        {
            logger->logDebug("Downloading news update.");
            mSuccess = networkManager->downloadFile(hostname, filename);
            logger->logDebug("Finished downloading news update.");
            AG_ButtonEnable(button);
            std::string news = readFile(resourceManager->getWritablePath() + filename);
            if (!news.empty())
                AG_LabelText(text, news.c_str());
        }

        if (mAccepted)
        {
            logger->logDebug("Retrieving character list");
            Packet *packet = new Packet(PAMSG_CHAR_LIST);
            networkManager->sendPacket(packet);
			mAccepted = false;
        }

        if (inputManager->getKey(AG_KEY_ESCAPE))
		{
			return false;
		}

        SDL_Delay(0);

        return true;
    }
}
