/*********************************************
 *
 *	Author: David Athay
 *
 *	License: New BSD License
 *
 *	Copyright (c) 2008, CasualGamer.co.uk
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
 *	- Neither the name of the author nor the names of its contributors
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
 *	Date of file creation: 08-11-14
 *
 *	$Id$
 *
 ********************************************/

#ifndef IRCLIB_PARSER
#define IRCLIB_PARSER

#include <string>

namespace IRC
{
    class Command;

    class IRCParser
    {
    public:
        /**
         * Parse the data received from server
         * @param data The data received
         * @param length The size of the data
         * @return Returns the command received
         */
        Command* parse(char *data, unsigned int length);

        /**
         * Parse the command
         * Create a command based on the arguments
         * @param prefix The prefix
         * @param command The command
         * @param params The parameters
         * @return Returns the command
         */
        Command* parseCommand(std::string &prefix,
                              const std::string &command,
                              std::string &params);

        /**
         * Parse the word
         * @param data The data (must be null terminated)
         * @return Returns the word found in data
         */
        std::string parseWord(char *data);

        /**
         * Lookup the command
         * Changes a string to its enum
         */
        unsigned int lookupCommand(const std::string &command);
    };
}

extern IRC::IRCParser gIrcParser;

#endif
