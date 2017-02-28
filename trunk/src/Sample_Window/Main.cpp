#ifdef DEMO_CHECK_MEM_LEAK
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif // DEMO_CHECK_MEM_LEAK

#include "LuaInitApp.h"

#include "TSString.h"
#include "TSLog.h"
#include "ConsoleWindow.h"
#include "TSConsole.h"

#include "ConfigDef.h"
#include "GlobleClass.h"
#include "TSMemAllocator.h"

#ifdef BUILD_TEST
#include "TSTxtFileReader.h"
#include "TSTxtFileWriter.h"
#include "TSUTF8FileReader.h"
#include "TSUTF8FileWriter.h"
#include "TSBinaryFileProcessor.h"
#include "XMLResTest.h"
#endif // BUILD_TEST

TSun::MemAllocator _globalAllocator;

int WindowLoop()
{
	//HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32PROJECT1));

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, NULL, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return 1;
}

#ifdef BUILD_TEST

int runTest(HINSTANCE hInstance, LPSTR lpCmLine, int nCmdShow)
{
	/*TLunaEngine::SharedPtr<int> a = TLunaEngine::SharedPtr<int>(new int);
	TLunaEngine::SharedPtr<int> b = TLunaEngine::SharedPtr<int>(new int);
	if (a < b)
	{
		printf("a < b\n");
	}*/

	ConsoleWindow* consoleWnd = ConsoleWindow::getSingletonPtr();
	consoleWnd->GetConsoleOutput()->addText(L"支払い");
	
	//char a[64] = { "こんにちは" };

	//wchar_t* newBuff = new wchar_t[64];
	////mbstowcs(newBuff,m_szPtr.getPointer(),m_nLength);
	//int toNum = ::MultiByteToWideChar(CP_ACP, 0, a, -1, 0, 0);
	//::MultiByteToWideChar(CP_ACP, 0, a, -1, newBuff, toNum);

	//FILE* fp = NULL;
	//if (fopen_s(&fp, "test2.txt", "w+b") == 0)
	//{
	//	/*char tmp[64] = { "こんにちは" };
	//	toNum = ::WideCharToMultiByte(CP_UTF8, 0, newBuff, -1, 0, 0, 0, 0);
	//	::WideCharToMultiByte(CP_UTF8, 0, newBuff, -1, tmp, toNum, 0, 0);*/

	//	char tmp[64] = { "こんにちは" };
	//	wchar_t tmpW[64] = { L"你好" };

	//	//::MultiByteToWideChar(CP_ACP, 0, tmp, -1, tmpW, 64);
	//	::WideCharToMultiByte(CP_ACP, 0, tmpW, -1, tmp, 64, 0, 0);

	//	fwrite(tmp, 1, strlen(tmp), fp);

	//	fclose(fp);
	//}

	//TSun::String str("你好");
	//TLunaEngine::SharedPtr<TSun::TWCHAR> wstr = str.GetWString();
	//TSun::TWCHAR wsz[64] = { L"しあわせ" };
	//TSun::String sz(wsz);

	//FILE* fp = NULL;
	/*if (fopen_s(&fp, "test2.txt", "wt") == 0)
	{
		fwrite(sz.GetString(), 1, sz.GetLength(), fp);
		fclose(fp);
	}*/

	//if (fopen_s(&fp, "test2.txt", "r+, ccs=UTF-8") == 0)
	//{
	//	//char tmp[64] = { 0 };
	//	//fread(tmp, 1, 64, fp);
	//	//fclose(fp);

	//	//TSun::String strA(tmp);
	////	strA.ConvertToANSI();
	////	consoleWnd->GetConsoleOutput()->addText(strA);

	//	TSun::TWCHAR tmp[64] = { L"0" };
	//	fread(tmp, sizeof(TSun::TWCHAR), 64, fp);
	//	fclose(fp);

	//	consoleWnd->GetConsoleOutput()->addText(/*TSun::WString(strA.ToWString().getPointer())*/tmp);
	//}

	/*TLunaEngine::UTF8FileReader::OpenTxtFile("test2.txt", &fp);

	TSun::TWCHAR tmp[64] = { L"0" };
	TSun::TS32 sTmp[5] = { 0 };
	TLunaEngine::UTF8FileReader::ReadLineWString(tmp, fp, 0, 0, 64, 0);
	TLunaEngine::UTF8FileReader::ReadLineInteger(sTmp, fp, 5, L' ');
	consoleWnd->GetConsoleOutput()->addText(tmp);

	TLunaEngine::UTF8FileReader::CloseTxtFile(fp);*/

	/*TLunaEngine::UTF8FileReader::OpenTxtFile("test2.txt", &fp);

	TSun::TWCHAR tmp[64] = { L"0" };
	TSun::TBOOL re = TSun::TFALSE;
	TLunaEngine::UTF8FileReader::ReadLineWString(tmp, fp, L"你好你是谁是啊", &re, 64, 0);
	consoleWnd->GetConsoleOutput()->addText(tmp);

	TLunaEngine::UTF8FileReader::CloseTxtFile(fp);*/

	/*TLunaEngine::UTF8FileWriter::OpenTxtFile("test3.txt", &fp);

	TLunaEngine::UTF8FileWriter::WriteLineWString(tmp, fp, (TSun::TU32)wcslen(tmp));
	TSun::TS32 sTmp[5] = { 8,0,9,10,11 };
	TLunaEngine::UTF8FileWriter::WriteLineInteger(sTmp, fp, 5, L';');

	TLunaEngine::UTF8FileWriter::CloseTxtFile(fp);*/
	

	//XMLResTest test;
	/*test.newXMLDocument();
	if (test.saveXMLFile("test0.xml") == TSun::TFALSE)
	{
		consoleWnd->GetConsoleOutput()->addText(L"save xml file error");
	}*/
	//test.loadXMLFile("test0.xml");

	void* a = _globalAllocator.allocateMem(4);
	void* b = _globalAllocator.allocateMem(4);
	void* c = _globalAllocator.allocateMem(4);
	void* d = _globalAllocator.allocateMem(4);
	_globalAllocator.freeMem(c);
	c = 0;
	c = _globalAllocator.allocateMem(4);
	_globalAllocator.freeMem(b);
	b = 0;
	_globalAllocator.freeMem(a);
	a = 0;
	_globalAllocator.freeMem(c);
	c = 0;
	_globalAllocator.freeMem(d);
	d = 0;
	return 0;
}
#endif // BUILD_TEST

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmLine, int nCmdShow)
{
#ifdef DEMO_CHECK_MEM_LEAK
	//_CrtSetBreakAlloc(538);
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif // DEMO_CHECK_MEM_LEAK

	// memory management
	_globalAllocator.initialize(1024 * 1024 * 10);

	// init variables
	TSun::String initScriptFile("initapp.lua");
	bool bEditor = false;
	bool bTest = false;

	// read command
	TSun::String strCmd(lpCmLine);
	TSun::TU32 nCmd = 1;
	TSun::List<TSun::String> cmds = strCmd.Split(' ', &nCmd);
	for (int i = 0; i < (int)cmds.size(); ++i)
	{
		if (cmds[i] == "-initfile" && i + 1 < (int)cmds.size())
		{
			initScriptFile = cmds[i + 1];
		}
#ifdef BUILD_TEST
		if (cmds[i] == "-test")
		{
			bTest = true;
		}
#endif
	}

	int re = 1;

	// read init script
	if (!LuaInit::getSingletonPtr()->InitWindowScript(initScriptFile.GetString()))
		return 1;
	LuaInit::getSingletonPtr()->LoadParameters();

	// init log system
	TSun::String strResDir(LuaInit::getSingletonPtr()->m_szResDir);
	TSun::String strLogConfig = strResDir + TSun::String(LuaInit::getSingletonPtr()->m_szLogConfigFile);
	TSun::String strLogDir = strResDir + "log\\";
	if (!TSun::Log::InitLogSystem(strLogConfig.GetString(), strLogDir.GetString()))
	{
		TSun::Log::DestroyLogSystem();
		LuaInit::delSingletonPtr();
		return 1;
	}

	// init console window
	ConsoleWindow* consoleWnd = ConsoleWindow::getSingletonPtr();
	if (consoleWnd->InitWindow(hInstance, nCmdShow,
		LuaInit::getSingletonPtr()->m_consoleWidth, LuaInit::getSingletonPtr()->m_consoleHeight) != S_OK)
	{
		ConsoleWindow::getSingletonPtr()->DestroyWindow();
		ConsoleWindow::delSingletonPtr();
		TSun::Log::DestroyLogSystem();
		LuaInit::delSingletonPtr();
		return 1;
	}

	// GlobleClass
	if (!GlobleClass::getSingletonPtr()->InitGlobleClass(strResDir, LuaInit::getSingletonPtr()->m_szSysLangDict))
	{
		GlobleClass::getSingletonPtr()->DestroyGlobleClass();
		GlobleClass::delSingletonPtr();
		ConsoleWindow::getSingletonPtr()->DestroyWindow();
		ConsoleWindow::delSingletonPtr();
		TSun::Log::DestroyLogSystem();
		LuaInit::delSingletonPtr();
		return 1;
	}

	//TSun::Log::WriteLine(TSun::Log::LOG_LEVEL_ERROR, TSun::TTRUE, L"你好你是谁是啊");

#ifdef BUILD_TEST
	if (bTest)
	{
		re = runTest(hInstance, lpCmLine, nCmdShow);
	}
#endif // BUILD_TEST

	// run
	re = WindowLoop();

	// destroy
	GlobleClass::getSingletonPtr()->DestroyGlobleClass();
	GlobleClass::delSingletonPtr();
	ConsoleWindow::getSingletonPtr()->DestroyWindow();
	ConsoleWindow::delSingletonPtr();
	TSun::Log::DestroyLogSystem();
	LuaInit::delSingletonPtr();

	// memory management
	_globalAllocator.destroy();
	return re;
}