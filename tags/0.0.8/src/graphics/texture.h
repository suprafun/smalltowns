/*********************************************
 *
 *	Author: David Athay
 *
 *	License: New BSD License
 *
 *	Copyright (c) 2008, CT Games
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
 *	Date of file creation: 08-09-21
 *
 *	$Id$
 *
 ********************************************/

/**
 * The Texture class is used to store a texture's data
 */

#ifndef ST_TEXTURE_HEADER
#define ST_TEXTURE_HEADER

#include <SDL_opengl.h>
#include <string>

struct SDL_Surface;

namespace ST
{
	class Texture
	{
	public:
		/**
		 * Constructor
		 */
		Texture(std::string name);
		Texture(std::string name, int width, int height);

		/**
		 * Destructor
		 */
		~Texture();

		/**
		 * Set Pixels
		 * Puts a SDL Surface's pixels into a GL Texture
		 */
		void setPixels(SDL_Surface *surface);

		/**
		 * Set Image
		 * Stores the SDL Surface
		 */
		void setImage(SDL_Surface *surface);

		/**
		 * Set GL texture
		 * When we already have a created texture
		 */
        void setGLTexture(unsigned int texture);

        /**
         * Set Size
         * Sets the size of the texture
         */
        void setSize(unsigned int w, unsigned int h);

		/**
		 * Increase instance count
		 */
		void increaseCount() { ++mInstances; }

		/**
		 * Get Name
		 * @return Returns the name of the texture
		 */
		std::string getName() const;

		/**
		 * Get Height
		 * @return Returns the height of the texture
		 */
		int getHeight() const;

		/**
		 * Get Width
		 * @return Returns the width of the texture
		 */
		int getWidth() const;

		/**
		 * Get GL Texture
		 * @return Returns the GL Texture
		 */
		GLuint getGLTexture();

		/**
		 * Get SDL Surface
		 * @return Returns the SDL Surface
		 */
		SDL_Surface* getSDLSurface();

		/**
		 * Remove
		 * This will eventually delete the texture
		 * when the number of instances equals 0
		 */
		void remove();

	private:
		std::string mName;
		int mInstances;
		int mWidth;
		int mHeight;
		GLuint mGLTexture;
		SDL_Surface *mSurface;
	};
}

#endif
