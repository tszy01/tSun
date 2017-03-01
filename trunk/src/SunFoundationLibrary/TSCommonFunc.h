#ifndef _TSCOMMONFUNC_H_
#define _TSCOMMONFUNC_H_

#include "TSCommonTypeDef.h"

namespace TSun {
	template<typename T> void swap(T& l, T& r)
	{
		T tmp = l;
		l = r;
		r = tmp;
	}

	template<typename T> TBOOL less(const T& l, const T& r)
	{
		if (l < r)
		{
			return TTRUE;
		}
		return TFALSE;
	}

	/** Determines whether the number is power-of-two or not.
	@note 0 and 1 are tread as power of two.
	*/
	template<typename T> TBOOL isPO2(T n)
	{
		return (n & (n - 1)) == 0;
	}
}
#endif