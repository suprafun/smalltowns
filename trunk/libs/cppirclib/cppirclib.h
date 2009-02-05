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
 *	Date of file creation: 08-10-09
 *
 *	$Id$
 *
 ********************************************/

#ifndef IRCLIB
#define IRCLIB

#ifdef WIN32
#include <winsock2.h>
typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
#define EXPORT __declspec(dllexport)
#else
#include <netinet/in.h>
#define EXPORT
#endif

#include <list>
#include <map>
#include <string>
#include <vector>

namespace IRC
{
    class Socket
    {
    public:
        Socket();
        ~Socket();
        unsigned int getSocket() const;
        bool doConnection(uint32_t host, uint16_t port);
    private:
        unsigned int mDescriptor;
        struct sockaddr_in mAddress;
    };

    class Command
    {
    public:
        /**
         * IRC Commands
         */
        enum {
            IRC_NONE = 0,
            IRC_CONNECT,
            IRC_QUIT,
            IRC_PING,
            IRC_PONG,
            IRC_NOTICE,
            IRC_VERSION,
            IRC_PASS,
            IRC_NICK,
            IRC_USER,
            IRC_SERVER,
            IRC_JOIN,
            IRC_PART,
            IRC_SAY,
            IRC_OP,
            IRC_DEOP,
            IRC_NAMES
        };

        /**
         * Constructor
         */
        EXPORT Command();

        /**
         * Destructor
         */
        EXPORT ~Command();

        /**
         * Set Command
         */
        EXPORT void setCommand(unsigned int command);

        /**
         * Set Params
         */
        EXPORT void setParams(const std::string &params);

        /**
         * Set Info
         */
        EXPORT void setUserInfo(const std::string &info);
        EXPORT void setChanInfo(const std::string &info);

        /**
         * Get Command
         */
        EXPORT unsigned int getCommand() const;

        /**
         * Get Params
         */
        EXPORT std::string getParam(unsigned int param) const;
        EXPORT int numParams() const;

        /**
         * Get Info
         */
        EXPORT std::string getUserInfo() const;
        EXPORT std::string getChanInfo() const;

    private:
        unsigned int mCommand;
        std::vector<std::string> mParams;
        std::string mUserInfo;
        std::string mChanInfo;
    };

    class IRCConnection
    {
    public:
        /**
         * Constructor
         */
        IRCConnection();

        /**
         * Destructor
         */
        ~IRCConnection();

        /**
         * Check for data
         * @param data A pointer to a data buffer to store the data
         * @param length The size of the data buffer
         * @return Returns length of data or 0 if no data
         */
        int checkForData(char *data, unsigned int length) const;

        /**
         * Send data
         * @param data The data to send to the host
         */
        void sendData(const char *data, unsigned int length);

        /**
         * Get Socket
         * @return Returns the socket
         */
        Socket* getSocket() const;

    private:
        Socket *mSocket;
        char *mData;
        int mLength;
        int mPos;
    };

    class IRCUser
    {
    public:
        /**
         * Constructor
         * @param nick The user's nickname
         * @param flags the Mode of the user
         */
        IRCUser(const std::string &nick, int flags);

    private:
        std::string mNick;
        unsigned int flags;
    };

    class IRCChannel
    {
    public:
        /**
         * Constructor
         * @param name Name of the channel
         */
        IRCChannel(const std::string &name);

        /**
         * Destructor
         */
        ~IRCChannel();

        /**
         * Add user
         * @param nick The nickname of the user
         * @param flags The mode of the user
         */
        void addUser(const std::string &nick, int flags);

    private:
        std::string mName;
        std::list<IRCUser*> mUsers;
    };

    class IRCClient
    {
    public:
        /**
         * Constructor
         */
        EXPORT IRCClient();

        /**
         * Destructor
         */
        ~IRCClient();

        /**
         * Init
         * Must be called before connecting
         */
        EXPORT void init();

        /**
         * Connect to IRC Server
         * @param hostname The hostname to connect to
         * @return Returns whether its a valid hostname
         */
        EXPORT bool connectTo(const std::string &hostname, unsigned int port);

        /**
         * Is Connected
         * @return Returns true when connected to the server
         */
        EXPORT bool isConnected() const;

        /**
         * Ping
         * Checks if there is data to recieve
         * @return Returns the number of commands received
         */
        EXPORT int ping();

        /**
         * Send Command
         * @param Command to send
         */
        EXPORT void sendCommand(Command *command);

        /**
         * Get Command
         * @return Returns the IRC Command
         */
        EXPORT Command* getCommand();

        /**
         * Add Command
         */
        EXPORT void addCommand(char *data, unsigned int length);

        /**
         * Do Registration
         * Register with server (sends PASS, NICK, USER
         */
        EXPORT void doRegistration(const std::string &password,
                            const std::string &nick,
                            const std::string &user);

        /**
         * Add Channel
         * @param channel The name of the channel to add
         */
        EXPORT void addChannel(const std::string &channel);

        /**
         * Get Channel
         */
        EXPORT IRCChannel* getChannel(const std::string &channel);

    private:
        IRCConnection *mConnection;
        std::vector<Command*> mCommands;
        std::map<std::string, IRCChannel*> mChannels;
        bool mConnected;
        bool mInitDone;
    };

}

#endif
