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

#include "../input.h"

#include "../graphics/graphics.h"
#include "../graphics/texture.h"

#include "../utilities/stringutils.h"

#include <sstream>

namespace ST
{
	TextBox::TextBox(const std::string &name) : Window(name)
	{
	    mIndex = 0;
        mRows = 1;
		mTextSize = 12;

		graphicsEngine->loadTexture("scrollbutton.png");
		graphicsEngine->loadTexture("scrollline.png");

		mScrollButton = graphicsEngine->getTexture("scrollbutton.png");
		mScrollBar = graphicsEngine->getTexture("scrollline.png");
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

	void TextBox::addRow(const std::string &text)
	{
	    int maxSize = getWidth();
		int totalTextWidth = (int)graphicsEngine->getFontWidth(text);

	    if (totalTextWidth > maxSize)
	    {
	        int sizePerLetter = totalTextWidth / text.size();
	        int maxLetters = maxSize / (sizePerLetter + 1);
	        int sofar = 0;
	        while (sofar < text.size())
	        {
	            if (maxLetters + sofar > text.size())
	            {
	                maxLetters = text.size() - sofar;
	            }
                mTextHistory.push_back(text.substr(sofar, maxLetters));
                sofar += maxLetters;
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

        // loop through the history
        // start with latest first (subtract 1, since counting starts from 0)
        // make sure i doesnt go under the starting row
        int rowNumber = 0;
		int startRow = 0;
		int lastRow = 0;
        Point pos;

		if (mTextHistory.size() < 11)
		{
			lastRow = mTextHistory.size() - 1;
		}
		else
		{
			lastRow = mTextHistory.size() - mIndex - 1;
		}

		startRow = lastRow - 11;

		if (startRow < 0)
		{
			startRow = 0;
		}

        for (int i = lastRow; i >= startRow; --i)
        {
            if (rowNumber < mRows)
            {
                // move to the right a little
                pos.x = rect.x + 5;
                // calculate how low to start writing
                // this should be maxRows - currentRow + 1 * the font height
                pos.y = rect.y - ((11 - rowNumber + 1) * (int)graphicsEngine->getFontHeight());
                if (pos.x > rect.x && pos.y < rect.y &&
                    pos.x < rect.x + rect.width && pos.y > rect.y - rect.height)
                graphicsEngine->drawText(pos, mTextHistory[i], mTextSize);
            }
            ++rowNumber;
        }

        // draw thin line
        rect.x = getPosition().x + getWidth() - 12;
        rect.y = getPosition().y - 4;
        rect.width = 1;
        rect.height = getHeight() - 8;
		graphicsEngine->drawTexturedRect(rect, mScrollBar->getGLTexture());

        // if more rows than max rows
        if (mTextHistory.size() > 11)
        {
			//draw scroll button
            Rectangle scrollRect;
            scrollRect.x = getPosition().x + getWidth() - 16;
            scrollRect.y = getPosition().y - getHeight() + 8 + (mIndex * 5);
            if (scrollRect.y >= getPosition().y - 8)
                scrollRect.y = getPosition().y - 8;
            scrollRect.width = 8;
            scrollRect.height = 8;
            graphicsEngine->drawTexturedRect(scrollRect, mScrollButton->getGLTexture());
        }
	}

	void TextBox::scrollTo(int index)
	{
	    if (index < 0)
            mIndex = 0;
        else if (index > mTextHistory.size() - 1)
            mIndex = mTextHistory.size() - 1;
        else
            mIndex = index;
	}

	void TextBox::processKey(SDL_keysym key)
	{

	}

	void TextBox::processMouse(MouseButton *button)
	{
	    if (button->state == SDL_PRESSED)
	    {
            if (button->x >= (getPosition().x + getWidth() - 16))
            {
                int index = button->y - (getPosition().y - getHeight());
                scrollTo(index / 5);
            }
	    }
	}
}

