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

/**
 * The Body Part class stores different body parts
 */

#ifndef ST_BODYPART_HEADER
#define ST_BODYPART_HEADER

#include <string>
#include <map>

namespace ST
{
    enum { PART_BODY = 0, PART_HAIR, PART_CHEST, PART_LEGS, PART_FEET };

    class Texture;
    class BodyPart// : class GameObject
    {
    public:
        BodyPart(int id, int type, Texture *tex);
        void addTexture(int dir, const std::string &filename);
        void addTexture(int dir, const std::string &filename, char *data, int size);
        Texture* getTexture(int dir) { return mTextures[dir]; }
        Texture* getIcon() { return mIconTexture; }
        int getId() { return mId; }
        int getType() { return mType; }

    private:
        int mId;
        int mType;
        std::map<int, Texture*> mTextures;
        Texture *mIconTexture;
    };
}

#endif
