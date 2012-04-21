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
