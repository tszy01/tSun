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

#endif

#endif
