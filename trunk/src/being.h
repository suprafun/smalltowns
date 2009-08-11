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

/**
 * The Being class is used as a basis for Characters, and NPCs
 */

#ifndef ST_BEING_HEADER
#define ST_BEING_HEADER

#include "graphics/node.h"
#include "utilities/types.h"

#include <string>

namespace ST
{
    struct Look
    {
        int hair;
        int face;
        int body;
        int chest;
        int legs;
        int feet;
        int gender;
        Colour hairColour;
        Colour eyeColour;
        Colour skinColour;
    };

    class Texture;

    class Being : public AnimatedNode
    {
    public:
        Being(int id, const std::string &name, Texture *avatar);
        virtual ~Being();

        virtual std::string getName();
        virtual int getId() const;

        virtual void setLook(int body, int hair) { look.body = body; look.hair = hair; }
        virtual void setAnimation(const std::string &name);

        /**
         * Logic called each frame
         * @param ms Number of milliseconds since last frame
         */
        virtual void logic(int ms);

    public:
        Look look;

    protected:
        int mId;
    };
}

#endif
