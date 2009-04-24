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
 *	Date of file creation: 09-01-22
 *
 *	$Id$
 *
 ********************************************/

/**
 * The Network manager class manages the networking
 */

#ifndef ST_NETWORK_MANAGER_HEADER
#define ST_NETWORK_MANAGER_HEADER

#include <string>

namespace ST
{
	class Host;
	class Packet;
	class NetworkManager
	{
	public:
		NetworkManager();
		~NetworkManager();

		/**
		 * Connect to a server
		 */
		void connect();
		void connect(const std::string &hostname, int port);

		/**
		 * Process is called every frame to get packets and process them
		 */
		void process();

		/**
		 * Checks if connected to server yet
		 */
        bool isConnected();

        /**
         * Disconnect from the server (or stop trying to connect)
         */
        void disconnect();

        /**
         * Send a packet to the server
         */
        void sendPacket(Packet *packet);

        /**
         * Send client version to the server
         */
        void sendVersion();

        /**
         * Download file for updates
         */
        bool downloadUpdateFile(const std::string &file);

        /**
         * Set default settings
         */
        void setDefault(const std::string &hostname, int port);

    private:
		/**
		 * Process the packets
		 */
		void processPacket(Packet*);

	private:
		Host *mHost;
		std::string mDefaultHost;
		int mDefaultPort;
	};

	extern NetworkManager *networkManager;
}

#endif
