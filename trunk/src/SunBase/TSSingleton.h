#ifndef _TSSINGLETON_H_
#define _TSSINGLETON_H_

#include "TSCommonTypeDef.h"
#include "TSMemDef.h"
namespace TSun {

	template<class T>
	class Singleton
	{
	public:
		Singleton(TVOID)
		{
		}

		virtual ~Singleton(TVOID)
		{
		}
	public:
		static T* getSingletonPtr(MemAllocator* allocator = getDefaultMemAllocator())
		{
			m_allocator = allocator;
			if(!m_Ptr)
				m_Ptr = T_NEW(getMemAllocator(), T);
			return m_Ptr;
		};
		static TVOID delSingletonPtr()
		{
			if(m_Ptr)
			{
				T_DELETE(getMemAllocator(), T, m_Ptr);
				m_Ptr = 0;
			}
		}
	protected:
		static T* m_Ptr;
		DEFINE_MEM_ALLOCATOR_MEMBER;
	};

}

//// Singleton Macro Define
//#define T_SINGLETON_DEF(classname) \
//	private:\
//		static classname* sPtr;\
//	public:\
//		static classname* getSingletonPtr()\
//		{\
//			if(!sPtr)\
//				sPtr = new classname();\
//			return sPtr;\
//		}\
//		static TVOID delSingletonPtr()\
//		{\
//			if(sPtr)\
//				delete sPtr;\
//			sPtr = 0;\
//		}
//
//#define T_SINGLETON_IMP(classname) \
//	classname* classname::sPtr = 0;

#endif