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
 *	Date of file creation: 09-02-04
 *
 *	$Id$
 *
 ********************************************/

#include "player.h"
#include "character.h"
#include "resourcemanager.h"

#include "graphics/graphics.h"
#include "graphics/texture.h"

#include "resources/bodypart.h"

#include <SDL/SDL.h>

namespace ST
{
    Player::Player()
    {
        mSelected = NULL;
    }

    Player::~Player()
    {
        removeCharacters();
    }

    std::string Player::getName() const
    {
        return mUsername;
    }

    void Player::setName(const std::string &name)
    {
        mUsername = name;
    }

    int Player::getId() const
    {
        return mId;
    }

    void Player::setId(int id)
    {
        mId = id;
    }

    void Player::addCharacter(Character *c)
    {
        std::vector<Character*>::iterator itr, itr_end;
        itr_end = mCharacters.end();
        for (itr = mCharacters.begin(); itr != itr_end; ++itr)
        {
            if ((*itr)->getName() == c->getName())
            {
                return;
            }
        }
        mCharacters.push_back(c);
    }

    int Player::getNumChars() const
    {
        return mCharacters.size();
    }

    Character* Player::getCharacter(unsigned int slot)
    {
        if (slot < mCharacters.size())
        {
            return mCharacters[slot];
        }

        return NULL;
    }

    void Player::removeCharacters()
    {
        for (size_t i = 0; i < mCharacters.size(); ++i)
        {
            delete mCharacters[i];
        }
        mCharacters.clear();
    }

    void Player::setCharacter(int slot)
    {
        if (slot < mCharacters.size())
        {
            mSelected = mCharacters[slot];
        }
    }

    void Player::createAvatar()
    {
        if (mSelected)
        {
            // create surface to render to
            SDL_Surface *surface = new SDL_Surface;

            Texture *hairTex = 0;
            Texture *bodyTex = 0;
            Texture *chestTex = 0;
            Texture *legsTex = 0;
            Texture *feetTex = 0;

            // all the body parts that make up the avatar
            BodyPart *hair = resourceManager->getBodyPart(mSelected->look.hair);
            BodyPart *body = resourceManager->getBodyPart(mSelected->look.body);
            BodyPart *chest = resourceManager->getBodyPart(mSelected->look.chest);
            BodyPart *legs = resourceManager->getBodyPart(mSelected->look.chest);
            BodyPart *feet = resourceManager->getBodyPart(mSelected->look.chest);

            // load all the textures
            if (hair)
                hairTex = hair->getTexture();
            if (body)
                bodyTex = body->getTexture();
            if (chest)
                chestTex = chest->getTexture();
            if (legs)
                legsTex = legs->getTexture();
            if (feet)
                feetTex = feet->getTexture();

            // write all the textures to the surface
            if (hairTex)
                SDL_BlitSurface(hairTex->getSDLSurface(), NULL, surface, NULL);
            if (bodyTex)
                SDL_BlitSurface(bodyTex->getSDLSurface(), NULL, surface, NULL);
            if (chestTex)
                SDL_BlitSurface(chestTex->getSDLSurface(), NULL, surface, NULL);
            if (legsTex)
                SDL_BlitSurface(legsTex->getSDLSurface(), NULL, surface, NULL);
            if (feetTex)
                SDL_BlitSurface(feetTex->getSDLSurface(), NULL, surface, NULL);

            mSelected->mTexture = new Texture("Player");
            mSelected->mTexture->setImage(surface);
        }
    }
}
