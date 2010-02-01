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

#ifndef ST_RESOURCEMANAGER_HEADER
#define ST_RESOURCEMANAGER_HEADER

#include <list>
#include <map>
#include <string>
#include <vector>

namespace ST
{
    class Animation;
    class BeingAnimation;
    class BodyPart;

    /**
     * The Resource Manager is the class used for storing resources
     */
    class ResourceManager
    {
    public:
        ResourceManager(const std::string &path);
        ~ResourceManager();
        /**
         * Loads the body parts from file
         */
        void loadBodyParts(const std::string &filename);
        /**
         * Loads the animations from file
         */
        void loadAnimations(const std::string &filename);

        /**
         * Returns width of body parts
         */
        int getBodyWidth() const;
        /**
         * Returns height of body parts
         */
        int getBodyHeight() const;

        /**
         * Returns Body part based on id
         * @param id The id of the body part
         */
        BodyPart* getBodyPart(int id);
        /**
         * Returns the default body part based on type
         * @param type The type of body part
         */
        BodyPart* getDefaultBody(int type);
        BodyPart* getFemaleBody();
        /**
         * Returns a list of all body parts of a certain type
         * @param type The type of body part
         */
        std::vector<BodyPart*> getBodyList(int type);

        /**
         * Returns Animation based on body part id and animation name
         * @param bodypart Id of body part
         * @param name Name of animation
         */
        Animation* getAnimation(int bodypart, const std::string &name);

        /**
         * Returns the path to data directory
         */
        std::string getDataPath();

        /**
         * Check a file or path exists
         */
        bool doesExist(const std::string &filename);

    private:
        int mBodyWidth;
        int mBodyHeight;
        int mDefaultBody;
        int mDefaultFemale;
        int mDefaultHair;
        std::vector<BodyPart*> mBodyParts;
        typedef std::vector<BodyPart*>::iterator BodyPartItr;
        std::map<std::string, std::list<BeingAnimation*> > mAnimations;
        typedef std::map<std::string, std::list<BeingAnimation*> >::iterator AnimationItr;

        std::string mDataPath;
    };

    extern ResourceManager *resourceManager;
}

#endif
