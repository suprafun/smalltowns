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

#include "../map.h"
#include "../resourcemanager.h"

#include "../interface/interfacemanager.h"

#include "../resources/bodypart.h"

#include "../utilities/log.h"
#include "../utilities/math.h"
#include "../utilities/types.h"

#include <SDL.h>
#include <SDL_image.h>
#include <sstream>
#include <cassert>

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
        // nodes should have been deleted by their owners
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

	void GraphicsEngine::addNode(Node *node)
	{
	    assert(node);
	    mNodes.push_back(node);
	}

	void GraphicsEngine::removeNode(Node *node)
	{
	    mNodes.remove(node);
	}

	void GraphicsEngine::setCamera(Camera *cam)
	{
		mCamera = cam;
	}

    Camera* GraphicsEngine::getCamera() const
    {
        return mCamera;
    }

	void GraphicsEngine::renderFrame()
	{
		AG_LockVFS(agView);
	    AG_BeginRendering();

        setupScene();

		// Display the nodes on screen (if theres a camera to view them)
		if (mCamera)
			outputNodes();

        interfaceManager->drawWindows();

		endScene();

        AG_EndRendering();
		AG_UnlockVFS(agView);
	}

	void GraphicsEngine::outputNodes()
	{
		// create iterators for looping
		NodeItr itr = mNodes.begin(), itr_end = mNodes.end();

        Point pt = mCamera->getPosition();

		// keep looping until reached the end of the list
		while (itr != itr_end)
		{
		    Node *node = (*itr);
			// dont draw if not on screen
/*			if (!checkInside(node->getPosition(), mCamera->getViewBounds()))
			{
				continue;
			}
*/
			// dont draw if not visible
			if (!node->getVisible())
			{
				continue;
			}

			Rectangle rect = node->getBounds();
			rect.x -= pt.x;
			rect.y -= pt.y;

			drawTexturedRect(rect, node->getTexture());

			if (node->showName())
			{
			    interfaceManager->drawName(node->getName(), node->getPosition());
			}

			++itr;
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

    Texture* GraphicsEngine::createAvatar(unsigned int id, int bodyId, int hairId)
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

        Texture *hairTex = NULL;
        Texture *bodyTex = NULL;
        Texture *chestTex = NULL;
        Texture *legsTex = NULL;
        Texture *feetTex = NULL;

        // all the body parts that make up the avatar
        BodyPart *hair = resourceManager->getBodyPart(hairId);
        BodyPart *body = resourceManager->getBodyPart(bodyId);
        BodyPart *chest = NULL;
        BodyPart *legs = NULL;
        BodyPart *feet = NULL;

        // load all the textures
        if (hair)
            hairTex = hair->getTexture();
        if (body)
            bodyTex = body->getTexture();
        if (chest)
            chestTex = chest->getTexture();
        if (legs)
            legsTex = legs->getTexture();
        if (feet)
            feetTex = feet->getTexture();

        std::stringstream str;
        str << "Character" << id;

		Texture *tex = new Texture(str.str(), resourceManager->getBodyWidth(),
			resourceManager->getBodyHeight());

        // write all the textures to the surface
        // start with the body as the base
        if (mOpenGL)
		{
		    // create surface to render to
            SDL_Surface *surface = SDL_CreateRGBSurface(SDL_SWSURFACE,
                64, 128,
                mScreen->format->BitsPerPixel, rmask, gmask, bmask, amask);

            if (bodyTex)
            {
                SDL_Surface *s = createSurface(bodyTex->getGLTexture(), 64, 128);
                SDL_SetAlpha(s, 0, 255);
                SDL_BlitSurface(s, NULL, surface, NULL);
            }
            if (hairTex)
            {
                SDL_Surface *s = createSurface(hairTex->getGLTexture(), 64, 128);
                SDL_SetAlpha(s, SDL_SRCALPHA | SDL_RLEACCEL, 0);
                SDL_BlitSurface(s, NULL, surface, NULL);
            }
            if (chestTex)
            {
                SDL_Surface *s = createSurface(chestTex->getGLTexture(), 64, 128);
                SDL_SetAlpha(s, SDL_SRCALPHA | SDL_RLEACCEL, 0);
                SDL_BlitSurface(s, NULL, surface, NULL);
            }
            if (legsTex)
            {
                SDL_Surface *s = createSurface(legsTex->getGLTexture(), 64, 128);
                SDL_SetAlpha(s, SDL_SRCALPHA | SDL_RLEACCEL, 0);
                SDL_BlitSurface(s, NULL, surface, NULL);
            }
            if (feetTex)
            {
                SDL_Surface *s = createSurface(feetTex->getGLTexture(), 64, 128);
                SDL_SetAlpha(s, SDL_SRCALPHA | SDL_RLEACCEL, 0);
                SDL_BlitSurface(s, NULL, surface, NULL);
            }

            tex->setSize(64, 128);
            tex->setPixels(surface);
			SDL_FreeSurface(surface);
		}
        else
        {
            // create surface to render to
            SDL_Surface *surface = SDL_CreateRGBSurface(SDL_SWSURFACE,
                resourceManager->getBodyWidth(), resourceManager->getBodyHeight(),
                mScreen->format->BitsPerPixel, rmask, gmask, bmask, amask);

            if (bodyTex)
            {
                SDL_SetAlpha(bodyTex->getSDLSurface(), 0, 255);
                SDL_BlitSurface(bodyTex->getSDLSurface(), NULL, surface, NULL);
            }
            if (hairTex)
            {
                SDL_SetAlpha(hairTex->getSDLSurface(), SDL_SRCALPHA | SDL_RLEACCEL, 0);
                SDL_BlitSurface(hairTex->getSDLSurface(), NULL, surface, NULL);
            }
            if (chestTex)
            {
                SDL_SetAlpha(chestTex->getSDLSurface(), SDL_SRCALPHA | SDL_RLEACCEL, 0);
                SDL_BlitSurface(chestTex->getSDLSurface(), NULL, surface, NULL);
            }
            if (legsTex)
            {
                SDL_SetAlpha(legsTex->getSDLSurface(), SDL_SRCALPHA | SDL_RLEACCEL, 0);
                SDL_BlitSurface(legsTex->getSDLSurface(), NULL, surface, NULL);
            }
            if (feetTex)
            {
                SDL_SetAlpha(feetTex->getSDLSurface(), SDL_SRCALPHA | SDL_RLEACCEL, 0);
                SDL_BlitSurface(feetTex->getSDLSurface(), NULL, surface, NULL);
            }

            tex->setImage(surface);
        }

        mTextures.insert(std::pair<std::string, Texture*>(tex->getName(), tex));
        return tex;
    }

    int GraphicsEngine::convertToXTile(int x)
    {
        int tile = mCamera->getPosition().x;
        tile += x;
        tile /= mapEngine->getTileWidth();
        return tile;
    }

    int GraphicsEngine::convertToYTile(int y)
    {
        int tile = mCamera->getPosition().y;
        tile += y;
        tile /= mapEngine->getTileHeight();
        return tile;
    }

	Node* GraphicsEngine::getNode(int x, int y)
    {
        Point pt;
        pt.x = x + mCamera->getPosition().x;
        pt.y = y + mCamera->getPosition().y;
        Rectangle rect;

        NodeReverseItr itr = mNodes.rbegin(), itr_end = mNodes.rend();
        while (itr != itr_end)
        {
            rect.x = (*itr)->getPosition().x;
            rect.y = (*itr)->getPosition().y;
            rect.width = (*itr)->getWidth();
            rect.height = (*itr)->getHeight();
            if (checkInside(pt, rect))
            {
                return *itr;
            }

            ++itr;
        }

        return NULL;
    }
}
