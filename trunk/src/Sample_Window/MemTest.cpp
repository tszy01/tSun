#include "MemTest.h"

#include "TSLog.h"

#ifdef BUILD_TEST

MemTest::MemTest() : mA(0)
{
	TSun::Log::WriteLine(TSun::Log::LOG_LEVEL_INFO, TSun::TTRUE, L"constructor called");
}

MemTest::~MemTest()
{
	TSun::Log::WriteLine(TSun::Log::LOG_LEVEL_INFO, TSun::TTRUE, L"destructor called");
}

#endif