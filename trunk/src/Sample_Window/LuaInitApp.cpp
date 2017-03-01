#include "LuaInitApp.h"

LuaInit* TSun::Singleton<LuaInit>::m_Ptr = 0;

LuaInit::LuaInit(void) : m_consoleWidth(800), m_consoleHeight(600)
{
	m_szLogConfigFile[0] = '\0';
	m_szResDir[0] = '\0';
	m_szSysLangDict[0] = '\0';
}

LuaInit::~LuaInit(void)
{
}

bool LuaInit::InitWindowScript(const char *scriptFile)
{
	if(!InitLuaBase(scriptFile))
		return false;
	return true;
}

void LuaInit::LoadParameters()
{
	// 得到结果
	GetVariable("consoleWidth");
	GetVariable("consoleHeight");
	GetVariable("resDir");
	GetVariable("logConfigFile");
	GetVariable("sysLangDict");

	const char* szSysLangDict = Lua_tostring(-1);
	memcpy_s(m_szSysLangDict, sizeof(char) * 256, szSysLangDict, sizeof(char) * 256);
	const char* szLogConfigFile = Lua_tostring(-2);
	memcpy_s(m_szLogConfigFile, sizeof(char) * 256, szLogConfigFile, sizeof(char) * 256);
	const char* szResDir = Lua_tostring(-3);
	memcpy_s(m_szResDir,sizeof(char)*256,szResDir,sizeof(char)*256);
	m_consoleHeight = (unsigned int)Lua_tonumber(-4);
	m_consoleWidth = (unsigned int)Lua_tonumber(-5);

	Pop(5);
}