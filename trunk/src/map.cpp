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
 *	Date of file creation: 08-10-05
 *
 *	$Id$
 *
 ********************************************/

#include "map.h"
#include "tile.h"

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
	Layer::Layer(unsigned int width, unsigned int height) :
		mWidth(width),
		mHeight(height)
	{

	}

	Layer::~Layer()
	{

	}

	void Layer::setData(unsigned char *data, int len, const std::string &tileset)
	{

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
				std::stringstream str;
				str << tileset << tile_id;
				Texture *tex = graphicsEngine->getTexture(str.str());
				setTile(x, y, tex);
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
	}

	void Layer::setDepth(unsigned int depth)
	{

	}

	void Layer::setTile(int x, int y, Texture *tex)
	{
	    std::stringstream str;
	    Point p;

	    str << "tile" << x << y;
	    int tempx = x * tex->getWidth(), tempy = y * tex->getHeight();
		p.x = tempx + (tempy * 0.5) - (tempx * 0.5);
		p.y = tempy - (tempx * 0.5) + (tempy * 0.5);

	    // add node and set its position
        Node *node = graphicsEngine->createNode(str.str(), tex->getName(), &p);
        addNode(node);
	}

	void Layer::addNode(Node *node)
	{
        mNodes.push_back(node);
	}

	Node* Layer::getNodeAt(unsigned int x, unsigned int y)
	{
		// iterators for searching through the list of tiles
		std::list<Node*>::iterator itr = mNodes.begin();
		std::list<Node*>::const_iterator itr_end = mNodes.end();
		// The position to look at tile in
		Point p;
		p.x = x;
		p.y = y;

		while (itr != itr_end)
		{
			// Check whether the co-ordinates are inside the tile
			if (checkInside(p, (*itr)->getBounds()))
			{
				return (*itr);
			}
			++itr;
		}
		return NULL;
	}

	Map::Map()
	{
        mWidth = 0;
        mHeight = 0;
        mTileWidth = 0;
        mTileHeight = 0;
	}

	Map::~Map()
	{

	}

	bool Map::loadMap(const std::string &filename)
	{
		logger->logDebug("Loading map " + filename);

        TiXmlDocument doc(filename.c_str());
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

        e = map.Child("tileset", 0).ToElement();

        if (!loadTileset(e))
        {
            return false;
        }

        e = map.Child("layer", 0).ToElement();

        if (!loadLayer(e))
        {
            return false;
        }

        logger->logDebug("Finished loading map");

		return true;
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
            logger->logError("No tilesets defined!");
            return false;
        }

        e = e->FirstChild("image")->ToElement();
        if (!e)
        {
            logger->logError("No images for tilesets defined!");
            return false;
        }

        mTileset = e->Attribute("source");

        if (mTileset.empty())
        {
            logger->logError("No source for image");
            return false;
        }

        if (!graphicsEngine->loadTextureSet(mTileset, mTileWidth, mTileHeight))
        {
            logger->logError("Unable to load texture for map");
            return false;
        }

        return true;
	}

	bool Map::loadLayer(TiXmlElement *e)
	{
	    if (!e)
        {
            logger->logError("No layers");
            return false;
        }

        int layerWidth, layerHeight;

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

        addLayer(layerWidth, layerHeight, layerData, inflatedSize, 0);

        return true;
	}

	void Map::addLayer(unsigned int width, unsigned int height, unsigned char *data,
		unsigned int length, unsigned int layer)
    {
        Layer *l = new Layer(width, height);
        l->setData(data, length, mTileset);
        l->setDepth(layer);

        mLayers.push_back(l);
    }
}


