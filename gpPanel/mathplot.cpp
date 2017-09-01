/////////////////////////////////////////////////////////////////////////////
// Name:            mathplot.cpp
// Purpose:         Framework for plotting in wxWindows
// Original Author: David Schalig
// Maintainer:      Davide Rondini
// Contributors:    Jose Luis Blanco, Val Greene
// Created:         21/07/2003
// Last edit:       09/09/2007
// Copyright:       (c) David Schalig, Davide Rondini
// Licence:         wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "mathplot.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include <wx/window.h>
//#include <wx/wxprec.h>

// Comment out for release operation:
// (Added by J.L.Blanco, Aug 2007)
// #define MATHPLOT_DO_LOGGING

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/object.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/log.h>
#include <wx/intl.h>
#include <wx/dcclient.h>
#include <wx/cursor.h>
#endif

#include "mathplot.h"
#include <wx/bmpbuttn.h>
#include <wx/module.h>
#include <wx/msgdlg.h>
#include <wx/image.h>
#include <wx/tipwin.h>

#include <cmath>
#include <cstdio> // used only for debug
#include <ctime> // used for representation of x axes involving date

// #include "pixel.xpm"

// Memory leak debugging
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Legend margins
#define mpLEGEND_MARGIN 5
#define mpLEGEND_LINEWIDTH 10

// Minimum axis label separation
#define mpMIN_X_AXIS_LABEL_SEPARATION 64
#define mpMIN_Y_AXIS_LABEL_SEPARATION 32

// Number of pixels to scroll when scrolling by a line
#define mpSCROLL_NUM_PIXELS_PER_LINE  10

WX_DEFINE_LIST(PointList);

// See doxygen comments.
double mpWindow::zoomIncrementalFactor = 1.5;

#define MP_POP_LANGUAGE_EN


#ifdef MP_POP_LANGUAGE_EN
#define MP_POPMENU_CENTER               _("Center")
#define MP_POPMENU_CENTER_INFO          _("Center plot view to this position")
#define MP_POPMENU_FIT                  _("Fit")
#define MP_POPMENU_FIT_INFO             _("Set plot view to show all items")
#define MP_POPMENU_ZOOM_IN              _("Zoom in")
#define MP_POPMENU_ZOOM_IN_INFO         _("Zoom in plot view.")
#define MP_POPMENU_ZOOM_OUT             _("Zoom out")
#define MP_POPMENU_ZOOM_OUT_INFO        _("Zoom out plot view.")
#define MP_POPMENU_LOCK_ASPECT          _("Lock aspect")
#define MP_POPMENU_LOCK_ASPECT_INFO     _("Lock horizontal and vertical zoom aspect.")
#define MP_POPMENU_HELP                 _("Show mouse commands...")
#define MP_POPMENU_HELP_INFO            _("Show help about the mouse commands.")
#endif

#ifdef MP_POP_LANGUAGE_FI
#define MP_POPMENU_CENTER               _("Keskitä")
#define MP_POPMENU_CENTER_INFO          _("Keskitä ikkuna tähän pisteeseen")
#define MP_POPMENU_FIT                  _("Sovita")
#define MP_POPMENU_FIT_INFO             _("Sovita ikkunaan")
#define MP_POPMENU_ZOOM_IN              _("Zoomaus sisään")
#define MP_POPMENU_ZOOM_IN_INFO         _("Zoomaus sisään")
#define MP_POPMENU_ZOOM_OUT             _("Zoomaus ulos")
#define MP_POPMENU_ZOOM_OUT_INFO        _("Zoomaus ulos")
#define MP_POPMENU_LOCK_ASPECT          _("Lukitse suhde")
#define MP_POPMENU_LOCK_ASPECT_INFO     _("Lukitse suhde")
#define MP_POPMENU_HELP                 _("Tietoa hiirilekomennoista")
#define MP_POPMENU_HELP_INFO            _("Tietoa hiirilekomennoista")
#endif

//-----------------------------------------------------------------------------
// mpLayer
//-----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(mpLayer, wxObject)

mpLayer::mpLayer() : m_type(mpLAYER_UNDEF)
{
    SetPen((wxPen&) *wxBLACK_PEN);
    SetBrush((wxBrush&) *wxTRANSPARENT_BRUSH);
    SetFont((wxFont&) *wxNORMAL_FONT);
    m_continuous = FALSE; // Default
    m_showName   = TRUE;  // Default
    m_drawOutsideMargins = TRUE;
	m_visible = true;

	m_pointType  = mpPOINT;
	m_RectSize = wxSize(4,4);

	//m_drawingOrder = 5;
}

wxBitmap mpLayer::GetColourSquare(int side)
{
    wxBitmap square(side, side, -1);
    wxColour filler = m_pen.GetColour();
    wxBrush brush(filler, wxSOLID);
    wxMemoryDC dc;
    dc.SelectObject(square);
    dc.SetBackground(brush);
    dc.Clear();
    dc.SelectObject(wxNullBitmap);
    return square;
}

//-----------------------------------------------------------------------------
// mpInfoLayer
//-----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC_CLASS(mpInfoLayer, mpLayer)

mpInfoLayer::mpInfoLayer()
{
    m_dim = wxRect(0,0,1,1);
    m_brush = *wxTRANSPARENT_BRUSH;
    m_reference.x = 0; m_reference.y = 0;
    m_winX = 1; //parent->GetScrX();
    m_winY = 1; //parent->GetScrY();
    m_type = mpLAYER_INFO;
}

mpInfoLayer::mpInfoLayer(wxRect rect, const wxBrush* brush) : m_dim(rect)
{
    SetBrush(*brush);
    m_reference.x = rect.x;
    m_reference.y = rect.y;
    m_winX = 1; //parent->GetScrX();
    m_winY = 1; //parent->GetScrY();
    m_type = mpLAYER_INFO;
}

mpInfoLayer::~mpInfoLayer()
{

}

//void mpInfoLayer::UpdateInfo(mpWindow& w, wxEvent& event)
void mpInfoLayer::UpdateInfo(mpWindow& WXUNUSED(w), wxEvent& WXUNUSED(event))
{

}

bool mpInfoLayer::Inside(wxPoint& point)
{
    return m_dim.Contains(point);
}

void mpInfoLayer::Move(wxPoint delta)
{
    m_dim.SetX(m_reference.x + delta.x);
    m_dim.SetY(m_reference.y + delta.y);
}

void mpInfoLayer::UpdateReference()
{
    m_reference.x = m_dim.x;
    m_reference.y = m_dim.y;
}

void mpInfoLayer::Plot(wxDC & dc, mpWindow & w)
{
    if (m_visible) {
        // Adjust relative position inside the window
        int scrx = w.GetScrX();
        int scry = w.GetScrY();

		// Avoid dividing by 0
		if(scrx == 0) scrx=1;
		if(scry == 0) scry=1;

        if ((m_winX != scrx) || (m_winY != scry)) {
#ifdef MATHPLOT_DO_LOGGING
            // wxLogMessage(_("mpInfoLayer::Plot() screen size has changed from %d x %d to %d x %d"), m_winX, m_winY, scrx, scry);
#endif
            if (m_winX != 1) m_dim.x = (int) floor((double)(m_dim.x*scrx/m_winX));
            if (m_winY != 1) {
                m_dim.y = (int) floor((double)(m_dim.y*scry/m_winY));
                UpdateReference();
            }
            // Finally update window size
            m_winX = scrx;
            m_winY = scry;
        }
        dc.SetPen(m_pen);
//     wxImage image0(wxT("pixel.png"), wxBITMAP_TYPE_PNG);
//     wxBitmap image1(image0);
//     wxBrush semiWhite(image1);
        dc.SetBrush(m_brush);
        dc.DrawRectangle(m_dim.x, m_dim.y, m_dim.width, m_dim.height);
    }
}

wxPoint mpInfoLayer::GetPosition()
{
    return m_dim.GetPosition();
}

wxSize mpInfoLayer::GetSize()
{
    return m_dim.GetSize();
}

mpInfoCoords::mpInfoCoords(unsigned int x_labelType) : mpInfoLayer()		//LOGtest "unsigned int x_labelType" added
{
    m_labelType = x_labelType;
}

mpInfoCoords::mpInfoCoords(wxRect rect, const wxBrush* brush) : mpInfoLayer(rect, brush)
{
/* It seems that Windows port of wxWidgets don't support multi-line test to be drawn in a wxDC.
   wxGTK instead works perfectly with it.
   Info on wxForum: http://wxforum.shadonet.com/viewtopic.php?t=3451&highlight=drawtext+eol */
/*#ifdef _WINDOWS
    m_format = wxT("x = %f y = %f");
#else*/
    //If use DrawLabel instead of DrawText, multiline works windows port too.
    m_format = wxT("x = %f\ny = %f");
//#endif
}

mpInfoCoords::~mpInfoCoords()
{

}
void mpInfoCoords::UpdateInfo(mpWindow& w, wxEvent& event)
//void mpInfoLegend::UpdateInfo(mpWindow& WXUNUSED(w), wxEvent& WXUNUSED(event))
{
    if (event.GetEventType() == wxEVT_MOTION) {
        int mouseX = ((wxMouseEvent&)event).GetX();
        int mouseY = ((wxMouseEvent&)event).GetY();

       m_content.Printf(m_format, w.p2x(mouseX), w.p2y(mouseY));

    }
}

void mpInfoCoords::Plot(wxDC & dc, mpWindow & w)
{
    if (m_visible) {
        // Adjust relative position inside the window
        int scrx = w.GetScrX();
        int scry = w.GetScrY();
        if ((m_winX != scrx) || (m_winY != scry)) {
#ifdef MATHPLOT_DO_LOGGING
            // wxLogMessage(_("mpInfoLayer::Plot() screen size has changed from %d x %d to %d x %d"), m_winX, m_winY, scrx, scry);
#endif
            if (m_winX != 1) m_dim.x = (int) floor((double)(m_dim.x*scrx/m_winX));
            if (m_winY != 1 && m_winY!=0) {
                m_dim.y = (int) floor((double)(m_dim.y*scry/m_winY));
                UpdateReference();
            }
            // Finally update window size
            m_winX = scrx;
            m_winY = scry;
        }
        dc.SetPen(m_pen);
//     wxImage image0(wxT("pixel.png"), wxBITMAP_TYPE_PNG);
//     wxBitmap image1(image0);
//     wxBrush semiWhite(image1);
        dc.SetBrush(m_brush);
        dc.SetFont(m_font);
        int textX, textY;
        dc.GetMultiLineTextExtent(m_content, &textX, &textY);
        if (m_dim.width < textX + 10) m_dim.width = textX + 10;
        if (m_dim.height < textY + 10) m_dim.height = textY + 10;
        dc.DrawRectangle(m_dim.x, m_dim.y, m_dim.width, m_dim.height);
        wxRect tmp = m_dim;
        tmp.x += 5;tmp.y += 5;
        tmp.width -=10; tmp.height -=10;
        dc.DrawLabel(m_content, tmp);
    }
}

mpInfoLegend::mpInfoLegend() : mpInfoLayer()
{}

mpInfoLegend::mpInfoLegend(wxRect rect, const wxBrush* brush) : mpInfoLayer(rect, brush)
{}

mpInfoLegend::~mpInfoLegend()
{}

void mpInfoLegend::UpdateInfo(mpWindow& w, wxEvent& event)
{}

void mpInfoLegend::Plot(wxDC & dc, mpWindow & w)
{
    if (m_visible) {
        // Adjust relative position inside the window
        int scrx = w.GetScrX();
        int scry = w.GetScrY();
        if ((m_winX != scrx) || (m_winY != scry)) {
#ifdef MATHPLOT_DO_LOGGING
            // wxLogMessage(_("mpInfoLayer::Plot() screen size has changed from %d x %d to %d x %d"), m_winX, m_winY, scrx, scry);
#endif
            if (m_winX != 1) m_dim.x = (int) floor((double)(m_dim.x*scrx/m_winX));
            if (m_winY != 1) {
                m_dim.y = (int) floor((double)(m_dim.y*scry/m_winY));
                UpdateReference();
            }
            // Finally update window size
            m_winX = scrx;
            m_winY = scry;
        }
//     wxImage image0(wxT("pixel.png"), wxBITMAP_TYPE_PNG);
//     wxBitmap image1(image0);
//     wxBrush semiWhite(image1);
        dc.SetBrush(m_brush);
        dc.SetFont(m_font);
        const int baseWidth = (mpLEGEND_MARGIN*2 + mpLEGEND_LINEWIDTH);
        int textX = baseWidth, textY = mpLEGEND_MARGIN;
        int plotCount = 0;
        int posY = 0;
        int tmpX = 0, tmpY = 0;
        mpLayer* ly = NULL;
        wxPen lpen;
        wxString label;
        for (unsigned int p = 0; p < w.CountAllLayers(); p++) {
            ly = w.GetLayer(p);
            if ((ly->GetLayerType() == mpLAYER_PLOT) && (ly->IsVisible())) {
                label = ly->GetName();
                dc.GetTextExtent(label, &tmpX, &tmpY);
                textX = (textX > (tmpX + baseWidth)) ? textX : (tmpX + baseWidth + mpLEGEND_MARGIN);
                textY += (tmpY);
#ifdef MATHPLOT_DO_LOGGING
                // wxLogMessage(_("mpInfoLegend::Plot() Adding layer %d: %s"), p, label.c_str());
#endif
            }
        }
        dc.SetPen(m_pen);
        dc.SetBrush(m_brush);
        m_dim.width = textX;
		if (textY != mpLEGEND_MARGIN) { // Don't draw any thing if there are no visible layers
			textY += mpLEGEND_MARGIN;
			m_dim.height = textY;
			dc.DrawRectangle(m_dim.x, m_dim.y, m_dim.width, m_dim.height);
			for (unsigned int p2 = 0; p2 < w.CountAllLayers(); p2++) {
				ly = w.GetLayer(p2);
				if ((ly->GetLayerType() == mpLAYER_PLOT) && (ly->IsVisible())) {
					label = ly->GetName();
					lpen = ly->GetPen();
					dc.GetTextExtent(label, &tmpX, &tmpY);
					dc.SetPen(lpen);
					//textX = (textX > (tmpX + baseWidth)) ? textX : (tmpX + baseWidth);
					//textY += (tmpY + mpLEGEND_MARGIN);
					posY = m_dim.y + mpLEGEND_MARGIN + plotCount*tmpY + (tmpY>>1);
					dc.DrawLine(m_dim.x + mpLEGEND_MARGIN,   // X start coord
								posY,                        // Y start coord
								m_dim.x + mpLEGEND_LINEWIDTH + mpLEGEND_MARGIN, // X end coord
								posY);
					//dc.DrawRectangle(m_dim.x + 5, m_dim.y + 5 + plotCount*tmpY, 5, 5);
					dc.DrawText(label, m_dim.x + baseWidth, m_dim.y + mpLEGEND_MARGIN + plotCount*tmpY);
					plotCount++;
				}
			}
		}
    }
}


/**
*   mpNoteLegend by Jussi Vatjus-Anttila    21.9.2009
*/
mpNoteLegend::mpNoteLegend() : mpInfoLayer()
{}

mpNoteLegend::mpNoteLegend(wxRect rect, const wxBrush* brush) : mpInfoLayer(rect, brush)
{}

mpNoteLegend::~mpNoteLegend()
{}

void mpNoteLegend::UpdateInfo(mpWindow& w, wxEvent& event)
{}

void mpNoteLegend::Plot(wxDC & dc, mpWindow & w)
{
    if (m_visible) {
        // Adjust relative position inside the window
        int scrx = w.GetScrX();
        int scry = w.GetScrY();
        if ((m_winX != scrx) || (m_winY != scry)) {
#ifdef MATHPLOT_DO_LOGGING
            // wxLogMessage(_("mpInfoLayer::Plot() screen size has changed from %d x %d to %d x %d"), m_winX, m_winY, scrx, scry);
#endif
            if (m_winX != 1) m_dim.x = (int) floor((double)(m_dim.x*scrx/m_winX));
            if (m_winY != 1) {
                m_dim.y = (int) floor((double)(m_dim.y*scry/m_winY));
                UpdateReference();
            }
            // Finally update window size
            m_winX = scrx;
            m_winY = scry;
        }
        // Finally update window size
        m_winX = scrx;
        m_winY = scry;

        wxPen lpen;
        wxString label;

        dc.SetBrush(m_brush);
        dc.SetFont(m_font);
        dc.SetPen(m_pen);
        label = GetName();

        const int baseWidth = (mpLEGEND_MARGIN/**2*/ + mpLEGEND_LINEWIDTH);
        int textX = baseWidth, textY = mpLEGEND_MARGIN;
        int tmpX = 0, tmpY = 0;

        //dc.GetTextExtent(label, &tmpX, &tmpY);
        dc.GetMultiLineTextExtent(label, &tmpX, &tmpY);
        textX = (textX > (tmpX + baseWidth)) ? textX : (tmpX + baseWidth + mpLEGEND_MARGIN/2);
        textY += (tmpY);

        m_dim.width = textX;

        textY += mpLEGEND_MARGIN;
        m_dim.height = textY;
        dc.DrawRectangle(m_dim.x, m_dim.y, m_dim.width, m_dim.height);

        wxRect tmp(m_dim.x+baseWidth-mpLEGEND_MARGIN, m_dim.y+mpLEGEND_MARGIN, m_dim.width, m_dim.height);
        dc.DrawLabel(label, tmp );
    }
}


//-----------------------------------------------------------------------------
// mpLayer implementations - functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// mpFXYBar implementation - by Jussi Vatjus-Anttila 2009
//-----------------------------------------------------------------------------
IMPLEMENT_ABSTRACT_CLASS(mpFXYBar, mpLayer)

mpFXYBar::mpFXYBar(wxString name, int flags)
{
    SetName(name);
    m_flags = flags;
    m_type = mpLAYER_PLOT;
    m_pen.SetStyle(wxPENSTYLE_SOLID);
    m_gradienBackground = true;
    m_showLabel = true;
    m_useCustomLabel = false;
}

void mpFXYBar::Plot(wxDC & dc, mpWindow & w)
{
	if (m_visible) {
		dc.SetPen( m_pen );
		dc.SetBrush(m_brush);
		dc.SetFont( m_font );

		double x, y, lsb;
		wxString label;



		wxCoord startPx = m_drawOutsideMargins ? 0 : w.GetMarginLeft();
		wxCoord endPx   = m_drawOutsideMargins ? w.GetScrX() : w.GetScrX() - w.GetMarginRight();
		wxCoord minYpx  = m_drawOutsideMargins ? 0 : w.GetMarginTop();
		wxCoord maxYpx  = m_drawOutsideMargins ? w.GetScrY() : w.GetScrY() - w.GetMarginBottom();

		wxCoord xPx = 0, yPx = 0,
                widthPx, heightPx,
                y0Px = w.y2p(0),
                x0Px = w.x2p(0);

        Rewind();
        while (GetNextXY(x, y, label))
        {

            xPx = w.x2p(x);
            yPx = w.y2p(y);

            if(GetLSB(lsb))
            {
                widthPx = w.x2p(x+lsb)-xPx;
            }
            else widthPx = 1;
            xPx -= ( w.x2p(lsb/2) - x0Px );


            heightPx = y0Px-yPx;

            //this avoid balk to disapear when rolling up
            if(yPx < minYpx && (yPx+heightPx)>minYpx)
            {
                heightPx -= minYpx-yPx;
                yPx = minYpx;
            }
            if((yPx+heightPx)>maxYpx)
            {
                heightPx = maxYpx-yPx;
            }
            if(yPx > maxYpx){
                heightPx += (yPx-maxYpx);
                yPx = maxYpx;
            }

            if(xPx < startPx && (xPx+widthPx) > startPx )
            {
                widthPx -= startPx-xPx;
                xPx = startPx;
            }

            //show allways something, at least one pixel
            if( widthPx <= 1 ) widthPx=1;
            if( heightPx < 1 && heightPx > -1 ) heightPx=1;


            if (m_drawOutsideMargins || ((xPx >= startPx) && (xPx <= endPx) && (yPx >= minYpx) && (yPx <= maxYpx)))
            {
                if(!m_gradienBackground || widthPx==1 || heightPx==1 )
                {
					//Precisa fazer checagem se o gráfico é invertido ou não
                  //dc.DrawRectangle(xPx, yPx, widthPx, heightPx);
					dc.DrawRectangle(xPx, 0, widthPx, heightPx);
                }
                else{
                    dc.GradientFillLinear( wxRect( xPx, yPx, widthPx/2, heightPx),
                                                  wxColour(100,100,150),
                                                  wxColour(150,150,255),
                                                  wxDirection(wxRIGHT) );
                    dc.GradientFillLinear( wxRect( xPx+widthPx/2, yPx, widthPx/2, heightPx),
                                                  wxColour(100,100,150),
                                                  wxColour(150,150,255),
                                                  wxDirection(wxLEFT) );
                }



                if( m_showLabel )
                {
                    if(!m_useCustomLabel)
                    {
                        if( y==floor(y)){
                            label = wxString::Format(_("%.0f"), y);
                        } else{
                            label = wxString::Format(_("%.2f"), y+0.005); //rounding..
                        }
                    }
                    wxCoord dx, dy;
                    dc.GetTextExtent(label, &dx, &dy);
                    dy = (yPx>y0Px) ? (yPx+5) : (yPx-10-dy);

                    if(!label.IsEmpty() && dx < widthPx )
                    {

                        if(dy >=minYpx && dy < maxYpx){
                          //  dc.DrawText( label, xPx+widthPx/2-dx/2, dy);
							dc.DrawText(label, xPx + widthPx / 2 - dx / 2, heightPx);
                        }
                        label = _("");
                    }

                }

            }

            /*
            else if(((ix >= startPx) && (ix <= endPx) && (iy >= minYpx) && (iy <= maxYpx)))
            {
                if(ix < startPx && (ix+width) > startPx)
                    dc.DrawRectangle(w.x2p(0)+width, iy, width, height);
            }
            */


        }



		if (!m_name.IsEmpty() && m_showName)
		{

			wxCoord tx, ty;
			dc.GetTextExtent(m_name, &tx, &ty);

			// xxx implement else ... if (!HasBBox())
			{
				const int sx = w.GetScrX()>>1;
				const int sy = w.GetScrY()>>1;

				if ((m_flags & mpALIGNMASK) == mpALIGN_NE)
				{
					tx = sx - tx - 8;
					ty = -sy + 8;
				}
				else if ((m_flags & mpALIGNMASK) == mpALIGN_NW)
				{
					tx = -sx + 8;
					ty = -sy + 8;
				}
				else if ((m_flags & mpALIGNMASK) == mpALIGN_SW)
				{
					tx = -sx + 8;
					ty = sy - 8 - ty;
				}
				else
				{
					tx = sx - tx - 8;
					ty = sy - 8 - ty;
				}
			}

			dc.DrawText( m_name, tx, ty);
		}
	}
}
//-----------------------------------------------------------------------------
// mpFYXBar implementation - by Jussi Vatjus-Anttila 2009
//-----------------------------------------------------------------------------
IMPLEMENT_ABSTRACT_CLASS(mpFYXBar, mpLayer)

mpFYXBar::mpFYXBar(wxString name, int flags)
{
    SetName(name);
    m_flags = flags;
    m_type = mpLAYER_PLOT;
    m_pen.SetStyle(wxPENSTYLE_SOLID);
    m_brush = *wxTRANSPARENT_BRUSH;
    m_gradienBackground = false;
    m_showLabel = true;
    m_useCustomLabel = false;
}

void mpFYXBar::Plot(wxDC & dc, mpWindow & w)
{
	if (m_visible) {
		dc.SetPen   ( m_pen   );
		dc.SetBrush ( m_brush );
		dc.SetFont( m_font );

		double x, y, lsb;
		wxString label;


		wxCoord startPx = m_drawOutsideMargins ? 0 : w.GetMarginLeft();
		wxCoord endPx   = m_drawOutsideMargins ? w.GetScrX() : w.GetScrX() - w.GetMarginRight();
		wxCoord minYpx  = m_drawOutsideMargins ? 0 : w.GetMarginTop();
		wxCoord maxYpx  = m_drawOutsideMargins ? w.GetScrY() : w.GetScrY() - w.GetMarginBottom();

		wxCoord xPx = 0, yPx = 0, widthPx, heightPx,
                x0Px = w.x2p(0),
                y0Px = w.y2p(0);
		//int lsbPx = w.y2p(0)-w.y2p(lsb);


        Rewind();
        while (GetNextXY(x, y, label))
        {
            dc.SetBrush( m_brush );
            xPx = w.x2p(x);
            yPx = w.y2p(y);

            if(GetLSB(lsb))
            {
                heightPx = yPx - w.y2p(y+lsb);
            }
            else heightPx = 1;

            yPx -= ( y0Px - w.y2p(lsb/2) );

            widthPx = x0Px - xPx;



            //this avoid balk to disapear when rolling up/down
            if(yPx < minYpx && (yPx+heightPx)>minYpx)
            {
                heightPx -= minYpx-yPx;
                yPx = minYpx;
            }
            if((yPx+heightPx)>maxYpx && yPx <= maxYpx )
            {
                heightPx -= (yPx+heightPx - maxYpx);
            }

            //this avoid balk to disapear when rolling left/right

            //X < 0 + rolling left
            if(xPx < startPx && xPx+widthPx > startPx )
            {
                widthPx -=  startPx - xPx;
                xPx = startPx;
            }
            //X > 0 + rolling left
            if(xPx > startPx && xPx+widthPx < startPx)
            {
                widthPx -=  (xPx+widthPx)-startPx;
            }
            //rolling right
            if(xPx+widthPx < endPx && xPx > endPx)
            {
                widthPx +=  xPx - endPx ;
                xPx = endPx;
            }


            //show allways something, at least one pixel
            if( widthPx < 1 && widthPx > -1 ) widthPx=1;
            if( heightPx < 1) heightPx=1;


            if (m_drawOutsideMargins || ((xPx >= startPx) && (xPx <= endPx) && (yPx >= minYpx) && (yPx <= maxYpx)))
            {
                if(!m_gradienBackground || widthPx==1 || heightPx==1 )
                {
                    dc.DrawRectangle(xPx, yPx, widthPx, heightPx);
                }
                else{
                    dc.GradientFillLinear( wxRect( xPx, yPx, widthPx, heightPx/2),
                                                  wxColour(100,100,150),
                                                  wxColour(150,150,255),
                                                  wxDirection(wxUP) );
                    dc.GradientFillLinear( wxRect( xPx, yPx+heightPx/2, widthPx, heightPx/2),
                                                  wxColour(100,100,150),
                                                  wxColour(150,150,255),
                                                  wxDirection(wxDOWN) );
                }



                if(  m_showLabel)
                {

                        if(!m_useCustomLabel)
                        {
                            label = wxString::Format(_("%.2f"), y);
                        }

                        wxCoord dx, dy;
                        dc.GetTextExtent(label, &dx, &dy);
                        dx = (xPx<x0Px) ? (xPx-10-dx) : (xPx+10);

                        if(!label.IsEmpty() && dy <= heightPx)
                        {

                            if( dx >= startPx && dx < endPx ){
                                dc.DrawText( label, dx, yPx + heightPx/2-dy/2);
                        }
                        label.clear();
                    }
                }

            }

            /*
            else if(((ix >= startPx) && (ix <= endPx) && (iy >= minYpx) && (iy <= maxYpx)))
            {
                if(ix < startPx && (ix+width) > startPx)
                    dc.DrawRectangle(w.x2p(0)+width, iy, width, height);
            }
            */


        }



		if (!m_name.IsEmpty() && m_showName)
		{

			wxCoord tx, ty;
			dc.GetTextExtent(m_name, &tx, &ty);

			// xxx implement else ... if (!HasBBox())
			{
				const int sx = w.GetScrX()>>1;
				const int sy = w.GetScrY()>>1;

				if ((m_flags & mpALIGNMASK) == mpALIGN_NE)
				{
					tx = sx - tx - 8;
					ty = -sy + 8;
				}
				else if ((m_flags & mpALIGNMASK) == mpALIGN_NW)
				{
					tx = -sx + 8;
					ty = -sy + 8;
				}
				else if ((m_flags & mpALIGNMASK) == mpALIGN_SW)
				{
					tx = -sx + 8;
					ty = sy - 8 - ty;
				}
				else
				{
					tx = sx - tx - 8;
					ty = sy - 8 - ty;
				}
			}

			dc.DrawText( m_name, tx, ty);
		}
	}
}
//-----------------------------------------------------------------------------
// mpFXCandleStick implementation - by Jussi Vatjus-Anttila 2009
//-----------------------------------------------------------------------------
IMPLEMENT_ABSTRACT_CLASS(mpFXYCandleStick, mpLayer)

mpFXYCandleStick::mpFXYCandleStick(wxString name, int flags)
{
    SetName(name);
    m_flags = flags;
    m_type = mpLAYER_PLOT;
    m_pen.SetStyle(wxPENSTYLE_SOLID);
    m_gradienBackground = true;
    m_candleBrush = *wxBLACK_BRUSH;
    m_candleBrush2 = *wxWHITE_BRUSH;
    m_candlePen = *wxBLACK_PEN;
    m_stickPen = *wxBLACK_PEN;
}
void mpFXYCandleStick::Plot(wxDC & dc, mpWindow & w)
{
	if (m_visible) {
		dc.SetPen( m_pen );

		double x, yCandleMin, yCandleMax, yStickMin, yStickMax, lsb;
		wxCoord startPx = m_drawOutsideMargins ? 0 : w.GetMarginLeft();
		wxCoord endPx   = m_drawOutsideMargins ? w.GetScrX() : w.GetScrX() - w.GetMarginRight();
		wxCoord minYpx  = m_drawOutsideMargins ? 0 : w.GetMarginTop();
		wxCoord maxYpx  = m_drawOutsideMargins ? w.GetScrY() : w.GetScrY() - w.GetMarginBottom();

		wxCoord xPx = 0, yCandleMinPx, yCandleMaxPx, yStickMinPx, yStickMaxPx,widthPx, heightPx,
                stickXpx;

        Rewind();
        while (GetNext(x, yCandleMin, yCandleMax, yStickMin, yStickMax))
        {
            dc.SetBrush( m_brush );
            xPx = w.x2p(x);
            yCandleMinPx = w.y2p(yCandleMax);
            yCandleMaxPx = w.y2p(yCandleMin);
            yStickMinPx = w.y2p(yStickMax);
            yStickMaxPx = w.y2p(yStickMin);

            //calculate bar width
            if(GetLSB(lsb))
            {
                widthPx = w.x2p(x+lsb)-xPx;
            }
            else widthPx = 1;
            stickXpx=xPx+widthPx/2;

            heightPx = yCandleMaxPx-yCandleMinPx;


            //this avoid balk to disapear when rolling left/rigth up/down

            //stick corrections
            if(yStickMaxPx > maxYpx && yStickMinPx < maxYpx)yStickMaxPx = maxYpx;
            if(yStickMinPx < minYpx && yStickMaxPx > minYpx)yStickMinPx = minYpx;

            //rolling down
            if( yCandleMinPx > maxYpx && yCandleMaxPx > maxYpx);
            else if( yCandleMinPx > maxYpx)
            {
                yCandleMinPx = maxYpx;
                heightPx = yCandleMaxPx-yCandleMinPx;
            }
            else if(yCandleMaxPx > maxYpx)
            {
                yCandleMaxPx=maxYpx;
                heightPx = yCandleMaxPx-yCandleMinPx;
            }

            //rolling up
            if( yCandleMinPx < minYpx && yCandleMaxPx < minYpx);
            else if( yCandleMinPx < minYpx)
            {
                yCandleMinPx = minYpx;
                heightPx = yCandleMaxPx-yCandleMinPx;
            }
            else if(yCandleMaxPx < minYpx)
            {
                yCandleMaxPx=minYpx;
                heightPx = yCandleMaxPx-yCandleMinPx;
            }



            //show allways something, at least one pixel
            if(widthPx<=0)
            {
                widthPx=1;
                if(m_gradienBackground)widthPx++;
            }
            if(heightPx==0)heightPx=1;

            if(xPx < startPx){
                if(xPx+widthPx > startPx)
                {
                    widthPx -= (startPx-xPx);
                    xPx = startPx;
                }
            }

            if (m_drawOutsideMargins || ((xPx >= startPx) && (xPx <= endPx) && (yStickMinPx >= minYpx) && (yStickMaxPx <= maxYpx)))
            {
                if(stickXpx > startPx && stickXpx < endPx)
                {
                    dc.SetPen( m_stickPen );
                    dc.DrawLine(stickXpx, yStickMinPx, stickXpx, yStickMaxPx);
                }

                if( (yCandleMinPx >= minYpx) && (yCandleMaxPx <= maxYpx) && xPx >= startPx)
                {
                    if(!m_gradienBackground)
                    {
                        if( yCandleMinPx < yCandleMaxPx )
                            dc.SetBrush( m_candleBrush );
                        else dc.SetBrush( m_candleBrush2 );

                        dc.SetPen( m_candlePen );
                        dc.DrawRectangle(xPx, yCandleMinPx, widthPx, heightPx);
                    }
                    else{
                        dc.GradientFillLinear( wxRect( xPx, yCandleMinPx, widthPx/2, heightPx),
                                                      wxColour(100,100,150),
                                                      wxColour(150,150,255),
                                                      wxDirection(wxRIGHT) );
                        dc.GradientFillLinear( wxRect( xPx+widthPx/2, yCandleMinPx, widthPx/2, heightPx),
                                                      wxColour(100,100,150),
                                                      wxColour(150,150,255),
                                                      wxDirection(wxLEFT) );
                    }
                }
            }

        }



		if (!m_name.IsEmpty() && m_showName)
		{
			dc.SetFont(m_font);

			wxCoord tx, ty;
			dc.GetTextExtent(m_name, &tx, &ty);

			// xxx implement else ... if (!HasBBox())
			{
				const int sx = w.GetScrX()>>1;
				const int sy = w.GetScrY()>>1;

				if ((m_flags & mpALIGNMASK) == mpALIGN_NE)
				{
					tx = sx - tx - 8;
					ty = -sy + 8;
				}
				else if ((m_flags & mpALIGNMASK) == mpALIGN_NW)
				{
					tx = -sx + 8;
					ty = -sy + 8;
				}
				else if ((m_flags & mpALIGNMASK) == mpALIGN_SW)
				{
					tx = -sx + 8;
					ty = sy - 8 - ty;
				}
				else
				{
					tx = sx - tx - 8;
					ty = sy - 8 - ty;
				}
			}

			dc.DrawText( m_name, tx, ty);
		}
	}
}


IMPLEMENT_ABSTRACT_CLASS(mpFX2Y, mpLayer)

mpFX2Y::mpFX2Y(wxString name, int flags)
{
    SetName(name);
    m_flags = flags;
    m_type = mpLAYER_PLOT;
}
void mpFX2Y::Plot(wxDC & dc, mpWindow & w)
{
	if (m_visible) {
		dc.SetPen( m_pen);

		double x, y1, y2;
		Rewind();

		wxCoord startPx = m_drawOutsideMargins ? 0 : w.GetMarginLeft();
		wxCoord endPx   = m_drawOutsideMargins ? w.GetScrX() : w.GetScrX() - w.GetMarginRight();
		wxCoord minYpx  = m_drawOutsideMargins ? 0 : w.GetMarginTop();
		wxCoord maxYpx  = m_drawOutsideMargins ? w.GetScrY() : w.GetScrY() - w.GetMarginBottom();

		wxCoord ix = 0, iy = 0, iy1 = 0, iy2 = 0;

		if (!m_continuous)
		{
			// for some reason DrawPoint does not use the current pen,
			// so we use DrawLine for fat pens
			if (m_pen.GetWidth() <= 1)
			{
				while (GetNextX2Y(x, y1, y2))
				{
					ix = w.x2p(x);
					iy1 = w.y2p(y1);
					iy2 = w.y2p(y2);
					if(iy1 >= iy2)  iy = iy2 + (iy1-iy2)/2;
					else            iy  = iy1 + (iy2-iy1)/2;

					if (m_drawOutsideMargins || ((ix >= startPx) && (ix <= endPx) && (iy >= minYpx) && (iy <= maxYpx)))
						dc.DrawPoint(ix, iy);

					if (m_drawOutsideMargins || ((ix >= startPx) && (ix <= endPx) && (iy1 >= minYpx) && (iy1 <= maxYpx)))
						dc.DrawPoint(ix, iy1);

					if (m_drawOutsideMargins || ((ix >= startPx) && (ix <= endPx) && (iy2 >= minYpx) && (iy2 <= maxYpx)))
						dc.DrawPoint(ix, iy2);
				}
			}
			else
			{
				while (GetNextX2Y(x, y1, y2))
				{
					ix = w.x2p(x);
					iy1 = w.y2p(y1);
					iy2 = w.y2p(y2);
					if(iy1 >= iy2)  iy = iy2 + (iy1-iy2)/2;
					else            iy = iy1 + (iy2-iy1)/2;

					if (m_drawOutsideMargins || ((ix >= startPx) && (ix <= endPx) && (iy >= minYpx) && (iy <= maxYpx)))
						dc.DrawLine(ix, iy, ix, iy);

					if (m_drawOutsideMargins || ((ix >= startPx) && (ix <= endPx) && (iy1 >= minYpx) && (iy1 <= maxYpx)))
						dc.DrawLine(ix, iy1, ix, iy1);

					if (m_drawOutsideMargins || ((ix >= startPx) && (ix <= endPx) && (iy2 >= minYpx) && (iy2 <= maxYpx)))
						dc.DrawLine(ix, iy2, ix, iy2);
				}
			}
		}
		else
		{
			wxCoord cx0=0,cya0=0, cyb0=0, cy0=0;
			wxCoord cx=0, cy1=0, cy2=0, cy=0;
			bool    first = TRUE;
			while (GetNextX2Y(x, y1, y2))
			{
				cx = w.x2p(x); // (wxCoord) ((x - w.GetPosX()) * w.GetScaleX());
				cy1 = w.y2p(y1); // (wxCoord) ((w.GetPosY() - y) * w.GetScaleY());
				cy2 = w.y2p(y2); // (wxCoord) ((w.GetPosY() - y) * w.GetScaleY());

				if(cy1 >= cy2)  cy = cy2 + (cy1-cy2)/2;
                else            cy = cy1 + (cy2-cy1)/2;


				if (first)
				{
					cx0=cx;
					cy0 = cy;
					cya0=cy1;
					cyb0=cy2;

					first=FALSE;
				}
				dc.DrawLine(cx0, cya0, cx, cy1);
				dc.DrawLine(cx0, cyb0, cx, cy2);
				dc.DrawLine(cx0, cy0, cx, cy);

				cx0=cx;
				cy0=cy;
				cya0=cy1;
				cyb0=cy2;
			}
		}

		if (!m_name.IsEmpty() && m_showName)
		{
			dc.SetFont(m_font);

			wxCoord tx, ty;
			dc.GetTextExtent(m_name, &tx, &ty);

			// xxx implement else ... if (!HasBBox())
			{
				const int sx = w.GetScrX()>>1;
				const int sy = w.GetScrY()>>1;

				if ((m_flags & mpALIGNMASK) == mpALIGN_NE)
				{
					tx = sx - tx - 8;
					ty = -sy + 8;
				}
				else if ((m_flags & mpALIGNMASK) == mpALIGN_NW)
				{
					tx = -sx + 8;
					ty = -sy + 8;
				}
				else if ((m_flags & mpALIGNMASK) == mpALIGN_SW)
				{
					tx = -sx + 8;
					ty = sy - 8 - ty;
				}
				else
				{
					tx = sx - tx - 8;
					ty = sy - 8 - ty;
				}
			}

			dc.DrawText( m_name, tx, ty);
		}
	}
}

IMPLEMENT_ABSTRACT_CLASS(mpFX, mpLayer)

mpFX::mpFX(wxString name, int flags)
{
    SetName(name);
    m_flags = flags;
    m_type = mpLAYER_PLOT;
    m_markCorners = false;
    m_drawOutsideMargins = false;
    m_cornerMarkSize = 3;
}

void mpFX::Plot(wxDC & dc, mpWindow & w)
{
    if (m_visible) {
		dc.SetPen( m_pen);

		wxCoord startPx = m_drawOutsideMargins ? 0 : w.GetMarginLeft();
		wxCoord endPx   = m_drawOutsideMargins ? w.GetScrX() : w.GetScrX() - w.GetMarginRight();
		wxCoord minYpx  = m_drawOutsideMargins ? 0 : w.GetMarginTop();
		wxCoord maxYpx  = m_drawOutsideMargins ? w.GetScrY() : w.GetScrY() - w.GetMarginBottom();

		wxCoord iy = 0;
		if (m_pen.GetWidth() <= 1)
		{
			for (wxCoord i = startPx; i < endPx; ++i)
			{
				iy = w.y2p( GetY(w.p2x(i)));
				// Draw the point only if you can draw outside margins or if the point is inside margins
				if (m_drawOutsideMargins || ((iy >= minYpx) && (iy <= maxYpx)))
                {
                    dc.DrawPoint(i, iy );// (wxCoord) ((w.GetPosY() - GetY( (double)i / w.GetScaleX() + w.GetPosX()) ) * w.GetScaleY()));
					//dc.DrawLine( i, iy, i, iy);
					if(m_markCorners)
                    {
                        dc.DrawCircle(i, iy, m_cornerMarkSize);
                    }
                }

			}
		}
		else
		{
			for (wxCoord i = startPx; i < endPx; ++i)
			{
				iy = w.y2p( GetY(w.p2x(i)));
				// Draw the point only if you can draw outside margins or if the point is inside margins
				if (m_drawOutsideMargins || ((iy >= minYpx) && (iy <= maxYpx)))
                {
                    dc.DrawPoint(i, iy );
                    //dc.DrawLine( i, iy, i, iy);
					if(m_markCorners)
                    {
                        dc.DrawCircle(i, iy, m_cornerMarkSize);
                    }
                }

	//             wxCoord c = w.y2p( GetY(w.p2x(i)) ); //(wxCoord) ((w.GetPosY() - GetY( (double)i / w.GetScaleX() + w.GetPosX()) ) * w.GetScaleY());

			}
		}

		if (!m_name.IsEmpty() && m_showName)
		{
			dc.SetFont(m_font);

			wxCoord tx, ty;
			dc.GetTextExtent(m_name, &tx, &ty);

			/*if ((m_flags & mpALIGNMASK) == mpALIGN_RIGHT)
				tx = (w.GetScrX()>>1) - tx - 8;
			else if ((m_flags & mpALIGNMASK) == mpALIGN_CENTER)
				tx = -tx/2;
			else
				tx = -(w.GetScrX()>>1) + 8;
			*/
			if ((m_flags & mpALIGNMASK) == mpALIGN_RIGHT)
				tx = (w.GetScrX() - tx) - w.GetMarginRight() - 8;
			else if ((m_flags & mpALIGNMASK) == mpALIGN_CENTER)
				tx = ((w.GetScrX() - w.GetMarginRight() - w.GetMarginLeft() - tx) / 2) + w.GetMarginLeft();
			else
				tx = w.GetMarginLeft() + 8;
			dc.DrawText( m_name, tx, w.y2p(GetY(w.p2x(tx))) ); // (wxCoord) ((w.GetPosY() - GetY( (double)tx / w.GetScaleX() + w.GetPosX())) * w.GetScaleY()) );
		}
	}
}
void mpFX::MarkCorners(bool t){m_markCorners = t;}

IMPLEMENT_ABSTRACT_CLASS(mpFY, mpLayer)

mpFY::mpFY(wxString name, int flags)
{
    SetName(name);
    m_flags = flags;
    m_type = mpLAYER_PLOT;
}

void mpFY::Plot(wxDC & dc, mpWindow & w)
{
	if (m_visible) {
		dc.SetPen( m_pen);

		wxCoord i, ix;

		wxCoord startPx = m_drawOutsideMargins ? 0 : w.GetMarginLeft();
		wxCoord endPx   = m_drawOutsideMargins ? w.GetScrX() : w.GetScrX() - w.GetMarginRight();
		wxCoord minYpx  = m_drawOutsideMargins ? 0 : w.GetMarginTop();
		wxCoord maxYpx  = m_drawOutsideMargins ? w.GetScrY() : w.GetScrY() - w.GetMarginBottom();

		if (m_pen.GetWidth() <= 1)
		{
			for (i = minYpx; i < maxYpx; ++i)
			{
				ix = w.x2p(GetX(w.p2y(i)));
				if (m_drawOutsideMargins || ((ix >= startPx) && (ix <= endPx)))
                {
                    dc.DrawPoint(ix, i);
                }

			}
		}
		else
		{
			for (i=0;i< w.GetScrY(); ++i)
			{
				ix = w.x2p(GetX(w.p2y(i)));
				if (m_drawOutsideMargins || ((ix >= startPx) && (ix <= endPx)))
                {
                    //dc.DrawLine(ix, i, ix, i);
					dc.DrawPoint(ix, i);
                }
	//             wxCoord c =  w.x2p(GetX(w.p2y(i))); //(wxCoord) ((GetX( (double)i / w.GetScaleY() + w.GetPosY()) - w.GetPosX()) * w.GetScaleX());
	//             dc.DrawLine(c, i, c, i);
			}
		}

		if (!m_name.IsEmpty() && m_showName)
		{
			dc.SetFont(m_font);

			wxCoord tx, ty;
			dc.GetTextExtent(m_name, &tx, &ty);

			if ((m_flags & mpALIGNMASK) == mpALIGN_TOP)
				ty = w.GetMarginTop() + 8;
			else if ((m_flags & mpALIGNMASK) == mpALIGN_CENTER)
				ty = ((w.GetScrY() - w.GetMarginTop() - w.GetMarginBottom() - ty) / 2) + w.GetMarginTop();
			else
				ty = w.GetScrY() - 8 - ty - w.GetMarginBottom();

			dc.DrawText( m_name, w.x2p(GetX(w.p2y(ty))), ty ); // (wxCoord) ((GetX( (double)i / w.GetScaleY() + w.GetPosY()) - w.GetPosX()) * w.GetScaleX()), -ty);
		}
	}
}

IMPLEMENT_ABSTRACT_CLASS(mpFXY, mpLayer)

mpFXY::mpFXY(wxString name, int flags)
{
    SetName(name);
    m_flags = flags;
    m_type = mpLAYER_PLOT;
    m_markCorners = false;
    m_drawOutsideMargins = false;
    m_cornerMarkSize = 3;
}
void mpFXY::UpdateViewBoundary(wxCoord xnew, wxCoord ynew)
{
	// Keep track of how many points have been drawn and the bouding box
	maxDrawX = (xnew > maxDrawX) ? xnew : maxDrawX;
	minDrawX = (xnew < minDrawX) ? xnew : minDrawX;
	maxDrawY = (maxDrawY > ynew) ? maxDrawY : ynew;
	minDrawY = (minDrawY < ynew) ? minDrawY : ynew;
	//drawnPoints++;
}
bool mpFXY::GetNearestCoord(mpWindow & w, double & x, double & y)
{
    double x_=0, y_=0, x__=0, y__=0;
    bool oldInside=false;
    int ix, iy;
    wxCoord startPx = m_drawOutsideMargins ? 0 : w.GetMarginLeft();
    wxCoord endPx   = m_drawOutsideMargins ? w.GetScrX() : w.GetScrX() - w.GetMarginRight();
    wxCoord minYpx  = m_drawOutsideMargins ? 0 : w.GetMarginTop();
    wxCoord maxYpx  = m_drawOutsideMargins ? w.GetScrY() : w.GetScrY() - w.GetMarginBottom();

    Rewind();
    while (GetNextXY(x_, y_))
    {
        ix = w.x2p(x);
        iy = w.y2p(y);
        if (m_drawOutsideMargins || ((ix >= startPx) && (ix <= endPx) && (iy >= minYpx) && (iy <= maxYpx)))
        {
            //value must be inside
            if(x_>=x)
            {
                if(x_ - x <= x - x__)
                {
                    x = x_;
                    y = y_;
                    return true;
                }
                else if(oldInside)
                {
                    x = x__;
                    y = y__;
                    return true;
                }

            }
            oldInside=true;
        }
        else oldInside=false;
        x__ = x_;
        y__ = y_;

    }
    return false;
}
void mpFXY::MarkCorners(bool t)
{
    m_markCorners = t;
}
void mpFXY::Plot(wxDC & dc, mpWindow & w)
{
	if (m_visible) {
		dc.SetPen( m_pen);
		dc.SetBrush( m_brush );

		double x, y;
		// Do this to reset the counters to evaluate bounding box for label positioning
		//Rewind(); GetNextXY(x, y);

		///@bug  Why insert double values to wxCoord..?
		//maxDrawX = (wxCoord)x; minDrawX = (wxCoord)x; maxDrawY = (wxCoord)y; minDrawY = (wxCoord)y;
		//drawnPoints = 0;


		wxCoord startPx = m_drawOutsideMargins ? 0 : w.GetMarginLeft();
		wxCoord endPx   = m_drawOutsideMargins ? w.GetScrX() : w.GetScrX() - w.GetMarginRight();
		wxCoord minYpx  = m_drawOutsideMargins ? 0 : w.GetMarginTop();
		wxCoord maxYpx  = m_drawOutsideMargins ? w.GetScrY() : w.GetScrY() - w.GetMarginBottom();

		wxCoord ix = 0, iy = 0;

        Rewind();
        CurrentBounds( w.p2x(startPx), w.p2x(endPx) );

		if (!m_continuous)
		{
			// for some reason DrawPoint does not use the current pen,
			// so we use DrawLine for fat pens
			if (m_pen.GetWidth() <= 1)
			{
				while (GetNextXY(x, y))
				{
					ix = w.x2p(x);
					iy = w.y2p(y);
					if (m_drawOutsideMargins || ((ix >= startPx) && (ix <= endPx) && (iy >= minYpx) && (iy <= maxYpx)))
					{
						switch(m_pointType)
						{	//
							case mpCIRCLE:
								dc.DrawCircle(ix,iy,2);
								break;
							case mpRECT:
								dc.DrawRectangle(ix,iy,m_RectSize.x,m_RectSize.y);
								break;
							case mpPOINT:
								dc.DrawPoint(ix, iy);
								break;
                            default: break;
						}
 						UpdateViewBoundary(ix, iy);
 						if(m_markCorners)
                        {
                            dc.DrawCircle(ix, iy, m_cornerMarkSize);
                        }
                    }
				}
			}
			else
			{
				while (GetNextXY(x, y))
				{
					ix = w.x2p(x);
					iy = w.y2p(y);
					if (m_drawOutsideMargins || ((ix >= startPx) && (ix <= endPx) && (iy >= minYpx) && (iy <= maxYpx)))
					{
						switch(m_pointType)
						{	//
							case mpCIRCLE:
							{
								dc.DrawCircle(ix,iy,2);
								break;
							}
							case mpRECT:
							{
								dc.DrawRectangle(ix,iy,m_RectSize.x,m_RectSize.y);
								break;
							}
							case mpPOINT:
							{
								dc.DrawLine(ix, iy, ix, iy);
								break;
							}
							default: break;
						}

						if(m_markCorners)
                        {
                            dc.DrawCircle(ix, iy, m_cornerMarkSize);
                        }
					}
                }
			}
		}
		else
		{
			// Old code
			wxCoord x0=0,c0=0,
                    *nextX0=0,
                    *nextY0=0;
			bool    first = TRUE;
			while (GetNextXY(x, y))
			{
				wxCoord x1 = w.x2p(x); // (wxCoord) ((x - w.GetPosX()) * w.GetScaleX());
				wxCoord c1 = w.y2p(y); // (wxCoord) ((w.GetPosY() - y) * w.GetScaleY());
				if (first)
				{
					first=FALSE;
					x0=x1;c0=c1;
				}
				bool outUp, outDown;
				if( (x1 >= startPx) && (x0 <= endPx)) {
					outDown = (c0 > maxYpx) && (c1 > maxYpx);   // check that at least one of point is inside borders
					outUp = (c0 < minYpx) && (c1 < minYpx);
					if (!outUp && !outDown) {


                        // this could comment in more detail..
                        /// @bug There is some bugs!
                        if (c1 != c0) {
                            if (c0 < minYpx) { //y0 < minY
                                x0 = (int)(((double)(minYpx - c0))/((double)(c1 - c0))*(x1-x0)) + x0;
                                c0 = minYpx;
                            }
                            if (c0 > maxYpx) { //y0 > maxY
                                x0 = (int)(((double)(maxYpx - c0))/((double)(c1 - c0))*(double)(x1-x0)) + x0;
                                //wxLogDebug(wxT("old x0 = %d, new x0 = %d"), x0, newX0);
                                c0 = maxYpx;
                            }
                            if (c1 < minYpx) {  //y1 > minY
                                nextX0 = new wxCoord(x1);   //Save x1 value because it needed for next loop!
                                nextY0 = new wxCoord(c1);   //Save y1 value because it needed for next loop!
                                x1 = (int)(((double)(minYpx - c0))/((double)(c1 - c0))*(x1-x0)) + x0;
                                c1 = minYpx;
                            }
                            if (c1 > maxYpx) { //y1 > maxY
                                nextX0 = new wxCoord(x1);   //Save x1 value because it needed for next loop!
                                nextY0 = new wxCoord(c1);   //Save y1 value because it needed for next loop!

                                x1 = (int)(((double)(maxYpx - c0))/((double)(c1 - c0))*(x1-x0)) + x0;
                                c1 = maxYpx;
                            }

                        }

                        //fix left and right boundary in x axis
                        if (x1 != x0) {
                            if (x0 < startPx) {
                                c0 = (int)(((double)(startPx - x0))/((double)(x1 -x0))*(c1 -c0)) + c0;
                                x0 = startPx;
                            }
                            if (x1 > endPx) {
                                c1 = (int)(((double)(endPx - x0))/((double)(x1 -x0))*(c1 -c0)) + c0;
                                x1 = endPx;
                            }
                        }

						dc.DrawLine(x0, c0, x1, c1);
						UpdateViewBoundary(x1, c1);

						if(m_markCorners)
						{
                            dc.DrawCircle(x1, c1, m_cornerMarkSize);
						}
					}
				}
				x0=x1; c0=c1;
				if(nextX0){x0 = *nextX0; wxDELETE(nextX0);}
				if(nextY0){c0 = *nextY0; wxDELETE(nextY0);}
			}
		}

		if (!m_name.IsEmpty() && m_showName)
		{
			dc.SetFont(m_font);

			wxCoord tx, ty;
			dc.GetTextExtent(m_name, &tx, &ty);

			// xxx implement else ... if (!HasBBox())
			{
				// const int sx = w.GetScrX();
				// const int sy = w.GetScrY();

				if ((m_flags & mpALIGNMASK) == mpALIGN_NW)
				{
					tx = minDrawX + 8;
					ty = maxDrawY + 8;
				}
				else if ((m_flags & mpALIGNMASK) == mpALIGN_NE)
				{
					tx = maxDrawX - tx - 8;
					ty = maxDrawY + 8;
				}
				else if ((m_flags & mpALIGNMASK) == mpALIGN_SE)
				{
					tx = maxDrawX - tx - 8;
					ty = minDrawY - ty - 8;
				}
				else
				{ // mpALIGN_SW
					tx = minDrawX + 8;
					ty = minDrawY - ty - 8;
				}
			}

			dc.DrawText( m_name, tx, ty);
		}
	}
}

//-----------------------------------------------------------------------------
// mpProfile implementation
//-----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(mpProfile, mpLayer)

mpProfile::mpProfile(wxString name, int flags)
{
    SetName(name);
    m_flags = flags;
    m_type = mpLAYER_PLOT;
}

void mpProfile::Plot(wxDC & dc, mpWindow & w)
{
	if (m_visible) {
        dc.SetPen( m_pen);

		wxCoord startPx = m_drawOutsideMargins ? 0 : w.GetMarginLeft();
		wxCoord endPx   = m_drawOutsideMargins ? w.GetScrX() : w.GetScrX() - w.GetMarginRight();
		wxCoord minYpx  = m_drawOutsideMargins ? 0 : w.GetMarginTop();
		wxCoord maxYpx  = m_drawOutsideMargins ? w.GetScrY() : w.GetScrY() - w.GetMarginBottom();

        // Plot profile linking subsequent point of the profile, instead of mpFY, which plots simple points.
        for (wxCoord i = startPx; i < endPx; ++i)
        {
            wxCoord c0 = w.y2p( GetY(w.p2x(i)) ); // (wxCoord) ((w.GetYpos() - GetY( (double)i / w.GetXscl() + w.GetXpos()) ) * w.GetYscl());
            wxCoord c1 = w.y2p( GetY(w.p2x(i+1)) );//(wxCoord) ((w.GetYpos() - GetY( (double)(i+1) / w.GetXscl() + (w.GetXpos() ) ) ) * w.GetYscl());
                // c0 = (c0 <= maxYpx) ? ((c0 >= minYpx) ? c0 : minYpx) : maxYpx;
                // c1 = (c1 <= maxYpx) ? ((c1 >= minYpx) ? c1 : minYpx) : maxYpx;
            if (!m_drawOutsideMargins) {
                    c0 = (c0 <= maxYpx) ? ((c0 >= minYpx) ? c0 : minYpx) : maxYpx;
                    c1 = (c1 <= maxYpx) ? ((c1 >= minYpx) ? c1 : minYpx) : maxYpx;
            }
            dc.DrawLine(i, c0, i+1, c1);
		};
		if (!m_name.IsEmpty())
		{
			dc.SetFont(m_font);

			wxCoord tx, ty;
			dc.GetTextExtent(m_name, &tx, &ty);

			if ((m_flags & mpALIGNMASK) == mpALIGN_RIGHT)
				tx = (w.GetScrX() - tx) - w.GetMarginRight() - 8;
			else if ((m_flags & mpALIGNMASK) == mpALIGN_CENTER)
				tx = ((w.GetScrX() - w.GetMarginRight() - w.GetMarginLeft() - tx) / 2) + w.GetMarginLeft();
			else
				tx = w.GetMarginLeft() + 8;

			dc.DrawText( m_name, tx, w.y2p( GetY( w.p2x(tx) ) ) );//(wxCoord) ((w.GetPosY() - GetY( (double)tx / w.GetScaleX() + w.GetPosX())) * w.GetScaleY()) );
		}
	}
}

//-----------------------------------------------------------------------------
// mpLayer implementations - furniture (scales, ...)
//-----------------------------------------------------------------------------

#define mpLN10 2.3025850929940456840179914546844

IMPLEMENT_DYNAMIC_CLASS(mpScaleX, mpLayer)

mpScaleX::mpScaleX(wxString name, int flags, bool ticks, unsigned int type)
{
    SetName(name);
    SetFont( (wxFont&) *wxSMALL_FONT);
    SetPen( (wxPen&) *wxGREY_PEN);
    m_pTicksLabelColour= wxColour(*wxBLACK);
    m_flags = flags;
    m_ticks = ticks;
    m_labelType = type;
    m_type = mpLAYER_AXIS;
	m_labelFormat = wxT("");
}
bool mpScaleX::Inside(const wxPoint& point)
{
    return m_labelRect.Contains( point );
}
void mpScaleX::Plot(wxDC & dc, mpWindow & w)
{
	if (m_visible) {
		dc.SetPen( m_pen);
		dc.SetFont( m_font );
		dc.SetTextForeground( m_pTicksLabelColour );
		int orgy=0;

		const int extend = w.GetScrX(); //  /2;
		if (m_flags == mpALIGN_CENTER)
		orgy   = w.y2p(0); //(int)(w.GetPosY() * w.GetScaleY());
		if (m_flags == mpALIGN_TOP) {
			if (m_drawOutsideMargins)
				orgy = X_BORDER_SEPARATION;
			else
				orgy = w.GetMarginTop();
		}
		if (m_flags == mpALIGN_BOTTOM) {
			if (m_drawOutsideMargins)
				orgy = X_BORDER_SEPARATION;
			else
				orgy = w.GetScrY() - w.GetMarginBottom();
		}
		if (m_flags == mpALIGN_BORDER_BOTTOM )
		orgy = w.GetScrY() - 1;//dc.LogicalToDeviceY(0) - 1;
		if (m_flags == mpALIGN_BORDER_TOP )
		orgy = 1;//-dc.LogicalToDeviceY(0);

		dc.DrawLine( 0, orgy, w.GetScrX(), orgy);

		// To cut the axis line when draw outside margin is false, use this code
        /*if (m_drawOutsideMargins == true)
            dc.DrawLine( 0, orgy, w.GetScrX(), orgy);
        else
            dc.DrawLine( w.GetMarginLeft(), orgy, w.GetScrX() - w.GetMarginRight(), orgy); */

		const double dig  = floor( log( 128.0 / w.GetScaleX() ) / mpLN10 );
		//const double step = exp( mpLN10 * dig);
		double step = exp( mpLN10 * dig );	//LOGtest
		//const double end  = w.GetPosX() + (double)extend / w.GetScaleX();
		double end = w.GetPosX() + (double)extend / w.GetScaleX();	//LOGtest

		if( m_labelType == mpX_LOGARITHMIC ) {		//LOGtest
			step = 1;
			end = 10*(w.GetPosX() + (double)extend / w.GetScaleX());
		}

		wxCoord tx, ty;
		wxString s;
		wxString fmt;
		int tmp = (int)dig;
		if (m_labelType == mpX_NORMAL) {
			if (!m_labelFormat.IsEmpty()) {
				fmt = m_labelFormat;
			} else {
				if (tmp>=1) {
					fmt = wxT("%.f");
				} else {
					tmp=8-tmp;
					fmt.Printf(wxT("%%.%df"), tmp >= -1 ? 2 : -tmp);
				}
			}
		}
		//##########LOGtest######################
		else if( m_labelType == mpX_LOGARITHMIC ) {
			if( !m_labelFormat.IsEmpty() ) {
				fmt = m_labelFormat;
			} else {
				fmt = wxT("10^%.f");
			}
		}
		//#######################################
		else if( m_labelType == mpX_DATETIME || m_labelType == mpX_DATE || m_labelType == mpX_TIME ) {		//Added: " if( m_labelType == mpX_DATETIME || mpX_DATE || mpX_TIME )"
			// Date and/or time axis representation
			if (m_labelType == mpX_DATETIME) {
				fmt = (wxT("%04.0f-%02.0f-%02.0fT%02.0f:%02.0f:%02.0f"));
			} else if (m_labelType == mpX_DATE) {
				fmt = (wxT("%04.0f-%02.0f-%02.0f"));
			} else if ((m_labelType == mpX_TIME) && (end/60 < 2)) {
				fmt = (wxT("%02.0f:%02.3f"));
			} else {
				fmt = (wxT("%02.0f:%02.0f:%02.0f"));
			}
		}

		//double n = floor( (w.GetPosX() - (double)extend / w.GetScaleX()) / step ) * step ;
		double n0 = floor( (w.GetPosX() /* - (double)(extend - w.GetMarginLeft() - w.GetMarginRight())/ w.GetScaleX() */) / step ) * step ;
		double n = 0;
#ifdef MATHPLOT_DO_LOGGING
		wxLogMessage(wxT("mpScaleX::Plot: dig: %f , step: %f, end: %f, n: %f"), dig, step, end, n0);
#endif
		wxCoord startPx = m_drawOutsideMargins ? 0 : w.GetMarginLeft();
		wxCoord endPx   = m_drawOutsideMargins ? w.GetScrX() : w.GetScrX() - w.GetMarginRight();
		wxCoord minYpx  = m_drawOutsideMargins ? 0 : w.GetMarginTop();
		wxCoord maxYpx  = m_drawOutsideMargins ? w.GetScrY() : w.GetScrY() - w.GetMarginBottom();

		tmp=-65535;
		int labelH = 0; // Control labels heigth to decide where to put axis name (below labels or on top of axis)
		int maxExtent = 0;
		double nx = 1;	//LOGtest
		int dek = 0, p, px;	//LOGtest
		for (n = n0; n < end; n += step) {													//step size between axis ticks
			//for( n=n0; n<=end; n+=step) {		//LOGtest
			//const int p = (int)((n - w.GetPosX()) * w.GetScaleX());
			if(	m_labelType == mpX_LOGARITHMIC )	//LOGtest
			{
				if( dek == 0 ) p = (int)((log10(nx) - w.GetPosX()) * w.GetScaleX());	//LOGtest //w.GetPosX() defines  PlotStartpoint
				else p = px + (int)((log10(nx)/* - w.GetPosX()*/) * w.GetScaleX());		//LOGtest
				if( nx==10 )
				{
					nx=1;
					dek++;
					px=p;
				}			//LOGtest
			nx += 1;	//LOGtest
			}
			else p = (int)((n - w.GetPosX()) * w.GetScaleX());	//LOGtest

#ifdef MATHPLOT_DO_LOGGING
		wxLogMessage(wxT("mpScaleX::Plot: n: %f -> p = %d"), n, p);
#endif
			if ((p >= startPx) && (p <= endPx)) {
				if (m_ticks) { // draw axis ticks
					if (m_flags == mpALIGN_BORDER_BOTTOM)
						dc.DrawLine( p, orgy, p, orgy-4);
					else
						dc.DrawLine( p, orgy, p, orgy+4);
				} else { // draw grid dotted lines
					m_pen.SetStyle(wxPENSTYLE_DOT);
					dc.SetPen(m_pen);
					if ((m_flags == mpALIGN_BOTTOM) && !m_drawOutsideMargins) {
						dc.DrawLine( p, orgy+4, p, minYpx );
					} else {
						if ((m_flags == mpALIGN_TOP) && !m_drawOutsideMargins) {
							dc.DrawLine( p, orgy-4, p, maxYpx );
						} else {
							dc.DrawLine( p, 0/*-w.GetScrY()*/, p, w.GetScrY() );
						}
					}
					m_pen.SetStyle(wxPENSTYLE_SOLID);
					dc.SetPen(m_pen);
				}
				// Write ticks labels in s string
				if (m_labelType == mpX_NORMAL)
					s.Printf(fmt, n);
				//##############LOGtest####################
				else if ( m_labelType == mpX_LOGARITHMIC )
					s.Printf( fmt, n);
				//#########################################
				else if (m_labelType == mpX_DATETIME) {
					time_t when = (time_t)n;
					struct tm tm = *localtime(&when);
					s.Printf(fmt, (double)tm.tm_year+1900, (double)tm.tm_mon+1, (double)tm.tm_mday, (double)tm.tm_hour, (double)tm.tm_min, (double)tm.tm_sec);
				} else if (m_labelType == mpX_DATE) {
					time_t when = (time_t)n;
					struct tm tm = *localtime(&when);
					s.Printf(fmt, (double)tm.tm_year+1900, (double)tm.tm_mon+1, (double)tm.tm_mday);
				} else if ((m_labelType == mpX_TIME) || (m_labelType == mpX_HOURS)) {
					double modulus = fabs(n);
					double sign = n/modulus;
					double hh = floor(modulus/3600);
					double mm = floor((modulus - hh*3600)/60);
					double ss = modulus - hh*3600 - mm*60;
	#ifdef MATHPLOT_DO_LOGGING
					wxLogMessage(wxT("%02.0f Hours, %02.0f minutes, %02.0f seconds"), sign*hh, mm, ss);
	#endif // MATHPLOT_DO_LOGGING
					if (fmt.Len() == 20) // Format with hours has 11 chars
						s.Printf(fmt, sign*hh, mm, floor(ss));
					else
						s.Printf(fmt, sign*mm, ss);
				}
				dc.GetTextExtent(s, &tx, &ty);
				labelH = (labelH <= ty) ? ty : labelH;
/*				if ((p-tx/2-tmp) > 64) { // Problem about non-regular axis labels
					if ((m_flags == mpALIGN_BORDER_BOTTOM) || (m_flags == mpALIGN_TOP)) {
						dc.DrawText( s, p-tx/2, orgy-4-ty);
					} else {
						dc.DrawText( s, p-tx/2, orgy+4);
					}
					tmp=p+tx/2;
				}
				*/
				maxExtent = (tx > maxExtent) ? tx : maxExtent; // Keep in mind max label width
			}
		}
		// Actually draw labels, taking care of not overlapping them, and distributing them regularly
		double labelStep = ceil((maxExtent + mpMIN_X_AXIS_LABEL_SEPARATION)/(w.GetScaleX()*step))*step;
		for (n = n0; n < end; n += labelStep) {
			const int p = (int)((n - w.GetPosX()) * w.GetScaleX());
#ifdef MATHPLOT_DO_LOGGING
		wxLogMessage(wxT("mpScaleX::Plot: n_label = %f -> p_label = %d"), n, p);
#endif
			if ((p >= startPx) && (p <= endPx)) {
				// Write ticks labels in s string
				if (m_labelType == mpX_NORMAL)
					s.Printf(fmt, n);
				//##############LOGtest####################
				else if ( m_labelType == mpX_LOGARITHMIC )
					s.Printf( fmt, n );
				//#########################################
				else if (m_labelType == mpX_DATETIME) {
					time_t when = (time_t)n;
					struct tm tm = *localtime(&when);
					s.Printf(fmt, (double)tm.tm_year+1900, (double)tm.tm_mon+1, (double)tm.tm_mday, (double)tm.tm_hour, (double)tm.tm_min, (double)tm.tm_sec);
				} else if (m_labelType == mpX_DATE) {
					time_t when = (time_t)n;
					struct tm tm = *localtime(&when);
					s.Printf(fmt, (double)tm.tm_year+1900, (double)tm.tm_mon+1, (double)tm.tm_mday);
				} else if ((m_labelType == mpX_TIME) || (m_labelType == mpX_HOURS)) {
					double modulus = fabs(n);
					double sign = n/modulus;
					double hh = floor(modulus/3600);
					double mm = floor((modulus - hh*3600)/60);
					double ss = modulus - hh*3600 - mm*60;
	#ifdef MATHPLOT_DO_LOGGING
					wxLogMessage(wxT("%02.0f Hours, %02.0f minutes, %02.0f seconds"), sign*hh, mm, ss);
	#endif // MATHPLOT_DO_LOGGING
					if (fmt.Len() == 20) // Format with hours has 11 chars
						s.Printf(fmt, sign*hh, mm, floor(ss));
					else
						s.Printf(fmt, sign*mm, ss);
				}
				dc.GetTextExtent(s, &tx, &ty);
				if ((m_flags == mpALIGN_BORDER_BOTTOM) || (m_flags == mpALIGN_TOP)) {
					dc.DrawText( s, p-tx/2, orgy-4-ty);
				} else {
					dc.DrawText( s, p-tx/2, orgy+4);
				}
			}
		}

		// Draw axis name
		dc.GetTextExtent(m_name, &tx, &ty);
		m_labelRect.width = tx;
        m_labelRect.height = ty;
		switch (m_flags) {
			case mpALIGN_BORDER_BOTTOM:
				dc.DrawText( m_name, m_labelRect.x=(extend - tx - 4), m_labelRect.y=(orgy - 8 - ty - labelH));
			break;
			case mpALIGN_BOTTOM: {
				if ((!m_drawOutsideMargins) && (w.GetMarginBottom() > (ty + labelH + 8))) {
					dc.DrawText( m_name, m_labelRect.x=((endPx - startPx - tx)>>1), m_labelRect.y=(orgy + 6 + labelH));
				} else {
					dc.DrawText( m_name, m_labelRect.x= (extend - tx - 4), m_labelRect.y=(orgy - 4 - ty));
				}
			} break;
			case mpALIGN_CENTER:
				dc.DrawText( m_name, m_labelRect.x= (extend - tx - 4), m_labelRect.y= (orgy - 4 - ty));
			break;
			case mpALIGN_TOP: {
				if ((!m_drawOutsideMargins) && (w.GetMarginTop() > (ty + labelH + 8))) {
					dc.DrawText( m_name, m_labelRect.x= ((endPx - startPx - tx)>>1), m_labelRect.y= (orgy - 6 - ty - labelH));
				} else {
					dc.DrawText( m_name, m_labelRect.x= (extend - tx - 4), m_labelRect.y= (orgy + 4));
				}
			} break;
			case mpALIGN_BORDER_TOP:
				dc.DrawText( m_name, m_labelRect.x= (extend - tx - 4),m_labelRect.y=  (orgy + 6 + labelH));
			break;
			default:
			break;
		}
	}
/*    if (m_flags != mpALIGN_TOP) {

        if ((m_flags == mpALIGN_BORDER_BOTTOM) || (m_flags == mpALIGN_TOP)) {
            dc.DrawText( m_name, extend - tx - 4, orgy - 4 - (ty*2));
        } else {
            dc.DrawText( m_name, extend - tx - 4, orgy - 4 - ty); //orgy + 4 + ty);
        }
    }; */
}
bool mpScaleX::HasBBox()            { return false; }
bool mpScaleX::IsScaleXLayer()      { return true;  }
void mpScaleX::SetAlign(int align)  { m_flags = align; }
void mpScaleX::SetTicks(bool ticks) { m_ticks = ticks; }
bool mpScaleX::GetTicks()           { return m_ticks; }

IMPLEMENT_DYNAMIC_CLASS(mpScaleY, mpLayer)

mpScaleY::mpScaleY(wxString name, int flags, bool ticks)
{
    SetName(name);
    SetFont( (wxFont&) *wxSMALL_FONT);
    SetPen( (wxPen&) *wxGREY_PEN);
    m_pTicksLabelColour = wxColour(*wxBLACK);
    m_flags = flags;
    m_ticks = ticks;
    m_type = mpLAYER_AXIS;
	m_labelFormat = wxT("");
}
bool mpScaleY::Inside(const wxPoint& point)
{
    return m_labelRect.Contains( point );
}
void mpScaleY::Plot(wxDC & dc, mpWindow & w)
{
	if (m_visible) {
		dc.SetPen( m_pen );
		dc.SetFont( m_font );
        dc.SetTextForeground( m_pTicksLabelColour );
		int orgx=0;
		const int extend = w.GetScrY(); // /2;
		if (m_flags == mpALIGN_CENTER)
			orgx   = w.x2p(0); //(int)(w.GetPosX() * w.GetScaleX());
		if (m_flags == mpALIGN_LEFT) {
			if (m_drawOutsideMargins)
				orgx = Y_BORDER_SEPARATION;
			else
				orgx = w.GetMarginLeft();
		}
		if (m_flags == mpALIGN_RIGHT) {
			if (m_drawOutsideMargins)
				orgx = w.GetScrX() - Y_BORDER_SEPARATION;
			else
				orgx = w.GetScrX() - w.GetMarginRight();
		}
		if (m_flags == mpALIGN_BORDER_RIGHT )
			orgx = w.GetScrX() - 1; //dc.LogicalToDeviceX(0) - 1;
		if (m_flags == mpALIGN_BORDER_LEFT )
			orgx = 1; //-dc.LogicalToDeviceX(0);


        // Draw line
		dc.DrawLine( orgx, 0, orgx, extend);

		// To cut the axis line when draw outside margin is false, use this code
        /* if (m_drawOutsideMargins == true)
		    dc.DrawLine( orgx, 0, orgx, extend);
        else
		    dc.DrawLine( orgx, w.GetMarginTop(), orgx, w.GetScrY() - w.GetMarginBottom()); */

		const double dig  = floor( log( 128.0 / w.GetScaleY() ) / mpLN10 );
		const double step = exp( mpLN10 * dig);
		const double end  = w.GetPosY() + (double)extend / w.GetScaleY();

		wxCoord tx, ty;
		wxString s;
		wxString fmt;
		int tmp = (int)dig;
		double maxScaleAbs = fabs(w.GetDesiredYmax());
		double minScaleAbs = fabs(w.GetDesiredYmin());
		double endscale = (maxScaleAbs > minScaleAbs) ? maxScaleAbs : minScaleAbs;

		if (m_labelFormat.IsEmpty()) {
			if ((endscale < 1e4) && (endscale > 1e-3))
				fmt = wxT("%.2f");
			else if ((endscale < 1e7) && (endscale > 1e-6))
				fmt = wxT("%.0f");
			else
				fmt = wxT("%.1e");
		} else {
			fmt = m_labelFormat;
		}

		/*
		if ((endscale < 1e4) && (endscale > 1e-3))
			fmt = wxT("%.2f");
		else
			fmt = wxT("%.1e");
*/
	/*    if (tmp>=1)
		{*/
		//    fmt = wxT("%7.5g");
	//     }
	//     else
	//     {
	//         tmp=8-tmp;
	//         fmt.Printf(wxT("%%.%dg"), (tmp >= -1) ? 2 : -tmp);
	//     }

		double n = floor( (w.GetPosY() - (double)(extend - w.GetMarginTop() - w.GetMarginBottom())/ w.GetScaleY()) / step ) * step ;

		/* wxCoord startPx = m_drawOutsideMargins ? 0 : w.GetMarginLeft(); */
		wxCoord endPx   = m_drawOutsideMargins ? w.GetScrX() : w.GetScrX() - w.GetMarginRight();
		wxCoord minYpx  = m_drawOutsideMargins ? 0 : w.GetMarginTop();
		wxCoord maxYpx  = m_drawOutsideMargins ? w.GetScrY() : w.GetScrY() - w.GetMarginBottom();

		tmp=65536;
		int labelW = 0;
		// Before staring cycle, calculate label height
		int labelHeigth = 0;
		s.Printf(fmt,n);
		dc.GetTextExtent(s, &tx, &labelHeigth);
		for (;n < end; n += step) {
			const int p = (int)((w.GetPosY() - n) * w.GetScaleY());
            if ((p >= minYpx) && (p <= maxYpx)) {
                if (m_ticks) { // Draw axis ticks
                    if (m_flags == mpALIGN_BORDER_LEFT) {
                        dc.DrawLine( orgx, p, orgx+4, p);
                    } else {
                        dc.DrawLine( orgx-4, p, orgx, p); //( orgx, p, orgx+4, p);
                    }
                } else {
                    m_pen.SetStyle(wxPENSTYLE_DOT);
                    dc.SetPen( m_pen);
                    if ((m_flags == mpALIGN_LEFT) && !m_drawOutsideMargins) {
                        dc.DrawLine( orgx-4, p, endPx, p);
                    } else {
                        if ((m_flags == mpALIGN_RIGHT) && !m_drawOutsideMargins) {
                        dc.DrawLine( minYpx, p, orgx+4, p);
                                    } else {
                        dc.DrawLine( 0/*-w.GetScrX()*/, p, w.GetScrX(), p);
                            }
                    }
                    m_pen.SetStyle(wxPENSTYLE_SOLID);
                    dc.SetPen( m_pen);
                }
                // Print ticks labels
                s.Printf(fmt, std::fabs(n));
				//para inverter
				//s.Printf(fmt, std::fabs(n - maxScaleAbs));
                dc.GetTextExtent(s, &tx, &ty);
    #ifdef MATHPLOT_DO_LOGGING
                if (ty != labelHeigth) wxLogMessage(wxT("mpScaleY::Plot: ty(%f) and labelHeigth(%f) differ!"), ty, labelHeigth);
    #endif
                labelW = (labelW <= tx) ? tx : labelW;
                if ((tmp-p+labelHeigth/2) > mpMIN_Y_AXIS_LABEL_SEPARATION) {
                    if ((m_flags == mpALIGN_BORDER_LEFT) || (m_flags == mpALIGN_RIGHT))
                        dc.DrawText( s, orgx+4, p-ty/2);
                    else
                        dc.DrawText( s, orgx-4-tx, p-ty/2); //( s, orgx+4, p-ty/2);
                    tmp=p-labelHeigth/2;
                }
            }
		}
		// Draw axis name
		dc.GetTextExtent(m_name, &tx, &ty);
		m_labelRect.width = tx;
        m_labelRect.height = ty;

		switch (m_flags)
		{
			case mpALIGN_BORDER_LEFT:
				dc.DrawText( m_name, m_labelRect.x=(labelW + 8), m_labelRect.y=4);
			break;
			case mpALIGN_LEFT: {
				if ((!m_drawOutsideMargins) && (w.GetMarginLeft() > (ty + labelW + 8))) {
					dc.DrawRotatedText( m_name, m_labelRect.x = (orgx - 15 - labelW - ty), m_labelRect.y=((maxYpx - minYpx + tx)>>1), 90);
					m_labelRect.width = ty;
                    m_labelRect.height = tx;
                    m_labelRect.y -= tx;

				} else {
					dc.DrawText( m_name, m_labelRect.x = (orgx + 4), m_labelRect.y=4);
				}
            }   break;
			case mpALIGN_CENTER:
				dc.DrawText( m_name, orgx + 4, 4);
                break;
			case mpALIGN_RIGHT: {
				if ((!m_drawOutsideMargins) && (w.GetMarginRight() > (ty + labelW + 8))) {
					dc.DrawRotatedText( m_name, m_labelRect.x = (orgx + 6 + labelW), m_labelRect.y= ((maxYpx - minYpx + tx)>>1), 90);
					m_labelRect.width = ty;
                    m_labelRect.height = tx;
                    m_labelRect.y -= tx;
				} else {
					dc.DrawText( m_name, m_labelRect.x = (orgx - tx - 4), m_labelRect.y = 4);
				}
			} break;
			case mpALIGN_BORDER_RIGHT:
				dc.DrawText( m_name, m_labelRect.x = (orgx - 6 - tx -labelW), m_labelRect.y = 4);
                break;
			default:
                break;
		}
	}

/*    if (m_flags != mpALIGN_RIGHT) {
	dc.GetTextExtent(m_name, &tx, &ty);
	if (m_flags == mpALIGN_BORDER_LEFT) {
            dc.DrawText( m_name, orgx-tx-4, -extend + ty + 4);
        } else {
            if (m_flags == mpALIGN_BORDER_RIGHT )
                dc.DrawText( m_name, orgx-(tx*2)-4, -extend + ty + 4);
            else
                dc.DrawText( m_name, orgx + 4, -extend + 4);
        }
    }; */
}

//-----------------------------------------------------------------------------
// mpWindow
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(mpWindow, wxWindow)

BEGIN_EVENT_TABLE(mpWindow, wxWindow)
    EVT_PAINT    ( mpWindow::OnPaint)
    EVT_SIZE     ( mpWindow::OnSize)
    EVT_SCROLLWIN_THUMBTRACK(mpWindow::OnScrollThumbTrack)
    EVT_SCROLLWIN_PAGEUP(mpWindow::OnScrollPageUp)
    EVT_SCROLLWIN_PAGEDOWN(mpWindow::OnScrollPageDown)
    EVT_SCROLLWIN_LINEUP(mpWindow::OnScrollLineUp)
    EVT_SCROLLWIN_LINEDOWN(mpWindow::OnScrollLineDown)
    EVT_SCROLLWIN_TOP(mpWindow::OnScrollTop)
    EVT_SCROLLWIN_BOTTOM(mpWindow::OnScrollBottom)

    EVT_MIDDLE_DOWN( mpWindow::OnMouseMiddleDown )
    EVT_MIDDLE_UP( mpWindow::OnMouseMiddleRelease)
    EVT_RIGHT_DOWN( mpWindow::OnMouseRightDown) // JLB
    EVT_RIGHT_UP ( mpWindow::OnMouseRightRelease)
    EVT_MOUSEWHEEL( mpWindow::OnMouseWheel )   // JLB
    EVT_MOTION( mpWindow::OnMouseMove )   // JLB
    EVT_LEFT_DOWN( mpWindow::OnMouseLeftDown)
    EVT_LEFT_UP( mpWindow::OnMouseLeftRelease)
    EVT_LEAVE_WINDOW( mpWindow::OnMouseLeaveWindow)

    EVT_MENU( mpID_CENTER,    mpWindow::OnCenter)
    EVT_MENU( mpID_FIT,       mpWindow::OnFit)
    EVT_MENU( mpID_ZOOM_IN,   mpWindow::OnZoomIn)
    EVT_MENU( mpID_ZOOM_OUT,  mpWindow::OnZoomOut)
    EVT_MENU( mpID_LOCKASPECT,mpWindow::OnLockAspect)
    EVT_MENU( mpID_HELP_MOUSE,mpWindow::OnMouseHelp)

END_EVENT_TABLE()

mpWindow::mpWindow(){}
mpWindow::mpWindow( wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long flag, const wxString& name )
    : wxWindow( parent, id, pos, size, flag, name )
{
    m_scaleX = m_scaleY = 1.0;
    m_posX   = m_posY   = 0;
    m_desiredXmin=m_desiredYmin=0;
    m_desiredXmax=m_desiredYmax=1;
    m_scrX   = m_scrY   = 64; // Fixed from m_scrX = m_scrX = 64;
    m_minX   = m_minY   = 0;
    m_maxX   = m_maxY   = 0;
    m_last_lx= m_last_ly= 0;
    m_buff_bmp = NULL;
    m_enableDoubleBuffer        = FALSE;
    m_enableMouseNavigation     = TRUE;
    m_enableMousePopup          = TRUE;
    m_mouseMovedAfterRightClick = FALSE;
    m_mouseMovedAfterMiddleClickWithCtrl = FALSE;

    m_movingInfoLayer = NULL;
    m_movingPointLayer = NULL;
    // Set margins to 0
    m_marginTop = 0; m_marginRight = 0; m_marginBottom = 0; m_marginLeft = 0;
    //SetMaximumXZoomLevel(1);

    m_lockBoundMinY=false;
    m_lockBoundMaxY=false;
    m_lockBoundMinX=false;
    m_lockBoundMaxX=false;

    /*m_scaleXmin = 0.25;
    m_scaleXmax = 2;
    m_scaleYmin = 0.25;
    m_scaleYmax = 2;
    m_scaleBoundsEnable = true;*/

    OnMouseHelpString = _("Supported Mouse commands:\n \
        - Left button down + Mark area: Rectangular zoom\n \
        - Middle button down + Mark are: Horizontal zoom\n \
        - Right button down + Move: Pan (Move)\n \
        - Wheel: Vertical scroll\n \
        - Wheel + SHIFT: Horizontal scroll\n \
        - Wheel + CTRL: Zoom in/out");
    OnMouseHelpStringTitle =_("Mouse Help");

    m_zoomingRect = NULL;
    m_zoomingHorizontally = true;


    m_lockaspect = FALSE;

    InitPopupMenu();

    m_layers.clear();
    SetBackgroundColour( *wxWHITE );
    m_bgColour = *wxWHITE;
	m_fgColour = *wxBLACK;


    //defaultly gradien background is disabled but colours defined.
    m_gradienBackColour=false;
    m_gradienInitialColour = wxColour(150,150,200);
    m_gradienDestColour = wxColour(255,255,255);
    m_gradienDirect = wxDirection(wxUP);


	 m_mouseZoomMode = mpZOOM_XY;

    m_enableScrollBars = false;
    SetSizeHints(128, 128);

    // J.L.Blanco: Eliminates the "flick" with the double buffer.
    SetBackgroundStyle( wxBG_STYLE_CUSTOM );

    UpdateAll();
}

mpWindow::~mpWindow()
{
	// Free all the layers:
	DelAllLayers( true, false );

    wxDELETE(m_buff_bmp);
}

void mpWindow::InitPopupMenu()
{
    m_popmenu.Append( mpID_CENTER,     MP_POPMENU_CENTER,      MP_POPMENU_CENTER_INFO);
    m_popmenu.Append( mpID_FIT,        MP_POPMENU_FIT,         MP_POPMENU_FIT_INFO);
    m_popmenu.Append( mpID_ZOOM_IN,    MP_POPMENU_ZOOM_IN,     MP_POPMENU_ZOOM_IN_INFO);
    m_popmenu.Append( mpID_ZOOM_OUT,   MP_POPMENU_ZOOM_OUT,    MP_POPMENU_ZOOM_OUT_INFO);
    m_popmenu.AppendCheckItem( mpID_LOCKASPECT, MP_POPMENU_LOCK_ASPECT, MP_POPMENU_LOCK_ASPECT_INFO);
    m_popmenu.Append( mpID_HELP_MOUSE,   MP_POPMENU_HELP,    MP_POPMENU_HELP_INFO);

}

// Mouse handler, for detecting when the user drag with the right button or just "clicks" for the menu
// JLB
void mpWindow::OnMouseRightDown(wxMouseEvent     &event)
{
    m_mouseMovedAfterRightClick = FALSE;
    m_mouseRClick_X = event.GetX();
    m_mouseRClick_Y = event.GetY();
    wxPoint position = event.GetPosition();

    if(m_zoomingRect)wxDELETE(m_zoomingRect);

#ifdef MATHPLOT_DO_LOGGING
    wxLogMessage(_("mpWindow::OnMouseRightDown() X = %d , Y = %d"), event.GetX(), event.GetY());
#endif


    if (!m_enableMouseNavigation) return;

	//SetCursor( *wxCROSS_CURSOR );

}

// Process mouse wheel events
// JLB
void mpWindow::OnMouseWheel( wxMouseEvent &event )
{
    if(m_zoomingRect)wxDELETE(m_zoomingRect);

    if (!m_enableMouseNavigation)
    {
        event.Skip();
        return;
    }

    //GetClientSize( &m_scrX,&m_scrY);

    if (event.m_controlDown)
    {
        wxPoint clickPt( event.GetX(),event.GetY() );
        // CTRL key hold: Zoom in/out:
        if (event.GetWheelRotation()>0){
            ZoomIn( clickPt );
        }
        else {
            ZoomOut( clickPt );
        }
    }
    else
    {
        // Scroll vertically or horizontally (this is SHIFT is hold down).
        int change = - event.GetWheelRotation(); // Opposite direction (More intuitive)!
        double changeUnitsX = change / m_scaleX;
        double changeUnitsY = change / m_scaleY;

        if (event.m_shiftDown)
        {
            m_posX 		    += changeUnitsX;
            m_desiredXmax 	+= changeUnitsX;
            m_desiredXmin 	+= changeUnitsX;

            if(m_lockBoundMaxX && m_desiredXmax > m_maxX){
                m_posX -= changeUnitsX;
                m_desiredXmax -= changeUnitsX;
                m_desiredXmin -= changeUnitsX;
            }
            else if(m_lockBoundMinX && m_desiredXmin < m_minX){
                m_posX -= changeUnitsX;
                m_desiredXmax -= changeUnitsX;
                m_desiredXmin -= changeUnitsX;
            }
        }
        else
        {

            m_posY 	-= changeUnitsY;

            m_desiredYmax	-= changeUnitsY;
            m_desiredYmin	-= changeUnitsY;

            if(m_lockBoundMaxY && m_desiredYmax > m_maxY){
                m_posY += changeUnitsY;
                m_desiredYmax += changeUnitsY;
                m_desiredYmin += changeUnitsY;
            }
            else if(m_lockBoundMinY && m_desiredYmin < m_minY){
                m_posY += changeUnitsY;
                m_desiredYmax += changeUnitsY;
                m_desiredYmin += changeUnitsY;
            }
        }

        UpdateAll();
    }
}

// If the user "drags" with the right buttom pressed, do "pan"
// JLB
void mpWindow::OnMouseMove(wxMouseEvent     &event)
{
    wxLayerList::iterator li;
    //int mouseX = m_mousePosition_X
    //    = event.GetX();
    //int mouseY = m_mousePosition_Y
    //    = event.GetY();

    //allways update point layer coordinates..
	wxCursor mouseCursor(wxCURSOR_ARROW);

    wxPoint pos = event.GetPosition();
    for (li = m_layers.begin(); li != m_layers.end(); li++) {
        if ((*li)->IsPointLayer())
        {
            mpPointLayer *ly = (mpPointLayer*)(*li);
            ly->UpdateMouseCoord(*this, event);

			if( ly->Inside( pos ) )
			{mouseCursor = wxCURSOR_CROSS; break;}

        }
        if((*li)->IsScaleXLayer())
        {
            mpScaleX *ly = dynamic_cast<mpScaleX*>(*li);
            if(ly->Inside(pos) )
            { mouseCursor = wxCURSOR_RIGHT_ARROW;  break;}
        }
        if((*li)->IsScaleYLayer())
        {
            mpScaleY *ly = dynamic_cast<mpScaleY*>(*li);
            if(ly->Inside(pos) )
            { mouseCursor = wxCURSOR_RIGHT_ARROW;  break;}
        }
        if((*li)->IsInfo())
        {
            mpInfoLayer *ly = dynamic_cast<mpInfoLayer*>(*li);
            if(ly->Inside(pos) )
            { mouseCursor = wxCURSOR_CROSS;  break;}
        }
    }
    SetCursor( mouseCursor );


    //If point layer is selected, move it while mouse moves
    if(m_movingPointLayer != NULL && event.m_leftDown)
    {
        m_movingPointLayer->UpdatePoint(*this, event);
        UpdateAll();
        return;
    }

    /*if (!m_enableMouseNavigation)
    {
        event.Skip();
        return;
    }*/

    if (event.m_rightDown && m_enableMouseNavigation)
    {
        m_mouseMovedAfterRightClick = TRUE;  // Hides the popup menu after releasing the button!

        // The change:
        int  Ax= m_mouseRClick_X - event.GetX();
        int  Ay= m_mouseRClick_Y - event.GetY();

        // For the next event, use relative to this coordinates.
        m_mouseRClick_X = event.GetX();
        m_mouseRClick_Y = event.GetY();

        double   Ax_units = Ax / m_scaleX;
        double   Ay_units = -Ay / m_scaleY;

        m_posX += Ax_units;
        m_posY += Ay_units;

        m_desiredXmax 	+= Ax_units;
        m_desiredXmin 	+= Ax_units;

        m_desiredYmax 	+= Ay_units;
        m_desiredYmin 	+= Ay_units;

        if(m_lockBoundMaxX && m_desiredXmax > m_maxX){
            m_posX -= Ax_units;
            m_desiredXmax -= Ax_units;
            m_desiredXmin -= Ax_units;
        }
        else if(m_lockBoundMinX && m_desiredXmin < m_minX){
            m_posX -= Ax_units;
            m_desiredXmax -= Ax_units;
            m_desiredXmin -= Ax_units;
        }
        if(m_lockBoundMaxY && m_desiredYmax > m_maxY){
            m_posY -= Ay_units;
            m_desiredYmax -= Ay_units;
            m_desiredYmin -= Ay_units;
        }
        else if(m_lockBoundMinY && m_desiredYmin < m_minY){
            m_posY -= Ay_units;
            m_desiredYmax -= Ay_units;
            m_desiredYmin -= Ay_units;
        }


        UpdateAll();

    #ifdef MATHPLOT_DO_LOGGING
            wxLogMessage(_("[mpWindow::OnMouseMove] Ax:%i Ay:%i m_posX:%f m_posY:%f"),Ax,Ay,m_posX,m_posY);
    #endif

    }
    else if (event.m_leftDown )
    {
        if (m_movingInfoLayer == NULL && m_enableMouseNavigation && m_mouseDownHasHappened )
        {
            //zooming:
            if(m_zoomingRect==NULL) m_zoomingRect = new wxRect();


            if( m_mouseZoomMode == mpZOOM_XY )
                *m_zoomingRect = wxRect(m_mouseLClick_X, m_mouseLClick_Y, event.GetX() - m_mouseLClick_X, event.GetY() - m_mouseLClick_Y);

            else if(m_mouseZoomMode == mpZOOM_X)
            {
                *m_zoomingRect = wxRect(m_mouseLClick_X,
                                        GetMarginTop(),
                                        event.GetX()-m_mouseLClick_X,
                                        GetScrY() - GetMarginBottom());
            }
        }
        else if(m_movingInfoLayer)
        {
            wxPoint moveVector(event.GetX() - m_mouseLClick_X, event.GetY() - m_mouseLClick_Y);
            if(m_movingInfoLayer)
                m_movingInfoLayer->Move(moveVector);
        }
        UpdateAll();
        //RefreshRect(*m_zoomingRect);
    }
    else if(event.m_middleDown && m_enableMouseNavigation) {

        if( event.m_controlDown ){
            //tape measurement
            m_mouseMovedAfterMiddleClickWithCtrl = TRUE;
            UpdateAll();
            //Refresh( FALSE );
        }
        else if (m_mouseMovedAfterMiddleClickWithCtrl){
            m_mouseMovedAfterMiddleClickWithCtrl = FALSE;
            UpdateAll();
            //Refresh( FALSE );
        }
        else if (!m_mouseMovedAfterMiddleClickWithCtrl) {
            //zooming:
            if(m_zoomingRect==NULL) m_zoomingRect = new wxRect();

            if( abs(event.GetX()-m_mouseMClick_X) > abs(event.GetY() - m_mouseMClick_Y) )
                m_zoomingHorizontally = true;
            else m_zoomingHorizontally = false;

            if(m_zoomingHorizontally){
                *m_zoomingRect = wxRect(m_mouseMClick_X,
                                        GetMarginTop(),
                                        event.GetX()-m_mouseMClick_X,
                                        GetScrY() - GetMarginBottom());
            } else {
                *m_zoomingRect = wxRect(GetMarginLeft(),
                                        m_mouseMClick_Y,
                                        GetScrX() - GetMarginRight(),
                                        event.GetY()-m_mouseMClick_Y);
            }

            //RefreshRect(*m_zoomingRect);
            UpdateAll();
            //Refresh( FALSE );
        }
    }
    else {
        if(m_zoomingRect)wxDELETE(m_zoomingRect);
        //update all info layers
        for (li = m_layers.begin(); li != m_layers.end(); li++) {
            if ((*li)->IsInfo() && (*li)->IsVisible()) {
                mpInfoLayer* tmpLyr = (mpInfoLayer*) (*li);
                tmpLyr->UpdateInfo(*this, event);
                UpdateAll();
                //RefreshRect(tmpLyr->GetRectangle());
            }
        }
        /*
        // if (m_coordTooltip) {
            wxString toolTipContent;
            toolTipContent.Printf(_("X = %f\nY = %f"), p2x(event.GetX()), p2y(event.GetY()));
            wxTipWindow** ptr = NULL;
            wxRect rectBounds(event.GetX(), event.GetY(), 5, 5);
            wxTipWindow* tip = new wxTipWindow(this, toolTipContent, 100, ptr, &rectBounds);

        //} */
    }

    event.Skip();
}

void mpWindow::OnMouseLeftDown (wxMouseEvent &event)
{
    m_mouseDownHasHappened = true;
    m_mouseLClick_X = event.GetX();
    m_mouseLClick_Y = event.GetY();
#ifdef MATHPLOT_DO_LOGGING
    wxLogMessage(_("mpWindow::OnMouseLeftDown() X = %d , Y = %d"), event.GetX(), event.GetY());/*m_mouseLClick_X, m_mouseLClick_Y);*/
#endif
    wxPoint pointClicked = event.GetPosition();
    m_movingInfoLayer = IsInsideInfoLayer(pointClicked);
    if (m_movingInfoLayer != NULL) {
#ifdef MATHPLOT_DO_LOGGING
        wxLogMessage(_("mpWindow::OnMouseLeftDown() started moving layer %lx"), (long int) m_movingInfoLayer);/*m_mouseLClick_X, m_mouseLClick_Y);*/
#endif
		event.Skip(); return;
    }
	m_movingPointLayer = IsInsidePointLayer(pointClicked);
	if (m_movingPointLayer != NULL) {
#ifdef MATHPLOT_DO_LOGGING
		wxLogMessage(_("mpWindow::OnMouseRightDown() started moving layer %lx"), (long int) m_movingPointLayer);/*m_mouseLClick_X, m_mouseLClick_Y);*/
#endif
		this->SetCursor(wxCURSOR_CROSS);
		m_movingPointLayer->UpdatePoint(*this, event);
		event.Skip(); return;
	}
    event.Skip();
}

void mpWindow::OnMouseLeftRelease (wxMouseEvent &event)
{
    wxPoint release(event.GetX(), event.GetY());
    wxPoint press(m_mouseLClick_X, m_mouseLClick_Y);

    if(m_zoomingRect)wxDELETE(m_zoomingRect);

    if (m_movingInfoLayer != NULL) {
        m_movingInfoLayer->UpdateReference();
        m_movingInfoLayer = NULL;
    }else if(m_movingPointLayer!=NULL) {
        //if point layer is enabled, and relase left mouse button, update point
        m_movingPointLayer->UpdatePoint(*this, event);
        UpdateAll();
        m_movingPointLayer = NULL;
        this->SetCursor(wxCURSOR_ARROW);
	}else{

        if( release != press && m_enableMouseNavigation &&
            !event.m_rightDown  //this allows to cancel the zooming
          )
        {
            //only when mouseNavigation is enabled

            if( m_mouseDownHasHappened )
            {
                // only when there has previously been a mouse down event.

                //if mpZOOM_X is selected, Y axis not zooming!
                if( m_mouseZoomMode == mpZOOM_X){
                    press.y     = GetMarginTop();
                    release.y   = GetScrY() - GetMarginBottom();
                }
                if(m_mouseZoomMode == mpZOOM_Y)
                {
                    /// @todo not tested
                    press.x     = GetMarginLeft();
                    release.x   = GetScrX() - GetMarginRight();
                }

                //pressed to avoid injury
                //must be more than 8 pixel different
                if( press.x>>4 != release.x>>4 &&
                    press.y>>4 != release.y>>4 )
                    ZoomRect(press, release);
            }
            else
            {

            }

        } /*else {
            if (m_coordTooltip) {
                wxString toolTipContent;
                toolTipContent.Printf(_("X = %f\nY = %f"), p2x(event.GetX()), p2y(event.GetY()));
                SetToolTip(toolTipContent);
            }
        }*/
    }
    m_mouseDownHasHappened = false;
    event.Skip();
}

void mpWindow::Fit()
{
	if (UpdateBBox())
		Fit(m_minX,m_maxX,m_minY,m_maxY );
}


// JL
void mpWindow::Fit(double xMin, double xMax, double yMin, double yMax, wxCoord *printSizeX,wxCoord *printSizeY)
{
	// Save desired borders:
	m_desiredXmin=xMin; m_desiredXmax=xMax;
	m_desiredYmin=yMin; m_desiredYmax=yMax;

	if (printSizeX!=NULL && printSizeY!=NULL)
	{
		// Printer:
		m_scrX = *printSizeX;
		m_scrY = *printSizeY;
	}
	else
	{
		// Normal case (screen):
		GetClientSize( &m_scrX,&m_scrY);
	}

	double Ax,Ay;

	Ax = xMax - xMin;
	Ay = yMax - yMin;

	m_scaleX = (Ax!=0) ? (m_scrX - m_marginLeft - m_marginRight)/Ax : 1; //m_scaleX = (Ax!=0) ? m_scrX/Ax : 1;
	m_scaleY = (Ay!=0) ? (m_scrY - m_marginTop - m_marginBottom)/Ay : 1; //m_scaleY = (Ay!=0) ? m_scrY/Ay : 1;

	if (m_lockaspect)
	{
#ifdef MATHPLOT_DO_LOGGING
	wxLogMessage(_("mpWindow::Fit()(lock) m_scaleX=%f,m_scaleY=%f"), m_scaleX,m_scaleY);
#endif
		// Keep the lowest "scale" to fit the whole range required by that axis (to actually "fit"!):
		double s = m_scaleX < m_scaleY ? m_scaleX : m_scaleY;
		m_scaleX = s;
		m_scaleY = s;
	}

	// Adjusts corner coordinates: This should be simply:
	//   m_posX = m_minX;
	//   m_posY = m_maxY;
	// But account for centering if we have lock aspect:
	m_posX = (xMin+xMax)/2 - ((m_scrX - m_marginLeft - m_marginRight)/2 + m_marginLeft)/m_scaleX ; // m_posX = (xMin+xMax)/2 - (m_scrX/2)/m_scaleX;
//	m_posY = (yMin+yMax)/2 + ((m_scrY - m_marginTop - m_marginBottom)/2 - m_marginTop)/m_scaleY;  // m_posY = (yMin+yMax)/2 + (m_scrY/2)/m_scaleY;
	m_posY = (yMin+yMax)/2 + ((m_scrY - m_marginTop - m_marginBottom)/2 + m_marginTop)/m_scaleY;  // m_posY = (yMin+yMax)/2 + (m_scrY/2)/m_scaleY;

#ifdef MATHPLOT_DO_LOGGING
	wxLogMessage(_("mpWindow::Fit() m_desiredXmin=%f m_desiredXmax=%f  m_desiredYmin=%f m_desiredYmax=%f"), xMin,xMax,yMin,yMax);
	wxLogMessage(_("mpWindow::Fit() m_scaleX = %f , m_scrX = %d,m_scrY=%d, Ax=%f, Ay=%f, m_posX=%f, m_posY=%f"), m_scaleX, m_scrX,m_scrY, Ax,Ay,m_posX,m_posY);
#endif

	// It is VERY IMPORTANT to DO NOT call Refresh if we are drawing to the printer!!
	// Otherwise, the DC dimensions will be those of the window instead of the printer device
	if (printSizeX==NULL || printSizeY==NULL)
		UpdateAll();
}

// Patch ngpaton
void mpWindow::DoZoomInXCalc (const int staticXpixel)
{
	// Preserve the position of the clicked point:
	double staticX = p2x( staticXpixel );
	// Zoom in:
	m_scaleX = m_scaleX * zoomIncrementalFactor;
	// Adjust the new m_posx
	m_posX = staticX - (staticXpixel / m_scaleX);
    // Adjust desired
	m_desiredXmin = m_posX;
	m_desiredXmax = m_posX + (m_scrX - (m_marginLeft + m_marginRight)) / m_scaleX;
#ifdef MATHPLOT_DO_LOGGING
	wxLogMessage(_("mpWindow::DoZoomInXCalc() prior X coord: (%f), new X coord: (%f) SHOULD BE EQUAL!!"), staticX, p2x(staticXpixel));
#endif
}

void mpWindow::DoZoomInYCalc (const int staticYpixel)
{
	// Preserve the position of the clicked point:
	double staticY = p2y( staticYpixel );
	// Zoom in:
	m_scaleY = m_scaleY * zoomIncrementalFactor;
	// Adjust the new m_posy:
	m_posY = staticY + (staticYpixel / m_scaleY);
    // Adjust desired
	m_desiredYmax = m_posY;
	m_desiredYmin = m_posY - (m_scrY - (m_marginTop + m_marginBottom)) / m_scaleY;
#ifdef MATHPLOT_DO_LOGGING
	wxLogMessage(_("mpWindow::DoZoomInYCalc() prior Y coord: (%f), new Y coord: (%f) SHOULD BE EQUAL!!"), staticY, p2y(staticYpixel));
#endif
}

void mpWindow::DoZoomOutXCalc  (const int staticXpixel)
{
	// Preserve the position of the clicked point:
	double staticX = p2x( staticXpixel );
	// Zoom out:
	m_scaleX = m_scaleX / zoomIncrementalFactor;
	// Adjust the new m_posx/y:
	m_posX = staticX - (staticXpixel / m_scaleX);
    // Adjust desired
	m_desiredXmin = m_posX;
	m_desiredXmax = m_posX + (m_scrX - (m_marginLeft + m_marginRight)) / m_scaleX;
#ifdef MATHPLOT_DO_LOGGING
	wxLogMessage(_("mpWindow::DoZoomOutXCalc() prior X coord: (%f), new X coord: (%f) SHOULD BE EQUAL!!"), staticX, p2x(staticXpixel));
#endif
}

void mpWindow::DoZoomOutYCalc  (const int staticYpixel)
{
	// Preserve the position of the clicked point:
	double staticY = p2y( staticYpixel );
	// Zoom out:
	m_scaleY = m_scaleY / zoomIncrementalFactor;
	// Adjust the new m_posx/y:
	m_posY = staticY + (staticYpixel / m_scaleY);
    // Adjust desired
	m_desiredYmax = m_posY;
	m_desiredYmin = m_posY - (m_scrY - (m_marginTop + m_marginBottom)) / m_scaleY;
#ifdef MATHPLOT_DO_LOGGING
	wxLogMessage(_("mpWindow::DoZoomOutYCalc() prior Y coord: (%f), new Y coord: (%f) SHOULD BE EQUAL!!"), staticY, p2y(staticYpixel));
#endif
}


void mpWindow::ZoomIn(const wxPoint& centerPoint )
{
	wxPoint	c(centerPoint);
	if (c == wxDefaultPosition)
	{
		GetClientSize(&m_scrX, &m_scrY);
		c.x = (m_scrX - m_marginLeft - m_marginRight)/2 + m_marginLeft; // c.x = m_scrX/2;
		c.y = (m_scrY - m_marginTop - m_marginBottom)/2 - m_marginTop; // c.y = m_scrY/2;
	}

	// Preserve the position of the clicked point:
	double prior_layer_x = p2x( c.x );
	double prior_layer_y = p2y( c.y );

	// Zoom in:
	m_scaleX *= zoomIncrementalFactor;
	m_scaleY *= (m_mouseZoomMode==mpZOOM_XY ? zoomIncrementalFactor : 1 );

	// Adjust the new m_posx/y:
	m_posX = prior_layer_x - c.x / m_scaleX;
	m_posY = prior_layer_y + c.y / m_scaleY;

	m_desiredXmin = m_posX;
	m_desiredXmax = m_posX + (m_scrX - m_marginLeft - m_marginRight) / m_scaleX; // m_desiredXmax = m_posX + m_scrX / m_scaleX;
	m_desiredYmax = m_posY;
	m_desiredYmin = m_posY - (m_scrY - m_marginTop - m_marginBottom) / m_scaleY; // m_desiredYmin = m_posY - m_scrY / m_scaleY;


#ifdef MATHPLOT_DO_LOGGING
	wxLogMessage(_("mpWindow::ZoomIn() prior coords: (%f,%f), new coords: (%f,%f) SHOULD BE EQUAL!!"), prior_layer_x,prior_layer_y, p2x(c.x),p2y(c.y));
#endif

	UpdateAll();
}

void mpWindow::ZoomOut(const wxPoint& centerPoint )
{
	wxPoint	c(centerPoint);
	if (c == wxDefaultPosition)
	{
		GetClientSize(&m_scrX, &m_scrY);
		c.x = (m_scrX - m_marginLeft - m_marginRight)/2 + m_marginLeft; // c.x = m_scrX/2;
		c.y = (m_scrY - m_marginTop - m_marginBottom)/2 - m_marginTop; // c.y = m_scrY/2;
	}

	// Preserve the position of the clicked point:
	double prior_layer_x = p2x( c.x );
	double prior_layer_y = p2y( c.y );
	/*double  tmpPosX = m_posX,
            tmpPosY = m_posY,
            tmpDesXmin = m_desiredXmin,
            tmpDesXmax = m_desiredXmax,
            tmpDesYmin = m_desiredYmin,
            tmpDesYmax = m_desiredYmax,
            tmpScaleX = m_scaleX,
            tmpScaleY = m_scaleY;*/

	// Zoom out:

	/*if(m_scaleBoundsEnable)
	{
	    if(m_scaleXmax > (m_scaleX / zoomIncrementalFactor) );
            m_scaleX /= zoomIncrementalFactor;

	    if( m_scaleYmax > ( m_scaleY / (m_mouseZoomMode==mpZOOM_XY ? zoomIncrementalFactor : 1)  ) )
            m_scaleY /= (m_mouseZoomMode==mpZOOM_XY ? zoomIncrementalFactor : 1);

	}
	else*/
	{
        m_scaleX /= zoomIncrementalFactor;
        m_scaleY /= (m_mouseZoomMode==mpZOOM_XY ? zoomIncrementalFactor : 1);
    }

	// Adjust the new m_posx/y:
	m_posX = prior_layer_x - c.x / m_scaleX;

	if(m_mouseZoomMode==mpZOOM_XY)
        m_posY = prior_layer_y + c.y / m_scaleY;

	m_desiredXmin = m_posX;
	m_desiredXmax = m_posX + (m_scrX - m_marginLeft - m_marginRight) / m_scaleX; // m_desiredXmax = m_posX + m_scrX / m_scaleX;
	m_desiredYmax = m_posY;
	m_desiredYmin = m_posY - (m_scrY - m_marginTop - m_marginBottom) / m_scaleY; // m_desiredYmin = m_posY - m_scrY / m_scaleY;

	/*if(m_lockBoundMaxX && m_desiredXmax > m_maxX){
        m_posX = tmpPosX;
        m_scaleX = tmpScaleX;
        m_desiredXmax = tmpDesXmax;
    }
    if(m_lockBoundMinX && m_desiredXmin < m_minX){
        m_posX = tmpPosX;
        m_scaleX = tmpScaleX;
        m_desiredXmin = tmpDesXmin;
    }
    if(m_lockBoundMaxY && m_desiredYmax > m_maxY){
        m_posX = tmpPosY;
        m_scaleY = tmpScaleY;
        m_desiredYmax = tmpDesYmax;
    }
    if(m_lockBoundMinY && m_desiredYmin < m_minY){
        m_posY = tmpPosY;
        m_scaleY = tmpScaleY;
        m_desiredYmin = tmpDesYmin;
    }*/

#ifdef MATHPLOT_DO_LOGGING
	wxLogMessage(_("mpWindow::ZoomOut() prior coords: (%f,%f), new coords: (%f,%f) SHOULD BE EQUAL!!"), prior_layer_x,prior_layer_y, p2x(c.x),p2y(c.y));
#endif
	UpdateAll();
}

void mpWindow::ZoomInX()
{
    m_scaleX = m_scaleX * zoomIncrementalFactor;
    UpdateAll();
}

void mpWindow::ZoomOutX()
{
    m_scaleX = m_scaleX / zoomIncrementalFactor;
    UpdateAll();
}

void mpWindow::ZoomInY()
{
    m_scaleY = m_scaleY * zoomIncrementalFactor;
    UpdateAll();
}

void mpWindow::ZoomOutY()
{
    m_scaleY = m_scaleY / zoomIncrementalFactor;
    UpdateAll();
}

void mpWindow::ZoomRect(wxPoint p0, wxPoint p1)
{
	// Compute the 2 corners in graph coordinates:
	double p0x = p2x(p0.x);
	double p0y = p2y(p0.y);
	double p1x = p2x(p1.x);
	double p1y = p2y(p1.y);

	// Order them:
	double zoom_x_min = p0x<p1x ? p0x:p1x;
	double zoom_x_max = p0x>p1x ? p0x:p1x;
	double zoom_y_min = p0y<p1y ? p0y:p1y;
	double zoom_y_max = p0y>p1y ? p0y:p1y;

#ifdef MATHPLOT_DO_LOGGING
	wxLogMessage(_("Zoom: (%f,%f)-(%f,%f)"),zoom_x_min,zoom_y_min,zoom_x_max,zoom_y_max);
#endif

	Fit(zoom_x_min,zoom_x_max,zoom_y_min,zoom_y_max);
}

void mpWindow::LockAspect(bool enable)
{
	m_lockaspect = enable;
	m_popmenu.Check(mpID_LOCKASPECT, enable);

	// Try to fit again with the new config:
	Fit( m_desiredXmin, m_desiredXmax, m_desiredYmin, m_desiredYmax );
}

void mpWindow::OnShowPopupMenu(wxMouseEvent &event)
{

}

void mpWindow::OnMouseLeaveWindow(wxMouseEvent &event)
{
    if(m_zoomingRect)wxDELETE(m_zoomingRect);
    m_mouseDownHasHappened = false;
}

void mpWindow::OnMouseMiddleDown(wxMouseEvent     &event)
{
    m_mouseMovedAfterMiddleClickWithCtrl = FALSE;
    m_mouseMClick_X = event.GetX();
    m_mouseMClick_Y = event.GetY();
    wxPoint pointClicked = event.GetPosition();
}
void mpWindow::OnMouseMiddleRelease(wxMouseEvent  &event)
{
    wxPoint release(event.GetX(), event.GetY());
    wxPoint press(m_mouseMClick_X, m_mouseMClick_Y);

    if(m_zoomingRect)wxDELETE(m_zoomingRect);
    if(m_mouseMovedAfterMiddleClickWithCtrl)
    {
        m_mouseMovedAfterMiddleClickWithCtrl = FALSE;
        return;
    }

    if( release != press && m_enableMouseNavigation &&
        !event.m_rightDown  //this allows to cancel the zooming
      )
    {
        //only when mouseNavigation is enabled



        //must be more than 8 pixel different
        //pressed to avoid injury
        if(press.x>>4 != release.x>>4)
        {

            if(m_zoomingHorizontally){
                press.y     = GetMarginTop();
                release.y   = GetScrY() - GetMarginBottom();
            } else { /// @bug there might be some bug
                press.x     = GetMarginLeft();
                release.x   = GetScrX() - GetMarginRight();
            }
            ZoomRect(press, release);
        }
    }

    event.Skip();
}
void mpWindow::OnMouseRightRelease(wxMouseEvent &event)
{
    // Only display menu if the user has not "dragged" the figure


    SetCursor( *wxSTANDARD_CURSOR );
    wxPoint pos = event.GetPosition();

    //Check if there is point layer point in this position
    for (wxLayerList::iterator li = m_layers.begin(); li != m_layers.end(); li++) {
        if ((*li)->IsPointLayer())
        {
            mpPointLayer *ly = dynamic_cast<mpPointLayer*>(*li);
            if(ly->Inside(pos) )
            {
                ly->popup(*this, event);
                return; //nothing more.
            }
        }
        if((*li)->IsScaleXLayer())
        {
            mpScaleX *ly = dynamic_cast<mpScaleX*>(*li);
            if(ly->Inside(pos) )
            {
                ly->popup(*this, event);
                return; //nothing more.
            }
        }
        if((*li)->IsScaleYLayer())
        {
            mpScaleY *ly = dynamic_cast<mpScaleY*>(*li);
            if(ly->Inside(pos) )
            {
                ly->popup(*this, event);
                return; //nothing more.
            }
        }
        if((*li)->IsInfoLegendLayer())
        {
            mpInfoLegend *ly = dynamic_cast<mpInfoLegend*>(*li);
            if(ly->Inside(pos) )
            {
                ly->popup(*this, event);
                return; //nothing more.
            }
        }
    }

	if (!m_enableMouseNavigation)
    {
        return;
    }


    if (!m_mouseMovedAfterRightClick && m_enableMousePopup)   // JLB
    {
        m_clickedX = event.GetX();
        m_clickedY = event.GetY();
        PopupMenu( &m_popmenu, event.GetX(), event.GetY());
    }
}

void mpWindow::OnLockAspect(wxCommandEvent& WXUNUSED(event))
{
    LockAspect( !m_lockaspect );
}
void mpWindow::OnMouseHelp(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(OnMouseHelpString, OnMouseHelpStringTitle, wxOK, this);
}

void mpWindow::OnFit(wxCommandEvent& WXUNUSED(event))
{
    Fit();
}
void mpWindow::OnCenter(wxCommandEvent& WXUNUSED(event))
{
	GetClientSize(&m_scrX, &m_scrY);
        int centerX = (m_scrX - m_marginLeft - m_marginRight)/2; // + m_marginLeft; // c.x = m_scrX/2;
	int centerY = (m_scrY - m_marginTop - m_marginBottom)/2; // - m_marginTop; // c.y = m_scrY/2;
        SetPos( p2x(m_clickedX - centerX), p2y(m_clickedY - centerY) );
	//SetPos( p2x(m_clickedX-m_scrX/2), p2y(m_clickedY-m_scrY/2) );  //SetPos( (double)(m_clickedX-m_scrX/2) / m_scaleX + m_posX, (double)(m_scrY/2-m_clickedY) / m_scaleY + m_posY);
}

void mpWindow::OnZoomIn(wxCommandEvent& WXUNUSED(event))
{
	ZoomIn( wxPoint(m_mouseRClick_X,m_mouseRClick_Y) );
}

void mpWindow::OnZoomOut(wxCommandEvent& WXUNUSED(event))
{
    ZoomOut();
}

void mpWindow::OnSize( wxSizeEvent& WXUNUSED(event) )
{
    // Try to fit again with the new window size:
    Fit( m_desiredXmin, m_desiredXmax, m_desiredYmin, m_desiredYmax );
#ifdef MATHPLOT_DO_LOGGING
    wxLogMessage(_("mpWindow::OnSize() m_scrX = %d, m_scrY = %d"), m_scrX, m_scrY);
#endif // MATHPLOT_DO_LOGGING
}


bool mpWindow::AddLayer( mpLayer* layer, bool refreshDisplay )
{
    if (layer != NULL) {
        m_layers.push_back( layer );

        if( layer->IsPointLayer() ) SetLayerBottom(layer);

    	if (refreshDisplay) UpdateAll();
    	return true;
    	};
    return false;
}

bool mpWindow::DelLayer(
    mpLayer*    layer,
    bool        alsoDeleteObject,
    bool        refreshDisplay )
{
    wxLayerList::iterator layIt;
    for (layIt = m_layers.begin(); layIt != m_layers.end(); layIt++)
    {
    	if (*layIt == layer)
	{
	        // Also delete the object?
        	if (alsoDeleteObject)
			delete *layIt;
	    	m_layers.erase(layIt); // this deleted the reference only
	    	if (refreshDisplay)
			UpdateAll();
	    	return true;
	}
    }
    return false;
}

void mpWindow::DelAllLayers( bool alsoDeleteObject, bool refreshDisplay)
{
	while ( m_layers.size()>0 )
    {
		// Also delete the object?
		if (alsoDeleteObject) delete m_layers[0];
		m_layers.erase( m_layers.begin() ); // this deleted the reference only
    }
	if (refreshDisplay)  UpdateAll();
}

// void mpWindow::DoPrepareDC(wxDC& dc)
// {
//     dc.SetDeviceOrigin(x2p(m_minX), y2p(m_maxY));
// }

void mpWindow::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    wxPaintDC dc(this);
    dc.GetSize(&m_scrX, &m_scrY);   // This is the size of the visible area only!
//     DoPrepareDC(dc);

#ifdef MATHPLOT_DO_LOGGING
    {
        int px, py;
        GetViewStart( &px, &py );
        wxLogMessage(_("[mpWindow::OnPaint] vis.area:%ix%i px=%i py=%i"),m_scrX,m_scrY,px,py);
    }
#endif

    // Selects direct or buffered draw:
    wxDC    *trgDc;

    // J.L.Blanco @ Aug 2007: Added double buffer support
    if (m_enableDoubleBuffer)
    {
        if (m_last_lx!=m_scrX || m_last_ly!=m_scrY)
        {
            wxDELETE(m_buff_bmp);
            m_buff_bmp = new wxBitmap(m_scrX,m_scrY);
            m_buff_dc.SelectObject(*m_buff_bmp);
            m_last_lx=m_scrX;
            m_last_ly=m_scrY;
        }
        trgDc = &m_buff_dc;
    }
    else
    {
        trgDc = &dc;
    }

    // Draw background:
    //trgDc->SetDeviceOrigin(0,0);


    trgDc->SetPen( *wxTRANSPARENT_PEN );
    wxBrush brush( GetBackgroundColour() );
    trgDc->SetBrush( brush );
    trgDc->SetTextForeground(m_fgColour);

    if(m_gradienBackColour)
        trgDc->GradientFillLinear(wxRect(0,0,m_scrX,m_scrY), m_gradienInitialColour, m_gradienDestColour, m_gradienDirect);
    else
        trgDc->DrawRectangle(0,0,m_scrX,m_scrY);

    if(m_zoomingRect)
    {

        //wxToolTip* tip = GetToolTip();
        //if(tip)tip->Enable(false);

        wxPen pen(*wxBLACK, 1, wxDOT);
        trgDc->SetPen(pen);


        if( m_mouseZoomMode == mpZOOM_XY )
        {
            trgDc->SetBrush(*wxLIGHT_GREY_BRUSH);
            trgDc->DrawRectangle(*m_zoomingRect);
        }
        else if(m_mouseZoomMode == mpZOOM_X)
        {
            wxCoord  width, height;
            trgDc->GetSize(&width, &height);

            trgDc->DrawLine(m_zoomingRect->x, 0,  m_zoomingRect->x, height);
            trgDc->DrawLine(m_zoomingRect->x+m_zoomingRect->width, 0,  m_zoomingRect->x+m_zoomingRect->width, height);
            trgDc->SetBrush(*wxLIGHT_GREY_BRUSH);
            trgDc->DrawRectangle(*m_zoomingRect);

            trgDc->SetBrush(*wxTRANSPARENT_BRUSH);

        }
    }
    else {
        /*wxToolTip* tip = wxWindow::GetToolTip();
        if(tip)tip->Enable(true);*/
    }

    // Draw all the layers:
    //trgDc->SetDeviceOrigin( m_scrX>>1, m_scrY>>1);  // Origin at the center
    wxLayerList::iterator li;
    for (li = m_layers.begin(); li != m_layers.end(); li++)
    {
		mpLayer* ptLayer = (*li);
		ptLayer->Plot(*trgDc, *this);
    };


    #ifndef ZEROZERO
    if(m_mouseMovedAfterMiddleClickWithCtrl)
    {
        // Measure tape.
        // show dx, dy and distance
        wxPoint pos0 = wxPoint(m_mouseMClick_X, m_mouseMClick_Y);
        wxPoint pos1 = wxPoint(m_mousePosition_X, m_mousePosition_Y);
        double  dx = p2x(pos0.x)-p2x(pos1.x),
                dy = p2y(pos0.y)-p2y(pos1.y),
                distance = sqrtf(dx*dx+dy*dy);
        wxString label = wxString::Format( _("dX: %.2f\ndY: %.2f\nDistance: %.2f"),
                                            dx, dy, distance );
        wxRect rect;
        rect.x = pos0.x+(pos1.x - pos0.x)/2;
        rect.y = pos0.y+(pos1.y - pos0.y)/2;

        trgDc->SetPen( *wxBLACK_PEN );
        trgDc->DrawLine(pos0, pos1);

        trgDc->GetMultiLineTextExtent(label, &rect.width, &rect.height);
        trgDc->SetBrush(*wxTRANSPARENT_BRUSH);
        trgDc->DrawRectangle( rect.x-5, rect.y-5, rect.width+10, rect.height+10 );
        trgDc->DrawLabel( label, rect );
    }
    #endif


    // If doublebuffer, draw now to the window:
    if (m_enableDoubleBuffer)
    {
        //trgDc->SetDeviceOrigin(0,0);
        //dc.SetDeviceOrigin(0,0);  // Origin at the center
        dc.Blit(0,0,m_scrX,m_scrY,trgDc,0,0);
    }

/*    if (m_coordTooltip) {
        wxString toolTipContent;
        wxPoint mousePoint =  wxGetMousePosition();
        toolTipContent.Printf(_("X = %f\nY = %f"), p2x(mousePoint.x), p2y(mousePoint.y));
        SetToolTip(toolTipContent);
    }*/
    // If scrollbars are enabled, refresh them
    if (m_enableScrollBars) {
/*       m_scrollX = (int) floor((m_posX - m_minX)*m_scaleX);
       m_scrollY = (int) floor((m_maxY - m_posY )*m_scaleY);
       Scroll(m_scrollX, m_scrollY);*/
       // Scroll(x2p(m_posX), y2p(m_posY));
//             SetVirtualSize((int) ((m_maxX - m_minX)*m_scaleX), (int) ((m_maxY - m_minY)*m_scaleY));
//         int centerX = (m_scrX - m_marginLeft - m_marginRight)/2; // + m_marginLeft; // c.x = m_scrX/2;
// 	int centerY = (m_scrY - m_marginTop - m_marginBottom)/2; // - m_marginTop; // c.y = m_scrY/2;
        /*SetScrollbars(1, 1, (int) ((m_maxX - m_minX)*m_scaleX), (int) ((m_maxY - m_minY)*m_scaleY));*/ //, x2p(m_posX + centerX/m_scaleX), y2p(m_posY - centerY/m_scaleY), true);
    }

}

// void mpWindow::OnScroll2(wxScrollWinEvent &event)
// {
// #ifdef MATHPLOT_DO_LOGGING
//     wxLogMessage(_("[mpWindow::OnScroll2] Init: m_posX=%f m_posY=%f, sc_pos = %d"),m_posX,m_posY, event.GetPosition());
// #endif
//     // If scrollbars are not enabled, Skip operation
//     if (!m_enableScrollBars) {
//         event.Skip();
//         return;
//     }
// //     m_scrollX = (int) floor((m_posX - m_minX)*m_scaleX);
// //     m_scrollY = (int) floor((m_maxY - m_posY /*- m_minY*/)*m_scaleY);
// //     Scroll(m_scrollX, m_scrollY);
//
// //     GetClientSize( &m_scrX, &m_scrY);
//     //Scroll(x2p(m_desiredXmin), y2p(m_desiredYmin));
//     int pixelStep = 1;
//     if (event.GetOrientation() == wxHORIZONTAL) {
//         //m_desiredXmin -= (m_scrollX - event.GetPosition())/m_scaleX;
//         //m_desiredXmax -= (m_scrollX - event.GetPosition())/m_scaleX;
//         m_posX -= (m_scrollX - event.GetPosition())/m_scaleX;
//         m_scrollX = event.GetPosition();
//     }
//     Fit(m_desiredXmin, m_desiredXmax, m_desiredYmin, m_desiredYmax);
// // /*    int pixelStep = 1;
// //     if (event.GetOrientation() == wxHORIZONTAL) {
// //         m_posX 		-= (px - event.GetPosition())/m_scaleX;//(pixelStep/m_scaleX);
// // 	m_desiredXmax 	-= (px - event.GetPosition())/m_scaleX;//(pixelStep/m_scaleX);
// // 	m_desiredXmin 	-= (px - event.GetPosition())/m_scaleX;//(pixelStep/m_scaleX);
// //         //SetPosX( (double)px / GetScaleX() + m_minX + (double)(width>>1)/GetScaleX());
// // //         m_posX = p2x(px); //m_minX + (double)(px /*+ (m_scrX)*/)/GetScaleX();
// //     } else {
// //         m_posY 		+= (py - event.GetPosition())/m_scaleY;//(pixelStep/m_scaleY);
// // 	m_desiredYmax	+= (py - event.GetPosition())/m_scaleY;//(pixelStep/m_scaleY);
// // 	m_desiredYmax	+= (py - event.GetPosition())/m_scaleY;//(pixelStep/m_scaleY);
// //         //SetPosY( m_maxY - (double)py / GetScaleY() - (double)(height>>1)/GetScaleY());
// //         //m_posY = m_maxY - (double)py / GetScaleY() - (double)(height>>1)/GetScaleY();
// // //         m_posY = p2y(py);//m_maxY - (double)(py /*+ (m_scrY)*/)/GetScaleY();
// //     }*/
// #ifdef MATHPLOT_DO_LOGGING
//     int px, py;
//     GetViewStart( &px, &py);
//     wxLogMessage(_("[mpWindow::OnScroll2] End:  m_posX = %f, m_posY = %f, px = %f, py = %f"),m_posX, m_posY, px, py);
// #endif
//
//     UpdateAll();
// //     event.Skip();
// }

void mpWindow::SetMPScrollbars(bool status)
{
    // Temporary behaviour: always disable scrollbars
    m_enableScrollBars = status; //false;
    if (status == false)
    {
        SetScrollbar(wxHORIZONTAL, 0, 0, 0);
        SetScrollbar(wxVERTICAL, 0, 0, 0);
    }
    // else the scroll bars will be updated in UpdateAll();
    UpdateAll();

//     EnableScrolling(false, false);
//     m_enableScrollBars = status;
//     EnableScrolling(status, status);
/*    m_scrollX = (int) floor((m_posX - m_minX)*m_scaleX);
    m_scrollY = (int) floor((m_posY - m_minY)*m_scaleY);*/
//     int scrollWidth = (int) floor((m_maxX - m_minX)*m_scaleX) - m_scrX;
//     int scrollHeight = (int) floor((m_minY - m_maxY)*m_scaleY) - m_scrY;

// /*    m_scrollX = (int) floor((m_posX - m_minX)*m_scaleX);
//     m_scrollY = (int) floor((m_maxY - m_posY /*- m_minY*/)*m_scaleY);
//     int scrollWidth = (int) floor(((m_maxX - m_minX) - (m_desiredXmax - m_desiredXmin))*m_scaleX);
//     int scrollHeight = (int) floor(((m_maxY - m_minY) - (m_desiredYmax - m_desiredYmin))*m_scaleY);
// #ifdef MATHPLOT_DO_LOGGING
//     wxLogMessage(_("mpWindow::SetMPScrollbars() scrollWidth = %d, scrollHeight = %d"), scrollWidth, scrollHeight);
// #endif
//     if(status) {
//         SetScrollbars(1,
//                       1,
//                       scrollWidth,
//                       scrollHeight,
//                       m_scrollX,
//                       m_scrollY);
// //         SetVirtualSize((int) (m_maxX - m_minX), (int) (m_maxY - m_minY));
//     }
//     Refresh(false);*/
};

bool mpWindow::UpdateBBox()
{
    bool first = TRUE;

    for (wxLayerList::iterator li = m_layers.begin(); li != m_layers.end(); li++)
    {
        mpLayer* f = *li;

        if (f->HasBBox() && f->IsVisible()) //updated: If not visible, don't check bounding boxes! 10.11.-09 by Jussi V-A
        {
            if (first)
            {
                first = FALSE;
                m_minX = f->GetMinX(); m_maxX=f->GetMaxX();
                m_minY = f->GetMinY(); m_maxY=f->GetMaxY();
            }
            else
            {
                if (f->GetMinX()<m_minX) m_minX=f->GetMinX(); if (f->GetMaxX()>m_maxX) m_maxX=f->GetMaxX();
                if (f->GetMinY()<m_minY) m_minY=f->GetMinY(); if (f->GetMaxY()>m_maxY) m_maxY=f->GetMaxY();
            }
        }
        //node = node->GetNext();
    }
#ifdef MATHPLOT_DO_LOGGING
	wxLogDebug(wxT("[mpWindow::UpdateBBox] Bounding box: Xmin = %f, Xmax = %f, Ymin = %f, YMax = %f"), m_minX, m_maxX, m_minY, m_maxY);
#endif // MATHPLOT_DO_LOGGING
    return first == FALSE;
}

// void mpWindow::UpdateAll()
// {
    // GetClientSize( &m_scrX,&m_scrY);
/*    if (m_enableScrollBars) {
        // The "virtual size" of the scrolled window:
        const int sx = (int)((m_maxX - m_minX) * GetScaleX());
        const int sy = (int)((m_maxY - m_minY) * GetScaleY());
	SetVirtualSize(sx, sy);
	SetScrollRate(1, 1);*/
//         const int px = (int)((GetPosX() - m_minX) * GetScaleX());// - m_scrX); //(cx>>1));

        // J.L.Blanco, Aug 2007: Formula fixed:
//         const int py = (int)((m_maxY - GetPosY()) * GetScaleY());// - m_scrY); //(cy>>1));
//         int px, py;
//         GetViewStart(&px0, &py0);
// 	px = (int)((m_posX - m_minX)*m_scaleX);
// 	py = (int)((m_maxY - m_posY)*m_scaleY);

//         SetScrollbars( 1, 1, sx - m_scrX, sy - m_scrY, px, py, TRUE);
//     }

// Working code
// 	UpdateBBox();
//    Refresh( FALSE );
// end working code

// Old version
/*   bool box = UpdateBBox();
    if (box)
    {
        int cx, cy;
        GetClientSize( &cx, &cy);

        // The "virtual size" of the scrolled window:
        const int sx = (int)((m_maxX - m_minX) * GetScaleX());
        const int sy = (int)((m_maxY - m_minY) * GetScaleY());

        const int px = (int)((GetPosX() - m_minX) * GetScaleX() - (cx>>1));

        // J.L.Blanco, Aug 2007: Formula fixed:
        const int py = (int)((m_maxY - GetPosY()) * GetScaleY() - (cy>>1));

        SetScrollbars( 1, 1, sx, sy, px, py, TRUE);

#ifdef MATHPLOT_DO_LOGGING
        wxLogMessage(_("[mpWindow::UpdateAll] Size:%ix%i ScrollBars:%i,%i"),sx,sy,px,py);
#endif
}

    FitInside();
    Refresh( FALSE );
*/
// }

void mpWindow::UpdateAll()
{
	if (UpdateBBox())
    {
        if (m_enableScrollBars)
        {
            int cx, cy;
            GetClientSize( &cx, &cy);
            // Do x scroll bar
            {
                // Convert margin sizes from pixels to coordinates
                double leftMargin  = m_marginLeft / m_scaleX;
                // Calculate the range in coords that we want to scroll over
                double maxX = (m_desiredXmax > m_maxX) ? m_desiredXmax : m_maxX;
                double minX = (m_desiredXmin < m_minX) ? m_desiredXmin : m_minX;
                if ((m_posX + leftMargin) < minX)
                    minX = m_posX + leftMargin;
                // Calculate scroll bar size and thumb position
                int sizeX = (int) ((maxX - minX) * m_scaleX);
                int thumbX = (int)(((m_posX + leftMargin) - minX) * m_scaleX);
                SetScrollbar(wxHORIZONTAL, thumbX, cx - (m_marginRight + m_marginLeft), sizeX);
            }
            // Do y scroll bar
            {
                // Convert margin sizes from pixels to coordinates
                double topMargin = m_marginTop / m_scaleY;
                // Calculate the range in coords that we want to scroll over
                double maxY = (m_desiredYmax > m_maxY) ? m_desiredYmax : m_maxY;
                if ((m_posY - topMargin) > maxY)
                    maxY = m_posY - topMargin;
                double minY = (m_desiredYmin < m_minY) ? m_desiredYmin : m_minY;
                // Calculate scroll bar size and thumb position
                int sizeY = (int)((maxY - minY) * m_scaleY);
                int thumbY = (int)((maxY - (m_posY - topMargin)) * m_scaleY);
                SetScrollbar(wxVERTICAL, thumbY, cy - (m_marginTop + m_marginBottom), sizeY);
            }
        }
    }

    Refresh( FALSE );
}

void mpWindow::DoScrollCalc    (const int position, const int orientation)
{
    if (orientation == wxVERTICAL)
    {
        // Y axis
        // Get top margin in coord units
        double topMargin = m_marginTop / m_scaleY;
        // Calculate maximum Y coord to be shown in the graph
        double maxY = m_desiredYmax > m_maxY ? m_desiredYmax  : m_maxY;
        // Set new position
        SetPosY((maxY - (position / m_scaleY)) + topMargin);
    }
    else
    {
        // X Axis
        // Get left margin in coord units
        double leftMargin  = m_marginLeft / m_scaleX;
        // Calculate minimum X coord to be shown in the graph
        double minX = (m_desiredXmin < m_minX) ? m_desiredXmin : m_minX;
        // Set new position
        SetPosX((minX + (position / m_scaleX)) - leftMargin);
    }
}

void mpWindow::OnScrollThumbTrack (wxScrollWinEvent &event)
{
    DoScrollCalc(event.GetPosition(), event.GetOrientation());
}

void mpWindow::OnScrollPageUp (wxScrollWinEvent &event)
{
    int scrollOrientation = event.GetOrientation();
    // Get position before page up
    int position = GetScrollPos(scrollOrientation);
    // Get thumb size
    int thumbSize = GetScrollThumb(scrollOrientation);
    // Need to adjust position by a page
    position -= thumbSize;
    if (position < 0)
        position = 0;

    DoScrollCalc(position, scrollOrientation);
}
void mpWindow::OnScrollPageDown (wxScrollWinEvent &event)
{
    int scrollOrientation = event.GetOrientation();
    // Get position before page up
    int position = GetScrollPos(scrollOrientation);
    // Get thumb size
    int thumbSize = GetScrollThumb(scrollOrientation);
    // Get scroll range
    int scrollRange = GetScrollRange(scrollOrientation);
    // Need to adjust position by a page
    position += thumbSize;
    if (position > (scrollRange - thumbSize))
        position = scrollRange - thumbSize;

    DoScrollCalc(position, scrollOrientation);
}

void mpWindow::OnScrollLineUp     (wxScrollWinEvent &event)
{
    int scrollOrientation = event.GetOrientation();
    // Get position before page up
    int position = GetScrollPos(scrollOrientation);
    // Need to adjust position by a line
    position -= mpSCROLL_NUM_PIXELS_PER_LINE;
    if (position < 0)
        position = 0;

    DoScrollCalc(position, scrollOrientation);
}

void mpWindow::OnScrollLineDown   (wxScrollWinEvent &event)
{
    int scrollOrientation = event.GetOrientation();
    // Get position before page up
    int position = GetScrollPos(scrollOrientation);
    // Get thumb size
    int thumbSize = GetScrollThumb(scrollOrientation);
    // Get scroll range
    int scrollRange = GetScrollRange(scrollOrientation);
    // Need to adjust position by a page
    position += mpSCROLL_NUM_PIXELS_PER_LINE;
    if (position > (scrollRange - thumbSize))
        position = scrollRange - thumbSize;

    DoScrollCalc(position, scrollOrientation);
}

void mpWindow::OnScrollTop(wxScrollWinEvent &event)
{
    DoScrollCalc(0, event.GetOrientation());
}

void mpWindow::OnScrollBottom(wxScrollWinEvent &event)
{
    int scrollOrientation = event.GetOrientation();
    // Get thumb size
    int thumbSize = GetScrollThumb(scrollOrientation);
    // Get scroll range
    int scrollRange = GetScrollRange(scrollOrientation);

    DoScrollCalc(scrollRange - thumbSize, scrollOrientation);
}
// End patch ngpaton

void mpWindow::SetScaleX(double scaleX)
{
    if (scaleX!=0) m_scaleX=scaleX;
    UpdateAll();
}

// New methods implemented by Davide Rondini

unsigned int mpWindow::CountLayers()
{
    //wxNode *node = m_layers.GetFirst();
    unsigned int layerNo = 0;
    for(wxLayerList::iterator li = m_layers.begin(); li != m_layers.end(); li++)//while(node)
    	{
        if ((*li)->HasBBox()) layerNo++;
	// node = node->GetNext();
    	};
    return layerNo;
}

mpLayer* mpWindow::GetLayer(int position)
{
    if ((position >= (int) m_layers.size()) || position < 0) return NULL;
    return m_layers[position];
}

mpLayer* mpWindow::GetLayer( const wxString &name)
{
    for (wxLayerList::iterator it=m_layers.begin();it!=m_layers.end();it++)
        if (! (*it)->GetName().Cmp( name ) )
            return *it;
    return NULL;    // Not found
}
void mpWindow::SetLayerPosition(mpLayer* layer, int position)
{
    wxLayerList::iterator it;
    wxLayerList old = m_layers;
    int old_position=0;
    for (it=old.begin();it!=old.end();it++,old_position++)
    { if( *it == layer ) break; }
    if( it == old.end() ) return; //not found
    if(position==old_position) return; //already in this position
    m_layers.clear();
    for(int i=0;i<(int)old.size();i++)
    {
        if(i==old_position)continue;
        if(i==position) m_layers.push_back(layer);
        m_layers.push_back(old[i]);
    }
}
void mpWindow::SetLayerBottom(mpLayer* layer)
{
    SetLayerPosition(layer, 0);
}
void mpWindow::SetLayerTop(mpLayer* layer)
{
    SetLayerPosition(layer, m_layers.size()-1);
}

void mpWindow::GetBoundingBox(double* bbox)
{
	bbox[0] = m_minX;
	bbox[1] = m_maxX;
	bbox[2] = m_minY;
	bbox[3] = m_maxY;
}

bool mpWindow::SaveScreenshot(const wxString& filename, wxBitmapType type, wxSize imageSize, bool fit)
{
	int sizeX, sizeY;
	int bk_scrX, bk_scrY;
	if (imageSize == wxDefaultSize) {
		sizeX = m_scrX;
		sizeY = m_scrY;
	} else {
		sizeX = imageSize.x;
		sizeY = imageSize.y;
		bk_scrX = m_scrX;
		bk_scrY = m_scrY;
		SetScr(sizeX, sizeY);
	}

    wxBitmap screenBuffer(sizeX,sizeY);
    wxMemoryDC screenDC;
    screenDC.SelectObject(screenBuffer);
    screenDC.SetPen( *wxTRANSPARENT_PEN );
    wxBrush brush( GetBackgroundColour() );
    screenDC.SetBrush( brush );
    screenDC.DrawRectangle(0,0,sizeX,sizeY);

	if (fit) {
		Fit(m_minX, m_maxX, m_minY, m_maxY, &sizeX, &sizeY);
	} else {
		Fit(m_desiredXmin, m_desiredXmax, m_desiredYmin, m_desiredYmax, &sizeX, &sizeY);
	}
    // Draw all the layers:
    wxLayerList::iterator li;
    for (li = m_layers.begin(); li != m_layers.end(); li++)
    	(*li)->Plot(screenDC, *this);

	if (imageSize != wxDefaultSize) {
		// Restore dimensions
		SetScr(bk_scrX, bk_scrY);
                Fit(m_desiredXmin, m_desiredXmax, m_desiredYmin, m_desiredYmax, &bk_scrX, &bk_scrY);
		UpdateAll();
	}
    // Once drawing is complete, actually save screen shot
    wxImage screenImage = screenBuffer.ConvertToImage();
    return screenImage.SaveFile(filename, type);
}

void mpWindow::SetMargins(int top, int right, int bottom, int left)
{
    m_marginTop = top;
    m_marginRight = right;
    m_marginBottom = bottom;
    m_marginLeft = left;
}

mpInfoLayer* mpWindow::IsInsideInfoLayer(wxPoint& point)
{
    wxLayerList::iterator li;
    for (li = m_layers.begin(); li != m_layers.end(); li++) {
#ifdef MATHPLOT_DO_LOGGING
        wxLogMessage(_("mpWindow::IsInsideInfoLayer() examinining layer = %p"), (*li));
#endif // MATHPLOT_DO_LOGGING
        if ((*li)->IsInfo() && (*li)->IsVisible()) {
            mpInfoLayer* tmpLyr = (mpInfoLayer*) (*li);
#ifdef MATHPLOT_DO_LOGGING
            wxLogMessage(_("mpWindow::IsInsideInfoLayer() layer = %p"), (*li));
#endif // MATHPLOT_DO_LOGGING
            if (tmpLyr->Inside(point)) {
                return tmpLyr;
            }
        }
    }
    return NULL;
}
mpPointLayer* mpWindow::IsInsidePointLayer(wxPoint& point)
{
    wxLayerList::iterator li;
    for (li = m_layers.begin(); li != m_layers.end(); li++) {
#ifdef MATHPLOT_DO_LOGGING
        wxLogMessage(_("mpWindow::IsInsidePointLayer() examinining layer = %p"), (*li));
#endif // MATHPLOT_DO_LOGGING
        //
        if( (*li)->IsPointLayer() && (*li)->IsVisible() ) {
            mpPointLayer* tmpLyr = (mpPointLayer*) (*li);
#ifdef MATHPLOT_DO_LOGGING
            wxLogMessage(_("mpWindow::IsInsidePointLayer() layer = %p"), (*li));
#endif // MATHPLOT_DO_LOGGING

            //Check if point is enabled
            //IsAllowedToMove
            if(  tmpLyr->Inside(point) ||
                !tmpLyr->IsPointDefined() ||
                !tmpLyr->IsDraggingOnlyFromPoint() )

            {
                return tmpLyr;
            }

        }
    }
    return NULL;
}
void mpWindow::SetLayerVisible(const wxString &name, bool viewable)
{
	mpLayer* lx = GetLayer(name);
	if ( lx ) {
		lx->SetVisible(viewable);
		UpdateAll();
	}
}

bool mpWindow::IsLayerVisible(const wxString &name )
{
	mpLayer* lx = GetLayer(name);
	return (lx) ? lx->IsVisible() : false;
}

void mpWindow::SetLayerVisible(const unsigned int position, bool viewable)
{
	mpLayer* lx = GetLayer(position);
	if ( lx ) {
		lx->SetVisible(viewable);
		UpdateAll();
	}
}

bool mpWindow::IsLayerVisible(const unsigned int position )
{
	mpLayer* lx = GetLayer(position);
	return (lx) ? lx->IsVisible() : false;
}

void mpWindow::SetColourTheme(const wxColour& bgColour, const wxColour& drawColour, const wxColour& axesColour)
{
	 SetBackgroundColour(bgColour);
	 SetForegroundColour(drawColour);
	 m_bgColour = bgColour;
	 m_fgColour = drawColour;
	 m_axColour = axesColour;
	// cycle between layers to set colours and properties to them
    wxLayerList::iterator li;
    for (li = m_layers.begin(); li != m_layers.end(); li++) {
		if ((*li)->GetLayerType() == mpLAYER_AXIS) {
			wxPen axisPen = (*li)->GetPen(); // Get the old pen to modify only colour, not style or width
			axisPen.SetColour(axesColour);
			(*li)->SetPen(axisPen);

			mpScaleX* scale = dynamic_cast<mpScaleX*>(*li);
			scale->SetTicksColour( axesColour );

		}
		else if ((*li)->GetLayerType() == mpLAYER_INFO) {
			wxPen infoPen = (*li)->GetPen(); // Get the old pen to modify only colour, not style or width
			infoPen.SetColour(drawColour);
			(*li)->SetPen(infoPen);
		}
		else if ((*li)->GetLayerType() == mpLAYER_PLOT) {
            wxPen plotPen = (*li)->GetPen();  //Get the old pen to modify only colour, not style or width
            plotPen.SetColour( drawColour );
            (*li)->SetPen(plotPen);

            wxBrush plotBrush = (*li)->GetBrush(); //Get old brush
            plotBrush.SetColour( drawColour );
            (*li)->SetBrush( plotBrush );
		}
		else if ((*li)->GetLayerType() == mpLayer_POINT) {

		}
		else{
		    wxBrush plotBrush = (*li)->GetBrush(); //Get old brush
            plotBrush.SetColour( drawColour );
            (*li)->SetBrush( plotBrush );

		    wxPen plotPen = (*li)->GetPen();  //Get the old pen to modify only colour, not style or width
            plotPen.SetColour( drawColour );
            (*li)->SetPen(plotPen);
		}
	}
}

// void mpWindow::EnableCoordTooltip(bool value)
// {
//      m_coordTooltip = value;
// //      if (value) GetToolTip()->SetDelay(100);
// }

/*
double mpWindow::p2x(wxCoord pixelCoordX, bool drawOutside )
{
    if (drawOutside) {
        return m_posX + pixelCoordX/m_scaleX;
    }
    // Draw inside margins
    double marginScaleX = ((double)(m_scrX - m_marginLeft - m_marginRight))/m_scrX;
    return m_marginLeft + (m_posX + pixelCoordX/m_scaleX)/marginScaleX;
}

double mpWindow::p2y(wxCoord pixelCoordY, bool drawOutside )
{
    if (drawOutside) {
        return m_posY - pixelCoordY/m_scaleY;
    }
    // Draw inside margins
    double marginScaleY = ((double)(m_scrY - m_marginTop - m_marginBottom))/m_scrY;
    return m_marginTop + (m_posY - pixelCoordY/m_scaleY)/marginScaleY;
}

wxCoord mpWindow::x2p(double x, bool drawOutside)
{
    if (drawOutside) {
        return (wxCoord) ((x-m_posX) * m_scaleX);
    }
    // Draw inside margins
    double marginScaleX = ((double)(m_scrX - m_marginLeft - m_marginRight))/m_scrX;
#ifdef MATHPLOT_DO_LOGGING
    wxLogMessage(wxT("x2p ScrX = %d, marginRight = %d, marginLeft = %d, marginScaleX = %f"), m_scrX, m_marginRight, m_marginLeft,  marginScaleX);
#endif // MATHPLOT_DO_LOGGING
    return (wxCoord) (int)(((x-m_posX) * m_scaleX)*marginScaleX) - m_marginLeft;
}

wxCoord mpWindow::y2p(double y, bool drawOutside)
{
    if (drawOutside) {
        return (wxCoord) ( (m_posY-y) * m_scaleY);
    }
    // Draw inside margins
    double marginScaleY = ((double)(m_scrY - m_marginTop - m_marginBottom))/m_scrY;
#ifdef MATHPLOT_DO_LOGGING
    wxLogMessage(wxT("y2p ScrY = %d, marginTop = %d, marginBottom = %d, marginScaleY = %f"), m_scrY, m_marginTop, m_marginBottom, marginScaleY);
#endif // MATHPLOT_DO_LOGGING
    return (wxCoord) ((int)((m_posY-y) * m_scaleY)*marginScaleY) - m_marginTop;
}
*/


//-----------------------------------------------------------------------------
// mpFXYVector implementation - by Jose Luis Blanco (AGO-2007)
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(mpFXYVector, mpFXY)

// Constructor
mpFXYVector::mpFXYVector(wxString name, int flags ) : mpFXY(name,flags)
{
    m_index = 0;
    m_minX  = -1;
    m_maxX  = 1;
    m_minY  = -1;
    m_maxY  = 1;
    m_type = mpLAYER_PLOT;
}

void mpFXYVector::Rewind()
{
    m_index = 0;
}

bool mpFXYVector::GetNextXY(double & x, double & y)
{
    if (m_index>=m_xs.size())
        return FALSE;
    else
    {
        x = m_xs[m_index];
        y = m_ys[m_index];
        return (++m_index)<=m_xs.size();
    }
}
void mpFXYVector::Clear()
{
    m_xs.clear();
    m_ys.clear();
}

void mpFXYVector::SetData( const std::vector<double> &xs,const std::vector<double> &ys)
{
	// Check if the data vectora are of the same size
	if (xs.size() != ys.size()) {
		wxLogError(_("wxMathPlot error: X and Y vector are not of the same length!"));
		return;
	}
    // Copy the data:
    m_xs = xs;
    m_ys = ys;


    // Update internal variables for the bounding box.
    if (xs.empty())
    {
        m_minX  = xs[0];
        m_maxX  = xs[0];
        m_minY  = ys[0];
        m_maxY  = ys[0];

        std::vector<double>::const_iterator  it;

        for (it=xs.begin();it!=xs.end();it++)
        {
            if (*it<m_minX) m_minX=*it;
            if (*it>m_maxX) m_maxX=*it;
        }
        for (it=ys.begin();it!=ys.end();it++)
        {
            if (*it<m_minY) m_minY=*it;
            if (*it>m_maxY) m_maxY=*it;
        }
        m_minX-=0.5f;
        m_minY-=0.5f;
        m_maxX+=0.5f;
        m_maxY+=0.5f;
    }
    else
    {
        m_minX  = -1;
        m_maxX  = 1;
        m_minY  = -1;
        m_maxY  = 1;
    }
}

//-----------------------------------------------------------------------------
// mpText - provided by Val Greene
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(mpText, mpLayer)


/** @param name text to be displayed
@param offsetx x position in percentage (0-100)
@param offsetx y position in percentage (0-100)
*/
mpText::mpText( wxString name, int offsetx, int offsety )
{
    SetName(name);

    if (offsetx >= 0 && offsetx <= 100)
        m_offsetx = offsetx;
    else
        m_offsetx = 5;

    if (offsety >= 0 && offsety <= 100)
        m_offsety = offsety;
    else
        m_offsetx = 50;
    m_type = mpLAYER_INFO;
}

/** mpText Layer plot handler.
This implementation will plot the text adjusted to the visible area.
*/

void mpText::Plot(wxDC & dc, mpWindow & w)
{
	if (m_visible) {
		dc.SetPen(m_pen);
		dc.SetFont(m_font);

		wxCoord tw=0, th=0;
		dc.GetTextExtent( GetName(), &tw, &th);

	//     int left = -dc.LogicalToDeviceX(0);
	//     int width = dc.LogicalToDeviceX(0) - left;
	//     int bottom = dc.LogicalToDeviceY(0);
	//     int height = bottom - -dc.LogicalToDeviceY(0);

	/*    dc.DrawText( GetName(),
		(int)((((float)width/100.0) * m_offsety) + left - (tw/2)),
		(int)((((float)height/100.0) * m_offsetx) - bottom) );*/
		int px = m_offsetx*(w.GetScrX() - w.GetMarginLeft() - w.GetMarginRight())/100;
		int py = m_offsety*(w.GetScrY() - w.GetMarginTop() - w.GetMarginBottom())/100;
		dc.DrawText( GetName(), px, py);
	}
}

//-----------------------------------------------------------------------------
// mpPrintout - provided by Davide Rondini
//-----------------------------------------------------------------------------

mpPrintout::mpPrintout(mpWindow *drawWindow, const wxChar *title) : wxPrintout(title)
{
    drawn = false;
    plotWindow = drawWindow;
}

bool mpPrintout::OnPrintPage(int page)
{

    wxDC *trgDc = GetDC();
    if ((trgDc) && (page == 1)) {
        wxCoord m_prnX, m_prnY;
        int marginX = 50;
        int marginY = 50;
        trgDc->GetSize(&m_prnX, &m_prnY);

        m_prnX -= (2*marginX);
        m_prnY -= (2*marginY);
        trgDc->SetDeviceOrigin(marginX, marginY);

#ifdef MATHPLOT_DO_LOGGING
        wxLogMessage(wxT("Print Size: %d x %d\n"), m_prnX, m_prnY);
        wxLogMessage(wxT("Screen Size: %d x %d\n"), plotWindow->GetScrX(), plotWindow->GetScrY());
#endif

	// Set the scale according to the page:
        plotWindow->Fit(
                        plotWindow->GetDesiredXmin(),
                        plotWindow->GetDesiredXmax(),
                        plotWindow->GetDesiredYmin(),
                        plotWindow->GetDesiredYmax(),
                        &m_prnX,
                        &m_prnY );


		// Get the colours of the plotWindow to restore them ath the end
		wxColour oldBgColour = plotWindow->GetBackgroundColour();
		wxColour oldFgColour = plotWindow->GetForegroundColour();
		wxColour oldAxColour = plotWindow->GetAxesColour();

        // Draw background, ensuring to use white background for printing.
        trgDc->SetPen( *wxTRANSPARENT_PEN );
        // wxBrush brush( plotWindow->GetBackgroundColour() );
		wxBrush brush = *wxWHITE_BRUSH;
        trgDc->SetBrush( brush );
        trgDc->DrawRectangle(0,0,m_prnX,m_prnY);

        // Draw all the layers:
        //trgDc->SetDeviceOrigin( m_prnX>>1, m_prnY>>1);  // Origin at the center
        mpLayer *layer;
        for (unsigned int li = 0; li < plotWindow->CountAllLayers(); li++) {
            layer = plotWindow->GetLayer(li);
            layer->Plot(*trgDc, *plotWindow);
        };
        // Restore device origin
        // trgDc->SetDeviceOrigin(0, 0);
		// Restore colours
		plotWindow->SetColourTheme(oldBgColour, oldFgColour, oldAxColour);
        // Restore drawing
        plotWindow->Fit(plotWindow->GetDesiredXmin(), plotWindow->GetDesiredXmax(), plotWindow->GetDesiredYmin(), plotWindow->GetDesiredYmax(), NULL, NULL);
        plotWindow->UpdateAll();
    }
    return true;
}

bool mpPrintout::HasPage(int page)
{
    return (page == 1);
}


//-----------------------------------------------------------------------------
// mpMovableObject - provided by Jose Luis Blanco
//-----------------------------------------------------------------------------
void mpMovableObject::TranslatePoint( double x,double y, double &out_x, double &out_y )
{
    double ccos = cos( m_reference_phi );  // Avoid computing cos/sin twice.
    double csin = sin( m_reference_phi );

    out_x = m_reference_x + ccos * x - csin * y;
    out_y = m_reference_y + csin * x + ccos * y;
}

// This method updates the buffers m_trans_shape_xs/ys, and the precomputed bounding box.
void mpMovableObject::ShapeUpdated()
{
    // Just in case...
    if (m_shape_xs.size()!=m_shape_ys.size())
    {
        wxLogError(wxT("[mpMovableObject::ShapeUpdated] Error, m_shape_xs and m_shape_ys have different lengths!"));
    }
    else
    {
        double ccos = cos( m_reference_phi );  // Avoid computing cos/sin twice.
        double csin = sin( m_reference_phi );

        m_trans_shape_xs.resize(m_shape_xs.size());
        m_trans_shape_ys.resize(m_shape_xs.size());

        std::vector<double>::iterator itXi, itXo;
        std::vector<double>::iterator itYi, itYo;

        m_bbox_min_x=1e300;
        m_bbox_max_x=-1e300;
        m_bbox_min_y=1e300;
        m_bbox_max_y=-1e300;

        for (itXo=m_trans_shape_xs.begin(),itYo=m_trans_shape_ys.begin(),itXi=m_shape_xs.begin(),itYi=m_shape_ys.begin();
              itXo!=m_trans_shape_xs.end(); itXo++,itYo++,itXi++,itYi++)
        {
            *itXo = m_reference_x + ccos * (*itXi) - csin * (*itYi);
            *itYo = m_reference_y + csin * (*itXi) + ccos * (*itYi);

            // Keep BBox:
            if (*itXo < m_bbox_min_x) m_bbox_min_x = *itXo;
            if (*itXo > m_bbox_max_x) m_bbox_max_x = *itXo;
            if (*itYo < m_bbox_min_y) m_bbox_min_y = *itYo;
            if (*itYo > m_bbox_max_y) m_bbox_max_y = *itYo;
        }
    }
}

void mpMovableObject::Plot(wxDC & dc, mpWindow & w)
{
	if (m_visible) {
		dc.SetPen( m_pen);


		std::vector<double>::iterator  itX=m_trans_shape_xs.begin();
		std::vector<double>::iterator  itY=m_trans_shape_ys.begin();

		if (!m_continuous)
		{
			// for some reason DrawPoint does not use the current pen,
			// so we use DrawLine for fat pens
			if (m_pen.GetWidth() <= 1)
			{
				while (itX!=m_trans_shape_xs.end())
				{
					dc.DrawPoint( w.x2p(*(itX++)), w.y2p( *(itY++) ) );
				}
			}
			else
			{
				while (itX!=m_trans_shape_xs.end())
				{
					wxCoord cx = w.x2p(*(itX++));
					wxCoord cy = w.y2p(*(itY++));
					dc.DrawLine(cx, cy, cx, cy);
				}
			}
		}
		else
		{
			wxCoord cx0=0,cy0=0;
			bool    first = TRUE;
			while (itX!=m_trans_shape_xs.end())
			{
				wxCoord cx = w.x2p(*(itX++));
				wxCoord cy = w.y2p(*(itY++));
				if (first)
				{
					first=FALSE;
					cx0=cx;cy0=cy;
				}
				dc.DrawLine(cx0, cy0, cx, cy);
				cx0=cx; cy0=cy;
			}
		}

		if (!m_name.IsEmpty() && m_showName)
		{
			dc.SetFont(m_font);

			wxCoord tx, ty;
			dc.GetTextExtent(m_name, &tx, &ty);

			if (HasBBox())
			{
				wxCoord sx = (wxCoord) (( m_bbox_max_x - w.GetPosX()) * w.GetScaleX());
				wxCoord sy = (wxCoord) ((w.GetPosY() - m_bbox_max_y ) * w.GetScaleY());

				tx = sx - tx - 8;
				ty = sy - 8 - ty;
			}
			else
			{
				const int sx = w.GetScrX()>>1;
				const int sy = w.GetScrY()>>1;

				if ((m_flags & mpALIGNMASK) == mpALIGN_NE)
				{
					tx = sx - tx - 8;
					ty = -sy + 8;
				}
				else if ((m_flags & mpALIGNMASK) == mpALIGN_NW)
				{
					tx = -sx + 8;
					ty = -sy + 8;
				}
				else if ((m_flags & mpALIGNMASK) == mpALIGN_SW)
				{
					tx = -sx + 8;
					ty = sy - 8 - ty;
				}
				else
				{
					tx = sx - tx - 8;
					ty = sy - 8 - ty;
				}
			}

			dc.DrawText( m_name, tx, ty);
		}
	}
}

//-----------------------------------------------------------------------------
// mpCovarianceEllipse - provided by Jose Luis Blanco
//-----------------------------------------------------------------------------

// Called to update the m_shape_xs, m_shape_ys vectors, whenever a parameter changes.
void mpCovarianceEllipse::RecalculateShape()
{
    m_shape_xs.clear();
    m_shape_ys.clear();

    // Preliminar checks:
    if (m_quantiles<0)  { wxLogError(wxT("[mpCovarianceEllipse] Error: quantiles must be non-negative")); return; }
    if (m_cov_00<0)     { wxLogError(wxT("[mpCovarianceEllipse] Error: cov(0,0) must be non-negative")); return; }
    if (m_cov_11<0)     { wxLogError(wxT("[mpCovarianceEllipse] Error: cov(1,1) must be non-negative")); return; }

    m_shape_xs.resize( m_segments,0 );
    m_shape_ys.resize( m_segments,0 );

    // Compute the two eigenvalues of the covariance:
    // -------------------------------------------------
    double b = -m_cov_00 - m_cov_11;
    double c = m_cov_00*m_cov_11 - m_cov_01*m_cov_01;

    double D = b*b - 4*c;

    if (D<0)     { wxLogError(wxT("[mpCovarianceEllipse] Error: cov is not positive definite")); return; }

    double eigenVal0 =0.5*( -b + sqrt(D) );
    double eigenVal1 =0.5*( -b - sqrt(D) );

    // Compute the two corresponding eigenvectors:
    // -------------------------------------------------
    double  eigenVec0_x,eigenVec0_y;
    double  eigenVec1_x,eigenVec1_y;

    if (fabs(eigenVal0 - m_cov_00)>1e-6)
    {
        double k1x = m_cov_01 / ( eigenVal0 - m_cov_00 );
        eigenVec0_y = 1;
        eigenVec0_x = eigenVec0_y * k1x;
    }
    else
    {
        double k1y = m_cov_01 / ( eigenVal0 - m_cov_11 );
        eigenVec0_x = 1;
        eigenVec0_y = eigenVec0_x * k1y;
    }

    if (fabs(eigenVal1 - m_cov_00)>1e-6)
    {
        double k2x = m_cov_01 / ( eigenVal1 - m_cov_00 );
        eigenVec1_y = 1;
        eigenVec1_x = eigenVec1_y * k2x;
    }
    else
    {
        double k2y = m_cov_01 / ( eigenVal1 - m_cov_11 );
        eigenVec1_x = 1;
        eigenVec1_y = eigenVec1_x * k2y;
    }

    // Normalize the eigenvectors:
    double len = sqrt( eigenVec0_x*eigenVec0_x + eigenVec0_y*eigenVec0_y );
    eigenVec0_x /= len;  // It *CANNOT* be zero
    eigenVec0_y /= len;

    len = sqrt( eigenVec1_x*eigenVec1_x + eigenVec1_y*eigenVec1_y );
    eigenVec1_x /= len;  // It *CANNOT* be zero
    eigenVec1_y /= len;


    // Take the sqrt of the eigenvalues (required for the ellipse scale):
    eigenVal0 = sqrt(eigenVal0);
    eigenVal1 = sqrt(eigenVal1);

    // Compute the 2x2 matrix M = diag(eigVal) * (~eigVec)  (each eigen vector is a row):
    double M_00 = eigenVec0_x * eigenVal0;
    double M_01 = eigenVec0_y * eigenVal0;

    double M_10 = eigenVec1_x * eigenVal1;
    double M_11 = eigenVec1_y * eigenVal1;

    // The points of the 2D ellipse:
    double ang;
    double Aang = 6.283185308/(m_segments-1);
    int    i;
    for (i=0,ang=0;i<m_segments;i++,ang+= Aang )
    {
        double ccos = cos(ang);
        double csin = sin(ang);

        m_shape_xs[i] = m_quantiles * (ccos * M_00 + csin * M_10 );
        m_shape_ys[i] = m_quantiles * (ccos * M_01 + csin * M_11 );
    } // end for points on ellipse


    ShapeUpdated();
}

//-----------------------------------------------------------------------------
// mpPolygon - provided by Jose Luis Blanco
//-----------------------------------------------------------------------------
void mpPolygon::setPoints(
    const std::vector<double>&  points_xs,
    const std::vector<double>&  points_ys,
    bool                        closedShape )
{
    if ( points_xs.size()!=points_ys.size() )
    {
        wxLogError(wxT("[mpPolygon] Error: points_xs and points_ys must have the same number of elements"));
    }
    else
    {
        m_shape_xs = points_xs;
        m_shape_ys = points_ys;

        if ( closedShape && points_xs.size())
        {
            m_shape_xs.push_back( points_xs[0] );
            m_shape_ys.push_back( points_ys[0] );
        }

        ShapeUpdated();
    }
}

//-----------------------------------------------------------------------------
// mpBitmapLayer - provided by Jose Luis Blanco
//-----------------------------------------------------------------------------
void mpBitmapLayer::GetBitmapCopy( wxImage &outBmp ) const
{
    if (m_validImg)
        outBmp = m_bitmap;
}

void mpBitmapLayer::SetBitmap( const wxImage &inBmp, double x, double y, double lx, double ly )
{
    if (!inBmp.Ok())
    {
        wxLogError(wxT("[mpBitmapLayer] Assigned bitmap is not Ok()!"));
    }
    else
    {
        m_bitmap = inBmp; //.GetSubBitmap( wxRect(0, 0, inBmp.GetWidth(), inBmp.GetHeight()));
        m_min_x = x;
        m_min_y = y;
        m_max_x = x+lx;
        m_max_y = y+ly;
        m_validImg = true;
    }
}


void mpBitmapLayer::Plot(wxDC & dc, mpWindow & w)
{
    if (m_visible && m_validImg)
    {
	/*	1st: We compute (x0,y0)-(x1,y1), the pixel coordinates of the real outer limits
		     of the image rectangle within the (screen) mpWindow. Note that these coordinates
		     might fall well far away from the real view limits when the user zoom in.

		2nd: We compute (dx0,dy0)-(dx1,dy1), the pixel coordinates the rectangle that will
		     be actually drawn into the mpWindow, i.e. the clipped real rectangle that
		     avoids the non-visible parts. (offset_x,offset_y) are the pixel coordinates
		     that correspond to the window point (dx0,dy0) within the image "m_bitmap", and
		     (b_width,b_height) is the size of the bitmap patch that will be drawn.

	(x0,y0) .................  (x1,y0)
	    .                          .
	    .                          .
	(x0,y1) ................   (x1,y1)
                  (In pixels!!)
	*/

	// 1st step -------------------------------
        wxCoord x0 = w.x2p(m_min_x);
        wxCoord y0 = w.y2p(m_max_y);
        wxCoord x1 = w.x2p(m_max_x);
        wxCoord y1 = w.y2p(m_min_y);

	// 2nd step -------------------------------
	// Precompute the size of the actual bitmap pixel on the screen (e.g. will be >1 if zoomed in)
	double screenPixelX = ( x1-x0 ) / (double)m_bitmap.GetWidth();
	double screenPixelY = ( y1-y0 ) / (double)m_bitmap.GetHeight();

	// The minimum number of pixels that the streched image will overpass the actual mpWindow borders:
	wxCoord borderMarginX = (wxCoord)(screenPixelX+1); // ceil
	wxCoord borderMarginY = (wxCoord)(screenPixelY+1); // ceil

	// The actual drawn rectangle (dx0,dy0)-(dx1,dy1) is (x0,y0)-(x1,y1) clipped:
	wxCoord dx0=x0,dx1=x1,dy0=y0,dy1=y1;
	if (dx0<0) dx0=-borderMarginX;
	if (dy0<0) dy0=-borderMarginY;
	if (dx1>w.GetScrX()) dx1=w.GetScrX()+borderMarginX;
	if (dy1>w.GetScrY()) dy1=w.GetScrY()+borderMarginY;

	// For convenience, compute the width/height of the rectangle to be actually drawn:
	wxCoord d_width = dx1-dx0+1;
	wxCoord d_height = dy1-dy0+1;

	// Compute the pixel offsets in the internally stored bitmap:
	wxCoord offset_x= (wxCoord) ( (dx0-x0)/screenPixelX );
	wxCoord offset_y= (wxCoord) ( (dy0-y0)/screenPixelY );

	// and the size in pixel of the area to be actually drawn from the internally stored bitmap:
	wxCoord b_width  = (wxCoord) ( (dx1-dx0+1)/screenPixelX );
	wxCoord b_height = (wxCoord) ( (dy1-dy0+1)/screenPixelY );

	#ifdef MATHPLOT_DO_LOGGING
		wxLogMessage(_("[mpBitmapLayer::Plot] screenPixel: x=%f y=%f  d_width=%ix%i"),screenPixelX,screenPixelY,d_width,d_height);
		wxLogMessage(_("[mpBitmapLayer::Plot] offset: x=%i y=%i  bmpWidth=%ix%i"),offset_x,offset_y,b_width,b_height);
	#endif

	// Is there any visible region?
	if (d_width>0 && d_height>0)
	{
		// Build the scaled bitmap from the image, only if it has changed:
		if (m_scaledBitmap.GetWidth()!=d_width ||
		    m_scaledBitmap.GetHeight()!=d_height ||
		    m_scaledBitmap_offset_x != offset_x ||
		    m_scaledBitmap_offset_y != offset_y  )
		{
			wxRect r(wxRect(offset_x,offset_y,b_width,b_height));
			// Just for the case....
			if (r.x<0) r.x=0;
			if (r.y<0) r.y=0;
			if (r.width>m_bitmap.GetWidth()) r.width=m_bitmap.GetWidth();
			if (r.height>m_bitmap.GetHeight()) r.height=m_bitmap.GetHeight();

			m_scaledBitmap = wxBitmap(
				wxBitmap(m_bitmap).GetSubBitmap( r ).ConvertToImage()
				.Scale(d_width,d_height) );
			m_scaledBitmap_offset_x = offset_x;
			m_scaledBitmap_offset_y = offset_y;
		}

		// Draw it:
		dc.DrawBitmap( m_scaledBitmap, dx0,dy0, true );
	}
    }

    // Draw the name label
    if (!m_name.IsEmpty() && m_showName)
    {
        dc.SetFont(m_font);

        wxCoord tx, ty;
        dc.GetTextExtent(m_name, &tx, &ty);

        if (HasBBox())
        {
            wxCoord sx = (wxCoord) (( m_max_x - w.GetPosX()) * w.GetScaleX());
            wxCoord sy = (wxCoord) ((w.GetPosY() - m_max_y ) * w.GetScaleY());

            tx = sx - tx - 8;
            ty = sy - 8 - ty;
        }
        else
        {
            const int sx = w.GetScrX()>>1;
            const int sy = w.GetScrY()>>1;

            if ((m_flags & mpALIGNMASK) == mpALIGN_NE)
            {
                tx = sx - tx - 8;
                ty = -sy + 8;
            }
            else if ((m_flags & mpALIGNMASK) == mpALIGN_NW)
            {
                tx = -sx + 8;
                ty = -sy + 8;
            }
            else if ((m_flags & mpALIGNMASK) == mpALIGN_SW)
            {
                tx = -sx + 8;
                ty = sy - 8 - ty;
            }
            else
            {
                tx = sx - tx - 8;
                ty = sy - 8 - ty;
            }
        }

        dc.DrawText( m_name, tx, ty);
    }
}


/**
    mpPointLayer by Jussi Vatjus-Anttila
    09.2009
*/
IMPLEMENT_DYNAMIC_CLASS(mpPointLayer, mpLayer)

mpPointLayer::mpPointLayer(mpLayer *parent)
{
    m_brush = *wxTRANSPARENT_BRUSH;
    mpParent = parent;
    m_EnableModify = false;
    m_defined = false;
    m_SecondPointLayer = 0;
}

mpPointLayer::mpPointLayer( mpLayer         *mplayer,
                            wxString        layerName,
                            const wxBrush   *brush,
                            wxWindow        *wxparent,
                            long int        eventid
                        )
{
    m_name = layerName;
    this->ShowName(true);
    SetBrush(*brush);
    m_type = mpLayer_POINT;
    m_visible = true;
    //this->SetDrawingOrder(0);

    mpParent = mplayer;
    m_EnableModify = true;
    m_DragOnlyPoint = true;
    m_defined = false;

    wxParent = wxparent;
    eventId = eventid;
    m_drawOutsideMargins = false;

    m_shape = mpCIRCLE;
    m_diagonal = 7;
    m_taggline = 0;

	m_ShowShadows = 1;
	m_tooltip = 0;
	m_externalObject = 0;
	m_SecondPointLayer = 0;

    m_taggPen = wxPen( *wxBLACK, 1, wxPENSTYLE_DOT); //default taggpen

    int idd;
    wxMenu *submenu = new wxMenu();
        submenu->Append( idd=wxNewId(),  _("Disable point"),  _("") );
        //Connect( idd,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&mpPointLayer::OnMenuItemGraphLockEvent);
    m_popmenu.Append(wxNewId(),  _("Edit"), submenu, _("Edit") );

    //Call from parent:
    //Connect(eventid, wxEVT_MOTION, eventfunction)  called when mouse move
    //Connect(eventid, wxEVT_LEFT_UP, eventfunction) called when mouse left up.
}
mpPointLayer::~mpPointLayer()
{
}
void mpPointLayer::SetSecondPoint(mpPointLayer* layer)
{
    m_SecondPointLayer = layer;
}

/**
*  The function will return true if the point x,y is inside the polygon, or
*  NO if it is not.  If the point is exactly on the edge of the polygon,
*  then the function may return true or false.
*
*  @param x   Xpoint to be tested
*  @param y   Ypoint to be tested
*  @return true if inside, other false
*
*  Note that division by zero is avoided because the division is protected
*  by the "if" clause which surrounds it.
*  http://alienryderflex.com/polygon/
*/
bool mpPointLayer::pointInPolygon(int x, int y)
{
    int polySides = m_polygon.size();

    int     i, j=polySides-1;
    bool    oddNodes=false;

    for (i=0; i<polySides; i++)
    {
        wxPoint polygoni = *(wxPoint*)m_polygon.Item(i)->GetData();
        wxPoint polygonj = *(wxPoint*)m_polygon.Item(j)->GetData();
        if ((polygoni.y<y && polygonj.y >= y) || (polygonj.y<y && polygoni.y>=y)) {
            if (polygoni.x+(y-polygoni.y)/(polygonj.y-polygoni.y)*(polygonj.x-polygoni.x)<x) {
                oddNodes=!oddNodes;
            }
        }
        j=i;
    }
    return oddNodes;
}
void mpPointLayer::UpdatePoint(mpWindow& w, wxEvent& event)
{

    int xi = ((wxMouseEvent&)event).GetX();
    int yi = ((wxMouseEvent&)event).GetY();

    if(m_DragOnlyPoint) //if automatic modify enable.
    {
        if( ((wxMouseEvent&)event).m_leftDown )
        {
            if( pointInPolygon(xi, yi) || !m_defined )
                m_cursorInside = true;

        }
        else m_cursorInside=false;
    }

    //be sure update is allowed by enduser
    if((m_cursorInside||!m_DragOnlyPoint)&&m_EnableModify)
    {

        double  xd = w.p2x( xi ),
                yd = w.p2y( yi );

        //if there is graph in parent layer get coordinate
        if(mpParent->GetNearestCoord(w, xd,yd))
        {

            SetPosition(xd,yd);
            //coordinate founded

            if(wxParent) //if wxParent is set, post event to it
            {
                event.SetEventType(wxEVT_LEFT_UP);
                event.SetId(eventId);
                wxPostEvent( wxParent, event );
            }

            /*
            wxString toolTipContent;
            toolTipContent.Printf(_("X = %f\nY = %f"), xd, yd);
            wxRect rectBounds(w.x2p(xd),w.x2p(yd), 5, 5);
            if(!m_tooltip) m_tooltip = new wxTipWindow(&w, toolTipContent, 100, NULL, &rectBounds);
            else
            {
                m_tooltip->SetBoundingRect(rectBounds);
            }*/

        }
        else
        {
            m_defined=false;
        }

    }
}
void mpPointLayer::UpdateMouseCoord(mpWindow& w, wxEvent& event)
{
    //event type is valid, but is it inside diagram
    int xi = ((wxMouseEvent&)event).GetX();
    int yi = ((wxMouseEvent&)event).GetY();
    double  xd = w.p2x( xi ),
            yd = w.p2y( yi );

    //if there is graph in parent layer get coordinate
    if(mpParent->GetNearestCoord(w, xd,yd))
    {
        mouseX=xd;
        mouseY=yd;
        m_mouseDefined=true;
        //m_cursorInside = pointInPolygon(xi,yi);

        if(wxParent)
        {
            event.SetEventType(wxEVT_MOTION);
            event.SetId(eventId);
            wxPostEvent( wxParent, event );
        }
    }
    else m_mouseDefined=false;
}
bool mpPointLayer::GetPosition(double &x, double &y)
{
    if(m_defined)
    {
        //if position is set
        x = m_x; y = m_y;
        return true;
    }
    return false;
}
bool mpPointLayer::SetPosition(double & x, double & y)
{
    m_x = x; m_y = y;
    m_defined =true;
    return false;
}
void mpPointLayer::UnDefine()
{
    m_defined =false;
}
void mpPointLayer::SetShape(mpShape shape)
{
    m_shape = shape;
}
void mpPointLayer::SetShapeSize(int diagonal)
{
    m_diagonal = diagonal;
}
bool mpPointLayer::GetCursorPosition(double &x, double &y)
{
    if(m_mouseDefined)
    {
        x = mouseX;
        y = mouseY;
        return true;
    }
    return false;
}
void mpPointLayer::Plot(wxDC & dc, mpWindow & w)
{
    /**
     * polygon shape is used, as the location of
     * the mouse can be easily identified inside a polygon
     *    bool Inside(wxPoint)
     *    wxPoint is wxDC coordinate,
     *    example from wxMouseEvent
     *
     *  m_polygon is wxDC coordiante
     */
    if(this->m_visible && m_defined)
    {
        wxCoord startPx = m_drawOutsideMargins ? 0 : w.GetMarginLeft();
		wxCoord endPx   = m_drawOutsideMargins ? w.GetScrX() : w.GetScrX() - w.GetMarginRight();
		wxCoord minYpx  = m_drawOutsideMargins ? 0 : w.GetMarginTop();
		wxCoord maxYpx  = m_drawOutsideMargins ? w.GetScrY() : w.GetScrY() - w.GetMarginBottom();


        wxPoint coord(w.x2p(m_x), w.y2p(m_y));
        dc.SetBrush(m_brush);
        dc.SetPen(m_pen);

        m_polygon.clear();
        //m_polygon.DeleteContents(true);

        /*
        if(coord.x < startPx) return;
        if(coord.x > endPx) return;
        if(coord.y < minYpx) return;
        if(coord.y > maxYpx) return;
        */

        if(m_shape == mpCIRCLE)
        {
            for(double t=0;t<2*M_PI; t+=0.4)
            {
                m_polygon.Append(new wxPoint(coord.x+ (int)(m_diagonal*cos(t)), (int)(coord.y + m_diagonal*sin(t))));

            }
        }
        else if(m_shape == mpSQUARE)
        {
            m_polygon.Append(new wxPoint(coord.x- m_diagonal, coord.y - m_diagonal));
            m_polygon.Append(new wxPoint(coord.x- m_diagonal, coord.y + m_diagonal));
            m_polygon.Append(new wxPoint(coord.x+ m_diagonal, coord.y + m_diagonal));
            m_polygon.Append(new wxPoint(coord.x+ m_diagonal, coord.y - m_diagonal));
            m_polygon.Append(new wxPoint(coord.x- m_diagonal, coord.y - m_diagonal));

        }
        else if(m_shape == mpDIAMOND)
        {
            m_polygon.Append(new wxPoint(coord.x, coord.y - m_diagonal));
            m_polygon.Append(new wxPoint(coord.x- m_diagonal, coord.y));
            m_polygon.Append(new wxPoint(coord.x, coord.y + m_diagonal));
            m_polygon.Append(new wxPoint(coord.x+ m_diagonal, coord.y));
            m_polygon.Append(new wxPoint(coord.x, coord.y - m_diagonal));
        }
        else if(m_shape == mpELLIPSE)
        {
            for(double t=0;t<2*M_PI; t+=0.4)
            {
                m_polygon.Append(new wxPoint(coord.x+ (int)(m_diagonal*cos(t)), (int)(coord.y + 2*m_diagonal*sin(t))));
            }
        }
        else if(m_shape == mpARROW_UP)
        {
            m_polygon.Append(new wxPoint(coord.x, coord.y));
            m_polygon.Append(new wxPoint(coord.x- m_diagonal, coord.y + m_diagonal));
            m_polygon.Append(new wxPoint(coord.x+ m_diagonal, coord.y + m_diagonal));
            m_polygon.Append(new wxPoint(coord.x, coord.y));
        }
        else if(m_shape == mpARROW_DOWN)
        {
            m_polygon.Append(new wxPoint(coord.x, coord.y));
            m_polygon.Append(new wxPoint(coord.x- m_diagonal, coord.y - m_diagonal));
            m_polygon.Append(new wxPoint(coord.x+ m_diagonal, coord.y - m_diagonal));
            m_polygon.Append(new wxPoint(coord.x, coord.y));
        }
        else if(m_shape == mpARROW_UPDOWN)
        {
            //up arrow
            m_polygon.Append(new wxPoint(coord.x, coord.y));
            m_polygon.Append(new wxPoint(coord.x- m_diagonal, coord.y + m_diagonal));
            m_polygon.Append(new wxPoint(coord.x+ m_diagonal, coord.y + m_diagonal));
            m_polygon.Append(new wxPoint(coord.x, coord.y));
            //down arrow
            m_polygon.Append(new wxPoint(coord.x, coord.y));
            m_polygon.Append(new wxPoint(coord.x- m_diagonal, coord.y - m_diagonal));
            m_polygon.Append(new wxPoint(coord.x+ m_diagonal, coord.y - m_diagonal));
            m_polygon.Append(new wxPoint(coord.x, coord.y));
        }

        dc.SetPen( m_pen);

        #if wxCHECK_VERSION(3, 0, 0)
            // 2.9< -> void wxDC::DrawPolygon	(	const wxPointList *points,wxCoord 	xoffset = 0,wxCoord 	yoffset = 0,wxPolygonFillMode 	fill_style = wxODDEVEN_RULE )
            dc.DrawPolygon((wxPointList*)&m_polygon);
        #elif wxCHECK_VERSION(2, 8, 0)
            // 2.8.x -> void DrawPolygon(wxList *points, wxCoord xoffset = 0, wxCoord yoffset = 0, int fill_style = wxODDEVEN_RULE)
            dc.DrawPolygon((wxList*)&m_polygon);
        #else
            #warning NOT SUPPOTED
        #endif


        if(m_taggline && coord.x > startPx && coord.x < endPx)
        {
            // wxDOT_DASH / wxDOT / wxSHORT_DASH
            dc.SetPen( m_taggPen );

            if(m_taggline&mpVERTICAL_LINE)
            {   //draw vertical line over the window
                dc.DrawLine(coord.x, minYpx, coord.x, maxYpx);
            }
            if(m_taggline&mpHORIZONTAL_LINE)
            {   //draw vertical line over the window
                dc.DrawLine(startPx, coord.y, endPx, coord.y);
            }
            if(m_taggline&mpCROSS_LINES)
            {   //draw vertical line over the window
                dc.DrawLine(coord.x-50, coord.y+50, coord.x+50, coord.y-50);
                dc.DrawLine(coord.x-50, coord.y-50, coord.x+50, coord.y+50);
            }
            dc.SetPen(m_pen);
        }


        if(m_SecondPointLayer)
		{
		    double x,y;
		    int px, py;
            if(m_SecondPointLayer->GetPosition(x,y))
            {
                wxPoint coord2(w.x2p(x), w.y2p(y));
                wxPen tmp(*wxBLUE, 10);
                if(x < m_x)
                    tmp.SetColour( *wxRED );

                if(coord.x < startPx) coord.x = startPx;
                else if(coord.x > endPx) coord.x = endPx;

                if(coord2.x < startPx) coord2.x = startPx;
                else if(coord2.x > endPx) coord2.x = endPx;

                dc.SetPen( tmp );
                dc.DrawLine(coord.x, maxYpx, coord2.x, maxYpx);

                dc.SetPen( m_pen);

                //if show shadows..
                // drawn between the points reflecting the average of the shading
                if(!m_externalObject && m_ShowShadows && m_x < x) //show shading only when P1 < P2
                {
                    if(m_ShowShadows == 1)
                    {
                        //draw bar, wich are y value height
                        bool ok=false;
                        double x2, y2, x0=m_x;
                        int width;
                        dynamic_cast<mpFXY*>(mpParent)->Rewind();
                        while( dynamic_cast<mpFXY*>(mpParent)->GetNextXY(x2,y2 ) )
                        {
                            if(!ok)
                            {
                                if(x2==m_x)ok=true; //first
                                else continue;
                            }
                            px = w.x2p(x2);
                            py = w.y2p(y2);
                            width = py - w.x2p(x0);

                            dc.GradientFillLinear( wxRect( px, py, width, maxYpx - py),
                                                  wxColour(150,150,200),
                                                  wxColour(255,255,255),
                                                  wxDirection(wxDOWN) );
                            x0 = x2;

                            if(x2==x)break;

                        }
                    }
                    if(m_ShowShadows == 2)
                    {
                        bool ok=false;
                        double x2, y2;
                        double sumy=0;
                        double n=0;
                        dynamic_cast<mpFXY*>(mpParent)->Rewind();
                        while( dynamic_cast<mpFXY*>(mpParent)->GetNextXY(x2,y2 ) )
                        {
                            if(!ok)
                            {
                                if(x2==m_x)ok=true; //first
                                else continue;
                            }
                            sumy += y2;
                            n++;

                            if(x2==x)break;
                        }
                        y2 = sumy/n;

                        px = w.x2p(m_x);
                        if(px<startPx)px=startPx;
                        if(px>endPx)px=endPx;
                        py = w.y2p(y2);
                        int width = w.x2p(x2) - px;
                        dc.GradientFillLinear( wxRect( px,py , width, maxYpx - py),
                                                  wxColour(150,150,200),
                                                  wxColour(255,255,255),
                                                  wxDirection(wxDOWN) );

                    }
                }

                if (m_externalObject)
                {
                    m_externalObject->customPlot(dc, w, m_x, m_y, x, y, coord, coord2, maxYpx);
                }
            }
		}


		if (!m_name.IsEmpty() && m_showName)
		{
		    dc.SetFont(m_font);
			dc.DrawText( GetName(), coord.x-5, coord.y+20);
		}

    }
}


mpXYArea::mpXYArea(wxString name, int flags)
{
	SetName(name);
	m_flags = flags;
	m_type = mpLAYER_PLOT;
	m_markCorners = false;
	m_drawOutsideMargins = false;
	m_cornerMarkSize = 3;
}

void mpXYArea::Plot(wxDC & dc, mpWindow & w)
{
	if (m_visible) 
	{
		dc.SetPen(m_pen);
		dc.SetBrush(m_brush);
		double x, y;
		wxCoord startPx = m_drawOutsideMargins ? 0 : w.GetMarginLeft();
		wxCoord endPx = m_drawOutsideMargins ? w.GetScrX() : w.GetScrX() - w.GetMarginRight();
		wxCoord minYpx = m_drawOutsideMargins ? 0 : w.GetMarginTop();
		wxCoord maxYpx = m_drawOutsideMargins ? w.GetScrY() : w.GetScrY() - w.GetMarginBottom();
		wxCoord ix = 0, iy = 0;
		Rewind();
		CurrentBounds(w.p2x(startPx), w.p2x(endPx));


		if (!m_continuous)
		{
			// for some reason DrawPoint does not use the current pen,
			// so we use DrawLine for fat pens
			if (m_pen.GetWidth() <= 1)
			{
				while (GetNextXY(x, y))
				{
					ix = w.x2p(x);
					iy = w.y2p(y);
					if (m_drawOutsideMargins || ((ix >= startPx) && (ix <= endPx) && (iy >= minYpx) && (iy <= maxYpx)))
					{
						switch (m_pointType)
						{	//
						case mpCIRCLE:
							dc.DrawCircle(ix, iy, 2);
							break;
						case mpRECT:
							dc.DrawRectangle(ix, iy, m_RectSize.x, m_RectSize.y);
							break;
						case mpPOINT:
							dc.DrawPoint(ix, iy);
							break;
						default: break;
						}
						UpdateViewBoundary(ix, iy);
						if (m_markCorners)
						{
							dc.DrawCircle(ix, iy, m_cornerMarkSize);
						}
					}
				}
			}
			else
			{
				while (GetNextXY(x, y))
				{
					ix = w.x2p(x);
					iy = w.y2p(y);
					if (m_drawOutsideMargins || ((ix >= startPx) && (ix <= endPx) && (iy >= minYpx) && (iy <= maxYpx)))
					{
						switch (m_pointType)
						{	//
						case mpCIRCLE:
						{
							dc.DrawCircle(ix, iy, 2);
							break;
						}
						case mpRECT:
						{
							dc.DrawRectangle(ix, iy, m_RectSize.x, m_RectSize.y);
							break;
						}
						case mpPOINT:
						{
							dc.DrawLine(ix, iy, ix, iy);
							break;
						}
						default: break;
						}

						if (m_markCorners)
						{
							dc.DrawCircle(ix, iy, m_cornerMarkSize);
						}
					}
				}
			}
		}
		else
		{
			//wxPoint pts[4];
			std::vector<wxPoint> pts;
			// Old code
			wxCoord x0 = 0, c0 = 0,
				*nextX0 = 0,
				*nextY0 = 0;
			bool    first = TRUE;
			while (GetNextXY(x, y))
			{
				wxCoord x1 = w.x2p(x); // (wxCoord) ((x - w.GetPosX()) * w.GetScaleX());
				wxCoord c1 = w.y2p(y); // (wxCoord) ((w.GetPosY() - y) * w.GetScaleY());
				if (first)
				{
					first = FALSE;
					x0 = x1; c0 = c1;
					pts.push_back(wxPoint(x0, w.y2p(GetMaxY())));
					pts.push_back(wxPoint(x0, w.y2p(GetMaxY()) - c0 + GetMinY()));
				}
				bool outUp, outDown;
				if ((x1 >= startPx) && (x0 <= endPx)) {
					outDown = (c0 > maxYpx) && (c1 > maxYpx);   // check that at least one of point is inside borders
					outUp = (c0 < minYpx) && (c1 < minYpx);
					if (!outUp && !outDown) {


						// this could comment in more detail..
						/// @bug There is some bugs!
						if (c1 != c0) {
							if (c0 < minYpx) { //y0 < minY
								x0 = (int)(((double)(minYpx - c0)) / ((double)(c1 - c0))*(x1 - x0)) + x0;
								c0 = minYpx;
							}
							if (c0 > maxYpx) { //y0 > maxY
								x0 = (int)(((double)(maxYpx - c0)) / ((double)(c1 - c0))*(double)(x1 - x0)) + x0;
								//wxLogDebug(wxT("old x0 = %d, new x0 = %d"), x0, newX0);
								c0 = maxYpx;
							}
							if (c1 < minYpx) {  //y1 > minY
								nextX0 = new wxCoord(x1);   //Save x1 value because it needed for next loop!
								nextY0 = new wxCoord(c1);   //Save y1 value because it needed for next loop!
								x1 = (int)(((double)(minYpx - c0)) / ((double)(c1 - c0))*(x1 - x0)) + x0;
								c1 = minYpx;
							}
							if (c1 > maxYpx) { //y1 > maxY
								nextX0 = new wxCoord(x1);   //Save x1 value because it needed for next loop!
								nextY0 = new wxCoord(c1);   //Save y1 value because it needed for next loop!

								x1 = (int)(((double)(maxYpx - c0)) / ((double)(c1 - c0))*(x1 - x0)) + x0;
								c1 = maxYpx;
							}

						}

						//fix left and right boundary in x axis
						if (x1 != x0) {
							if (x0 < startPx) {
								c0 = (int)(((double)(startPx - x0)) / ((double)(x1 - x0))*(c1 - c0)) + c0;
								x0 = startPx;
							}
							if (x1 > endPx) {
								c1 = (int)(((double)(endPx - x0)) / ((double)(x1 - x0))*(c1 - c0)) + c0;
								x1 = endPx;
							}
						}

						//dc.DrawLine(x0, c0, x1, c1);
						pts.push_back(wxPoint(x1, w.y2p(GetMinY()) - c1));
						UpdateViewBoundary(x1, c1);

						if (m_markCorners)
						{
							dc.DrawCircle(x1, c1, m_cornerMarkSize);
						}
					}
				}
				x0 = x1; c0 = c1;
				if (nextX0) { x0 = *nextX0; wxDELETE(nextX0); }
				if (nextY0) { c0 = *nextY0; wxDELETE(nextY0); }
			}
			pts.push_back(wxPoint(x0, w.y2p(GetMaxY())));
			dc.DrawPolygon(pts.size(), pts.data());
		}

		if (!m_name.IsEmpty() && m_showName)
		{
			dc.SetFont(m_font);

			wxCoord tx, ty;
			dc.GetTextExtent(m_name, &tx, &ty);

			// xxx implement else ... if (!HasBBox())
			{
				// const int sx = w.GetScrX();
				// const int sy = w.GetScrY();

				if ((m_flags & mpALIGNMASK) == mpALIGN_NW)
				{
					tx = minDrawX + 8;
					ty = maxDrawY + 8;
				}
				else if ((m_flags & mpALIGNMASK) == mpALIGN_NE)
				{
					tx = maxDrawX - tx - 8;
					ty = maxDrawY + 8;
				}
				else if ((m_flags & mpALIGNMASK) == mpALIGN_SE)
				{
					tx = maxDrawX - tx - 8;
					ty = minDrawY - ty - 8;
				}
				else
				{ // mpALIGN_SW
					tx = minDrawX + 8;
					ty = minDrawY - ty - 8;
				}
			}

			dc.DrawText(m_name, tx, ty);
		}
	}
}
