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
 *	Date of file creation: 09-04-08
 *
 *	$Id$
 *
 ********************************************/

#include "base64.h"

#include <cstring>

namespace ST
{
    int decode_value(char value)
    {
        static const char decoding[] = {
                                        62, -1, -1, -1,
                                        63, 52, 53, 54,
                                        55, 56, 57, 58,
                                        59, 60, 61, -1,
                                        -1, -1, -2, -1,
                                        -1, -1,  0,  1,
                                         2,  3,  4,  5,
                                         6,  7,  8,  9,
                                        10, 11, 12, 13,
                                        14, 15, 16, 17,
                                        18, 19, 20, 21,
                                        22, 23, 24, 25,
                                        -1, -1, -1, -1,
                                        -1, -1, 26, 27,
                                        28, 29, 30, 31,
                                        32, 33, 34, 35,
                                        36, 37, 38, 39,
                                        40, 41, 42, 43,
                                        44, 45, 46, 47,
                                        48, 49, 50, 51
                                       };
    	static const char decoding_size = sizeof(decoding);
    	value -= 43;
    	if (value < 0 || value > decoding_size)
            return -1;
    	return decoding[(int)value];
    }

    unsigned char* Base64::decode(const char *inStr, unsigned char *outStr)
    {
        int shift = 0;
        int accum = 0;
        int n = 0;

        for (int i = 0; i < strlen(inStr); ++i)
        {
            int value = decode_value(inStr[i]);

            if(value >= 0)
            {
                accum <<= 6;
                shift += 6;
                accum |= value;

                if(shift >= 8)
                {
                    shift -= 8;
                    outStr[n] = (char)((accum >> shift) & 0xFF);
                    ++n;
                }
            }
        }

        return outStr;
    }

    int Base64::decodeSize(int stringSize)
    {
        // get the length
        int len = stringSize;

        int length = (len / 4) * 3;

        // add remainder
        if ((len % 4) == 3)
            length += 2;
        if ((len % 4) == 2)
            length += 1;

        return length;
    }
}
