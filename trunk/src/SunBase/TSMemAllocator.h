#ifndef _TSMEMALLOCATOR_H_
#define _TSMEMALLOCATOR_H_

#include "TSCommonTypeDef.h"
#include "TSLock.h"
#include <list>
#include <map>

namespace TSun {
	class MemAllocator
	{
	public:
		MemAllocator();
		~MemAllocator();
	private:
		enum MEM_STATE
		{
			MEM_STATE_UNUSED = 1,
			MEM_STATE_ALLOCATED = 2,
			MEM_STATE_DELETED = 3,
		};
		struct MEM_BLOCK
		{
			TVOID* _p;
			TSIZE _size;
			MEM_STATE _state;

			MEM_BLOCK() : _p(0), _size(0), _state(MEM_STATE_UNUSED)
			{

			}

			MEM_BLOCK(TVOID* p, TSIZE size, MEM_STATE state) :
				_p(p), _size(size), _state(state)
			{

			}

			~MEM_BLOCK()
			{
				_p = 0;
				_size = 0;
				_state = MEM_STATE_UNUSED;
			}
		};
	private:
		std::map<void*, MEM_BLOCK> _memList;
		std::list<std::map<void*, MEM_BLOCK>::iterator> _unusedList;
		MEM_BLOCK _globalMem;
		Mutex _memMutex;
	public:
		TBOOL initialize(TSIZE totalSize);
		TVOID destroy();
		TVOID* allocateMem(TSIZE size);
		TBOOL freeMem(TVOID* p);
	private:
		TBOOL addUnusedMemBlock(TVOID* p, TSIZE size, std::map<void*, MEM_BLOCK>::iterator& itrWhere);
		TBOOL findAvailableBlock(TSIZE size, std::map<void*, MEM_BLOCK>::iterator& itrFind);
		TVOID onMemBlockChanged(std::map<void*, MEM_BLOCK>::iterator& itrWhere, MEM_STATE oldState, MEM_STATE newState);
	};
}

#endif
