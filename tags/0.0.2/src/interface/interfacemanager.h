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
 *	Date of file creation: 08-10-23
 *
 *	$Id$
 *
 ********************************************/

/**
 * The Interface manager class manages the interface
 */

#ifndef ST_INTERFACE_MANAGER_HEADER
#define ST_INTERFACE_MANAGER_HEADER

#include <map>
#include <string>
#include <SDL.h>

namespace ST
{
	class Window;
	struct MouseButton;

	class InterfaceManager
	{
	public:
		InterfaceManager();
		~InterfaceManager();

		/**
		 * Load GUI Sheet
		 * Loads a gui from an xml file
		 */
		bool loadGuiSheet(const std::string &filename);

		/**
		 * Unload GUI Sheet
		 */
		void unloadGuiSheet();

		/**
		 * Add a new window
		 */
		void addWindow(Window *window);

		/**
		 * Add a new window to another
		 */
		void addSubWindow(Window *parent, Window *window);

		/**
		 * Remove a window
		 */
		void removeWindow(const std::string &name);
		void removeWindow(Window *window);

		/**
		 * Remove all windows currently added
		 */
		void removeAllWindows();

		/**
		 * Returns the window
		 */
		Window* getWindow(const std::string &name);
		Window* getWindow(int x, int y);

		/**
		 * Draw windows
		 */
		void drawWindows();

		/**
		 * Change focus
		 */
        void changeFocus(Window *window);

        /**
         *
         */
        Window* getFocused();

        /**
         * Send a key event to the interface
         */
        void sendKey(SDL_keysym key);

        /**
         * Send a mouse event to the interface
         */
        void sendMouse(MouseButton *button);

	private:
		/**
		 * Draw window
		 */
		void drawWindow(Window *window);

	private:
		std::map<std::string, Window*> mWindows;
		typedef std::map<std::string, Window*>::iterator WindowItr;
		Window* mFocused;
	};

	extern InterfaceManager *interfaceManager;
}

#endif
