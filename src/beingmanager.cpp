/*********************************************
 *
 *	Author: David Athay
 *
 *	License: New BSD License
 *
 *	Copyright (c) 2009, The Small Towns Dev Team
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
 *	Date of file creation: 09-07-15
 *
 *	$Id$
 *
 ********************************************/

#include "beingmanager.h"
#include "being.h"

#include "graphics/graphics.h"

namespace ST
{
    BeingManager::BeingManager()
    {

    }

    BeingManager::~BeingManager()
    {
        BeingIterator itr = mBeingMap.begin(), itr_end = mBeingMap.end();
        while (itr != itr_end)
        {
            delete itr->second;
            ++itr;
        }
        mBeingMap.clear();
    }

    Being* BeingManager::findBeing(unsigned int id)
    {
        BeingIterator itr = mBeingMap.find(id);
        if (itr != mBeingMap.end())
        {
            return itr->second;
        }
        return NULL;
    }

    Being* BeingManager::findBeing(const std::string &name)
    {
        BeingIterator itr = mBeingMap.begin(), itr_end = mBeingMap.end();
        while (itr != itr_end)
        {
            if (itr->second->getName() == name)
            {
                return itr->second;
            }

            ++itr;
        }

        return NULL;
    }

    Being* BeingManager::findBeing(int x, int y)
    {
        BeingIterator itr = mBeingMap.begin(), itr_end = mBeingMap.end();
        while (itr != itr_end)
        {
            Point pt = itr->second->getTilePosition();
            if (pt.x == x && pt.y == y)
            {
                return itr->second;
            }

            ++itr;
        }

        return NULL;
    }

    void BeingManager::addBeing(Being *being)
    {
        mBeingMap.insert(std::pair<unsigned int, Being*>(being->getId(), being));
    }

    void BeingManager::saveBeingInfo(unsigned int id, const Point &finish, int dir)
    {
        BeingInfo info;
        info.finish = finish;
        info.dir = dir;
        mInfoMap.insert(std::pair<unsigned int, BeingInfo>(id, info));
    }

    Point BeingManager::getSavedDestination(unsigned int id)
    {
        Point pt;
        pt.x = 0;
        pt.y = 0;

        InfoIterator itr = mInfoMap.find(id);

        if (itr != mInfoMap.end())
        {
            pt = itr->second.finish;
        }

        return pt;
    }

    int BeingManager::getSavedDirection(unsigned int id)
    {
        int dir = -1;

        InfoIterator itr = mInfoMap.find(id);

        if (itr != mInfoMap.end())
        {
            dir = itr->second.dir;
        }

        return dir;
    }

    void BeingManager::removeBeing(unsigned int id)
    {
        BeingIterator itr = mBeingMap.find(id);
        if (itr != mBeingMap.end())
        {
            graphicsEngine->removeNode(itr->second);
            delete itr->second;
            mBeingMap.erase(itr);
        }
    }

    void BeingManager::logic(int ms)
    {
        BeingIterator itr = mBeingMap.begin(), itr_end = mBeingMap.end();
        while (itr != itr_end)
        {
            itr->second->logic(ms);
            ++itr;
        }
    }
}
