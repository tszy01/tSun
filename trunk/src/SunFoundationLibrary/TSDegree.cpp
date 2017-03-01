#include "TSDegree.h"
#include "TSRadian.h"
#include "TSFPU.h"

namespace TSun
{
	Degree::Degree ( const Radian& r ) : mDeg(r.valueDegrees())
	{
	}

	Degree& Degree::operator = ( const Radian& r )
	{
		mDeg = r.valueDegrees(); 
		return *this;
	}

	TF32 Degree::valueRadians() const
	{
		return mDeg * DEGTORAD;
	}

	Degree Degree::operator + ( const Radian& r ) const
	{
		return Degree ( mDeg + r.valueDegrees() );
	}

	Degree& Degree::operator += ( const Radian& r )
	{
		mDeg += r.valueDegrees(); 
		return *this;
	}

	Degree Degree::operator - ( const Radian& r ) const
	{
		return Degree ( mDeg - r.valueDegrees() );
	}

	Degree& Degree::operator -= ( const Radian& r )
	{
		mDeg -= r.valueDegrees(); 
		return *this;
	}
}