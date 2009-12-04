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
 *	Date of file creation: 09-02-25
 *
 *	$Id$
 *
 ********************************************/

#include "being.h"
#include "resourcemanager.h"
#include "map.h"

#include "graphics/animation.h"
#include "graphics/graphics.h"
#include "graphics/texture.h"

#include "resources/bodypart.h"

#include "utilities/log.h"

#include <sstream>
#include <cmath>

namespace ST
{

    int getDirection(const Point &start, const Point &end)
    {
        int x = start.x - end.x;
        int y = start.y - end.y;

        if (x > 0)
        {
            if (y > 0)
                return DIRECTION_NORTHWEST;
            else if (y < 0)
                return DIRECTION_SOUTHWEST;

            return DIRECTION_WEST;
        }
        else if (x < 0)
        {
            if (y > 0)
                return DIRECTION_NORTHEAST;
            else if (y < 0)
                return DIRECTION_SOUTHEAST;

            return DIRECTION_EAST;
        }

        if (y > 0)
            return DIRECTION_NORTH;

        return DIRECTION_SOUTH;

    }

    Being::Being(int id, const std::string &name, Texture *avatar):
        AnimatedNode(name, avatar), mId(id)
    {
    }

    Being::~Being()
    {
        delete mSetAnimation;
    }

    std::string Being::getName()
    {
        return mName;
    }

    int Being::getId() const
    {
        return mId;
    }

    void Being::logic(int ms)
    {
        AnimatedNode::logic(ms);

        //TODO: Being processing
        switch (mState)
        {
        case STATE_IDLE:
            break;
        case STATE_MOVING:
            if (mPosition.x == mDestination.x &&
                mPosition.y == mDestination.y)
            {
                setAnimation("");
                setState(STATE_IDLE);
                return;
            }

            move(ms);
            break;
        }
    }

    void Being::setState(int state)
    {
        mState = state;
    }

    int Being::getState()
    {
        return mState;
    }

    void Being::setAnimation(const std::string &name)
    {
        // delete any old animation that was set
        delete mSetAnimation;

        // if name is empty, unset the animation
        if (name.empty())
        {
            mSetAnimation = NULL;
            mUpdateTime = 0;
            return;
        }

        // get existing animation
	    Animation *body = resourceManager->getAnimation(look.body, name);
	    if (!body)
            return;

        // get existing animation
	    Animation *hair = resourceManager->getAnimation(look.hair, name);
	    if (!hair)
            return;

        // create new animation to store the new frames
        mSetAnimation = new Animation;

        // create new frames and store them in the animation
        for (unsigned int i = 0; i < body->getFrames(); ++i)
        {
            Texture *tex = graphicsEngine->createAvatarFrame(mId, i, body->getTexture(), hair->getTexture());
            mSetAnimation->addTexture(tex);
            body->nextFrame();
            hair->nextFrame();
        }

        mUpdateTime = 1000 / mSetAnimation->getFrames();
    }

    bool Being::calculateNextDestination(const Point &finish)
    {
        bool found = false;
        Point start;
        Point end = mapEngine->getMapPosition(finish);
        Point pt = mapEngine->getMapPosition(mPosition);
        Point mapPos;
        Point screenPos;
        int dir = DIRECTION_NORTH;
        std::stringstream str;

        mLastPosition.x = mPosition.x;
        mLastPosition.y = mPosition.y;

        std::vector<int> scores;

        start.x = mapPos.x = screenPos.x = 0;
        start.y = mapPos.y = screenPos.y = 0;

        // move map position to where being is
        while (start.x != pt.x || start.y != pt.y)
        {
            dir = getDirection(start, pt);
            mapPos = mapEngine->walkTile(mapPos, dir);
            start = mapEngine->walkMap(start, dir);
        }

        str << "Being tile position " << mapPos.x << "," << mapPos.y << std::endl;
        str << "Being map position " << start.x << "," << start.y << std::endl;
        str << "Destination map position " << end.x << "," << end.y << std::endl;

        // keep moving a tile towards destination until reached
        while(!found)
        {
            dir = getDirection(start, end);
            mapPos = mapEngine->walkTile(mapPos, dir);
            start = mapEngine->walkMap(start, dir);

            if (start.x == end.x && start.y == end.y)
            {
                found = true;
            }

            screenPos.x = 0.5 * (mapPos.x - mapPos.y) * mapEngine->getTileWidth();
            screenPos.y = 0.5 * (mapPos.x + mapPos.y) * mapEngine->getTileHeight();

            //str << "Direction was " << dir << std::endl;
            //str << "Next waypoint is " << screenPos.x << "," << screenPos.y << std::endl;

            mWaypoints.push_back(screenPos);
        }

		mWaypoints.push_back(finish);

        str << "Destination tile position " << mapPos.x << "," << mapPos.y;

        logger->logDebug(str.str());

        return found;
    }

    bool Being::calculateNextDestination()
    {
        return calculateNextDestination(mDestination);
    }

    void Being::move(int ms)
    {
        Pointf nextPos;
        Point nextDest;
        Point movePos;
        float speed = 12.0f;
        float distx = 0.0f;
        float disty = 0.0f;
        float distance = 0.0f;
        float time = 0.0f;
        float length = 0.0f;

        // check theres waypoints
        if (mWaypoints.size() == 0)
        {
            return;
        }

        // check if reached way point
        if (mPosition.x == mWaypoints[0].x && mPosition.y == mWaypoints[0].y)
        {
            std::stringstream str;
            str << "Player reached " << mPosition.x << "," << mPosition.y;
            logger->logDebug(str.str());
            mWaypoints.pop_front();
            if (mWaypoints.size() == 0)
            {
                setAnimation("");
                setState(STATE_IDLE);
                return;
            }
        }

        // set the next destination
        nextDest = mWaypoints[0];

        // calculate next position by taking the last position and
        // destination and finding position after travelling since last frame
        distx = nextDest.x - mLastPosition.x;
        disty = nextDest.y - mLastPosition.y;

        distance = distx * distx + disty * disty;
        if (distance < 1 && distance > -1)
        {
            mPosition.x = mWaypoints[0].x;
            mPosition.y = mWaypoints[0].y;
            return;
        }
        distance = sqrtf(distance);

        time = ms / 1000.0f;
        speed *= time;

        if (speed > distance)
            speed = distance;

        nextPos.x = mLastPosition.x + (distx / distance) * speed;
        nextPos.y = mLastPosition.y + (disty / distance) * speed;


        movePos.x = nextPos.x;
        movePos.y = nextPos.y;

        moveNode(&movePos);

        mLastPosition = nextPos;
    }

    void Being::saveDestination(const Point &pos)
    {
        mDestination = pos;
    }
}
