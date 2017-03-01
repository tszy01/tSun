#ifndef _TSAUTOPTR_H_
#define _TSAUTOPTR_H_

#include "TSMemDef.h"

namespace TSun{

	template<typename T>
	class AutoPtr
	{
	public:
		enum FREE_METHOD
		{
			FREE_METHOD_DELETE,
			FREE_METHOD_DELETE_T,
			FREE_METHOD_FREE
		};
	public:
		AutoPtr(MemAllocator* allocator) : m_pPtr(0), m_FreeMethod(FREE_METHOD_DELETE), m_allocator(allocator)
		{
		}

		AutoPtr(const T* ptr, FREE_METHOD freeMethod, MemAllocator* allocator) : m_pPtr(ptr), m_FreeMethod(freeMethod), m_allocator(allocator)
		{
		}

		AutoPtr(const AutoPtr<T>& other) : m_pPtr(other.m_pPtr), m_FreeMethod(other.m_FreeMethod), m_allocator(other.m_allocator)
		{
		}

		TVOID destroy()
		{
			if (m_pPtr)
			{
				switch (m_FreeMethod)
				{
				case FREE_METHOD_DELETE:
					T_DELETE(getMemAllocator(), T, m_pPtr);
					break;
				case FREE_METHOD_DELETE_T:
					T_DELETE_ARRAY(getMemAllocator(), T, m_pPtr);
					break;
				case FREE_METHOD_FREE:
					getMemAllocator()->freeMem(m_pPtr, __FILE__, __LINE__);
					break;
				}

				m_pPtr = 0;
			}
		}

		~AutoPtr(TVOID)
		{
			destroy();
			m_allocator = 0;
		}
	protected:
		T* m_pPtr;
		FREE_METHOD m_FreeMethod;
		// memory allocator
		DEFINE_MEM_ALLOCATOR_MEMBER;
	public:
		inline AutoPtr<T>& operator=(const AutoPtr<T>& other)
		{
			if(other.GetPtr() == m_pPtr)
				return *this;

			destroy();
			m_pPtr = other.m_pPtr;
			m_FreeMethod = other.m_FreeMethod;
			m_allocator = other.m_allocator;
		}

		inline TVOID bind(const T* ptr, MemAllocator* allocator, FREE_METHOD freeMethod) {
			if (ptr == m_pPtr)
				return;
			m_allocator = allocator;
			destroy();
			m_pPtr = ptr;
			m_FreeMethod = freeMethod;
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