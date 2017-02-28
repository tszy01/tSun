#include "TSMemAllocator.h"

namespace TSun {
	MemAllocator::MemAllocator()
	{
		_memMutex.CreateMutexHandle();
	}


	MemAllocator::~MemAllocator()
	{
		destroy();
	}

	TBOOL MemAllocator::initialize(TSIZE totalSize)
	{
		if (_globalMem._p || _memList.size() > 0)
			return TFALSE;
		TVOID* p = ::malloc(totalSize);
		if (!p)
			return TFALSE;
		memset(p, 0, totalSize);
		_globalMem._p = p;
		_globalMem._size = totalSize;
		addUnusedMemBlock(p, totalSize, _memList.begin());
		return TTRUE;
	}

	void MemAllocator::destroy()
	{
		_memMutex.CloseMutexHandle();
		if (_globalMem._p)
		{
			::free(_globalMem._p);
			_globalMem.~MEM_BLOCK();
		}
		_memList.clear();
	}

	TVOID* MemAllocator::allocateMem(TSIZE size)
	{
		Lock(_memMutex);
		std::map<void*, MEM_BLOCK>::iterator itr;
		if (!findAvailableBlock(size, itr))
			return 0;
		if (itr->second._size == size)
		{
			itr->second._state = MEM_STATE_ALLOCATED;
			onMemBlockChanged(itr, MEM_STATE_UNUSED, MEM_STATE_ALLOCATED);
			return itr->second._p;
		}
		TVOID* p = itr->second._p;
		TSIZE origSize = itr->second._size;

		itr->second._size = size;
		itr->second._state = MEM_STATE_ALLOCATED;
		onMemBlockChanged(itr, MEM_STATE_UNUSED, MEM_STATE_ALLOCATED);

		TVOID* nextHead = (TUByte*)(p) + size;
		TSIZE nextSize = origSize - size;
		addUnusedMemBlock(nextHead, nextSize, itr);
		
		return p;
	}

	TBOOL MemAllocator::freeMem(TVOID* p)
	{
		if (!p)
			return TFALSE;
		Lock(_memMutex);
		std::map<void*, MEM_BLOCK>::iterator itrFind = _memList.find(p);
		if (itrFind == _memList.end())
			return TFALSE;
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
		memset(itrFind->second._p, 0, itrFind->second._size);
		if (needAddUnused)
		{
			onMemBlockChanged(itrFind, MEM_STATE_ALLOCATED, MEM_STATE_UNUSED);
		}
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
}