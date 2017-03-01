#ifndef _XMLRESTEST_H_
#define _XMLRESTEST_H_

#include "TSXMLBase.h"
#include "ConfigDef.h"

#ifdef BUILD_TEST
class XMLResTest : public TSun::XMLBase
{
public:
	// constructor
	XMLResTest();
	// destructor
	virtual ~XMLResTest();
public:
	// ---------------- overwrite functions -------------------
	// new xml document
	// rewrite this in subclass
	virtual TSun::TBOOL newXMLDocument();
protected:
	// ---------------- overwrite functions -------------------
	// start writing function called by saveXMLFile
	// rewrite this function in subclass
	virtual TSun::TBOOL onStartWriting();
	// element reading callback
	// rewrite this function to get key-value
	virtual TSun::TBOOL onElementRead(TiXmlElement* parent, TiXmlElement* element, const TSun::String& eleName);
};
#endif

#endif
