#ifndef __GP_BAR_LAYER__
#define __GP_BAR_LAYER__

#include "gpLayer.h"
#include "baseData.h"
#include "gpSeries.h"

#include <vector>


#ifdef THIS_LAYER_TYPE
#undef THIS_LAYER_TYPE
#endif
#define THIS_LAYER_TYPE gpLAYER_BAR


class gpBarLayer : public gpLayer
{
public:
	enum class Type
	{
		BAR,
		LINE
	};
protected:
	/* Protected variables */
	std::vector<gpSeries*> m_Series;
	mpNoteLegend    *note = nullptr;
	int             fftlenght;
	wxString        m_XLabel;
	wxString        m_YLabel;
	double          samplerate;
	barChartLayer* m_barChartLayer = nullptr;
	Type m_type;
	gpSeries* FindSeries(wxString SeriesLabel);
	int FindSeriesIndice(wxString SeriesLabel);
public:
	void type(Type type);
	//! Constructor
	gpBarLayer(wxString label, wxString x_label, wxString y_label, Type type = Type::LINE);
	virtual ~gpBarLayer();
	virtual void myDataPush(double x, double y, wxString SeriesLabel);
	virtual void DataClear(wxString SeriesLabel);
	virtual void RefreshChart(void);
	gpSeries* AddSeriesLayer(wxString label);
	mpLayer* FindLayer(wxString SeriesLabel);
	void SetPen(wxPen pen, wxString SeriesLabel);
	void DeleteSeries(wxString SeriesLabel);
	void AddLegendInfo(int x = 200, int y = 20);
	void AddCoordInfo(int x = 80, int y = 20);
	xyMultimap_t GetData(wxString SeriesLabel);
	//mpLayerList_t* GetLayerList() override;
	gpCHART_KIND GetChartKind(void)
	{
		return gpChart_kind;
	}
	virtual const int gpType(void)
	{
		return THIS_LAYER_TYPE;
	} //LAYER_LINE
};

#endif /** __GP_BAR_LAYER__ **/
