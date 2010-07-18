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

		atexit(SDL_Quit);

		// needed for input
		SDL_EnableUNICODE(true);
		SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

		// Set the window title
		SDL_WM_SetCaption("Towns Life", NULL);

		mCamera = NULL;
		mFrames = 0;
		mAverageTime = 5;
		mAverageFPS = 0;
	}

	GraphicsEngine::~GraphicsEngine()
	{
		if (mCamera)
			delete mCamera;

//		mNodes.clear();

//		SDL_FreeSurface(mScreen);
	}

	Node* GraphicsEngine::createNode(std::string name, std::string texture, Point *point)
	{
        assert(0);
		Node *node = new Node(name, graphicsEngine->getTexture(texture));
		if (point)
            node->moveNode(point);
		mapEngine->getLayer(mapEngine->getLayers() - 1)->addNode(node);
		return node;
	}

	void GraphicsEngine::addNode(Node *node)
	{
	    assert(node);
//		mNodes.push_back(node);
	}

	void GraphicsEngine::removeNode(Node *node)
	{
        assert(node);
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
	    ++mFrames;

        AG_LockVFS(&agDrivers);
        if (agDriverSw)
            AG_BeginRendering(agDriverSw);

        setupScene();

		// Display the nodes on screen (if theres a camera to view them)
		if (mCamera)
        {
            for (unsigned int i = 0; i < mapEngine->getLayers(); ++i)
            {
                if (mapEngine->getLayer(i)->isCollisionLayer())
                    continue;
                outputNodes(i);
            }
        }

        interfaceManager->drawWindows();

		endScene();

        if (agDriverSw)
            AG_EndRendering(agDriverSw);
        AG_UnlockVFS(&agDrivers);
	}

	void GraphicsEngine::outputNodes(int layer)
	{
	    // create iterators for looping
        NodeItr itr = mapEngine->getLayer(layer)->getFrontNode();
        NodeItr itr_end = mapEngine->getLayer(layer)->getEndNode();

        Point pt = mCamera->getPosition();

        // keep looping until reached the end of the list
        while (itr != itr_end)
        {
            Node *node = (*itr);

            // dont draw if not visible
            if (!node->getVisible())
                continue;

            Rectangle rect = node->getBounds();
            rect.x -= pt.x;
            rect.x -= node->getAnchor();
            rect.y -= pt.y;

            drawTexturedRect(rect, node->getTexture());

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

		if (name.find_last_of('.') == std::string::npos)
			return NULL;

		SDL_Surface* s = NULL;

		// Load in the texture
		s = IMG_Load(name.c_str());
		Texture *tex = NULL;
		if (s)
		{
			tex = createTexture(s, name, 0, 0, s->w, s->h);
			SDL_FreeSurface(s);
			s = NULL;
		}
		else
		{
		    logger->logError("Image not found: " + name);
		}

		return tex;
	}

	Texture* GraphicsEngine::loadTexture(const std::string &filename, char *data, int size)
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

		if (size == 0 || !data)
            return NULL;

        // create surface for texture
        SDL_RWops *rw = SDL_RWFromMem (data, size);
        SDL_Surface *s = IMG_Load_RW(rw, 0);
        SDL_FreeRW(rw);

        // create texture
        Texture *tex = createTexture(s, filename, 0, 0, s->w, s->h);
        SDL_FreeSurface(s);
        s = NULL;

        return tex;
	}

    bool GraphicsEngine::loadTextureSet(const std::string &name, int w, int h)
    {
        return loadTextureSet(name, name, w, h);
    }

    bool GraphicsEngine::loadTextureSet(const std::string &name, const std::string &file, int w, int h)
	{
		// Load in the texture set
		SDL_Surface *s = IMG_Load(file.c_str());

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

	bool GraphicsEngine::loadTextureSet(const std::string &name, char *data, int size, int w, int h)
	{
		// Load in the texture set
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

		if (size == 0 || !data)
            return NULL;

        // create surface for texture
        SDL_RWops *rw = SDL_RWFromMem (data, size);
        SDL_Surface *s = IMG_Load_RW(rw, 0);
        SDL_FreeRW(rw);

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
			mScreen->format->BitsPerPixel, rmask, gmask, bmask, amask);

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

		if (!texture)
		{
		    logger->logError("Unable to create texture " + name);
		    return NULL;
		}
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
		if (bpp == 3)
		{
			if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
				*pixel = pixel[0] << 16 | pixel[1] << 8 | pixel[2];
			else
				*pixel = pixel[0] | pixel[1] << 8 | pixel[2] << 16;
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

	Texture* GraphicsEngine::getAnimatedTexture(const std::string &name, int frame)
	{
	    std::stringstream str;
	    str << name << frame;
        std::map<std::string, Texture*>::iterator itr;
		itr = mTextures.find(str.str());

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

    Texture* GraphicsEngine::createAvatar(unsigned int id, const std::map<int, int> &partIds, int dir)
    {
        std::map<int, Texture*> textures;
        std::map<int, int>::const_iterator itr;

        // all the body parts that make up the avatar
        BodyPart *part = NULL;

        itr = partIds.find(PART_BODY);
        if (itr != partIds.end())
        {
            part = resourceManager->getBodyPart(itr->second);
            if (part)
                textures[PART_BODY] = part->getTexture(dir);
        }

        itr = partIds.find(PART_HAIR);
        if (itr != partIds.end())
        {
            part = resourceManager->getBodyPart(itr->second);
            if (part)
                textures[PART_HAIR] = part->getTexture(dir);
        }

        itr = partIds.find(PART_CHEST);
        if (itr != partIds.end())
        {
            part = resourceManager->getBodyPart(itr->second);
            if (part)
                textures[PART_CHEST] = part->getTexture(dir);
        }

        itr = partIds.find(PART_LEGS);
        if (itr != partIds.end())
        {
            part = resourceManager->getBodyPart(itr->second);
            if (part)
                textures[PART_LEGS] = part->getTexture(dir);
        }

        itr = partIds.find(PART_FEET);
        if (itr != partIds.end())
        {
            part = resourceManager->getBodyPart(itr->second);
            if (part)
                textures[PART_FEET] = part->getTexture(dir);
        }

        return createAvatarFrame(id, 0, textures, dir);
    }

    Texture* GraphicsEngine::createAvatarFrame(unsigned int id, unsigned int frame, const std::map<int, Texture*> &textures, int dir)
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

        std::stringstream str;
        str << "Being" << id << "_" << dir << "_" << frame;

        TextureItr itr = mTextures.find(str.str());
        if (itr != mTextures.end())
        {
            delete itr->second;
            mTextures.erase(itr);
        }

        int bodyWidth = resourceManager->getBodyWidth();
        int bodyHeight = resourceManager->getBodyHeight();

		Texture *tex = new Texture(str.str(), bodyWidth, bodyHeight);

        // write all the textures to the surface
        // start with the body as the base
        if (mOpenGL)
		{
		    // create surface to render to
            SDL_Surface *surface = SDL_CreateRGBSurface(SDL_SWSURFACE,
                bodyWidth, bodyHeight,
                mScreen->format->BitsPerPixel, rmask, gmask, bmask, amask);

            if (textures.find(PART_BODY) != textures.end())
            {
                SDL_Surface *s = createSurface(textures.find(PART_BODY)->second->getGLTexture(), bodyWidth, bodyHeight);
                SDL_SetAlpha(s, 0, 255);
                SDL_BlitSurface(s, NULL, surface, NULL);
            }
            if (textures.find(PART_HAIR) != textures.end())
            {
                SDL_Surface *s = createSurface(textures.find(PART_HAIR)->second->getGLTexture(), bodyWidth, bodyHeight);
                SDL_SetAlpha(s, SDL_SRCALPHA | SDL_RLEACCEL, 0);
                SDL_BlitSurface(s, NULL, surface, NULL);
            }
            if (textures.find(PART_LEGS) != textures.end())
            {
                SDL_Surface *s = createSurface(textures.find(PART_LEGS)->second->getGLTexture(), bodyWidth, bodyHeight);
                SDL_SetAlpha(s, SDL_SRCALPHA | SDL_RLEACCEL, 0);
                SDL_BlitSurface(s, NULL, surface, NULL);
            }
            if (textures.find(PART_CHEST) != textures.end())
            {
                SDL_Surface *s = createSurface(textures.find(PART_CHEST)->second->getGLTexture(), bodyWidth, bodyHeight);
                SDL_SetAlpha(s, SDL_SRCALPHA | SDL_RLEACCEL, 0);
                SDL_BlitSurface(s, NULL, surface, NULL);
            }
            if (textures.find(PART_FEET) != textures.end())
            {
                SDL_Surface *s = createSurface(textures.find(PART_FEET)->second->getGLTexture(), bodyWidth, bodyHeight);
                SDL_SetAlpha(s, SDL_SRCALPHA | SDL_RLEACCEL, 0);
                SDL_BlitSurface(s, NULL, surface, NULL);
            }

            tex->setPixels(surface);
			SDL_FreeSurface(surface);
		}
        else
        {
            // create surface to render to
            SDL_Surface *surface = SDL_CreateRGBSurface(SDL_SWSURFACE,
                bodyWidth, bodyHeight,
                mScreen->format->BitsPerPixel, rmask, gmask, bmask, amask);

            if (textures.find(PART_BODY) != textures.end())
            {
                SDL_SetAlpha(textures.find(PART_BODY)->second->getSDLSurface(), 0, 255);
                SDL_BlitSurface(textures.find(PART_BODY)->second->getSDLSurface(), NULL, surface, NULL);
            }
            if (textures.find(PART_HAIR) != textures.end())
            {
                SDL_SetAlpha(textures.find(PART_HAIR)->second->getSDLSurface(), SDL_SRCALPHA | SDL_RLEACCEL, 0);
                SDL_BlitSurface(textures.find(PART_HAIR)->second->getSDLSurface(), NULL, surface, NULL);
            }
            if (textures.find(PART_LEGS) != textures.end())
            {
                SDL_SetAlpha(textures.find(PART_LEGS)->second->getSDLSurface(), SDL_SRCALPHA | SDL_RLEACCEL, 0);
                SDL_BlitSurface(textures.find(PART_LEGS)->second->getSDLSurface(), NULL, surface, NULL);
            }
            if (textures.find(PART_CHEST) != textures.end())
            {
                SDL_SetAlpha(textures.find(PART_CHEST)->second->getSDLSurface(), SDL_SRCALPHA | SDL_RLEACCEL, 0);
                SDL_BlitSurface(textures.find(PART_CHEST)->second->getSDLSurface(), NULL, surface, NULL);
            }
            if (textures.find(PART_FEET) != textures.end())
            {
                SDL_SetAlpha(textures.find(PART_FEET)->second->getSDLSurface(), SDL_SRCALPHA | SDL_RLEACCEL, 0);
                SDL_BlitSurface(textures.find(PART_FEET)->second->getSDLSurface(), NULL, surface, NULL);
            }
            tex->setImage(surface);
        }

        mTextures.insert(std::pair<std::string, Texture*>(tex->getName(), tex));
        return tex;
    }


	Node* GraphicsEngine::getNode(int x, int y)
    {
        Point pt; pt.x = x; pt.y = y;

        for (int i = mapEngine->getLayers() - 1; i >= 0; --i)
        {
            NodeItr itr = mapEngine->getLayer(i)->getFrontNode();
            NodeItr itr_end = mapEngine->getLayer(i)->getEndNode();

            while (itr != itr_end)
            {
                Node *node = *itr;
                if (checkInside(pt, node->getBounds()))
                {
                    return node;
                }
                ++itr;
            }
        }

        return NULL;
    }

    Node* GraphicsEngine::getTile(int x, int y)
    {
        Point pt;
        pt.x = x + mCamera->getPosition().x;
        pt.y = y + mCamera->getPosition().y;
;
        return mapEngine->getTile(pt);
    }

    void GraphicsEngine::setCameraToShow(const Point &pt, int delay)
    {
        Point camPt;
        camPt.x = pt.x - (mWidth >> 1);
        camPt.y = pt.y - (mHeight >> 1);
        mCamera->setDestination(camPt, delay);
    }

    void GraphicsEngine::warpCamera(const Point &pt)
    {
        Point camPt;
        camPt.x = pt.x - (mWidth >> 1);
        camPt.y = pt.y - (mHeight >> 1);
        mCamera->setPosition(camPt);
    }

    void GraphicsEngine::saveFrames()
    {
        mFPS.push_back(mFrames);
        mFrames = 0;

        if (mFPS.size() > mAverageTime)
        {
            int av = 0;
            for (unsigned int i = 0; i < mAverageTime; ++i)
            {
                av += mFPS[i];
            }
            av = av / mAverageTime;
            mFPS.clear();
            mAverageFPS = av;
        }
    }

    unsigned int GraphicsEngine::getFPS()
    {
        return mAverageFPS;
    }

    void GraphicsEngine::sort()
    {
        for (unsigned int i = 0; i < mapEngine->getLayers(); ++i)
        {
            if (mapEngine->getLayer(i)->isCollisionLayer())
                continue;
            mapEngine->getLayer(i)->sortNodes(0, mapEngine->getLayer(i)->getSize());
        }
    }
}
