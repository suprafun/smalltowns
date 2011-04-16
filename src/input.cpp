/*********************************************
 *
 *	Author: David Athay
 *
 *	License: New BSD License
 *
 *	Copyright (c) 2008, CT Games
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
        AG_DriverEvent dev;

        if (AG_PendingEvents(NULL) > 0)
        {
            do
            {
                if (AG_GetNextEvent(NULL, &dev) == 1)
                {
                    /* Forward the event to Agar. */
                    if (AG_ProcessEvent(NULL, &dev) != 0)
                        return;
                    switch (dev.type)
                    {
                    case AG_DRIVER_MOUSE_BUTTON_DOWN:
                        interfaceManager->handleMouseEvent(dev.data.button.which,
                               dev.data.button.x, dev.data.button.y,
                               0);
                        break;
                    case AG_DRIVER_MOUSE_BUTTON_UP:
                        interfaceManager->handleMouseEvent(dev.data.button.which,
                               dev.data.button.x, dev.data.button.y,
                               1);
                        break;
                    case AG_DRIVER_MOUSE_MOTION:
                        interfaceManager->handleMouseEvent(0, dev.data.motion.x, dev.data.motion.y, 0);
                        break;
                    case AG_DRIVER_KEY_DOWN:
                        keysDown.push_back(dev.data.key.ks);
                        break;
                    case AG_DRIVER_CLOSE:
                        keysDown.push_back(AG_KEY_ESCAPE);
                        break;
                    default:
                        break;
                    }
                }
            } while (AG_PendingEvents(NULL) > 0);
        }
	}

	bool InputManager::getKey(AG_KeySym key)
	{
		std::list<AG_KeySym>::iterator itr;
		itr = std::find(keysDown.begin(), keysDown.end(), key);
		if (itr != keysDown.end())
		{
			keysDown.erase(itr);
			return true;
		}

		return false;
	}
}
