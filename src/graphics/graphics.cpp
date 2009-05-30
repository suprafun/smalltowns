/*********************************************
 *
 *	Author: David Athay
 *
 *	License: New BSD License
 *
 *	Copyright (c) 2008, The Small Towns Dev Team
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
 *	Date of file creation: 08-09-20
 *
 *	$Id$
 *
 ********************************************/

#include "graphics.h"
#include "animation.h"
#include "camera.h"
#include "node.h"
#include "texture.h"

#include "../interface/interfacemanager.h"

#include "../utilities/log.h"
#include "../utilities/types.h"

#include <SDL.h>
#include <SDL_image.h>
#include <sstream>

namespace ST
{
	GraphicsEngine::GraphicsEngine()
	{
		// Initialise SDL
		if (SDL_Init(SDL_INIT_VIDEO) != 0)
		{
			logger->logError("GraphicsEngine::GraphicsEngine"
				"failed to initialise SDL");
		}

		// needed for input
		SDL_EnableUNICODE(true);
		SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

		// Set the window title
		SDL_WM_SetCaption("Towns Life", NULL);

		mWidth = 1024;
		mHeight = 768;

		mCamera = NULL;
	}

	GraphicsEngine::~GraphicsEngine()
	{
		// clean up nodes
		std::list<Node*>::iterator itr = mNodes.begin();
		std::list<Node*>::const_iterator itr_end = mNodes.end();
		while (itr != itr_end)
		{
			delete (*itr);
			++itr;
		}
		mNodes.clear();

		if (mCamera)
			delete mCamera;

		// Do SDL cleanup
		SDL_Quit();
	}

	Node* GraphicsEngine::createNode(std::string name, std::string texture, Point *point)
	{
		Node *node = new Node(name, graphicsEngine->getTexture(texture));
		node->moveNode(point);
		mNodes.push_back(node);
		return node;
	}

	void GraphicsEngine::setCamera(Camera *cam)
	{
		mCamera = cam;
	}

	void GraphicsEngine::renderFrame()
	{
		AG_LockVFS(agView);
	    AG_BeginRendering();

        setupScene();

		// Display the nodes on screen (if theres a camera to view them)
		if (mCamera)
			outputNodes();

		endScene();

        interfaceManager->drawWindows();

        AG_EndRendering();
		AG_UnlockVFS(agView);
	}

	void GraphicsEngine::outputNodes()
	{
		// create an iterator for looping
		std::list<Node*>::iterator itr = mNodes.begin();
		// create an iterator to test for end of list
		std::list<Node*>::iterator itr_end = mNodes.end();

		// camera position, and view size
		Point pt = mCamera->getPosition();
		const int w = mCamera->getViewWidth();
		const int h = mCamera->getViewHeight();

		// keep looping until reached the end of the list
		for (; itr != itr_end; ++itr)
		{
			// dont draw if not on screen
			if ((*itr)->getPosition().x + (*itr)->getWidth() < pt.x ||
				(*itr)->getPosition().x > pt.x + w ||
				(*itr)->getPosition().y + (*itr)->getHeight() < pt.y ||
				(*itr)->getPosition().y > pt.y + h)
			{
				continue;
			}

			// dont draw if not visible
			if (!(*itr)->getVisible())
			{
				continue;
			}

			Rectangle rect;

			// set position and size to local variables
			rect.x = (*itr)->getPosition().x - pt.x;
			rect.y = (*itr)->getPosition().y - pt.y;
			rect.width = (*itr)->getWidth();
			rect.height = (*itr)->getHeight();

			drawTexturedRect(rect, (*itr)->getTexture());
		}
	}

	Texture* GraphicsEngine::loadTexture(const std::string &name)
	{
		std::map<std::string, Texture*>::iterator itr;
		itr = mTextures.find(name);
		if (itr != mTextures.end())
		{
			itr->second->increaseCount();
			return itr->second;
		}

		SDL_Surface* s = NULL;

		// Load in the texture
		s = IMG_Load(name.c_str());
		Texture *tex = NULL;
		if (s)
		{
			tex = createTexture(s, name,0, 0, s->w, s->h);
			SDL_FreeSurface(s);
			s = NULL;
		}
		else
		{
		    logger->logError("Image not found: " + name);
		}

		return tex;
	}

	bool GraphicsEngine::loadTextureSet(const std::string &name, int w, int h)
	{
		// Load in the texture set
		SDL_Surface *s = IMG_Load(name.c_str());

		if (s)
		{
			if (w && h)
			{
				// keep creating textures
				int imgX = s->w / w;
				int imgY = s->h / h;
				int id = 1;
				for (int i = 0; i < imgY; ++i)
				{
					for (int j = 0; j < imgX; ++j)
					{
						std::stringstream str;
						str << name << id;
						createTexture(s, str.str(), j*w, i*h, w, h);
						++id;
					}
				}
				SDL_FreeSurface(s);
				s = NULL;
				return true;
			}
			else
			{
				SDL_FreeSurface(s);
				s = NULL;
				logger->logError("Bad dimension for image: " + name);
			}
		}
		else
		{
		    logger->logError("Image not found: " + name);
		}

		return false;
	}

	SDL_Surface *GraphicsEngine::loadSDLTexture(const std::string &name)
	{
		SDL_Surface *s = IMG_Load(name.c_str());

		if (!s)
			logger->logError("File not found");

		return s;
	}

	Texture* GraphicsEngine::createTexture(SDL_Surface *surface, std::string name,
									   unsigned int x, unsigned int y,
									   unsigned int width, unsigned int height)
	{
		// Set the byte order of RGBA
		Uint32 rmask, gmask, bmask, amask;
		#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		rmask = 0xff000000;
		gmask = 0x00ff0000;
		bmask = 0x0000ff00;
		amask = 0x000000ff;
		#else
		rmask = 0x000000ff;
		gmask = 0x0000ff00;
		bmask = 0x00ff0000;
		amask = 0xff000000;
		#endif

		if (!surface)
		{
			logger->logError("Invalid surface");
			return NULL;
		}

		// Put the frame into new surface
		SDL_Surface *tex = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height,
			surface->format->BitsPerPixel, rmask, gmask, bmask, amask);

		if (!tex)
		{
			// error
			logger->logError("Could not create new surface");
			SDL_FreeSurface(surface);
			surface = NULL;
			return NULL;
		}

		// Create area to copy over
		SDL_Rect rect;
		rect.x = x;
		rect.y = y;
		rect.w = width;
		rect.h = height;

		// Copy frame over
		Uint8 alpha = surface->format->alpha;
		SDL_SetAlpha(surface, 0, 0);
		SDL_BlitSurface(surface, &rect, tex, NULL);
		SDL_SetAlpha(tex, SDL_SRCALPHA, alpha);

		// Create texture from frame
		Texture *texture = new Texture(name, width, height);
		if (mOpenGL)
		{
			texture->setPixels(tex);
			SDL_FreeSurface(tex);
		}
		else
		{
			texture->setImage(tex);
		}

		mTextures.insert(std::pair<std::string, Texture*>(texture->getName(),
			texture));

		return texture;
	}

	unsigned int GraphicsEngine::getPixel(SDL_Surface *s, int x, int y) const
	{
		// Lock the surface so we can get pixels from it
		SDL_LockSurface(s);

		// Get the number of bytes per pixel (different formats, ie 16 bit,
		// 24 bit, 32 bit)
		int bpp = s->format->BytesPerPixel;

		// Retrieve the pixel at x and y
		Uint32 *pixel = (Uint32*)((Uint8*)s->pixels + y * s->pitch + x * bpp);

		// if 24 bit (3 bytes - RGB), swap bytes
		switch(bpp)
		{
		case 3:
			if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
				*pixel = pixel[0] << 16 | pixel[1] << 8 | pixel[2];
			else
				*pixel = pixel[0] | pixel[1] << 8 | pixel[2] << 16;
			break;

		case 4:
			break;

		default:
			break;
		}

		// Unlock the surface again
		SDL_UnlockSurface(s);

		return *pixel;
	}

	Texture* GraphicsEngine::getTexture(const std::string &name)
	{
		std::map<std::string, Texture*>::iterator itr;
		itr = mTextures.find(name);

		if (itr != mTextures.end())
		{
			return itr->second;
		}

		return NULL;
	}

    int GraphicsEngine::getScreenWidth() const
    {
        return mWidth;
    }

    int GraphicsEngine::getScreenHeight() const
    {
        return mHeight;
    }
}
