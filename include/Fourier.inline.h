/*******************************************************************************
 File:				Fourier.inline.h

 Author: 			Gaspard Petit (gaspardpetit@gmail.com)
 Last Revision:		March 14, 2007

 This code may be reused without my permission as long as credits are given to
 the original author.  If you find bugs, please send me a note...
*******************************************************************************/
#ifndef __MKCORE_MATH_GEOMETRY_FOURIER_INLINE_H__
#define __MKCORE_MATH_GEOMETRY_FOURIER_INLINE_H__

#include "Fourier.h"
#include <complex>
#include <cmath>

//--------------------------------------------------------------------------
//
template<class T>
void Fourier::forwardTransform2D(std::complex<T> *data, int width, int height)
{
	int rowLength = width;

	for (int i = 0; i < height; ++i)
		fastTransform<T, FORWARD>(&data[i*rowLength], width, 1);

	for (int i = 0; i < width; ++i)
		fastTransform<T, FORWARD>(&data[i], height, rowLength);
}

//--------------------------------------------------------------------------
//
template<class T>
void Fourier::reverseTransform2D(std::complex<T> *data, int width, int height)
{
	int rowLength = width;

	for (int i = 0; i < width; ++i)
		fastTransform<T, REVERSE>(&data[i], height, rowLength);

	for (int i = 0; i < height; ++i)
		fastTransform<T, REVERSE>(&data[i*rowLength], width, 1);
}

//--------------------------------------------------------------------------
//
template<class T, int SIZE>
void Fourier::forwardTransform(std::complex<T> *data)
{
	fastTransform<T, FORWARD, SIZE, 1>(data);
}

//--------------------------------------------------------------------------
//
template<class T>
void Fourier::forwardTransform(std::complex<T> *data, unsigned int size)
{
	fastTransform<T, FORWARD>(data, size, 1);
}

//--------------------------------------------------------------------------
//
template<class T, int SIZE>
void Fourier::reverseTransform(std::complex<T> *data)
{
//		fastTransform<T, REVERSE>(data, SIZE, 1);
	fastTransform<T, REVERSE, SIZE, 1>(data);
}

//--------------------------------------------------------------------------
//
template<class T>
void Fourier::reverseTransform(std::complex<T> *data, unsigned int size)
{
	fastTransform<T, REVERSE>(data, size, 1);
}

template<class T, int SIZE, int LEVEL, int INCREMENT>
void Fourier::Internal<T, SIZE, LEVEL, INCREMENT>::transform(std::complex<T> *data)
{
	static const unsigned int mask = SIZE-1;
	static double sin_t[SIZE];
	static double cos_t[SIZE];
	static const T twoPi = 2*M_PI;
	static const T twoPiOverN = twoPi / static_cast<T>(SIZE);
	static bool inited = false;
	if (!inited)
	{
		for (int i = 0; i < SIZE; ++i)
		{
			cos_t[i] = cos(i*twoPiOverN);
			sin_t[i] = sin(i*twoPiOverN);
		}
		inited = true;
	}

	const int ie = SIZE / LEVEL;
	int ia = 0;

	for (unsigned int j = 0; j < LEVEL/2; j++)
	{
		T cosA = cos_t[ia & mask];
		T sinA = sin_t[ia & mask];
		ia += ie;

		for (unsigned int i = j*INCREMENT; i < SIZE*INCREMENT; i += LEVEL*INCREMENT)
		{
			int i2 = i + LEVEL*INCREMENT/2;

			std::complex<T> temp = data[i] - data[i2];
			data[i] += data[i2];
			data[i2].real() = cosA * temp.real() + sinA * temp.imag();
			data[i2].imag() = cosA * temp.imag() - sinA * temp.real();
		 }
	}

	Fourier::Internal<T, SIZE, LEVEL/2, INCREMENT>::transform(data);
}

template<class T, int SIZE, int INCREMENT>
void Fourier::Internal<T, SIZE, 8, INCREMENT>::transform(std::complex<T> *data)
{
	static double sin_t[SIZE];
	static double cos_t[SIZE];
	static const T twoPi = 2*M_PI;
	static const T twoPiOverN = twoPi / static_cast<T>(SIZE);
	static bool inited = false;
	if (!inited)
	{
		for (int i = 0; i < SIZE; ++i)
		{
			cos_t[i] = cos(i*twoPiOverN);
			sin_t[i] = sin(i*twoPiOverN);
		}
		inited = true;
	}

	Internal2<T, SIZE/4, INCREMENT*4>::transform(data);

	for (unsigned int i = 1*INCREMENT; i < SIZE*INCREMENT; i += 8*INCREMENT)
	{
//				T cosA = M_SQRT1_2 = cos(twoPi / 8);
//				T sinA = M_SQRT1_2 = sin(twoPi / 8);
		int i2 = i + 8*INCREMENT/2;

		std::complex<T> temp = data[i] - data[i2];
		data[i] += data[i2];
		data[i2].real() = M_SQRT1_2 * temp.real() + M_SQRT1_2 * temp.imag();
		data[i2].imag() = M_SQRT1_2 * temp.imag() - M_SQRT1_2 * temp.real();
	}

	Internal4<T, SIZE/2, INCREMENT*2>::transform(data);

	for (unsigned int i = 3*INCREMENT; i < SIZE*INCREMENT; i += 8*INCREMENT)
	{
//				T cosA = -M_SQRT1_2;//cos(3 * twoPi / 8);
//				T sinA = M_SQRT1_2;//sin(3 * twoPi / 8);
		int i2 = i + 8*INCREMENT/2;

		std::complex<T> temp = data[i] - data[i2];
		data[i] += data[i2];
		data[i2].real() = -M_SQRT1_2 * temp.real() + M_SQRT1_2 * temp.imag();
		data[i2].imag() = -M_SQRT1_2 * temp.imag() - M_SQRT1_2 * temp.real();
	}

	Internal<T, SIZE, 4, INCREMENT>::transform(data);
}

template<class T, int SIZE, int INCREMENT>
void Fourier::Internal4<T, SIZE, INCREMENT>::transform(std::complex<T> *data)
{
	for (unsigned int i = INCREMENT; i < SIZE*INCREMENT; i += 4*INCREMENT)
	{
		int i2 = i + 2*INCREMENT;

		std::complex<T> temp = data[i] - data[i2];
		data[i] 		=  data[i] + data[i2];
		data[i2].real() =  temp.imag();
		data[i2].imag() = -temp.real();
	}
}


template<class T, int SIZE, int INCREMENT>
void Fourier::Internal<T, SIZE, 4, INCREMENT>::transform(std::complex<T> *data)
{
	Internal2<T, SIZE/2, INCREMENT*2>::transform(data);
	Internal4<T, SIZE, INCREMENT>::transform(data);

	Internal<T, SIZE, 2, INCREMENT>::transform(data);
}


template<class T, int SIZE, int INCREMENT>
void Fourier::Internal2<T, SIZE, INCREMENT>::transform(std::complex<T> *data)
{
	for (unsigned int i = 0; i < SIZE*INCREMENT; i += 2*INCREMENT)
	{
		std::complex<T> 	temp = data[i];
		data[i] 		  = data[i] + data[i+INCREMENT];
		data[i+INCREMENT] = temp 	- data[i+INCREMENT];
	}
}


template<class T, int SIZE, int INCREMENT>
void Fourier::Internal<T, SIZE, 2, INCREMENT>::transform(std::complex<T> *data)
{
	Internal2<T, SIZE, INCREMENT>::transform(data);
}

template<class T, int SIZE, int INCREMENT>
void Fourier::Internal<T, SIZE, 1, INCREMENT>::transform(std::complex<T> *data)
{
	return;
}



//--------------------------------------------------------------------------
//
template<class T, int TYPE, int SIZE, int INCREMENT>
void Fourier::fastTransform(std::complex<T> *data)
{
	static const unsigned int end = SIZE*INCREMENT;

	// Conjugate if backward transform
	if (TYPE == REVERSE)
	{
		for (unsigned int i = 0; i < end; i+=INCREMENT )
			data[i].imag() = -data[i].imag();
	}

	// Main loop
	Internal<T, SIZE, SIZE, INCREMENT>::transform(data);

	// Bit reversal permutation
	unsigned int j = 0;
	for (unsigned int i = 0; i < SIZE-1; i++)
	{
		if (i < j)
		{
			unsigned int iIncrement = i*INCREMENT;
			unsigned int jIncrement = j*INCREMENT;

			std::complex<T> temp = data[jIncrement];
			data[jIncrement] = data[iIncrement];
			data[iIncrement] = temp;
		}
		unsigned int k = SIZE>>1;
		while ( k < j+1 )
		{
			j -= k;
			k >>= 1;
		}
		j += k;
	}

	if (TYPE == REVERSE)
	{
		for (unsigned int i = 0; i < end; i+=INCREMENT)
			data[i].imag() = -data[i].imag();
	}

//		const T oneOverSqrtOfSize = 1.0/sqrt(size);
//		for (unsigned int i = 0; i < end; i+=increment)
//			data[i] *= oneOverSqrtOfSize;
}


//--------------------------------------------------------------------------
//
template<class T, int TYPE>
void Fourier::fastTransform(std::complex<T> *data, unsigned int size, int increment)
{
//		if (!isPow2(size))
//		{
//			fprintf(stderr, "Can only transform power of 2 data sizes.  %d is not a power of 2\n", size);
//			return;
//		}

	const unsigned int end = size*increment;

	const T TWO_PI = 2*M_PI;
	unsigned int m = log2(size);

	// Quick return for size=1
	if (size == 1) return;

	// Conjugate if backward transform
	if (TYPE == REVERSE)
	{
		for (unsigned int i = 0; i < end; i+=increment )
			data[i] = conj(data[i]);
	}

	// Main loop
	const T twoPiOverN = TWO_PI / static_cast<T>(size);
	unsigned int size2 = size;
	for (unsigned int k = 1; k <= m; k++)
	{
		unsigned int size1 = size2;
		size2 >>= 1;
		int ie = size / size1;
		int ia = 1;
		for (unsigned int j = 0; j < size2; j++)
		{
			T a = static_cast<T>(ia-1) * twoPiOverN;
			T cosA = cos(a);
			T sinA = sin(a);
			ia += ie;
			for (unsigned int i = j; i < size; i += size1)
			{
				int l = i + size2;
				int iIncrement = i*increment;
				int lIncrement = l*increment;

				std::complex<T> temp = data[iIncrement] - data[lIncrement];
				data[iIncrement] += data[lIncrement];
				data[lIncrement].real() = cosA * temp.real() + sinA * temp.imag();
				data[lIncrement].imag() = cosA * temp.imag() - sinA * temp.real();
			 }
		}
	}

	// Bit reversal permutation
	unsigned int j = 0;
	for (unsigned int i = 0; i < size-1; i++)
	{
		if (i < j)
		{
			unsigned int iIncrement = i*increment;
			unsigned int jIncrement = j*increment;

			std::complex<T> temp = data[jIncrement];
			data[jIncrement] = data[iIncrement];
			data[iIncrement] = temp;
		}
		unsigned int k = size>>1;
		while ( k < j+1 )
		{
			j -= k;
			k >>= 1;
		}
		j += k;
	}

	if (TYPE == REVERSE)
	{
		for (unsigned int i = 0; i < end; i+=increment)
			data[i] = conj(data[i]);
	}

//		const T oneOverSqrtOfSize = 1.0/sqrt(size);
//		for (unsigned int i = 0; i < end; i+=increment)
//			data[i] *= oneOverSqrtOfSize;
}


bool Fourier::isPow2(unsigned int val)
{
	return (val & (val - 1)) == 0;
}

//--------------------------------------------------------------------------
//
int Fourier::log2(unsigned int value)
{
	//	this works only on CPUs with IEEE floating numbers, 32 bits int
	//	and 64 bits double.  Otherwise, use the code commented at the end
	/*static const int isLittleEndian =  (*(int*)"abcd") != 'abcd';

	union {
		unsigned int asInt[2];
		double asDouble;
	} val;

	val.asDouble = static_cast<double>(value) + 0.5;
	return (val.asInt[isLittleEndian] >> 20)-1023;
	*/

	//	if the above fails, use this instead
    int l2 = 0;
    while (value)
        ++l2, value >>= 1;
    return l2;
}


#endif // __MKCORE_MATH_GEOMETRY_FOURIER_INLINE_H__
