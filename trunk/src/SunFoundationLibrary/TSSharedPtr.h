/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2009 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
#ifndef __TSSHAREDPTR_H__
#define __TSSHAREDPTR_H__

#include "TSLock.h"
#include "TSCommonFunc.h"
#include "TSMemDef.h"
#include <assert.h>

namespace TSun {

	/// The method to use to free memory on destruction
	enum SharedPtrFreeMethod
	{
		SPFM_DELETE,
		SPFM_DELETE_T,
		SPFM_FREE
	};
	template<class T> class SharedPtr
	{
	protected:
		T* pRep;
		TU32* pUseCount;
		SharedPtrFreeMethod useFreeMethod;
		// memory allocator
		DEFINE_MEM_ALLOCATOR_MEMBER
	public:
		Mutex mMutex;

		SharedPtr(MemAllocator* allocator) : 
			pRep(0), pUseCount(0), useFreeMethod(SPFM_DELETE), m_allocator(allocator)
        {
			mMutex.SetNull();
        }

        template< class Y>
		explicit SharedPtr(Y* rep, MemAllocator* allocator, SharedPtrFreeMethod freeMethod = SPFM_DELETE)
			: pRep(rep)
			, pUseCount(0)
			, useFreeMethod(freeMethod)
			, m_allocator(allocator)
		{
			mMutex.SetNull();
			//OGRE_NEW_AUTO_SHARED_MUTEX
			// change ogre 1.7
			// 这里改成如果有指针，也就是如果有usecount，才构造，否则构造出来的mutex就不会被释放了
			if(rep)
			{
				pUseCount = T_NEW(getMemAllocator(), TU32);
				(*pUseCount) = 1;
				mMutex.CreateMutexHandle();
			}
		}
		SharedPtr(const SharedPtr& r)
            : pRep(0), pUseCount(0), useFreeMethod(SPFM_DELETE), m_allocator(r.m_allocator)
		{
			// lock & copy other mutex pointer
            
			mMutex.SetNull();
			if(r.mMutex.IsNull()==TFALSE)
            {
				Lock lock;
				lock.LockMutex(r.mMutex);
				mMutex.CopyMutexHandle(r.mMutex);
			    pRep = r.pRep;
			    pUseCount = r.pUseCount; 
				useFreeMethod = r.useFreeMethod;
			    // Handle zero pointer gracefully to manage STL containers
			    if(pUseCount)
			    {
				    ++(*pUseCount); 
			    }
            }
		}
		SharedPtr& operator=(const SharedPtr& r) {
			if (pRep == r.pRep)
				return *this;
			// Swap current data into a local copy
			// this ensures we deal with rhs and this being dependent
			SharedPtr<T> tmp(r);
			swap(tmp);
			return *this;
		}
		
		template< class Y>
		SharedPtr(const SharedPtr<Y>& r)
            : pRep(0), pUseCount(0), useFreeMethod(SPFM_DELETE), m_allocator(r.m_allocator)
		{
			// lock & copy other mutex pointer

			mMutex.SetNull();
			if(r.mMutex.IsNull()==TFALSE)
            {
				Lock lock;
				lock.LockMutex(r.mMutex);
				mMutex.CopyMutexHandle(r.mMutex);
			    pRep = r.getPointer();
			    pUseCount = r.useCountPointer();
				useFreeMethod = r.freeMethod();
			    // Handle zero pointer gracefully to manage STL containers
			    if(pUseCount)
			    {
				    ++(*pUseCount);
			    }
            }
		}
		template< class Y>
		SharedPtr& operator=(const SharedPtr<Y>& r) {
			if (pRep == r.getPointer())
				return *this;
			// Swap current data into a local copy
			// this ensures we deal with rhs and this being dependent
			SharedPtr<T> tmp(r);
			swap(tmp);
			return *this;
		}
		virtual ~SharedPtr() {
            release();
			m_allocator = 0;
		}


		inline T& operator*() const { assert(pRep); return *pRep; }
		inline T* operator->() const { assert(pRep); return pRep; }
		inline T* get() const { return pRep; }

		/** Binds rep to the SharedPtr.
			@remarks
				Assumes that the SharedPtr is uninitialised!
		*/
		TVOID bind(T* rep, MemAllocator* allocator, SharedPtrFreeMethod freeMethod = SPFM_DELETE) {
			assert(!pRep && !pUseCount && mMutex.IsNull());
			mMutex.CreateMutexHandle();
			Lock lock;
			lock.LockMutex(mMutex);
			m_allocator = allocator;
			pUseCount = T_NEW(getMemAllocator(), TU32);
			(*pUseCount) = 1;
			pRep = rep;
			useFreeMethod = freeMethod;
		}

		inline TBOOL unique() const 
		{ 
			Lock lock;
			lock.LockMutex(mMutex); 
			assert(pUseCount); 
			return *pUseCount == 1; 
		}
		inline TU32 useCount() const
		{
			Lock lock;
			lock.LockMutex(mMutex); 
			assert(pUseCount); 
			return *pUseCount; 
		}
		inline TU32* useCountPointer() const { return pUseCount; }

		inline T* getPointer() const { return pRep; }
		inline SharedPtrFreeMethod freeMethod() const { return useFreeMethod; }

		inline TBOOL isNull(TVOID) const { return pRep == 0; }

        inline TVOID setNull(TVOID) { 
			if (pRep)
			{
				// can't scope lock mutex before release in case deleted
				release();
				pRep = 0;
				pUseCount = 0;
			}
        }

    protected:

        inline TVOID release(TVOID)
        {
			TBOOL destroyThis = TFALSE;

            /* If the mutex is not initialized to a non-zero value, then
               neither is pUseCount nor pRep.
             */

            if(mMutex.IsNull()==TFALSE)
			{
				// lock own mutex in limited scope (must unlock before destroy)
				Lock lock;
				lock.LockMutex(mMutex); 
				if (pUseCount)
				{
					if (--(*pUseCount) == 0) 
					{
						destroyThis = TTRUE;
	                }
				}
            }
			if (destroyThis)
				destroy();

			mMutex.SetNull();
        }

        virtual TVOID destroy(TVOID)
        {
            // IF YOU GET A CRASH HERE, YOU FORGOT TO FREE UP POINTERS
            // BEFORE SHUTTING OGRE DOWN
            // Use setNull() before shutdown or make sure your pointer goes
            // out of scope before OGRE shuts down to avoid this.
			switch(useFreeMethod)
			{
			case SPFM_DELETE:
				T_DELETE(getMemAllocator(),T,pRep);
				break;
			case SPFM_DELETE_T:
				T_DELETE_ARRAY(getMemAllocator(), T, pRep);
				break;
			case SPFM_FREE:
				getMemAllocator()->freeMem(pRep, __FILE__, __LINE__);
				break;
			};
			// use OGRE_FREE instead of OGRE_DELETE_T since 'unsigned int' isn't a destructor
			// we only used OGRE_NEW_T to be able to use constructor
			T_DELETE(getMemAllocator(), TU32, pUseCount);
			mMutex.CloseMutexHandle();
        }

		virtual TVOID swap(SharedPtr<T> &other) 
		{
			TSun::swap(pRep, other.pRep);
			TSun::swap(pUseCount, other.pUseCount);
			TSun::swap(useFreeMethod, other.useFreeMethod);
			mMutex.SwapMutexHandle(other.mMutex);
		}
	};

	template<class T, class U> inline TBOOL operator==(SharedPtr<T> const& a, SharedPtr<U> const& b)
	{
		return a.get() == b.get();
	}

	template<class T, class U> inline TBOOL operator!=(SharedPtr<T> const& a, SharedPtr<U> const& b)
	{
		return a.get() != b.get();
	}

	template<class T, class U> inline TBOOL operator<(SharedPtr<T> const& a, SharedPtr<U> const& b)
	{
		return TSun::less((TVOID*)a.get(), (TVOID*)b.get());
	}
	/** @} */
	/** @} */
}



#endif
