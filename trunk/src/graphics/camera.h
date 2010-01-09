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
 *	Date of file creation: 08-09-21
 *
 *	$Id$
 *
 ********************************************/

/**
 * The camera class is used to display to the screen
 */

#ifndef ST_CAMERA_HEADER
#define ST_CAMERA_HEADER

#include "../utilities/types.h"

#include <string>

namespace ST
{
	class Camera
	{
	protected:
		Camera();

	public:
		/**
		 * Constructor
		 * @param name The name of the camera
		 * @param rect Rectangle representing the viewport to display
		 */
		Camera(std::string name, Rectangle *rect);

		/**
		 * Get Position
		 * @return Returns the offset from the map of the camera
		 */
		Point getPosition();

        /**
         * Set Position
         * Move the camera
         * @param pos Position to move the camera
         */
        void setPosition(const Point &pt);

        /**
         * Set Destination
         * Move camera smoothly
         * @param pt The position to move to
         * @param delay The number of milliseconds before starting to move
         */
        void setDestination(const Point &pt, int delay);

		/**
		 * Get View Width
		 * @return Returns the width of the viewport
		 */
		const int getViewWidth() const;

		/**
		 * Get View Height
		 * @return Returns the height of the viewport
		 */
		const int getViewHeight() const;

		/**
		 * Get View Bounds
		 * @return Returns the viewport bounds as a rectangle
		 */
		Rectangle& getViewBounds() { return mViewport; }

		/**
		 * Logic
		 * called each frame to move cam to destination
		 * @param ms Number of milliseconds passed since last called
		 */
        void logic(int ms);

	private:
		std::string mName;
		Rectangle mViewport;
		Point mDest;
		Pointf mPos;
		int mDelay;
	};
}

#endif
