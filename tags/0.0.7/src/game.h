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
 *	Date of file creation: 08-09-20
 *
 *	$Id$
 *
 ********************************************/

/**
 * \mainpage Towns Life
 *
 * \section intro_sec Introduction
 * This is the documentation for Towns Life
 * It is constantly being worked on.
 * You can help by submitting patches to http://code.google.com/p/smalltowns
 */

#ifndef ST_GAME_HEADER
#define ST_GAME_HEADER

#include <string>

#include <string>

namespace ST
{
	class GameState;

    /**
     * The Game class controls the graphics, input, sound and networking
     * It loops until the user exits
     */
	class Game
	{
	public:
        Game(const std::string &path);

		/**
		 * Destructor
		 *
		 * Cleans up Game
		 */
		~Game();

		/**
		 * Re-start
		 *
		 * Kills renderering so it can run again, used for changing graphics options
		 */
        void restart(int opengl, int fullscreen, int x, int y);

		/**
		 * Run
		 *
		 * Creates everything needed to run the game
		 */
		void run();

		/**
		 * Loop
		 *
		 * Loops indefinitely until quit
		 */
        void loop();

		/**
		 * Change State
		 *
		 * Changes which state the client is in
		 * @param state The gamestate to change to
		 */
        void changeState(GameState *state);

        /**
         * Disconnects from a server
         *
         * This actually just sends a disconnect packet
         * since its using connection-less UDP packets
         */
        void disconnect();

        /**
         * Connect to a server
         *
         * This actually just sends a message,
         * as its using connection-less UDP packets
         * @param server The hostname of the server
         * @param port The port of the server
         */
        bool connect(const std::string &server, int port);

    private:
        void cleanUp();

	private:
		GameState *mState;
		GameState *mOldState;
	};
}

extern ST::Game *game;

#endif

