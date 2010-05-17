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

/**
 * The Graphics Engine is the main rendering class
 */

#ifndef ST_GRAPHICS_HEADER
#define ST_GRAPHICS_HEADER

#include <list>
#include <map>
#include <string>
#include <vector>

struct ag_surface;
struct SDL_Surface;

namespace ST
{
	class Node;
	class Entity;
	class Geometry;
	class Overlay;
	class Texture;
	class Camera;
	class GameState;
	struct Point;
	struct Rectangle;

	class GraphicsEngine
	{
	public:
		/**
		 * Constructor
		 * Creates the renderer, should only be created once
		 */
		GraphicsEngine();

		/**
		 * Destructor
		 * Cleans up class
		 */
		virtual ~GraphicsEngine();

		/**
		 * Initialise the engine
		 */
		virtual bool init(int fullscreen, int x, int y) = 0;

		/**
		 * Create Node
		 * Creates a new node, and returns it
		 * @param name The name of the Node
		 * @param texture The name of the texture to use
		 * @param point The position of the Node
		 * @return Returns a pointer to the Node created
		 */
		Node* createNode(std::string name, std::string texture, Point *point = NULL);

		/**
		 * Add Node
		 * Adds an already created node to the render list
		 * @param node The node to add
		 */
        void addNode(Node *node);

        /**
         * Remove Node
         * Removes a node from displaying
         */
        void removeNode(Node *node);

		/**
		 * Create Entity
		 * Creates a new entity, and returns it
		 * @param name The name of the Entity
		 * @param texture The name of the texture to use
		 * @param point The position of the Entity
		 * @return Returns a pointer to the Entity created
		 */
		Entity* createEntity(std::string name, std::string texture, Point *point = NULL);

		/**
		 * Set Camera
		 * Sets the default camera to use for drawing to
		 * @param cam The camera to use
		 */
		void setCamera(Camera *cam);

        /**
         * Get Camera
         * Returns the default camera
         */
        Camera* getCamera() const;

		/**
		 * Render Frame
		 * Renders a single frame to the screen
		 */
		void renderFrame();

		/**
		 * Display Nodes
		 * This puts the nodes on screen
		 * It loops through all the nodes and draws them if visible
         * @param layer The layer to output
		 */
		void outputNodes(int layer);

		/**
		 * Draw Untextured Rectangle
		 */
		virtual void drawRect(Rectangle &rect, bool filled) = 0;

		/**
		 * Draw Textured Rectangle
		 */
		virtual void drawTexturedRect(Rectangle &rect, Texture *texture) = 0;

		/**
		 * Returns the current SDL surface
		 */
        SDL_Surface* getSurface() { return mScreen; }

		/**
		 * Load a single texture
		 */
		Texture* loadTexture(const std::string &name);
		bool loadTextureSet(const std::string &name, int w, int h);
        bool loadTextureSet(const std::string &name, const std::string &file, int w, int h);
		SDL_Surface* loadSDLTexture(const std::string &name);

		/**
		 * Create Texture
		 * Creates a new texture
		 * @param surface The surface of the texture
		 * @param width The width of the texture
		 * @param height The height of the texture
		 * @return Returns the texture created
		 */
		Texture* createTexture(SDL_Surface *surface, std::string name,
						   unsigned int x, unsigned int y,
						   unsigned int width, unsigned height);

		/**
		 * Create Surface
		 * Creates a new SDL_Surface from a GL texture
		 */
		virtual SDL_Surface* createSurface(unsigned int texture, int width, int height) = 0;

		/**
		 * Get Pixel
		 * @return Returns the pixel at the position of the SDL Surface
		 */
		unsigned int getPixel(SDL_Surface *s, int x, int y) const;

		/**
		 * Get Texture
		 * @param name The name of the texture
		 * @return Returns the texture or NULL if texture not found
		 */
		Texture* getTexture(const std::string &name);
		Texture* getAnimatedTexture(const std::string &name, int frame);

		/**
		 * Get Screen Width
		 */
        int getScreenWidth() const;

        /**
         * Get Screen Height
         */
        int getScreenHeight() const;

        /**
         * Get whether using OpenGL
         */
        int isOpenGL() const { return mOpenGL; }

        /**
         * Create a texture based on body parts
         */
        Texture* createAvatar(unsigned int id, int bodyId, int hairId, int dir);
        Texture* createAvatarFrame(unsigned int id, unsigned int frame, Texture *bodyTex, Texture *hairTex, int dir);

        /**
         * Get the node at that position
         */
        Node* getNode(int x, int y);

        /**
         * Get the tile at that position
         */
        Node* getTile(int x, int y);

        /**
         * Set Camera to Show a Point on screen
         */
        void setCameraToShow(const Point &pt, int delay);

        /**
         * Warp camera to a point on screen
         */
        void warpCamera(const Point &pt);

        /**
         * Save FPS
         */
        void saveFrames();

        /**
         * Get FPS average
         */
        unsigned int getFPS();

	protected:
		SDL_Surface *mScreen;
		int mWidth;
		int mHeight;
		int mOpenGL;

		virtual void setupScene() = 0;
		virtual void endScene() = 0;

	private:
        Camera *mCamera;
		unsigned int mFrames;
		unsigned int mAverageTime; // how often to update fps average
		unsigned int mAverageFPS;
		std::vector<unsigned int> mFPS;

        typedef std::vector<Node*>::iterator NodeItr;

		// list of textures
		std::map<std::string, Texture*> mTextures;
		typedef std::map<std::string, Texture*>::iterator TextureItr;
	};

	extern GraphicsEngine *graphicsEngine;
}

#endif
