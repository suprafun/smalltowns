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
 *	Date of file creation: 08-10-02
 *
 *	$Id$
 *
 ********************************************/

#include "node.h"
#include "texture.h"

#include "../utilities/log.h"

namespace ST
{
	Node::Node(std::string name, Texture *texture)
		: mName(name),
		mVisible(true)
	{
		if (texture)
		{
			texture->increaseCount();
			mTexture = texture;
			mHeight = mTexture->getHeight();
			mWidth = mTexture->getWidth();
		}
		else
		{
			logger->logError("Invalid texture assigned to node");
		}

		mPosition.x = 0;
		mPosition.y = 0;
	}

	Node::~Node()
	{
		if (mTexture)
			mTexture->remove();
	}

	bool Node::getVisible() const
	{
		return mVisible;
	}

	const int Node::getHeight() const
	{
		return mHeight;
	}

	const int Node::getWidth() const
	{
		return mWidth;
	}

	Point& Node::getPosition()
	{
		return (mPosition);
	}

	void Node::moveNode(Point *position)
	{
		mPosition.x = position->x;
		mPosition.y = position->y;
	}

	GLuint Node::getGLTexture()
	{
		return mTexture->getGLTexture();
	}
}
