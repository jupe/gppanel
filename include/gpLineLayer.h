#ifndef LINE_LAYER_H
#define LINE_LAYER_H

#include "gpLayer.h"
#include "baseData.h"
#include "gpSeries.h"

#include <vector>


#ifdef THIS_LAYER_TYPE
#undef THIS_LAYER_TYPE
#endif
#define THIS_LAYER_TYPE     gpLAYER_LINE

typedef std::deque<mpPointLayer*> mpPointList_t;

class gpLineLayer : public gpLayer
{
    protected:
        /* Protected variables */

        std::vector<gpSeries*> m_Series;
        //lineChartLayer *chartLayer;
        //baseXYData chartData;
        gpSeries* FindSeries( wxString SeriesLabel );
        int FindSeriesIndice( wxString SeriesLabel );

        mpNoteLegend    *note;

        mpPointList_t   pointList;
        int             PointId;

        double          samplerate;
        int             fftlenght;

    public:

        //! Constructor
        gpLineLayer (wxString label, wxString x_label, wxString y_label); //: gpLayer(label);

        //! Destructor
        virtual ~gpLineLayer();

		int GetPointCount()
            { return pointList.size(); }

        mpPointLayer* GetPointLayer(int index)
            { return pointList[index]; }

        mpPointLayer* AddSinglePoint(wxString name, wxString SeriesLabel,
                                     wxWindow* parent=0);

        mpPointLayer* AddDualPoint(wxString name1, wxString name2,
                                   wxString SeriesLabel, wxWindow* parent=0);


        void SetPointShape(int i);

        void SetTaggingShape(int i);


		void SetFormula(wxString yformula, wxString xformula=wxEmptyString);

        //void DataSet(xyMultimap_t &data);

        /*virtual void SetMaxX(double x){barLayer->GetMaxX()=x;}
		virtual void SetMaxY(double y){barLayer->GetMaxY()=y;}
		virtual void SetMinX(double x){barLayer->GetMinX()=x;}
		virtual void SetMinY(double y){barLayer->GetMinX()=y;}*/


        virtual void myDataPush(double x, double y, wxString SeriesLabel);

        virtual void DataClear( wxString SeriesLabel );

        virtual const int gpType( void )
            { return THIS_LAYER_TYPE; } //LAYER_LINE

        //virtual void ShowCornerMarkers(bool visibility)
        //    { chartLayer->MarkCorners(visibility); }

        //virtual void SetContinousLine(bool continous)
        //    { chartLayer->SetContinuity(continous); }

        virtual void RefreshChart( void );

        //virtual void RefreshToolTip();

        gpSeries* AddSeriesLayer( wxString label );
        mpLayer* FindLayer( wxString SeriesLabel );
        void SetPen( wxPen pen, wxString SeriesLabel );
        void DeleteSeries( wxString SeriesLabel );

        void AddLegendInfo( int x = 200, int y = 20 );
        void AddCoordInfo( int x = 80, int y = 20 );

        gpCHART_KIND GetChartKind( void )
            { return gpChart_kind; }

        xyMultimap_t GetData( wxString SeriesLabel );



    protected:




};

#endif
