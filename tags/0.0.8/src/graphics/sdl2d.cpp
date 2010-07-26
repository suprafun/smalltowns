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
 *	Date of file creation: 09-05-30
 *
 *	$Id$
 *
 ********************************************/

#include "sdl2d.h"
#include "texture.h"

#include "../utilities/log.h"
#include "../utilities/types.h"

#include <SDL.h>
#include <sstream>

namespace ST
{
	SDLGraphics::SDLGraphics() : GraphicsEngine()
	{
		mOpenGL = 0;
	}

	SDLGraphics::~SDLGraphics()
	{
	}

	bool SDLGraphics::init(int fullscreen, int x, int y)
	{
	    // set width and height
	    mWidth = x;
	    mHeight = y;

		// get bpp of desktop
		const SDL_VideoInfo* video = SDL_GetVideoInfo();
		int bpp = video->vfmt->BitsPerPixel;

        // set fullscreen
        int flags = SDL_HWSURFACE|SDL_DOUBLEBUF;
        if (fullscreen)
            flags |= SDL_FULLSCREEN;

		mScreen = SDL_SetVideoMode(mWidth, mHeight, bpp, flags);

        std::stringstream str;
        str << "Using SDL renderer at " << mWidth << "x" << mHeight << "x" << bpp;
        logger->logDebug(str.str());

		return mScreen ? true : false;
	}

	void SDLGraphics::drawRect(Rectangle &rect, bool filled)
	{
	}

	void SDLGraphics::drawTexturedRect(Rectangle &rect, Texture *texture)
	{
	    if (!texture)
            return;
		SDL_Rect dstRect;
		dstRect.x = rect.x;
		dstRect.y = rect.y - rect.height; // draw from bottom
		dstRect.w = 0; // not used
		dstRect.h = 0; // not used
		SDL_Rect srcRect;
		srcRect.x = 0;
		srcRect.y = 0;
		srcRect.w = texture->getWidth();
		srcRect.h = texture->getHeight();
		SDL_SetAlpha(texture->getSDLSurface(), SDL_SRCALPHA | SDL_RLEACCEL, 0);
		SDL_BlitSurface(texture->getSDLSurface(), &srcRect, mScreen, &dstRect);
	}

	void SDLGraphics::setupScene()
	{
	    SDL_FillRect(mScreen, NULL, 0);
	}

	void SDLGraphics::endScene()
	{
		SDL_Flip(mScreen);
	}

	SDL_Surface* SDLGraphics::createSurface(unsigned int texture, int width, int height)
	{
		// This is not supported by SDL
		return NULL;
	}
}
