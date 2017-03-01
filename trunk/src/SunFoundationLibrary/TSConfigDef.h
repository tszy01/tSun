#ifndef _TSCONFIGDEF_H_
#define _TSCONFIGDEF_H_

// memory allocator type
// 1 : sun
// 2 : system
#define MEM_ALLOCATOR_TYPE 1

#define MEM_ALLOCATOR_LOG

#ifdef MEM_ALLOCATOR_LOG
// log level
// 1 : all
// 2 : error
#define MEM_ALLOCATOR_LOG_LEVEL 1
#endif

#endif