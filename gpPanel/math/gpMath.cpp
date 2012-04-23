/**
 *	@file		gpMath.h
 *	@author     Jussi Vatjus-Anttila
 *	@date		23.04.2012
 *	@version	0.1
 *
 *  Purpose:         Mathematical functions for data
 *  Original Author: Jussi Vatjus-Anttila
 *  Created:         23/04/2012
 *  Last edit:       23/04/2012
 *  Copyright:       (c) Jussi Vatjus-Anttila
 *  Licence:         wxWindows Library License v3.1
 *
 */

#include "gpMath.h"

mean::mean()
{
    counter = 0;
}

mean::~mean()
{
}

bool mean::getNextXY(int & c, double & x, double & y)
{
	if(getNextXY(x, y)){
		c = ++counter;
		return true;
	}
	return false;
}

void mean::reset()
{
	resetI();
	counter = 0;
}

double mean::getAverage()
{ return getArithmeticMean(); }

double mean::getArithmeticMean()
{
	double x = 0, y = 0, sum = 0;
	int n = 0;
	reset();
	while(getNextXY(n, x, y))
	{
		sum += y;
	}
	return sum/ (double)n;
}
double mean::getGeometricMean()
{
	double x = 0, y = 0;
	long double p=1;
	int n = 0;
	reset();
	while(getNextXY(n, x, y))
	{
		p *= y;
	}
	return pow(p, (1.0/(double)n) );
}
double mean::getHarmonicMean()
{
	double x = 0, y = 0, d=0;
	int n = 0;
	reset();
	while(getNextXY(n, x, y))
	{
		d += (1.0/y);
	}
	return (double)n / d;
}
double mean::getWeightedMean(map<int, double> & w)
{
	double x, y, d=0, wsize = (double)w.size();
	int n;
	reset();
	while(getNextXY(n, x, y))
	{
		if(wsize >=n)return -1;
		d += y*w[n];
	}
	return d/(double)n;
}
double mean::getQuadraticMean() //RMS
{
	double x = 0, y = 0, s=0;
	int n = 0;
	reset();
	while(getNextXY(n, x, y))
	{
		s += y*y;
	}
	return pow(s/(double)n, 0.5);
}
double mean::getMidrange(midrange e)
{
	double max=0, min=0, x=0, y=0, k=0;
	bool first=true;
	reset();
	while(getNextXY(x, y))
	{
		switch(e){
			default:
			case(X):k=x;break;
			case(Y):k=y;break;
		}
		if(first){
			min = max = k;
			first=false;
		}
		if(k > max) max = k;
		if(k < min) min = k;
	}
	return (max-min)/2.0;
}
double mean::getStandardDeviation()
{
	double m, x, y, t=0;
	int n=0;
	m= getArithmeticMean();
	reset();
	while(getNextXY(n, x, y))
	{
		t += pow((y-m), 2);
	}

	return pow( t / ((double)n-1.0) , 0.5);
}
double mean::getAverageAbsoluteDeviation(averageAbsoluteDeviation e, double mode)
{
	double m,x,y,s=0;
	int n = 0;
	switch(e)
	{
		default:
		case(MEAN):
			m = getArithmeticMean();
			break;
		case(MEDIAN):
			m = getMedian();
			break;
		case(MODE):
			m = mode;
			break;
	}
	reset();
	while(getNextXY(n, x, y))
	{
		s += fabs(y-m);
	}
	return s/(double)n;
}
double mean::getMedian()
{
	//{2,3,4,1,45} = 4
	double x = 0, y = 0;
	int n = 0, c = 0;
	reset();
	while(getNextXY(n, x, y));
	c=n/2+1;
	reset();
	while(getNextXY(n, x, y))if(n==c)break;
	return y;
}
int mean::getSize()
{
	double x, y;
	int n = 0;
	reset();
	while(getNextXY(n, x, y));
	return n;
}
double mean::getAccumulativeY()
{

	double x, y, ysum=0;
	int n;
	reset();
	while(getNextXY(n, x, y))ysum+=y;
	return ysum;
}
double mean::getYsum()
{
	double x, y, ysum=0;
	int n;
	reset();
	while(getNextXY(n, x, y))ysum+=y;
	return ysum;
}
double mean::getXsum()
{
	double x, y, xsum=0;
	int n;
	reset();
	while(getNextXY(n, x, y))xsum+=x;
	return xsum;
}


gpMath::gpMath(){}
gpMath::~gpMath(){}
