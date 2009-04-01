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
 *	Date of file creation: 08-10-29
 *
 *	$Id$
 *
 ********************************************/

#include "interfacemanager.h"
#include "window.h"

#include "../input.h"

#include "../graphics/graphics.h"

#include "../utilities/log.h"

#include <tinyxml.h>
#include <agar/core.h>
#include <agar/gui.h>

namespace ST
{
	InterfaceManager::InterfaceManager()
	{
        if (AG_InitCore("townslife", 0) == -1)
        {
            logger->logError("Error: Unable to init agar");
        }

        if (AG_InitVideoSDL(graphicsEngine->getSurface(), 0) == -1)
        {
            logger->logError("Error: Unable to init video");
        }
	}

	InterfaceManager::~InterfaceManager()
	{
		removeAllWindows();
		AG_Destroy();
	}

	bool InterfaceManager::loadGuiSheet(const std::string &filename)
	{
	    bool loaded = false;

        // Open XML file
        TiXmlDocument doc(filename.c_str());
        loaded = doc.LoadFile();

        // Check it opened successfully
        if (!loaded)
        {
            return false;
        }

        return true;
	}

	void InterfaceManager::unloadGuiSheet()
	{

	}

	void InterfaceManager::addWindow(AG_Window *window)
	{
	    mWindows.push_back(window);
	}

	AG_Window* InterfaceManager::getWindow(const std::string &name)
	{
        WindowItr itr = mWindows.begin();
        WindowItr itr_end = mWindows.end();

        while (itr != itr_end)
        {
            char object_name[255];

            AG_ObjectCopyName((*itr), object_name, 255);

            if (strncmp(object_name, name.c_str(), name.size()) == 0)
            {
                return (*itr);
            }

            ++itr;
        }

        return NULL;
	}

	void InterfaceManager::removeWindow(const std::string &name)
	{
	    /*
		WindowItr itr = mWindows.find(name);
		if (itr != mWindows.end())
		{
		    if (mFocused == itr->second)
                mFocused = NULL;
		    delete itr->second;
			mWindows.erase(itr);
		}
		*/
	}

	void InterfaceManager::removeAllWindows()
	{
		WindowItr itr_end = mWindows.end();
		for (WindowItr itr = mWindows.begin(); itr != itr_end; ++itr)
		{
			AG_ViewDetach(*itr);
		}
		mWindows.clear();
	}

	void InterfaceManager::drawWindows()
	{
	    AG_LockVFS(agView);
	    AG_BeginRendering();

		WindowItr itr_end = mWindows.end();
		for (WindowItr itr = mWindows.begin(); itr != itr_end; ++itr)
		{
			if (AG_WindowIsVisible(*itr))
            {
                AG_WindowDraw(*itr);
            }
		}

		AG_EndRendering();
		AG_UnlockVFS(agView);
	}

	void InterfaceManager::sendKey(SDL_keysym key)
	{

	}
}
