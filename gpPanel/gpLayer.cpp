/**
 *	@file		gpLayer.cpp
 *	@author     Jussi Vatjus-Anttila
 *	@date		10.09.2009
 *	@version	0.1
 *
 *  Purpose:         gpLayer implementations
 *  Original Author: Jussi Vatjus-Anttila
 *  Created:         10/09/2009
 *  Last edit:       21/04/2012
 *  Copyright:       © Jussi Vatjus-Anttila
 *  Licence:         wxWindows Library License v3.1
 *	URL:			 http://code.google.com/p/gppanel/
 *
 *	@note
 *  See UML: http://jussiva.dyndns.org/gpPanel/gpPanelUML.png
 *
 */
#include "gpLayer.h"

gpLayer::gpLayer() : wxToolTip(wxEmptyString)
{
    yaxis = 0;
    xaxis = 0;
    nfo = 0;
    leg = 0;
    m_bitmap = 0;
    m_boundsLockXmin =
    m_boundsLockXmax =
    m_boundsLockYmax =
    m_boundsLockYmin = true;
    m_chartBoundaryIsDefined = false;
    m_visible = false;
    m_enable = true;
    m_refreshNeeded = true;
    m_sameFormatAsPreviously = false;
}

gpLayer::gpLayer(wxString name, wxString toolTip) : wxToolTip(toolTip)
{
    m_name = name;
    yaxis = 0;
    xaxis = 0;
    nfo = 0;
    leg = 0;
    m_bitmap = 0;


    m_boundsLockXmin =
    m_boundsLockXmax =
    m_boundsLockYmax =
    m_boundsLockYmin = true;
    m_chartBoundaryIsDefined = false;
    m_visible = false;
    m_enable = true;
    m_refreshNeeded = true;

    m_sameFormatAsPreviously = false;

    //default: tooltip is disabled
    wxToolTip::Enable(false);
    //default: support only default kind of layer
    gpChart_kind = gpCHART_DEFAULT;
    gpYaxis_type = gpAXIS_DEFAULT;
    gpXaxis_type = gpAXIS_DEFAULT;



    yLabelList[gpCHART_DEFAULT]         = _("Y");
    yLabelList[gpCHART_DNL]             = _("DNL");
    yLabelList[gpCHART_INL]             = _("INL");
    yLabelList[gpCHART_FFT]             = _("Y");
    yLabelList[gpCHART_ACCUMULATION]    = _("Accumulation");
    yLabelList[gpCHART_CUSTOM]          = _("Custom");

    xLabelList[gpCHART_DEFAULT]         = _("X");
    xLabelList[gpCHART_DNL]             = _("Y");
    xLabelList[gpCHART_INL]             = _("Y");
    xLabelList[gpCHART_FFT]             = _("Frequency");
    xLabelList[gpCHART_ACCUMULATION]    = _("Y");
    xLabelList[gpCHART_CUSTOM]          = _("x");


    for(int i=0; i< (int)gpCHART_END_; i++)
    {
        SetAxisUnitLabel((gpCHART_KIND)i, gpAXIS_DEFAULT,    _("X"), _("Y"));
        SetAxisUnitLabel((gpCHART_KIND)i, gpAXIS_DISTANCE,   _("m"), _("m"));
        SetAxisUnitLabel((gpCHART_KIND)i, gpAXIS_TIME,       _("s"), _("s"));
        SetAxisUnitLabel((gpCHART_KIND)i, gpAXIS_PROCENT,    _("%"), _("%"));
        SetAxisUnitLabel((gpCHART_KIND)i, gpAXIS_N,          _("n"), _("n"));
    }




    //POPUP LIST FOR CHART DEFAULT
    gpPopupGroup group(gpXAXIS, _("Unit"), _("Change Y axis units") );
    group.push( gpPopup( gpAXIS_DEFAULT,   ((wxMenu*)group)->Append(wxNewId(), _("Y"), _("Low signed bit"), wxITEM_RADIO)  ) );
    group.push( gpPopup( gpAXIS_PROCENT,   ((wxMenu*)group)->Append(wxNewId(), _("%"), _("Procent"), wxITEM_RADIO)  )  );
    group.push( gpPopup( gpAXIS_DISTANCE,    ((wxMenu*)group)->Append(wxNewId(), _("m"), _("Length"), wxITEM_RADIO)  )  );
    group.push( gpPopup( gpAXIS_TIME,      ((wxMenu*)group)->Append(wxNewId(), _("s"), _("Time"), wxITEM_RADIO)  )  );
    group.push( gpPopup( gpAXIS_N,         ((wxMenu*)group)->Append(wxNewId(), _("n"), _("count"), wxITEM_RADIO)  )  );
    group.push( gpPopup( gpAXIS_CUSTOM,    ((wxMenu*)group)->Append(wxNewId(), _("Custom"), _("Custom formula"), wxITEM_RADIO)  )  );
    popupGroupList[gpYAXIS] = group;

    group = gpPopupGroup(gpYAXIS, _("Unit"), _("Change X axis units") );
    group.push( gpPopup( gpAXIS_DEFAULT,   ((wxMenu*)group)->Append(wxNewId(), _("X"), _("Low signed bit"), wxITEM_RADIO)  )  );
    group.push( gpPopup( gpAXIS_PROCENT,   ((wxMenu*)group)->Append(wxNewId(), _("%"), _("Procent"), wxITEM_RADIO)  )  );
    group.push( gpPopup( gpAXIS_DISTANCE,    ((wxMenu*)group)->Append(wxNewId(), _("m"), _("Length"), wxITEM_RADIO)  )  );
    group.push( gpPopup( gpAXIS_TIME,      ((wxMenu*)group)->Append(wxNewId(), _("s"), _("Time"), wxITEM_RADIO)  )  );
    group.push( gpPopup( gpAXIS_N,         ((wxMenu*)group)->Append(wxNewId(), _("n"), _("count"), wxITEM_RADIO)  )  );
    group.push( gpPopup( gpAXIS_CUSTOM,    ((wxMenu*)group)->Append(wxNewId(), _("Custom"), _("Custom formula"), wxITEM_RADIO)  )  );
    popupGroupList[gpXAXIS] = group;

    group = gpPopupGroup(gpWINDOW, _("Chart"), _("Convert chart") );
    group.push( gpPopup( gpCHART_DEFAULT,       ((wxMenu*)group)->Append(wxNewId(), _("Default"), _("Default mode"), wxITEM_RADIO)  ) );
    group.push( gpPopup( gpCHART_DNL,           ((wxMenu*)group)->Append(wxNewId(), _("DNL"), _("Differential nonlinearity"), wxITEM_RADIO)  )  );
    group.push( gpPopup( gpCHART_INL,           ((wxMenu*)group)->Append(wxNewId(), _("INL"), _("Integral nonlinearity"), wxITEM_RADIO)  )  );
    group.push( gpPopup( gpCHART_ACCUMULATION,  ((wxMenu*)group)->Append(wxNewId(), _("Accumulation"), _("Accumulation"), wxITEM_RADIO)  )  );
    group.push( gpPopup( gpCHART_FFT,           ((wxMenu*)group)->Append(wxNewId(), _("FFT"), _("Fourier transform"), wxITEM_RADIO)  )  );
    group.push( gpPopup( gpCHART_CUSTOM,        ((wxMenu*)group)->Append(wxNewId(), _("Custom"), _("Custom formula"), wxITEM_RADIO)  )  );
    popupGroupList[gpWINDOW] = group;

    popupGroupList[gpWINDOW].DisableAllItems();
    popupGroupList[gpXAXIS].DisableAllItems();
    popupGroupList[gpYAXIS].DisableAllItems();

    //Enable items for popups
    /*

    // Default
    popupGroupList[gpYAXIS].Enable(gpCHART_DEFAULT, gpYAXIS_DEFAULT);
    popupGroupList[gpYAXIS].Enable(gpCHART_DEFAULT, gpYAXIS_DISTANCE);
    popupGroupList[gpYAXIS].Enable(gpCHART_DEFAULT, gpYAXIS_TIME);
    popupGroupList[gpYAXIS].Enable(gpCHART_DEFAULT, gpYAXIS_CUSTOM);
    popupGroupList[gpXAXIS].Enable(gpCHART_DEFAULT, gpXAXIS_N);
    popupGroupList[gpXAXIS].Enable(gpCHART_DEFAULT, gpXAXIS_CUSTOM);

    //For DNL
    popupGroupList[gpYAXIS].Enable(gpCHART_DNL, gpYAXIS_DEFAULT);
    popupGroupList[gpYAXIS].Enable(gpCHART_DNL, gpYAXIS_DISTANCE);
    popupGroupList[gpYAXIS].Enable(gpCHART_DNL, gpYAXIS_TIME);
    popupGroupList[gpYAXIS].Enable(gpCHART_DNL, gpYAXIS_PROCENT);
    popupGroupList[gpYAXIS].Enable(gpCHART_DNL, gpYAXIS_CUSTOM);
    */

    //popupGroupList[gpYAXIS].SetLabel( gpYAXIS_DEFAULT, _("DEFAULT") );

}
gpLayer::~gpLayer()
{
}
wxString gpLayer::GetName()
{
    return m_name;
}
void gpLayer::SetName(wxString s)
{
    m_name = s;
}
void gpLayer::LockChartX(bool b){ m_chartLockX = b; }
void gpLayer::LockChartY(bool b){ m_chartLockY = b; }
bool gpLayer::GetLockX(){ return m_chartLockX; }
bool gpLayer::GetLockY(){ return m_chartLockY; }
void gpLayer::SetGraphPosition(int pos) { m_graphPosition = pos; }
int  gpLayer::GetGraphPosition() const  { return m_graphPosition; }
void gpLayer::SetChartBounds(bool b){ m_chartBoundaryIsDefined=b; }
void gpLayer::SetChartBounds( double xmin,
                              double xmax,
                              double ymin,
                              double ymax)
{
    if(xmin==xmax){xmin-=2;xmax+=2;}
    if(ymin==ymax){ymax+=2;}
    m_chartBoundaryXmin = xmin;
    m_chartBoundaryXmax = xmax;
    m_chartBoundaryYmin = ymin;
    m_chartBoundaryYmax = ymax;
    m_chartBoundaryIsDefined = true;
}
void gpLayer::SetChartBounds( double *xmin,
                              double *xmax,
                              double *ymin,
                              double *ymax)
{
    if(xmin)m_chartBoundaryXmin = *xmin;
    if(xmax)m_chartBoundaryXmax = *xmax;
    if(ymin)m_chartBoundaryYmin = *ymin;
    if(ymax)m_chartBoundaryYmax = *ymax;
    m_chartBoundaryIsDefined = true;
}
bool gpLayer::GetChartBounds( double& xmin,
                              double& xmax,
                              double& ymin,
                              double& ymax)
{
    if(!m_chartBoundaryIsDefined)
        return false;
    xmin = m_chartBoundaryXmin;
    xmax = m_chartBoundaryXmax;
    ymin = m_chartBoundaryYmin;
    ymax = m_chartBoundaryYmax;
    return m_chartBoundaryIsDefined;
}

void gpLayer::SetBoundLocks( bool xmin,
                             bool xmax,
                             bool ymin,
                             bool ymax)
{
    m_boundsLockXmin = xmin;
    m_boundsLockXmax = xmax;
    m_boundsLockYmin = ymin;
    m_boundsLockYmax = ymax;

}
void gpLayer::GetBoundLocks( bool& xmin,
                             bool& xmax,
                             bool& ymin,
                             bool &ymax)
{
    xmin=m_boundsLockXmin;
    xmax=m_boundsLockXmax;
    ymin=m_boundsLockYmin;
    ymax=m_boundsLockYmax;

}
double gpLayer::GetZoomIncrementFactory() { return 1.2; }
bool gpLayer::IsWindowScrollbarsEnabled() { return true; }



void gpLayer::RefreshLabels()
{
	wxString label;

	if(xaxis) xaxis->SetName( GetXAxisLabel(gpXaxis_type) );
	if(yaxis) yaxis->SetName( GetYAxisLabel(gpYaxis_type) );

	popupGroupList[gpYAXIS].Select(gpChart_kind);
	popupGroupList[gpXAXIS].Select(gpChart_kind);

}

void gpLayer::SetXAxisType(int kind)
{
	//mpX_TIME/ mpX_NORMAL / mpX_HOUR
	if(xaxis)xaxis->SetLabelMode(kind);
}
void gpLayer::SetSamplerate(double d){m_samplerate = d;}
void gpLayer::SetFftLength(int i){m_fft_lenght = i;}


bool gpLayer::RefreshNeeded() { return m_refreshNeeded; }

void gpLayer::Refresh()
{
	m_refreshNeeded = false;
	RefreshChart();
	RefreshToolTip();
	m_sameFormatAsPreviously = true;
}

bool gpLayer::GetVisibility()   		{ return m_visible; }
void gpLayer::SetVisibility(bool visible){ m_visible = visible; }
bool gpLayer::GetEnable()            	{ return m_enable; }
void gpLayer::SetEnable(bool enabled)	{ m_enable = enabled; }
mpScaleX* gpLayer::GetXAxis()			{ return xaxis; }
mpScaleY* gpLayer::GetYAxis()			{ return yaxis; }

void gpLayer::SetYXFormula(wxString yxformula)  { m_customYXFormula = yxformula; }
void gpLayer::SetXFormula(wxString xformula) 	{ m_customXFormula = xformula; }
void gpLayer::SetYFormula(wxString yformula) 	{ m_customYFormula = yformula; }
bool gpLayer::IsYXFormula()						{ return m_customYXFormula.IsEmpty()?false:true; }
bool gpLayer::IsXFormula()						{ return m_customXFormula.IsEmpty()?false:true; }
bool gpLayer::IsYFormula()						{ return m_customYFormula.IsEmpty()?false:true; }
wxString gpLayer::GetYXFormula() const			{ return m_customYXFormula; }
wxString gpLayer::GetXFormula() const 			{ return m_customXFormula; }
wxString gpLayer::GetYFormula() const 			{ return m_customYFormula; }

void  gpLayer::ShowGrid(bool visibility, int mode)
{
	if(xaxis && (mode&gpXAXIS) )xaxis->SetTicks(visibility);
	if(yaxis && (mode&gpYAXIS) )yaxis->SetTicks(visibility);
}
void gpLayer::ShowCornerMarkers(bool visibility) { }

void gpLayer::SetContinousLine(bool continous) { };

void gpLayer::ShowInfoLayer(bool visibility)
{
	if(nfo) nfo->SetVisible(visibility);
}
bool gpLayer::SelectXAxisScale(gpAXIS_SCALE type)
{
	//if(!IsXScaleSupported(type)) return false;
	if(!IsXPopupEnabled(gpChart_kind, type)) return false;
	gpXaxis_type = type;
	m_sameFormatAsPreviously = false;
	Refresh();
	RefreshLabels();

	return true;
}

bool gpLayer::SelectYAxisScale(gpAXIS_SCALE type)
{
	//if(!IsYScaleSupported(type)) return false;
	if(!IsYPopupEnabled(gpChart_kind, type)) return false;
	gpYaxis_type = type;
	m_sameFormatAsPreviously = false;
	Refresh();
	RefreshLabels();
	return true;
}

bool gpLayer::SetChartKind(gpCHART_KIND kind)
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


void gpLayer::SetLabel(gpCHART_KIND kind, gpLabel_e flg, wxString lb)
{
	switch(flg)
	{
		default:
		case(gpLABEL):      LabelList[kind]=lb; break;
		case(gpLABEL_X):    xLabelList[kind]=lb; break;
		case(gpLABEL_Y):    yLabelList[kind]=lb; break;
	}
}
void gpLayer::SetAxisUnitLabel(gpCHART_KIND kind, gpAXIS_SCALE axisType, wxString xlabel, wxString ylabel)
{
	xUnitList[kind][axisType] = xlabel;
	yUnitList[kind][axisType] = ylabel;
}
void gpLayer::SetAxisPopupLabel(gpAXIS_SCALE axisType, wxString xlabel, wxString ylabel)
{
	popupGroupList[gpXAXIS].SetLabel( axisType, xlabel );
	popupGroupList[gpYAXIS].SetLabel( axisType, ylabel );
}

void gpLayer::EnableMainPopup(gpCHART_KIND kind, bool enabled)
{
	popupGroupList[gpWINDOW].Enable(kind, enabled);
}
void gpLayer::EnablePopup( gpCHART_KIND kind, gpAXIS_SCALE axis, bool Xenabled, bool Yenabled)
{
	popupGroupList[gpXAXIS].Enable(kind, axis, Xenabled);
	popupGroupList[gpYAXIS].Enable(kind, axis, Yenabled);
}

bool gpLayer::IsChartTypeEnabled(gpCHART_KIND kind)
{
	return popupGroupList[gpWINDOW].IsEnabled(kind);
}
bool gpLayer::IsXPopupEnabled( gpCHART_KIND kind, gpAXIS_SCALE axis)
{
	return popupGroupList[gpXAXIS].IsEnabled(axis);
}
bool gpLayer::IsYPopupEnabled( gpCHART_KIND kind, gpAXIS_SCALE axis)
{
	return popupGroupList[gpYAXIS].IsEnabled(axis);
}

wxString gpLayer::GetXAxisLabel(gpAXIS_SCALE type)
{
	wxString label = xLabelList[gpChart_kind];
	if( type == gpAXIS_CUSTOM )
		label += _(" [X=") + m_customXFormula + _("]");
	else if( ! xUnitList[gpChart_kind][type].IsEmpty() )
		label += _(" [") + xUnitList[gpChart_kind][type] + _("]");
	return label;
}

wxString gpLayer::GetYAxisLabel(gpAXIS_SCALE type)
{
	wxString label = yLabelList[gpChart_kind];
	if( type == gpAXIS_CUSTOM )
		label += _(" [Y=") + m_customYFormula + _("]");
	else if( ! yUnitList[gpChart_kind][type].IsEmpty() )
		label += _(" [") + yUnitList[gpChart_kind][type] + _("]");
	return label;
}

/*wxString& gpLayer::GetUnitString(gpKindLabelList_t& list, int kind, int type)
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

/* BASIC FUNCTION FOR HANDLE mpLayers */


bool gpLayer::AddLayer(mpLayer* layer)
{
	if(LayerExist(layer)==false)
	{
		mpLayerList.push_back(layer);
		return true;
	}
	return false;   //layer exist already!
}

bool gpLayer::DelLayer(mpLayer* layer, bool alsoDeleteObject)
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

void gpLayer::DelAllLayers(bool alsoDeleteObjects )
{
	mpLayerList_t::iterator it;
	for(it = mpLayerList.begin(); it != mpLayerList.end(); it++)
	{
		DelLayer(*it, alsoDeleteObjects);
	}
}

mpLayer* gpLayer::GetLayerByName(wxString name)
{
	mpLayerList_t::iterator it;
	for(it= mpLayerList.begin(); it!=mpLayerList.end(); it++)
		if( (*it)->GetName() == name) return *it;
	return NULL;
}

mpLayerList_t* gpLayer::GetLayerList(){return &mpLayerList;}



/* FUNCTION FOR HANDLE POPUP GROUPS */

bool gpLayer::GetPopupGroup(int type, gpPopupGroup& group)
{
	if(popupGroupList.find(type)==popupGroupList.end())return false;
	group = popupGroupList[type];
	return true;
}

bool gpLayer::IdBelong(int id)
{
	gpPopupGroupList_t::iterator it;
	for(it=popupGroupList.begin(); it!= popupGroupList.end(); ++it)
	{
		if( it->second.IdBelong(id) ) return true;
	}
	if( MenuIdBelongs(id) ) return true;
	return false;
}


/* Function for Init gpLayers*/

bool gpLayer::IsThereContinousLines()    { return false; }
bool gpLayer::IsThereInfoLayer()         { return nfo?true:false; }
bool gpLayer::IsThereMarkCorner()        { return false; }

bool gpLayer::GetDefaultContinousLines() { return false; }
bool gpLayer::GetDefaultShowInfoLayer()  { return nfo ? nfo->IsVisible() : false; }
bool gpLayer::GetDefaultMarkCorners()    { return false; }




wxBitmap*   gpLayer::GetBitmap()      				{ return m_bitmap; }
void        gpLayer::SetBitmap( wxBitmap* bitmap) 	{ m_bitmap = bitmap; }
void        gpLayer::SetBitmap( wxString filename) 	{ m_bitmap = new wxBitmap(wxImage(filename)); }


/* PRIVATE */

bool gpLayer::LayerExist(mpLayer* layer)
{
	if(GetLayerIterator(layer)!=mpLayerList.end())return true;
	return false;
}
mpLayerList_t::iterator gpLayer::GetLayerIterator(mpLayer* layer)
{
	mpLayerList_t::iterator it;
	for(it= mpLayerList.begin(); it!=mpLayerList.end(); it++)
		if( *it == layer) return it;
	return mpLayerList.end();
}


