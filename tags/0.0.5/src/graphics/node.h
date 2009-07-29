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
 * The node class is used by everything that will be displayed on screen
 */

#ifndef ST_NODE_HEADER
#define ST_NODE_HEADER

#include <SDL_opengl.h>
#include <string>

#include "../utilities/types.h"

namespace ST
{
	class Texture;

	class Node
	{
	private:
		/**
		 * Default constructor
		 * Not to be used
		 */
		Node();
	public:
		/**
		 * Constructor
		 * @param name The name of the node
		 * @param texture The name of the texture
		 */
		Node(std::string name, Texture *texture);
		virtual ~Node();

		/**
		 * Get Position
		 * @return Returns the position of the node
		 */
		Point& getPosition();
		Point getTilePosition();

		/**
		 * Get Width
		 * @return Returns the width of the node
		 */
		const int getWidth() const;

		/**
		 * Get Height
		 * @return Returns the height of the node
		 */
		const int getHeight() const;

		/**
		 * Get Bounds
		 * @return Returns a rectangle with the position, and size
		 */
		Rectangle& getBounds();

		/**
		 * Get Visible
		 * @return Returns if the node is visible
		 */
		bool getVisible() const;

		/**
		 * Set Visible
		 * @param visible Sets if the node will be drawn
		 */
		void setVisible(bool visible);

		/**
		 * Move Node
		 * Moves the node to a new position
		 * @param position The new position for the node to be
		 */
		virtual void moveNode(Point *position);

		/**
		 * Get GL Texture
		 * @return Returns the GL Texture for drawing
		 */
		virtual Texture* getTexture();

		/**
		 * Toggle Name
		 * Toggles whether the name of the node is shown
         */
        void toggleName();

        /**
         * Show Name
         * Returns whether to show the name
         */
        bool showName();

        /**
         * Get Name
         * Returns the name of the node
         */
        std::string getName() const;

	protected:
		std::string mName;
		Texture *mTexture;
		Point mPosition;
		Rectangle mBounds;
		int mWidth;
		int mHeight;
		bool mVisible;
		bool mShowName;
	};
}

#endif
