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

#include "packet.h"

#include "enet/enet.h"

#ifdef WIN32
typedef unsigned int uint32_t;
#endif

namespace ST
{
    Packet::Packet(int id)
    : mSize(DEFAULT_SIZE), mId(id), mPosition(0)
    {
        mData = new char[DEFAULT_SIZE];
    }

    Packet::Packet(char *data, unsigned int length)
    : mData(data), mSize(length), mPosition(0)
    {
        mId = getInteger();
    }

    Packet::~Packet()
    {
        if (mData)
            delete[] mData;
    }

    void Packet::expand(unsigned int size)
    {
        if (size > mSize)
        {
            mSize = size << 1;
            delete[] mData;
            mData = new char[mSize];
        }
    }

    unsigned char Packet::getByte()
    {
        int c = -1;
        if (mPosition < mSize)
        {
            c = (unsigned char) mData[mPosition];
        }
        mPosition += 1;
        return c;
    }

    void Packet::setByte(unsigned char c)
    {
        expand(mPosition + 1);
        mData[mPosition] = c;
        mPosition += 1;
    }

    int32_t Packet::getInteger()
    {
        int num = -1;
        if (mPosition + 4 <= mSize)
        {
            uint32_t t;
            memcpy(&t, mData + mPosition, 4);
            num = ENET_NET_TO_HOST_32(t);
        }
        mPosition += 4;
        return num;
    }

    void Packet::setInteger(int32_t num)
    {
        expand(mPosition + 4);
        uint32_t t = ENET_HOST_TO_NET_32(num);
        memcpy(mData + mPosition, &t, 4);
        mPosition += 4;
    }

    std::string Packet::getString()
    {
        std::string s = "";
        while (mPosition < mSize && mData[mPosition] != '\0')
        {
            s += mData[mPosition];
            ++mPosition;
        }

        return s;
    }

    void Packet::setString(std::string s)
    {
        expand(mPosition + s.size());
        memcpy(mData + mPosition, s.c_str(), s.size());
        mPosition += s.size();
    }
}
