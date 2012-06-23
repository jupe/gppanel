/*******************************************************************************
 File:				Fourier.h

 Author: 			Gaspard Petit (gaspardpetit@gmail.com)
 Last Revision:		March 14, 2007

 This code may be reused without my permission as long as credits are given to
 the original author.  If you find bugs, please send me a note...
*******************************************************************************/
#ifndef __MKCORE_MATH_GEOMETRY_FOURIER_H__
#define __MKCORE_MATH_GEOMETRY_FOURIER_H__

//==============================================================================
//	FORWARD DECLARATION
//==============================================================================
namespace std { template<class T> class complex; }
#define SQUARE(real,imag)   sqrt((real)*(real)+(imag)*(imag))

//==============================================================================
//	CLASS Fourier
//==============================================================================
class Fourier
{
public:
	enum
	{
		FORWARD = -1,
		REVERSE = 1
	};

public:
	template<class T>
	static void forwardTransform2D(std::complex<T> *data, int width, int height);

	template<class T>
	static void reverseTransform2D(std::complex<T> *data, int width, int height);

	template<class T, int SIZE>
	static void forwardTransform(std::complex<T> *data);

	template<class T>
	static void forwardTransform(std::complex<T> *data, unsigned int size);

	template<class T, int SIZE>
	static void reverseTransform(std::complex<T> *data);

	template<class T>
	static void reverseTransform(std::complex<T> *data, unsigned int size);

	template<class T, int TYPE, int SIZE, int INCREMENT>
	static void fastTransform(std::complex<T> *data);

	template<class T, int TYPE>
	static void fastTransform(std::complex<T> *data, unsigned int size, int increment);

public:
	template<class T, int SIZE, int LEVEL, int INCREMENT>
	class Internal
	{
	public:
		static void transform(std::complex<T> *data);
	};

	template<class T, int SIZE, int INCREMENT>
	class Internal<T, SIZE, 8, INCREMENT>
	{
	public:
		static void transform(std::complex<T> *data);
	};

	template<class T, int SIZE, int INCREMENT>
	class Internal4
	{
	public:
		static void transform(std::complex<T> *data);
	};

	template<class T, int SIZE, int INCREMENT>
	class Internal<T, SIZE, 4, INCREMENT>
	{
	public:
		static void transform(std::complex<T> *data);
	};

	template<class T, int SIZE, int INCREMENT>
	class Internal2
	{
	public:
		static void transform(std::complex<T> *data);
	};

	template<class T, int SIZE, int INCREMENT>
	class Internal<T, SIZE, 2, INCREMENT>
	{
	public:
		static void transform(std::complex<T> *data);
	};

	template<class T, int SIZE, int INCREMENT>
	class Internal<T, SIZE, 1, INCREMENT>
	{
	public:
		static void transform(std::complex<T> *data);
	};

public:
	static inline bool isPow2(unsigned int val);
	static inline int log2(unsigned int value);

private:
	Fourier();

};


#include "Fourier.inline.h"


#endif // __MKCORE_MATH_GEOMETRY_FOURIER_H__
