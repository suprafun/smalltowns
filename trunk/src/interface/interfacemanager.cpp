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
 *	Date of file creation: 08-10-29
 *
 *	$Id$
 *
 ********************************************/

#include "interfacemanager.h"

#include "../being.h"
#include "../beingmanager.h"
#include "../input.h"
#include "../resourcemanager.h"

#include "../graphics/camera.h"
#include "../graphics/graphics.h"
#include "../graphics/texture.h"

#include "../utilities/log.h"
#include "../utilities/xml.h"

#include <agar/core.h>
#include <agar/gui.h>

void end_chat(AG_Event *event)
{
    int id = AG_INT(1);
    ST::Being *being = ST::beingManager->findBeing(id);
    if (being)
    {
        being->setTalking(false);
        being->toggleName();
    }
    AG_WindowHide(ST::interfaceManager->getWindow("/NPC"));
    AG_WindowHide(ST::interfaceManager->getWindow("/Avatar"));
}

namespace ST
{
	InterfaceManager::InterfaceManager()
	{
        if (AG_InitCore("townslife", 0) == -1)
        {
            logger->logError("Error: Unable to init agar");
        }

        if (AG_InitVideoSDL(graphicsEngine->getSurface(), AG_VIDEO_OPENGL_OR_SDL) == -1)
        {
            logger->logError("Error: Unable to init video");
        }

        mGuiSheet = 0;
		int halfScreenWidth = (int)(graphicsEngine->getScreenWidth() * 0.5);
		mErrorWindow = AG_WindowNewNamed(0, "Error");
		AG_WindowSetCaption(mErrorWindow, "Error");
		AG_WindowSetGeometry(mErrorWindow, halfScreenWidth - 150, 50, 300, 75);

		mErrorCaption = AG_LabelNewString(mErrorWindow, 0, "");
		AG_LabelSizeHint(mErrorCaption, 1, "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
		AG_LabelJustify(mErrorCaption, AG_TEXT_CENTER);

		mPlayerWindow = AG_WindowNewNamed(AG_WINDOW_NOMOVE|AG_WINDOW_PLAIN|AG_WINDOW_NOBUTTONS, "Player");
		mPlayerLabel = AG_LabelNewString(mPlayerWindow, 0, "");
		AG_LabelSizeHint(mPlayerLabel, 1, "XXXXXXXXXXXX");
		AG_LabelJustify(mPlayerLabel, AG_TEXT_CENTER);

		mNPCWindowPos.x = 80;
		mNPCWindowPos.y = graphicsEngine->getScreenHeight()-110;

		mNPCWindow = AG_WindowNewNamed(AG_WINDOW_NOMOVE|AG_WINDOW_PLAIN|AG_WINDOW_NOBUTTONS, "NPC");
		AG_WindowSetCaption(mNPCWindow, "NPC");
		mNPCLabel = AG_LabelNewString(mNPCWindow, 0, "");
		AG_LabelSizeHint(mNPCLabel, 1, "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
		AG_LabelJustify(mNPCLabel, AG_TEXT_CENTER);
		mNPCButton = AG_ButtonNewS(mNPCWindow, 0, "Bye!");
		AG_WindowSetGeometry(mNPCWindow, mNPCWindowPos.x, mNPCWindowPos.y, 200, 110);

		mNPCAvatar = AG_WindowNewNamed(
                                 AG_WINDOW_NOMOVE|AG_WINDOW_PLAIN|AG_WINDOW_NOBUTTONS|AG_WINDOW_NOBACKGROUND,
                                 "Avatar");
        AG_WindowSetGeometry(mNPCAvatar, graphicsEngine->getScreenWidth()-272, graphicsEngine->getScreenHeight()-349, 272, 379);

		mPlayerWindowPos.x = 0;
		mPlayerWindowPos.y = 0;
		cachedCamPt.x = 0;
		cachedCamPt.y = 0;

        mouse = new Mouse;
		mouse->cursor = NULL;
		mouse->cursorPos.x = 0;
		mouse->cursorPos.y = 0;

		reset();
	}

	InterfaceManager::~InterfaceManager()
	{
	    delete mouse;
		AG_ObjectDetach(mErrorWindow);
		removeAllWindows();
		AG_DestroyVideo();
		AG_Destroy();
	}

	void InterfaceManager::reset()
	{
        AG_ResizeDisplay(graphicsEngine->getScreenWidth(), graphicsEngine->getScreenHeight());
	}

	bool InterfaceManager::loadGuiSheet(const std::string &filename)
	{
		if (mGuiSheet == 0)
			mGuiSheet = new XMLFile;

        // Check it opened successfully
        if (!mGuiSheet->load(filename))
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
/*	    AG_Window *win;
	    char driverName[255];
	    AG_ObjectCopyName(&agDrivers, driverName, 255);
	    std::string str = driverName;
	    str.append(name);
	    AG_FOREACH_WINDOW(win, agDriverSw)
        {
            char windowName[255];
            AG_ObjectCopyName(win, windowName, 255);
            if (strncmp(windowName, str.c_str(), str.size()) == 0)
            {
                return win;
            }
        }

        return NULL;
*/
        return static_cast<AG_Window*>(AG_WidgetFind(agDriverSw, name.c_str()));
	}

	void InterfaceManager::removeWindow(const std::string &name)
	{
	    AG_Window *win = getWindow(name);
	    AG_ObjectDetach(win);
	    mWindows.remove(win);
	}

	void InterfaceManager::removeAllWindows()
	{
		WindowItr itr_end = mWindows.end();
		for (WindowItr itr = mWindows.begin(); itr != itr_end; ++itr)
		{
			AG_ObjectDetach(*itr);
		}
		mWindows.clear();
        NameItr name_itr = mNames.begin(), name_itr_end = mNames.end();
        while (name_itr != name_itr_end)
        {
            AG_ObjectDestroy(name_itr->second);
            ++name_itr;
        }
        mNames.clear();
	}

	void InterfaceManager::showWindow(const std::string &name, bool value)
	{
	    AG_Window *win = getWindow(name);
	    if (win)
	    {
            value ? AG_WindowShow(win) : AG_WindowHide(win);
	    }
	}

	void InterfaceManager::drawWindows()
	{
	    moveWindows();
		AG_Window *win;
		AG_FOREACH_WINDOW(win, agDriverSw)
		{
		    AG_ObjectLock(win);
		    AG_WindowDraw(win);
		    AG_ObjectUnlock(win);
		}
	}

	void InterfaceManager::moveWindows(bool force)
	{
	    Point camPt;
	    camPt.x = 0;
	    camPt.y = 0;
	    Point pt;
	    if (graphicsEngine->getCamera())
	    {
	        camPt = graphicsEngine->getCamera()->getPosition();
	    }
	    if (cachedCamPt.x != camPt.x || cachedCamPt.y != camPt.y || force)
	    {
	        cachedCamPt = camPt;
	        pt = mPlayerWindowPos;
            AG_WindowSetGeometry(mPlayerWindow, pt.x - camPt.x, pt.y - camPt.y, 75, 20);
            pt = mNPCWindowPos;
            AG_WindowSetGeometry(mNPCWindow, pt.x - camPt.x, pt.y - camPt.y, 120, 45);
	    }
	}

	void InterfaceManager::drawName(const std::string &name, const Point &pt, bool draw)
	{
	    AG_LabelTextS(mPlayerLabel, name.c_str());
	    if (!draw)
	    {
	        if (AG_WindowIsVisible(mPlayerWindow))
                AG_WindowHide(mPlayerWindow);
	    }
        else
        {
            if (!AG_WindowIsVisible(mPlayerWindow))
                AG_WindowShow(mPlayerWindow);
        }

        mPlayerWindowPos = pt;
        mPlayerWindowPos.y -= 5;
	    AG_WindowSetGeometry(mPlayerWindow, mPlayerWindowPos.x, mPlayerWindowPos.y, 75, 20);
	    moveWindows(true);

	}

	AG_Widget* InterfaceManager::getChild(AG_Widget *parent, const std::string &name)
	{
	    AG_Widget *widget;
	    std::string str = agDrivers.archivePath;
	    str.append(name);
	    AGOBJECT_FOREACH_CHILD(widget, parent, ag_widget)
        {
            char widgetName[64];
            AG_ObjectCopyName(widget, widgetName, 30);
            if (strncmp(widgetName, str.c_str(), str.size()) == 0)
            {
                return widget;
            }
        }

        return NULL;
	}

	void InterfaceManager::setErrorMessage(const std::string &msg)
	{
		AG_LabelString(mErrorCaption, msg.c_str());
	}

	void InterfaceManager::showErrorWindow(bool show)
	{
		if (show)
		{
			AG_WindowShow(mErrorWindow);
			AG_WindowFocus(mErrorWindow);
		}
		else
		{
			AG_WindowHide(mErrorWindow);
		}
	}

	void InterfaceManager::sendToChat(const std::string &msg)
	{
		int lines = 0;
		const unsigned int lineSize = 50;

        // find how many lines are needed
		lines = msg.size() / lineSize;
		if ((msg.size() % lineSize) > 0)
			++lines;

        AG_Console *chat = static_cast<AG_Console*>(AG_WidgetFind(agDriverSw, "/ChatWindow/ChannelsFolder/GlobalChat/Chat"));
        if (chat)
        {
            unsigned int pos = 0;
            unsigned int npos = msg.size();

            // check if the size of the message is greater than the max number of lines allowed
            if (npos > lineSize)
                npos = lineSize;

            // cycle through each line and output to console each line
            for (int line = 0; line < lines; ++line)
            {
                AG_ConsoleAppendLine(chat, msg.substr(pos, npos).c_str());
                pos += npos;
                if (pos + npos > msg.size())
                    npos = msg.size() - pos;
            }
        }
	}

	void InterfaceManager::addNPCChat(Being *being, const std::string &msg)
	{
	    AG_LabelTextS(mNPCLabel, msg.c_str());
	    AG_SetEvent(mNPCButton, "button-pushed", end_chat, "%d", being->getId());
	    AG_Surface *surface = NULL;
	    if (graphicsEngine->isOpenGL())
        {
            AG_PixmapFromTexture(mNPCAvatar, 0, resourceManager->getBeingAvatar(being->getId())->getGLTexture(), 0);
        }
        else
        {
            SDL_Surface *s = resourceManager->getBeingAvatar(being->getId())->getSDLSurface();
            SDL_LockSurface(s);
            unsigned int rmask, gmask, bmask, amask;
            #if SDL_BYTEORDER == SDL_BIG_ENDIAN
            rmask = 0xff000000;
            gmask = 0x00ff0000;
            bmask = 0x0000ff00;
            amask = 0x000000ff;
            #else
            rmask = 0x000000ff;
            gmask = 0x0000ff00;
            bmask = 0x00ff0000;
            amask = 0xff000000;
            #endif
            surface = AG_SurfaceFromPixelsRGBA(s->pixels, s->w, s->h, s->format->BitsPerPixel, rmask, gmask, bmask, amask);
            SDL_UnlockSurface(s);
            AG_PixmapFromSurface(mNPCAvatar, 0, surface);
        }

        AG_WindowSetGeometry(mNPCWindow, mNPCWindowPos.x, mNPCWindowPos.y, 12 * strlen(msg.c_str()), 110);

        AG_WindowShow(mNPCWindow);
        AG_WindowShow(mNPCAvatar);
        moveWindows(true);
	}

	void InterfaceManager::addMouseListener(myfunc func)
	{
	    mListeners.push_back(func);
	}

	void InterfaceManager::removeMouseListeners()
	{
	    mListeners.clear();
	}

	void InterfaceManager::handleMouseEvent(int button, int x, int y, int type)
	{
        ListenerItr itr = mListeners.begin(), itr_end = mListeners.end();
        while (itr != itr_end)
        {
            Event evt;
            evt.button = button;
            evt.x = x;
            evt.y = y;
            evt.type = type;
            (*(*itr))(&evt);

            ++itr;
        }
	}
}
