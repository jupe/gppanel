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

 #include "baseData.h"

xy_s::xy_s(double x_, double y_): x(x_), y(y_)
{}
xy_s::xy_s(const xy_s& xy)
{ x = xy.x; y = xy.y; }


xyMultimapLabel::xyMultimapLabel( xyMultimapIt_t _it, wxString lbl )
{ it=_it; label=lbl; }

xyMultimapLabel::xyMultimapLabel( const xyMultimapLabel& copy )
{ it=copy.it; label=copy.label; }

/* CminMax */
CMinMax::CMinMax(const CMinMax& copy)
{
	xMax = copy.GetMaxX();
	yMax = copy.GetMaxY();
	xMin = copy.GetMinX();
	yMin = copy.GetMinY();

}
double CMinMax::GetMaxX() const { return xMax; }
double CMinMax::GetMinX() const 			{ return xMin; }
double CMinMax::GetMaxY() const 			{ return yMax; }
double CMinMax::GetMinY() const 			{ return yMin; }
void   CMinMax::SetMaxX( double d )  		{ xMax=d; }
void   CMinMax::SetMinX( double d )  		{ xMin=d; }
void   CMinMax::SetMaxY( double d )  		{ yMax=d; }
void   CMinMax::SetMinY( double d )  		{ yMin=d; }



/* Protected */
CMinMax::CMinMax()
{
	xMax = xMin = yMax = yMin = 0;
}
/** Destructor */
CMinMax::~CMinMax(){}

/**
 *  Handle min and max values
 *  @param[in] x X value to be check if it's min or max
 *  @param[in] y Y value to be check if it's min or max
 */
void CMinMax::minmax(const double &x, const double &y)
{
	//check if values are supported. in limit.
	//if(!isanyinf<double>(x))return;
	//if(!isanyinf<double>(y))return;

	if(xMax < x)  xMax = x;
	if(xMin > x)  xMin = x;
	if(yMax < y)  yMax = y;
	if(yMin > y)  yMin = y;
}
void CMinMax::minmaxX(const double &x)
{
	if(xMax < x)  xMax = x;
	if(xMin > x)  xMin = x;
}
void CMinMax::minmaxY(const double &y)
{
	if(yMax < y)  yMax = y;
	if(yMin > y)  yMin = y;
}


/* ******************************************** */
/*  		MIXED DATA CONTAINER  				*/
/* ******************************************** */
baseXYMixedData::baseXYMixedData()
{
	m_it   = m_data.begin();
}
baseXYMixedData::~baseXYMixedData(){}

/* public */
void baseXYMixedData::SetData(xyDeque_t& newdata)
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

void baseXYMixedData::PushData(double &x, double &y)
{
	m_data.push_back( xy_s(x,y ) );
	minmax(x, y);
}
xyDeque_t& baseXYMixedData::GetData() { return m_data; }

/* protected */
bool baseXYMixedData::GetNextXY( double & x, double & y )
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
void   baseXYMixedData::Rewind() 	{ m_it = m_data.begin(); }
double baseXYMixedData::GetMinY()	{ return yMin; }
double baseXYMixedData::GetMaxY()	{ return yMax; }
double baseXYMixedData::GetMinX()	{ return xMin; }
double baseXYMixedData::GetMaxX() 	{ return xMax; }



/* ********************************************* */
/*  		XY DATA CONTAINER  					 */
/* ********************************************* */

baseXYData::baseXYData()
{
	m_it        = m_data.begin();
	m_it_end    = m_data.end();
	m_first     = true;
	m_useTime   = false;
	m_iteratorIncrement = 1;
	m_showOnlyLastN = -1;
	m_checkPoint = 0;
	m_currentBoundaryXmax = -1;
	m_currentBoundaryXmin = -1;
}
baseXYData::baseXYData(const baseXYData& copy)
 : CMinMax(copy)
{
	m_data      = copy.GetData();
	m_it        = m_data.begin();
	m_it_end    = m_data.end();
	m_first     = true;

	m_useTime   = copy.IsTimeHolderEnabled();
	//m_time      = *(timeList_t*)copy.GetTimeHolder();
	m_iteratorIncrement = 1;
	m_showOnlyLastN = copy.m_showOnlyLastN;
	m_checkPoint = copy.m_checkPoint;

	m_currentBoundaryXmax = -1;
	m_currentBoundaryXmin = -1;
}
baseXYData::~baseXYData(){}

/** Get smallest X value, which Y-value  not zero*/
double baseXYData::GetMinXwhichNotZero()
{
	xyMultimapIt_t it = m_data.begin();
	while( it != m_data.end() )
	{
		if(it->second > 0) return it->first;
		it++;
	}
	return 0;
}
double baseXYData::GetMaxXwitchNotZero()
{
	xyMultimap_t::reverse_iterator rit = m_data.rbegin();
	while(rit != m_data.rend() )
	{
		if(rit->second > 0) return rit->first;
		rit++;
	}
	if( rit != m_data.rend() )
        return rit->first;
    return -1;
}

//void ShowOnlyLast(int n){ m_showOnlyLastN = n; }

bool baseXYData::IsEmpty() 									{ return m_data.size()==0?true:false; }
int  baseXYData::size() const								{ return m_data.size(); }
bool baseXYData::IsTimeHolderEnabled() const        		{ return m_useTime; }
void baseXYData::UseTimeHolder(bool hold)           		{ m_useTime = hold; }
const timeList_t* baseXYData::GetTimeHolder()       		{ return &m_time; }
void baseXYData::SetTimeHolder(const timeList_t* timeList)	{ if(timeList) m_time = *timeList; }
xyMultimap_t baseXYData::GetData() const 					{ return m_data; }
xyMultimap_t* baseXYData::GetDataP() 						{ return &m_data; }
xyMultimap_t baseXYData::GetLastData(int amount)
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
void baseXYData::CheckPoint()
{
	m_checkPoint = m_data.size();
}

void baseXYData::FillZeros(double min, double max, double intervall)
{
	for( double x = min; x < max; x+=intervall )
	{
		if( m_data.find(x) == m_data.end() )
			m_data.insert( pair<double, double>(x, 0) );
	}
}
void baseXYData::InitZeros()
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
xyMultimap_t baseXYData::GetDataFromCheckPoint()
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
/*void baseXYData::SetXMinMax(double min, double max)
{
	CMinMax::SetMinX(min);
	CMinMax::SetMaxX(max);
}*/
/*void baseXYData::SetYMinMax(double min, double max)
{
	CMinMax::SetMinY(min);
	CMinMax::SetMaxY(max);
}*/
//void baseXYData::LockX(bool b=true){m_LockX = b;}
//void baseXYData::LockY(bool b=true){m_LockY = b;}

/*
 * base XY data functions
 */
void baseXYData::DataSet(xyMultimap_t newdata)
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
void baseXYData::DataSetFromPointer(xyMultimap_t *newdata)
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
 void baseXYData::DataPush(xyMultimap_t data)
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
void baseXYData::DataPush(double &x, double &y, wxString &label)
{
	xyMultimap_t::iterator it = DataPush(x,y);
	//m_labels.insert( std::pair< xyMultimapIt_t, wxString >( it, label ) );
	m_labels.push_back( xyMultimapLabel( it, label ) );
}
/*void baseXYData::DataPush(double &x, double &y )
{
	(void)(xyMultimap_t::iterator)DataPush(double &x, double &y);
}*/
xyMultimap_t::iterator baseXYData::DataPush(double &x, double &y)
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
xyMultimap_t::iterator baseXYData::DataPushUniq(double &x, double &y)
{
	if( m_data.find(x) != m_data.end() )
		m_data.erase( m_data.find(x) );
	return DataPush(x, y);
}
void baseXYData::DataClear()
{
	m_data.clear();
	m_checkPoint = 0;
	m_first = true;
}
//xyMultimap_t& baseXYData::GetData() { return m_data;}
/** Get Data without zeros */
xyMultimap_t baseXYData::GetDataWithoutZeros()
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
void baseXYData::CurrentBounds(double& xmin, double& xmax)
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
void baseXYData::Rewind()
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
void baseXYData::Forward()
{
	m_it = m_data.end();
}
bool baseXYData::GetNextXY( double & x, double & y, wxString & label )
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
bool baseXYData::GetNextXY( double & x, double & y )
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
bool baseXYData::GetPrevXY(double & x, double & y )
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
void baseXYData::MultiplyAllYvalues(double mul, double offset)
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
void baseXYData::MultiplyAllXvalues(double mul, double offset)
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
void baseXYData::ConvertToXtime()
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
xyMultimap_t baseXYData::GetCustom(wxString Xformula, wxString Yformula)
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
xyMultimap_t baseXYData::GetFft(int len, double samplerate)
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
	std::complex<double>* four = new std::complex<double>[SIZE];
	for (i = 0; i < SIZE; ++i) four[i] = signal[i];

	// forward fourier transform

	Fourier::forwardTransform<double>(four, SIZE);

	map.clear();

	x = samplerate/(double)SIZE;
	for (int i = 0; i < SIZE; ++i)
	{
		y = SQUARE(four[i].real(), four[i].imag());
		map.insert( pair<double, double>( x*(double)i, y ) );
	}
	free(signal);
    delete[] four;
	return map;
}
xyMultimap_t baseXYData::GetDNL(bool arithMean, double *En, std::vector<double> *Evect)
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
xyMultimap_t baseXYData::GetINL(bool arithMean, double *En, std::vector<double> *Evect)
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
xyMultimap_t baseXYData::GetINL(xyMultimap_t in)
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

xyMultimap_t baseXYData::GetAccumulation(bool inProcents )
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

xyMultimap_t baseXYData::GetDataWithoutEndsZeros()
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

/* private */
//for gpMath
bool baseXYData::getNextXY(double& x, double& y){return GetNextXY(x,y);}
void baseXYData::resetI(){return Rewind();}


/* ********************************************* */
/*  		XBAR CHART LAYER  					 */
/* ********************************************* */
barChartLayer::barChartLayer(wxString label) : mpFXYBar( label ), baseXYData( )
{
	m_drawOutsideMargins 	= false;
	m_lsb   				= 1;
}

barChartLayer::~barChartLayer(){}
void barChartLayer::SetLSB(double lsb)			{ m_lsb = lsb; }

/* Virtual functions for mpFXYBar*/
bool barChartLayer::GetNextXY( double & x, double & y, wxString & label )
{ return baseXYData::GetNextXY(x,y, label); }
bool barChartLayer::GetLSB(double &l)	{ l = m_lsb; return true;}
void barChartLayer::Rewind() 			{ baseXYData::Rewind(); }
double barChartLayer::GetMinY()		{ return baseXYData::GetMinY(); }
double barChartLayer::GetMaxY()		{ return baseXYData::GetMaxY(); }
double barChartLayer::GetMinX()		{ return baseXYData::GetMinX() - m_lsb/2; }
double barChartLayer::GetMaxX() 		{ return baseXYData::GetMaxX() + m_lsb/2; }


/* ********************************************* */
/*  		YBAR CHART LAYER  					 */
/* ********************************************* */
YbarChartLayer::YbarChartLayer(wxString label) : mpFYXBar( label ), baseXYData( )
{
	m_drawOutsideMargins 	= false;
	m_lsb   				= 1;
}

YbarChartLayer::~YbarChartLayer(){}
/**
*	Set LSB of bar chart
*	@param lsb	LSB
*/
void YbarChartLayer::SetLSB(double lsb)			{ m_lsb = lsb; }

/* virtual functions for mpFYXBar */
bool YbarChartLayer::GetNextXY( double & x, double & y, wxString& label)
						{ return baseXYData::GetNextXY(x,y, label); }
bool YbarChartLayer::GetLSB(double &l)	{ l = m_lsb; return true;}
void YbarChartLayer::Rewind() 			{ baseXYData::Rewind(); }
double YbarChartLayer::GetMinY()		{ return baseXYData::GetMinY() - m_lsb/2; }
double YbarChartLayer::GetMaxY()		{ return baseXYData::GetMaxY() + m_lsb/2; }
double YbarChartLayer::GetMinX()		{ return baseXYData::GetMinX(); }
double YbarChartLayer::GetMaxX() 		{ return baseXYData::GetMaxX(); }


 /* ******************************************** */
/*  		MIXED LINE CHART LAYER  			 */
/* ********************************************* */
MixedLineChartLayer::MixedLineChartLayer(wxString label) : mpFXY( label ), baseXYMixedData( )
{
	m_drawOutsideMargins 	= false;
}

/* protected */

/* Virtual functions for mpFXYBar */

bool MixedLineChartLayer::GetNextXY( double & x, double & y )
{ return baseXYMixedData::GetNextXY(x,y); }

void MixedLineChartLayer::Rewind()
{ baseXYMixedData::Rewind(); }


 /* ******************************************** */
/*  		DEFAULT LINE CHART LAYER  			 */
/* ********************************************* */
lineChartLayer::lineChartLayer(wxString label) : mpXYArea( label ), baseXYData( )
{
	m_drawOutsideMargins 	= false;
}

/* Virtual functions for mpFXY */
bool lineChartLayer::GetNextXY( double & x, double & y )
										{ return baseXYData::GetNextXY(x,y); }
void lineChartLayer::Rewind() 			{ baseXYData::Rewind(); }
double lineChartLayer::GetMinY()		{ return baseXYData::GetMinY(); }
double lineChartLayer::GetMaxY()		{ return baseXYData::GetMaxY(); }
double lineChartLayer::GetMinX()		{ return baseXYData::GetMinX(); }
double lineChartLayer::GetMaxX() 		{ return baseXYData::GetMaxX(); }
void  lineChartLayer::CurrentBounds(double xmin, double xmax)
										{ baseXYData::CurrentBounds(xmin, xmax);}

