#include <gpBarLayer.h>

gpSeries * gpBarLayer::FindSeries(wxString SeriesLabel)
{
	for (unsigned int i = 0; i<m_Series.size(); i++)
	{
		if (m_Series[i]->IsLabel(SeriesLabel))
		{
			return m_Series[i];
		}
	}
	return nullptr;
}

int gpBarLayer::FindSeriesIndice(wxString SeriesLabel)
{
	for (unsigned int i = 0; i < m_Series.size(); i++)
	{
		if (m_Series[i]->IsLabel(SeriesLabel))
		{
			return i;
		}
	}
	return 0;
}

void gpBarLayer::type(Type type)
{
	m_type = type;
}

gpBarLayer::gpBarLayer(wxString label, wxString x_label, wxString y_label, Type type)
{
	m_type = type;
	m_XLabel = x_label;
	m_YLabel = y_label;
	wxFont graphFont(11, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
		wxFONTWEIGHT_NORMAL);
	xaxis = new mpScaleX(x_label, mpALIGN_BOTTOM, false, mpX_NORMAL);
	xaxis->SetFont(graphFont);
	xaxis->SetVisible(true);
	xaxis->SetDrawOutsideMargins(false);
	yaxis = new mpScaleY(y_label, mpALIGN_LEFT, false);
	yaxis->SetFont(graphFont);
	yaxis->SetDrawOutsideMargins(false);
	yaxis->SetVisible(true);
	wxToolTip::Enable(false);
	this->SetDelay(5000);
	this->SetTip(label + _("\n                                                                                                                                            "));
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

	this->AddLayer(xaxis);
	this->AddLayer(yaxis);
	samplerate = 1;
	fftlenght = 64;
	gpChart_kind = gpCHART_DEFAULT;
	gpXaxis_type = gpAXIS_CUSTOM;
	gpYaxis_type = gpAXIS_CUSTOM;
	RefreshLabels();
}

gpBarLayer::~gpBarLayer()
{
	for (unsigned int i = 0; i< m_Series.size(); i++)
	{
		delete m_Series[i];
	}
}

void gpBarLayer::myDataPush(double x, double y, wxString SeriesLabel)
{
	gpSeries *pointer = FindSeries(SeriesLabel);
	if (pointer != nullptr)
	{
		pointer->DataPush(x, y);
		m_refreshNeeded = true;
	}
}

void gpBarLayer::DataClear(wxString SeriesLabel)
{
	gpSeries *pointer = FindSeries(SeriesLabel);
	if (pointer != nullptr)
	{
		pointer->DataClear();
		m_refreshNeeded = true;
	}
}

void gpBarLayer::RefreshChart(void)
{
	for (unsigned int i = 0; i< m_Series.size(); i++)
	{
		m_Series[i]->RefreshChart(gpChart_kind, 
								  samplerate, 
								  fftlenght,
								  m_customXFormula, 
								  m_customYFormula, 
								  m_customYXFormula,
								  gpXaxis_type, 
								  gpYaxis_type);
	}
}

gpSeries * gpBarLayer::AddSeriesLayer(wxString label)
{
	gpSeries *Series = new gpSeries(label);
	m_Series.push_back(Series);
	Series->SetContinuity(true);
	Series->SetVisible(true);
	Series->ShowName(false);
	Series->GetLayer();
	double min, max;
	switch (m_type)
	{
	case gpBarLayer::Type::BAR:
		this->AddLayer(Series->getBarChartLayer());
		break;
	case gpBarLayer::Type::LINE:
		this->AddLayer(Series->GetLayer());
		break;
	default:
		break;
	}	
	return Series;
}

mpLayer * gpBarLayer::FindLayer(wxString SeriesLabel)
{
	gpSeries *pointer = FindSeries(SeriesLabel);
	if (pointer != NULL)
	{
		return pointer->GetLayer();
	}
	return nullptr;
}

void gpBarLayer::SetPen(wxPen pen, wxString SeriesLabel)
{
	FindSeries(SeriesLabel)->SetPen(pen);
}

void gpBarLayer::DeleteSeries(wxString SeriesLabel)
{
	int indice;
	gpSeries *pointer = FindSeries(SeriesLabel);
	if (pointer != nullptr)
	{
		DelAllLayers();
		/*
		return;
		mpLayer* object = pointer->GetLayer();
		this->DelLayer(object, false);
		indice = FindSeriesIndice(SeriesLabel);
		m_Series.erase(m_Series.begin() + indice);
		*/
		delete pointer;
	}
}

void gpBarLayer::AddLegendInfo(int x, int y)
{
	wxBrush hatch2(wxColour(163, 208, 212), wxSOLID);
	leg = new mpInfoLegend(wxRect(x, y, 40, 40), &hatch2);
	leg->SetVisible(true);
	this->AddLayer(leg);
}

void gpBarLayer::AddCoordInfo(int x, int y)
{
	wxBrush hatch(wxColour(200, 200, 200), wxSOLID);
	nfo = new mpInfoCoords(wxRect(x, y, 10, 10), &hatch);
	nfo->SetFormat(m_XLabel + wxT(" = %f\n" + m_YLabel + wxT(" = %f")));
	nfo->SetVisible(true);
	this->AddLayer(nfo);
}

xyMultimap_t gpBarLayer::GetData(wxString SeriesLabel)
{
	xyMultimap_t result;
	gpSeries *pointer = FindSeries(SeriesLabel);
	if (pointer != nullptr)
	{
		result = pointer->GetData();
	}
	return result;
}

/*
mpLayerList_t * gpBarLayer::GetLayerList()
{
	return nullptr;
}
*/