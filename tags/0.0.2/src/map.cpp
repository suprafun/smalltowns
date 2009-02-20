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

#include "graphics/node.h"

#include "utilities/log.h"
#include "utilities/math.h"

#include <sstream>

namespace ST
{
	Layer::Layer()
	{

	}

	Layer::~Layer()
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

	}

	Map::~Map()
	{

	}

	bool Map::load(const std::string &filename)
	{
		// Open the file
		mFile.open(filename.c_str(), std::fstream::in);
		
		//Check the file opened successfully
		if (!mFile.is_open())
		{
			logger->logError("Error opening map file");
			return false;
		}

		// TODO: Read in the file

		mFile.close();

		return true;
	}

	void Map::addTile(unsigned int tile, Texture *texture, unsigned int layer, bool blocking)
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
}


