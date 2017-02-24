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
	mLangDict = new TSun::LangDict();
	if (!mLangDict->loadFromFile(strLangDictFullFile.GetString()))
	{
		delete mLangDict;
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
		delete mLangDict;
		mLangDict = 0;
	}
}

TSun::TBOOL GlobleClass::OnLoop()
{
	return TSun::TTRUE;
}