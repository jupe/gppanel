#include "gpLineLayer.h"


/*!
 *  \brief Constructor
 */
gpLineLayer::gpLineLayer(wxString label, wxString x_label, wxString y_label)
                        : gpLayer(label)
{
    //wxLogMessage(_("gpLineLayer::gpLineLayer()"));
    wxFont graphFont(11, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
                     wxFONTWEIGHT_NORMAL);

    xaxis = new mpScaleX(x_label, mpALIGN_BOTTOM, true, mpX_NORMAL);
    xaxis->SetFont(graphFont);
    xaxis->SetVisible(true);
    xaxis->SetDrawOutsideMargins(false);

    yaxis = new mpScaleY(y_label, mpALIGN_LEFT, true);
    yaxis->SetFont(graphFont);
    yaxis->SetDrawOutsideMargins(false);
    yaxis->SetVisible(true);


    /*note = new mpNoteLegend(wxRect(200,20,40,40), &hatch2);
    note->SetVisible(true);
    note->SetName(_("Average: -\nDeviation: -\nMin/Max: -\n"));*/

    wxToolTip::Enable(false);
    this->SetDelay(5000);
    this->SetTip( label +_("\n                                                                                                                                            ") );


    //default labels for axis
    LabelList[gpCHART_DEFAULT] = label;
    xLabelList[gpCHART_DEFAULT] = x_label;
    yLabelList[gpCHART_DEFAULT] = y_label;

    LabelList[gpCHART_INL] = label;
    xLabelList[gpCHART_INL] = x_label;
    yLabelList[gpCHART_INL] = _("INL");

    LabelList[gpCHART_DNL] = label;
    xLabelList[gpCHART_DNL] = x_label;
    yLabelList[gpCHART_DNL] = _("DNL");

    LabelList[gpCHART_FFT] = label;
    xLabelList[gpCHART_FFT] = _("Frequency");
    yLabelList[gpCHART_FFT] = _("Y");

    LabelList[gpCHART_ACCUMULATION] = label;
    xLabelList[gpCHART_ACCUMULATION] = _("Y");
    yLabelList[gpCHART_ACCUMULATION] = _("Accumulation");

    LabelList[gpCHART_CUSTOM] = label;
    xLabelList[gpCHART_CUSTOM] = _("X");
    yLabelList[gpCHART_CUSTOM] = _("custom");


    popupGroupList[gpWINDOW].Enable(gpCHART_DEFAULT);
    popupGroupList[gpWINDOW].Enable(gpCHART_DNL);
    popupGroupList[gpWINDOW].Enable(gpCHART_INL);
    popupGroupList[gpWINDOW].Enable(gpCHART_FFT);
    popupGroupList[gpWINDOW].Enable(gpCHART_ACCUMULATION);
    popupGroupList[gpWINDOW].Enable(gpCHART_CUSTOM);

    //          chart type       axis type          X       Y
    EnablePopup(gpCHART_DEFAULT, gpAXIS_DEFAULT,    true,  true);
    EnablePopup(gpCHART_DEFAULT, gpAXIS_DISTANCE,   false, false);
    EnablePopup(gpCHART_DEFAULT, gpAXIS_N,          false, false);
    EnablePopup(gpCHART_DEFAULT, gpAXIS_PROCENT,    false, true);
    EnablePopup(gpCHART_DEFAULT, gpAXIS_TIME,       true,  false);
    EnablePopup(gpCHART_DEFAULT, gpAXIS_CUSTOM,     true,   true);

    //          chart type       axis type          X       Y
    EnablePopup(gpCHART_DNL, gpAXIS_DEFAULT,        true,  true);
    EnablePopup(gpCHART_DNL, gpAXIS_DISTANCE,       false, false);
    EnablePopup(gpCHART_DNL, gpAXIS_N,              true,  false);
    EnablePopup(gpCHART_DNL, gpAXIS_PROCENT,        false, true);
    EnablePopup(gpCHART_DNL, gpAXIS_TIME,           true,  false);
    EnablePopup(gpCHART_DNL, gpAXIS_CUSTOM,         false, false);

    //          chart type       axis type          X       Y
    EnablePopup(gpCHART_INL, gpAXIS_DEFAULT,        true,  true);
    EnablePopup(gpCHART_INL, gpAXIS_DISTANCE,       false, false);
    EnablePopup(gpCHART_INL, gpAXIS_N,              true,  false);
    EnablePopup(gpCHART_INL, gpAXIS_PROCENT,        false, true);
    EnablePopup(gpCHART_INL, gpAXIS_TIME,           true,  false);
    EnablePopup(gpCHART_INL, gpAXIS_CUSTOM,         false, false);

    //          chart type              axis type           X       Y
    EnablePopup(gpCHART_ACCUMULATION, gpAXIS_DEFAULT,       true,   true);
    EnablePopup(gpCHART_ACCUMULATION, gpAXIS_DISTANCE,      false,  false);
    EnablePopup(gpCHART_ACCUMULATION, gpAXIS_N,             false,  false);
    EnablePopup(gpCHART_ACCUMULATION, gpAXIS_PROCENT,       true,   true);
    EnablePopup(gpCHART_ACCUMULATION, gpAXIS_TIME,          false,  false);
    EnablePopup(gpCHART_ACCUMULATION, gpAXIS_CUSTOM,        false,  false);




    this->AddLayer( xaxis );
    this->AddLayer( yaxis );

    //this->AddLayer( note );


    samplerate = 1;
    fftlenght = 64;

    RefreshLabels();
}


/*!
 *  \brief Destructor
 */
gpLineLayer::~gpLineLayer()
{

    //layers deleting handle mpWindow!;
    //wxDELETE(chartLayer);

    for(unsigned int i=0; i< m_Series.size(); i++)
    {
        //wxLogDebug( _T("Deleting a series") );
        delete m_Series[i];
    }
    //wxLogDebug( _T("Deleting axis") );
    //wxLogDebug( _T("xaxis = %p"), xaxis );
    //delete xaxis;
    //delete yaxis;
}


mpPointLayer* gpLineLayer::AddSinglePoint(wxString name, wxString SeriesLabel,
                                        wxWindow* parent)
{
    mpLayer *Series = FindLayer( SeriesLabel );
    mpPointLayer *point = new mpPointLayer(Series, name,wxLIGHT_GREY_BRUSH,
                                           0 ,PointId = wxNewId());
    point->SetVisible(true);
    point->EnableModify(true);
    point->DragOnlyPoint(true);
    point->SetShapeSize(5);
    point->SetShape(mpDIAMOND);
    point->SetTaggingShape(mpVERTICAL_LINE/*|mpHORIZONTAL_LINE|mpCROSS_LINES*/);
    point->SetParent(parent);
    pointList.push_back(point);
    this->AddLayer( point );
    return point;
}


mpPointLayer* gpLineLayer::AddDualPoint(wxString name1, wxString name2,
                                      wxString SeriesLabel, wxWindow* parent)
{
    mpPointLayer *point2 = AddSinglePoint(name2, SeriesLabel, parent),
                 *point1 = AddSinglePoint(name1, SeriesLabel, parent);

    point1->SetSecondPoint( point2 );
    point1->ShowShadows(2);
    return point1;
}

void gpLineLayer::SetPointShape(int i)
{
    mpPointList_t::iterator it;
    for(it=pointList.begin(); it!= pointList.end(); it++)
    {
        switch(i)
        {
            case(0): (*it)->SetShape(mpCIRCLE);break;
            case(1): (*it)->SetShape(mpSQUARE);break;
            case(2): (*it)->SetShape(mpDIAMOND);break;
            case(3): (*it)->SetShape(mpELLIPSE);break;
            case(4): (*it)->SetShape(mpARROW_UP);break;
            case(5): (*it)->SetShape(mpARROW_DOWN);break;
            case(6): (*it)->SetShape(mpARROW_UPDOWN);break;
            default: break;
        }
    }
}


void gpLineLayer::SetTaggingShape(int i)
{
    mpPointList_t::iterator it;
    for(it=pointList.begin(); it!= pointList.end(); it++)
    {
        switch(i)
        {
            case(0): (*it)->SetTaggingShape(mpVERTICAL_LINE); break;
            case(1): (*it)->SetTaggingShape(mpHORIZONTAL_LINE); break;
            case(2): (*it)->SetTaggingShape(mpCROSS_LINES); break;
            default: break;
        }
    }
}


void gpLineLayer::SetFormula(wxString yformula, wxString xformula)
{
    m_customXFormula = xformula;
    m_customYFormula = xformula;
    yLabelList[gpCHART_CUSTOM] = _("Y= ") + m_customYFormula;
    if(!m_customXFormula.IsEmpty())
        xLabelList[gpCHART_CUSTOM] = _("X= ") + m_customXFormula;
}


//void gpLineLayer::DataSet(xyMultimap_t &data)
//{
//    chartData.DataSet(data);
//    m_refreshNeeded = true;
//}


/*!
 *  \brief Pushes data to the Series
 *
 *  \param x
 *  The X value to push
 *
 *  \param y
 *  The y value to push
 *
 *  \param label
 *  The label of the Series to push to
 */
void gpLineLayer::myDataPush(double x, double y, wxString SeriesLabel)
{
    gpSeries *pointer = FindSeries( SeriesLabel );
    if( pointer == NULL )
    {
       // wxLogDebug( _T("ERROR, could not find series") );
        return;
    }
    pointer->DataPush(x,y);
    m_refreshNeeded = true;
}


/*!
 *  \brief Clears the data from the specified series
 *
 *  \param SeriesLabel
 *  The label of the series to clear
 */
void gpLineLayer::DataClear( wxString SeriesLabel )
{
    gpSeries *pointer = FindSeries( SeriesLabel );
    if( pointer == NULL )
    {
        //wxLogDebug( _T("ERROR, could not find series") );
        return;
    }
    pointer->DataClear();
    m_refreshNeeded = true;
}


/*!
 *  \brief Refreshes the entire series
 */
void gpLineLayer::RefreshChart( void )
{
    for( unsigned int i=0; i< m_Series.size(); i++)
    {
        m_Series[i]->RefreshChart( gpChart_kind, samplerate, fftlenght,
            m_customXFormula, m_customYFormula,m_customYXFormula,
            gpXaxis_type, gpYaxis_type);
    }

}


//void gpLineLayer::RefreshToolTip()
//{
//    return;
//
//    double arith_mean1 = chartLayer->getArithmeticMean();
//    double geo_mean2 = chartLayer->getGeometricMean();
//    double harm_mean2 = chartLayer->getHarmonicMean();
//    double quadr_mean2 = chartLayer->getQuadraticMean();
//    double midrange = chartLayer->getMidrange(Y);
//    double stand_dev = chartLayer->getStandardDeviation();
//    double av_absdev_mean = chartLayer->getAverageAbsoluteDeviation(MEAN);
//    double av_absdev_median = chartLayer->getAverageAbsoluteDeviation(MEDIAN);
//    //double ar_mean2 = chartLayer->getAverageAbsoluteDeviation(MODE, 1);
//    double median = chartLayer->getMedian();
//    double maxY = chartLayer->GetMaxY();
//    double minY = chartLayer->GetMinY();
//    double maxX = chartLayer->GetMaxX();
//    double minX = chartLayer->GetMinX();
//
//    SetTip( LabelList[gpChart_kind] + _("\n")+
//            wxString::Format(_("Arithmetic mean: %.3f\n\
//                                Geometric mean: %.3f\n\
//                                Harmonic mean: %.3f\n\
//                                Quadratic mean: %.3f\n\
//                                Midrange: %.3f\n\
//                                Standard deviation: %.3f\n\
//                                Average absolute deviation mean: %.3f\n\
//                                Average absolute deviation median: %.3f\n\
//                                Median: %.3f\n\
//                                Y min / max: %.3f / %.3f\n\
//                                X min / max: %.3f / %.3f\
//                               "),
//                            arith_mean1
//                            ,geo_mean2
//                            ,harm_mean2
//                            ,quadr_mean2
//                            ,midrange
//                            ,stand_dev
//                            ,av_absdev_mean
//                            ,av_absdev_median
//                            ,median
//                            ,minY,maxY
//                            ,minX,maxX
//                )
//            );
//}


/*!
 *  \brief Adds another Series to the graph
 *
 *  \param label
 *  The name label of the series to add
 *
 *  \return a pointer to the newly added series
 */
gpSeries* gpLineLayer::AddSeriesLayer( wxString label )
{
    gpSeries *Series;

    Series =new gpSeries( label );

    m_Series.push_back( Series );

    Series->SetContinuity( true );
    Series->SetVisible( true );
    Series->ShowName( false );

    this->AddLayer( Series->GetLayer() );

    return Series;
}


/*!
 *  \brief Finds a series in the series vector
 *
 *  \param SeriesLabel
 *  label of the series to find
 *
 *  \return a pointer to the found series
 */
gpSeries* gpLineLayer::FindSeries( wxString SeriesLabel )
{
    for( unsigned int i=0; i<m_Series.size(); i++)
    {
        if(m_Series[i]->IsLabel( SeriesLabel ) )
        {
            return m_Series[i];
        }
    }

    return NULL;
}


/*!
 *  \brief Finds a lineChartLayer within the specified series
 *
 *  \param SeriesLabel
 *  The label of the series to find
 *
 *  \return a pointer to the lineChartLayer in the found series
 */
mpLayer* gpLineLayer::FindLayer( wxString SeriesLabel )
{
    gpSeries *pointer = FindSeries( SeriesLabel );
    if(pointer == NULL)
    {
        //wxLogDebug( _T("ERROR, could not find series") );
        return NULL;
    }
    else
    {
        return pointer->GetLayer();
    }
}


/*!
 *  \brief Sets the pen color/style of the specified series
 *
 *  \param pen
 *  a wxPen stye
 *
 *  \param SeriesLabel
 *  The label of the series to change the pen style
 */
void gpLineLayer::SetPen( wxPen pen, wxString SeriesLabel )
{
    FindSeries( SeriesLabel )->SetPen( pen );
}


/*!
 *  \brief Adds the Legend info to the gpLineLayer.
 *  This can be done at any time so the user can customize whether it
 *  appears on top of or below other layers.
 */
void gpLineLayer::AddLegendInfo( void )
{
    wxBrush hatch2(wxColour(163,208,212), wxSOLID);
    leg = new mpInfoLegend(wxRect(200,20,40,40), &hatch2);
    leg->SetVisible(true);

    this->AddLayer( leg );
}


/*!
 *  \brief Adds the coordinate info to the gpLineLayer.
 *  This can be done at any time so the user can customize whether it
 *  appears on top of or below the other layers.
 */

void gpLineLayer::AddCoordInfo( void )
{
    wxBrush hatch(wxColour(200,200,200), wxSOLID);
    nfo = new mpInfoCoords(wxRect(80,20,10,10), &hatch);
    nfo->SetVisible(true);
    //nfo->SetFormat(_("(X,Y):(%.3f,%.3f)"));

    this->AddLayer( nfo );
}


void gpLineLayer::DeleteSeries( wxString SeriesLabel )
{
    int indice;
    gpSeries *pointer = FindSeries( SeriesLabel );
    if(pointer == NULL)
    {
        //wxLogDebug( _T("ERROR, could not find series") );
        return;
    }
    DelAllLayers();
    return;
    //wxLogDebug( _T("Deleting layer") );
    mpLayer* object = pointer->GetLayer();
    this->DelLayer( object , false );

    //wxLogDebug( _T("Deleting Series") );
    indice = FindSeriesIndice( SeriesLabel );
    //wxLogDebug( _T("indice = %d"), indice);
    m_Series.erase( m_Series.begin() + indice );
    //wxLogDebug( _T("Series deleted") );

    delete pointer;
}


/*!
 *  \brief Finds the index of the specified series in the series vector
 *
 *  \param SeriesLabel
 *  label of the series to find
 *
 *  \return the indice of the series
 */
int gpLineLayer::FindSeriesIndice( wxString SeriesLabel )
{
    unsigned int i;
    for( i=0; i<m_Series.size(); i++)
    {
        if(m_Series[i]->IsLabel( SeriesLabel ) )
        {
            return i;
        }
    }

    return (int) i;
}
