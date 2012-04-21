/**
 *	@file		gpLayer.h
 *	@author     Jussi Vatjus-Anttila
 *	@date		10.09.2009
 *	@version	0.1
 *
 *  Purpose:         Common heritable class for all gpLayers
 *  Original Author: Jussi Vatjus-Anttila
 *  Created:         10/09/2009
 *  Last edit:       4/11/2009
 *  Copyright:       © Jussi Vatjus-Anttila
 *  Licence:         wxWindows Library License v3.1
 *	URL:			 http://code.google.com/p/gppanel/
 *
 *	@note
 *  See UML: http://jussiva.dyndns.org/gpPanel/gpPanelUML.png
 *
 */
#ifndef GPLAYER_H
#define GPLAYER_H

#include <deque>
#include <map>
#include <wx/tooltip.h>     //Hereditary class for menus
#include "mathplot.h"       //wxMathPlot
#include "gpMenu.h"         //Hereditary class for menus
#include "math/symbols.h"   //contains all math basic symbols

/**
*   gpLAYER type
*/
enum gpLAYER_TYPE
{
     gpLAYER_UNDEFINED
    ,gpLAYER_HISTOGRAM
    ,gpLAYER_HITS
    ,gpLAYER_LINE
    ,gpLAYER_TDCRESULT
};

/**
* gpLayer types
*/
enum gpCHART_KIND{
     gpCHART_DEFAULT        //!< Default type
    ,gpCHART_FFT            //!< x-axis is in frequence
    ,gpCHART_INL            //!< integral nonlinearity
    ,gpCHART_DNL            //!< differential nonlinearity
    ,gpCHART_ACCUMULATION   //!< Accumulation
    ,gpCHART_CUSTOM         //!< Custom formula by formula
    ,gpCHART_END_
};

/**
* gpAxis type
*/
enum gpAXIS_SCALE{
     gpAXIS_DEFAULT         //!< Defautl scaling
    ,gpAXIS_TIME            //!< Time scaling
    ,gpAXIS_PROCENT         //!< Procent scaling
    ,gpAXIS_DISTANCE        //!< Distance scaling
    ,gpAXIS_N               //!< N scaling
    ,gpAXIS_CUSTOM          //!< Custom scaling by formula
};

/**
* label enumeration
*/
enum gpLabel_e
{
    gpLABEL                 //!< default label
    ,gpLABEL_X              //!< X-axis label
    ,gpLABEL_Y              //!< Y-axis label
};


#include "gpPopup.h"


/** typedef for list of mpLayer */
typedef std::deque< mpLayer* > mpLayerList_t;
/** typedef for map for gpLabels */
typedef std::map<int, wxString> gpLabelList_t;
/** typedef for kind labels*/
typedef std::map<int, gpLabelList_t> gpKindLabelList_t;


/*
enum gpMenu_e
{
    gpXAXIS,
    gpYAXIS,
    gpWINDOW
};
*/


#define gpXAXIS 1		//!< X axis
#define gpYAXIS 2		//!< Y axis
#define gpWINDOW 3		//!< Window

/**
 *  Abstract class.
 *
 *	Graph Layer contrains:
 *	- N count of mpLayers
 *	- wxMenu which contains N count of wxMenuItems
 *
 *  gpWIndow contrains gpLayers
 *  One gpLayer is one mpWindow
 *
 */
class gpLayer : public gpMenu, public wxToolTip
{
    private:
        // Private variables

    	wxString 	    m_name;           	//!< gpLayer name
		mpLayerList_t 	mpLayerList;    	//!< this contains all mpLayers

		bool            m_visible;			//!< Flag that tells is layer visible
		bool            m_enable;			//!< Flag that tells is layer enabled
		bool            m_chartLockX;		//!< Chart Lock for X scale
		bool            m_chartLockY;		//!< Chart Lock for Y scale

        bool            m_chartBoundaryIsDefined;	//!< Flag true, if chart boundary is defined

        bool            m_boundsLockXmin,			//!< Flag for bound Lock X minimum value
                        m_boundsLockXmax,			//!< Flag for bound Lock X maximum value
                        m_boundsLockYmax,			//!< Flag for bound Lock Y minimum value
                        m_boundsLockYmin;			//!< Flag for bound Lock Y maximum value

        int             m_graphPosition;            //!< Graph position identify

    protected:
        int             m_fft_lenght;
        double          m_samplerate;


	public:
        /** Default constructor */
        gpLayer();
		/**
         *  Constructor
         *  @param name Name of gpLayer
		 *	@param toolTip	ToolTip string
         */
		gpLayer(wxString name, wxString toolTip = wxEmptyString);
		/**
		*   Destructor
		*   gpPanel take carry out the deletings of mpLayers
		*/
		virtual ~gpLayer();

		wxString            GetName();                             //!< @return   Get gpLayer name
		void                SetName(wxString s);                   //!< @param    s   set layer name

		virtual const int   gpType() = 0;                          //!< @return   Get gpLayer type
		virtual mpLayer*    GetLayer()       { return 0; }         //!< @return   Get mpLayer of gpLayer


		void LockChartX(bool b);                                   //!< @param  b LockX
		void LockChartY(bool b);                                   //!< @param  b LockY
		bool GetLockX();                                           //!< @return Get LockX
		bool GetLockY();                                           //!< @return Get LockY

		void SetGraphPosition(int pos){ m_graphPosition = pos; }
		int GetGraphPosition() const{ return m_graphPosition; }


        void SetChartBounds(bool b);								//!< @param b Enable/Disable Chart bounds
		/**
		 * 	Set Chart bounds
		 * 	@param xmin X-min
		 *	@param xmax X-max
		 *	@param ymin	Y-min
		 *	@param ymax Y-max
		 */
		void SetChartBounds(double xmin, double xmax, double ymin, double ymax);
		/**
		*   Same as before, but now doubles in pointers, NULL pointer not set.
		*/
		void SetChartBounds(double *xmin, double *xmax, double *ymin, double *ymax);
		/**
		*	Get chart boundary values
		*	@param xmin[out]	X min
		*	@param xmax[out]	X max
		*	@param ymin[out]	Y min
		*	@param ymax[out]	Y max
		*	@return true if bounds defined
		*/
		bool GetChartBounds(double& xmin, double& xmax, double& ymin, double& ymax);
		/**
		*	Set chart boundary locks
		*	@param xmin	X min lock
		*	@param xmax	X max lock
		*	@param ymin	Y min lock
		*	@param ymax	Y min lock
		*/
        void SetBoundLocks(bool xmin, bool xmax, bool ymin, bool ymax);
		/**
		*	Get chart boundary locks
		*	@param xmin	X min lock
		*	@param xmax	X max lock
		*	@param ymin	Y min lock
		*	@param ymax	Y min lock
		*/
		void GetBoundLocks(bool& xmin, bool& xmax, bool& ymin, bool &ymax);
		/** Get Zoom Increment Factory */
		double GetZoomIncrementFactory();
		/** @return true if Window scrollbars Enabled
		 */
		bool IsWindowScrollbarsEnabled();

        /**
        *   Inhrerited function for
        *   push data to layer
        *   @param x    x
        *   @param y    y
        *   @param z    for future usage
        */
		virtual void        DataPush(double x, double y, double z=0){}
		/** Inhrerited function for clear data content
        */
		virtual void        DataClear()      { }
		/* * Inhrerited function for resize data size
         * /
		virtual void        DataResize(int newsize) { }*/


        /** Inhrerited function for refresh chart
        */
		virtual void        RefreshChart() = 0;
		/** Inhrerited function for refresh tooltip
        */
		virtual void        RefreshToolTip() { }
		/** Inhrerited function for refresh labels
        */
		virtual void        RefreshLabels()
		{
            wxString label;

            if(xaxis) xaxis->SetName( GetXAxisLabel(gpXaxis_type) );
            if(yaxis) yaxis->SetName( GetYAxisLabel(gpYaxis_type) );

            popupGroupList[gpYAXIS].Select(gpChart_kind);
            popupGroupList[gpXAXIS].Select(gpChart_kind);

        }

        virtual void SetXAxisType(int kind)
        {
            //mpX_TIME/ mpX_NORMAL / mpX_HOUR
            if(xaxis)xaxis->SetLabelMode(kind);
        }
        virtual void SetSamplerate(double d){m_samplerate = d;}
        virtual void SetFftLength(int i){m_fft_lenght = i;}
        /** Inhrerited function for Set max value of x
        */
		//virtual void        SetMaxX(double x){ }
		/** Inhrerited function for Set min value of x
        */
		//virtual void        SetMinX(double x){ }
		/** Inhrerited function for Set max value of y
        */
		//virtual void        SetMaxY(double y){ }
		/** Inhrerited function for Set min value of y
        */
		//virtual void        SetMinY(double y){ }
		/** Inhrerited function for check if layer kind is supported
		*	@param kind  chart type to check
		*   @return true if supported
        */
		//virtual bool        IsSupported(gpCHART_KIND kind) { return false; }
		/** Inhrerited function for check if layer y axis type is supported
		*	@param type 	scale type to check
		*   @return true if supported
        */
		//virtual bool        IsXScaleSupported(gpAXIS_SCALE type) { return false; }
		/** Inhrerited function for check if layer X axis type is supported
		*	@param type 	scale type to check
		*   @return true if supported
        */
		//virtual bool        IsYScaleSupported(gpAXIS_SCALE type) { return false; }


        /** @return Tell if Refresh is needed */
        bool RefreshNeeded() { return m_refreshNeeded; }
        /** Refresh:
        *   -chart
        *   -tooltip
        */
        void Refresh()
        {
            m_refreshNeeded = false;
            RefreshChart();
            RefreshToolTip();
            m_sameFormatAsPreviously = true;
        }
        /** Get visibility.  @return visibility */
        bool GetVisibility()            { return m_visible; }
        /** Set visibility.  @param visibility boolean */
        void SetVisibility(bool visible){ m_visible = visible; }
        /** Get enable status.  @return enable status in boolean */
        bool GetEnable()            { return m_enable; }
        /** Set enablity.  @param enabled boolean */
        void SetEnable(bool enabled){ m_enable = enabled; }

        /** @return pointer to mpScaleX.
		 * exmpl. when want use axis popup menu */
        mpScaleX* GetXAxis(){ return xaxis; }

		/** @return pointer to mpScaleY. */
        mpScaleY* GetYAxis(){ return yaxis; }

        void SetYXFormula(wxString yxformula) { m_customYXFormula = yxformula; }	//!< @param yxformula Set formula for chart
        void SetXFormula(wxString xformula) { m_customXFormula = xformula; }		//!< @param xformula Set formula for x scale
        void SetYFormula(wxString yformula) { m_customYFormula = yformula; }		//!< @param yformula Set formula for y scale
        bool IsYXFormula(){ return m_customYXFormula.IsEmpty()?false:true; }		//!< @return true if chart formula is defined
        bool IsXFormula(){ return m_customXFormula.IsEmpty()?false:true; }			//!< @return true if X formula is defined
        bool IsYFormula(){ return m_customYFormula.IsEmpty()?false:true; }			//!< @return true if Y formula is defined
        wxString GetYXFormula() const { return m_customYXFormula; }					//!< @return chart formula
        wxString GetXFormula() const { return m_customXFormula; }					//!< @return X-scale formula
        wxString GetYFormula() const { return m_customYFormula; }					//!< @return Y-scale formula

        /**
         * @param  visibility   grid visibility
         * @param  mode         X&Y grids? default both: (gpXAXIS|gpYAXIS)
         */
		void  ShowGrid(bool visibility, int mode=gpXAXIS|gpYAXIS)
		{
		    if(xaxis && (mode&gpXAXIS) )xaxis->SetTicks(visibility);
		    if(yaxis && (mode&gpYAXIS) )yaxis->SetTicks(visibility);
		}
		/**
		 * Show Corner markers
		 * Not effects if inherited class not declare
		 * this virtual function
		 *  @param visibility
		 */
		virtual void ShowCornerMarkers(bool visibility) { }
		/**
		 * 	Set continous lines in chart
		 * 	Not effects if inherited class not declare
		 * 	this virtual function
		 *  @param continous
		 */
		virtual void SetContinousLine(bool continous) { };
		/**
		 *	Change event handler call this function
		 *	Change inherited class chart Y axel scale
		 *	@param type	  Y axis scale type
		 */
        /**
         *
         */
        virtual void ShowInfoLayer(bool visibility)
        {
            if(nfo) nfo->SetVisible(visibility);
        }
		bool SelectXAxisScale(gpAXIS_SCALE type)
		{
		    //if(!IsXScaleSupported(type)) return false;
		    if(!IsXPopupEnabled(gpChart_kind, type)) return false;
		    gpXaxis_type = type;
		    m_sameFormatAsPreviously = false;
		    Refresh();
		    RefreshLabels();

		    return true;
		}
		/**
		 *	Change event handler call this function
		 *	Change inherited class chart X axel scale
		 *	@param type	X axis scale type
		 *	@return true if success
		 */
		bool SelectYAxisScale(gpAXIS_SCALE type)
		{
		    //if(!IsYScaleSupported(type)) return false;
		    if(!IsYPopupEnabled(gpChart_kind, type)) return false;
		    gpYaxis_type = type;
		    m_sameFormatAsPreviously = false;
		    Refresh();
		    RefreshLabels();
		    return true;
		}
        /** Change chart kind
        *   @param kind     kind to change the layer
        *   @return true if is supported and modified layer
        */
		bool SetChartKind(gpCHART_KIND kind)
		{
		    //if(!IsSupported(kind))  return false;
		    if(!IsChartTypeEnabled(kind))  return false;
		    gpChart_kind = kind;

		    gpYaxis_type = gpAXIS_DEFAULT;
		    gpXaxis_type = gpAXIS_DEFAULT;

		    m_sameFormatAsPreviously = false;
		    RefreshChart();
		    RefreshLabels();

		    //last refresh tooltip for current view
		    /// @note maybe there is case, when tooltip
		    ///       don't want refresh to this kind of chart view...
		    ///       example with fft, wan't show tips before fft..
		    RefreshToolTip();
		    return true;
		}

		/**
		*	Set Label of:
		*	gpLABEL
			gpLABEL_X
			gpLABEL_Y
		*/
        void SetLabel(gpCHART_KIND kind, gpLabel_e flg, wxString lb)
        {
            switch(flg)
            {
                default:
                case(gpLABEL):      LabelList[kind]=lb; break;
                case(gpLABEL_X):    xLabelList[kind]=lb; break;
                case(gpLABEL_Y):    yLabelList[kind]=lb; break;
            }
        }
        void SetAxisUnitLabel(gpCHART_KIND kind, gpAXIS_SCALE axisType, wxString xlabel, wxString ylabel)
        {
            xUnitList[kind][axisType] = xlabel;
            yUnitList[kind][axisType] = ylabel;
        }
        void SetAxisPopupLabel(gpAXIS_SCALE axisType, wxString xlabel, wxString ylabel)
        {
            popupGroupList[gpXAXIS].SetLabel( axisType, xlabel );
            popupGroupList[gpYAXIS].SetLabel( axisType, ylabel );
        }

        void EnableMainPopup(gpCHART_KIND kind, bool enabled)
        {
            popupGroupList[gpWINDOW].Enable(kind, enabled);
        }
        void EnablePopup( gpCHART_KIND kind, gpAXIS_SCALE axis, bool Xenabled, bool Yenabled)
        {
            popupGroupList[gpXAXIS].Enable(kind, axis, Xenabled);
            popupGroupList[gpYAXIS].Enable(kind, axis, Yenabled);
        }

        bool IsChartTypeEnabled(gpCHART_KIND kind)
        {
            return popupGroupList[gpWINDOW].IsEnabled(kind);
        }
        bool IsXPopupEnabled( gpCHART_KIND kind, gpAXIS_SCALE axis)
        {
            return popupGroupList[gpXAXIS].IsEnabled(axis);
        }
        bool IsYPopupEnabled( gpCHART_KIND kind, gpAXIS_SCALE axis)
        {
            return popupGroupList[gpYAXIS].IsEnabled(axis);
        }

        wxString GetXAxisLabel(gpAXIS_SCALE type)
        {
            wxString label = xLabelList[gpChart_kind];
            if( type == gpAXIS_CUSTOM )
                label += _(" [X=") + m_customXFormula + _("]");
            else if( ! xUnitList[gpChart_kind][type].IsEmpty() )
                label += _(" [") + xUnitList[gpChart_kind][type] + _("]");
            return label;
        }

        wxString GetYAxisLabel(gpAXIS_SCALE type)
        {
            wxString label = yLabelList[gpChart_kind];
            if( type == gpAXIS_CUSTOM )
                label += _(" [Y=") + m_customYFormula + _("]");
            else if( ! yUnitList[gpChart_kind][type].IsEmpty() )
                label += _(" [") + yUnitList[gpChart_kind][type] + _("]");
            return label;
        }

        /*wxString& GetUnitString(gpKindLabelList_t& list, int kind, int type)
        {
            return list[kind][type];
            wxString label;
            gpKindLabelList_t::iterator ret;
            gpLabelList_t::iterator it;
            ret = list.find(kind);
            if( ret!=list.end() ){
                it = ret->second.find(type);
                if( it != ret->second.end() ) {
                    if( !it->second.IsEmpty() )
                        return &(*it);
                }
            }
            return 0;
        }*/

		/**************************************
		 * BASIC FUNCTION FOR HANDLE mpLayers
		 * @group  mpLayerHandler\
         *         mpLayer handler functions
         * @{
		 ************************************ */

        /**
         *  Add new mpLayer to gpLayer
         *  @param layer mpLayer to be added
         */
		bool AddLayer(mpLayer* layer)
		{
            if(LayerExist(layer)==false)
            {
                mpLayerList.push_back(layer);
                return true;
            }
            return false;   //layer exist already!
		}
		/**
		 *  Del layer.
		 *  Be default mpWindow deconstructor delete object also
		 *  @param layer                layer to be deleted
		 *  @param alsoDeleteObject     true if wan't delete object
		 *  @return true if layer founded and deleted
		 */
		bool DelLayer(mpLayer* layer, bool alsoDeleteObject = true)
		{
		    if(LayerExist(layer))
		    {
		        mpLayerList_t::iterator it = GetLayerIterator(layer);
                mpLayerList.erase( it );
                if(alsoDeleteObject)
                {
                    delete layer;
                }
		        return true;
		    }
		    return false;
		}
		/**
		 *  Del all layers
		 *  @param alsoDeleteObject     true if wan't delete object
		 */
		void DelAllLayers(bool alsoDeleteObjects = true )
		{
		    mpLayerList_t::iterator it;
		    for(it = mpLayerList.begin(); it != mpLayerList.end(); it++)
		    {
		        DelLayer(*it, alsoDeleteObjects);
		    }
		}
		/**
		 *  Get mpLayer by name
		 *  @param name Name of mpLayer
		 *  @return mpLayer pointer
		 */
		mpLayer* GetLayerByName(wxString name)
		{
		    mpLayerList_t::iterator it;
            for(it= mpLayerList.begin(); it!=mpLayerList.end(); it++)
                if( (*it)->GetName() == name) return *it;
            return NULL;
		}
		/**
		 *  @return mpLayerList_t*
		 */
		mpLayerList_t* GetLayerList(){return &mpLayerList;}


		/// @}


		/* FUNCTION FOR HANDLE POPUP GROUPS */

		/**
		 *	Get popup group
		 *	@param type		gpYAXIS/gpXAXIS/gpWINDOW
		 *	@param group[out]	popupgroup for type
		 *	@return true if foudn
		 */
		bool GetPopupGroup(int type, gpPopupGroup& group)
		{
		    if(popupGroupList.find(type)==popupGroupList.end())return false;
		    group = popupGroupList[type];
		    return true;
		}
		/**
		*	Test if menu/popup id belongs to this gpLayer
		*	Can detect which gpLayer is in use when
		*	called event handler.
		*	PopupGroup contains all wxMenuItems and those
		*	contains its unique id. That why we can detect it.
		*/
		bool IdBelong(int id)
		{
		    gpPopupGroupList_t::iterator it;
		    for(it=popupGroupList.begin(); it!= popupGroupList.end(); ++it)
		    {
		        if( it->second.IdBelong(id) ) return true;
		    }
		    if( MenuIdBelongs(id) ) return true;
		    return false;
		}


		/** Function for Init gpLayers*/


		virtual bool IsThereContinousLines()    { return false; }
		virtual bool IsThereInfoLayer()         { return nfo?true:false; }
		virtual bool IsThereMarkCorner()        { return false; }

		virtual bool GetDefaultContinousLines() { return false; }
		virtual bool GetDefaultShowInfoLayer()  { return nfo ? nfo->IsVisible() : false; }
		virtual bool GetDefaultMarkCorners()    { return false; }




		wxBitmap*   GetBitmap()      { return m_bitmap; }
		void        SetBitmap( wxBitmap* bitmap) { m_bitmap = bitmap; }
		void        SetBitmap( wxString filename) { m_bitmap = new wxBitmap(wxImage(filename)); }

    protected:

        /* Member variables for each gpLayer inherited class usage */
        mpScaleY        *yaxis;                 //!< Each gpLayer has reserved one yaxis
        mpScaleX        *xaxis;                 //!< Each gpLayer has reserved one xaxis
        mpInfoCoords    *nfo;                   //!< Each gpLayer has reserved one mpInfoCoords layer
        mpInfoLegend    *leg;                   //!< Each gpLayer has reserved one mpInfoLegend layer

		// chart kind and axis scales
        gpCHART_KIND    gpChart_kind;	        //!< Current chart kind (default: gpCHART_DEFAULT)
        gpAXIS_SCALE    gpYaxis_type	        //!< Current Y scale (default: gpAXIS_DEFAULT)
                       ,gpXaxis_type;	        //!< Current X scale (default: gpAXIS_DEFAULT)


        gpPopupGroupList_t  popupGroupList;	    //!< Popup group list

        gpLabelList_t   LabelList;              //!< kind label list for chart
        gpLabelList_t   xLabelList;             //!< kind X-label list for chart
        gpLabelList_t   yLabelList;             //!< kind Y-label list for chart

        gpKindLabelList_t   xUnitList;          //!< kind X-label list for chart
        gpKindLabelList_t   yUnitList;          //!< kind Y-label list for chart

        wxString        m_customYXFormula,      //!< custom formula for Y(X) =
                        m_customXFormula,       //!< custom formula for X axis. (example for scaling)
                        m_customYFormula;       //!< custom formula for Y axis. (example for scaling)

        double          m_chartBoundaryXmin;    //!< Chart boundary X min
		double          m_chartBoundaryXmax;    //!< Chart boundary X max
		double          m_chartBoundaryYmin;    //!< Chart boundary Y min
		double          m_chartBoundaryYmax;    //!< Chart boundary Y min


		bool            m_refreshNeeded;      	//!< Used when graph is allowed to refresh.

		wxBitmap        *m_bitmap;				//!< Each gpLayer is reserved for icon bitmap. example for wxToolbar.

		bool            m_sameFormatAsPreviously;   //!< This possible to update data so, that only changed data pushed to graph...


	//virtual
	private:

        /**
         *  Check if layer exist
         *  @param layer
         *  @return true if exist
         */
        bool LayerExist(mpLayer* layer)
        {
            if(GetLayerIterator(layer)!=mpLayerList.end())return true;
            return false;
        }
        /**
         *  Get Layer iterator by mpLayer
         *  @param layer    mpLayer*
         *  @return mpLayerList_t::iterator
         */
        mpLayerList_t::iterator GetLayerIterator(mpLayer* layer)
        {
            mpLayerList_t::iterator it;
            for(it= mpLayerList.begin(); it!=mpLayerList.end(); it++)
                if( *it == layer) return it;
            return mpLayerList.end();
        }
};

#endif
