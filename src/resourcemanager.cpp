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

#include "graphics/animation.h"
#include "graphics/graphics.h"

#include "resources/bodypart.h"

#include "utilities/xml.h"

#include <physfs.h>
#include <sstream>

namespace ST
{
    ResourceManager::ResourceManager(const std::string &path)
    {
        mDefaultBody = 0;
        mDefaultHair = 0;
        mBodyWidth = 0;
        mBodyHeight = 0;
        // physfs code
        PHYSFS_init(path.c_str());

#ifndef __APPLE__
        mDataPath = "data/";
        PHYSFS_addToSearchPath("data", 0);
#else
        //TODO: Add OSX support
#endif
    }

    ResourceManager::~ResourceManager()
    {
        {
            BodyPartItr itr = mBodyParts.begin(), itr_end = mBodyParts.end();
            while (itr != itr_end)
            {
                delete (*itr);
                ++itr;
            }
            mBodyParts.clear();
        }

        {
            AnimationItr itr = mAnimations.begin(), itr_end = mAnimations.end();
            while (itr != itr_end)
            {
                delete itr->second;
                ++itr;
            }
            mAnimations.clear();
        }

        // remember to deinit physfs
        PHYSFS_deinit();
    }


    void ResourceManager::loadBodyParts(const std::string &filename)
    {
        XMLFile file;
		if (file.load(filename))
		{
		    // set size
		    mBodyWidth = file.readInt("size", "width");
		    mBodyHeight = file.readInt("size", "height");

            // set defaults
		    mDefaultBody = file.readInt("default", "body");
            mDefaultFemale = file.readInt("default", "female");
		    mDefaultHair = file.readInt("default", "hair");

            // add all the body parts
            do
            {
                int id = file.readInt("body", "id");
                std::string img = mDataPath + file.readString("body", "file");
                std::string icon = mDataPath + file.readString("body", "icon");
                int part = file.readInt("body", "part");

                BodyPart *body = new BodyPart(id, part, img, icon);

                mBodyParts.push_back(body);
            } while (file.next("body"));
		}
    }

    void ResourceManager::loadAnimations(const std::string &filename)
    {
        XMLFile file;
		if (file.load(filename))
		{
		    // add all the animations
            do
            {
                std::string name = file.readString("animation", "name");
                std::string img = mDataPath + file.readString("animation", "file");
                int frames = file.readInt("animation", "frames");
                int width = file.readInt("animation", "width");
                int height = file.readInt("animation", "height");

                if (graphicsEngine->loadTextureSet(name, img, width, height))
                {
                    Animation *anim = new Animation;
                    for (int i = 1; i <= frames; ++i)
                    {
                        std::stringstream str;
                        str << name << i;
                        anim->addTexture(graphicsEngine->getTexture(str.str()));
                    }
                    mAnimations.insert(std::pair<std::string, Animation*>(name, anim));
                }
            } while (file.next("animation"));
        }
    }

    int ResourceManager::getBodyWidth() const
    {
        return mBodyWidth;
    }

    int ResourceManager::getBodyHeight() const
    {
        return mBodyHeight;
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

    BodyPart* ResourceManager::getFemaleBody()
    {
        return getBodyPart(mDefaultFemale);
    }

    std::vector<BodyPart*> ResourceManager::getBodyList(int type)
    {
        std::vector<BodyPart*> vec;

        BodyPartItr itr = mBodyParts.begin(), itr_end = mBodyParts.end();
        while (itr != itr_end)
        {
            if ((*itr)->getType() == type)
            {
                vec.push_back(*itr);
            }

            ++itr;
        }

        return vec;
    }

    Animation* ResourceManager::getAnimation(const std::string &name)
    {
        AnimationItr itr = mAnimations.find(name);
        if (itr != mAnimations.end())
        {
            return itr->second;
        }

        return NULL;
    }

    std::string ResourceManager::getDataPath()
    {
        return mDataPath;
    }

    bool ResourceManager::doesExist(const std::string &filename)
    {
        return (PHYSFS_exists(filename.c_str()) != 0);
    }
}
