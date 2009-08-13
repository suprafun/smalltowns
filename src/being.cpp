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

#include "graphics/animation.h"
#include "graphics/graphics.h"
#include "graphics/texture.h"

#include "resources/bodypart.h"

namespace ST
{
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
    }

    void Being::setAnimation(const std::string &name)
    {
        // if name is empty, unset the animation
        if (name.empty())
        {
            mSetAnimation = NULL;
            mUpdateTime = 0;
            return;
        }

        // delete any old animation that was set
        delete mSetAnimation;

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
}
