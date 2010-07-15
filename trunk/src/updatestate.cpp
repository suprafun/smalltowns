/*********************************************
 *
 *	Author: David Athay
 *
 *	License: New BSD License
 *
 *	Copyright (c) 2008, CT Games
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
#include "utilities/stringutils.h"
#include "utilities/xml.h"

#include <SDL.h>
#include <sstream>

namespace ST
{
    void clicked(AG_Event *event)
    {
        logger->logDebug("Retrieving character list");
        Packet *packet = new Packet(PAMSG_CHAR_LIST);
        networkManager->sendPacket(packet);
        AG_ButtonDisable(static_cast<AG_Button*>(AG_SELF()));
    }

    UpdateState::UpdateState()
    {
        XMLFile file;
		if (file.load(resourceManager->getDataPath("townslife.cfg")))
        {
            file.setElement("newshost");
            hostname = file.readString("newshost", "host");
            filename = file.readString("newshost", "file");

            // find language for news and replace with current language
            size_t pos = filename.find("$lang");
            if (pos != std::string::npos)
            {
                filename.replace(pos, 5, game->getLanguage());
            }
        }
        else
        {
            logger->logWarning("Error loading configuration for newshost.");
        }
		file.close();
        mContent = 0;
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

		button = AG_ButtonNewFn(newsWin, 0, "OK", clicked, "");
		AG_ButtonDisable(button);

        // get news file
		if (networkManager->downloadFile(hostname, filename))
		{
		    std::string news = utils::readFile(resourceManager->getWritablePath() + filename);
		    AG_LabelText(text, "%s", news.c_str());
		}
		else
		{
		    AG_LabelText(text, "%s", "Error downloading news.");
		}

		// get content files
        mContent = networkManager->downloadContent(hostname);
    }

    void UpdateState::exit()
    {
        interfaceManager->removeAllWindows();
    }

    bool UpdateState::update()
    {
        if (mContent < networkManager->getTotalDownloads())
        {
            mContent = networkManager->downloadContent(hostname);

            if (mContent == -1)
            {
                logger->logError("Unable to download content updates");
                return false;
            }
            std::stringstream str;
            str << "Downloading " << mContent << " of " << networkManager->getTotalDownloads();
            logger->logDebug(str.str());
        }
        else
        {
            AG_ButtonEnable(button);
        }

        if (inputManager->getKey(AG_KEY_ESCAPE))
		{
			return false;
		}

        SDL_Delay(0);

        return true;
    }
}
