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
 *	Date of file creation: 09-07-14
 *
 *	$I$
 *
 ********************************************/

/**
 * The Being Manager class stores all the beings the player knows about
 */

#ifndef ST_BEINGMANAGER_HEADER
#define ST_BEINGMANAGER_HEADER

#include "utilities/types.h"

#include <map>

namespace ST
{
    class Being;
    class BeingManager
    {
    public:
        BeingManager();
        ~BeingManager();

        /**
         * Return a being based on matching id
         */
        Being* findBeing(unsigned int id);

        /**
         * Add a being
         */
        void addBeing(Being *being);

        /**
         * Save being position
         */
        void saveBeingPosition(unsigned int id, int x, int y);

        /**
         * Get saved position
         */
        Point getSavedPosition(unsigned int id);

    private:
        std::map<unsigned int, Point> mPositionMap;
        std::map<unsigned int, Being*> mBeingMap;
        typedef std::map<unsigned int, Being*>::iterator BeingIterator;
        typedef std::map<unsigned int, Point>::iterator PositionIterator;
    };

    extern BeingManager *beingManager;
}

#endif
