﻿#ifndef _TSLOCK_H_
#define _TSLOCK_H_
#include "TSMutex.h"

namespace TSun {
	class Lock
	{
	public:

		Lock(TVOID)
		{
		}

		~Lock(TVOID)
		{
			UnlockMutex();
		}

	private:
		Mutex mMutex;
	public:
		TBOOL LockMutex(const Mutex& mutex)
		{
			if(mMutex.IsNull()==TFALSE)
				return TFALSE;
			DWORD dwWaitResult = ::WaitForSingleObject( 
				mutex.GetMutexHandle(),    // handle to mutex
				INFINITE);  // no time-out interval
 
			switch (dwWaitResult) 
			{
				// The thread got ownership of the mutex
				case WAIT_OBJECT_0:
					{
						mMutex = mutex;
					}
					break;
				default:
					return TFALSE;
			}
			return TTRUE;
		}

		TVOID UnlockMutex()
		{
			if(mMutex.IsNull()==TFALSE)
			{
				::ReleaseMutex(mMutex.GetMutexHandle());
				mMutex.SetNull();
			}
		}
	};

}

#endif