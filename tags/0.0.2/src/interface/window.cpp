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

#include "window.h"

#include "../input.h"

#include "../graphics/graphics.h"
#include "../graphics/texture.h"

#include <SDL_opengl.h>

namespace ST
{
	WindowTitle::WindowTitle() : mHeight(0)
	{

	}

	int WindowTitle::getHeight() const
	{
		return mHeight;
	}

	std::string WindowTitle::getTitle()
	{
		return mTitle;
	}

	void WindowTitle::setHeight(int height)
	{
		mHeight = height;
	}

	void WindowTitle::setTitle(const std::string &title)
	{
		mTitle = title;
	}

	Window::Window(std::string name)
		: mName(name), mParent(NULL), mBackground(NULL), mVisible(true), mFocus(false)
	{
		setPosition(0, 0);
		setSize(1, 1);
	}

	Window::Window(Window *parent, std::string name)
		: mName(name), mParent(parent)
	{
		setPosition(0, 0);
		setSize(1, 1);
	}

    void Window::addChild(Window *window)
    {
        mChildren.push_back(window);
    }

	void Window::addParent(Window *window)
	{
		mParent = window;

		if (mParent)
		{
			// get current width and height
			int width = mSize.x;
			int height = mSize.y;
			// get position based on offset from parent window
			int x = mParent->getPosition().x + mPosition.x;
			int y = (mParent->getPosition().y - mParent->getHeight()) + mPosition.y;
			// get boundaries
			int max_x = mParent->getPosition().x + mParent->getWidth();
			int max_y = mParent->getPosition().y;

			// ensure the position isnt outside the parent window
			if (x > max_x)
			{
				x = max_x - width;
				if (x < mParent->getPosition().x)
					x = mParent->getPosition().x;
			}
			if (y > max_y)
			{
				setVisible(false);
			}

			//ensure the size doesnt take it outside the parent window
			if (x + width > max_x)
			{
				width = max_x - x;
			}
			if (y - height > max_y)
			{
				height = max_y - y;
			}

			// set new position and size based on parent window
			setSize(width, height);
			setPosition(x, y);
		}
	}

	void Window::setVisible(bool visible)
	{
		mVisible = visible;
	}

	bool Window::getVisible() const
	{
		return mVisible;
	}

	std::string Window::getName() const
	{
		return mName;
	}

    void Window::setPosition(int x, int y)
    {
		mPosition.x = x;
		mPosition.y = y;

		mClipArea.x = x;
		mClipArea.y = y;
    }

	Point& Window::getPosition()
	{
		return mPosition;
	}

	void Window::setSize(int width, int height)
	{
		mSize.x = width;
		mSize.y = height;

		mClipArea.width = mPosition.x + width;
		mClipArea.height = mPosition.y + height;
	}

	int Window::getWidth() const
	{
		return mSize.x;
	}

	int Window::getHeight() const
	{
		return mSize.y;
	}

	void Window::setFocus(bool focused)
	{
	    mFocus = focused;
	}

	void Window::setTitleHeight(int height)
	{
		mTitle.setHeight(height);
	}

	Rectangle& Window::getClipArea()
	{
		return mClipArea;
	}

	void Window::setClipArea(Rectangle &rect)
	{
		mClipArea.x = rect.x;
		mClipArea.y = rect.y;
		mClipArea.width = rect.width;
		mClipArea.height = rect.height;
	}

	int Window::getNumChildren() const
	{
	    return mChildren.size();
	}

	void Window::setBackground(const std::string &bg)
	{
		if (mBackground)
		{
			delete mBackground;
			mBackground = NULL;
		}
		graphicsEngine->loadTexture(bg);
		mBackground = graphicsEngine->getTexture(bg);
	}

	void Window::drawWindow()
	{
		if (!mVisible)
			return;

		// get window, make room for title bar
	    Rectangle rect;
		rect.x = mPosition.x;
		rect.y = mPosition.y - mTitle.getHeight();
		rect.width = mSize.x;
		rect.height = mSize.y;

		// draw title
		if (mTitle.getHeight() > 0)
		{
			Rectangle titleRect;
			titleRect.x = rect.x;
			titleRect.y = mPosition.y;
			titleRect.width = getWidth();
			titleRect.height = mTitle.getHeight();

			graphicsEngine->drawRect(titleRect, true);
		}

		if (!mTitle.getTitle().empty())
		{
			Point p = getPosition();
			p.y -= mTitle.getHeight() + 2;
			graphicsEngine->drawText(p, mTitle.getTitle(), 10);
		}

		// enable texture mapping
		if (mBackground)
		{
			//draw filled texture mapped rectangle
			graphicsEngine->drawTexturedRect(rect, mBackground->getGLTexture());
		}
		else
		{
			graphicsEngine->drawRect(rect, false);
		}
	}

	void Window::processKey(SDL_keysym key)
	{

	}

	void Window::processMouse(MouseButton *button)
	{

	}
}
