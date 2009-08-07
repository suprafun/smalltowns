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
 *	Date of file creation: 09-05-28
 *
 *	$Id$
 *
 ********************************************/

#include "opengl.h"
#include "texture.h"

#include "../utilities/types.h"

#include <SDL.h>
#include <SDL_opengl.h>
#include <agar/core.h>
#include <agar/gui.h>

namespace ST
{
	OpenGLGraphics::OpenGLGraphics() : GraphicsEngine()
	{
		mOpenGL = 1;
	}

	OpenGLGraphics::~OpenGLGraphics()
	{
	}

	bool OpenGLGraphics::init()
	{
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

		// get bpp of desktop
		const SDL_VideoInfo* video = SDL_GetVideoInfo();
		int bpp = video->vfmt->BitsPerPixel;

		mScreen = SDL_SetVideoMode(mWidth, mHeight, bpp, SDL_OPENGL);

		glViewport(0, 0, mWidth, mHeight);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0.0f, mWidth, mHeight, 0.0f, -1.0f, 1.0f);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glClearColor(1.0f, 1.0f , 1.0f, 0.5f);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthFunc(GL_LEQUAL);

		return mScreen ? true : false;
	}

	void OpenGLGraphics::drawRect(Rectangle &rect, bool filled)
	{
        glPushAttrib(GL_POLYGON_BIT|GL_LIGHTING_BIT|GL_DEPTH_BUFFER_BIT);
		glPushMatrix();

		// reset identity matrix
		glLoadIdentity();

		// set position and size from rectangle
		float x = (float)rect.x;
		float y = (float)rect.y;
		float width = (float)rect.width;
		float height = (float)rect.height;

		// move to the correct position
		glTranslatef(x, y, 0.0f);

		// disable depth testing
		glDisable(GL_DEPTH_TEST);

		glColor3f(0.0f, 0.0f, 0.0f);

		if (filled)
			glPolygonMode(GL_FRONT, GL_FILL);
		else
			glPolygonMode(GL_FRONT, GL_LINE);

        // draw the box
		glBegin(GL_QUADS);

		glTexCoord2i(0, 0);
		glVertex3f(0.0f, -height, 0.0f);
		glTexCoord2i(1, 0);
		glVertex3f(width, -height, 0.0f);
		glTexCoord2i(1, 1);
		glVertex3f(width, 0.0f, 0.0f);
		glTexCoord2i(0, 1);
		glVertex3f(0.0f, 0.0f, 0.0f);

		glEnd();

		glPopMatrix();
		glPopAttrib();
	}

	void OpenGLGraphics::drawTexturedRect(Rectangle &rect, Texture *texture)
	{
	    if (!texture)
            return;

		// reset identity matrix
		glLoadIdentity();

		glPushAttrib(GL_POLYGON_BIT|GL_LIGHTING_BIT|GL_ENABLE_BIT|GL_DEPTH_BUFFER_BIT|GL_TEXTURE_BIT);
		glPushMatrix();

		// set position and size to local variables
		float x = (float)rect.x;
		float y = (float)rect.y;
		float width = (float)rect.width;
		float height = (float)rect.height;

		float data[12];
		data[0] = 0.0f; data[1] = -height; data[2] = 0.0f;
		data[3] = width; data[4] = -height; data[5] = 0.0f;
		data[6] = width; data[7] = 0.0f; data[8] = 0.0f;
		data[9] = 0.0f; data[10] = 0.0f; data[11] = 0.0f;

		int coords[8];
		coords[0] = 0; coords[1] = 0;
		coords[2] = 1; coords[3] = 0;
		coords[4] = 1; coords[5] = 1;
		coords[6] = 0; coords[7] = 1;

		unsigned int indices[4];
		indices[0] = 0; indices[1] = 1; indices[2] = 2; indices[3] = 3;

		// move to the correct position
		glTranslatef(x, y, 0.0f);

		// enable transparancy
		glEnable(GL_BLEND);

		// disable depth testing
		glDisable(GL_DEPTH_TEST);

		// enable texture mapping
		GLuint tex = texture->getGLTexture();
		glBindTexture(GL_TEXTURE_2D, tex);
		glEnable(GL_TEXTURE_2D);
		glPolygonMode(GL_FRONT, GL_FILL);
		glColor3f(1.0f, 1.0f, 1.0f);

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, data);
		glTexCoordPointer(2, GL_INT, 0, coords);

		glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, indices);

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);

		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);

		glPopMatrix();
		glPopAttrib();
	}

	void OpenGLGraphics::setupScene()
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}

	void OpenGLGraphics::endScene()
	{

	}

	SDL_Surface* OpenGLGraphics::createSurface(unsigned int texture, int width, int height)
	{
		SDL_Surface *surface = NULL;
		glBindTexture(GL_TEXTURE_2D, texture);
		surface = AG_SurfaceRGBA(width, height, 32, 0,
#if AG_BYTEORDER == AG_BIG_ENDIAN
                                0xff000000,
                                0x00ff0000,
                                0x0000ff00,
                                0x000000ff
#else
                                0x000000ff,
                                0x0000ff00,
                                0x00ff0000,
                                0xff000000
#endif
                                );
		if (surface)
		{
			glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
		}
		glBindTexture(GL_TEXTURE_2D, 0);

		return surface;
	}
}
