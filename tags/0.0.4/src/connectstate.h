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
 *	Date of file creation: 09-01-30
 *
 *	$Id$
 *
 ********************************************/

/**
 * The Connect State is used for connecting to a game server
 */

#ifndef ST_CONNECTSTATE_HEADER
#define ST_CONNECTSTATE_HEADER

#include "gamestate.h"

#include <agar/core.h>
#include <agar/gui.h>

namespace ST
{
	class ConnectState : public GameState
	{
	public:
		ConnectState();

		/**
		 * Enter
		 * Called when entering the state
		 */
		void enter();

		/**
		 * Exit
		 * Called when leaving the state
		 */
		void exit();

		/**
		 * Update
		 * Called every frame
		 * Return false to exit the game
		 */
		bool update();

    private:
		/**
		 * Do connections
		 * Connects to the server
		 */
        void doConnection();

    private:
        AG_Label *error;
	};
}

#endif