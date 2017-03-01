#ifndef _LUAINITAPP_H_
#define _LUAINITAPP_H_
#include "TSLuaBase.h"
#include "TSSingleton.h"

class LuaInit : public TSun::Singleton<LuaInit>, public TSun::LuaBase
{
	friend class TSun::Singleton<LuaInit>;
protected:
	LuaInit(void);
	virtual ~LuaInit(void);
public:
	unsigned int m_consoleHeight;	// 控制台高
	unsigned int m_consoleWidth;	// 控制台宽
	char m_szLogConfigFile[256];	// Log config file
	char m_szResDir[256];	// 资源根目录
	char m_szSysLangDict[256]; // Engine Language Dictionary File
public:
	bool InitWindowScript(const char *scriptFile);
	void LoadParameters();
};

#endif