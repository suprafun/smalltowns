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

#include <SDL_opengl.h>
#include <FTGL/ftgl.h>

namespace ST
{
	TextField::TextField(const std::string &name) : Window(name)
	{
        font = new FTGLPixmapFont("st.ttf");
        font->FaceSize(12);
	}

    TextField::~TextField()
    {
        delete font;
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
	    font->FaceSize(size);
	}

	void TextField::drawWindow()
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

		font->Render(mText.c_str(), mText.size(), FTPoint(x + 5.0f, y - 15.0f));

		glEnable(GL_DEPTH_TEST);
	}

	void TextField::processKey(SDLKey key)
	{
	    if (key == SDLK_RETURN || key == SDLK_TAB)
            return;
        if (key == SDLK_BACKSPACE)
        {
            mText = mText.substr(0, mText.size() - 1);
            return;
        }
	    mText += SDL_GetKeyName(key);
	}
}
