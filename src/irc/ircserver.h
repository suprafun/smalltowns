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
 *	Date of file creation: 09-02-03
 *
 *	$Id$
 *
 ********************************************/

#ifndef ST_IRCSERVER_HEADER
#define ST_IRCSERVER_HEADER

#include <string>

namespace IRC
{
class Command;
class IRCClient;
}

namespace ST
{
    class IRCMessage;

    class IRCServer
    {
    public:
        /**
         * Constructor
         * @param hostname The hostname of the IRC server
         */
        IRCServer();

        /**
         * Destructor
         */
        ~IRCServer();

        /**
         * Connect to IRC
         * This connects the client to the IRC server
         */
        void connect(const std::string &hostname);

        /**
         * Check
         * This checks for packets from the IRC server
         * and processes them if found
         */
        void process();

        /**
         * Process messages
         * This process the message from the IRC server
         * @param command The command to process
         */
        void processMessage(IRC::Command *command);

        /**
         * Send message
         * Sends a message to the IRC server
         * @param command The command to send
         */
        void sendMessage(IRCMessage *msg);


    private:
        std::string mHostname;
        std::string mNick;
        std::string mCurrentChannel;
        IRC::IRCClient *mClient;
        bool mRegistering;
        bool mRegistered;
    };
}

#endif
