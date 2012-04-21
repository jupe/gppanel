/**
 *	@file		gpMath.h
 *	@author     Jussi Vatjus-Anttila
 *	@date		13.10.2009
 *	@version	0.1
 *
 *  Purpose:         Mathematical functions for data
 *  Original Author: Jussi Vatjus-Anttila
 *  Created:         10/09/2009
 *  Last edit:       21/04/2012
 *  Copyright:       (c) Jussi Vatjus-Anttila
 *  Licence:         wxWindows Library License v3.1
 *
 */
#ifndef GPMATH_H
#define GPMATH_H

#include <math.h>
#include <string>
#include <map>
using std::map;

class gpMath;
class mean;

#include "symbols.h"

/**
*	Namespace for mean variables
*/

namespace mean_n
{
    enum midrange
    {
        X,Y
    };
    enum averageAbsoluteDeviation
    {
        MEAN, MEDIAN, MODE
    };

    /*float GetDeviantion(U32 n, double n_sum, double n_square_sum)
    {
        return sqrt(
                        ((double)1.0 / ((double)n-1)) *
                        ( n_square_sum - ((double)1/(double)n)*(n_sum*n_sum ) )
                    );
    }
    */
};	//namespace mean
using namespace mean_n;


/**
*	Statistic mathematically functions for
*	get various means from data.
*	-
*/
class mean
{
    protected:
        int counter;		//!< counter for private using

    public:

		/** Default constructor*/
        mean(){}
		/** Default destructor*/
        virtual ~mean(){}
		/**
		*	Virtual function to get next XY from data.
		*	@note Must be impelmented!
		*	@param 	x	X value
		*	@param	y	Y value
		*	@return	true if found next
		*/
        virtual bool getNextXY(double &x, double &y) = 0;
		/**
		*	Virtual function to reset iterator.
		*	@note Must be implemented!
		*/
        virtual void resetI() =0;

    private:
		/**
		*	Helper function to get next XY with index C
		*	@param c	index of XY pair
		*	@param x	X value
		*	@param y	Y value
		*	@return	true if found next
		*/
        bool getNextXY(int & c, double & x, double & y)
        {
            if(getNextXY(x, y)){
                c = ++counter;
                return true;
            }
            return false;
        }
		/** Reset private index + virtual iterator */
        void reset()
        {
            resetI();
            counter = 0;
        }
    public:

        #define getAverage(x)	getArithmeticMean(x)	//!< Get basic average value

		/**	@return get arithmetic mean	*/
        double getArithmeticMean()
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
		/**	@return get geometric mean	*/
        double getGeometricMean()
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
		/**	@return get harmonic mean	*/
        double getHarmonicMean()
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
		/**	@return get weighted mean
		*	@param	w 	weight values in map (index->weight)
		*/
        double getWeightedMean(map<int, double> & w)
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
		/**	@return get quadratic mean	*/
        double getQuadraticMean() //RMS
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
		/**
		*	@param e	X or Y midrange
		*	@return get midrage mean
		*/
        double getMidrange(midrange e = X)
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
		/**
		*   sigma = sqrt[ SUM( x - 'x ) ^2 / (n-1) ]
		*   @return Get standard deviation
		*/
        double getStandardDeviation()
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
		/**
		*	Get average absolute deviation by parameters.
		*	modes: MEAN, MEDIAN, MODE
		*	@param e	mode of deviation
		*	@param mode	special value
		*/
        double getAverageAbsoluteDeviation(averageAbsoluteDeviation e = MEAN, double mode = 0)
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
		/** @return Get median */
        double getMedian()
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
		/** @return Get datatable size */
        int getSize()
        {
            double x, y;
            int n = 0;
            reset();
            while(getNextXY(n, x, y));
            return n;
        }
		/** @return Get accumulative Y */
        double getAccumulativeY()
        {

            double x, y, ysum=0;
            int n;
            reset();
            while(getNextXY(n, x, y))ysum+=y;
            return ysum;
        }
		/** @return Get Y sum */
        double getYsum()
        {
            double x, y, ysum=0;
            int n;
            reset();
            while(getNextXY(n, x, y))ysum+=y;
            return ysum;
        }
		/** @return Get X sum */
        double getXsum()
        {
            double x, y, xsum=0;
            int n;
            reset();
            while(getNextXY(n, x, y))xsum+=x;
            return xsum;
        }

};	//class mean

/**
*	gpMath class
*	inherited  from mean functions
*	@todo the possibility of further development of new mathematical / statistical indicators mission
*/
class gpMath : public mean
{
    public:
		/** Default constructor */
        gpMath(){}
		/** Default destructor */
        virtual ~gpMath(){}
};




#endif
