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
        mean();
		/** Default destructor*/
        virtual ~mean();
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
        bool getNextXY(int & c, double & x, double & y);

		/** Reset private index + virtual iterator */
        void reset();

    public:

		/**	@return get arithmetic mean	*/
        double getArithmeticMean();
        /** @return Get basic average value */
		double getAverage();

		/**	@return get geometric mean	*/
        double getGeometricMean();

		/**	@return get harmonic mean	*/
        double getHarmonicMean();

		/**	@return get weighted mean
		*	@param	w 	weight values in map (index->weight)
		*/
        double getWeightedMean(map<int, double> & w);

		/**	@return get quadratic mean	*/
        double getQuadraticMean(); //RMS
		/**
		*	@param e	X or Y midrange
		*	@return get midrage mean
		*/
        double getMidrange(midrange e = X);
		/**
		*   sigma = sqrt[ SUM( x - 'x ) ^2 / (n-1) ]
		*   @return Get standard deviation
		*/
        double getStandardDeviation();
		/**
		*	Get average absolute deviation by parameters.
		*	modes: MEAN, MEDIAN, MODE
		*	@param e	mode of deviation
		*	@param mode	special value
		*/
        double getAverageAbsoluteDeviation(averageAbsoluteDeviation e = MEAN, double mode = 0);
		/** @return Get median */
        double getMedian();
		/** @return Get datatable size */
        int getSize();
		/** @return Get accumulative Y */
        double getAccumulativeY();
		/** @return Get Y sum */
        double getYsum();
		/** @return Get X sum */
        double getXsum();

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
        gpMath();
		/** Default destructor */
        virtual ~gpMath();
};




#endif
