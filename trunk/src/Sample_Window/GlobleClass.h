#ifndef _GLOBLECLASS_H_
#define _GLOBLECLASS_H_

#include "TSString.h"
#include "TSSingleton.h"

namespace TSun
{
	class LangDict;
};
	
/*
	*	本类存储全局静态变量，静态方法
	*/
class GlobleClass : public TSun::Singleton<GlobleClass>
{
	friend class TSun::Singleton<GlobleClass>;
protected:
	GlobleClass(TSun::TVOID);
	~GlobleClass(TSun::TVOID);
private:
	// 临时用 -----------------------
	// 保存资源跟路径
	TSun::String m_strResDir;
	// language dictionary
	TSun::LangDict* mLangDict;
public:
	// ----- 方法 -------------------
	// 初始化
	TSun::TBOOL InitGlobleClass(const TSun::String& resDir, const TSun::String& dictFile);
	// 销毁
	TSun::TVOID DestroyGlobleClass();
	// Loop
	TSun::TBOOL OnLoop();
};

#endif