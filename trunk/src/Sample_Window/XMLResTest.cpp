#include "XMLResTest.h"

#include "TSLog.h"

XMLResTest::XMLResTest() : TSun::XMLBase()
{
	mVersion = "1.0.0";
}

XMLResTest::~XMLResTest()
{

}

TSun::TBOOL XMLResTest::newXMLDocument()
{
	return TSun::XMLBase::newXMLDocument("ResTest");
}

TSun::TBOOL XMLResTest::onStartWriting()
{
	TiXmlElement* newEle = addChildElement(getRootElement(), "firstEle");
	addStringAttribute(newEle, "testString", "bcs");
	return TSun::TTRUE;
}

TSun::TBOOL XMLResTest::onElementRead(TiXmlElement* parent, TiXmlElement* element, const TSun::String& eleName)
{
	if (element == TSun::TNULL)
		return TSun::TFALSE;
	if (eleName == "ResTest")
	{
		TSun::Log::WriteLine(TSun::Log::LOG_LEVEL_INFO, TSun::TTRUE, L"reading root");
		TSun::WString log(L"xml version: ");
		log += mVersion.ToWString().get();
		TSun::Log::WriteLine(TSun::Log::LOG_LEVEL_INFO, TSun::TTRUE, log.GetWString());
	}
	else if (eleName == "firstEle")
	{
		TSun::Log::WriteLine(TSun::Log::LOG_LEVEL_INFO, TSun::TTRUE, L"reading firstEle");
		TSun::String strVal("");
		getStringAttribute(element, "testString", strVal);
		TSun::WString log(L"testString value: ");
		log += strVal.ToWString().get();
		TSun::Log::WriteLine(TSun::Log::LOG_LEVEL_INFO, TSun::TTRUE, log.GetWString());
	}
	return TSun::TTRUE;
}