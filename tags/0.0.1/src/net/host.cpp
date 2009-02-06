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
 *	Date of file creation: 09-01-28
 *
 *	$Id$
 *
 ********************************************/

#include "host.h"
#include "packet.h"

namespace ST
{
    Host::Host():
        mServer(NULL), mConnected(false)
    {
        // create a new client host for connecting to the server
        mClient = enet_host_create (NULL /* create a client host */,
                1 /* only allow 1 outgoing connection */,
                57600 / 8 /* 56K modem with 56 Kbps downstream bandwidth */,
                14400 / 8 /* 56K modem with 14 Kbps upstream bandwidth */);
    }

    Host::~Host()
    {
        enet_host_destroy(mClient);
    }

    void Host::connect(const std::string &hostname, unsigned int port)
    {
        // set the address of the server to connect to
        enet_address_set_host(&mAddress, hostname.c_str());
        mAddress.port = port;

        // connect to the server
        mServer = enet_host_connect (mClient, &mAddress, 1);
    }

    void Host::process()
    {
        // check for data
        ENetEvent event;
        while (enet_host_service(mClient, &event, 0) > 0)
        {
            switch (event.type)
            {
            case ENET_EVENT_TYPE_CONNECT:
                {
                    mConnected = true;
                } break;

            case ENET_EVENT_TYPE_RECEIVE:
                {
                    Packet *packet = new Packet((char*)event.packet->data,
                                                event.packet->dataLength);
                    mPackets.push_back(packet);
                    enet_packet_destroy (event.packet);
                } break;

            case ENET_EVENT_TYPE_DISCONNECT:
                {
                    mConnected = false;
                } break;
            }
        }
    }

    Packet* Host::getPacket()
    {
        if (mPackets.size() > 0)
        {
            Packet *p = mPackets.front();
            delete mPackets.front();
            mPackets.pop_front();
            return p;
        }

        return NULL;
    }

    bool Host::isConnected()
    {
        return mConnected;
    }
}
