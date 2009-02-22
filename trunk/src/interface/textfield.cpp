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

#include "../utilities/stringutils.h"

namespace ST
{
	TextField::TextField(const std::string &name) : Window(name)
	{
		setBackground("textfield.png");
		mTextSize = 12;
		mCaratPos = 0;
		mPassword = false;
	}

    TextField::~TextField()
    {

    }

    void TextField::setText(const std::string &text)
	{
	    mText = text;
		mCaratPos = mText.size();
	}

	std::string TextField::getText()
	{
	    return mText;
	}

	void TextField::setPassword(bool value)
	{
	    mPassword = value;
	}

	bool TextField::getPassword() const
	{
	    return mPassword;
	}

	void TextField::setFontSize(int size)
	{
	    mTextSize = size;
	}

	void TextField::drawWindow()
	{
		Window::drawWindow();

        int startAtLetter = 0;
		if (graphicsEngine->getFontWidth(mText) > getWidth())
        {
			int sizePerLetter = (int)graphicsEngine->getFontWidth(mText) / mText.size();
			startAtLetter = mText.size() - (getWidth() / sizePerLetter) - 1;
			if (startAtLetter < 0)
				startAtLetter = 0;
        }

		Point pos;
		pos.x = getPosition().x + 5;
		pos.y = getPosition().y - 18;

        if (mPassword && !mText.empty())
        {
            std::string password = mText;
            for (int i = 0; i < mText.size(); ++i)
            {
                password.replace(i, mText.size(), "*");
            }
            graphicsEngine->drawText(pos, password.substr(startAtLetter), mTextSize);
        }
        else
        {
            graphicsEngine->drawText(pos, mText.substr(startAtLetter), mTextSize);
        }

		if (mFocus)
		{
			pos.y = getPosition().y;
			graphicsEngine->drawCarat(pos, mText.substr(startAtLetter, mCaratPos));
		}
	}

	void TextField::processKey(SDL_keysym key)
	{
	    if (key.sym == SDLK_RETURN || key.sym == SDLK_TAB)
            return;
        else if (key.sym == SDLK_BACKSPACE)
        {
			if (mText.empty())
				return;
			if (mCaratPos > 0)
			{
				mText = mText.erase(mCaratPos - 1, 1);
				--mCaratPos;
			}
            return;
        }
		else if (key.sym == SDLK_DELETE)
		{
			if (mText.empty())
				return;
			if (mCaratPos < mText.size())
			{
				mText = mText.erase(mCaratPos, 1);
			}
			return;
		}
        else if (key.sym == SDLK_SPACE)
        {
            mText.append(" ");
            return;
        }
		else if (key.sym == SDLK_LEFT)
		{
			if (mCaratPos > 0)
				--mCaratPos;
		}
		else if (key.sym == SDLK_RIGHT)
		{
			if (mCaratPos < mText.size())
				++mCaratPos;
		}

        if (utils::isCharacter(key.unicode))
        {
            char c = key.unicode;
            std::stringstream str;
            str << c;
            mText.insert(mCaratPos, str.str());
			++mCaratPos;
        }
	}
}
