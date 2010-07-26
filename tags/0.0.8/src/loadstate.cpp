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
 *	Date of file creation: 2009-11-08
 *
 *	$Id$
 *
 ********************************************/

#include "loadstate.h"
#include "game.h"
#include "connectstate.h"
#include "input.h"
#include "player.h"

#include "interface/interfacemanager.h"

#include "net/networkmanager.h"
#include "net/packet.h"
#include "net/protocol.h"

#include "utilities/log.h"
#include "utilities/stringutils.h"

#include <SDL.h>
#include <sstream>

namespace ST
{
	LoadState::LoadState()
	{
	}

	void LoadState::enter()
	{
	    mTimeouts = 0;
		mLastTime = SDL_GetTicks();
        mConnecting = true;
    }

	void LoadState::exit()
	{
		interfaceManager->removeAllWindows();
	}

	bool LoadState::update()
	{
        // Check for input, if escape pressed, exit
		if (inputManager->getKey(AG_KEY_ESCAPE))
		{
			GameState *state = new ConnectState;
			game->changeState(state);
		}

		if (mConnecting && (SDL_GetTicks() - mLastTime) > 500)
        {
            int tag = networkManager->getTag();

            //send connect msg to game server
            Packet *p = new Packet(PGMSG_CONNECT);
            p->setInteger(player->getId());
            p->setInteger(tag);
            networkManager->sendPacket(p);

            //log that we sent the tag
            std::stringstream str;
            str << "Sending tag " << tag;
            logger->logDebug(str.str());

            ++mTimeouts;
            mLastTime = SDL_GetTicks();
        }
        // check if its timedout
		if (mTimeouts > 6)
		{
		    mConnecting = false;
		    networkManager->disconnect();

		    // reset timeout, and log the error
		    logger->logWarning("Connecting timed out");
		    interfaceManager->showWindow("/Connection", true);

			// set error label, and stop connecting
		    interfaceManager->setErrorMessage("Error: Connection timed out");
		    interfaceManager->showErrorWindow(true);
		}

		SDL_Delay(0);

		return true;
	}

}


