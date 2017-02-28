#ifndef _TSAUTOPTR_H_
#define _TSAUTOPTR_H_

#include "TSMemDef.h"

namespace TSun{

	template<typename T>
	class AutoPtr
	{
	public:
		AutoPtr(MemAllocator* allocator = getDefaultMemAllocator()) : m_pPtr(0), m_allocator(allocator)
		{
		}

		AutoPtr(const T* ptr, MemAllocator* allocator) : m_pPtr(ptr), m_allocator(allocator)
		{
		}

		AutoPtr(const AutoPtr<T>& other) : m_pPtr(other.m_pPtr), m_allocator(other.m_allocator)
		{
		}

		~AutoPtr(TVOID)
		{
			if(m_pPtr)
			{
				T_DELETE(getMemAllocator(),T,m_pPtr);
				m_pPtr = 0;
			}
			m_allocator = 0;
		}
	protected:
		T* m_pPtr;
		// memory allocator
		DEFINE_MEM_ALLOCATOR_MEMBER;
	public:
		inline AutoPtr<T>& operator=(const AutoPtr<T>& other)
		{
			if(other.GetPtr() == m_pPtr)
				return *this;

			if(m_pPtr)
			{
				T_DELETE(getMemAllocator(), T, m_pPtr);
				m_pPtr = 0;
			}
			m_pPtr = other.m_pPtr;
			m_allocator = other.m_allocator;
		}

		inline AutoPtr<T>& operator=(const T* ptr)
		{
			if(ptr == m_pPtr)
				return *this;

			if(m_pPtr)
			{
				T_DELETE(getMemAllocator(), T, m_pPtr);
				m_pPtr = 0;
			}
			m_pPtr = ptr;
			m_allocator = other.m_allocator;
		}

		inline TBOOL IsNullPtr() const
		{
			return m_pPtr==0;
		}

		inline T* GetPtr() const
		{
			return m_pPtr;
		}

		inline T* operator->() const
		{
			return m_pPtr;
		}

		inline T& operator*() const
		{
			return *m_pPtr;
		}
	};
}

#endif