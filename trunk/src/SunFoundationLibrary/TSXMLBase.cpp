#include "TSXMLBase.h"

#include "tinyxml.h"

namespace TSun {
	XMLBase::XMLBase() : mTiDoc(TNULL), mVersion("")
	{
	}

	XMLBase::~XMLBase()
	{
		deleteXMLDocument();
	}

	TBOOL XMLBase::loadXMLFile(const TCHAR* filename)
	{
		if (!filename)
			return TFALSE;
		deleteXMLDocument();
		mTiDoc = T_NEW(getStructMemAllocator(), TiXmlDocument);
		if (!mTiDoc->LoadFile(filename))
			return TFALSE;
		// read root
		TiXmlElement* root = getRootElement();
		mVersion = root->Attribute("version");
		if (!onElementRead(TNULL, root, root->Value()))
		{
			return TFALSE;
		}
		// read child
		for (TiXmlElement* child = root->FirstChildElement(); 
			child != TNULL; child = child->NextSiblingElement())
		{
			if (!readChildElement(root, child))
			{
				return TFALSE;
			}
		}
		return TTRUE;
	}

	TBOOL XMLBase::saveXMLFile(const TCHAR* filename)
	{
		if (!filename)
			return TFALSE;
		if (mTiDoc == TNULL)
			return TFALSE;
		if (!onStartWriting())
			return TFALSE;
		if (!mTiDoc->SaveFile(filename))
			return TFALSE;
		return TTRUE;
	}

	TBOOL XMLBase::newXMLDocument()
	{
		return newXMLDocument("root");
	}

	TVOID XMLBase::deleteXMLDocument()
	{
		if (mTiDoc != TNULL)
		{
			T_DELETE(getStructMemAllocator(), TiXmlDocument, mTiDoc);
			mTiDoc = TNULL;
		}
	}

	TBOOL XMLBase::newXMLDocument(const String& rootName)
	{
		deleteXMLDocument();
		mTiDoc = T_NEW(getStructMemAllocator(), TiXmlDocument);
		TiXmlElement ele(rootName.GetString());
		ele.SetAttribute("version", mVersion.GetString());
		mTiDoc->InsertEndChild(ele);
		return TTRUE;
	}

	TiXmlElement* XMLBase::getRootElement()
	{
		if (mTiDoc == TNULL)
			return TNULL;
		return mTiDoc->RootElement();
	}

	// -------------------------- writing functions ------------------------------------

	TBOOL XMLBase::onStartWriting()
	{
		return TTRUE;
	}

	TiXmlElement* XMLBase::addChildElement(TiXmlElement* parent, const TCHAR* name)
	{
		if (parent == TNULL)
			return TNULL;
		TiXmlElement ele(name);
		return (TiXmlElement*)parent->InsertEndChild(ele);
	}

	TVOID XMLBase::addIntAttribute(TiXmlElement* element, const TCHAR* name, TS32 value)
	{
		if (element == TNULL)
			return ;
		if (name == TNULL)
			return;
		element->SetAttribute(name, value);
	}

	TVOID XMLBase::addDoubleAttribute(TiXmlElement* element, const TCHAR* name, TF64 value)
	{
		if (element == TNULL)
			return;
		if (name == TNULL)
			return;
		element->SetDoubleAttribute(name, value);
	}

	TVOID XMLBase::addStringAttribute(TiXmlElement* element, const TCHAR* name, const TCHAR* value)
	{
		if (element == TNULL)
			return;
		if (name == TNULL)
			return;
		if (value == TNULL)
			return;
		element->SetAttribute(name, value);
	}

	TVOID XMLBase::addUIntAttribute(TiXmlElement* element, const TCHAR* name, TU32 value)
	{
		if (element == TNULL)
			return;
		if (name == TNULL)
			return;
		TCHAR szTmp[64] = { 0 };
		sprintf(szTmp, "%u", value);
		element->SetAttribute(name, szTmp);
	}

	TVOID XMLBase::addBoolAttribute(TiXmlElement* element, const TCHAR* name, TBOOL value)
	{
		if (element == TNULL)
			return;
		if (name == TNULL)
			return;
		if(value == TTRUE)
		{
			addUIntAttribute(element, name, 1);
		}
		else
		{
			addUIntAttribute(element, name, 0);
		}
	}

	TVOID XMLBase::addFloatAttribute(TiXmlElement* element, const TCHAR* name, TF32 value)
	{
		if (element == TNULL)
			return;
		if (name == TNULL)
			return;
		TCHAR szTmp[64] = { 0 };
		sprintf(szTmp, "%f", value);
		element->SetAttribute(name, szTmp);
	}

	// ------------------------- reading functions ------------------------------------

	TBOOL XMLBase::readChildElement(TiXmlElement* parent, TiXmlElement* element)
	{
		if (element == TNULL)
			return TFALSE;
		if (!onElementRead(parent, element, element->Value()))
		{
			return TFALSE;
		}
		// read children
		for (TiXmlElement* child = element->FirstChildElement();
			child != TNULL; child = child->NextSiblingElement())
		{
			if (!readChildElement(element, child))
			{
				return TFALSE;
			}
		}
		return TTRUE;
	}

	TBOOL XMLBase::onElementRead(TiXmlElement* parent, TiXmlElement* element, const String& eleName)
	{
		return TTRUE;
	}

	TBOOL XMLBase::getIntAttribute(TiXmlElement* element, const TCHAR* name, TS32& value)
	{
		if (element == TNULL || name == TNULL)
			return TFALSE;
		if (element->Attribute(name) == TNULL)
			return TFALSE;
		TS32 tmpValue = 0;
		element->QueryIntAttribute(name, &tmpValue);
		value = tmpValue;
		return TTRUE;
	}

	TBOOL XMLBase::getUIntAttribute(TiXmlElement* element, const TCHAR* name, TU32& value)
	{
		if (element == TNULL || name == TNULL)
			return TFALSE;
		if (element->Attribute(name) == TNULL)
			return TFALSE;
		TU32 tmpValue = 0;
		element->QueryUnsignedAttribute(name, &tmpValue);
		value = tmpValue;
		return TTRUE;
	}

	TBOOL XMLBase::getBoolAttribute(TiXmlElement* element, const TCHAR* name, TBOOL& value)
	{
		if (element == TNULL || name == TNULL)
			return TFALSE;
		if (element->Attribute(name) == TNULL)
			return TFALSE;
		TU32 tmpValue = 0;
		element->QueryUnsignedAttribute(name, &tmpValue);
		value = tmpValue > 0 ? TTRUE : TFALSE;
		return TTRUE;
	}

	TBOOL XMLBase::getDoubleAttribute(TiXmlElement* element, const TCHAR* name, TF64& value)
	{
		if (element == TNULL || name == TNULL)
			return TFALSE;
		if (element->Attribute(name) == TNULL)
			return TFALSE;
		TF64 tmpValue = 0;
		element->Attribute(name, &tmpValue);
		value = tmpValue;
		return TTRUE;
	}

	TBOOL XMLBase::getFloatAttribute(TiXmlElement* element, const TCHAR* name, TF32& value)
	{
		if (element == TNULL || name == TNULL)
			return TFALSE;
		if (element->Attribute(name) == TNULL)
			return TFALSE;
		TF32 tmpValue = 0;
		element->QueryFloatAttribute(name, &tmpValue);
		value = tmpValue;
		return TTRUE;
	}

	TBOOL XMLBase::getStringAttribute(TiXmlElement* element, const TCHAR* name, String& value)
	{
		if (element == TNULL || name == TNULL)
			return TFALSE;
		if (element->Attribute(name) == TNULL)
			return TFALSE;
		value = element->Attribute(name);
		return TTRUE;
	}
}