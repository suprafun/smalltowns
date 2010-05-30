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
 *	Date of file creation: 08-10-02
 *
 *	$Id$
 *
 ********************************************/

#include "texture.h"

#include <SDL.h>

namespace ST
{
	Texture::Texture(std::string name) : mName(name), mInstances(1), mGLTexture(0), mSurface(0)
	{
		mWidth = 0;
		mHeight = 0;
	}

	Texture::Texture(std::string name, int width, int height)
		: mName(name),
		mInstances(1),
		mWidth(width),
		mHeight(height),
		mGLTexture(0),
		mSurface(0)
	{
	}

	Texture::~Texture()
	{
		if (mSurface)
		{
			SDL_FreeSurface(mSurface);
			mSurface = 0;
		}
		if (mGLTexture)
		{
			glDeleteTextures(1, &mGLTexture);
			mGLTexture = 0;
		}
	}

	void Texture::setPixels(SDL_Surface *surface)
	{
		int mode = GL_RGBA;

		// set mode based on bpp
		if (surface->format->BytesPerPixel == 3)
		{
			mode = GL_RGB;
		}

		// Set the width and height of the texture
		mWidth = surface->w;
		mHeight = surface->h;

		// Generate 1 texture
		glGenTextures(1, &mGLTexture);

		// Bind the texture first
		glBindTexture(GL_TEXTURE_2D, mGLTexture);

		// test
		//glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

		// Put the SDL pixels into the texture
		SDL_LockSurface(surface);
		gluBuild2DMipmaps(GL_TEXTURE_2D, mode, mWidth, mHeight,
                   mode, GL_UNSIGNED_BYTE, surface->pixels );
		SDL_UnlockSurface(surface);

        // Set params for filter to make the image look nice
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	}

	void Texture::setImage(SDL_Surface *surface)
	{
		mSurface = surface;
	}

	void Texture::setGLTexture(unsigned int texture)
	{
	    mGLTexture = texture;
	}

	void Texture::setSize(unsigned int w, unsigned int h)
	{
	    mWidth = w;
	    mHeight = h;
	}

	std::string Texture::getName() const
	{
		return mName;
	}

	int Texture::getWidth() const
	{
		return mWidth;
	}

	int Texture::getHeight() const
	{
		return mHeight;
	}

	GLuint Texture::getGLTexture()
	{
		return mGLTexture;
	}

	SDL_Surface* Texture::getSDLSurface()
	{
		return mSurface;
	}

	void Texture::remove()
	{
		--mInstances;
		if (mInstances <= 0)
		{
			delete this;
		}
	}
}
