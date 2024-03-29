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
 *	Date of file creation: 08-09-27
 *
 *	$Id$
 *
 ********************************************/

#ifndef ST_MAP_HEADER
#define ST_MAP_HEADER

#include <list>
#include <string>
#include <vector>

#include "utilities/types.h"

class TiXmlElement;

namespace ST
{
	class Node;
	class Texture;

	enum
	{
	    DIRECTION_NORTH = 0,
	    DIRECTION_NORTHEAST,
	    DIRECTION_EAST,
	    DIRECTION_SOUTHEAST,
	    DIRECTION_SOUTH,
	    DIRECTION_SOUTHWEST,
	    DIRECTION_WEST,
	    DIRECTION_NORTHWEST
	};

    /**
     * The Tileset class holds data for each set of tiles
     */
	class Tileset
	{
	public:
		int id;
		int width;
		int height;
		std::string tilename;
	};

	/**
	 * The Layer class holds map layer data
	 */
	class Layer
	{
    public:
        typedef std::vector<Node*>::iterator NodeItr;
	public:
		Layer(const std::string &name, unsigned int width, unsigned int height);
		~Layer();

        /**
         * Set as collision layer
         */
        void setCollisionLayer();

        /**
         * Returns if this is the collision layer
         */
        bool isCollisionLayer() { return mCollisionLayer; }

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
		 * Remove Node
		 * Removes a node from the layer
		 * @param node The node to remove
		 */
        void removeNode(Node *node);

		/**
		 * Get Tile At
		 * Returns the tile at x, y
		 * @param x The x position of the tile to return
		 * @param y The y position of the tile to return
		 * @return Returns the Tile found at the given location
		 */
		Node* getNodeAt(unsigned int x, unsigned int y);

        /**
         * Get Node iterator
         */
        NodeItr getFrontNode();
        NodeItr getEndNode();

        /**
         * Sort Nodes
         */
        void sortNodes(int first, int size);
        int findMiddleNode(int first, int size);

        /**
         * Get Size
         * Returns the number of nodes in layer
         */
        int getSize() const;

        /**
         * Get Name
         * Returns the name of the layer
         */
        const std::string& getName() const { return mName; }

    private:
		std::vector<Node*> mNodes;
		std::string mName;
		unsigned int mWidth;
		unsigned int mHeight;
		bool mCollisionLayer;
	};

	/**
     * The Map class contains the map data
     */

	class Map
	{
	public:
		/**
		 * Constructor
		 */
		Map();
		~Map();

		/**
		 * Load Map.
		 * Reads the map from a file
		 * @param filename The name of the file to read the map from
		 * @return Returns true if successfully read in map
		 */
		bool loadMap(const std::string &filename);

		/**
		 * Map Loaded.
		 * Has the map been loaded?
		 * @return Returns whether the map has been loaded successfully.
		 */
        bool mapLoaded() { return mLoaded; }

        /**
		 * Unload map
		 * Call when done
		 */
        void unload();

		/**
		 * Returns width of tiles
		 */
        unsigned int getTileWidth() { return mTileWidth; }

        /**
         * Returns height of tiles
         */
        unsigned int getTileHeight() { return mTileHeight; }

        /**
         * Returns width of map in tiles
         */
        unsigned int getWidth() { return mWidth; }

        /**
         * Returns height of map in tiles
         */
        unsigned int getHeight() { return mHeight; }

        /**
         * Returns the number of layers
         */
        unsigned int getLayers() { return mLayers.size(); }

        /**
         * Returns a layer by number
         */
        Layer* getLayer(unsigned int layer);

        /**
         * Returns a layer by name
         */
        Layer* getLayer(const std::string &layer);

        /**
         * Walks a tile on the map
         */
        Point walkMap(const Point &pos, int dir);

        /**
         * Return tile in direction
         * @param pos The tile position
         * @param dir The direction from that position
         * @return Returns the node at that position + direction
         */
        Node* getTile(const Point &pos, int dir);

        /**
         * Return map tile
         * @param pos The world pixel position of the tile
         * @return Returns the node at that position
         */
        Node* getTile(const Point &pos);

        /**
         * Return map tile
         * @param x The tile x position
         * @param y The tile y position
         * @param layer The layer of the tile
         * @return Returns the node at that position
         */
        Node* getTile(int x, int y, unsigned int layer);

        /**
         * Return tile position
         * @param x Pixel x to convert
         * @param y Pixel y to convert
         * @return The tile x and y
         */
        Point convertPixelToTile(int x, int y);

        /**
         * Return pixel position
         * @param pt Tile position
         * @return The pixel position
         */
        Point convertTileToPixel(const Point &pt);

        /**
         * Calculate distance between 2 positions in tiles
         * @param pos1 Position 1
         * @param pos2 Position 2
         * @return The distance in tiles
         */
        int calculateDistanceInTiles(const Point &pos1, const Point &pos2);

        /**
         * Return if a tile is blocking
         * @param pos The tile position of the tile
         */
        bool blocked(const Point &pos);

        /**
         * Remove a node
         * Removes the node from the map
         */
        void removeNode(Node *node);

    private:
		/**
		 * Add Layer.
		 * Adds a layer to the map
		 */
        void addLayer(const std::string &name, unsigned int width, unsigned int height, unsigned char *data,
			unsigned int len);

        /**
         * Load Map Info from xml file.
         * Gets the attributes from the XML element and stores them
         * @return Returns whether it succeeded
         */
        bool loadMapInfo(TiXmlElement *e);

        /**
         * Load Tileset from xml file.
         * Load in images for tileset
         * @return Returns whether it succeeded
         */
        bool loadTileset(TiXmlElement *e);

        /**
         * Load Layer from xml file.
         * Load in data for layer
         * @return Returns whether it succeeded
         */
        bool loadLayer(TiXmlElement *e);

	private:
		std::vector<Layer*> mLayers;
		typedef std::vector<Layer*>::iterator LayerItr;
		std::vector<Tileset*> mTilesets;
        Point mTileWalk[8];
		int mWidth;
		int mHeight;
		int mTileWidth;
		int mTileHeight;
		bool mLoaded;
	};

	extern Map *mapEngine;
}

#endif
