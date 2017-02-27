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
		addMemBlock(p, totalSize, MEM_STATE_UNUSED, _memList.begin());
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
		std::list<MEM_BLOCK>::iterator itr;
		if (!findAvailableBlock(size, itr))
			return 0;
		if (itr->_size == size)
		{
			itr->_state = MEM_STATE_ALLOCATED;
			return itr->_p;
		}
		TVOID* nextHead = (TUByte*)(itr->_p) + size;
		size_t nextSize = itr->_size - size;
		addMemBlock(nextHead, nextSize, MEM_STATE_UNUSED, itr);

		itr->_size = size;
		itr->_state = MEM_STATE_ALLOCATED;
		return itr->_p;
	}

	TBOOL MemAllocator::freeMem(TVOID* p)
	{
		if (!p)
			return TFALSE;
		Lock(_memMutex);
		TBOOL find = TFALSE;
		std::list<MEM_BLOCK>::iterator itr = _memList.begin();
		std::list<MEM_BLOCK>::iterator itrFind = _memList.end();
		for (; itr != _memList.end();)
		{
			if (itr->_p == p)
			{
				itr->_state = MEM_STATE_UNUSED;
				find = TTRUE;
				itrFind = itr;
				++itr;
			}
			else
			{
				if (find)
				{
					if (itr->_state == MEM_STATE_UNUSED)
					{
						itrFind->_size += itr->_size;
						itr = _memList.erase(itr);
					}
					else
					{
						break;
					}
				}
				else
				{
					++itr;
				}
			}
		}
		if (!find)
			return TFALSE;
		itrFind->_state = MEM_STATE_UNUSED;
		memset(itrFind->_p, 0, itrFind->_size);
		return TTRUE;
	}

	TBOOL MemAllocator::addMemBlock(TVOID* p, TSIZE size, MemAllocator::MEM_STATE state, std::list<MEM_BLOCK>::iterator& itrWhere)
	{
		if (!p)
			return TFALSE;
		_memList.insert(itrWhere, MEM_BLOCK(p, size, state));
		return TTRUE;
	}

	TBOOL MemAllocator::findAvailableBlock(TSIZE size, std::list<MEM_BLOCK>::iterator& itrFind)
	{
		std::list<MEM_BLOCK>::iterator itr = _memList.begin();
		for (; itr != _memList.end(); ++itr)
		{
			if (itr->_state == MEM_STATE_UNUSED)
			{
				if (itr->_size == size)
				{
					itrFind = itr;
					return TTRUE;
				}
			}
		}
		itr = _memList.begin();
		for (; itr != _memList.end(); ++itr)
		{
			if (itr->_state == MEM_STATE_UNUSED)
			{
				if (itr->_size >= size)
				{
					itrFind = itr;
					return TTRUE;
				}
			}
		}
		return TFALSE;
	}
}