#ifndef _TSMEMDEF_H_
#define _TSMEMDEF_H_

#include "TSMemAllocator.h"

namespace TSun {
	template<typename T>
	T* new_class(TSun::MemAllocator* allocator, const char *file, int line)
	{
		if (!allocator)
		{
			return TSun::TNULL;
		}
		T* newPtr = new (allocator->allocateMem(sizeof(T), file, line)) T;
		return newPtr;
	}

	template<typename T>
	T* new_class_array(TSun::MemAllocator* allocator, TSun::TSIZE count, const char *file, int line)
	{
		if (!allocator || count == 0)
		{
			return TSun::TNULL;
		}
		T* newPtr = new (allocator->allocateMem(sizeof(T) * count + sizeof(TSIZE), file, line)) T[count];
		return newPtr;
	}

	template<typename T>
	TSun::TVOID delete_class(TSun::MemAllocator* allocator, T* p, const char *file, int line)
	{
		if (!allocator || !p)
		{
			return ;
		}
		p->~T();
		allocator->freeMem(p, file, line);
	}

	template<typename T>
	TSun::TVOID beforeDeleteArray(TSun::TVOID* p, TSun::TSIZE size)
	{
		T* tP = (T*)p;
		TSun::TSIZE count = size / sizeof(T);
		for (TSIZE i = 0; i < count; ++i)
		{
			tP[i].~T();
		}
	}

	template<typename T>
	TSun::TVOID delete_class_array(TSun::MemAllocator* allocator, T* p, TSun::TVOID(*beforeFree)(TSun::TVOID*, TSun::TSIZE), const char *file, int line)
	{
		if (!allocator || !p)
		{
			return;
		}
		TSun::TUByte* arrayHead = (TSun::TUByte*)p - sizeof(TSIZE);
		TSun::TSIZE count = 0;
		memcpy(&count, arrayHead, sizeof(TSIZE));
		for (TSIZE i = 0; i < count; ++i)
		{
			p[i].~T();
		}
		allocator->freeMem(arrayHead, file, line);
	}

	// please rewrite this in app
	TSun::MemAllocator* getDefaultMemAllocator();
}

#define T_NEW(A, T) (TSun::new_class<T>(A, __FILE__, __LINE__))
#define T_NEW_ARRAY(A, T, C) (TSun::new_class_array<T>(A, C, __FILE__, __LINE__))
#define T_DELETE(A, T, P) (TSun::delete_class<T>(A, P, __FILE__, __LINE__))
#define T_DELETE_ARRAY(A, T, P) (TSun::delete_class_array<T>(A, P, &(TSun::beforeDeleteArray<T>), __FILE__, __LINE__))

#define T_SAFE_DELETE(A, T, P) \
T_DELETE(A, T, P);\
P = TSun::TNULL;

#define T_SAFE_DELETE_ARRAY(A, T, P) \
T_DELETE_ARRAY(A, T, P);\
P = TSun::TNULL;

#define DEFINE_MEM_ALLOCATOR_MEMBER \
protected: \
	TSun::MemAllocator* m_allocator; \
public: \
	TSun::MemAllocator* getMemAllocator() const \
	{ \
		if(m_allocator == TSun::TNULL) \
		{ \
			return TSun::getDefaultMemAllocator(); \
		} \
		return m_allocator; \
	}
#endif