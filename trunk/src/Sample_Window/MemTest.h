#ifndef _MEMTEST_H_
#define _MEMTEST_H_

#include "ConfigDef.h"
#include "TSMemDef.h"
#include "TSCommonTypeDef.h"

#ifdef BUILD_TEST
class MemTest
{
public:
	MemTest();
	~MemTest();
private:
	TSun::TS32 mA;
public:
	TSun::TVOID setA(TSun::TS32 value)
	{
		mA = value;
	}
};

//template<>
//MemTest* new_class(TSun::MemAllocator* allocator, const char *file, int line)
//{
//	if (!allocator)
//	{
//		return TSun::TNULL;
//	}
//	MemTest* newPtr = new (allocator->allocateMem(sizeof(MemTest))) MemTest();
//	return newPtr;
//};
//
//template<>
//TSun::TVOID delete_class(TSun::MemAllocator* allocator, MemTest* p, const char *file, int line)
//{
//	if (!allocator || !p)
//	{
//		return;
//	}
//	p->~MemTest();
//	allocator->freeMem(p);
//};

#endif

#endif
