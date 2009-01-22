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

#include "game.h"

#include "input.h"
#include "map.h"
#include "loginstate.h"
#include "graphics/graphics.h"
#include "interface/interfacemanager.h"
#include "net/networkmanager.h"
#include "utilities/log.h"

namespace ST
{
	// globals
	Log *logger = NULL;
	GraphicsEngine *graphicsEngine = NULL;
	InputManager *inputManager = NULL;
	Map *mapEngine = NULL;
	InterfaceManager *interfaceManager = NULL;
	NetworkManager *networkManager = NULL;

	Game::~Game()
	{
		delete networkManager;
		delete interfaceManager;
		delete mapEngine;
		delete inputManager;
		delete graphicsEngine;
		delete logger;
	}

	void Game::run()
	{
		logger = new Log();
		graphicsEngine = new GraphicsEngine();
		inputManager = new InputManager();
		mapEngine = new Map();
		interfaceManager = new InterfaceManager();
		networkManager = new NetworkManager();

		// create new test state, for testing
		state = new LoginState();
		state->enter();

		// Update the state each frame
		// Render the frame
		// Get Input
		while (state->update())
		{
			graphicsEngine->renderFrame();
			interfaceManager->drawWindows();
			inputManager->getEvents();
			networkManager->process();
		}
	}
}
