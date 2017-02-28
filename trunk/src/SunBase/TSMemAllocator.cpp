#include "TSMemAllocator.h"
#include <assert.h>

namespace TSun {
#if MEM_ALLOCATOR_TYPE == 1
	MemAllocator::MemAllocator() : _name("")
#ifdef MEM_ALLOCATOR_LOG
		, _logFp(TNULL)
#endif
	{
		_memMutex.CreateMutexHandle();
	}


	MemAllocator::~MemAllocator()
	{
		destroy();
	}

	TBOOL MemAllocator::initialize(TSIZE totalSize, const TCHAR* name)
	{
		if (_globalMem._p || _memList.size() > 0)
			return TFALSE;
		if (name != TNULL)
		{
			_name = name;
		}
#ifdef MEM_ALLOCATOR_LOG
		{
			if (_logFp != TNULL)
				return TFALSE;
			TCHAR szName[256] = { 0 };
			sprintf_s(szName, 255, "%s_log.txt", name);
			TS32 re = fopen_s(&_logFp, szName, "at");
			if (re != 0 || _logFp == TNULL)
			{
				return TFALSE;
			}
			log("initialize start", totalSize);
		}
#endif
		TVOID* p = ::malloc(totalSize);
		if (!p)
		{
#ifdef MEM_ALLOCATOR_LOG
			log("malloc failed", totalSize);
#endif
			assert(false);
			return TFALSE;
		}
		memset(p, 0, totalSize);
		_globalMem._p = p;
		_globalMem._size = totalSize;
		addUnusedMemBlock(p, totalSize, _memList.begin());
#ifdef MEM_ALLOCATOR_LOG
#if MEM_ALLOCATOR_LOG_LEVEL == 1
		log("initialize end", totalSize);
#endif
#endif
		return TTRUE;
	}

	void MemAllocator::destroy()
	{
#ifdef MEM_ALLOCATOR_LOG
#if MEM_ALLOCATOR_LOG_LEVEL == 1
		log("destory called", 0);
#endif
		log("starting log unreleased mem info...", 0);
		std::map<void*, MEM_BLOCK>::iterator itr = _memList.begin();
		for(;itr!=_memList.end();++itr)
		{
			if (itr->second._state != MEM_STATE_UNUSED)
			{
				logUnReleased(itr->second._p, itr->second._size, itr->second._file.c_str(), itr->second._line);
			}
		}
		log("log unreleased mem info end", 0);
#endif
		_memMutex.CloseMutexHandle();
		if (_globalMem._p)
		{
			::free(_globalMem._p);
			_globalMem.~MEM_BLOCK();
		}
		_memList.clear();
#ifdef MEM_ALLOCATOR_LOG
		log("destory end", 0);
		if (_logFp)
		{
			::fclose(_logFp);
			_logFp = TNULL;
		}
#endif
	}

	TVOID* MemAllocator::allocateMem(TSIZE size, const TCHAR *file, TS32 line)
	{
		Lock(_memMutex);
		std::map<void*, MEM_BLOCK>::iterator itr;
		if (!findAvailableBlock(size, itr))
		{
#ifdef MEM_ALLOCATOR_LOG
			log("no available memory", size);
			logFileLine(file, line);
#endif
			assert(false);
			return 0;
		}
		if (itr->second._size == size)
		{
			itr->second._state = MEM_STATE_ALLOCATED;
			itr->second._file = std::string(file);
			itr->second._line = line;
			onMemBlockChanged(itr, MEM_STATE_UNUSED, MEM_STATE_ALLOCATED);
			return itr->second._p;
		}
		TVOID* p = itr->second._p;
		TSIZE origSize = itr->second._size;

		itr->second._size = size;
		itr->second._state = MEM_STATE_ALLOCATED;
		itr->second._file = std::string(file);
		itr->second._line = line;
		onMemBlockChanged(itr, MEM_STATE_UNUSED, MEM_STATE_ALLOCATED);

		TVOID* nextHead = (TUByte*)(p) + size;
		TSIZE nextSize = origSize - size;
		addUnusedMemBlock(nextHead, nextSize, itr);

#ifdef MEM_ALLOCATOR_LOG
#if MEM_ALLOCATOR_LOG_LEVEL == 1
		log("memory allocated", size);
		logFileLine(file, line);
#endif
#endif
		return p;
	}

	TBOOL MemAllocator::freeMem(TVOID* p, const TCHAR *file, TS32 line)
	{
		if (!p)
			return TFALSE;
		Lock(_memMutex);
		std::map<void*, MEM_BLOCK>::iterator itrFind = _memList.find(p);
		if (itrFind == _memList.end())
		{
#ifdef MEM_ALLOCATOR_LOG
			log("count find pointer", 0);
			logFileLine(file, line);
#endif
			assert(false);
			return TFALSE;
		}
#ifdef MEM_ALLOCATOR_LOG
#if MEM_ALLOCATOR_LOG_LEVEL == 1
		TSIZE origSize = itrFind->second._size;
#endif
#endif
		bool needAddUnused = true;
		std::map<void*, MEM_BLOCK>::iterator itr = itrFind;
		while (true)
		{
			if (itr == _memList.begin())
			{
				break;
			}
			--itr;
			if (itr->second._state != MEM_STATE_UNUSED)
			{
				break;
			}
			else
			{
				itrFind->second._state = MEM_STATE_UNUSED;
				itrFind = itr;
				needAddUnused = false;
			}
		}
		itr = itrFind;
		++itr;
		for (; itr != _memList.end();)
		{
			if (itr->second._state == MEM_STATE_UNUSED)
			{
				itrFind->second._size += itr->second._size;
				onMemBlockChanged(itr, MEM_STATE_UNUSED, MEM_STATE_DELETED);
				itr = _memList.erase(itr);
			}
			else
			{
				break;
			}
		}
		itrFind->second._state = MEM_STATE_UNUSED;
		itrFind->second._file = std::string("");
		itrFind->second._line = 0;
		memset(itrFind->second._p, 0, itrFind->second._size);
		if (needAddUnused)
		{
			onMemBlockChanged(itrFind, MEM_STATE_ALLOCATED, MEM_STATE_UNUSED);
		}
#ifdef MEM_ALLOCATOR_LOG
#if MEM_ALLOCATOR_LOG_LEVEL == 1
		log("memory freed", origSize);
		logFileLine(file, line);
#endif
#endif
		return TTRUE;
	}

	TBOOL MemAllocator::addUnusedMemBlock(TVOID* p, TSIZE size, std::map<void*, MEM_BLOCK>::iterator& itrWhere)
	{
		if (!p)
			return TFALSE;
		std::map<void*, MEM_BLOCK>::iterator itr = _memList.insert(itrWhere, std::pair<void*, MEM_BLOCK>(p, MEM_BLOCK(p, size, MEM_STATE_UNUSED)));
		onMemBlockChanged(itr, MEM_STATE_UNUSED, MEM_STATE_UNUSED);
		return TTRUE;
	}

	TVOID MemAllocator::onMemBlockChanged(std::map<void*, MEM_BLOCK>::iterator& itrWhere, MEM_STATE oldState, MEM_STATE newState)
	{
		if (newState == MEM_STATE_UNUSED)
		{
			_unusedList.push_back(itrWhere);
		}
		else if (newState == MEM_STATE_ALLOCATED || newState == MEM_STATE_DELETED)
		{
			std::list<std::map<void*, MEM_BLOCK>::iterator>::iterator itrUnused = _unusedList.begin();
			for (; itrUnused != _unusedList.end(); ++itrUnused)
			{
				if ((*itrUnused) == itrWhere)
				{
					_unusedList.erase(itrUnused);
					break;
				}
			}
		}
		
	}

	TBOOL MemAllocator::findAvailableBlock(TSIZE size, std::map<void*, MEM_BLOCK>::iterator& itrFind)
	{
		TBOOL find = TFALSE;
		TSIZE findSize = 0;
		std::list<std::map<void*, MEM_BLOCK>::iterator>::iterator itrUnused = _unusedList.begin();
		std::list<std::map<void*, MEM_BLOCK>::iterator>::iterator itrFindA = _unusedList.end();
		for (; itrUnused != _unusedList.end(); ++itrUnused)
		{
			if ((*itrUnused)->second._size == size)
			{
				itrFind = (*itrUnused);
				return TTRUE;
			}
			else if ((*itrUnused)->second._size >= size)
			{
				if (find == TFALSE)
				{
					itrFindA = itrUnused;
					find = TTRUE;
					findSize = (*itrUnused)->second._size;
				}
				else
				{
					if ((*itrUnused)->second._size < findSize)
					{
						itrFindA = itrUnused;
						findSize = (*itrUnused)->second._size;
					}
				}
			}
		}

		if (find)
		{
			itrFind = (*itrFindA);
			return TTRUE;
		}
		return TFALSE;
	}

#ifdef MEM_ALLOCATOR_LOG
	TVOID MemAllocator::log(const TCHAR* str, TSIZE size)
	{
		if (!str || !_logFp)
			return;
		//SYSTEMTIME sysTime;
		//::GetLocalTime(&sysTime);
		//TCHAR szTmp[16] = { 0 };
		//sprintf_s(szTmp, 15, "%d:%d:%d\n", sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
		//fwrite(szTmp, sizeof(TCHAR), strlen(szTmp), _logFp);
		TCHAR szTmp2[256] = { 0 };
		sprintf_s(szTmp2, 255, "%s size:%llu\n", str, size);
		fwrite(szTmp2, sizeof(TCHAR), strlen(szTmp2), _logFp);
	}

	TVOID MemAllocator::logFileLine(const TCHAR *file, TS32 line)
	{
		if (!file || !_logFp)
			return;
		TCHAR szTmp2[512] = { 0 };
		sprintf_s(szTmp2, 511, "file:%s, line:%d\n", file, line);
		fwrite(szTmp2, sizeof(TCHAR), strlen(szTmp2), _logFp);
	}

	TVOID MemAllocator::logUnReleased(TVOID* p, TSIZE size, const TCHAR *file, TS32 line)
	{
		if (!file || !_logFp)
			return;
		TCHAR szTmp2[1024] = { 0 };
		sprintf_s(szTmp2, 1023, "memory addr:%p, size:%llu, file:%s, line:%d\n", p, size, file, line);
	}
#endif
#else
	MemAllocator::MemAllocator() : _name("")
	#ifdef MEM_ALLOCATOR_LOG
	, _logFp(TNULL)
	#endif
	{
		_memMutex.CreateMutexHandle();
	}


	MemAllocator::~MemAllocator()
	{
		destroy();
	}

	TBOOL MemAllocator::initialize(TSIZE totalSize, const TCHAR* name)
	{
		if (name != TNULL)
		{
			_name = name;
		}
#ifdef MEM_ALLOCATOR_LOG
		{
			if (_logFp != TNULL)
				return TFALSE;
			TCHAR szName[256] = { 0 };
			sprintf_s(szName, 255, "%s_log.txt", name);
			TS32 re = fopen_s(&_logFp, szName, "at");
			if (re != 0 || _logFp == TNULL)
			{
				return TFALSE;
			}
			log("initialize start", totalSize);
		}
#endif
#ifdef MEM_ALLOCATOR_LOG
#if MEM_ALLOCATOR_LOG_LEVEL == 1
		log("initialize end", totalSize);
#endif
#endif
		return TTRUE;
	}

	void MemAllocator::destroy()
	{
#ifdef MEM_ALLOCATOR_LOG
#if MEM_ALLOCATOR_LOG_LEVEL == 1
		log("destory called", 0);
#endif
#endif
		_memMutex.CloseMutexHandle();
#ifdef MEM_ALLOCATOR_LOG
		log("destory end", 0);
		if (_logFp)
		{
			::fclose(_logFp);
			_logFp = TNULL;
		}
#endif
	}

	TVOID* MemAllocator::allocateMem(TSIZE size, const TCHAR *file, TS32 line)
	{
		Lock(_memMutex);
		TVOID* p = ::malloc(size);
		if (!p)
		{
#ifdef MEM_ALLOCATOR_LOG
			log("no available memory", size);
			logFileLine(file, line);
#endif
			assert(false);
			return 0;
		}
#ifdef MEM_ALLOCATOR_LOG
#if MEM_ALLOCATOR_LOG_LEVEL == 1
		log("memory allocated", size);
		logFileLine(file, line);
#endif
#endif
		return p;
	}

	TBOOL MemAllocator::freeMem(TVOID* p, const TCHAR *file, TS32 line)
	{
		if (!p)
			return TFALSE;
		Lock(_memMutex);
		free(p);
#ifdef MEM_ALLOCATOR_LOG
#if MEM_ALLOCATOR_LOG_LEVEL == 1
		log("memory freed", 0);
		logFileLine(file, line);
#endif
#endif
		return TTRUE;
	}

#ifdef MEM_ALLOCATOR_LOG
	TVOID MemAllocator::log(const TCHAR* str, TSIZE size)
	{
		if (!str || !_logFp)
			return;
		//SYSTEMTIME sysTime;
		//::GetLocalTime(&sysTime);
		//TCHAR szTmp[16] = { 0 };
		//sprintf_s(szTmp, 15, "%d:%d:%d\n", sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
		//fwrite(szTmp, sizeof(TCHAR), strlen(szTmp), _logFp);
		TCHAR szTmp2[256] = { 0 };
		sprintf_s(szTmp2, 255, "%s size:%llu\n", str, size);
		fwrite(szTmp2, sizeof(TCHAR), strlen(szTmp2), _logFp);
	}

	TVOID MemAllocator::logFileLine(const TCHAR *file, TS32 line)
	{
		if (!file || !_logFp)
			return;
		TCHAR szTmp2[512] = { 0 };
		sprintf_s(szTmp2, 511, "file:%s, line:%d\n", file, line);
		fwrite(szTmp2, sizeof(TCHAR), strlen(szTmp2), _logFp);
	}

	TVOID MemAllocator::logUnReleased(TVOID* p, TSIZE size, const TCHAR *file, TS32 line)
	{
		if (!file || !_logFp)
			return;
		TCHAR szTmp2[1024] = { 0 };
		sprintf_s(szTmp2, 1023, "memory addr:%p, size:%llu, file:%s, line:%d\n", p, size, file, line);
	}
#endif
#endif
}