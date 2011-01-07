/*********************************************
 *
 *	Author: David Athay
 *
 *	License: New BSD License
 *
 *	Copyright (c) 2009, CT Games
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
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shlobj.h>
#endif

namespace ST
{
    ResourceManager::ResourceManager(const std::string &path)
    {
        mDefaultBody = 0;
        mDefaultFemale = 0;
        mDefaultHair = 0;
        mDefaultChest = 0;
        mDefaultLegs = 0;
        mDefaultFeet = 0;
        mBodyWidth = 0;
        mBodyHeight = 0;
        mNumParts = 5; // TODO: Calculate based on body.cfg
        std::string datapath = "";
        std::string error;

        // physfs code
        PHYSFS_init(path.c_str());

        // add paths
        // writable first, since thats where updates will go to
#if defined __unix__
        mWriteDataPath = PHYSFS_getUserDir();
#elif defined __APPLE__
		mWriteDataPath = PHYSFS_getUserDir();
		mWriteDataPath.append("Library/Application Support");
#elif defined _WIN32
        TCHAR writePath[MAX_PATH];
        SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, writePath);
        mWriteDataPath = writePath;
#endif
        PHYSFS_setWriteDir(mWriteDataPath.c_str());
        if (!doesExist("townslife"))
        {
			if (PHYSFS_mkdir("townslife") == 0)
			{
			    error = PHYSFS_getLastError();
			}
			PHYSFS_setWriteDir("townslife");
        }

#ifndef __unix__
        mWriteDataPath.append("/");
#else
        mWriteDataPath.append(".");
#endif
        mWriteDataPath.append("townslife/");
        addPath(mWriteDataPath);

        // now add cfg and /data directory
#if defined __unix__
        datapath = PHYSFS_getBaseDir() + "data/";
        addPath(datapath);
#elif defined __APPLE__
        CFBundleRef mainBundle = CFBundleGetMainBundle();
		CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
		char resPath[PATH_MAX];
		CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)resPath, PATH_MAX);
		CFRelease(resourcesURL);
		addPath(resPath);
#elif defined _WIN32
        TCHAR exePath[MAX_PATH];
        GetModuleFileName(0, exePath, MAX_PATH);
        datapath = exePath;
        datapath = datapath.substr(0, datapath.find_last_of("\\") + 1);
        addPath(datapath);
        addPath(datapath + "data\\");
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
        int size;
        char *data = loadFile(filename, size);

		if (data && file.parse(data))
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
		    mDefaultChest = file.readInt("default", "chest");
		    mDefaultLegs = file.readInt("default", "legs");
		    mDefaultFeet = file.readInt("default", "feet");

            // add all the body parts
            file.setElement("body");
            do
            {
                file.setSubElement("image");
                int id = file.readInt("body", "id");
                std::string icon = file.readString("body", "icon");
                int part = file.readInt("body", "part");
                std::string colour = file.readString("body", "colour");

                Texture *iconTex = NULL;
                if (getDataPath(icon).find_first_of(".zip") == std::string::npos)
                {
                    iconTex = graphicsEngine->loadTexture(getDataPath(icon));
                }
                else
                {
                    int iconBufSize = 0;
                    char *buffer = loadFile(icon, iconBufSize);
                    iconTex = graphicsEngine->loadTexture(icon, buffer, iconBufSize);
                    free(buffer);
                }

                if (iconTex == NULL)
                {
                    logger->logError("Unable to load icon: " + icon);
                }

                BodyPart *body = new BodyPart(id, part, iconTex);

                do
                {
                    int dir = -1;
                    // check if img is in a content update
                    std::string img = file.readString("image", "file");
                    std::string dirstr = file.readString("image", "dir");

                    if (dirstr == "SE")
                        dir = DIRECTION_SOUTHEAST;
                    else if (dirstr == "SW")
                        dir = DIRECTION_SOUTHWEST;
                    else if (dirstr == "NE")
                        dir = DIRECTION_NORTHEAST;
                    else if (dirstr == "NW")
                        dir = DIRECTION_NORTHWEST;

                    if (getDataPath(img).find_first_of(".zip") == std::string::npos)
                    {
                        body->addTexture(dir, getDataPath(img));
                    }
                    else
                    {
                        int imgBufSize = 0;
                        char *buffer = loadFile(img, imgBufSize);
                        body->addTexture(dir, img, buffer, imgBufSize);
                        free(buffer);
                    }

                } while (file.nextSubElement("image"));

                mBodyParts.push_back(body);
                file.clear("image");

            } while (file.nextElement("body"));
		}
    }

    void ResourceManager::loadAnimations(const std::string &filename)
    {
        XMLFile file;
        int size;
        bool loaded = false;
        char *data = loadFile(filename, size);

		if (data && file.parse(data))
		{
		    // add all the animations
		    file.setElement("animation");
            do
            {
                file.setSubElement("body");

                int id = file.readInt("animation", "id");
                std::string name = file.readString("animation", "name");
                int frames = file.readInt("animation", "frames");
                int width = file.readInt("animation", "width");
                int height = file.readInt("animation", "height");

                // get list of animations for each body part
                std::list<BeingAnimation*> animList;
                do
                {
                    std::string img = file.readString("body", "file");
                    int part = file.readInt("body", "part");

                    std::stringstream texName;
                    texName << part << name;

                    // check if animation is in content update
                    if (getDataPath(img).find_first_of(".zip") == std::string::npos)
                    {
                        img = getDataPath(img);
                        loaded = graphicsEngine->loadTextureSet(texName.str(), img, width, height);
                    }
                    else
                    {
                        int imgBufSize = 0;
                        char *buffer = loadFile(img, imgBufSize);
                        loaded = graphicsEngine->loadTextureSet(texName.str(), buffer, imgBufSize, width, height);
                        free(buffer);
                    }

                    // load in all the frames of animation
                    if (loaded)
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

    unsigned int ResourceManager::numBodyParts() const
    {
        return mNumParts;
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
        case PART_CHEST:
            return getBodyPart(mDefaultChest);
        case PART_LEGS:
            return getBodyPart(mDefaultLegs);
        case PART_FEET:
            return getBodyPart(mDefaultFeet);
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

    void ResourceManager::addPath(const std::string &path)
    {
        if (PHYSFS_addToSearchPath(path.c_str(), 0) == 0)
        {
            logger->logError("Unable to add path: " + path);
            logger->logError(PHYSFS_getLastError());
        }
    }

    std::string ResourceManager::getDataPath(std::string file)
    {
        if (doesExist(file))
        {
            std::string path = PHYSFS_getRealDir(file.c_str());
#ifndef _WIN32
            return path + "/" + file;
#else
            return path + file;
#endif
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

    char* ResourceManager::loadFile(const std::string &filename, int &size)
    {
        PHYSFS_file *file = PHYSFS_openRead(filename.c_str());
        if (file == NULL || filename.empty())
        {
            logger->logError("Invalid file: " + filename);
            logger->logError(PHYSFS_getLastError());
            return NULL;
        }

        size = PHYSFS_fileLength(file);
        char *buffer = (char*)malloc(size + 1);
        PHYSFS_read(file, buffer, 1, size);
        PHYSFS_close(file);
        buffer[size] = '\0';

        return buffer;
    }

    void ResourceManager::loadGlowingTiles()
    {
        int size = 0;
        char *buffer;
        if (getDataPath("glowtile_red").find_first_of(".zip") == std::string::npos)
        {
            graphicsEngine->loadTexture(getDataPath("glowtile_red.png"));
        }
        else
        {
            buffer = loadFile("glowtile_red.png", size);
            graphicsEngine->loadTexture("glowtile_red.png", buffer, size);
            free(buffer);
        }

        if (getDataPath("glowtile_green").find_first_of(".zip") == std::string::npos)
        {
            graphicsEngine->loadTexture(getDataPath("glowtile_green.png"));
        }
        else
        {
            size = 0;
            buffer = loadFile("glowtile_green.png", size);
            graphicsEngine->loadTexture("glowtile_green.png", buffer, size);
            free(buffer);
        }
    }
}
