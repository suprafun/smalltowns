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

#include "utilities/base64.h"
#include "utilities/log.h"
#include "utilities/math.h"

#include <sstream>
#include <tinyxml.h>

namespace ST
{
	Layer::Layer()
	{

	}

	Layer::~Layer()
	{

	}

	void Layer::setData(char *data, Texture *texture)
	{

	}

	void Layer::setDepth(unsigned int depth)
	{

	}

	void Layer::addNode(Node *node)
	{

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
        TiXmlElement* e;
        TiXmlHandle map = hDoc.FirstChild("map");

        e = map.ToElement();
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

        e = map.Child("tileset", 0).ToElement();
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

        std::string tile = e->Attribute("source");

        if (tile.empty())
        {
            logger->logError("No source for image");
            return false;
        }

        graphicsEngine->loadTexture(tile);
        Texture *tex = graphicsEngine->getTexture(tile);

        e = map.Child("layer", 0).ToElement();
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

        //TODO: Load in compressed base64 map
        e = e->FirstChild("data")->ToElement();
        std::string data = e->GetText();
        if (data.empty())
        {
            logger->logError("No data");
            return false;
        }

        std::string layerData;

        // convert from base 64
        Base64::decode(data, layerData);

        if (layerData.empty())
        {
            logger->logError("Unable to convert from base64");
            return false;
        }

        logger->logDebug("Finished loading map");

		return true;
	}

/*	void Map::addTile(unsigned int tile, Texture *texture, unsigned int layer, bool blocking)
	{
		if (layer > mLayers.size())
		{
			logger->logError("Failed to add tile");
			return;
		}

		Tile *t = new Tile(tile, texture);
		t->setBlocking(blocking);

		mLayers[layer]->addNode(t);
	}
	*/
	void Map::addLayer(unsigned int width, unsigned int height, char *data,
                       Texture *texture, unsigned int layer)
    {
        Layer *layer = new Layer(width, height);
        layer->setData(data, texture);
        layer->setDepth(layer);

        mLayers.push_back(layer);
    }
}


