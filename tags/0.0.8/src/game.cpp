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
 *	Date of file creation: 08-09-20
 *
 *	$Id$
 *
 ********************************************/

#include "game.h"

#include "beingmanager.h"
#include "connectstate.h"
#include "input.h"
#include "languagestate.h"
#include "map.h"
#include "player.h"
#include "resourcemanager.h"
#include "graphics/graphics.h"
#include "graphics/opengl.h"
#include "graphics/sdl2d.h"
#include "interface/interfacemanager.h"
#include "net/networkmanager.h"
#include "utilities/log.h"
#include "utilities/xml.h"

#include <time.h>

namespace ST
{
	// globals
	Log *logger = NULL;
	GraphicsEngine *graphicsEngine = NULL;
	InputManager *inputManager = NULL;
	Map *mapEngine = NULL;
	InterfaceManager *interfaceManager = NULL;
	NetworkManager *networkManager = NULL;
	ResourceManager *resourceManager = NULL;
	BeingManager *beingManager = NULL;
	Player *player = NULL;

    Game::Game(const std::string &path)
    {
        resourceManager = new ResourceManager(path);
        logger = new Log(resourceManager->getWritablePath() + "log.txt");
		logger->logDebug("Data Path: " + resourceManager->getDataPath("tree.png"));
		logger->logDebug("Writable Path: " + resourceManager->getWritablePath());
    }

	Game::~Game()
	{
	    delete player;
	    delete beingManager;
	    delete resourceManager;
		delete networkManager;
		delete interfaceManager;
		delete mapEngine;
		delete inputManager;
		delete graphicsEngine;
		delete logger;
	}

	void Game::restart(int opengl, int fullscreen, int x, int y)
	{
	    // recreate graphicsEngine
	    delete graphicsEngine;

	    if (opengl != 0)
	    {
	        graphicsEngine = new OpenGLGraphics;
	    }
	    else
	    {
	        graphicsEngine = new SDLGraphics;
	    }

	    graphicsEngine->init(fullscreen, x, y);
	    interfaceManager->reset();
	}

	void Game::run()
	{
		// load in configuration file
		XMLFile file;
		std::string hostname;
		int port = 0;
		int opengl = 0;
		int resx = 1024;
		int resy = 768;
        std::string fullscreen;
        std::string lang;

        if (file.load(resourceManager->getDataPath("townslife.cfg")))
        {
            file.setElement("server");
            hostname = file.readString("server", "host");
            port = file.readInt("server", "port");
            file.setElement("graphics");
			opengl = file.readInt("graphics", "opengl");
            fullscreen = file.readString("graphics", "fullscreen");
            resx = file.readInt("graphics", "width");
            resy = file.readInt("graphics", "height");
            file.setElement("language");
            lang = file.readString("language", "value");
        }

		file.close();

		// check whether opengl should be used
		opengl ? graphicsEngine = new OpenGLGraphics : graphicsEngine = new SDLGraphics;

        if (fullscreen == "true")
            graphicsEngine->init(1, resx, resy);
        else
            graphicsEngine->init(0, resx, resy);

		inputManager = new InputManager;
		mapEngine = new Map;
		interfaceManager = new InterfaceManager;
		networkManager = new NetworkManager;
		beingManager = new BeingManager;
		player = new Player;

		if (hostname.empty() || port == 0)
		{
		    logger->logWarning("Error loading configuration, using defaults");
		    networkManager->setDefault("localhost", 9910);
		}
		else
		{
		    networkManager->setDefault(hostname, port);
		}

		// Create state to connect to server
		mOldState = 0;

		if (lang.empty())
		{
		    mState = new LanguageState;
		}
		else
		{
		    setLanguage(lang);
		    mState = new ConnectState();
		}

		mState->enter();

		loop();
	}

	void Game::loop()
	{
        // Update the state each frame
		// Render the frame
		// Get Input
		while (mState->update())
		{
			graphicsEngine->renderFrame();
			inputManager->getEvents();
			networkManager->process();
			cleanUp();
		}
	}

	void Game::changeState(GameState *state)
	{
        mState->exit();
        mOldState = mState;
        mState = state;
        mState->enter();
	}

	void Game::cleanUp()
	{
	    if (mOldState && mOldState->noKeep())
        {
            delete mOldState;
            mOldState = NULL;
        }
	}

    void Game::disconnect()
    {
        networkManager->disconnect();
        networkManager->process();
        while (networkManager->isConnected() && mState->update())
        {
            graphicsEngine->renderFrame();
			inputManager->getEvents();
			networkManager->process();
        }
    }

    bool Game::connect(const std::string &server, int port)
    {
        networkManager->connect(server, port);
        networkManager->process();
        time_t timeout = time(NULL) + 10;
		time_t curTime = time(NULL);
        while (!networkManager->isConnected() && mState->update())
        {
            if (curTime > timeout)
                return false;
            graphicsEngine->renderFrame();
			inputManager->getEvents();
			networkManager->process();
            curTime = time(NULL);
        }

        return true;
    }

    void Game::setLanguage(const std::string &language)
    {
        mLang = language;
    }

    std::string Game::getLanguage() const
    {
        return mLang;
    }
}
