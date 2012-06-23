/**
 *	@file		lineLayer.cpp
 *	@author     Jussi Vatjus-Anttila
 *	@date		10.09.2009
 *	@version	0.1
 *
 *  Purpose:         Example layer for line diagram
 *  Original Author: Jussi Vatjus-Anttila
 *  Created:         10/09/2009
 *  Last edit:       21/04/2012
 *  Copyright:       (c) Jussi Vatjus-Anttila
 *
 */

#ifndef LINELAYER_H
#define LINELAYER_H

#include "gpLayer.h"
#include "baseData.h"

#ifdef THIS_LAYER_TYPE
#undef THIS_LAYER_TYPE
#endif
#define THIS_LAYER_TYPE     gpLAYER_LINE

typedef std::deque<mpPointLayer*> mpPointList_t;

class lineLayer : public gpLayer
{
    protected:
        /* Protected variables */

        lineChartLayer  *chartLayer;    /// Chart layer
        baseXYData      chartData;      /// original chart data
        mpNoteLegend    *note;

        mpPointList_t   pointList;
        int             PointId;

        double          samplerate;
        int             fftlenght;

    public:
        /** Default constructor */
        lineLayer (wxString label, wxString x_label, wxString y_label): gpLayer(label)
        {
            //wxLogMessage(_("lineLayer::lineLayer()"));
            wxFont graphFont(11, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
            xaxis = new mpScaleX(x_label, mpALIGN_BOTTOM, true, mpX_NORMAL);
            xaxis->SetFont(graphFont);
            xaxis->SetVisible(true);
            xaxis->SetDrawOutsideMargins(false);

            yaxis = new mpScaleY(y_label, mpALIGN_LEFT, true);
            yaxis->SetFont(graphFont);
            yaxis->SetDrawOutsideMargins(false);
            yaxis->SetVisible(true);

            wxBrush hatch(wxColour(200,200,200), wxSOLID);
            nfo = new mpInfoCoords(wxRect(80,20,10,10), &hatch);
            nfo->SetVisible(true);
            //nfo->SetFormat(_("(X,Y):(%.3f,%.3f)"));

            wxBrush hatch2(wxColour(163,208,212), wxSOLID);
            leg = new mpInfoLegend(wxRect(200,20,40,40), &hatch2);
            leg->SetVisible(true);


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


            chartLayer = new lineChartLayer(label);
            chartLayer->SetContinuity(true);
            chartLayer->SetVisible(true);
            chartLayer->ShowName(false);

            this->AddLayer( chartLayer );
            this->AddLayer( xaxis );
            this->AddLayer( yaxis );
            this->AddLayer( nfo );
            this->AddLayer( leg );
            //this->AddLayer( note );


            samplerate = 1;
            fftlenght = 64;

            RefreshLabels();
        }
        /** Default destructor */
        virtual ~lineLayer()
		{
			//layers deleting handle mpWindow!;
			//wxDELETE(chartLayer);
		}
		int GetPointCount(){return pointList.size();}
        mpPointLayer* GetPointLayer(int index){return pointList[index];}

        mpPointLayer* AddSinglePoint(wxString name, wxWindow* parent=0)
        {
            mpPointLayer *point = new mpPointLayer(chartLayer, name,wxLIGHT_GREY_BRUSH,0,PointId = wxNewId());
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
        mpPointLayer* AddDualPoint(wxString name1, wxString name2, wxWindow* parent=0)
        {
            mpPointLayer *point2 = AddSinglePoint(name2, parent),
                         *point1 = AddSinglePoint(name1, parent);

            point1->SetSecondPoint( point2 );
            point1->ShowShadows(2);
            return point1;
        }

        void SetPointShape(int i)
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
        void SetTaggingShape(int i)
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


		void SetFormula(wxString yformula, wxString xformula=wxEmptyString)
		{
		    m_customXFormula = xformula;
		    m_customYFormula = xformula;
		    yLabelList[gpCHART_CUSTOM] = _("Y= ") + m_customYFormula;
		    if(!m_customXFormula.IsEmpty())
                xLabelList[gpCHART_CUSTOM] = _("X= ") + m_customXFormula;
        }

        void DataSet(xyMultimap_t &data)
        {
            chartData.DataSet(data);
            m_refreshNeeded = true;
        }
        /*virtual void SetMaxX(double x){barLayer->GetMaxX()=x;}
		virtual void SetMaxY(double y){barLayer->GetMaxY()=y;}
		virtual void SetMinX(double x){barLayer->GetMinX()=x;}
		virtual void SetMinY(double y){barLayer->GetMinX()=y;}*/
        virtual void DataPush(double x, double y, double z=0)
        {
            chartData.DataPush(x,y);
            m_refreshNeeded = true;
        }
        virtual void DataClear()
        {
            chartData.DataClear();
            m_refreshNeeded = true;
        }

        virtual const int   gpType(){return THIS_LAYER_TYPE;} //LAYER_LINE
        virtual mpLayer*    GetLayer(){return chartLayer;}

        virtual void ShowCornerMarkers(bool visibility)
        { chartLayer->MarkCorners(visibility); }
        virtual void SetContinousLine(bool continous)
        { chartLayer->SetContinuity(continous); }
        virtual void RefreshChart()
        {

            if(gpChart_kind== gpCHART_DEFAULT)
            {
                chartLayer->DataSet( chartData.GetData() );
            }
            else if(gpChart_kind== gpCHART_INL)
            {
                chartLayer->DataSet(chartData.GetINL(true));
            }
            else if(gpChart_kind== gpCHART_DNL)
            {
                chartLayer->DataSet(chartData.GetDNL(true));
            }
            else if(gpChart_kind== gpCHART_FFT)
            {
                chartLayer->DataSet( chartData.GetFft(fftlenght, samplerate) );
            }
            else if(gpChart_kind== gpCHART_CUSTOM)
            {
                chartLayer->DataSet( chartData.GetCustom(wxEmptyString, m_customYXFormula) );
            }
            else if(gpChart_kind== gpCHART_ACCUMULATION)
            {
                chartLayer->DataSet( chartData.GetAccumulation( gpYaxis_type == gpAXIS_PROCENT ) );

                if(chartLayer->GetMinY()>0)chartLayer->SetMinY(0);
            }

            if( gpYaxis_type == gpAXIS_CUSTOM )
                chartLayer->DataSet( chartLayer->GetCustom(wxEmptyString, m_customYFormula) );
            if( gpXaxis_type == gpAXIS_CUSTOM )
                chartLayer->DataSet( chartLayer->GetCustom(m_customXFormula, wxEmptyString) );

        }
        virtual void RefreshToolTip()
        {
            return;

            double arith_mean1 = chartLayer->getArithmeticMean();
            double geo_mean2 = chartLayer->getGeometricMean();
            double harm_mean2 = chartLayer->getHarmonicMean();
            double quadr_mean2 = chartLayer->getQuadraticMean();
            double midrange = chartLayer->getMidrange(Y);
            double stand_dev = chartLayer->getStandardDeviation();
            double av_absdev_mean = chartLayer->getAverageAbsoluteDeviation(MEAN);
            double av_absdev_median = chartLayer->getAverageAbsoluteDeviation(MEDIAN);
            //double ar_mean2 = chartLayer->getAverageAbsoluteDeviation(MODE, 1);
            double median = chartLayer->getMedian();
            double maxY = chartLayer->GetMaxY();
            double minY = chartLayer->GetMinY();
            double maxX = chartLayer->GetMaxX();
            double minX = chartLayer->GetMinX();

            SetTip( LabelList[gpChart_kind] + _("\n")+
                    wxString::Format(_("Arithmetic mean: %.3f\n\
Geometric mean: %.3f\n\
Harmonic mean: %.3f\n\
Quadratic mean: %.3f\n\
Midrange: %.3f\n\
Standard deviation: %.3f\n\
Average absolute deviation mean: %.3f\n\
Average absolute deviation median: %.3f\n\
Median: %.3f\n\
Y min / max: %.3f / %.3f\n\
X min / max: %.3f / %.3f\
"),
                                    arith_mean1
                                    ,geo_mean2
                                    ,harm_mean2
                                    ,quadr_mean2
                                    ,midrange
                                    ,stand_dev
                                    ,av_absdev_mean
                                    ,av_absdev_median
                                    //,ar_mean2
                                    ,median
                                    ,minY,maxY
                                    ,minX,maxX
                        )
                    );
        }




    protected:




};

#endif // LINELAYER_H
