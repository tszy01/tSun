#ifndef _TSCONFIGDEF_H_
#define _TSCONFIGDEF_H_

//#define USE_SUN_ALLOCATOR

#ifdef USE_SUN_ALLOCATOR

#define MEM_ALLOCATOR_LOG

// log level
// 1 : all
// 2 : error
#define MEM_ALLOCATOR_LOG_LEVEL 1

#endif

#endif