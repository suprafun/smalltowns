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

#include "cppirclib.h"
#include "ircparser.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

using namespace IRC;

IRCParser gIrcParser;

/****************
 * Socket class methods
 ***************/

Socket::Socket()
{
    mDescriptor = socket(PF_INET, SOCK_STREAM, 0);
    memset (&mAddress, 0, sizeof(mAddress));
}

Socket::~Socket()
{
    close(mDescriptor);
}

unsigned int Socket::getSocket() const
{
    return mDescriptor;
}


bool Socket::doConnection(uint32_t host, uint16_t port)
{
    mAddress.sin_family = AF_INET;
    mAddress.sin_addr.s_addr = host;
    mAddress.sin_port = htons(port);

    if (connect(mDescriptor, (struct sockaddr*)&mAddress, sizeof(mAddress)) < 0)
    {
        perror("Unable to connect");
        return false;
    }

    return true;
}

/****************
 * Command class methods
 ***************/

Command::Command()
: mCommand(0)
{

}

Command::~Command()
{

}

void Command::setCommand(unsigned int command)
{
    mCommand = command;
}

void Command::setParams(const std::string &params)
{
    // new position
    unsigned int position = 0;
    // previous position
    int prev = 0;

    // keep looping until out of params
    while(1)
    {
        position = params.find(' ', prev);
        if (position != std::string::npos)
        {
            mParams.push_back(params.substr(prev, position - prev));
            prev = position + 1;
        }
        else
        {
            mParams.push_back(params.substr(prev));
            break;
        }
    }
}

void Command::setUserInfo(const std::string &info)
{
    mUserInfo = info;
}

void Command::setChanInfo(const std::string &info)
{
    mChanInfo = info;
}

unsigned int Command::getCommand() const
{
    return mCommand;
}

std::string Command::getParam(unsigned int param) const
{
    if (param >= mParams.size())
    {
        return "";
    }
    return mParams[param];
}

int Command::numParams() const
{
    return mParams.size();
}

std::string Command::getUserInfo() const
{
    return mUserInfo;
}

std::string Command::getChanInfo() const
{
    return mChanInfo;
}

/****************
 * IRC Connection methods
 ***************/

IRCConnection::IRCConnection()
{
    mSocket = new Socket();
}

IRCConnection::~IRCConnection()
{
    delete mSocket;
}

int IRCConnection::checkForData(char *data, unsigned int length) const
{
    int len = 0;
    int pos = 0;
    unsigned int socket = mSocket->getSocket();
    timeval tv;
    fd_set readfd;

    tv.tv_sec = 0;
    tv.tv_usec = 100;
    FD_ZERO(&readfd);
    FD_SET(socket, &readfd);

    select(socket + 1, &readfd, NULL, NULL, &tv);

    if (FD_ISSET(socket, &readfd))
    {
        while (1)
        {
            // get one byte at a time
            len = recv(socket, &data[pos], 1, NULL);

            // all commands in irc end with endline
            if (data[pos] == '\n')
            {
                break;
            }

            // increase the position in the array
            ++pos;
            // check it doesnt overflow, if it does,
            // return 0 so it doesnt try processing
            if (pos > length)
                return 0;
        }
    }
    return pos;
}

void IRCConnection::sendData(char *data, unsigned int length)
{
    int len;

	while ((len = send(mSocket->getSocket(), data, length, NULL)) < 0)
		continue;
}

Socket* IRCConnection::getSocket() const
{
    return mSocket;
}

/****************
 * IRC Channel methods
 ***************/

IRCChannel::IRCChannel(const std::string &name)
: mName(name)
{

}

IRCChannel::~IRCChannel()
{

}

void IRCChannel::addUser(const std::string &nick, int flags)
{

}

/****************
 * IRC Client methods
 ***************/

IRCClient::IRCClient()
: mConnected(false), mInitDone(false)
{

}

IRCClient::~IRCClient()
{
    if (mConnection)
        delete mConnection;

    // clean up any channels that were joined
    std::map<std::string, IRCChannel*>::iterator chanItr = mChannels.begin();
    std::map<std::string, IRCChannel*>::const_iterator chanEnd = mChannels.end();
    while (chanItr != chanEnd)
    {
        delete chanItr->second;
        ++chanItr;
    }
    mChannels.clear();

}

void IRCClient::init()
{
    mConnection = new IRCConnection();
    mInitDone = true;
}

bool IRCClient::connectTo(const std::string &hostname, unsigned int port)
{
    if (!mInitDone)
    {
        return false;
    }

    uint32_t host;
    // change the hostname from a string to network readable int
    if (isdigit(hostname[0]))
    {
        // already an IP address
        host = inet_addr(hostname.c_str());
    }
    else
    {
        // change from a domain name to a host structure
        struct hostent *h = gethostbyname(hostname.c_str());
        if (h)
        {
            memcpy(&host, h->h_addr, h->h_length);
        }
    }

    if (!mConnection->getSocket()->doConnection(host, port))
    {
        printf("%s %d", "Unable to connect to", port);
        return false;
    }

    mConnected = true;
    return true;
}

bool IRCClient::isConnected() const
{
    return mConnected;
}

int IRCClient::ping()
{
    char data[255];
    int len;

    // check for new data to arrive from irc server
    if ((len = mConnection->checkForData(data, 255)) > 0)
    {
        addCommand(data, len);
        return 1;
    }

    return 0;
}

void IRCClient::sendCommand(Command *command)
{
    std::string data;
    switch (command->getCommand())
    {
        case Command::IRC_PASS:
            data = "PASS";
            break;

        case Command::IRC_NICK:
            data = "NICK";
            break;

        case Command::IRC_USER:
            data = "USER";
            break;

        case Command::IRC_PONG:
            data = "PONG";
            break;

        case Command::IRC_JOIN:
            data = "JOIN";
            break;

        case Command::IRC_SAY:
            data = "PRIVMSG";
            break;

        case Command::IRC_QUIT:
            data = "QUIT";
            break;

        case Command::IRC_VERSION:
            data = "CTCP VERSION";
            break;

    }

    int i = 0;
    while (command->getParam(i) != "")
    {
        data += " ";
        data += command->getParam(i);
        ++i;
    }
    data += "\r\n";
    mConnection->sendData((char*)data.c_str(), data.size());

    printf("%s", data.c_str());

    delete command;
}

Command* IRCClient::getCommand()
{
    Command *command = mCommands[0];
    mCommands.erase(mCommands.begin());
    return command;
}

void IRCClient::addCommand(char *data, unsigned int length)
{
    Command *command = gIrcParser.parse(data, length);
    mCommands.push_back(command);
}

void IRCClient::doRegistration(const std::string &password,
                               const std::string &nick,
                               const std::string &user)
{
    Command *passCommand = new Command();
    passCommand->setCommand(Command::IRC_PASS);
    passCommand->setParams(password);
    sendCommand(passCommand);

    Command *nickCommand = new Command();
    nickCommand->setCommand(Command::IRC_NICK);
    nickCommand->setParams(nick);
    sendCommand(nickCommand);

    Command *userCommand = new Command();
    userCommand->setCommand(Command::IRC_USER);
    userCommand->setParams(user);
    sendCommand(userCommand);
}

void IRCClient::addChannel(const std::string &channel)
{
    IRCChannel *chan = new IRCChannel(channel);
    mChannels.insert(std::pair<std::string, IRCChannel*>(channel, chan));
}

IRCChannel* IRCClient::getChannel(const std::string &channel)
{
    std::map<std::string, IRCChannel*>::iterator itr = mChannels.find(channel);
    if (itr != mChannels.end())
    {
        return itr->second;
    }

    return 0;
}
