
#include "gpSeries.h"

/*!
 *  \brief Constructor
 */
gpSeries::gpSeries( wxString label )
{
    m_Layer = new lineChartLayer( label );
	m_barLayer = new barChartLayer(label);
    m_Label = label;
}


/*!
 *  \brief Destructor
 */
gpSeries::~gpSeries( void )
{
    //delete m_Layer;
}


/*!
 *  \brief Calls a function to push data to m_Data
 *
 *  \param x
 *  The x value to push
 *
 *  \param y
 *  The y value to push
 */
void gpSeries::DataPush( double x, double y )
{
    m_Data.DataPush( x , y );
}


/*!
 *  \brief Refreshes the data from m_Data into m_Layer
 */
void gpSeries::RefreshChart( gpCHART_KIND gpChart_kind, double samplerate,
            int fftlenght, wxString customXFormula, wxString customYFormula,
            wxString customYXFormula, gpAXIS_SCALE gpXaxis_type,
            gpAXIS_SCALE gpYaxis_type )
{
    m_Layer->DataSet( m_Data.GetData() );
	m_barLayer->DataSet(m_Data.GetData());
	m_barLayer->SetGradientBackColour(false);

    if(gpChart_kind== gpCHART_DEFAULT)
    {
        m_Layer->DataSet( m_Data.GetData() );
    }

    else if(gpChart_kind== gpCHART_INL)
    {
        m_Layer->DataSet(m_Data.GetINL(true));
    }
    else if(gpChart_kind== gpCHART_DNL)
    {
        m_Layer->DataSet(m_Data.GetDNL(true));
    }
    else if(gpChart_kind== gpCHART_FFT)
    {
        m_Layer->DataSet( m_Data.GetFft(fftlenght, samplerate) );
    }
    else if(gpChart_kind== gpCHART_CUSTOM)
    {
        m_Layer->DataSet( m_Data.GetCustom(wxEmptyString,
                customYXFormula) );
    }
    else if(gpChart_kind== gpCHART_ACCUMULATION)
    {
        m_Layer->DataSet( m_Data.GetAccumulation(
                            gpYaxis_type == gpAXIS_PROCENT ) );

        if(m_Layer->GetMinY()>0)m_Layer->SetMinY(0);
    }

    if( gpYaxis_type == gpAXIS_CUSTOM )
    {
        m_Layer->DataSet( m_Layer->GetCustom(
                            wxEmptyString, customYFormula) );
    }

    if( gpXaxis_type == gpAXIS_CUSTOM )
    {
        m_Layer->DataSet( m_Layer->GetCustom(
                    customXFormula, wxEmptyString) );
    }
	 /*= getBarChartLayer()->GetMinY();
	double max = getBarChartLayer()->GetMaxY();
	getBarChartLayer()->SetMinY(max);
	getBarChartLayer()->SetMaxY(min);*/
}

/*!
 *  \brief Checks to see if a label is the same as the label for this series.
 *
 *  \param CompareLabel
 *  The label to check
 *
 *  \return
 *  True is the labels match.  False if they do not.
 */
bool gpSeries::IsLabel( wxString CompareLabel )
{
    if( CompareLabel == m_Label )
    {
        return true;
    }
    else
    {
        return false;
    }
}


/*!
 *  \brief Calls the SetContinuity function of m_Layer
 */
void gpSeries::SetContinuity( bool continuity )
{
    m_Layer->SetContinuity( continuity );
	m_barLayer->SetContinuity(continuity);
}


/*!
 *  \brief Calls the SetVisible function of m_Layer
 */
void gpSeries::SetVisible( bool show )
{
    m_Layer->SetVisible( show );
	m_barLayer->SetVisible(show);
}


/*!
 *  \brief Calls the ShowName function of m_Layer
 */
void gpSeries::ShowName( bool show )
{
   m_Layer->ShowName( show );
   m_barLayer->ShowName(show);
}

/*!
 *  \brief Gets a pointer to m_Layer
 *
 *  \return
 *  a pointer to m_Layer
 */
mpLayer* gpSeries::GetLayer( void )
{
    return m_Layer;
}


/*!
 *  \brief Calls the SetPen function of m_Layer
 */
void gpSeries::SetPen( wxPen pen )
{
    m_Layer->SetPen( pen );
	m_barLayer->SetPen(pen);
}

void gpSeries::SetBrush(wxBrush brush)
{
	m_Layer->SetBrush(brush);
	m_barLayer->SetBrush(brush);
}


/*!
 *  \brief Calls the DataClear function of m_Data
 */
void gpSeries::DataClear( void )
{
    m_Data.DataClear();
}


/*!
 *  \brief Gets the multimap data from m_Data
 *
 *  \return
 *  A multimap of the data
 */
xyMultimap_t gpSeries::GetData( void )
{
    return m_Data.GetData();
}

barChartLayer * gpSeries::getBarChartLayer()
{
	return m_barLayer;
}
