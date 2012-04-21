/**
 *	@file		baseData.h
 *	@author     Jussi Vatjus-Anttila
 *	@date		10.09.2009
 *	@version	0.1
 *
 *  Purpose:         This file presented all lowest level data structure classes.
 *  Original Author: Jussi Vatjus-Anttila
 *  Created:         10/09/2009
 *  Last edit:       4/11/2009
 *  Copyright:       (c) Jussi Vatjus-Anttila
 *  Licence:         wxWindows Library License v3.1
 *	URL:			 http://code.google.com/p/gppanel/
 *
 *	@note
 *  Structures:
 *  xy_s (double x, double y)
 *
 *  Classes:
 *  -baseXY        (currently holds only min/max values)
 *  -baseXYData    (inherited from baseXY, contains std::multimap< xy_s >
 *  -lineChartLayer(inherited from baseXYData, mpFXY)
 *  -barChartLayer (inherited from baseXYData, mpFXYBar)
 *
 *  See UML: http://jussiva.dyndns.org/gpPanel/gpPanelUML.png
 *
 */
#ifndef CHARTLAYER_H
#define CHARTLAYER_H

#include "mathplot.h"
#include <deque>
#include <map>
#include <limits>

using std::string;
using std::deque;
using std::multimap;
using std::pair;
using std::vector;


#include "math/gpMath.h"
#include "math/gpFormula.h"
#include "math/Fourier.h"

//This possible to handle a lot of data quicker
//plot just second 1000 point to display..
//#define USE_ITERATOR_INCREMENT

//#define WXSTRING(x) wxString(x.c_str(), wxConvUTF8)

/**
 * Struct contains double x&y variables
 * @{
 */
struct xy_s
{
    public:
        double x;	//!< x value
        double y;	//!< y value

	public:
		/** Default constructor
		 *	@param x_	x value
		 *	@param y_	y value
		 */
        xy_s(double x_, double y_): x(x_), y(y_){}
		/** Copy constructor
		 *	@param xy	class to copy
		 */
        xy_s(const xy_s& xy)    /// copy constructor
        { x = xy.x; y = xy.y; }

};
//! @}

/**
 *  xyDeque contains X&Y values.
 *  Used when not continuously x axis.
 *  Example line chart where is possible
 *  example: {[x,y],..} {[1,2],[3,4],[2,5],[6,3]}
 */
typedef deque< xy_s > xyDeque_t;
/**
 * Multimap make continously x-axis.
 * Many same x values but differents y is allowed.
 */
typedef multimap< double, double >      xyMultimap_t;
/** typedef for xyMultimap iterator*/
typedef xyMultimap_t::iterator          xyMultimapIt_t;

/**
*  key - value pairs:
*  key = xyMultimap_t iterator
*  value = label
*/
class xyMultimapLabel
{
    public:
        xyMultimapIt_t  it;			//!< iterator to xyMultiMap
        wxString        label;		//!< label

	public:
		/** Default constructor
		 *  @param _it	iterator
		 *	@param lbl	label
		 */
        xyMultimapLabel( xyMultimapIt_t _it, wxString lbl )
        { it=_it; label=lbl; }
		/**
		 *	Copy constructor
		 *	@param copy	class to copy
		 */
        xyMultimapLabel( const xyMultimapLabel& copy )
        { it=copy.it; label=copy.label; }
};
/** typedef for xyMultimapLabel*/
typedef std::vector< xyMultimapLabel >    xyMultimapLabel_t;


/**
 * Vector for hold time_t structure
 * Example when want store data with time
 */
typedef vector< time_t > timeList_t;


/**
*	Function that check if value is in numeric limits.
*/
template<typename T>
inline bool isanyinf(T value)
{
    return value >= std::numeric_limits<T>::min() && value <= std::numeric_limits<T>::max();
}

/**
 *  Base class for XY data
 */
class CMinMax
{
	// protected variables
	protected:

		double 	 xMax   ///< X max value
                ,yMax   ///< Y max value
                ,xMin   ///< X min value
                ,yMin;  ///< Y min value

    // public functions
	public:

        double GetMaxX() const { return xMax; }      /// @return max X value
        double GetMinX() const { return xMin; }      /// @return min X value
        double GetMaxY() const { return yMax; }      /// @return max Y value
        double GetMinY() const { return yMin; }      /// @return min Y value
        void SetMaxX( double d )  { xMax=d; }      /// @return max X value
        void SetMinX( double d )  { xMin=d; }      /// @return min X value
        void SetMaxY( double d )  { yMax=d; }      /// @return max Y value
        void SetMinY( double d )  { yMin=d; }      /// @return min Y value
		/** copy constructor */
        explicit CMinMax(const CMinMax& copy)
        {
            xMax = copy.GetMaxX();
            yMax = copy.GetMaxY();
            xMin = copy.GetMinX();
            yMin = copy.GetMinY();

        }

	//Protected functions
    protected:
        /** Constructor */
        CMinMax()
        {
            xMax = xMin = yMax = yMin = 0;
        }
		/** Destructor */
        ~CMinMax(){}

        /**
         *  Handle min and max values
         *  @param[in] x X value to be check if it's min or max
         *  @param[in] y Y value to be check if it's min or max
         */
        void minmax(const double &x, const double &y)
        {

            //check if values are supported. in limit.
            //if(!isanyinf<double>(x))return;
            //if(!isanyinf<double>(y))return;

            if(xMax < x)  xMax = x;
            if(xMin > x)  xMin = x;
            if(yMax < y)  yMax = y;
            if(yMin > y)  yMin = y;

        }
		/**
		*	Handle Y-value for minmax
		*	@param[in] x  x-value for check
		*/
        void minmaxX(const double &x)
        {
            if(xMax < x)  xMax = x;
            if(xMin > x)  xMin = x;
        }
		/**
		*	Handle Y-value for minmax
		*	@param[in] y  y-value for check
		*/
        void minmaxY(const double &y)
        {
            if(yMax < y)  yMax = y;
            if(yMin > y)  yMin = y;
        }



};

/**
 *  baseXYMixedData:
 *  -base mixed XY data class. Can be using straight with gpLayer or
 *  used by
 *  -use xyDeque_t for data storing
 */
class baseXYMixedData : protected CMinMax
{
	protected:
        /**
         *  Constructor
         *  @param name name of mpLayer
         */
		baseXYMixedData()
		{
            m_it   = m_data.begin();
		}
		virtual ~baseXYMixedData(){}    /// destructor
    public:
        /**
         * Define new XY data for graph
         * @param newdata[in]   new data
         */
		void SetData(xyDeque_t& newdata)
		{

			m_data.clear();
			if(newdata.empty())return;
			m_data = newdata;
			xyDeque_t::iterator it;

			//search largest and smallest values of data
			xMax = xMin = m_data[0].x;
			yMax = yMin = m_data[0].y;
			for( it = m_data.begin();
				 it != m_data.end();
				 it ++)
				{
                    minmax(it->x, it->y);
				}
		}
		/**
         *  alternative function to push only one x&y values to graph
         *  @param[in] x X value
         *  @param[in] y Y value
		 */
		void PushData(double &x, double &y)
		{
		    m_data.push_back( xy_s(x,y ) );
		    minmax(x, y);
		}
		/**
		 * get reference data from graph
		 * @return xyDeque_t
		 */
		xyDeque_t& GetData() { return m_data; }

    protected:

        /*
        * Virtual functions for mpLayer usage
        */

        /**
         *  GetNextXY virtual function for mpLayer
         */
		virtual bool GetNextXY( double & x, double & y )
        {
            if(m_it != m_data.end() )
			{
				x = m_it->x;
				y = m_it->y;
				m_it++;
				return true;
			}
			return false;
        }
        virtual void   Rewind() 	{ m_it = m_data.begin(); }
        virtual double GetMinY()	{ return yMin; }
        virtual double GetMaxY()	{ return yMax; }
        virtual double GetMinX()	{ return xMin; }
        virtual double GetMaxX() 	{ return xMax; }

	private:
		xyDeque_t              m_data;    //!< data stucture for graph
        xyDeque_t::iterator    m_it;      //!< data iterator for virtual functions

};
/**
 *  baseXYData:
 *  -base data class. Normal using with chart types class
 *  -use xyMultimap_t for data storing
 *
 *  CMinMax is used for bounds checked and it storing
 *  minimum and maximum values of X and Y.
 *
 *	gpMath possibles to calculate severals
 *  mean-values from data.
 */
class baseXYData : public CMinMax, public gpMath
{
	private:

        bool                    m_useTime;//!< Use time class list. Default is false
        timeList_t              m_time; //!< time container
		xyMultimap_t            m_data;	//!< graph data container
        xyMultimap_t::iterator  m_it;	//!< graph iterator for mpLayer
        xyMultimap_t::iterator  m_it_end;	//!< graph iterator for mpLayer
        xyMultimapLabel_t       m_labels;	//!< labelmap for x-y pairs

        int                     m_checkPoint;

        bool                    m_first;//!< helping when calculate min/max values

        //for mpFXY
        double                  m_currentBoundaryXmin;
        double                  m_currentBoundaryXmax;
        int                     m_iteratorIncrement;
        int                     m_showOnlyLastN;

    public:
		/** Default constructor */
		baseXYData()
		{
            m_it        = m_data.begin();
            m_first     = true;
            m_useTime   = false;
            m_iteratorIncrement = 1;
            m_showOnlyLastN = -1;
            m_checkPoint = 0;
            m_currentBoundaryXmax = -1;
            m_currentBoundaryXmin = -1;
		}
		/** Copy constructor
		 *	@param[in]	baseXYData	copy from class
		 */
		explicit baseXYData(const baseXYData& copy) : CMinMax(copy)
		{
            m_data      = copy.GetData();
            m_it        = m_data.begin();
            m_first     = true;

            m_useTime   = copy.IsTimeHolderEnabled();
            //m_time      = *(timeList_t*)copy.GetTimeHolder();
            m_iteratorIncrement = 1;
            m_showOnlyLastN = copy.m_showOnlyLastN;
            m_checkPoint = copy.m_checkPoint;
		}
		/**
		 *  Default destructor
		 */
		virtual ~baseXYData(){}

        /** Get smallest X value, which Y-value  not zero*/
		double GetMinXwhichNotZero()
		{
		    xyMultimapIt_t it = m_data.begin();
		    while( it != m_data.end() )
            {
                if(it->second > 0) return it->first;
                it++;
            }
            return 0;
		}
		double GetMaxXwitchNotZero()
		{
		    xyMultimap_t::reverse_iterator rit = m_data.rbegin();
		    while(rit != m_data.rend() )
		    {
		        if(rit->second > 0) return rit->first;
		        rit++;
		    }
		}

		//void ShowOnlyLast(int n){ m_showOnlyLastN = n; }

		bool IsEmpty() { return m_data.size()==0?true:false; }
		int  size() const { return m_data.size(); }

		bool IsTimeHolderEnabled() const        { return m_useTime; }
		void UseTimeHolder(bool hold)           { m_useTime = hold; }
		const timeList_t* GetTimeHolder()       { return &m_time; }
		void SetTimeHolder(const timeList_t* timeList){ if(timeList) m_time = *timeList; }

        xyMultimap_t GetData() const { return m_data; }
        xyMultimap_t* GetDataP() { return &m_data; }
        xyMultimap_t GetLastData(int amount)
        {
            if( (int)m_data.size() < amount )
                return m_data;

            xyMultimap_t out;
            xyMultimap_t::iterator it;
            amount = m_data.size() - amount;
            for( it=m_data.begin(); it!= m_data.end(); it++)
            {
                if( (--amount) == 0) break;
            }
            for( ; it!= m_data.end(); it++)
            {
                out.insert( std::pair<double, double>(it->first, it->second) );
            }
            return out;

        }

        /**
         * Set check point. Idea that can push data,
         * so that checkpoint is like offset.
         */
		void CheckPoint()
		{
		    m_checkPoint = m_data.size();
        }
        /**
        *   Fill zeros if missing.
        *  [0] = 2
        *  [2] = 4 -> [1]<=0
        *  [3] = 5
        *  [6] = 4 -> [4]=[5]<=0
        */
        void FillZeros(double min, double max, double intervall=1.0)
        {
            for( double x = min; x < max; x+=intervall )
            {
                if( m_data.find(x) == m_data.end() )
                    m_data.insert( pair<double, double>(x, 0) );
            }
        }
        void InitZeros()
        {
            for(    m_it  = m_data.begin();
                    m_it != m_data.end();
                    m_it++ )
            {
                m_it->second = 0;
            }
        }
        /**
         *  Get swapped X-Y to Y-X of chart data
         *
        */
		/*xyMultimap_t GetSwappedData() const
		{
		    xyMultimap_t out;
		    //xyMultimap_t::iterator it = m_data.begin();
		    //while( it != m_data.end() )
		    //{
		    //    out.insert( pair<double, double>( (double)it->second, (double)it->first) );
		    //    it++;
		    //}*
		    return m_data;
		}*/
		xyMultimap_t GetDataFromCheckPoint()
		{
		    xyMultimap_t out;
		    int i=m_checkPoint;
		    xyMultimap_t::iterator it = m_data.begin();
		    while(i--)it++;
		    while( it != m_data.end() )
		    {
		        out.insert( *it );
		        it++;
		    }
		    return out;
		}

        /**
         *  defines the minimum and maximum values for X and Y
         */
        /*void SetXMinMax(double min, double max)
        {
            CMinMax::SetMinX(min);
            CMinMax::SetMaxX(max);
        }*/
        /**
         *  defines the minimum and maximum values for X and Y
         */
        /*void SetYMinMax(double min, double max)
        {
            CMinMax::SetMinY(min);
            CMinMax::SetMaxY(max);
        }*/
        /**
         *  Lock min max values for chart
         */
        //void LockX(bool b=true){m_LockX = b;}
        //void LockY(bool b=true){m_LockY = b;}

        /**
         * base XY data functions
         */

        /**
         *  Set new data for graph
         *  (param itBegin  iterator where start inserting)
         *  @param newdata  xyMultimap_t data
         */
		void DataSet(xyMultimap_t newdata)
		{
			if(newdata.size() == 0)return;
            m_labels.clear();
			xyMultimap_t::iterator it;
            m_data = newdata;

            //search largest and smallest values of data to minmax
            xMin = xMax = m_data.begin()->first;
            yMin = yMax = m_data.begin()->second;

			m_first=false;

			//wxLogMessage(wxString::Format( _("Min and max %.0f %.0f"), xMin, yMin ) );
			for( it=m_data.begin(); it != m_data.end(); it ++)
                { minmax(it->first, it->second); }
		}
		/**
         *  Set new data from pointer for graph
         *  (param itBegin  iterator where start inserting)
         *  @param *newdata  xyMultimap_t data
         */
		void DataSetFromPointer(xyMultimap_t *newdata)
		{
			if(newdata==0 || newdata->size() == 0)return;
            m_labels.clear();
			xyMultimap_t::iterator it;
            m_data = *newdata;

            //search largest and smallest values of data to minmax
            xMin = xMax = m_data.begin()->first;
            yMin = yMax = m_data.begin()->second;

			m_first=false;

			//wxLogMessage(wxString::Format( _("Min and max %.0f %.0f"), xMin, yMin ) );
			for( it=m_data.begin(); it != m_data.end(); it ++)
                { minmax(it->first, it->second); }
		}
		/**
         *  alternative function to push xyMultimap_t
         *  @param data	Data to be push
		 */
		 void DataPush(xyMultimap_t data)
		 {
		    if(data.size() == 0)return;

			xyMultimap_t::iterator it=m_data.end(); it--;
            m_data.insert(data.begin(), data.end() );


			//wxLogMessage(wxString::Format( _("Min and max %.0f %.0f"), xMin, yMin ) );
			for( ;it != m_data.end(); it ++)
				{ minmax(it->first, it->second); }
		 }
		/**
		 *	Data push with label
		 *	@param[in] x	X value
		 *	@param[in] y	y value
		 *	@param[in] label	label for 2d-point
		 */
		void DataPush(double &x, double &y, wxString &label)
		{
		    xyMultimap_t::iterator it = DataPush(x,y);
		    //m_labels.insert( std::pair< xyMultimapIt_t, wxString >( it, label ) );
		    m_labels.push_back( xyMultimapLabel( it, label ) );
		}
		/**
         *  alternative function to push only one x&y values to graph
         *  @param x X value
         *  @param y Y value
		 *	@return iterator to multimap
		 */
        /*void DataPush(double &x, double &y )
        {
            (void)(xyMultimap_t::iterator)DataPush(double &x, double &y);
        }*/
        xyMultimap_t::iterator DataPush(double &x, double &y)
		{
		    xyMultimap_t::iterator it;
		    it = m_data.insert( pair<double, double>( x,y ) );
		    if( m_first ){
		        m_first=false;
		        //if was first values to push, init min and max values
		        xMin=xMax=x;
                yMin=yMax=y;
                //wxLogMessage(wxString::Format( _("Min and max %.0f %.0f"), x, y) );
		    }
		    else
		    {
		        minmax(x, y);
		    }
		    if(m_useTime){ m_time.push_back( time(0) ); }
		    return it;
		}
		xyMultimap_t::iterator DataPushUniq(double &x, double &y)
		{
		    if( m_data.find(x) != m_data.end() )
                m_data.erase( m_data.find(x) );
		    return DataPush(x, y);
		}
		/** Clear whole data */
		void DataClear()
		{
		    m_data.clear();
		    m_checkPoint = 0;
		    m_first = true;
		}
		/**
		 * get reference data map
		 * @return xyMultimap_t	reference
		 */

		//xyMultimap_t& GetData() { return m_data;}
        /** Get Data without zeros */
		xyMultimap_t GetDataWithoutZeros()
		{
		    xyMultimap_t out;
		    xyMultimap_t::iterator it;
		    it = m_data.begin();
		    while( it != m_data.end() )
		    {
		        if( it->second != 0) out.insert( *it );
		        it++;
		    }
		    return out;
		}


        /**
         *
		 *	@param xmin	min value to bounds
		 *	@param xmax max value to bounds
         */
        void CurrentBounds(double& xmin, double& xmax)
        {
            m_currentBoundaryXmin = xmin;
            m_currentBoundaryXmax = xmax;

            //lower boundary iterator
            while( m_it != m_data.end() && m_currentBoundaryXmin >= m_it->first)m_it++;
            if( m_it == m_data.end() ) return;
            if(m_it != m_data.begin()) m_it--;

            //upper boundary iterator
            m_it_end = m_it;
            while( m_it_end != m_data.end() && m_currentBoundaryXmax >= m_it_end->first)m_it_end++;
            if(m_it_end != m_data.end())m_it_end++;

            #ifdef USE_ITERATOR_INCREMENT
            //calculate iterator increment
            int size=0;
            xyMultimap_t::iterator it=m_it;
            while(it!=m_it_end){it++;size++;}
            m_iteratorIncrement = 1;
            if(size > 2000)
            {
                if(size < 10000) m_iteratorIncrement = 10;
                else if(size < 100000) m_iteratorIncrement = 100;
                else if(size < 1000000) m_iteratorIncrement = 1000;
                else
                    m_iteratorIncrement = 10000;
            }
            #endif
        }

		/* Virtual functions for gpMath */

		/**
		 *	Virtual function (gpMath)
		 *  Rewind data iterator
		 */
        virtual void   Rewind()
        {
            m_it = m_data.begin();
            m_it_end = m_data.end();
            #ifdef USE_ITERATOR_INCREMENT
            m_iteratorIncrement = 1;
            #endif

            /*if(m_showOnlyLastN>0 && m_data.size()>m_showOnlyLastN)
            {
                int tmp = m_data.size()-m_showOnlyLastN;
                while(tmp--)m_it++;
            }*/
        }
        virtual void Forward()
        {
            m_it = m_data.end();
        }

		/**
		 *	Virtual function (gpMath)
		 *	Get next X Y pair with label
		 */
        virtual bool GetNextXY( double & x, double & y, wxString & label )
        {
            label = _("");
            if(m_it != m_it_end )
            {
                xyMultimapLabel_t::iterator it;
                for( it = m_labels.begin(); it != m_labels.end(); it++)
                {
                    if( it->it == m_it )
                    { label = it->label; break; }
                }
                return GetNextXY(x,y);
            }
            return false;

        }
		/**
		 *	Virtual function (gpMath)
		 *	Get next X Y pair
		 */
		virtual bool GetNextXY( double & x, double & y )
        {
            if(m_it != m_it_end )
			{
				x = m_it->first;
				y = m_it->second;
				#ifdef USE_ITERATOR_INCREMENT
				int i = m_iteratorIncrement;
				while(i-- && m_it!=m_it_end )m_it++;
				#else
				m_it++;
				#endif
                return true;
			}
			return false;
        }
        /**
         * Virtual function: get Previous
         */
        virtual bool GetPrevXY(double & x, double & y )
        {
            if( m_it == m_data.begin() )
            {
                x = m_it->first;
                y = m_it->second;
                m_it--;
                return true;
            }
            return false;
        }

		/**
		*	multiply all Y values and increment offset
		*	@param mul		multiply
		*	@param offset	add value
		*/
        void MultiplyAllYvalues(double mul, double offset=0)
		{
            xyMultimap_t::iterator it;
            m_first = true;

            for(it=m_data.begin(); it!= m_data.end(); it++)
            {
                it->second *= mul;
                it->second += offset;
                if( m_first ){
                    m_first=false;
                    //if was first values to push, init min and max values
                    yMin=yMax=it->second;
                }
                minmaxY(it->second);
            }
		}
		/**
		*	multiply all X values and increment offset
		*	@param mul		multiply
		*	@param offset	add value
		*/
		void MultiplyAllXvalues(double mul, double offset=0)
		{
            xyMultimap_t out;
            xyMultimap_t::iterator it;
            m_first = true;
            double x;
            for(it=m_data.begin(); it!=m_data.end();it++)
            {
                x = it->first * mul + offset;
                out.insert(pair<double, double>(x, it->second));
                if( m_first ){
                    m_first=false;
                    //if was first values to push, init min and max values
                    xMin=xMax=x;
                }
                minmaxX(x);

            }
            m_data = out;
		}
		/* Below functions for:
		   Get various type of xyMultimap_t (example for graph)
		   -GetCustom()				for custom formula
		   -GetINL()				for integral non linearity
		   -GetDNL()				for differential non linearity
		   -GetFft()				for Fast fourier transfer
		   -GetAccumulation()		for the number of times each value occur in Y
		*/
		/**
		 * Convert X axis to time (use time vectors)
		 */
		void ConvertToXtime()
		{
		    if(!m_useTime || m_time.size() != m_data.size() )return;
		    xyMultimap_t map;
		    xyMultimap_t::iterator it;
            timeList_t::iterator it2=m_time.begin();
		    it = m_data.begin();
		    while(it != m_data.end() )
		    {
                map.insert( pair<double, double>( (double)*it2, it->second) );
                it++;
                it2++;

		    }
		    m_data = map;
		}
		/**
		*	Get custom formula calculated data
		*	variable can be used: "X" = x value, "Y" = y value
		*	@param xformula	formula for X values
		*	@param yformula	formula for Y values
		*	@return xyMultimap_t
		*/
        xyMultimap_t GetCustom(wxString Xformula, wxString Yformula)
        {
            xyMultimap_t map;
            double x,y;
            xyMultimap_t::iterator it;
            gpFormula xform( std::string( Xformula.mb_str()) );
            gpFormula yform( std::string( Yformula.mb_str()) );

            for(it=m_data.begin(); it!= m_data.end(); it++)
            {
                if(Xformula.IsEmpty())x=it->first;
                else{
                    xform.AddVariable("X", it->first);
                    xform.AddVariable("Y", it->second);
                    x = xform;
                }
                if(Yformula.IsEmpty())y=it->second;
                else{
                    yform.AddVariable("X", it->first);
                    yform.AddVariable("Y", it->second);
                    y = yform;
                }
				map.insert( pair<double, double>( x, y ) );
            }
            return map;
        }
		/**
		*	Calculate fft from data
		*	@param len			If len == -1 fft length is as long than can be
		*	@param samplerate 	how many samples are in seconds
		*	@return xyMultimap_t
		*/
        xyMultimap_t GetFft(int len=-1, double samplerate = 1.0)
        {
            xyMultimap_t map;
            xyMultimap_t::iterator it;
            int SIZE = len,i=0;
            double max,y, x;
            double *signal;

            if( (int)m_data.size() < len || (int)m_data.size() < 16 )
                return map;
            if( len > 0 && Fourier::isPow2(SIZE)==0 )
                return map;

            map=m_data;
            if(len==-1)
            {
                //find largest length whic are pow2
                while( Fourier::isPow2( map.size() ) == false )
                    map.erase( map.begin() ); //remove first element

                SIZE = (int)map.size();
            }
            else
            {
                while((int)map.size()!=len)
                    map.erase( map.begin() ); //remove first element
                SIZE = (int)map.size();
            }


            signal = (double*)malloc(SIZE*sizeof(double));


            //Get max y value
            max = map.begin()->second;
            for(it=map.begin(); it !=map.end(); it++ )
                if(max<it->second)max=it->second;


            for(it=map.begin(); it !=map.end(); it++ )
                signal[i++] = it->second/max;

            // convert these doubles to complex numbers
            std::complex<double> four[SIZE];
            for (i = 0; i < SIZE; ++i) four[i] = signal[i];

            // forward fourier transform

            Fourier::forwardTransform<double>(four, SIZE);

            map.clear();

            x = samplerate/(double)SIZE;
            for (int i = 0; i < SIZE; ++i)
            {
                y = SQUARE(four[i].real(), four[i].imag());
                map.insert( pair<double, double>( x*(double)i, y ) );
                it++;
            }
            free(signal);
            return map;
        }
        /**
         *  Deviation of code midpoint from ideal location
		 *  -if arithMean is selected function calculate arithmetic
		 *	 mean first and select that to the Expected value for each Y points.
		 *	 (Excpected than Y axis is Equal distribution)
		 *	-If En is selected User is defined own excpected value.
		 *	 (Excpected than Y axis is Equal distribution)
		 *	-If Evect is selected, std::vector<double> must be same size than data in class.
		 *	 Then every Y points can be select differents excpected values.
		 *	 (NOT Excpected than Y axis is Equal distribution)
         *  x <= x
         *  y <= y - arithMean					//Y axis Equal distribution
		 *
         *  y <= y / ( ysum / (maxX+1) ) - 1    //used when all parameters is undefined or En is selected
		 *										//Expected than X axis is continously 0...n
		 *										//and Y axis Equal distribution
         *
         * strange formula:
         * DNLw = (SUM(w) - a) / a  (a= y/SUM(w))
         * http://www.metrology.pg.gda.pl/full/2006/M&MS_2006_161.pdf
         *
		 *	@param arithMean use arithmetic mean
		 *	@param En Expected Y value
		 *	@param Evect  ecpected vector pointer. If null not in use.
		 *
         */
        xyMultimap_t GetDNL(bool arithMean=false, double *En=0, std::vector<double> *Evect=0)
        {
            xyMultimap_t map;
			int i=0;
            double  E=0 //expected value
                    ,ysum = getYsum()
                    ,xmax = GetMaxX()
                    ,x=0,y=0;

			if(arithMean) E = getArithmeticMean();
			else if(En) E=*En;
			else if (Evect==0 ) E = ( ysum / (xmax+1) );            //default
			else if (Evect->size() != m_data.size() ) return map;   //must be same size

            Rewind();
            while( GetNextXY(x, y) )
            {
				if(!arithMean && Evect==0) y = y / E - 1;
				else if(arithMean) y -= E;
				else if(Evect) y -= (*Evect)[i++];

				if(Evect){
                    y /= (*Evect)[i-1];
				} else{
                    y /= E;
                }

                map.insert(pair<double, double>(x,y));

            }
            return map;
        }
        /**
         *  Integrate nonlinearity
         *  Like DNL, but now increment y value:
		 *  sum=0;
         *  {sum <= sum + y
         *   y <= sum }
		 *	@param arithMean	use arithmetic mean
		 *	@param En	Expected value (pointer)
		 *	@param Evect	Expected values in vector (pointer)
		 *	@return xyMultimap_t
         */
        xyMultimap_t GetINL(bool arithMean=false, double *En=0, std::vector<double> *Evect=0)
        {
            double sum=0;
            xyMultimap_t map = GetDNL(arithMean, En, Evect);
			xyMultimap_t::iterator it;
            for(it=map.begin();it!=map.end();it++)
			{
				sum += it->second;
				it->second = sum;
			}
            return map;
        }
        xyMultimap_t GetINL(xyMultimap_t in)
        {
            double sum=0;
			xyMultimap_t::iterator it;
            for(it=in.begin();it!=in.end();it++)
			{
				sum += it->second;
				it->second = sum;
			}
            return in;
        }

		/**
		*	calculates the number of times each value occur in Y
		*	@param inProcents
		*	@return xyMultimap_t
		*/
		xyMultimap_t GetAccumulation(bool inProcents = false)
        {
            double x,y,n=0;
            xyMultimap_t map;
			std::map<double, int> tmp;
			std::map<double, int>::iterator it;

			// loop all values
			// increment tmp[y] by one when occur
			Rewind();
            while( GetNextXY(x, y) )
            {
                if(tmp.find(y)==tmp.end())tmp[y]=1;
				else tmp[y]++;
				n++;
            }
			// insert values to result map
			// map[Y] = number of value occurs
			for(it=tmp.begin(); it != tmp.end(); ++it)
			{
			    y = it->second;
			    if(inProcents) y=(y/n)*100.0;
				map.insert(pair<double, double>(it->first, y ) );
			}
            return map;
        }

        xyMultimap_t GetDataWithoutEndsZeros()
        {
            xyMultimap_t map;
            xyMultimapIt_t it,itbeg, itend;

            map = m_data;
            it = map.begin();
            itend = itbeg = map.end();

            while( it != map.end() )
            {
                if( itbeg == map.end())
                {
                    if( it->second != 0)
                    {
                        itbeg = it; //first
                        itend = it; //last
                    }
                }
                else
                {
                    if(it->second != 0)
                        itend = it; //last
                }
                it++;
            }

            if(itbeg != map.end())
                map.erase(map.begin(), itbeg); //first zeros
            if( itend != map.end() )
                map.erase(itend, map.end()); //last zeros

            return map;
        }

	private:


        //for gpMath
        virtual bool getNextXY(double& x, double& y){return GetNextXY(x,y);}
        virtual void resetI(){return Rewind();}

};

/**
 * bar Chart Layer
 * -use baseXYLayer (x scale is continous by default)
 */
class barChartLayer : public mpFXYBar, public baseXYData
{
    private:
		double				m_lsb;

    public:
		/** Constructor
		*	@param label
		*/
        barChartLayer(wxString label) : mpFXYBar( label ), baseXYData( )
        {
            m_drawOutsideMargins 	= false;
			m_lsb   				= 1;
        }

        virtual ~barChartLayer(){}
		/**
		*	Set LSB of bar chart
		*	@param lsb	LSB
		*/
		void SetLSB(double lsb)			{ m_lsb = lsb; }

        /**
         * Virtual functions for mpFXYBar
         */
        virtual bool GetNextXY( double & x, double & y, wxString & label )
                                        { return baseXYData::GetNextXY(x,y, label); }
        virtual bool GetLSB(double &l)	{ l = m_lsb; return true;}
        virtual void Rewind() 			{ baseXYData::Rewind(); }
        virtual double GetMinY()		{ return baseXYData::GetMinY(); }
        virtual double GetMaxY()		{ return baseXYData::GetMaxY(); }
        virtual double GetMinX()		{ return baseXYData::GetMinX() - m_lsb/2; }
        virtual double GetMaxX() 		{ return baseXYData::GetMaxX() + m_lsb/2; }
    protected:
};

/**
 * bar Chart Layer
 * -use baseXYLayer (x scale is continous by default)
 */
class YbarChartLayer : public mpFYXBar, public baseXYData
{
    private:
		double				m_lsb;

    public:
		/** Constructor
		*	@param label
		*/
        YbarChartLayer(wxString label) : mpFYXBar( label ), baseXYData( )
        {
            m_drawOutsideMargins 	= false;
			m_lsb   				= 1;
        }

        virtual ~YbarChartLayer(){}
		/**
		*	Set LSB of bar chart
		*	@param lsb	LSB
		*/
		void SetLSB(double lsb)			{ m_lsb = lsb; }

        /**
         * Virtual functions for mpFYXBar
         */
        virtual bool GetNextXY( double & x, double & y, wxString& label)
                                        { return baseXYData::GetNextXY(x,y, label); }
        virtual bool GetLSB(double &l)	{ l = m_lsb; return true;}
        virtual void Rewind() 			{ baseXYData::Rewind(); }
        virtual double GetMinY()		{ return baseXYData::GetMinY() - m_lsb/2; }
        virtual double GetMaxY()		{ return baseXYData::GetMaxY() + m_lsb/2; }
        virtual double GetMinX()		{ return baseXYData::GetMinX(); }
        virtual double GetMaxX() 		{ return baseXYData::GetMaxX(); }
    protected:
};

/**
 *  Mixed Line chart
 *  -use baseXYMixedLayer
 */
class MixedLineChartLayer : public mpFXY, public baseXYMixedData
{
    private:

    public:
        MixedLineChartLayer(wxString label) : mpFXY( label ), baseXYMixedData( )
        {
            m_drawOutsideMargins 	= false;
        }

    protected:

        /**
         * Virtual functions for mpFXYBar
         */
        virtual bool GetNextXY( double & x, double & y )
                                        { return baseXYMixedData::GetNextXY(x,y); }
        virtual void Rewind() 			{ baseXYMixedData::Rewind(); }

};
/**
 *  Mixed Line chart
 *  -use baseXYMixedLayer
 */
class lineChartLayer : public mpFXY,  public baseXYData
{
    private:

    public:
        lineChartLayer(wxString label) : mpFXY( label ), baseXYData( )
        {
            m_drawOutsideMargins 	= false;
        }
        baseXYData& operator=(const baseXYData& datacopy)
        {
            *this = datacopy;
            return *this;
        }

        /**
         * Virtual functions for mpFXY
         */
        virtual bool GetNextXY( double & x, double & y )
                                        { return baseXYData::GetNextXY(x,y); }
        virtual void Rewind() 			{ baseXYData::Rewind(); }
        virtual double GetMinY()		{ return baseXYData::GetMinY(); }
        virtual double GetMaxY()		{ return baseXYData::GetMaxY(); }
        virtual double GetMinX()		{ return baseXYData::GetMinX(); }
        virtual double GetMaxX() 		{ return baseXYData::GetMaxX(); }
        virtual void CurrentBounds(double xmin, double xmax){ baseXYData::CurrentBounds(xmin, xmax);}

    protected:
};

#endif
