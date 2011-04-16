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
 *	Date of file creation: 08-10-02
 *
 *	$Id$
 *
 ********************************************/

#include "camera.h"

#include <math.h>

namespace ST
{
	Camera::Camera(std::string name, ST::Rectangle *rect)
		: mName(name)
	{
		mViewport.height = rect->height;
		mViewport.width = rect->width;
		mViewport.x = rect->x;
		mViewport.y = rect->y;
        mDest.x = mViewport.x;
        mDest.y = mViewport.y;
	}

	const int Camera::getViewWidth() const
	{
		return mViewport.width;
	}

	const int Camera::getViewHeight() const
	{
		return mViewport.height;
	}

	Point Camera::getPosition()
	{
		Point p;
		p.x = mViewport.x;
		p.y = mViewport.y;
		return p;
	}

    void Camera::setPosition(const Point &pt)
    {
        mViewport.x = pt.x;
        mViewport.y = pt.y;
        mDest.x = mViewport.x;
        mDest.y = mViewport.y;
    }

    void Camera::setDestination(const Point &pt, int delay)
    {
        mPos.x = mViewport.x;
        mPos.y = mViewport.y;
        mDest = pt;
        mDelay = delay;
    }

    void Camera::logic(int ms)
    {
        if (mDest.x == mViewport.x && mDest.y == mViewport.y)
            return;

        mDelay -= ms;

        if (mDelay > 0)
            return;

        float speed = 60.0f;
        float time = 0.0f;

        // calculate next position by taking the last position and
        // destination and finding position after travelling since last frame
        float distx = mDest.x - mPos.x;
        float disty = mDest.y - mPos.y;

        // avoid divide by 0
        float distance = distx * distx + disty * disty;
        if (distance < 1.0f && distance > -1.0f)
        {
            mViewport.x = mDest.x;
            mViewport.y = mDest.y;
            return;
        }
        distance = sqrtf(distance);

        time = ms / 1000.0f;
        speed *= time;

        if (speed > distance)
            speed = distance;

        mPos.x = mPos.x + (distx / distance) * speed;
        mPos.y = mPos.y + (disty / distance) * speed;

        mViewport.x = static_cast<int>(mPos.x);
        mViewport.y = static_cast<int>(mPos.y);
    }
}

