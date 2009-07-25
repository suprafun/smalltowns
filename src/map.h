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
 *	Date of file creation: 08-09-27
 *
 *	$Id$
 *
 ********************************************/

/**
 * The Map class contains the map data
 */

#ifndef ST_MAP_HEADER
#define ST_MAP_HEADER

#include <list>
#include <string>
#include <vector>

class TiXmlElement;

namespace ST
{
	class Node;
	class Texture;

	class Tileset
	{
	public:
		int id;
		int width;
		int height;
		std::string tilename;
	};

	class Layer
	{
	public:
		Layer(unsigned int, unsigned int);
		~Layer();

        /**
         * Set a tile
         */
        void setTile(int x, int y, Texture *tex, int width, int height);

		/**
		 * Add Tile
		 * Adds a tile to the layer
		 * @param tile The tile to add
		 */
		void addNode(Node *node);

		/**
		 * Get Tile At
		 * Returns the tile at x, y
		 * @param x The x position of the tile to return
		 * @param y The y position of the tile to return
		 * @return Returns the Tile found at the given location
		 */
		Node* getNodeAt(unsigned int x, unsigned int y);

	private:
		std::list<Node*> mNodes;
        typedef std::list<Node*>::iterator NodeItr;
		unsigned int mWidth;
		unsigned int mHeight;
	};

	class Map
	{
	public:
		/**
		 * Constructor
		 */
		Map();
		~Map();

		/**
		 * Load
		 * Reads the map from a file
		 * @param filename The name of the file to read the map from
		 * @return Returns true if successfully read in map
		 */
		bool loadMap(const std::string &filename);

		/**
		 * Return width of tiles
		 * Return height of tiles
		 */
        unsigned int getTileWidth() { return mTileWidth; }
        unsigned int getTileHeight() { return mTileHeight; }

    private:
		/**
		 * Add Layer
		 * Adds a layer to the map
		 */
        void addLayer(unsigned int width, unsigned int height, unsigned char *data,
			unsigned int len);

        /**
         * Load Map Info from xml file
         * Gets the attributes from the XML element and stores them
         * @return Returns whether it succeeded
         */
        bool loadMapInfo(TiXmlElement *e);

        /**
         * Load Tileset from xml file
         * Load in images for tileset
         * @return Returns whether it succeeded
         */
        bool loadTileset(TiXmlElement *e);

        /**
         * Load Layer from xml file
         * Load in data for layer
         * @return Returns whether it succeeded
         */
        bool loadLayer(TiXmlElement *e);

	private:
		std::vector<Layer*> mLayers;
		std::vector<Tileset*> mTilesets;
		int mWidth;
		int mHeight;
		int mTileWidth;
		int mTileHeight;
	};

	extern Map *mapEngine;
}

#endif
