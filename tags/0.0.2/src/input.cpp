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
 *	Date of file creation: 08-09-21
 *
 *	$Id$
 *
 ********************************************/

#include "input.h"

#include "graphics/graphics.h"
#include "interface/interfacemanager.h"

#include <SDL.h>
#include <algorithm>

namespace ST
{
	void InputManager::getEvents()
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_KEYDOWN:
				{
					keysDown.push_back(event.key.keysym.sym);
					interfaceManager->sendKey(event.key.keysym);
				} break;
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                {
                    MouseButton *button = new MouseButton;
                    button->button = event.button.button;
                    button->state = event.button.state;
                    button->x = event.button.x;
                    button->y = graphicsEngine->getScreenHeight() - event.button.y;
                    interfaceManager->sendMouse(button);
                } break;
            case SDL_MOUSEMOTION:
                {
                    MouseButton *button = new MouseButton;
                    button->button = 0;
                    button->state = event.motion.state;
                    button->x = event.motion.x;
                    button->y = graphicsEngine->getScreenHeight() - event.motion.y;
                    interfaceManager->sendMouse(button);
                } break;
			case SDL_QUIT:
				{
					// fake escape being pressed
					keysDown.push_back(SDLK_ESCAPE);
				} break;
			}
		}
	}

	bool InputManager::getKey(SDLKey key)
	{
		std::list<SDLKey>::iterator itr;
		itr = std::find(keysDown.begin(), keysDown.end(), key);
		if (itr != keysDown.end())
		{
			keysDown.erase(itr);
			return true;
		}

		return false;
	}
}
