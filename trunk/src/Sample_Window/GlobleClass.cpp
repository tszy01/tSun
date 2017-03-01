#include "GlobleClass.h"
#include "TSLangDict.h"

GlobleClass* TSun::Singleton<GlobleClass>::m_Ptr = 0;

GlobleClass::GlobleClass(TSun::TVOID) : m_strResDir(""), mLangDict(0)
{
}

GlobleClass::~GlobleClass(TSun::TVOID)
{
}

TSun::TBOOL GlobleClass::InitGlobleClass(const TSun::String& resDir, const TSun::String& dictFile)
{
	m_strResDir = resDir;
	// init language dictionary
	TSun::String strDictFile = dictFile;
	if (strDictFile == "")
	{
		strDictFile = "lang_dict//sys_en.txt";
	}
	TSun::String strLangDictFullFile = m_strResDir + strDictFile;
	mLangDict = T_NEW(TSun::getStructMemAllocator(), TSun::LangDict);
	if (!mLangDict->loadFromFile(strLangDictFullFile.GetString()))
	{
		T_DELETE(TSun::getStructMemAllocator(), TSun::LangDict, mLangDict);
		mLangDict = 0;
		return TSun::TFALSE;
	}

	return TSun::TTRUE;
}

TSun::TVOID GlobleClass::DestroyGlobleClass()
{
	// destroy language dictionary
	if (mLangDict)
	{
		T_DELETE(TSun::getStructMemAllocator(), TSun::LangDict, mLangDict);
		mLangDict = 0;
	}
}

TSun::TBOOL GlobleClass::OnLoop()
{
	return TSun::TTRUE;
}