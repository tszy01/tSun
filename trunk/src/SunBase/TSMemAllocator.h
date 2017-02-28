#ifndef _TSMEMALLOCATOR_H_
#define _TSMEMALLOCATOR_H_

#include "TSConfigDef.h"

#ifdef USE_SUN_ALLOCATOR

#include "TSCommonTypeDef.h"
#include "TSLock.h"
#include <list>
#include <map>
#include <string>

#ifdef MEM_ALLOCATOR_LOG
#include <stdlib.h>
#endif

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
			std::string _file;
			TS32 _line;

			MEM_BLOCK() : _p(0), _size(0), _state(MEM_STATE_UNUSED), _file(""), _line(0)
			{

			}

			MEM_BLOCK(TVOID* p, TSIZE size, MEM_STATE state) :
				_p(p), _size(size), _state(state), _file(""), _line(0)
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
		std::string _name;
#ifdef MEM_ALLOCATOR_LOG
		FILE* _logFp;
#endif
	public:
		TBOOL initialize(TSIZE totalSize, const TCHAR* name);
		TVOID destroy();
		TVOID* allocateMem(TSIZE size, const TCHAR *file = TNULL, TS32 line = 0);
		TBOOL freeMem(TVOID* p, const TCHAR *file = TNULL, TS32 line = 0);
	private:
		TBOOL addUnusedMemBlock(TVOID* p, TSIZE size, std::map<void*, MEM_BLOCK>::iterator& itrWhere);
		TBOOL findAvailableBlock(TSIZE size, std::map<void*, MEM_BLOCK>::iterator& itrFind);
		TVOID onMemBlockChanged(std::map<void*, MEM_BLOCK>::iterator& itrWhere, MEM_STATE oldState, MEM_STATE newState);
#ifdef MEM_ALLOCATOR_LOG
		TVOID log(const TCHAR* str, TSIZE size);
		TVOID logFileLine(const TCHAR *file, TS32 line);
		TVOID logUnReleased(TVOID* p, TSIZE size, const TCHAR *file, TS32 line);
#endif
	};
}

#endif

#endif
