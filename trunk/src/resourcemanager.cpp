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
 *	Date of file creation: 09-07-01
 *
 *	$Id$
 *
 ********************************************/

#include "resourcemanager.h"

#include "resources/bodypart.h"

namespace ST
{
    ResourceManager::~ResourceManager()
    {
        BodyPartItr itr = mBodyParts.begin(), itr_end = mBodyParts.end();
        while (itr != itr_end)
        {
            delete (*itr);
            ++itr;
        }
        mBodyParts.clear();
    }


    void ResourceManager::loadBodyParts(const std::string &filename)
    {
        XMLFile file;
		if (file.load(filename))
		{
            // set defaults
		    mDefaultBody = file.readInt("default", "body"));
		    mDefaultHair = file.readInt("default", "hair"));

            // add all the body parts
            do
            {
                int id = file.readInt("body", "id");
                std::string file = file.readString("body", "file");
                std::string icon = file.readString("body", "icon");
                int part = file.readInt("body", "part");

                BodyPart *body = new BodyPart(id, part, file, icon);

                mBodyParts.push_back(body);
            } while (file.next("body"));
		}
    }

    BodyPart* ResourceManager::getBodyPart(int id)
    {
        BodyPartItr itr = mBodyParts.begin(), itr_end = mBodyParts.end();
        while (itr != itr_end)
        {
            if ((*itr)->getId() == id)
            {
                return *itr;
            }

            ++itr;
        }

        return NULL;
    }

    BodyPart* ResourceManager::getDefaultBody(int type)
    {
        switch (type)
        {
        case PART_BODY:
            return getBodyPart(mDefaultBody);
        case PART_HAIR:
            return getBodyPart(mDefaultHair);
        }

        return NULL;
    }

    int ResourceManager::getNumberOfBody(int type)
    {
        int count = 0;

        BodyPartItr itr = mBodyParts.begin(), itr_end = mBodyParts.end();
        while (itr != itr_end)
        {
            if ((*itr)->getType() == type)
            {
                ++count;
            }

            ++itr;
        }

        return count;
    }
}
