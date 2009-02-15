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

#include "textbox.h"

#include "../graphics/graphics.h"

#include "../utilities/stringutils.h"

#include <sstream>

namespace ST
{
	TextBox::TextBox(const std::string &name) : Window(name)
	{
        mRows = 1;
        mEditable = false;
		mTextSize = 12;
	}

    TextBox::~TextBox()
    {
    }

	void TextBox::setFontSize(int size)
	{
	    mTextSize = size;
	}

	void TextBox::setRows(int rows)
	{
	    mRows = rows;
	}

	void TextBox::setEditable(bool editable)
	{
	    mEditable = editable;
	}

	void TextBox::addRow(const std::string &text)
	{
	    int maxSize = getWidth();
		int textlength = (int)graphicsEngine->getFontWidth(text);
		int length = maxSize / (int)graphicsEngine->getFontWidth("m");
	    if (textlength > maxSize)
	    {
	        int sofar = 0;
	        while (sofar < text.size())
	        {
	            if (length + sofar > text.size())
	            {
	                length = text.size() - sofar;
	            }
                mTextHistory.push_back(text.substr(sofar, length));
                sofar += length;
	        }
	        return;
	    }
	    mTextHistory.push_back(text);
	}

	void TextBox::drawWindow()
	{
		if (!mVisible)
			return;

		Rectangle rect;
		rect.x = getPosition().x;
		rect.y = getPosition().y;
		rect.width = getWidth();
		rect.height = getHeight();
	    graphicsEngine->drawRect(rect, false);

        if (mEditable)
        {
			Point pos;
			pos.x = rect.x;
			pos.y = rect.y;
            graphicsEngine->drawText(pos, mText, mTextSize);
        }
        else
        {
            // loop through the history
            // start with latest first (subtract 1, since counting starts from 0)
            // make sure i doesnt go under 0
            int numberRows = 0;
			Point pos;
            for (int i = mTextHistory.size() - 1; i >= 0; --i)
            {
                if (numberRows < mRows)
                {
					pos.x = rect.x + 5;
					pos.y = rect.y - ((mRows - numberRows + 1) * (int)graphicsEngine->getFontHeight());
					if (pos.x > mClipArea.x && pos.y > mClipArea.y &&
						pos.x < mClipArea.width && pos.y < mClipArea.height)
                    graphicsEngine->drawText(pos, mTextHistory[i], mTextSize);
                }
                ++numberRows;
            }
        }
	}

	void TextBox::processKey(SDL_keysym key)
	{
	    if (!mEditable)
            return;
	    if (key.sym == SDLK_RETURN || key.sym == SDLK_TAB)
            return;
	    if (utils::isCharacter(key.unicode))
        {
            char c = key.unicode;
            std::stringstream str;
            str << c;
            mText.append(str.str());
        }
	}
}

