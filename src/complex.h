// ----------------------------------------------------------------------------

// Copyright (C) 2006  Ben Deane (ben@elbeno.com)

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

// ----------------------------------------------------------------------------

#ifndef _COMPLEX_H_
#define _COMPLEX_H_

// ----------------------------------------------------------------------------

#include <cmath>
#include <cassert>

// ----------------------------------------------------------------------------

template <typename T>
class Complex
{
	public:
		explicit Complex(T real, T imag = 0) : mReal(real), mImag(imag) {}
		
		inline T modulus_squared() const { return mReal * mReal + mImag * mImag; }
		inline T modulus() const { return sqrt(modulus_squared()); }
		inline T argument() const { return atan(mImag/mReal); }

		inline Complex conjugate() const { return Complex(mReal, -mImag); }
		
		template <typename T1>
		friend Complex<T1> operator+(Complex<T1> a, Complex<T1> b);
		template <typename T1>
		friend Complex<T1> operator-(Complex<T1> a, Complex<T1> b);
		template <typename T1>
		friend Complex<T1> operator*(Complex<T1> a, Complex<T1> b);
		template <typename T1>
		friend Complex<T1> operator/(Complex<T1> a, Complex<T1> b);
		
		inline void operator+=(const Complex a) const { mReal += a.mReal; mImag += a.mImag; }
		inline void operator-=(const Complex a) const { mReal -= a.mReal; mImag -= a.mImag; }
		inline void operator*=(const Complex a) const { T tmp = mReal; mReal = mReal*a.mReal - mImag*a.mImag; mImag = mImag*a.mReal + tmp*a.mImag; }
		inline void operator/=(const Complex a) const { assert(a.modulus_squared() != 0); T tmp = mReal; mReal = (mReal*a.mReal + mImag*a.mImag)/a.modulus_squared(); mImag = (mImag*a.mReal - tmp*a.mImag)/a.modulus_squared(); }
		inline bool operator==(const Complex a) const { return a.mReal == mReal && a.mImag == mImag; }
		inline bool operator!=(const Complex a) const { return !(a == *this); }
		
	private:
		T mReal;
		T mImag;
};

// ----------------------------------------------------------------------------

template <typename T>
inline Complex<T> operator+(Complex<T> a, Complex<T> b)
{
	return Complex<T>(a.mReal+b.mReal, a.mImag+b.mImag);
}

template <typename T>
inline Complex<T> operator-(Complex<T> a, Complex<T> b)
{
	return Complex<T>(a.mReal-b.mReal, a.mImag-b.mImag);
}

template <typename T>
inline Complex<T> operator*(Complex<T> a, Complex<T> b)
{
	return Complex<T>(a.mReal*b.mReal - a.mImag*b.mImag, 
					  a.mImag*b.mReal + a.mReal*b.mImag);
}

template <typename T>
inline Complex<T> operator/(Complex<T> a, Complex<T> b)
{
	assert(b.modulus_squared() != 0);
	return Complex<T>((a.mReal*b.mReal + a.mImag*b.mImag) / b.modulus_squared(), 
					  (a.mImag*b.mReal - a.mReal*b.mImag) / b.modulus_squared());
}

// ----------------------------------------------------------------------------

#endif //_COMPLEX_H_
