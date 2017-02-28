#ifndef _TSXMLBASE_H_
#define _TSXMLBASE_H_

#include "TSCommonTypeDef.h"
#include "TSString.h"

class TiXmlDocument;
class TiXmlElement;

namespace TSun {
	// XML document processing base class
	class XMLBase
	{
	public:
		// constructor
		XMLBase(MemAllocator* allocator = getDefaultMemAllocator());
		// destructor
		virtual ~XMLBase();
	private:
		// xml document
		TiXmlDocument* mTiDoc;
	protected:
		// version
		String mVersion;
		// memory allocator
		DEFINE_MEM_ALLOCATOR_MEMBER;
	public:
		// load from file
		TBOOL loadXMLFile(const TCHAR* filename);
		// save to file
		TBOOL saveXMLFile(const TCHAR* filename);
		// new xml document
		// rewrite this in subclass
		virtual TBOOL newXMLDocument();
	protected:
		// new xml document
		TBOOL newXMLDocument(const String& rootName);
		// delete xml document
		TVOID deleteXMLDocument();
		// get root element
		TiXmlElement* getRootElement();
	protected:
		// ------------------------------ writing functions -------------------------------------------
		// start writing function called by saveXMLFile
		// rewrite this function in subclass
		virtual TBOOL onStartWriting();
		// add child element
		// parent: parent element address
		// return: added element address
		TiXmlElement* addChildElement(TiXmlElement* parent, const TCHAR* name);
		// add int attribute
		TVOID addIntAttribute(TiXmlElement* element, const TCHAR* name, TS32 value);
		// add unsinged int attribute
		TVOID addUIntAttribute(TiXmlElement* element, const TCHAR* name, TU32 value);
		// add bool attribute
		TVOID addBoolAttribute(TiXmlElement* element, const TCHAR* name, TBOOL value);
		// add double attribute
		TVOID addDoubleAttribute(TiXmlElement* element, const TCHAR* name, TF64 value);
		// add float attribute
		TVOID addFloatAttribute(TiXmlElement* element, const TCHAR* name, TF32 value);
		// add string attribute
		TVOID addStringAttribute(TiXmlElement* element, const TCHAR* name, const TCHAR* value);
	protected:
		// ------------------------------ reading functions -------------------------------------------
		// read child element
		TBOOL readChildElement(TiXmlElement* parent, TiXmlElement* element);
		// element reading callback
		// rewrite this function to get key-value
		virtual TBOOL onElementRead(TiXmlElement* parent, TiXmlElement* element, const String& eleName);
		// get int attribute
		TBOOL getIntAttribute(TiXmlElement* element, const TCHAR* name, TS32& value);
		// get unsinged int attribute
		TBOOL getUIntAttribute(TiXmlElement* element, const TCHAR* name, TU32& value);
		// get bool attribute
		TBOOL getBoolAttribute(TiXmlElement* element, const TCHAR* name, TBOOL& value);
		// get double attribute
		TBOOL getDoubleAttribute(TiXmlElement* element, const TCHAR* name, TF64& value);
		// get float attribute
		TBOOL getFloatAttribute(TiXmlElement* element, const TCHAR* name, TF32& value);
		// get string attribute
		TBOOL getStringAttribute(TiXmlElement* element, const TCHAR* name, String& value);
	};
}

#endif