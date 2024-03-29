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

/**
 * The Network manager class manages the networking
 */

#ifndef ST_NETWORK_MANAGER_HEADER
#define ST_NETWORK_MANAGER_HEADER

#include <string>

namespace ST
{
    struct Point;
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
         * Get Server Tag
         */
        int getTag() const;

        /**
         * Download file for updates
         */
        bool downloadFile(const std::string &hostname, const std::string &file);

        /**
         * Download content files
         */
        int downloadContent(const std::string &hostname);

        /**
         * Set default settings
         */
        void setDefault(const std::string &hostname, int port);

        /**
         * Send the players position update to the server
         * Should be sent several times per second
         */
        void sendPositionUpdate(const Point &pos);

        /**
         * Return the ping time
         * This is a measure of latency
         */
        int getPing() const;

        /**
         * Save data to file (for curl)
         */
        void saveData(char *buffer, int size);

        /**
         * Returns the total number of content updates needed to download
         */
        unsigned int getTotalDownloads() const;

    private:
		/**
		 * Process the packets
		 */
		void processPacket(Packet*);

	private:
		Host *mHost;
		std::string mDefaultHost;
		int mDefaultPort;
		int mTag; /** << Used for reconnecting to game server */
		int mPing;
		unsigned int mNumDownloads;
		char *mFileData;
		unsigned int mPos;
	};

	extern NetworkManager *networkManager;
}

#endif
