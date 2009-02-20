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
 * The Window is the base class for the interface
 */

#ifndef ST_WINDOW_HEADER
#define ST_WINDOW_HEADER

#include "../utilities/types.h"

#include <list>
#include <string>
#include <SDL.h>

namespace ST
{
    struct MouseButton;
	class Texture;

	class WindowTitle
	{
	public:
		WindowTitle();
		int getHeight() const;
		std::string getTitle();
		void setHeight(int height);
		void setTitle(const std::string &title);

	private:
		int mHeight;
		std::string mTitle;
	};


	class Window
	{
	public:
		Window(std::string name);
		Window(Window *parent, std::string name);
		
		// Add dependant child window
		virtual void addChild(Window *window);
		virtual void addParent(Window *window);
		
		// set visibility
		void setVisible(bool visible);
		bool getVisible() const;

		// get info
		std::string getName() const;
		Point& getPosition();
		int getWidth() const;
		int getHeight() const;
		int getNumChildren() const;
		Rectangle& getClipArea();

		// set info
		void setPosition(int x, int y);
		void setSize(int width, int height);
		void setFocus(bool focused);
		void setTitleHeight(int height);
		void setClipArea(Rectangle &rect);
		
		// graphics
		virtual void drawWindow();
		void setBackground(const std::string &bg);

		// input
		virtual void processKey(SDL_keysym key);
		virtual void processMouse(MouseButton *button);

	protected:
		std::string mName;
		std::list<Window*> mChildren;
		Window *mParent;
		WindowTitle mTitle;
		Texture *mBackground;
		Rectangle mClipArea;
		Point mPosition;
		Point mSize;
		bool mVisible;
		bool mFocus;
	};
}

#endif
