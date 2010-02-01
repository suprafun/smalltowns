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

#include "net/networkmanager.h"

namespace ST
{
    Player::Player()
    {
        mSelected = NULL;
        mLastUpdate = 0;
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

    void Player::addCharacter(Character *c, int slot)
    {
        CharacterMap::iterator itr = mCharacters.find(slot);
        if (itr != mCharacters.end())
        {
            return;
        }
        mCharacters.insert(std::pair<int, Character*>(slot, c));
    }

    int Player::getNumChars() const
    {
        return mCharacters.size();
    }

    Character* Player::getCharacter(unsigned int slot)
    {
        CharacterMap::iterator itr = mCharacters.find(slot);
        if (itr != mCharacters.end())
        {
            return itr->second;
        }

        return NULL;
    }

    void Player::removeCharacters()
    {
        CharacterMap::iterator itr = mCharacters.begin(), itr_end = mCharacters.end();

        while (itr != itr_end)
        {
            delete itr->second;
            ++itr;
        }
        mCharacters.clear();
    }

    void Player::setCharacter(int slot)
    {
        CharacterMap::iterator itr = mCharacters.find(slot);
        if (itr != mCharacters.end())
        {
            mSelected = itr->second;
        }
    }

    void Player::logic(int ms)
    {
        if (!mSelected)
            return;
        mLastUpdate += ms;

        if (mLastUpdate > 200)
        {
            if (mSelected->getState() == STATE_MOVING)
                networkManager->sendPositionUpdate(mSelected->getPosition());
            mLastUpdate = 0;
        }
    }
}
