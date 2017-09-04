#ifndef GPSERIES_H
#define GPSERIES_H

#include "gpLayer.h"
#include "baseData.h"
#include <vector>

class gpSeries
{
protected:
	wxString m_Label;
	lineChartLayer *m_Layer = nullptr;
	barChartLayer *m_barLayer = nullptr;
	baseXYData m_Data;
public:
	bool IsLabel( wxString CompareLabel );
	//! Constructor
	gpSeries( wxString label );
	//! Destructor
	virtual ~gpSeries( void );
	void invertY();
	void DataPush( double x, double y );
	void RefreshChart(gpCHART_KIND gpChart_kind, double samplerate,
		int fftlenght, wxString customXFormula, wxString customYFormula,
		wxString customYXFormula, gpAXIS_SCALE gpXaxis_type,
		gpAXIS_SCALE gpYaxis_type);
	void SetContinuity( bool continuity );
	void SetVisible( bool show );
	void ShowName( bool show );
	void SetPen( wxPen pen );
	void SetBrush(wxBrush brush);
	void DataClear( void );
	mpLayer* GetLayer( void );
	xyMultimap_t GetData( void );
	barChartLayer* getBarChartLayer();
};

#endif
