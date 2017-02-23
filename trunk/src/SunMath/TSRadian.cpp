#include "TSRadian.h"
#include "TSDegree.h"
#include "TSFPU.h"

namespace TSun
{
	Radian::Radian ( const Degree& d ) : mRad(d.valueRadians())
	{
	}

	Radian& Radian::operator = ( const Degree& d ) {
		mRad = d.valueRadians(); return *this;
	}

	Radian Radian::operator + ( const Degree& d ) const {
		return Radian ( mRad + d.valueRadians() );
	}

	Radian& Radian::operator += ( const Degree& d ) {
		mRad += d.valueRadians();
		return *this;
	}

	Radian Radian::operator - ( const Degree& d ) const {
		return Radian ( mRad - d.valueRadians() );
	}

	Radian& Radian::operator -= ( const Degree& d ) {
		mRad -= d.valueRadians();
		return *this;
	}

	TF32 Radian::valueDegrees() const
	{
		return mRad * RADTODEG;
	}
}