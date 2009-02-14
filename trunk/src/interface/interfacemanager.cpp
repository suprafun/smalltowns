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

#include <tinyxml.h>

namespace ST
{
	InterfaceManager::InterfaceManager()
	{
        mFocused = NULL;
	}

	InterfaceManager::~InterfaceManager()
	{
		removeAllWindows();
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

	void InterfaceManager::addWindow(Window *window)
	{
	    if (mFocused == NULL)
            mFocused = window;
		mWindows.insert(std::pair<std::string, Window*>(window->getName(), window));
	}

	void InterfaceManager::addSubWindow(Window *parent, Window *window)
	{
		mWindows.insert(std::pair<std::string, Window*>(window->getName(), window));
        parent->addChild(window);
		window->addParent(parent);
	}

	void InterfaceManager::removeWindow(const std::string &name)
	{
		WindowItr itr = mWindows.find(name);
		if (itr != mWindows.end())
		{
		    if (mFocused == itr->second)
                mFocused = NULL;
		    delete itr->second;
			mWindows.erase(itr);
		}
	}

	void InterfaceManager::removeAllWindows()
	{
		WindowItr itr_end = mWindows.end();
		for (WindowItr itr = mWindows.begin(); itr != itr_end; ++itr)
		{
			delete itr->second;
		}
		mWindows.clear();
		mFocused = NULL;
	}

	Window* InterfaceManager::getWindow(const std::string &name)
	{
		Window *win = 0;
		WindowItr itr = mWindows.find(name);
		if (itr != mWindows.end())
		{
			win = itr->second;
		}
		return win;
	}

	Window* InterfaceManager::getWindow(int x, int y)
	{
	    Window *window;
	    WindowItr itr_end = mWindows.end();
		for (WindowItr itr = mWindows.begin(); itr != itr_end; ++itr)
		{
		    window = itr->second;
		    if (window->getNumChildren() > 0)
                continue;
			if (x >= window->getPosition().x && x <= window->getPosition().x + window->getWidth())
			{
			    if (y <= window->getPosition().y && y >= window->getPosition().y - window->getHeight())
			    {
			        return window;
			    }
			}
		}

		return NULL;
	}

	void InterfaceManager::drawWindows()
	{
		WindowItr itr_end = mWindows.end();
		for (WindowItr itr = mWindows.begin(); itr != itr_end; ++itr)
		{
			if (itr->second->getVisible())
				itr->second->drawWindow();
		}
	}

	void InterfaceManager::changeFocus(Window *window)
	{
	    window->setFocus(true);
	    mFocused->setFocus(false);
	    mFocused = window;
	}

	Window* InterfaceManager::getFocused()
	{
	    return mFocused;
	}

	void InterfaceManager::sendKey(SDL_keysym key)
	{
	    if (mFocused)
            mFocused->processKey(key);
	}

	void InterfaceManager::sendMouse(MouseButton *button)
	{
	    Window *win = getWindow(button->x, button->y);
	    if (win && win != mFocused)
	    {
            changeFocus(win);
	    }
	    if (mFocused)
	    {
            mFocused->processMouse(button);
	    }
	}
}
