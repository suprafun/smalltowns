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
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <FTGL/ftgl.h>

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

		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

		// Set the window title
		SDL_WM_SetCaption("Towns Life", NULL);

		// get bpp of desktop
		const SDL_VideoInfo* video = SDL_GetVideoInfo();
		int bpp = video->vfmt->BitsPerPixel;

		mWidth = 1024;
		mHeight = 768;

		// Initialise SDL to use OpenGL
		mScreen = SDL_SetVideoMode(mWidth, mHeight, bpp, SDL_OPENGL);

		glViewport(0, 0, mWidth, mHeight);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glClearColor(1.0f, 1.0f , 1.0f, 0.5f);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthFunc(GL_LEQUAL);

		mCamera = NULL;
		mFont = NULL;
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

		if (mFont)
			delete mFont;

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
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0.0f, mWidth, 0.0f, mHeight, -1.0f, 1.0f);

		// Display the nodes on screen (if theres a camera to view them)
		if (mCamera)
			outputNodes();

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        interfaceManager->drawWindows();

        // Swap between front and back buffer
		SDL_GL_SwapBuffers();
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
		while (itr != itr_end)
		{
			// dont draw if not on screen
			if ((*itr)->getPosition().x + (*itr)->getWidth() < pt.x ||
				(*itr)->getPosition().x > pt.x + w ||
				(*itr)->getPosition().y + (*itr)->getHeight() < pt.y ||
				(*itr)->getPosition().y > pt.y + h)
			{
				break;
			}

			// dont draw if not visible
			if (!(*itr)->getVisible())
			{
				break;
			}

			Rectangle rect;

			// set position and size to local variables
			rect.x = (*itr)->getPosition().x;
			rect.y = (*itr)->getPosition().y;
			rect.width = (*itr)->getWidth();
			rect.height = (*itr)->getHeight();

			drawTexturedRect(rect, (*itr)->getGLTexture());

			++itr;
		}
	}

	void GraphicsEngine::drawRect(Rectangle &rect, bool filled)
	{
		// reset identity matrix
		glLoadIdentity();

		glPushAttrib(GL_POLYGON_BIT|GL_LIGHTING_BIT|GL_DEPTH_BUFFER_BIT);
		glPushMatrix();

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

	void GraphicsEngine::drawTexturedRect(Rectangle &rect, unsigned int texture)
	{
		// reset identity matrix
		glLoadIdentity();

		glPushAttrib(GL_POLYGON_BIT|GL_LIGHTING_BIT|GL_ENABLE_BIT|GL_DEPTH_BUFFER_BIT|GL_TEXTURE_BIT);
		glPushMatrix();

		// set position and size to local variables
		float x = (float)rect.x;
		float y = (float)rect.y;
		float width = (float)rect.width;
		float height = (float)rect.height;

		// move to the correct position
		glTranslatef(x, y, 0.0f);

		// enable transparancy
		glEnable(GL_BLEND);

		// disable depth testing
		glDisable(GL_DEPTH_TEST);

		// enable texture mapping
		glBindTexture(GL_TEXTURE_2D, (GLuint)texture);
		glEnable(GL_TEXTURE_2D);
		glPolygonMode(GL_FRONT, GL_FILL);
		glColor3f(1.0f, 1.0f, 1.0f);

		glBegin(GL_QUADS);

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

		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);

		glPopMatrix();
		glPopAttrib();
	}

	void GraphicsEngine::drawText(Point &pos, const std::string &text, int fontSize)
	{
		if (mFont && text.size() > 0)
		{
			glLoadIdentity();
			glColor3f(0.0f, 0.0f, 0.0f); // black text
			glPolygonMode(GL_FRONT, GL_FILL);
			mFont->FaceSize(fontSize);
			mFont->Render(text.c_str(), text.size(), FTPoint(pos.x, pos.y));
		}
	}

	void GraphicsEngine::setFont(const std::string &font)
	{
		mFont = new FTGLTextureFont(font.c_str());
	}

	float GraphicsEngine::getFontHeight()
	{
		return mFont->LineHeight();
	}

	float GraphicsEngine::getFontWidth(const std::string &text)
	{
		return mFont->Advance(text.c_str(), text.size());
	}

	void GraphicsEngine::loadTexture(const std::string &name)
	{
		std::map<std::string, Texture*>::iterator itr;
		itr = mTextures.find(name);
		if (itr != mTextures.end())
			itr->second->increaseCount();
		SDL_Surface* s = NULL;

		// Load in the texture
		s = IMG_Load(name.c_str());
		if (s)
		{
			createTexture(s, name,0, 0, s->w, s->h);
		}
		else
		{
		    logger->logError("Image not found: " + name);
		}
	}

	SDL_Surface* GraphicsEngine::loadSDLTexture(const std::string &name)
	{
		SDL_Surface* s = NULL;

		// Load in the texture
		s = IMG_Load(name.c_str());
		if (!s)
		{
			logger->logError("Image not found: " + name);
		}

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

		// Put the frame into new surface
		SDL_Surface *tex = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height,
			surface->format->BitsPerPixel, rmask, gmask, bmask, amask);

		if (!tex)
		{
			// error
			logger->logError("Could not create new surface");
			SDL_FreeSurface(surface);
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
		SDL_FreeSurface(surface);

		// Create texture from frame
		Texture *texture = new Texture(name, width, height);
		texture->setPixels(tex);

		mTextures.insert(std::pair<std::string, Texture*>(texture->getName(),
			texture));

		SDL_FreeSurface(tex);

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
