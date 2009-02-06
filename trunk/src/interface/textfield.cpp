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
 *	Date of file creation: 09-01-22
 *
 *	$Id$
 *
 ********************************************/

#include "textfield.h"

#include "../graphics/graphics.h"
#include "../graphics/node.h"

#include "../utilities/stringutils.h"

namespace ST
{
	TextField::TextField(const std::string &name) : Window(name)
	{
		graphicsEngine->loadTexture("textfield.png");
		mTextSize = 12;
	}

    TextField::~TextField()
    {
        
    }

    void TextField::setText(const std::string &text)
	{
	    mText = text;
	}

	std::string TextField::getText()
	{
	    return mText;
	}

	void TextField::setFontSize(int size)
	{
	    mTextSize = size;
	}

	void TextField::drawWindow()
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
		else
		{
			graphicsEngine->drawRect(rect, false);
		}

        int length = 0;
		if (graphicsEngine->getFontWidth(mText) > rect.width)
        {
			length = mText.size() - rect.width / (int)graphicsEngine->getFontWidth("m");
			if (length < 0)
				length = 0;
        }

		Point pos;
		pos.x = rect.x + 5;
		pos.y = rect.y - 18;
		
		graphicsEngine->drawText(pos, mText.substr(length), mTextSize);

		if (mFocus)
		{
			pos.y += 18;
			graphicsEngine->drawCarat(pos, mText.substr(length));
		}
	}

	void TextField::addBackground()
	{
		setBackground("textfield.png");
	}

	void TextField::processKey(SDL_keysym key)
	{
	    if (key.sym == SDLK_RETURN || key.sym == SDLK_TAB)
            return;
        else if (key.sym == SDLK_BACKSPACE)
        {
            mText = mText.substr(0, mText.size() - 1);
            return;
        }
        else if (key.sym == SDLK_SPACE)
        {
            mText.append(" ");
            return;
        }

        if (utils::isCharacter(key.unicode))
        {
            char c = key.unicode;
            std::stringstream str;
            str << c;
            mText.append(str.str());
        }
	}
}
