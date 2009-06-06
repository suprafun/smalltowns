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
 *	Date of file creation: 09-02-03
 *
 *	$Id$
 *
 ********************************************/

#include "button.h"

#include "../input.h"

#include "../graphics/graphics.h"
#include "../graphics/node.h"

namespace ST
{
	Button::Button(const std::string &name) : Window(name)
	{
		graphicsEngine->loadTexture("background.png");
	    mPressed = false;
		mTextSize = 12;
	}

    Button::~Button()
    {
    }

    void Button::setText(const std::string &text)
	{
	    mText = text;
	}

	void Button::setFontSize(int size)
	{
	    mTextSize = size;
	}

	void Button::drawWindow()
	{
	    Rectangle rect;

		// set position and size to local variables
		rect.x = getPosition().x;
		rect.y = getPosition().y;
		rect.width = getWidth();
		rect.height = getHeight();

		if (mBackground)
		{
			graphicsEngine->drawTexturedRect(rect, mBackground->getGLTexture());
		}
		else
		{
			graphicsEngine->drawRect(rect, false);
		}

		Point pos;
		pos.x = rect.x + 10;
		pos.y = rect.y - 18;
		graphicsEngine->drawText(pos, mText, mTextSize);
	}

	bool Button::clicked()
	{
	    return mPressed;
	}

	void Button::addBackground()
	{
		setBackground("background.png");
	}

	void Button::processMouse(MouseButton *button)
	{
        mPressed = true;
	}
}
