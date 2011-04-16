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
 *	Date of file creation: 08-10-05
 *
 *	$Id$
 *
 ********************************************/

#include "map.h"
#include "tile.h"
#include "resourcemanager.h"

#include "graphics/graphics.h"
#include "graphics/node.h"
#include "graphics/texture.h"

#include "utilities/base64.h"
#include "utilities/gzip.h"
#include "utilities/log.h"
#include "utilities/math.h"

#include <sstream>
#include <tinyxml.h>

namespace ST
{
	Layer::Layer(const std::string &name, unsigned int width, unsigned int height) :
		mName(name),
		mWidth(width),
		mHeight(height),
		mCollisionLayer(false)
	{

	}

	Layer::~Layer()
	{
        NodeItr itr = mNodes.begin(), itr_end = mNodes.end();
		while (itr != itr_end)
		{
            if (*itr)// node may already have been deleted
            {
                delete (*itr);
            }
			++itr;
		}
		mNodes.clear();
	}

	void Layer::setCollisionLayer()
	{
	    mCollisionLayer = true;
	}

	void Layer::setTile(int x, int y, Texture *tex, int width, int height)
	{
	    std::stringstream str;
	    Point p;

	    str << "tile" << x << "-" << y;
		p.x = 0.5 * (x - y) * width;
		p.y = 0.5 * (x + y) * height;

	    // add node and set its position
        Node *node = new Node(str.str(), tex);
        node->moveNode(&p);
        addNode(node);
	}

	void Layer::addNode(Node *node)
	{
        mNodes.push_back(node);
	}

	void Layer::removeNode(Node *node)
	{
	    NodeItr itr = mNodes.begin(), itr_end = mNodes.end();
	    while (itr != itr_end)
	    {
	        if ((*itr)->getName() == node->getName())
	        {
	            mNodes.erase(itr);
	            return;
	        }
	        ++itr;
	    }
	}

	Node* Layer::getNodeAt(unsigned int x, unsigned int y)
	{
	    NodeItr itr = mNodes.begin(), itr_end = mNodes.end();
	    while (itr != itr_end)
	    {
	        Node *node = *itr;
	        Point pt = node->getTilePosition();
	        if (pt.x == x && pt.y == y)
	        {
	            return node;
	        }
	        ++itr;
	    }

	    return NULL;
	}

    Layer::NodeItr Layer::getFrontNode()
    {
        return mNodes.begin();
    }

    Layer::NodeItr Layer::getEndNode()
    {
        return mNodes.end();
    }

    void Layer::sortNodes(int first, int size)
    {
        Node *pivot;
        int last = first + size - 1;
        int middle;
        int lower = first;
        int higher = last;

        if (size <= 1)
            return;

        middle = findMiddleNode(first, size);
        pivot = mNodes[middle];
        mNodes[middle] = mNodes[first];

        while (lower < higher)
        {
            while ((pivot->getPosition().y - pivot->getHeight()) <
                   (mNodes[higher]->getPosition().y - mNodes[higher]->getHeight()) &&
                   lower < higher)
                --higher;

            if (higher != lower)
            {
                mNodes[lower] = mNodes[higher];
                ++lower;
            }

            while ((pivot->getPosition().y - pivot->getHeight()) >
                   (mNodes[lower]->getPosition().y - mNodes[lower]->getHeight()) &&
                   lower < higher)
                ++lower;

            if (higher != lower)
            {
                mNodes[higher] = mNodes[lower];
                --higher;
            }
        }

        mNodes[lower] = pivot;
        sortNodes(first, lower - first);
        sortNodes(lower + 1, last - lower);
    }

    int Layer::findMiddleNode(int first, int size)
    {
        int last = first + size - 1;
        int middle = first + (size >> 1);

        int fy = mNodes[first]->getPosition().y - mNodes[first]->getHeight();
        int my = mNodes[middle]->getPosition().y - mNodes[middle]->getHeight();
        int ly = mNodes[last]->getPosition().y - mNodes[last]->getHeight();

        if (fy > my && fy > ly)
        {
            if (my > ly)
                return middle;
            else
                return last;
        }

        if (my > fy && my > ly)
        {
            if (fy > ly)
                return first;
            else
                return last;
        }

        if (my > fy)
            return middle;
        else
            return first;
    }

    int Layer::getSize() const
    {
        return mNodes.size();
    }

	Map::Map()
	{
        mWidth = 0;
        mHeight = 0;
        mTileWidth = 0;
        mTileHeight = 0;
        mLoaded = false;

		mTileWalk[0].x = 0;
		mTileWalk[0].y = -1;
		mTileWalk[1].x = 1;
		mTileWalk[1].y = -1;
		mTileWalk[2].x = 1;
		mTileWalk[2].y = 0;
		mTileWalk[3].x = 1;
		mTileWalk[3].y = 1;
		mTileWalk[4].x = 0;
		mTileWalk[4].y = 1;
		mTileWalk[5].x = -1;
		mTileWalk[5].y = 1;
		mTileWalk[6].x = -1;
		mTileWalk[6].y = 0;
		mTileWalk[7].x = -1;
		mTileWalk[7].y = -1;
	}

	Map::~Map()
	{

	}

	bool Map::loadMap(const std::string &filename)
	{
	    std::string file = resourceManager->getDataPath(filename);
        logger->logDebug("Loading map " + file);

        TiXmlDocument doc(file.c_str());
        bool loaded = doc.LoadFile();
        if (!loaded)
        {
            logger->logError("Error loading map");
            return false;
        }

        TiXmlHandle hDoc(&doc);
        TiXmlElement *e;
        TiXmlHandle map = hDoc.FirstChild("map");

        if (!loadMapInfo(map.ToElement()))
        {
            return false;
        }

        int numTilesets = 0;

        while(1)
        {
            e = map.Child("tileset", numTilesets).ToElement();

            if (!loadTileset(e))
            {
                break;
            }
            ++numTilesets;
        }

        if (numTilesets == 0)
        {
            logger->logError("No tilesets found.");
            return false;
        }

        int numLayers = 0;
        while(1)
        {
            e = map.Child("layer", numLayers).ToElement();

            if (!loadLayer(e))
            {
                break;
            }
            ++numLayers;
        }
        if (numLayers == 0)
        {
            logger->logError("No layers found!");
            return false;
        }

        logger->logDebug("Finished loading map");

        mLoaded = true;

        return true;
    }

    void Map::unload()
    {
        for (unsigned i = 0; i < mLayers.size(); ++i)
        {
            delete mLayers[i];
        }
        mLayers.clear();
        mWidth = 0;
        mHeight = 0;
        mTileWidth = 0;
        mTileHeight = 0;
        mLoaded = false;
    }

    Layer* Map::getLayer(unsigned int layer)
    {
        if (layer >= mLayers.size())
            return NULL;
        return mLayers[layer];
    }

    Layer* Map::getLayer(const std::string &layer)
    {
        LayerItr itr = mLayers.begin(), itr_end = mLayers.end();
        while (itr != itr_end)
        {
            if ((*itr)->getName() == layer)
                return *itr;
            ++itr;
        }
        return NULL;
    }

    Point Map::walkMap(const Point &pos, int dir)
    {
        Point newPos = pos;

        newPos.x += mTileWalk[dir].x;
		newPos.y += mTileWalk[dir].y;

        return newPos;
    }

    Node* Map::getTile(const Point &pos, int dir)
    {
        Point newPos = walkMap(pos, dir);

        int tilex = 0.5 * (newPos.x - newPos.y) * mTileWidth;
        int tiley = 0.5 * (newPos.x + newPos.y) * mTileHeight;

        return mLayers[0]->getNodeAt(tilex,tiley);
    }

    Node* Map::getTile(const Point &pos)
    {
        Point tilePos = convertPixelToTile(pos.x, pos.y);

        return mLayers[0]->getNodeAt(tilePos.x, tilePos.y);
    }

    Node* Map::getTile(int x, int y, unsigned int layer)
    {
        if (layer >= mLayers.size())
            return NULL;
        return mLayers[layer]->getNodeAt(x, y);
    }

    bool Map::blocked(const Point &pos)
    {
        assert(mLayers.size() > 1);
        if (pos.x < 0 || pos.x > mWidth || pos.y < 0 || pos.y > mHeight)
            return true;
        LayerItr itr = mLayers.begin(), itr_end = mLayers.end();
        while (itr != itr_end)
        {
            if ((*itr)->isCollisionLayer())
            {
                Node *node = (*itr)->getNodeAt(pos.x, pos.y);
                if (!node)
                    return false;
                return true;
            }
            ++itr;
        }
        return false;
    }

	bool Map::loadMapInfo(TiXmlElement* e)
	{
	    if (!e)
        {
            logger->logError("Invalid map format");
            return false;
        }

	    if (e->QueryIntAttribute("width", &mWidth) != TIXML_SUCCESS)
        {
            logger->logError("Invalid map width");
            return false;
        }

        if (e->QueryIntAttribute("height", &mHeight) != TIXML_SUCCESS)
        {
            logger->logError("Invalid map height");
            return false;
        }

        if (e->QueryIntAttribute("tilewidth", &mTileWidth) != TIXML_SUCCESS)
        {
            logger->logError("Invalid tile width");
            return false;
        }

        if (e->QueryIntAttribute("tileheight", &mTileHeight) != TIXML_SUCCESS)
        {
            logger->logError("Invalid tile height");
            return false;
        }

        return true;
	}

	bool Map::loadTileset(TiXmlElement *e)
	{
        if (!e)
        {
            return false;
        }

		int id = 0;
		if (e->QueryIntAttribute("firstgid", &id) != TIXML_SUCCESS)
		{
			logger->logError("No tile id found");
            return false;
		}

		int width = 0;
		int height = 0;
		if (e->QueryIntAttribute("tilewidth", &width) != TIXML_SUCCESS)
		{
			width = mTileWidth;
		}
		if (e->QueryIntAttribute("tileheight", &height) != TIXML_SUCCESS)
		{
			height = mTileHeight;
		}

        e = e->FirstChild("image")->ToElement();
        if (!e)
        {
            logger->logError("No images for tilesets defined!");
            return false;
        }

		std::string imagefile = e->Attribute("source");

        if (imagefile.empty())
        {
            logger->logError("No source for image");
            return false;
        }

        imagefile = resourceManager->getDataPath(imagefile);

        if (imagefile.empty())
        {
            logger->logError(imagefile + " not found.");
            return false;
        }

        if (!graphicsEngine->loadTextureSet(imagefile, width, height))
        {
            logger->logError("Unable to load texture for map");
            return false;
        }

		Tileset *tileset = new Tileset;
		tileset->id = id;
		tileset->width = width;
		tileset->height = height;
		tileset->tilename = imagefile;
		mTilesets.push_back(tileset);

        return true;
	}

	bool Map::loadLayer(TiXmlElement *e)
	{
	    if (!e)
        {
            return false;
        }

        int layerWidth, layerHeight;
        std::string layerName;

        if (e->QueryIntAttribute("width", &layerWidth) != TIXML_SUCCESS)
        {
            logger->logError("No layer width");
            return false;
        }

        if (e->QueryIntAttribute("height", &layerHeight) != TIXML_SUCCESS)
        {
            logger->logError("No layer height");
            return false;
        }

        // layer name is optional, needed for collision layer
        layerName = e->Attribute("name");
        if (layerName == "Character")
        {
            Layer *l = new Layer(layerName, layerWidth, layerHeight);
            mLayers.push_back(l);
            return true;
        }

        // Load in compressed base64 map
        e = e->FirstChild("data")->ToElement();
        const char *data = e->GetText();
        if (!data)
        {
            logger->logError("No data");
            return false;
        }

        // convert from base 64
        int length = Base64::decodeSize(strlen(data));
        unsigned char *compressedData = new unsigned char[length];
        compressedData = Base64::decode(data, compressedData);

        if (length == 0)
        {
            delete compressedData;
            logger->logError("Unable to convert from base64");
            return false;
        }

        unsigned char *layerData;

        unsigned int inflatedSize = Gzip::inflateMemory(compressedData, length, layerData);

        delete compressedData;

        if (inflatedSize == 0)
        {
            logger->logError("Unable to uncompress map data");
            return false;
        }

        addLayer(layerName, layerWidth, layerHeight, layerData, inflatedSize);

        return true;
	}

	void Map::addLayer(const std::string &name, unsigned int width, unsigned int height, unsigned char *data,
		unsigned int len)
    {
        Layer *l = new Layer(name, width, height);

        if (name == "collision")
            l->setCollisionLayer();

        // load in the layer data
	    int length = len - 3;
	    int x = 0;
	    int y = 0;

        for (int i = 0; i < length; i += 4)
        {
            // get the tile id by putting
            // the data bytes into an integer
            int tile_id = data[i] | data[i + 1] << 8 |
                            data[i + 2] << 16 | data[i + 3] << 24;

			if (tile_id > 0)
			{
				// search out the tileset the tile_id belongs to
				for (size_t j = mTilesets.size()-1; j >= 0; --j)
				{
					// since we are searching backwards,
					// we assume that the later tilesets
					// have a greater tile_id, when tile_id
					// is then greater, it means its part of the tileset
					if (tile_id >= mTilesets[j]->id)
					{
						std::stringstream str;
						str << mTilesets[j]->tilename << (tile_id - mTilesets[j]->id) + 1;
						l->setTile(x, y, graphicsEngine->getTexture(str.str()), mTileWidth, mTileHeight);
						break;
					}
				}
			}
            ++x;

            // reached the end of the row
            if (x == mWidth)
            {
                x = 0;
                ++y;

                // reached the end of the map
                if (y == mHeight)
                    break;
            }
        }

        // finished with the data now, free it
        free(data);

        mLayers.push_back(l);
    }

    Point Map::convertPixelToTile(int x, int y)
    {
        Point pt;
        const float ratio = (float)mTileWidth / mTileHeight;

        x -= mTileWidth / 2;
        const float mx = y + (x / ratio);
        const float my = y - (x / ratio);

        pt.x = mx / mTileHeight;
        pt.y = my / mTileHeight;

        return pt;
    }

    Point Map::convertTileToPixel(const Point &pt)
    {
        Point pixel;

        pixel.x = 0.5 * (pt.x - pt.y) * mTileWidth;
        pixel.y = 0.5 * (pt.x + pt.y) * mTileHeight;

        return pixel;
    }

    void Map::removeNode(Node *node)
    {
        for (unsigned i = 0; i < mLayers.size(); ++i)
        {
            Layer *layer = getLayer(i);
            layer->removeNode(node);
        }
    }
}


