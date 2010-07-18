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
 *	Date of file creation: 09-01-22
 *
 *	$Id$
 *
 ********************************************/

#include "loginstate.h"
#include "connectstate.h"
#include "teststate.h"
#include "optionsstate.h"
#include "input.h"
#include "game.h"
#include "player.h"
#include "resourcemanager.h"

#include "net/networkmanager.h"
#include "net/packet.h"
#include "net/protocol.h"

#include "interface/interfacemanager.h"

#include "graphics/graphics.h"

#include "utilities/crypt.h"
#include "utilities/log.h"
#include "utilities/xml.h"

#include <SDL.h>

namespace ST
{
    void goto_options(AG_Event *event)
    {
        GameState *state = new OptionsState;
        game->changeState(state);
    }

    void submit_login(AG_Event *event)
    {
        std::string username;
	    std::string password;

	    AG_Textbox *one = static_cast<AG_Textbox*>(AG_PTR(1));
	    AG_Textbox *two = static_cast<AG_Textbox*>(AG_PTR(2));
		AG_Checkbox *three = static_cast<AG_Checkbox*>(AG_PTR(3));
		XMLFile *file = static_cast<XMLFile*>(AG_PTR(4));

	    if (one && two)
	    {
	        username = AG_TextboxDupString(one);
	        password = AG_TextboxDupString(two);
	    }

        if (!username.empty() && !password.empty())
        {
            player->setName(username);
            Packet *packet = new Packet(PAMSG_LOGIN);
            packet->setString(username);
            // use encrypted password
            packet->setString(encryptPassword(username+password));
            networkManager->sendPacket(packet);

			if (three)
			{
				file->changeInt("login", "save", three->state);
				if (three->state != 0)
				{
					file->changeString("login", "username", username);
				}
			}

            interfaceManager->setErrorMessage("");
            interfaceManager->showErrorWindow(false);
        }
        else
        {
            interfaceManager->setErrorMessage("Invalid username or password entered.");
            interfaceManager->showErrorWindow(true);
        }
    }

    void submit_register(AG_Event *event)
    {
        std::string username;
	    std::string password;

	    AG_Textbox *one = static_cast<AG_Textbox*>(AG_PTR(1));
	    AG_Textbox *two = static_cast<AG_Textbox*>(AG_PTR(2));

	    if (one && two)
	    {
	        username = AG_TextboxDupString(one);
	        password = AG_TextboxDupString(two);
	    }

        if (!username.empty() && !password.empty())
        {
            player->setName(username);
            Packet *packet = new Packet(PAMSG_REGISTER);
            packet->setString(username);
            packet->setString(encryptPassword(username+password));
            networkManager->sendPacket(packet);

            interfaceManager->setErrorMessage("");
            interfaceManager->showErrorWindow(false);
        }
        else
        {
            interfaceManager->setErrorMessage("Invalid username or password entered.");
            interfaceManager->showErrorWindow(true);
        }
    }

    void switch_login_window(AG_Event *event)
    {
        AG_Window *one = static_cast<AG_Window*>(AG_PTR(1));
        if (one)
            AG_WindowHide(one);

        AG_Window *two = static_cast<AG_Window*>(AG_PTR(2));
        if (two)
            AG_WindowShow(two);
    }

	LoginState::LoginState()
	{
        file = 0;
	}

	void LoginState::enter()
	{
		int screenWidth = graphicsEngine->getScreenWidth();
		int screenHeight = graphicsEngine->getScreenHeight();
		mHalfScreenWidth = screenWidth / 2;
		mHalfScreenHeight = screenHeight / 2;

		file = new XMLFile();
        file->load(resourceManager->getDataPath("townslife.cfg"));

		AG_Window *win = AG_WindowNew(AG_WINDOW_PLAIN|AG_WINDOW_DENYFOCUS|AG_WINDOW_KEEPBELOW);
		AG_WindowShow(win);
		AG_WindowMaximize(win);

		interfaceManager->addWindow(win);

		mLoginWindow = AG_WindowNewNamed(AG_WINDOW_NOBUTTONS, "LoginWindow");
		mRegisterWindow = AG_WindowNewNamed(AG_WINDOW_NOBUTTONS, "Register");

		createLoginWidgets();
		createRegisterWidgets();
	}

	void LoginState::exit()
	{
		interfaceManager->removeAllWindows();
		delete file;
	}

	bool LoginState::update()
	{

		// Check for input, if escape pressed, exit
		if (inputManager->getKey(AG_KEY_ESCAPE))
		{
			networkManager->disconnect();
			return false;
		}

		SDL_Delay(0);

		return true;
	}

	void LoginState::createLoginWidgets()
	{
	    // read from config file whether username was saved from last time
		file->setElement("login");
		int loginState = file->readInt("login", "save");
		std::string savedUser;

		if (loginState == 1)
		{
			savedUser = file->readString("login", "username");
		}

	    // Load windows from file
        XMLFile guifile;
        std::string filename;
        std::string name;
        std::string title;
        std::string userText;
        std::string passText;
        std::string saveText;
        std::string submitText;
        std::string regText;
        std::string optionsText;
        int w;
        int h;

        filename = "login.";
        filename.append(game->getLanguage());
        filename.append(".xml");

        if (guifile.load(resourceManager->getDataPath(filename)))
        {
            guifile.setElement("window");
            name = guifile.readString("window", "name");
            title = guifile.readString("window", "title");
            w = guifile.readInt("window", "width");
            h = guifile.readInt("window", "height");
            guifile.setSubElement("input");
			userText = guifile.readString("input", "text");
			guifile.nextSubElement("input");
			passText = guifile.readString("input", "text");
			guifile.nextSubElement("input");
			saveText = guifile.readString("input", "text");
			guifile.setSubElement("button");
			submitText = guifile.readString("button", "text");
			guifile.nextSubElement("button");
			regText = guifile.readString("button", "text");
			guifile.nextSubElement("button");
			optionsText = guifile.readString("button", "text");
			guifile.close();

            AG_WindowSetCaption(mLoginWindow, title.c_str());
            AG_WindowSetSpacing(mLoginWindow, 12);
            AG_WindowSetGeometry(mLoginWindow, mHalfScreenWidth - (w / 2) , mHalfScreenHeight - (h / 2), w, h);

            AG_Textbox *username = AG_TextboxNew(mLoginWindow, 0, userText.c_str());
            AG_TextboxSizeHint(username, "XXXXXXXXXXXXXXXX");
            if (!savedUser.empty())
            {
                AG_TextboxSetString(username, savedUser.c_str());
            }
            AG_Textbox *password = AG_TextboxNew(mLoginWindow, AG_TEXTBOX_PASSWORD, passText.c_str());
            AG_TextboxSizeHint(password, "XXXXXXXXXXXXXXXX");
            AG_Checkbox *save = AG_CheckboxNew(mLoginWindow, 0, saveText.c_str());
            if (loginState == 1)
            {
                AG_CheckboxToggle(save);
            }
            AG_SetEvent(password, "textbox-return", submit_login, "%p%p%p%p", username, password, save, file);

            AG_HBox *box = AG_HBoxNew(mLoginWindow, 0);
            AG_Button *button = AG_ButtonNewFn(box, 0, submitText.c_str(), submit_login, "%p%p%p%p",
                                               username, password, save, file);
            AG_ButtonJustify(button, AG_TEXT_CENTER);
            AG_Button *register_button = AG_ButtonNewFn(box, 0, regText.c_str(),
                                                        switch_login_window, "%p%p",
                                                        mLoginWindow, mRegisterWindow);
            AG_ButtonJustify(register_button, AG_TEXT_CENTER);

            AG_ButtonNewFn(mLoginWindow, 0, optionsText.c_str(), goto_options, 0);

            AG_WindowShow(mLoginWindow);
            interfaceManager->addWindow(mLoginWindow);
        }
        else
        {
            logger->logDebug("XML file not found");

            // create the login window's widgets
            AG_WindowSetCaption(mLoginWindow, "Login");
            AG_WindowSetSpacing(mLoginWindow, 12);
            AG_WindowSetGeometry(mLoginWindow, mHalfScreenWidth - 125, mHalfScreenHeight - 100, 225, 200);

            AG_Textbox *username = AG_TextboxNew(mLoginWindow, 0, "Username: ");
            AG_TextboxSizeHint(username, "XXXXXXXXXXXXXXXX");
            if (!savedUser.empty())
            {
                AG_TextboxSetString(username, savedUser.c_str());
            }
            AG_Textbox *password = AG_TextboxNew(mLoginWindow, AG_TEXTBOX_PASSWORD, "Password: ");
            AG_TextboxSizeHint(password, "XXXXXXXXXXXXXXXX");
            AG_Checkbox *save = AG_CheckboxNew(mLoginWindow, 0, "Save username for next time");
            if (loginState == 1)
            {
                AG_CheckboxToggle(save);
            }
            AG_SetEvent(password, "textbox-return", submit_login, "%p%p%p%p", username, password, save, file);

            AG_HBox *box = AG_HBoxNew(mLoginWindow, 0);
            AG_Button *button = AG_ButtonNewFn(box, 0, "Submit", submit_login, "%p%p%p%p",
                                               username, password, save, file);
            AG_ButtonJustify(button, AG_TEXT_CENTER);
            AG_Button *register_button = AG_ButtonNewFn(box, 0, "Register",
                                                        switch_login_window, "%p%p",
                                                        mLoginWindow, mRegisterWindow);
            AG_ButtonJustify(register_button, AG_TEXT_CENTER);

            AG_ButtonNewFn(mLoginWindow, 0, "Options", goto_options, 0);

            AG_WindowShow(mLoginWindow);
            interfaceManager->addWindow(mLoginWindow);
        }
	}

	void LoginState::createRegisterWidgets()
	{
	    // Load windows from file
        XMLFile guifile;
        std::string filename;
        std::string name;
        std::string title;
        std::string userText;
        std::string passText;
        std::string submitText;
        std::string loginText;
        int w;
        int h;

        filename = "register.";
        filename.append(game->getLanguage());
        filename.append(".xml");

        if (guifile.load(resourceManager->getDataPath(filename)))
        {
            guifile.setElement("window");
            name = guifile.readString("window", "name");
            title = guifile.readString("window", "title");
            w = guifile.readInt("window", "width");
            h = guifile.readInt("window", "height");
            guifile.setSubElement("input");
			userText = guifile.readString("input", "text");
			guifile.nextSubElement("input");
			passText = guifile.readString("input", "text");
			guifile.setSubElement("button");
			submitText = guifile.readString("button", "text");
			guifile.nextSubElement("button");
			loginText = guifile.readString("button", "text");
			guifile.close();

            // create registration widgets
            AG_WindowSetCaption(mRegisterWindow, title.c_str());
            AG_WindowSetSpacing(mRegisterWindow, 12);
            AG_WindowSetGeometry(mRegisterWindow, mHalfScreenWidth - (w / 2) , mHalfScreenHeight - (h / 2), w, h);

            AG_Textbox *reg_user = AG_TextboxNew(mRegisterWindow, 0, userText.c_str());
            AG_Textbox *reg_pass = AG_TextboxNew(mRegisterWindow, AG_TEXTBOX_PASSWORD, passText.c_str());
            AG_SetEvent(reg_pass, "textbox-return", submit_register, "%p%p", reg_user, reg_pass);
            AG_ExpandHoriz(reg_user);
            AG_ExpandHoriz(reg_pass);

            AG_HBox *reg_box = AG_HBoxNew(mRegisterWindow, 0);
            AG_Button *reg_button = AG_ButtonNewFn(reg_box, 0, submitText.c_str(), submit_register, "%p%p",
                                                   reg_user, reg_pass);
            AG_ButtonJustify(reg_button, AG_TEXT_CENTER);
            AG_Button *back_button = AG_ButtonNewFn(reg_box, 0, loginText.c_str(),
                                                    switch_login_window, "%p%p", mRegisterWindow, mLoginWindow);
            AG_ButtonJustify(back_button, AG_TEXT_CENTER);

            AG_WindowHide(mRegisterWindow);
            interfaceManager->addWindow(mRegisterWindow);
        }
        else
        {
            logger->logDebug("XML file not found");

            // create registration widgets
            AG_WindowSetCaption(mRegisterWindow, "Registration");
            AG_WindowSetSpacing(mRegisterWindow, 12);
            AG_WindowSetGeometry(mRegisterWindow, mHalfScreenWidth - 125, mHalfScreenHeight - 45, 225, 135);

            AG_Textbox *reg_user = AG_TextboxNew(mRegisterWindow, 0, "Username: ");
            AG_Textbox *reg_pass = AG_TextboxNew(mRegisterWindow, AG_TEXTBOX_PASSWORD, "Password: ");
            AG_SetEvent(reg_pass, "textbox-return", submit_register, "%p%p", reg_user, reg_pass);
            AG_ExpandHoriz(reg_user);
            AG_ExpandHoriz(reg_pass);

            AG_HBox *reg_box = AG_HBoxNew(mRegisterWindow, 0);
            AG_Button *reg_button = AG_ButtonNewFn(reg_box, 0, "Submit", submit_register, "%p%p",
                                                   reg_user, reg_pass);
            AG_ButtonJustify(reg_button, AG_TEXT_CENTER);
            AG_Button *back_button = AG_ButtonNewFn(reg_box, 0, "Back",
                                                    switch_login_window, "%p%p", mRegisterWindow, mLoginWindow);
            AG_ButtonJustify(back_button, AG_TEXT_CENTER);

            interfaceManager->addWindow(mRegisterWindow);
        }
	}
}
