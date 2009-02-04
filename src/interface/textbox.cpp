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

#include "../utilities/stringutils.h"

#include <SDL_opengl.h>
#include <FTGL/ftgl.h>

namespace ST
{
	TextBox::TextBox(const std::string &name) : Window(name)
	{
        font = new FTGLPixmapFont("st.ttf");
        font->FaceSize(12);
        mRows = 1;
        mEditable = false;
	}

    TextBox::~TextBox()
    {
        delete font;
    }

	void TextBox::setFontSize(int size)
	{
	    font->FaceSize(size);
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
	    int totallength = text.size() * 8;
	    int length = maxSize / 8;
	    if (totallength > maxSize)
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
	    // reset identity matrix
		glLoadIdentity();

		// set position and size to local variables
		float x = (float)getPosition().x;
		float y = (float)getPosition().y;
		float width = (float)getWidth();
		float height = (float)getHeight();

		// move to the correct position
		glTranslatef(x, y, 0.0f);

		// disable depth testing
		glDisable(GL_DEPTH_TEST);

		glColor3f(1.0f, 1.0f, 1.0f);

		glBegin(GL_LINE_LOOP);

		// draw quad
		glTexCoord2i(0, 0);
		glVertex3f(0.0f, -height, 0.0f);
		glTexCoord2i(1, 0);
		glVertex3f(width, -height, 0.0f);
		glTexCoord2i(1, 1);
		glVertex3f(width, 0.0f, 0.0f);
		glTexCoord2i(0, 1);
		glVertex3f(0.0f, 0.0f, 0.0f);

		glEnd();

        if (mEditable)
        {
            font->Render(mText.c_str(), mText.size(), FTPoint(x + 5.0f, y - 15.0f));
        }
        else
        {
            // loop through the history
            // start with latest first (subtract 1, since counting starts from 0)
            // make sure i doesnt go under 0
            int numberRows = 0;
            for (int i = mTextHistory.size() - 1; i >= 0; --i)
            {
                if (numberRows < mRows)
                {
                    font->Render(mTextHistory[i].c_str(), mTextHistory[i].size(), FTPoint(x + 5.0f, y - (i + 1) * 15.0f));
                }
                ++numberRows;
            }
        }

		glEnable(GL_DEPTH_TEST);
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

