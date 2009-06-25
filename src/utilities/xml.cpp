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
 *	Date of file creation: 09-04-24
 *
 *	$Id$
 *
 ********************************************/

#include "xml.h"
#include "log.h"

namespace ST
{
	XMLFile::XMLFile() : mDoc(NULL), mHandle(NULL), mNextChild(0)
	{

	}

	XMLFile::~XMLFile()
	{
		delete mHandle;
		mHandle = NULL;
		delete mDoc;
		mDoc = NULL;
	}

	bool XMLFile::load(const std::string &file)
	{
	    mDoc = new TiXmlDocument(file.c_str());
        bool loaded = mDoc->LoadFile();
        if (!loaded)
        {
            logger->logWarning("File not found: " + file);
            return false;
        }

        mHandle = new TiXmlHandle(mDoc);
        return true;
	}

	bool XMLFile::next(const std::string &element)
	{
        ++mNextChild;
        return mHandle->Child(element.c_str(), mNextChild).ToElement() ? true : false;
	}

	std::string XMLFile::readString(const std::string &element, const std::string &attribute)
	{
        std::string str;
        TiXmlElement *e;

        e = mHandle->Child(element.c_str(), mNextChild).ToElement();
        if (e)
        {
            str = e->Attribute(attribute.c_str());
        }

        return str;
	}

	int XMLFile::readInt(const std::string &element, const std::string &attribute)
	{
	    int value = 0;
	    TiXmlElement *e;

	    e = mHandle->Child(element.c_str(), mNextChild).ToElement();
        if (e)
        {
            e->QueryIntAttribute(attribute.c_str(), &value);
        }
	    return value;
	}

	void XMLFile::changeString(const std::string &element, const std::string &attribute,
		const std::string &value)
	{
		TiXmlElement *e;

	    e = mHandle->Child(element.c_str(), mNextChild).ToElement();
        if (e)
        {
			e->SetAttribute(attribute.c_str(), value.c_str());
        }
		mDoc->SaveFile();
	}

	void XMLFile::changeInt(const std::string &element, const std::string &attribute, int value)
	{
		TiXmlElement *e;

	    e = mHandle->Child(element.c_str(), mNextChild).ToElement();
        if (e)
        {
			e->SetAttribute(attribute.c_str(), value);
        }
		mDoc->SaveFile();
	}
}
