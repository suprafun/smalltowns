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

#include <SDL_opengl.h>

namespace ST
{
	Window::Window(std::string name)
		: mName(name), mParent(NULL), mVisible(true), mFocus(false)
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

	void Window::drawWindow()
	{
	    // reset identity matrix
		glLoadIdentity();

		// set position and size to local variables
		float x = (float)getPosition().x;
		float y = (float)getPosition().y;
		float width = (float)getWidth();
		float height = (float)getHeight();

		// move to the correct position
		glTranslatef(x, y, 0.0f);

		// enable transparancy
//		glEnable(GL_BLEND);

		// disable depth testing
		glDisable(GL_DEPTH_TEST);

		// enable texture mapping
		//glBindTexture(GL_TEXTURE_2D, (*itr)->getGLTexture());
		//glEnable(GL_TEXTURE_2D);
		glColor3f(1.0f, 1.0f, 1.0f);

		// begin with title bar
		glBegin(GL_QUADS);

		// draw quad
		glTexCoord2i(0, 0);
		glVertex3f(0.0f, 8.0f, 0.0f);
		glTexCoord2i(1, 0);
		glVertex3f(width, 8.0f, 0.0f);
		glTexCoord2i(1, 1);
		glVertex3f(width, 0.0f, 0.0f);
		glTexCoord2i(0, 1);
		glVertex3f(0.0f, 0.0f, 0.0f);

		glEnd();

        // now do the rest of the window
		glBegin(GL_LINE_LOOP);

		// draw outlines
		glTexCoord2i(0, 0);
		glVertex3f(0.0f, -height, 0.0f);
		glTexCoord2i(1, 0);
		glVertex3f(width, -height, 0.0f);
		glTexCoord2i(1, 1);
		glVertex3f(width, 0.0f, 0.0f);
		glTexCoord2i(0, 1);
		glVertex3f(0.0f, 0.0f, 0.0f);

		glEnd();

		// finish texture mapping
		//glDisable(GL_TEXTURE_2D);
//		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
	}

	void Window::processKey(SDLKey key)
	{

	}

	void Window::processMouse(MouseButton *button)
	{

	}
}
