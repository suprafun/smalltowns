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
    // tile relative direction
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
        look.hair = 0;
        look.face = 0;
        look.body = 0;
        look.chest = 0;
        look.legs = 0;
        look.feet = 0;
        look.gender = 0;
        look.hairColour.r = 0;
        look.hairColour.g = 0;
        look.hairColour.b = 0;
        look.eyeColour.r = 0;
        look.eyeColour.g = 0;
        look.eyeColour.b = 0;
        look.skinColour.r = 0;
        look.skinColour.g = 0;
        look.skinColour.b = 0;

        mAnchor = 16;
    }

    Being::~Being()
    {
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
        mSetAnimation = NULL;

        // if name is empty, unset the animation
        if (name.empty())
        {
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
        for (unsigned int i = 0; i < body->getFrames() && i < hair->getFrames(); ++i)
        {
            Texture *tex = graphicsEngine->createAvatarFrame(mId, i+1, body->getTexture(), hair->getTexture(), mDirection);
            mSetAnimation->addTexture(tex);
            body->nextFrame();
            hair->nextFrame();
        }

        // set the update rate based on number of frames per second
        mUpdateTime = 1000 / mSetAnimation->getFrames();
    }

    bool Being::calculateNextDestination(const Point &finish)
    {
        int hops = 0;
        int dir = DIRECTION_NORTH;
        int hw = mWidth >> 1;
        int mapWidth = mapEngine->getTileWidth();
        int mapHeight = mapEngine->getTileHeight();
        int hmh = mapHeight >> 1;

        // set start point
        mLastPosition.x = mPosition.x;
        mLastPosition.y = mPosition.y;

        // empty any previous path
        mWaypoints.clear();

        Point wayPos = getTilePosition();
        Point endPos = finish;//mapEngine->convertPixelToTile(finish.x, finish.y);
        Point screenPos = {0,0};

        // keep moving a tile towards destination until reached
        while(hops < 20)
        {
            if (wayPos.x == endPos.x && wayPos.y == endPos.y)
            {
                break;
            }

            // find a direction to move closer to the end point
            dir = getDirection(wayPos, endPos);
            // find the tile inbetween
            wayPos = getNextTile(wayPos, dir);

            // translate to screen position and store that
            screenPos = mapEngine->convertTileToPixel(wayPos);
            screenPos.x += hw >> 1;
            screenPos.y += hmh;

            mWaypoints.push_back(screenPos);
            ++hops;
        }

        // commented out as sabata wants to end on the tile not the pixel
		//mWaypoints.push_back(finish);

        return (hops < 20);
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
        float speed = 25.0f;
        float distx = 0.0f;
        float disty = 0.0f;
        float distance = 0.0f;
        float time = 0.0f;

        // check theres waypoints
        if (mWaypoints.size() == 0)
        {
            return;
        }

        // check if reached way point
        if (mPosition.x == mWaypoints[0].x && mPosition.y == mWaypoints[0].y)
        {
            mWaypoints.pop_front();
            if (mWaypoints.size() == 0)
            {
                mDirection = -1;
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

        // avoid divide by 0
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

        // check if direction changed, so we can turn the being and change its animation
        Point srcTile = mapEngine->convertPixelToTile(movePos.x, movePos.y);
        Point destTile = mapEngine->convertPixelToTile(nextDest.x, nextDest.y);
        if (srcTile.x != destTile.x || srcTile.y != destTile.y)
        {
            mTileChanged = true;
            graphicsEngine->sort();
            int dir = getDirection(srcTile, destTile);
            if (mDirection != dir)
            {
                turnNode(dir);
                changeAnimation();
            }
        }
        else
        {
            mTileChanged = false;
        }

        mLastPosition = nextPos;
    }

    Point Being::getNextTile(const Point &pt, int dir)
    {
        Point pos = mapEngine->walkMap(pt, dir);
        int attempts = 2;
        int diff = 1;

        // keep trying a different direction till valid tile found
        // or until maximum number of attempts failed
        while (attempts > 0 && mapEngine->blocked(pos))
        {
            dir += diff;
            pos = mapEngine->walkMap(pt, dir);
            // count down so it doesnt loop forever
            --attempts;
            // make it try different direction next time
            diff = -diff;

            // check for greater or less than minimum and maximum direction
            if (dir < DIRECTION_NORTH)
                dir = DIRECTION_NORTHWEST;
            if (dir > DIRECTION_NORTHWEST)
                dir = DIRECTION_NORTH;
        }

        return pos;
    }

    void Being::saveDestination(const Point &pos)
    {
        mDestination = pos;
    }

    void Being::changeAnimation()
    {
        std::stringstream str;

        // create the string for new animation
        // check gender
        // check direction
        // check state
        // final string will look like maleSEwalk

        if (look.body == 1) // TODO: fix this, server should send gender
            str << "male";
        else
            str << "female";

        // direction is tile based
        switch (mDirection)
        {
            case DIRECTION_WEST:
                str << "NW";
                break;
            case DIRECTION_NORTH:
            case DIRECTION_NORTHWEST:
                str << "NE";
                break;
            case DIRECTION_SOUTHWEST:
            case DIRECTION_SOUTH:
                str << "SW";
                break;
            case DIRECTION_SOUTHEAST:
            case DIRECTION_EAST:
            case DIRECTION_NORTHEAST:
                str << "SE";
                break;

            default:
                str << "SE";
                break;
        }

        if (mState == STATE_MOVING)
            str << "walk";

        setAnimation(str.str());
    }

    bool Being::tileChanged() const
    {
        return mTileChanged;
    }
}
