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

/**
 * The Player class is for storing player specific variables
 */

#ifndef ST_PLAYER_HEADER
#define ST_PLAYER_HEADER

#include <string>
#include <map>

namespace ST
{
    class Character;

    class Player
    {
    public:
        /**
         * Constructor
         */
        Player();
        ~Player();

        /**
         * Returns name
         */
        std::string getName() const;

        /**
         * Sets the player's name
         */
        void setName(const std::string &name);

        /**
         * Gets the player's id
         */
        int getId() const;

        /**
         * Sets the player's id
         */
        void setId(int id);

        /**
         * Add a character to list player owns
         */
        void addCharacter(Character *c, int slot);

        /**
         * Return number of characters the player has
         */
        int getNumChars() const;

        /**
         * Return the character in the specified slot
         */
        Character* getCharacter(unsigned int slot);
        Character* getSelectedCharacter();

        /**
         * Removes all the currently added characters
         */
        void removeCharacters();

        /**
         * Set which character player will use to play
         */
        void setCharacter(int slot);

    private:
        std::string mUsername;
		std::map<int, Character*> mCharacters;
		Character *mSelected;
		int mId;
		typedef std::map<int, Character*> CharacterMap;
    };

    extern Player *player;
}

#endif
