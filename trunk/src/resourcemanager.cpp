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
#include "map.h"

#include "graphics/animation.h"
#include "graphics/graphics.h"

#include "resources/bodypart.h"

#include "utilities/log.h"
#include "utilities/xml.h"

#include <physfs.h>
#include <sstream>
#ifdef __APPLE__
#include <CoreFoundation/CFBundle.h>
#endif

namespace ST
{
    ResourceManager::ResourceManager(const std::string &path)
    {
        mDefaultBody = 0;
        mDefaultHair = 0;
        mBodyWidth = 0;
        mBodyHeight = 0;
        std::string datapath = "";
        // physfs code
        PHYSFS_init(path.c_str());

#if defined __unix__
        mDataPaths.push_back("data/");
        mDataPaths.push_back("");
		mWriteDataPath = "";
        PHYSFS_addToSearchPath("data", 0);
#elif defined __APPLE__
        CFBundleRef mainBundle = CFBundleGetMainBundle();
		CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
		char resPath[PATH_MAX];
		CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)resPath, PATH_MAX);
		CFRelease(resourcesURL);
		datapath = resPath;
		datapath.append("/");
		PHYSFS_addToSearchPath(resPath, 0);

		mWriteDataPath = PHYSFS_getUserDir();
		mWriteDataPath.append("Library/Application Support/townslife/");
		mDataPaths.push_back(mWriteDataPath + "data/");
		mDataPaths.push_back(datapath);
#elif defined _WIN32
        mWriteDataPath = PHYSFS_getUserDir();
        mWriteDataPath.append("Documents/townslife/");
        mDataPaths.push_back(mWriteDataPath + "data/");
        mDataPaths.push_back("data/");
        mDataPaths.push_back("");
#endif
        if (!doesExist(mWriteDataPath))
        {
            PHYSFS_setWriteDir(PHYSFS_getUserDir());
			PHYSFS_mkdir(mWriteDataPath.c_str());
			PHYSFS_setWriteDir(mWriteDataPath.c_str());
        }
        PHYSFS_addToSearchPath(mWriteDataPath.c_str(), 0);
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
                std::list<BeingAnimation*>::iterator bitr = itr->second.begin(),
                                                 bitr_end = itr->second.end();
                while (bitr != bitr_end)
                {
                    delete *bitr;
                    ++bitr;
                }
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
		    file.setElement("size");
		    mBodyWidth = file.readInt("size", "width");
		    mBodyHeight = file.readInt("size", "height");

            // set defaults
            file.setElement("default");
		    mDefaultBody = file.readInt("default", "body");
            mDefaultFemale = file.readInt("default", "female");
		    mDefaultHair = file.readInt("default", "hair");

            // add all the body parts
            file.setElement("body");
            do
            {
                file.setSubElement("body", "image");
                int id = file.readInt("body", "id");
                std::string icon = getDataPath(file.readString("body", "icon"));
                int part = file.readInt("body", "part");

                BodyPart *body = new BodyPart(id, part, icon);

                do
                {
                    int dir = -1;
                    std::string img = getDataPath(file.readString("image", "file"));
                    std::string dirstr = file.readString("image", "dir");

                    if (dirstr == "SE")
                        dir = DIRECTION_SOUTHEAST;
                    else if (dirstr == "SW")
                        dir = DIRECTION_SOUTHWEST;
                    else if (dirstr == "NE")
                        dir = DIRECTION_NORTHEAST;
                    else if (dirstr == "NW")
                        dir = DIRECTION_NORTHWEST;

                    body->addTexture(dir, img);
                } while (file.nextSubElement("image"));

                mBodyParts.push_back(body);
                file.clear("image");

            } while (file.nextElement("body"));
		}
    }

    void ResourceManager::loadAnimations(const std::string &filename)
    {
        XMLFile file;
		if (file.load(filename))
		{
		    // add all the animations
		    file.setElement("animation");
            do
            {
                file.setSubElement("animation", "body");

                int id = file.readInt("animation", "id");
                std::string name = file.readString("animation", "name");
                int frames = file.readInt("animation", "frames");
                int width = file.readInt("animation", "width");
                int height = file.readInt("animation", "height");

                // get list of animations for each body part
                std::list<BeingAnimation*> animList;
                do
                {
                    std::string img = getDataPath(file.readString("body", "file"));
                    int part = file.readInt("body", "part");

                    std::stringstream texName;
                    texName << part << name;

                    // load in all the frames of animation
                    if (graphicsEngine->loadTextureSet(texName.str(), img, width, height))
                    {
                        BeingAnimation *anim = new BeingAnimation(id, part);
                        for (int i = 1; i <= frames; ++i)
                        {
                            std::stringstream str;
                            str << texName.str() << i;
                            anim->addTexture(graphicsEngine->getTexture(str.str()));
                        }
                        animList.push_back(anim);
                    }
                } while (file.nextSubElement("body"));

                mAnimations.insert(std::pair<std::string, std::list<BeingAnimation*> >(name, animList));
                file.clear("body");

            } while (file.nextElement("animation"));
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

    Animation* ResourceManager::getAnimation(int part, const std::string &name)
    {
        AnimationItr itr = mAnimations.find(name);
        if (itr != mAnimations.end())
        {
            std::list<BeingAnimation*>::iterator bitr = itr->second.begin(),
                                                 bitr_end = itr->second.end();
            while (bitr != bitr_end)
            {
                if ((*bitr)->getPart() == part)
                {
                    return *bitr;
                }
                ++bitr;
            }
        }

        return NULL;
    }

    std::string ResourceManager::getDataPath(std::string file)
    {
        for (unsigned int i = 0; i < mDataPaths.size(); ++i)
        {
            if (doesExist(mDataPaths[i] + file))
            {
                return mDataPaths[i] + file;
            }
        }
        return "";
    }

	std::string ResourceManager::getWritablePath()
	{
		return mWriteDataPath;
	}

    bool ResourceManager::doesExist(const std::string &filename)
    {
        return (PHYSFS_exists(filename.c_str()) != 0);
    }
}
