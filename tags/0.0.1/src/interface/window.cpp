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
#include "../graphics/node.h"
#include "../graphics/graphics.h"

#include <SDL_opengl.h>

namespace ST
{
	Window::Window(std::string name)
		: mName(name), mParent(NULL), mBackground(NULL), mVisible(true), mFocus(false)
	{
		mPosition.x = 0;
		mPosition.y = 0;
		mSize.x = 1;
		mSize.y = 1;
	}

	Window::Window(Window *parent, std::string name)
		: mName(name), mParent(parent)
	{

	}

    void Window::addChild(Window *window)
    {
        mChildren.push_back(window);
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
    }

	Point& Window::getPosition()
	{
		return mPosition;
	}

	void Window::setSize(int width, int height)
	{
	    mSize.x = width;
	    mSize.y = height;
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

	int Window::getNumChildren() const
	{
	    return mChildren.size();
	}

	void Window::addBackground()
	{

	}


	void Window::setBackground(const std::string &bg)
	{
		ST::Point pos = getPosition();
		mBackground = graphicsEngine->createNode(mName, bg, &pos);
		mBackground->setVisible(false);
	}

	void Window::drawWindow()
	{
	    Rectangle rect;

		// set position and size to local variables
		rect.x = getPosition().x;
		rect.y = getPosition().y - 8; // leave space for title
		rect.width = getWidth();
		rect.height = getHeight();

		// draw title
		Rectangle titleRect;
		titleRect.x = rect.x;
		titleRect.y = rect.y + 8;
		titleRect.width = getWidth();
		titleRect.height = 8;

		graphicsEngine->drawRect(titleRect, true);

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
