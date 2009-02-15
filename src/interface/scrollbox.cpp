/*********************************************
 *
 *	Author: David Athay
 *
 *	License: New BSD License
 *
 *	Copyright (c) 2009, The Small Towns Dev Team
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
 *	Date of file creation: 09-02-13
 *
 *	$Id$
 *
 ********************************************/

#include "scrollbox.h"

#include "../input.h"

#include "../graphics/graphics.h"
#include "../graphics/node.h"

namespace ST
{
	ScrollBox::ScrollBox(const std::string &name) : Window(name)
	{
		index = 0;

		graphicsEngine->loadTexture("scrollup.png");
		graphicsEngine->loadTexture("scrolldown.png");
		graphicsEngine->loadTexture("scrollbar.png");

		mUp = graphicsEngine->getTexture("scrollup.png");
		mDown = graphicsEngine->getTexture("scrolldown.png");
		mBar = graphicsEngine->getTexture("scrollbar.png");
	}

	void ScrollBox::drawWindow()
	{
		Rectangle rect;

		// set position and size to local variables
		rect.x = getPosition().x;
		rect.y = getPosition().y;
		rect.width = getWidth();
		rect.height = getHeight();

		if (mBackground)
		{
			// draw textured filled rectangle
			graphicsEngine->drawTexturedRect(rect, mBackground->getGLTexture());
		}
		
		rect.x = getPosition().x + getWidth() - 8;
		rect.y = getPosition().y;
		rect.width = 8;
		rect.height = getHeight();

		graphicsEngine->drawRect(rect, true);
		
		rect.x = getPosition().x + getWidth() - 8;
		rect.y = getPosition().y + getHeight();
		rect.width = 8;
		rect.height = 8;

		if (mUp)
			graphicsEngine->drawTexturedRect(rect, mUp->getGLTexture());

		rect.x = getPosition().x + getWidth() - 8;
		rect.y = getPosition().y;
		rect.width = 8;
		rect.height = 8;

		if (mDown)
			graphicsEngine->drawTexturedRect(rect, mDown->getGLTexture());

		rect.x = getPosition().x + getWidth() - 8;
		rect.y = getPosition().y + index;
		rect.width = 8;
		rect.height = 8;

		if (mBar)
			graphicsEngine->drawTexturedRect(rect, mBar->getGLTexture());
	}

	void ScrollBox::addChild(Window *window)
	{
		// set the child
		mChild = window;
	}

	void ScrollBox::processMouse(MouseButton *button)
	{
		if (button->x >= (getPosition().x + getWidth() - 4))
		{
			index = button->y - getPosition().y;
			scrollTo(index);
		}
		else
		{
			mChild->processMouse(button);
		}
	}

	void ScrollBox::scrollUp()
	{
		Rectangle rect = mChild->getClipArea();
		if (rect.y + 5 <= mChild->getPosition().y)
			rect.y += 5;
		mChild->setClipArea(rect);
	}

	void ScrollBox::scrollDown()
	{
		Rectangle rect = mChild->getClipArea();
		if (rect.y - 5 >= mChild->getPosition().y - mChild->getHeight())
			rect.y -= 5;
		mChild->setClipArea(rect);
	}

	void ScrollBox::scrollTo(int index)
	{
		Rectangle rect = mChild->getClipArea();
		rect.y = index;
		mChild->setClipArea(rect);
	}
}
