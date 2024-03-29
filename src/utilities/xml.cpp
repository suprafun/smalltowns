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
 *	Date of file creation: 09-04-24
 *
 *	$Id$
 *
 ********************************************/

#include "xml.h"
#include "log.h"

namespace ST
{
	XMLFile::XMLFile() : mDoc(NULL), mHandle(NULL), mCurrentElement(NULL)
	{

	}

	XMLFile::~XMLFile()
	{
	    mElements.clear();
		close();
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

	void XMLFile::close()
	{
		delete mHandle;
		mHandle = NULL;
		delete mDoc;
		mDoc = NULL;
	}

	bool XMLFile::nextSubElement(const std::string &element)
	{
        ElementItr itr = mElements.find(element);
        if (itr != mElements.end())
        {
            itr->second = itr->second->NextSiblingElement();
	        return itr->second != 0;
        }
        return false;
    }

    bool XMLFile::nextElement(const std::string &element)
    {
        ElementItr itr = mElements.find(element);
        if (itr != mElements.end())
        {
            itr->second = itr->second->NextSiblingElement();
            mCurrentElement = itr->second;
	        return itr->second != 0;
        }
        return false;
    }

	void XMLFile::setElement(const std::string &element)
	{
		assert(mHandle);
	    TiXmlElement *e = mHandle->FirstChild(element.c_str()).Element();
	    if (e)
	    {
	        mElements.insert(std::pair<std::string, TiXmlElement*>(element, e));
            mCurrentElement = e;
	    }
	}

	void XMLFile::setSubElement(const std::string &subelement)
	{
	    assert(mCurrentElement);
        TiXmlElement *e = mCurrentElement->FirstChildElement(subelement.c_str());
        mElements.insert(std::pair<std::string, TiXmlElement*>(subelement, e));
	}

    std::string XMLFile::readString(const std::string &element, const std::string &attribute)
    {
        std::string str;

        ElementItr itr = mElements.find(element);
        if (itr != mElements.end())
        {
            const char *c_str = itr->second->Attribute(attribute.c_str());
            if (c_str == NULL)
                return "";
            str = c_str;
        }

        return str;
    }

    int XMLFile::readInt(const std::string &element, const std::string &attribute)
    {
        int value = 0;

        ElementItr itr = mElements.find(element);
        if (itr != mElements.end())
        {
            itr->second->QueryIntAttribute(attribute.c_str(), &value);
        }
        return value;
    }

    void XMLFile::changeString(const std::string &element, const std::string &attribute,
        const std::string &value)
    {
        ElementItr itr = mElements.find(element);
        if (itr != mElements.end())
        {
            itr->second->SetAttribute(attribute.c_str(), value.c_str());
        }
    }

    void XMLFile::changeInt(const std::string &element, const std::string &attribute, int value)
    {
        ElementItr itr = mElements.find(element);
        if (itr != mElements.end())
        {
            itr->second->SetAttribute(attribute.c_str(), value);
        }
    }

    void XMLFile::addString(const std::string &element, const std::string &attribute,
        const std::string &value)
    {
        TiXmlElement *el = new TiXmlElement(element.c_str());
        el->SetAttribute(attribute.c_str(), value.c_str());
        mDoc->LinkEndChild(el);
    }

    void XMLFile::addInt(const std::string &element, const std::string &attribute, int value)
    {
        TiXmlElement *el = new TiXmlElement(element.c_str());
        el->SetAttribute(attribute.c_str(), value);
        mDoc->LinkEndChild(el);
    }

    void XMLFile::clear(const std::string &element)
    {
        ElementItr itr = mElements.find(element);
        if (itr != mElements.end())
        {
            mElements.erase(itr);
        }
    }

    bool XMLFile::parse(char *data)
    {
        mDoc = new TiXmlDocument;
        mDoc->Parse(data);
        mHandle = new TiXmlHandle(mDoc);
        return true;
    }

    void XMLFile::save()
    {
        mDoc->SaveFile();
    }
}
