/*********************************************
 *
 *	Author: David Athay
 *
 *	License: New BSD License
 *
 *	Copyright (c) 2009, CT Games
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
 *	- Neither the name of CT Games nor the names of its contributors
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

/**
 * This expands the graphics class with opengl graphics
 */

#ifndef ST_OPENGL_HEADER
#define ST_OPENGL_HEADER

#include "graphics.h"

#include <SDL_opengl.h>

namespace ST
{
	class Texture;
	struct Rectangle;

	class OpenGLGraphics : public GraphicsEngine
	{
	    GLuint mTile;
	public:
		/**
		 * Constructor
		 * Creates the renderer, should only be created once
		 */
		OpenGLGraphics();

		/**
		 * Destructor
		 * Cleans up class
		 */
		~OpenGLGraphics();

		/**
		 * Initialise the engine
		 */
		bool init(int fullscreen, int x, int y);

		/**
		 * Draw Untextured Rectangle
		 */
		void drawRect(Rectangle &rect, bool filled);

		/**
		 * Draw Textured Rectangle
		 */
		void drawTexturedRect(Rectangle &rect, Texture *texture);

		/**
		 * Setup the scene
		 */
		void setupScene();

		/**
		 * End Scene
		 */
		void endScene();

		/**
		 * Create a SDL_Surface from a GL texture
		 */
		SDL_Surface* createSurface(unsigned int texture, int width, int height);
	};
}

#endif
