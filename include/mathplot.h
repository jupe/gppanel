/////////////////////////////////////////////////////////////////////////////
// Name:            mathplot.cpp
// Purpose:         Framework for plotting in wxWindows
// Original Author: David Schalig
// Maintainer:      Davide Rondini
// Contributors:    Jose Luis Blanco, Val Greene
// Created:         21/07/2003
// Last edit:       22/02/2009
// Copyright:       (c) David Schalig, Davide Rondini
// Licence:         wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _MP_MATHPLOT_H_
#define _MP_MATHPLOT_H_

/** @file mathplot.h */
/** @mainpage wxMathPlot
    wxMathPlot is a framework for mathematical graph plotting in wxWindows.

    The framework is designed for convenience and ease of use.

    @section screenshots Screenshots
    <a href="http://wxmathplot.sourceforge.net/screenshot.shtml">Go to the screenshots page.</a>

    @section overview Overview
    The heart of wxMathPlot is mpWindow, which is a 2D canvas for plot layers.
    mpWindow can be embedded as subwindow in a wxPane, a wxFrame, or any other wxWindow.
    mpWindow provides a zoomable and moveable view of the layers. The current view can
    be controlled with the mouse, the scrollbars, and a context menu.

    Plot layers are implementations of the abstract base class mpLayer. Those can
    be function plots, scale rulers, or any other vector data visualisation. wxMathPlot provides two mpLayer implementations for plotting horizontal and vertical rulers: mpScaleX and mpScaleY.
    For convenient function plotting a series of classes derived from mpLayer are provided, like mpFX, mpProfile, mpLegend and so on. These base classes already come with plot code, user's own functions can be implemented by overriding just one member for retrieving a function value.

    mpWindow has built-in support for mouse-based pan and zoom through intuitive combinations of buttons and the mouse wheel. It also incorporates an optional double buffering mechanism to avoid flicker. Plots can be easily sent to printer evices or exported in bitmap formats like PNG, BMP or JPEG.

    @section coding Coding conventions
    wxMathPlot sticks to wxWindow's coding conventions. All entities defined by wxMathPlot have the prefix <i>mp</i>.

    @section author Author and license
    wxMathPlot is published under the terms of the wxWindow license.<br>
    The original author is David Schalig <mrhill@users.sourceforge.net>.<br>
    From June 2007 the project is maintained by Davide Rondini <cdron77@users.sourceforge.net>.<br>
    Authors can be contacted via the wxMathPlot's homepage at
    https://sourceforge.net/projects/wxmathplot<br>
    Contributors:<br>
    Jose Luis Blanco, Val Greene.<br>
*/

//this definition uses windows dll to export function.
//WXDLLIMPEXP_MATHPLOT definition definition changed to WXDLLIMPEXP_MATHPLOT
//mathplot_EXPORTS will be defined by cmake
#ifdef mathplot_EXPORTS
 #define WXDLLIMPEXP_MATHPLOT WXEXPORT
 #define WXDLLIMPEXP_DATA_MATHPLOT(type) WXEXPORT type
#else // not making DLL
 #define WXDLLIMPEXP_MATHPLOT
 #define WXDLLIMPEXP_DATA_MATHPLOT(type) type
#endif

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "mathplot.h"
#endif

#include <vector>

// #include <wx/wx.h>
#include <wx/defs.h>
#include <wx/menu.h>
#include <wx/scrolwin.h>
#include <wx/event.h>
#include <wx/dynarray.h>
#include <wx/pen.h>
#include <wx/dcmemory.h>
#include <wx/string.h>
#include <wx/print.h>
#include <wx/image.h>

#include <wx/tipwin.h>
#include <wx/gdicmn.h>

#include <deque>
#include <map>

WX_DECLARE_LIST(wxPoint, PointList);
#include <wx/listimpl.cpp>


// For memory leak debug
#ifdef _WINDOWS
#ifdef _DEBUG
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK ,__FILE__, __LINE__)
#else
#define DEBUG_NEW new
#endif // _DEBUG
#endif // _WINDOWS

// Separation for axes when set close to border
#define X_BORDER_SEPARATION 40
#define Y_BORDER_SEPARATION 60

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_MATHPLOT mpLayer;
class WXDLLIMPEXP_MATHPLOT mpFX;
class WXDLLIMPEXP_MATHPLOT mpFY;
class WXDLLIMPEXP_MATHPLOT mpFXY;
class WXDLLIMPEXP_MATHPLOT mpFXYBar;
class WXDLLIMPEXP_MATHPLOT mpFYXBar;
class WXDLLIMPEXP_MATHPLOT mpFXYCandleStick;
class WXDLLIMPEXP_MATHPLOT mpFXYVector;
class WXDLLIMPEXP_MATHPLOT mpScaleX;
class WXDLLIMPEXP_MATHPLOT mpScaleY;
class WXDLLIMPEXP_MATHPLOT mpWindow;
class WXDLLIMPEXP_MATHPLOT mpText;
class WXDLLIMPEXP_MATHPLOT mpPrintout;
class WXDLLIMPEXP_MATHPLOT mpPointLayer;

/** Command IDs used by mpWindow */
enum
{
    mpID_FIT = 2000,    //!< Fit view to match bounding box of all layers
    mpID_ZOOM_IN,       //!< Zoom into view at clickposition / window center
    mpID_ZOOM_OUT,      //!< Zoom out
    mpID_CENTER,        //!< Center view on click position
    mpID_LOCKASPECT,    //!< Lock x/y scaling aspect
    mpID_GRID,
    mpID_HELP_MOUSE     //!< Shows information about the mouse commands
};
typedef enum __mp_Mouse_Zoom_Type {
     mpZOOM_XY          //!< zoom 2 dimensialy
    ,mpZOOM_X
    ,mpZOOM_Y
} mpMouseZoomType;
//-----------------------------------------------------------------------------
// mpLayer
//-----------------------------------------------------------------------------

typedef enum __mp_Layer_Type {
     mpLAYER_UNDEF      //!< Layer type undefined
    ,mpLAYER_AXIS       //!< Axis type layer
    ,mpLAYER_PLOT       //!< Plot type layer
    ,mpLAYER_INFO       //!< Info box type layer
    ,mpLAYER_BITMAP     //!< Bitmap type layer
    ,mpLayer_POINT      //!< Point layer
} mpLayerType;

enum mpShape
{
    mpCIRCLE
    ,mpRECT
    ,mpPOINT
    ,mpDIAMOND
    ,mpSQUARE
    ,mpELLIPSE
    ,mpARROW_UP
    ,mpARROW_DOWN
    ,mpARROW_UPDOWN
};

struct mpXY
{
    double x, y;
    mpXY(double _x, double _y){x=_x;y=_y;}
    mpXY(const mpXY& copy){x=copy.x;y=copy.y;}
};

/** Plot layer, abstract base class.
    Any number of mpLayer implementations can be attached to mpWindow.
    Examples for mpLayer implementations are function graphs, or scale rulers.

    For convenience mpLayer defines a name, a font (wxFont), a pen (wxPen),
    and a continuity property (bool) as class members.
    The default values at constructor are the default font, a black pen, and
     continuity set to false (draw separate points).
    These may or may not be used by implementations.
*/
class WXDLLIMPEXP_MATHPLOT mpLayer : public wxObject
{
public:
    mpLayer();

    virtual ~mpLayer() {};

    /** Check whether this layer has a bounding box.
        The default implementation returns \a TRUE. Override and return
        FALSE if your mpLayer implementation should be ignored by the calculation
        of the global bounding box for all layers in a mpWindow.
        @retval TRUE Has bounding box
        @retval FALSE Has not bounding box
    */
    virtual bool   HasBBox() { return TRUE; }

    /** Check whether the layer is an info box.
        The default implementation returns \a FALSE. It is overrided to \a TRUE for mpInfoLayer
        class and its derivative. It is necessary to define mouse actions behaviour over
        info boxes.
        @return whether the layer is an info boxes
        @sa mpInfoLayer::IsInfo */
    virtual bool IsInfo() { return false; }
    virtual bool IsInfoLegendLayer() { return false; }
    virtual bool IsMovableLayer(){ return false; }
    virtual bool IsScaleXLayer(){return false;}
    virtual bool IsScaleYLayer(){return false;}

    /** Check whether the layer is an point.
        The default implementation returns \a FALSE. It is overrided to \a TRUE for mpInfoLayer
        class and its derivative. It is necessary to define mouse actions behaviour over
        point layer.
        @return whether the layer is an point layer
        @sa mpPointLayer::IsPointEnabled */
    virtual bool IsPointLayer() { return false; }
    virtual void UpdateMouseCoord(mpWindow& w, wxEvent& event){}
    /** Get inclusive left border of bounding box.
        @return Value
    */
    virtual double GetMinX() { return -1.0; }

    /** Get inclusive right border of bounding box.
        @return Value
    */
    virtual double GetMaxX() { return  1.0; }

    /** Get inclusive bottom border of bounding box.
        @return Value
    */
    virtual double GetMinY() { return -1.0; }

    /** Get inclusive top border of bounding box.
        @return Value
    */
    virtual double GetMaxY() { return  1.0; }
    /**
     *  Search for the closest coordinate of the curve  @Jussi Vatjus-Anttila 9.-09
     *  So far, only implemented on mpFXY layer.
     *  Used by mpPointLayer
     *  @param w    mpWindow
     *  @param x [in+out]  search x coordinate near and set curve coordinate if found
     *  @param y [in+out]  search y coordinate near and set curve coordinate if found
     *  @return true if found curve coordinate
     */
    virtual bool GetNearestCoord(mpWindow & w, double & x, double & y) { return false; }

    /** Plot given view of layer to the given device context.
        An implementation of this function has to transform layer coordinates to
        wxDC coordinates based on the view parameters retrievable from the mpWindow
        passed in \a w.
        Note that the public methods of mpWindow: x2p,y2p and p2x,p2y are already provided
        which transform layer coordinates to DC pixel coordinates, and <b>user code should rely
        on them</b> for portability and future changes to be applied transparently, instead of
        implementing the following formulas manually.

        The passed device context \a dc has its coordinate origin set to the top-left corner
        of the visible area (the default). The coordinate orientation is as show in the
        following picture:
        <pre>
        (wxDC origin 0,0)
           x-------------> ascending X ----------------+
           |                                           |
           |                                           |
           V ascending Y                               |
           |                                           |
           |                                           |
           |                                           |
	       +-------------------------------------------+  <-- right-bottom corner of the mpWindow visible area.
        </pre>
        Note that Y ascends in downward direction, whereas the usual vertical orientation
        for mathematical plots is vice versa. Thus Y-orientation will be swapped usually,
        when transforming between wxDC and mpLayer coordinates. This change of coordinates
        is taken into account in the methods p2x,p2y,x2p,y2p.

        <b> Rules for transformation between mpLayer and wxDC coordinates </b>
        @code
        dc_X = (layer_X - mpWindow::GetPosX()) * mpWindow::GetScaleX()
        dc_Y = (mpWindow::GetPosY() - layer_Y) * mpWindow::GetScaleY() // swapping Y-orientation

        layer_X = (dc_X / mpWindow::GetScaleX()) + mpWindow::GetPosX() // scale guaranteed to be not 0
        layer_Y = mpWindow::GetPosY() - (dc_Y / mpWindow::GetScaleY()) // swapping Y-orientation
        @endcode

        @param dc Device context to plot to.
        @param w  View to plot. The visible area can be retrieved from this object.
        @sa mpWindow::p2x,mpWindow::p2y,mpWindow::x2p,mpWindow::y2p
    */
    virtual void   Plot(wxDC & dc, mpWindow & w) = 0;

    /** Get layer name.
        @return Name
    */
    wxString       GetName() const { return m_name; }

    /** Get font set for this layer.
        @return Font
    */
    const wxFont&  GetFont() const { return m_font; }

    /** Get pen set for this layer.
        @return Pen
    */
    const wxPen&   GetPen()  const { return m_pen;  }

    /** Set the 'continuity' property of the layer (true:draws a continuous line, false:draws separate points).
      * @sa GetContinuity
      */
    void SetContinuity(bool continuity) {m_continuous = continuity;}

    /** Gets the 'continuity' property of the layer.
      * @sa SetContinuity
      */
    bool GetContinuity() const {return m_continuous;}

    /** Shows or hides the text label with the name of the layer (default is visible).
      */
    void ShowName(bool show) { m_showName = show; };

    /** Set layer name
        @param name Name, will be copied to internal class member
    */
    void SetName(wxString name) { m_name = name; }

    /** Set layer font
        @param font Font, will be copied to internal class member
    */
    void SetFont(wxFont& font)  { m_font = font; }

    /** Set layer pen
        @param pen Pen, will be copied to internal class member
    */
    void SetPen(wxPen pen)     { m_pen  = pen;  }

    /** Set Draw mode: inside or outside margins. Default is outside, which allows the layer to draw up to the mpWindow border.
        @param drawModeOutside The draw mode to be set */
    void SetDrawOutsideMargins(bool drawModeOutside) { m_drawOutsideMargins = drawModeOutside; };

    /** Get Draw mode: inside or outside margins.
        @return The draw mode */
    bool GetDrawOutsideMargins() { return m_drawOutsideMargins; };

    /** Get a small square bitmap filled with the colour of the pen used in the layer. Useful to create legends or similar reference to the layers.
        @param side side length in pixels
        @return a wxBitmap filled with layer's colour */
    wxBitmap GetColourSquare(int side = 16);

    /** Get layer type: a Layer can be of different types: plot lines, axis, info boxes, etc, this method returns the right value.
        @return An integer indicating layer type */
    mpLayerType GetLayerType() const { return m_type; }

    /** Get point type: each layer can be drawn with a specific point type e.g. mpPOINT, mpCIRCLE, mpRECT
		@return <-- the current point type*/
	mpShape GetPointType() const { return m_pointType; }

    /** Set point type: each layer can be drawn with a specific point type e.g. mpPOINT, mpCIRCLE, mpRECT
		@param pt --> set the current point type*/
	void SetPointType(const mpShape& pt) { m_pointType=pt; }

    /** Checks whether the layer is visible or not.
        @return \a true if visible */
    bool IsVisible() {return m_visible; };

    /** Sets layer visibility.
        @param show visibility bool. */
    void SetVisible(bool show) { m_visible = show; }

	/** Get brush set for this layer.
		@return brush. */
	const wxBrush&   GetBrush() const { return m_brush; };

	/** Set layer brush
		@param brush brush, will be copied to internal class member	*/
	void SetBrush(wxBrush brush) { m_brush = brush; };

    /** Set Layer drawing order
        n = {0..10}
        @param n  when layer drawn. 0= first, 10=last
    */
    //void SetDrawingOrder(int n){ m_drawingOrder = n; }

protected:
    wxFont   m_font;        //!< Layer's font
    wxPen    m_pen;         //!< Layer's pen
    wxBrush  m_brush;       //!< Layer's brush
    wxString m_name;        //!< Layer's name
    bool     m_continuous;  //!< Specify if the layer will be plotted as a continuous line or a set of points.
    bool     m_showName;    //!< States whether the name of the layer must be shown (default is true).
    bool     m_drawOutsideMargins; //!< select if the layer should draw only inside margins or over all DC
    mpLayerType m_type;     //!< Define layer type, which is assigned by constructor
	bool 	m_visible;	    //!< Toggles layer visibility

	mpShape m_pointType; //!< Define a point type, the default is a simple point
	wxSize  m_RectSize; //!< The size of rectangles, when plotstyle mpRECT is used
	//int     m_drawingOrder; //!< Drawing order
    DECLARE_DYNAMIC_CLASS(mpLayer)
};


//-----------------------------------------------------------------------------
// mpInfoLayer
//-----------------------------------------------------------------------------

/** @class mpInfoLayer
    @brief Base class to create small rectangular info boxes
    mpInfoLayer is the base class to create a small rectangular info box in transparent overlay over plot layers. It is used to implement objects like legends.
*/
class WXDLLIMPEXP_MATHPLOT mpInfoLayer : public mpLayer
{
public:
    /** Default constructor. */
    mpInfoLayer();

    /** Complete constructor.
        @param rect Sets the initial size rectangle of the layer.
        @param brush pointer to a fill brush. Default is transparent */
    mpInfoLayer(wxRect rect, const wxBrush* brush = wxTRANSPARENT_BRUSH);

    /** Destructor */
    virtual ~mpInfoLayer();

    /** Updates the content of the info box. Should be overidden by derived classes.
        Update may behave in different ways according to the type of event which called it.
        @param w parent mpWindow from which to obtain information
        @param event The event which called the update. */
    virtual void UpdateInfo(mpWindow& w, wxEvent& event);

    /** mpInfoLayer has not bounding box. @sa mpLayer::HasBBox
        @return always \a FALSE */
    virtual bool HasBBox() { return false; };

    /** Plot method. Can be overidden by derived classes.
        @param dc the device content where to plot
        @param w the window to plot
        @sa mpLayer::Plot */
    virtual void   Plot(wxDC & dc, mpWindow & w);

    /** Specifies that this is an Info box layer.
        @return always \a TRUE
        @sa mpLayer::IsInfo */
    virtual bool IsInfo() { return true; };

    /** Checks whether a point is inside the info box rectangle.
        @param point The point to be checked
        @return \a true if the point is inside the bounding box */
    virtual bool Inside(wxPoint& point);

    /** Moves the layer rectangle of given pixel deltas.
        @param delta The wxPoint container for delta coordinates along x and y. Units are in pixels. */
    virtual void Move(wxPoint delta);

    /** Updates the rectangle reference point. Used by internal methods of mpWindow to correctly move mpInfoLayers. */
    virtual void UpdateReference();

    /** Returns the position of the upper left corner of the box (in pixels)
        @return The rectangle position */
    wxPoint GetPosition();

    /** Returns the size of the box (in pixels)
        @return The rectangle size */
    wxSize GetSize();
     /** Returns the current rectangle coordinates.
        @return The info layer rectangle */
    const wxRect& GetRectangle() { return m_dim; };

protected:
    wxRect m_dim;           //!< The bounding rectangle of the box. It may be resized dynamically by the Plot method.
    wxPoint m_reference;    //!< Holds the reference point for movements
    wxBrush m_brush;        //!< The brush to be used for the background
    int m_winX, m_winY;     //!< Holds the mpWindow size. Used to rescale position when window is resized.

    DECLARE_DYNAMIC_CLASS(mpInfoLayer)
};

/** @class mpInfoCoords
    @brief Implements an overlay box which shows the mouse coordinates in plot units.
    When an mpInfoCoords layer is activated, when mouse is moved over the mpWindow, its coordinates (in mpWindow units, not pixels) are continuously reported inside the layer box. */
class WXDLLIMPEXP_MATHPLOT mpInfoCoords : public mpInfoLayer
{
public:
    /** Default constructor */
    mpInfoCoords(unsigned int x_labelType);														//LOGtest "unsigned int x_labelType" added
    /** Complete constructor, setting initial rectangle and background brush.
        @param rect The initial bounding rectangle.
        @param brush The wxBrush to be used for box background: default is transparent */
    mpInfoCoords(wxRect rect, const wxBrush* brush = wxTRANSPARENT_BRUSH);

    /** Default destructor */
    virtual ~mpInfoCoords();

    /** Set format of coordinate string */
    void SetFormat(wxString form){ m_format = form; }

    /** Updates the content of the info box. It is used to update coordinates.
        @param w parent mpWindow from which to obtain information
        @param event The event which called the update. */
    virtual void UpdateInfo(mpWindow& w, wxEvent& event);

    /** Plot method.
        @param dc the device content where to plot
        @param w the window to plot
        @sa mpLayer::Plot */
    virtual void   Plot(wxDC & dc, mpWindow & w);

protected:
    wxString m_content; //!< string holding the coordinates to be drawn.
    wxString m_format;  //!< Format of coordinate string. default: "x = %f y = %f"
	unsigned int m_labelType;		//Added for LOGtest
};

/** @class mpInfoLegend
    @brief Implements the legend to be added to the plot
    This layer allows to add a legend to describe the plots in the window. The legend uses the layer name as a label, and displays only layers of type mpLAYER_PLOT. */
class WXDLLIMPEXP_MATHPLOT mpInfoLegend : public mpInfoLayer
{
public:
    /** Default constructor */
    mpInfoLegend();

    /** Complete constructor, setting initial rectangle and background brush.
        @param rect The initial bounding rectangle.
        @param brush The wxBrush to be used for box background: default is transparent
        @sa mpInfoLayer::mpInfoLayer */
    mpInfoLegend(wxRect rect, const wxBrush* brush = wxTRANSPARENT_BRUSH);

    /**  Default destructor */
    virtual ~mpInfoLegend();

    virtual bool IsInfoLegendLayer() { return true; }

    /** Updates the content of the info box. Unused in this class.
        @param w parent mpWindow from which to obtain information
        @param event The event which called the update. */
    virtual void UpdateInfo(mpWindow& w, wxEvent& event);

    /** Plot method.
        @param dc the device content where to plot
        @param w the window to plot
        @sa mpLayer::Plot */
    virtual void   Plot(wxDC & dc, mpWindow & w);

    wxMenu* GetPopupMenu(){ return &m_popmenu;  }
    void popup(wxWindow& win, wxMouseEvent& event)
    { win.PopupMenu( &m_popmenu, event.GetX(), event.GetY()); }

protected:
private:
    wxMenu m_popmenu;

};

/** @class mpNoteLegend
    @brief Implements the legend to be added to the plot
    This layer allows to add a legend to describe the plots in the window.
    The legend uses the layer name as a label
    -Support multiline */
class WXDLLIMPEXP_MATHPLOT mpNoteLegend : public mpInfoLayer
{
public:
    /** Default constructor */
    mpNoteLegend();

    /** Complete constructor, setting initial rectangle and background brush.
        @param rect The initial bounding rectangle.
        @param brush The wxBrush to be used for box background: default is transparent
        @sa mpInfoLayer::mpInfoLayer */
    mpNoteLegend(wxRect rect, const wxBrush* brush = wxTRANSPARENT_BRUSH);

    /**  Default destructor */
    ~mpNoteLegend();

    /** Updates the content of the info box. Unused in this class.
        @param w parent mpWindow from which to obtain information
        @param event The event which called the update. */
    virtual void UpdateInfo(mpWindow& w, wxEvent& event);

    /** Plot method.
        @param dc the device content where to plot
        @param w the window to plot
        @sa mpLayer::Plot */
    virtual void   Plot(wxDC & dc, mpWindow & w);

protected:

};

//-----------------------------------------------------------------------------
// mpLayer implementations - functions
//-----------------------------------------------------------------------------

/** @name Label alignment constants
@{*/

/** @internal */
#define mpALIGNMASK    0x03
/** Aligns label to the right. For use with mpFX. */
#define mpALIGN_RIGHT  0x00
/** Aligns label to the center. For use with mpFX and mpFY. */
#define mpALIGN_CENTER 0x01
/** Aligns label to the left. For use with mpFX. */
#define mpALIGN_LEFT   0x02
/** Aligns label to the top. For use with mpFY. */
#define mpALIGN_TOP    mpALIGN_RIGHT
/** Aligns label to the bottom. For use with mpFY. */
#define mpALIGN_BOTTOM mpALIGN_LEFT
/** Aligns X axis to bottom border. For mpScaleX */
#define mpALIGN_BORDER_BOTTOM  0x04
/** Aligns X axis to top border. For mpScaleX */
#define mpALIGN_BORDER_TOP  0x05
/** Set label for X axis in normal mode */
#define mpX_NORMAL  0x00
/** Set label for X axis in time mode: the value is represented as minutes:seconds.milliseconds if time is less than 2 minutes, hours:minutes:seconds otherwise*/
#define mpX_TIME  0x01
/** Set label for X axis in hours mode: the value is always represented as hours:minutes:seconds*/
#define mpX_HOURS 0x02
/** Set label for X axis in date mode: the value is always represented as yyyy-mm-dd. */
#define mpX_DATE 0x03
/** Set label for X axis in datetime mode: the value is always represented as yyyy-mm-ddThh:mm:ss. */
#define mpX_DATETIME 0x04
/*LOGtest* Set label for X axis in logarithmic mode */
#define mpX_LOGARITHMIC 0x05									//################################LOGtest##########################
/** Aligns Y axis to left border. For mpScaleY */
#define mpALIGN_BORDER_LEFT mpALIGN_BORDER_BOTTOM
/** Aligns Y axis to right border. For mpScaleY */
#define mpALIGN_BORDER_RIGHT mpALIGN_BORDER_TOP
/** Aligns label to north-east. For use with mpFXY. */
#define mpALIGN_NE     0x00
/** Aligns label to north-west. For use with mpFXY. */
#define mpALIGN_NW     0x01
/** Aligns label to south-west. For use with mpFXY. */
#define mpALIGN_SW     0x02
/** Aligns label to south-east. For use with mpFXY. */
#define mpALIGN_SE     0x03

/*@}*/

/** @name mpLayer implementations - functions
@{*/

/** mpFXYBar class
*   by Jussi Vatjus-Anttila
*
*   |''|
*   |  |''|
*   |  |  |''|
* ''''''''''''''''''''''''''''''''
*/
class WXDLLIMPEXP_MATHPLOT mpFXYBar : public mpLayer
{
public:
    /** @param name  Label
        @param flags Label alignment, pass one of #mpALIGN_NE, #mpALIGN_NW, #mpALIGN_SW, #mpALIGN_SE.
    */
    mpFXYBar(wxString name = wxEmptyString, int flags = mpALIGN_NE);
    virtual ~mpFXYBar(){}

    /** Rewind value enumeration with mpFXY::GetNextXY.
        Override this function in your implementation.
    */
    virtual void Rewind() = 0;

    /** Get locus value for next N.
        Override this function in your implementation.
        @param x Returns X value
        @param y Returns Y value
    */
    virtual bool GetNextXY(double & x, double & y, wxString& label) = 0;
    /**
     * Bar chart Lowest significant bit
     * default: not in use (return false)
     */
    virtual bool GetLSB(double &l){ return false; }
    /** Returns the actual minimum X data (loaded in SetData).
      */
    virtual double GetMinX() = 0;

    /** Returns the actual minimum Y data (loaded in SetData).
      */
    virtual double GetMinY() = 0;

    /** Returns the actual maximum X data (loaded in SetData).
      */
    virtual double GetMaxX() = 0;

    /** Returns the actual maximum  Y data (loaded in SetData).
      */
    virtual double GetMaxY() = 0;

    /** Layer plot handler.
        This implementation will plot the locus in the visible area and
        put a label according to the aligment specified.
    */
    virtual void Plot(wxDC & dc, mpWindow & w);

    void SetGradientBackColour(bool t){m_gradienBackground=t;}
    void ShowLabel(bool t){m_showLabel = t; }
    void UseCustomLabel(bool t){ m_useCustomLabel = t;}

protected:
    int m_flags; //!< Holds label alignment
    bool m_gradienBackground;
    bool m_showLabel;
    bool m_useCustomLabel;

    DECLARE_CLASS(mpFXYBar)
};
/** mpFYXBar class
*   by Jussi Vatjus-Anttila
*
*   |-----+
*   |-----+---+
*   |--------++
*   |--------+
´* '''''''''''''''''''''
*/
class WXDLLIMPEXP_MATHPLOT mpFYXBar : public mpLayer
{
public:
    /** @param name  Label
        @param flags Label alignment, pass one of #mpALIGN_NE, #mpALIGN_NW, #mpALIGN_SW, #mpALIGN_SE.
    */
    mpFYXBar(wxString name = wxEmptyString, int flags = mpALIGN_NE);
    virtual ~mpFYXBar(){}

    /** Rewind value enumeration with mpFXY::GetNextXY.
        Override this function in your implementation.
    */
    virtual void Rewind() = 0;

    /** Get locus value for next N.
        Override this function in your implementation.
        @param x Returns X value
        @param y Returns Y value
    */
    virtual bool GetNextXY(double & x, double & y, wxString& label) = 0;
    /**
     * Bar chart Lowest significant bit
     * default: not in use (return false)
     */
    virtual bool GetLSB(double &l){ return false; }
    /** Returns the actual minimum X data (loaded in SetData).
      */
    virtual double GetMinX() = 0;

    /** Returns the actual minimum Y data (loaded in SetData).
      */
    virtual double GetMinY() = 0;

    /** Returns the actual maximum X data (loaded in SetData).
      */
    virtual double GetMaxX() = 0;

    /** Returns the actual maximum  Y data (loaded in SetData).
      */
    virtual double GetMaxY() = 0;

    /** Layer plot handler.
        This implementation will plot the locus in the visible area and
        put a label according to the aligment specified.
    */
    virtual void Plot(wxDC & dc, mpWindow & w);

    void SetGradientBackColour(bool t){m_gradienBackground=t;}
    void ShowLabel(bool t){m_showLabel = t; }
    void UseCustomLabel(bool t){ m_useCustomLabel = t;}

protected:
    int m_flags; //!< Holds label alignment
    bool m_gradienBackground;
    bool m_showLabel;
    bool m_useCustomLabel;

    DECLARE_CLASS(mpFYXBar)
};
/** mpFXYCandleStick class
*   by Jussi Vatjus-Anttila
*/
class WXDLLIMPEXP_MATHPLOT mpFXYCandleStick : public mpLayer
{
public:
    /** @param name  Label
        @param flags Label alignment, pass one of #mpALIGN_NE, #mpALIGN_NW, #mpALIGN_SW, #mpALIGN_SE.
    */
    mpFXYCandleStick(wxString name = wxEmptyString, int flags = mpALIGN_NE);
	/** virtual destructor */
    virtual ~mpFXYCandleStick(){}

    /** Rewind value enumeration with mpFXY::GetNextXY.
        Override this function in your implementation.
    */
    virtual void Rewind() = 0;

    /** Get locus value for next N.
        Override this function in your implementation.
        @param x Returns X value
        @param y Returns Y value
    */
    virtual bool GetNext(double & x, double & yCandleMin,double & yCandleMax, double& yStickMin, double& yStickMax) = 0;
    /**
     * Bar chart Lowest significant bit
     * default: not in use (return false)
     */
    virtual bool GetLSB(double &l){ return false; }

    /** Layer plot handler.
        This implementation will plot the locus in the visible area and
        put a label according to the aligment specified.
    */
    virtual void Plot(wxDC & dc, mpWindow & w);

    void SetGradientBackColour(bool t)	{ m_gradienBackground = t;	}	//!< Enable gradient backcolours
    void SetCandleBrush(wxBrush brush) 	{ m_candleBrush = brush; 	}	//!< Set candle brush when min<max. (default=black)
    void SetCandleBrush2(wxBrush brush) { m_candleBrush2 = brush; 	}	//!< Set candle brush when min>max. (default=white)
    void SetCandlePen(wxPen pen) 		{ m_candlePen = pen; 		}	//!< Set candle pen (default: black)
    void SetStickPen(wxPen pen) 		{ m_stickPen = pen; 		}	//!< Set stick pen (default: black)

protected:
    int m_flags; //!< Holds label alignment
    bool m_gradienBackground;	//!< True if gradien background enables

    wxPen m_stickPen;			//!< Stick pen
    wxBrush m_candleBrush;		//!< Candle brush, when yCandleMin < yCandleMax
    wxBrush m_candleBrush2;		//!< Candle brush, when yCandleMin > yCandleMax
    wxPen m_candlePen;			//!< Candle pen.


    DECLARE_CLASS(mpFXYCandleStick)
};
/** Double line with middle value
*/
class WXDLLIMPEXP_MATHPLOT mpFX2Y : public mpLayer
{
public:
    /** @param name  Label
        @param flags Label alignment, pass one of #mpALIGN_NE, #mpALIGN_NW, #mpALIGN_SW, #mpALIGN_SE.
    */
    mpFX2Y(wxString name = wxEmptyString, int flags = mpALIGN_NE);

    /** Rewind value enumeration with mpFXY::GetNextXY.
        Override this function in your implementation.
    */
    virtual void Rewind() = 0;

    /** Get locus value for next N.
        Override this function in your implementation.
        @param x Returns X value
        @param y Returns Y value
    */
    virtual bool GetNextX2Y(double & x, double & y1, double & y2) = 0;

    /** Layer plot handler.
        This implementation will plot the locus in the visible area and
        put a label according to the aligment specified.
    */
    virtual void Plot(wxDC & dc, mpWindow & w);

protected:
    int m_flags; //!< Holds label alignment

    DECLARE_DYNAMIC_CLASS(mpFX2Y)
};




/** Abstract base class providing plot and labeling functionality for functions F:X->Y.
    Override mpFX::GetY to implement a function.
    Optionally implement a constructor and pass a name (label) and a label alignment
    to the constructor mpFX::mpFX. If the layer name is empty, no label will be plotted.
*/
class WXDLLIMPEXP_MATHPLOT mpFX : public mpLayer
{
public:
    /** @param name  Label
        @param flags Label alignment, pass one of #mpALIGN_RIGHT, #mpALIGN_CENTER, #mpALIGN_LEFT.
    */
    mpFX(wxString name = wxEmptyString, int flags = mpALIGN_RIGHT);

    /** Get function value for argument.
        Override this function in your implementation.
        @param x Argument
        @return Function value
    */
    virtual double GetY( double x ) = 0;

    /** Layer plot handler.
        This implementation will plot the function in the visible area and
        put a label according to the aligment specified.
    */
    virtual void Plot(wxDC & dc, mpWindow & w);

    void MarkCorners(bool t);

protected:
    int m_flags;            //!< Holds label alignment
    bool m_markCorners;     //!< Mark the curve corners
    wxCoord m_cornerMarkSize;

    DECLARE_DYNAMIC_CLASS(mpFX)
};

/** Abstract base class providing plot and labeling functionality for functions F:Y->X.
    Override mpFY::GetX to implement a function.
    Optionally implement a constructor and pass a name (label) and a label alignment
    to the constructor mpFY::mpFY. If the layer name is empty, no label will be plotted.
*/
class WXDLLIMPEXP_MATHPLOT mpFY : public mpLayer
{
public:
    /** @param name  Label
        @param flags Label alignment, pass one of #mpALIGN_BOTTOM, #mpALIGN_CENTER, #mpALIGN_TOP.
    */
    mpFY(wxString name = wxEmptyString, int flags = mpALIGN_TOP);

    /** Get function value for argument.
        Override this function in your implementation.
        @param y Argument
        @return Function value
    */
    virtual double GetX( double y ) = 0;

    /** Layer plot handler.
        This implementation will plot the function in the visible area and
        put a label according to the aligment specified.
    */
    virtual void Plot(wxDC & dc, mpWindow & w);

protected:
    int m_flags; //!< Holds label alignment

    DECLARE_DYNAMIC_CLASS(mpFY)
};

/** Abstract base class providing plot and labeling functionality for a locus plot F:N->X,Y.
    Locus argument N is assumed to be in range 0 .. MAX_N, and implicitly derived by enumerating
    all locus values. Override mpFXY::Rewind and mpFXY::GetNextXY to implement a locus.
    Optionally implement a constructor and pass a name (label) and a label alignment
    to the constructor mpFXY::mpFXY. If the layer name is empty, no label will be plotted.
*/
class WXDLLIMPEXP_MATHPLOT mpFXY : public mpLayer
{
public:
    /** @param name  Label
        @param flags Label alignment, pass one of #mpALIGN_NE, #mpALIGN_NW, #mpALIGN_SW, #mpALIGN_SE.
    */
    mpFXY(wxString name = wxEmptyString, int flags = mpALIGN_NE);

    /** Rewind value enumeration with mpFXY::GetNextXY.
        Override this function in your implementation.
    */
    virtual void Rewind() = 0;
    virtual void CurrentBounds(double xmin, double xmax){}

    /** Get locus value for next N.
        Override this function in your implementation.
        @param x Returns X value
        @param y Returns Y value
    */
    virtual bool GetNextXY(double & x, double & y) = 0;
    /** Find nearest coordinate according arguments
        @param w mpWindow instance
        @param x x point in curve
        @param y y point in curve
        return true if found any
    */
    virtual bool GetNearestCoord(mpWindow & w, double & x, double & y);
    /**  Mark corners to curve
         @param t true/false
    */
    void MarkCorners(bool t = true);
    /** Layer plot handler.
        This implementation will plot the locus in the visible area and
        put a label according to the aligment specified.
    */
    virtual void Plot(wxDC & dc, mpWindow & w);

protected:
    int m_flags;                //!< Holds label alignment
    bool m_markCorners;         //!< Mark the curve corners
    wxCoord m_cornerMarkSize;

	// Data to calculate label positioning
	wxCoord maxDrawX, minDrawX, maxDrawY, minDrawY;
	//int drawnPoints;

    /** Update label positioning data
	    @param xnew New x coordinate
		@param ynew New y coordinate
	*/
	void UpdateViewBoundary(wxCoord xnew, wxCoord ynew);

    DECLARE_DYNAMIC_CLASS(mpFXY)
};

/** Abstract base class providing plot and labeling functionality for functions F:Y->X.
    Override mpProfile::GetX to implement a function.
    This class is similar to mpFY, but the Plot method is different. The plot is in fact represented by lines instead of points, which gives best rendering of rapidly-varying functions, and in general, data which are not so close one to another.
    Optionally implement a constructor and pass a name (label) and a label alignment
    to the constructor mpProfile::mpProfile. If the layer name is empty, no label will be plotted.
*/
class WXDLLIMPEXP_MATHPLOT mpProfile : public mpLayer
{
public:
    /** @param name  Label
        @param flags Label alignment, pass one of #mpALIGN_BOTTOM, #mpALIGN_CENTER, #mpALIGN_TOP.
    */
    mpProfile(wxString name = wxEmptyString, int flags = mpALIGN_TOP);

    /** Get function value for argument.
        Override this function in your implementation.
        @param x Argument
        @return Function value
    */
    virtual double GetY( double x ) = 0;

    /** Layer plot handler.
        This implementation will plot the function in the visible area and
        put a label according to the aligment specified.
    */
    virtual void Plot(wxDC & dc, mpWindow & w);

protected:
    int m_flags; //!< Holds label alignment

    DECLARE_DYNAMIC_CLASS(mpProfile)
};

/*@}*/

//-----------------------------------------------------------------------------
// mpLayer implementations - furniture (scales, ...)
//-----------------------------------------------------------------------------

/** @name mpLayer implementations - furniture (scales, ...)
 @{ */

/** Plot layer implementing a x-scale ruler.
    The ruler is fixed at Y=0 in the coordinate system. A label is plotted at
    the bottom-right hand of the ruler. The scale numbering automatically
    adjusts to view and zoom factor.
*/
class WXDLLIMPEXP_MATHPLOT mpScaleX : public mpLayer
{
public:
    /** Full constructor.
		@param name Label to plot by the ruler
		@param flags Set the position of the scale with respect to the window.
		@param ticks Select ticks or grid. Give TRUE (default) for drawing axis ticks, FALSE for drawing the grid.
		@param type mpX_NORMAL for normal labels, mpX_TIME for time axis in hours, minutes, seconds. */
    mpScaleX(wxString name = wxT("X"), int flags = mpALIGN_CENTER, bool ticks = true, unsigned int type = mpX_NORMAL);

    /** Layer plot handler.
        This implementation will plot the ruler adjusted to the visible area.
    */
    virtual void Plot(wxDC & dc, mpWindow & w);

    /** Check whether this layer has a bounding box.
        This implementation returns \a FALSE thus making the ruler invisible
        to the plot layer bounding box calculation by mpWindow.
    */
    virtual bool HasBBox();
    /** Virtual function for @return true because this is xscaler*/
    virtual bool IsScaleXLayer();

    /** Set X axis alignment.
        @param align alignment (choose between mpALIGN_BORDER_BOTTOM, mpALIGN_BOTTOM, mpALIGN_CENTER, mpALIGN_TOP, mpALIGN_BORDER_TOP */
    void SetAlign(int align);

    /** Set X axis ticks or grid
        @param ticks TRUE to plot axis ticks, FALSE to plot grid. */
    void SetTicks(bool ticks);

    /** Get X axis ticks or grid
        @return TRUE if plot is drawing axis ticks, FALSE if the grid is active. */
    bool GetTicks();

    /** Get X axis label colour of the text.
        @return the current wxColour of the ticks labels */
	const wxColour& GetTicksColour() const {return m_pTicksLabelColour;}

    /** Get X axis label colour of the text.
        @param c set the new colour of the ticks labels */
	void SetTicksColour(const wxColour &c) {m_pTicksLabelColour=c;}

    /** Get X axis label view mode.
        @return mpX_NORMAL for normal labels, mpX_TIME for time axis in hours, minutes, seconds. */
    unsigned int GetLabelMode() { return m_labelType; };

    /** Set X axis label view mode.
        @param mode mpX_NORMAL for normal labels, mpX_TIME for time axis in hours, minutes, seconds. */
    void SetLabelMode(unsigned int mode) { m_labelType = mode; };

	/** Set X axis Label format (used for mpX_NORMAL draw mode).
	    @param format The format string */
	void SetLabelFormat(const wxString& format) { m_labelFormat = format; };

	/** Get X axis Label format (used for mpX_NORMAL draw mode).
	@return The format string */
	const wxString& GetLabelFormat() { return m_labelFormat; };


    /** Get reference to context menu of the plot canvas.
        @return Pointer to menu. The menu can be modified.
    */
    wxMenu* GetPopupMenu() { return &m_popmenu; }
    virtual bool Inside(const wxPoint& point);
    void popup(wxWindow& win, wxMouseEvent& event)
    { win.PopupMenu( &m_popmenu, event.GetX(), event.GetY()); }

protected:
    int m_flags; //!< Flag for axis alignment
    bool m_ticks; //!< Flag to toggle between ticks or grid
    unsigned int m_labelType; //!< Select labels mode: mpX_NORMAL for normal labels, mpX_TIME for time axis in hours, minutes, seconds
	wxString m_labelFormat; //!< Format string used to print labels

    wxColour m_pTicksLabelColour; //!< The colour of the ticks at the axis. It can be get/set by Get/SetTicksColour member function.
    wxRect  m_labelRect;
    wxMenu  m_popmenu;  //!< user handled popupmenu when clicked the point shape

    DECLARE_DYNAMIC_CLASS(mpScaleX)
};

/** Plot layer implementing a y-scale ruler.
    If align is set to mpALIGN_CENTER, the ruler is fixed at X=0 in the coordinate system. If the align is set to mpALIGN_TOP or mpALIGN_BOTTOM, the axis is always drawn respectively at top or bottom of the window. A label is plotted at
    the top-right hand of the ruler. The scale numbering automatically
    adjusts to view and zoom factor.
*/
class WXDLLIMPEXP_MATHPLOT mpScaleY : public mpLayer
{
public:
    /** @param name Label to plot by the ruler
        @param flags Set position of the scale respect to the window.
        @param ticks Select ticks or grid. Give TRUE (default) for drawing axis ticks, FALSE for drawing the grid */
    mpScaleY(wxString name = wxT("Y"), int flags = mpALIGN_CENTER, bool ticks = true);

    /** Layer plot handler.
        This implementation will plot the ruler adjusted to the visible area.
    */
    virtual void Plot(wxDC & dc, mpWindow & w);

    /** Check whether this layer has a bounding box.
        This implementation returns \a FALSE thus making the ruler invisible
        to the plot layer bounding box calculation by mpWindow.
    */
    virtual bool HasBBox() { return FALSE; }

    virtual bool IsScaleYLayer(){return true;}

    /** Set Y axis alignment.
        @param align alignment (choose between mpALIGN_BORDER_LEFT, mpALIGN_LEFT, mpALIGN_CENTER, mpALIGN_RIGHT, mpALIGN_BORDER_RIGHT) */
    void SetAlign(int align) { m_flags = align; };

    /** Set Y axis ticks or grid
        @param ticks TRUE to plot axis ticks, FALSE to plot grid. */
    void SetTicks(bool ticks) { m_ticks = ticks; };

    /** Get Y axis ticks or grid
        @return TRUE if plot is drawing axis ticks, FALSE if the grid is active. */
    bool GetTicks() { return m_ticks; };


	/** Set Y axis Label format.
	@param format The format string */
	void SetLabelFormat(const wxString& format) { m_labelFormat = format; };

	/** Get Y axis Label format.
	@return The format string */
	const wxString& SetLabelFormat() { return m_labelFormat; };

    /** Get X axis label colour of the text.
        @return the current wxColour of the ticks labels */
	const wxColour& GetTicksColour() const {return m_pTicksLabelColour;}

    /** Get X axis label colour of the text.
        @param c set the new colour of the ticks labels */
	void SetTicksColour(const wxColour &c) {m_pTicksLabelColour=c;}

    /** Get reference to context menu of the plot canvas.
        @return Pointer to menu. The menu can be modified.
    */
    wxMenu* GetPopupMenu() { return &m_popmenu; }
    virtual bool Inside(const wxPoint& point);
    void popup(wxWindow& win, wxMouseEvent& event)
    { win.PopupMenu( &m_popmenu, event.GetX(), event.GetY()); }

protected:
    int m_flags;        //!< Flag for axis alignment
    bool m_ticks;       //!< Flag to toggle between ticks or grid
	wxString m_labelFormat; //!< Format string used to print labels

    wxColour m_pTicksLabelColour; //!< The colour of the ticks at the axis. It can be get/set by Get/SetTicksColour member function.

    wxRect  m_labelRect;
    wxMenu  m_popmenu;  //!< user handled popupmenu when clicked the point shape

    DECLARE_DYNAMIC_CLASS(mpScaleY)
};

//-----------------------------------------------------------------------------
// mpWindow
//-----------------------------------------------------------------------------

/** @name Constants defining mouse modes for mpWindow
@{*/

/** Mouse panning drags the view. Mouse mode for mpWindow. */
#define mpMOUSEMODE_DRAG    0
/** Mouse panning creates a zoom box. Mouse mode for mpWindow. */
#define mpMOUSEMODE_ZOOMBOX 1

/*@}*/
/** Define the type for the list of layers inside mpWindow */
//WX_DECLARE_HASH_MAP( int, mpLayer*, wxIntegerHash, wxIntegerEqual, wxLayerList );
typedef std::deque<mpLayer*> wxLayerList;

/** Canvas for plotting mpLayer implementations.

    This class defines a zoomable and moveable 2D plot canvas. Any number
    of mpLayer implementations (scale rulers, function plots, ...) can be
    attached using mpWindow::AddLayer.

    The canvas window provides a context menu with actions for navigating the view.
    The context menu can be retrieved with mpWindow::GetPopupMenu, e.g. for extending it
    externally.

    Since wxMathPlot version 0.03, the mpWindow incorporate the following features:
        - DoubleBuffering (Default=disabled): Can be set with EnableDoubleBuffer
        - Mouse based pan/zoom (Default=enable): Can be set with EnableMousePanZoom.

    The mouse commands can be visualized by the user through the popup menu, and are:
        - Mouse Move+CTRL: Pan (Move)
        - Mouse Wheel: Vertical scroll
        - Mouse Wheel+SHIFT: Horizontal scroll
        - Mouse Wheel UP+CTRL: Zoom in
        - Mouse Wheel DOWN+CTRL: Zoom out

*/
class WXDLLIMPEXP_MATHPLOT mpWindow : public wxWindow
{
public:
    mpWindow();
    mpWindow( wxWindow *parent, wxWindowID id,
                     const wxPoint &pos = wxDefaultPosition,
                     const wxSize &size = wxDefaultSize,
                     long flags = 0, const wxString& name = wxT("mathplot"));
    virtual ~mpWindow();

    /** Init popup menu.
        @todo It might be possible to format the menu outside of class some how...
    */
    void InitPopupMenu();

    /** Get reference to context menu of the plot canvas.
        @return Pointer to menu. The menu can be modified.
    */
    wxMenu* GetPopupMenu() { return &m_popmenu; }

    /** Add a plot layer to the canvas.
        @param layer Pointer to layer. The mpLayer object will get under control of mpWindow,
                     i.e. it will be delete'd on mpWindow destruction
        @param refreshDisplay States whether to refresh the display (UpdateAll) after adding the layer.
        @retval TRUE Success
        @retval FALSE Failure due to out of memory.
    */
    bool AddLayer( mpLayer* layer, bool refreshDisplay = true);

    /** Remove a plot layer from the canvas.
        @param layer Pointer to layer. The mpLayer object will be destructed using delete.
        @param alsoDeleteObject If set to true, the mpLayer object will be also "deleted", not just removed from the internal list.
        @param refreshDisplay States whether to refresh the display (UpdateAll) after removing the layer.
        @return true if layer is deleted correctly

        N.B. Only the layer reference in the mpWindow is deleted, the layer object still exists!
    */
    bool DelLayer( mpLayer* layer, bool alsoDeleteObject = false, bool refreshDisplay = true);

    /** Remove all layers from the plot.
        @param alsoDeleteObject If set to true, the mpLayer objects will be also "deleted", not just removed from the internal list.
        @param refreshDisplay States whether to refresh the display (UpdateAll) after removing the layers.
    */
    void DelAllLayers( bool alsoDeleteObject, bool refreshDisplay = true);


    /*! Get the layer in list position indicated.
        N.B. You <i>must</i> know the index of the layer inside the list!
        @param position position of the layer in the layers list
        @return pointer to mpLayer
    */
    mpLayer* GetLayer(int position);

    /*! Get the layer by its name (case sensitive).
        @param name The name of the layer to retrieve
        @return A pointer to the mpLayer object, or NULL if not found.
    */
    mpLayer* GetLayer( const wxString &name);

    /**
    *   CHange drawing order of layer.
    */
    void SetLayerPosition(mpLayer* layer, int position);
    void SetLayerBottom(mpLayer* layer);
    void SetLayerTop(mpLayer* layer);

    /** Get current view's X scale.
        See @ref mpLayer::Plot "rules for coordinate transformation"
        @return Scale
    */
    double GetXscl() { return m_scaleX; }
    double GetScaleX(void) const{ return m_scaleX; }; // Schaling's method: maybe another method esists with the same name

    /** Get current view's Y scale.
        See @ref mpLayer::Plot "rules for coordinate transformation"
        @return Scale
    */
    double GetYscl() const { return m_scaleY; }
    double GetScaleY(void) const { return m_scaleY; } // Schaling's method: maybe another method esists with the same name

    /** Get current view's X position.
        See @ref mpLayer::Plot "rules for coordinate transformation"
        @return X Position in layer coordinate system, that corresponds to the center point of the view.
    */
    double GetXpos() const { return m_posX; }
    double GetPosX(void) const { return m_posX; }

    /** Get current view's Y position.
        See @ref mpLayer::Plot "rules for coordinate transformation"
        @return Y Position in layer coordinate system, that corresponds to the center point of the view.
    */
    double GetYpos() const { return m_posY; }
    double GetPosY(void) const { return m_posY; }

    /** Get current view's X dimension in device context units.
        Usually this is equal to wxDC::GetSize, but it might differ thus mpLayer
        implementations should rely on the value returned by the function.
        See @ref mpLayer::Plot "rules for coordinate transformation"
        @return X dimension.
    */
    int GetScrX(void) const { return m_scrX; }
    int GetXScreen(void) const { return m_scrX; }

    /** Get current view's Y dimension in device context units.
        Usually this is equal to wxDC::GetSize, but it might differ thus mpLayer
        implementations should rely on the value returned by the function.
        See @ref mpLayer::Plot "rules for coordinate transformation"
        @return Y dimension.
    */
    int GetScrY(void) const { return m_scrY; }
    int GetYScreen(void) const { return m_scrY; }

    /** Set current view's X scale and refresh display.
        @param scaleX New scale, must not be 0.
    */
    void SetScaleX(double scaleX);

    /** Set current view's Y scale and refresh display.
        @param scaleY New scale, must not be 0.
    */
    void SetScaleY(double scaleY) { if (scaleY!=0) m_scaleY=scaleY; UpdateAll(); }

    /** Set current view's X position and refresh display.
        @param posX New position that corresponds to the center point of the view.
    */
    void SetPosX(double posX) { m_posX=posX; UpdateAll(); }

    /** Set current view's Y position and refresh display.
        @param posY New position that corresponds to the center point of the view.
    */
    void SetPosY(double posY) { m_posY=posY; UpdateAll(); }

    /** Set current view's X and Y position and refresh display.
        @param posX New position that corresponds to the center point of the view.
        @param posY New position that corresponds to the center point of the view.
    */
    void SetPos( double posX, double posY) { m_posX=posX; m_posY=posY; UpdateAll(); }

    /** Set current view's dimensions in device context units.
        Needed by plotting functions. It doesn't refresh display.
        @param scrX New position that corresponds to the center point of the view.
        @param scrY New position that corresponds to the center point of the view.
    */
    void SetScr( int scrX, int scrY) { m_scrX=scrX; m_scrY=scrY; }

    /** Set mouse zoom mode
     */
    void SetMouseZoomMode(mpMouseZoomType t){m_mouseZoomMode = t;}

    /** Get mouse zoom mode
     */
    mpMouseZoomType GetMouseZoomMode(){ return m_mouseZoomMode;}

    /** Converts mpWindow (screen) pixel coordinates into graph (floating point) coordinates, using current mpWindow position and scale.
      * @sa p2y,x2p,y2p */
//     double p2x(wxCoord pixelCoordX, bool drawOutside = true ); // { return m_posX + pixelCoordX/m_scaleX; }
    inline double p2x(wxCoord pixelCoordX ) { return m_posX + pixelCoordX/m_scaleX; }

    /** Converts mpWindow (screen) pixel coordinates into graph (floating point) coordinates, using current mpWindow position and scale.
      * @sa p2x,x2p,y2p */
//     double p2y(wxCoord pixelCoordY, bool drawOutside = true ); //{ return m_posY - pixelCoordY/m_scaleY; }
    inline double p2y(wxCoord pixelCoordY ) { return m_posY - pixelCoordY/m_scaleY; }

    /** Converts graph (floating point) coordinates into mpWindow (screen) pixel coordinates, using current mpWindow position and scale.
      * @sa p2x,p2y,y2p */
//     wxCoord x2p(double x, bool drawOutside = true); // { return (wxCoord) ( (x-m_posX) * m_scaleX); }
    inline wxCoord x2p(double x) { return (wxCoord) ( (x-m_posX) * m_scaleX); }

    /** Converts graph (floating point) coordinates into mpWindow (screen) pixel coordinates, using current mpWindow position and scale.
      * @sa p2x,p2y,x2p */
//     wxCoord y2p(double y, bool drawOutside = true); // { return (wxCoord) ( (m_posY-y) * m_scaleY); }
    inline wxCoord y2p(double y) { return (wxCoord) ( (m_posY-y) * m_scaleY); }


    /** Enable/disable the double-buffering of the window, eliminating the flicker (default=disabled).
     */
    void EnableDoubleBuffer( bool enabled ) { m_enableDoubleBuffer = enabled; }

    /** Enable/disable the feature of pan/zoom with the mouse (default=enabled)
     */
    void EnableMousePanZoom( bool enabled ) { m_enableMouseNavigation = enabled; }
    /** Enable/disable the feature of popup with the mouse (default=enabled)
     */
    void EnableMousePopup( bool enabled ) { m_enableMousePopup = enabled; }

    /** Enable or disable X/Y scale aspect locking for the view.
        @note Explicit calls to mpWindow::SetScaleX and mpWindow::SetScaleY will set
              an unlocked aspect, but any other action changing the view scale will
              lock the aspect again.
    */
    void LockAspect(bool enable = TRUE);

    void SetBoundLockMinY( bool lock ) { m_lockBoundMinY = lock; }
    void SetBoundLockMaxY( bool lock ) { m_lockBoundMaxY = lock; }
    void SetBoundLockMinX( bool lock ) { m_lockBoundMinX = lock; }
    void SetBoundLockMaxX( bool lock ) { m_lockBoundMaxX = lock; }

    /** Checks whether the X/Y scale aspect is locked.
        @retval TRUE Locked
        @retval FALSE Unlocked
    */
    inline bool IsAspectLocked() { return m_lockaspect; }

    /** Set view to fit global bounding box of all plot layers and refresh display.
        Scale and position will be set to a show all attached mpLayers.
        The X/Y scale aspect lock is taken into account.
    */
    void Fit();

    /** Set view to fit a given bounding box and refresh display.
        The X/Y scale aspect lock is taken into account.
	If provided, the parameters printSizeX and printSizeY are taken as the DC size, and the
        pixel scales are computed accordingly. Also, in this case the passed borders are not saved 
        as the "desired borders", since this use will be invoked only when printing.
    */
    void Fit(double xMin, double xMax, double yMin, double yMax,wxCoord *printSizeX=NULL,wxCoord *printSizeY=NULL);

    /** Zoom into current view and refresh display
      * @param centerPoint The point (pixel coordinates) that will stay in the same position on the screen after the zoom (by default, the center of the mpWindow).
      */
    void ZoomIn( const wxPoint& centerPoint = wxDefaultPosition );

    /** Zoom out current view and refresh display
      * @param centerPoint The point (pixel coordinates) that will stay in the same position on the screen after the zoom (by default, the center of the mpWindow).
      */
    void ZoomOut( const wxPoint& centerPoint = wxDefaultPosition );

    /** Zoom in current view along X and refresh display */
    void ZoomInX();
    /** Zoom out current view along X and refresh display */
    void ZoomOutX();
    /** Zoom in current view along Y and refresh display */
    void ZoomInY();
    /** Zoom out current view along Y and refresh display */
    void ZoomOutY();

    /** Zoom view fitting given coordinates to the window (p0 and p1 do not need to be in any specific order) */
    void ZoomRect(wxPoint p0, wxPoint p1);

    /** Refresh display */
    void UpdateAll();

    // Added methods by Davide Rondini

    /** Counts the number of plot layers, excluding axes or text: this is to count only the layers which have a bounding box.
    	\return The number of profiles plotted.
    */
    unsigned int CountLayers();

    /** Counts the number of plot layers, whether or not they have a bounding box.
    	\return The number of layers in the mpWindow. */
    unsigned int CountAllLayers() { return m_layers.size(); };

    /** Draws the mpWindow on a page for printing
        \param print the mpPrintout where to print the graph */
    //void PrintGraph(mpPrintout *print);


	/** Returns the left-border layer coordinate that the user wants the mpWindow to show (it may be not exactly the actual shown coordinate in the case of locked aspect ratio).
	  * @sa Fit
   	  */
	double GetDesiredXmin() {return m_desiredXmin; }

	/** Returns the right-border  layer coordinate that the user wants the mpWindow to show (it may be not exactly the actual shown coordinate in the case of locked aspect ratio).
	  * @sa Fit
   	  */
	double GetDesiredXmax() {return m_desiredXmax; }

	/** Returns the bottom-border  layer coordinate that the user wants the mpWindow to show (it may be not exactly the actual shown coordinate in the case of locked aspect ratio).
	  * @sa Fit
   	  */
	double GetDesiredYmin() {return m_desiredYmin; }

	/** Returns the top layer-border  coordinate that the user wants the mpWindow to show (it may be not exactly the actual shown coordinate in the case of locked aspect ratio).
	  * @sa Fit
   	  */
	double GetDesiredYmax() {return m_desiredYmax; }

	/** Returns the bounding box coordinates
		@param bbox Pointer to a 6-element double array where to store bounding box coordinates. */
	void GetBoundingBox(double* bbox);

    /** Enable/disable scrollbars
      @param status Set to true to show scrollbars */
    void SetMPScrollbars(bool status);

    /** Get scrollbars status.
      @return true if scrollbars are visible */
    bool GetMPScrollbars() {return m_enableScrollBars; };

    /** Draw the window on a wxBitmap, then save it to a file.
      @param filename File name where to save the screenshot
      @param type image type to be saved: see wxImage output file types for flags
	  @param imageSize Set a size for the output image. Default is the same as the screen size
	  @param fit Decide whether to fit the plot into the size*/
    bool SaveScreenshot(const wxString& filename, wxBitmapType type = wxBITMAP_TYPE_BMP, wxSize imageSize = wxDefaultSize, bool fit = false);

    /** This value sets the zoom steps whenever the user clicks "Zoom in/out" or performs zoom with the mouse wheel.
      *  It must be a number above unity. This number is used for zoom in, and its inverse for zoom out. Set to 1.5 by default. */
    static double zoomIncrementalFactor;

    //void SetMaximumXZoomLevel(double level){ m_MaximumXZoomLevel = level; }

    /** Set window margins, creating a blank area where some kind of layers cannot draw. This is useful for example to draw axes outside the area where the plots are drawn.
        @param top Top border
        @param right Right border
        @param bottom Bottom border
        @param left Left border */
    void SetMargins(int top, int right, int bottom, int left);

    /** Set the top margin. @param top Top Margin */
    void SetMarginTop(int top) { m_marginTop = top; };
    /** Set the right margin. @param right Right Margin */
    void SetMarginRight(int right) { m_marginRight = right; };
    /** Set the bottom margin. @param bottom Bottom Margin */
    void SetMarginBottom(int bottom) { m_marginBottom = bottom; };
    /** Set the left margin. @param left Left Margin */
    void SetMarginLeft(int left) { m_marginLeft = left; };

    /** Get the top margin. @param top Top Margin */
    int GetMarginTop() { return m_marginTop; };
    /** Get the right margin. @param right Right Margin */
    int GetMarginRight() { return m_marginRight; };
    /** Get the bottom margin. @param bottom Bottom Margin */
    int GetMarginBottom() { return m_marginBottom; };
    /** Get the left margin. @param left Left Margin */
    int GetMarginLeft() { return m_marginLeft; };

    /** Enable / disable gradien backcolour */
    void SetGradienBackColour(bool b){ m_gradienBackColour=b; }
    /** Set gradien background initial colour */
    void SetGradienInitialColour(wxColour colour){ m_gradienInitialColour = colour; }
    /** Set gradien background destination colour */
    void SetGradienDestColour(wxColour colour){ m_gradienDestColour = colour; }
    /**
    *   Set gradien background direction
    * wxUP | wxLEFT | ?wxRIGHT | ?wxUP | ?wxDOWN | ?wxTOP | ?wxBOTTOM | ?wxNORTH | ?wxSOUTH | ?wxWEST | ?wxEAST | ?wxALL
    */
    void SetGradienDirect(wxDirection direct){ m_gradienDirect = direct; }



    /** Sets whether to show coordinate tooltip when mouse passes over the plot. \param value true for enable, false for disable */
    // void EnableCoordTooltip(bool value = true);
    /** Gets coordinate tooltip status. \return true for enable, false for disable */
    // bool GetCoordTooltip() { return m_coordTooltip; };

    /** Check if a given point is inside the area of a mpInfoLayer and eventually returns its pointer.
        @param point The position to be checked
        @return If an info layer is found, returns its pointer, NULL otherwise */
    mpInfoLayer* IsInsideInfoLayer(wxPoint& point);

    mpPointLayer* IsInsidePointLayer(wxPoint& point);

	/** Sets the visibility of a layer by its name.
		@param name The layer name to set visibility
		@param viewable the view status to be set */
	void SetLayerVisible(const wxString &name, bool viewable);

	/** Check whether a layer with given name is visible
		@param name The layer name
		@return layer visibility status */
	bool IsLayerVisible(const wxString &name );

	/** Sets the visibility of a layer by its position in layer list.
		@param position The layer position in layer list
		@param viewable the view status to be set */
	void SetLayerVisible(const unsigned int position, bool viewable);

	/** Check whether the layer at given position is visible
		@param position The layer position in layer list
		@return layer visibility status */
	bool IsLayerVisible(const unsigned int position );
	/** Set Color theme. Provide colours to set a new colour theme.
	    @param bgColour Background colour
		@param drawColour The colour used to draw all elements in foreground, axes excluded
		@param axesColour The colour used to draw axes (but not their labels) */
	void SetColourTheme(const wxColour& bgColour, const wxColour& drawColour, const wxColour& axesColour);

	/** Get axes draw colour
		@return reference to axis colour used in theme */
	const wxColour& GetAxesColour() { return m_axColour; };


    void SetHelpString(wxString msg, wxString title){ OnMouseHelpString=msg; OnMouseHelpStringTitle=title;}

protected:
    void OnPaint         (wxPaintEvent     &event); //!< Paint handler, will plot all attached layers
    void OnSize          (wxSizeEvent      &event); //!< Size handler, will update scroll bar sizes
    // void OnScroll2       (wxScrollWinEvent &event); //!< Scroll handler, will move canvas

    void OnCenter        (wxCommandEvent   &event); //!< Context menu handler
    void OnFit           (wxCommandEvent   &event); //!< Context menu handler
    void OnZoomIn        (wxCommandEvent   &event); //!< Context menu handler
    void OnZoomOut       (wxCommandEvent   &event); //!< Context menu handler
    void OnLockAspect    (wxCommandEvent   &event); //!< Context menu handler
    void OnMouseHelp     (wxCommandEvent   &event); //!< Context menu handler
    void OnMouseWheel    (wxMouseEvent     &event); //!< Mouse handler for the wheel
    void OnMouseMove     (wxMouseEvent     &event); //!< Mouse handler for mouse motion (for pan)
    void OnMouseLeftDown (wxMouseEvent     &event); //!< Mouse left click (for rect zoom)
    void OnMouseLeftRelease (wxMouseEvent  &event); //!< Mouse left click (for rect zoom)
    void OnMouseRightDown(wxMouseEvent     &event); //!< Mouse handler, for detecting when the user drags with the right button or just "clicks" for the menu
    void OnMouseRightRelease(wxMouseEvent  &event); //!< Mouse right up
    void OnMouseMiddleDown(wxMouseEvent     &event); //!< Mouse middle down
    void OnMouseMiddleRelease(wxMouseEvent  &event); //!< Mouse middle up
    void OnShowPopupMenu (wxMouseEvent     &event); //!< Mouse handler, will show context menu
    void OnMouseLeaveWindow(wxMouseEvent   &event); //!< Mouse leaves window

    void OnScrollThumbTrack (wxScrollWinEvent &event); //!< Scroll thumb on scroll bar moving
    void OnScrollPageUp     (wxScrollWinEvent &event); //!< Scroll page up
    void OnScrollPageDown   (wxScrollWinEvent &event); //!< Scroll page down
    void OnScrollLineUp     (wxScrollWinEvent &event); //!< Scroll line up
    void OnScrollLineDown   (wxScrollWinEvent &event); //!< Scroll line down
    void OnScrollTop        (wxScrollWinEvent &event); //!< Scroll to top
    void OnScrollBottom     (wxScrollWinEvent &event); //!< Scroll to bottom

    void DoScrollCalc    (const int position, const int orientation);

    void DoZoomInXCalc   (const int         staticXpixel);
    void DoZoomInYCalc   (const int         staticYpixel);
    void DoZoomOutXCalc  (const int         staticXpixel);
    void DoZoomOutYCalc  (const int         staticYpixel);

    /** Recalculate global layer bounding box, and save it in m_minX,...
      * \return true if there is any valid BBox information.
      */
    virtual bool UpdateBBox();

    //wxList m_layers;    //!< List of attached plot layers
    wxLayerList m_layers; //!< List of attached plot layers
    wxMenu m_popmenu;   //!< Canvas' context menu
    bool   m_lockaspect;//!< Scale aspect is locked or not

    bool   m_lockBoundMinY; //!< this lock Y min axis so user cannot move chart over this.
    bool   m_lockBoundMaxY; //!< this lock Y max axis so user cannot move chart over this.
    bool   m_lockBoundMinX; //!< this lock X min axis so user cannot move chart over this.
    bool   m_lockBoundMaxX; //!< this lock X max axis so user cannot move chart over this.


    // bool   m_coordTooltip; //!< Selects whether to show coordinate tooltip
	wxColour m_bgColour;	//!< Background Colour
	wxColour m_fgColour;	//!< Foreground Colour
	wxColour m_axColour;	//!< Axes Colour

    double m_minX;      //!< Global layer bounding box, left border incl.
    double m_maxX;      //!< Global layer bounding box, right border incl.
    double m_minY;      //!< Global layer bounding box, bottom border incl.
    double m_maxY;      //!< Global layer bounding box, top border incl.
    double m_scaleX;    //!< Current view's X scale
    double m_scaleY;    //!< Current view's Y scale

    /*double m_scaleXmin;    //!< X scale minimum value
    double m_scaleXmax;    //!< X scale maximum value
    double m_scaleYmin;    //!< Y scale minimum value
    double m_scaleYmax;    //!< Y scale maximum value
    bool   m_scaleBoundsEnable; //!< enable scale bounds*/

    double m_posX;      //!< Current view's X position
    double m_posY;      //!< Current view's Y position
    int    m_scrX;      //!< Current view's X dimension
    int    m_scrY;      //!< Current view's Y dimension
    int    m_clickedX;  //!< Last mouse click X position, for centering and zooming the view
    int    m_clickedY;  //!< Last mouse click Y position, for centering and zooming the view

    /** These are updated in Fit() only, and may be different from the real borders (layer coordinates) only if lock aspect ratio is true.
      */
    double m_desiredXmin,m_desiredXmax,m_desiredYmin,m_desiredYmax;

    int m_marginTop, m_marginRight, m_marginBottom, m_marginLeft;

    int         m_last_lx,m_last_ly;   //!< For double buffering
    wxMemoryDC  m_buff_dc;             //!< For double buffering
    wxBitmap    *m_buff_bmp;            //!< For double buffering
    bool        m_enableDoubleBuffer;  //!< For double buffering
    bool        m_enableMouseNavigation;  //!< For pan/zoom with the mouse.
    bool        m_mouseDownHasHappened;     //!< For filtering mouse release event without having had a mouse down event.
    bool        m_enableMousePopup;         //!< For popup menu
    bool        m_mouseMovedAfterRightClick;
    bool        m_mouseMovedAfterMiddleClickWithCtrl;
    int         m_mousePosition_X, m_mousePosition_Y; //!< current mouse position
    int         m_mouseRClick_X, m_mouseRClick_Y; //!< For the right button "drag" feature
    int         m_mouseLClick_X, m_mouseLClick_Y; //!< Starting coords for rectangular zoom selection
    int         m_mouseMClick_X, m_mouseMClick_Y; //!< Starting coords for rectangular zoom selection
    bool        m_enableScrollBars;
    int         m_scrollX, m_scrollY;
    mpInfoLayer* m_movingInfoLayer;      //!< For moving info layers over the window area
    mpPointLayer* m_movingPointLayer;    //!< For moving point layers over the graph line

    mpMouseZoomType m_mouseZoomMode;        //!< Default mouse zoom works 2-dimensionsly x+y
    //double      m_MaximumXZoomLevel;         //!< Maximum zoom level.

    bool        m_gradienBackColour;
    wxColour    m_gradienInitialColour;
    wxColour    m_gradienDestColour;
    wxDirection m_gradienDirect;
    wxRect      *m_zoomingRect;
    bool        m_zoomingHorizontally;

    wxString    OnMouseHelpString;
    wxString    OnMouseHelpStringTitle;


    DECLARE_DYNAMIC_CLASS(mpWindow)
    DECLARE_EVENT_TABLE()
};

//-----------------------------------------------------------------------------
// mpFXYVector - provided by Jose Luis Blanco
//-----------------------------------------------------------------------------

/** A class providing graphs functionality for a 2D plot (either continuous or a set of points), from vectors of data.
     This class can be used directly, the user does not need to derive any new class. Simply pass the data as two vectors
     with the same length containing the X and Y coordinates to the method SetData.

     To generate a graph with a set of points, call
     \code
     layerVar->SetContinuity(false)
     \endcode

     or

     \code
     layerVar->SetContinuity(true)
     \endcode

     to render the sequence of coordinates as a continuous line.

     (Added: Jose Luis Blanco, AGO-2007)
*/
class WXDLLIMPEXP_MATHPLOT mpFXYVector : public mpFXY
{
public:
    /** @param name  Label
        @param flags Label alignment, pass one of #mpALIGN_NE, #mpALIGN_NW, #mpALIGN_SW, #mpALIGN_SE.
    */
    mpFXYVector(wxString name = wxEmptyString, int flags = mpALIGN_NE);

    /** Changes the internal data: the set of points to draw.
        Both vectors MUST be of the same length. This method DOES NOT refresh the mpWindow, do it manually.
      * @sa Clear
    */
    void SetData( const std::vector<double> &xs,const std::vector<double> &ys);

    /** Clears all the data, leaving the layer empty.
      * @sa SetData
      */
    void Clear();

protected:
    /** The internal copy of the set of data to draw.
      */
    std::vector<double>  m_xs,m_ys;

    /** The internal counter for the "GetNextXY" interface
      */
    size_t              m_index;

    /** Loaded at SetData
      */
    double              m_minX,m_maxX,m_minY,m_maxY;

    /** Rewind value enumeration with mpFXY::GetNextXY.
        Overridden in this implementation.
    */
    void Rewind();

    /** Get locus value for next N.
        Overridden in this implementation.
        @param x Returns X value
        @param y Returns Y value
    */
    bool GetNextXY(double & x, double & y);

    /** Returns the actual minimum X data (loaded in SetData).
      */
    double GetMinX() { return m_minX; }

    /** Returns the actual minimum Y data (loaded in SetData).
      */
    double GetMinY() { return m_minY; }

    /** Returns the actual maximum X data (loaded in SetData).
      */
    double GetMaxX() { return m_maxX; }

    /** Returns the actual maximum  Y data (loaded in SetData).
      */
    double GetMaxY() { return m_maxY; }

    int     m_flags; //!< Holds label alignment

    DECLARE_DYNAMIC_CLASS(mpFXYVector)
};

//-----------------------------------------------------------------------------
// mpText - provided by Val Greene
//-----------------------------------------------------------------------------

/** Plot layer implementing a text string.
The text is plotted using a percentage system 0-100%, so the actual
coordinates for the location are not required, and the text stays
on the plot reguardless of the other layers location and scaling
factors.
*/
class WXDLLIMPEXP_MATHPLOT mpText : public mpLayer
{
public:
    /** @param name text to be drawn in the plot
        @param offsetx holds offset for the X location in percentage (0-100)
        @param offsety holds offset for the Y location in percentage (0-100) */
    mpText(wxString name = wxT("Title"), int offsetx = 5, int offsety = 50);

    /** Text Layer plot handler.
        This implementation will plot text adjusted to the visible area. */
    virtual void Plot(wxDC & dc, mpWindow & w);

    /** mpText should not be used for scaling decisions. */
    virtual bool HasBBox() { return FALSE; }

protected:
    int m_offsetx; //!< Holds offset for X in percentage
    int m_offsety; //!< Holds offset for Y in percentage

    DECLARE_DYNAMIC_CLASS(mpText)
};


//-----------------------------------------------------------------------------
// mpPrintout - provided by Davide Rondini
//-----------------------------------------------------------------------------

/** Printout class used by mpWindow to draw in the objects to be printed.
    The object itself can then used by the default wxWidgets printing system
    to print mppWindow objects.
*/
class WXDLLIMPEXP_MATHPLOT mpPrintout : public wxPrintout
{
public:
    mpPrintout(mpWindow* drawWindow, const wxChar *title = _T("wxMathPlot print output"));
    virtual ~mpPrintout() {};

    void SetDrawState(bool drawState) {drawn = drawState;};
    bool OnPrintPage(int page);
    bool HasPage(int page);

private:
    bool drawn;
    mpWindow *plotWindow;
};


//-----------------------------------------------------------------------------
// mpMovableObject  - provided by Jose Luis Blanco
//-----------------------------------------------------------------------------
/** This virtual class represents objects that can be moved to an arbitrary 2D location+rotation.
  *  The current transformation is set through SetCoordinateBase.
  *  To ease the implementation of descendent classes, mpMovableObject will
  *  be in charge of Bounding Box computation and layer rendering, assuming that
  *  the object updates its shape in m_shape_xs & m_shape_ys.
  */
class WXDLLIMPEXP_MATHPLOT mpMovableObject : public mpLayer
{
public:
    /** Default constructor (sets location and rotation to (0,0,0))
      */
    mpMovableObject( ) :
        m_reference_x(0),
        m_reference_y(0),
        m_reference_phi(0),
        m_shape_xs(0),
        m_shape_ys(0)
    {
        m_type  = mpLAYER_PLOT;
		m_flags = 0;
		m_bbox_min_x=0;
		m_bbox_max_x=0;
		m_bbox_min_y=0;
		m_bbox_max_y=0;
    }

    virtual ~mpMovableObject() {};

    /** Get the current coordinate transformation.
      */
    void GetCoordinateBase( double &x, double &y, double &phi ) const
    {
        x = m_reference_x;
        y = m_reference_y;
        phi = m_reference_phi;
    }

    /** Set the coordinate transformation (phi in radians, 0 means no rotation).
      */
    void SetCoordinateBase( double x, double y, double phi = 0 )
    {
        m_reference_x = x;
        m_reference_y = y;
        m_reference_phi = phi;
        m_flags  = mpALIGN_NE;
        ShapeUpdated();
    }

    virtual bool IsMovableLayer() { return true; }
    virtual bool Inside(const wxPoint& point){ return false; }

    virtual bool HasBBox() { return m_trans_shape_xs.size()!=0; }

    /** Get inclusive left border of bounding box.
    */
    virtual double GetMinX() { return m_bbox_min_x; }

    /** Get inclusive right border of bounding box.
    */
    virtual double GetMaxX() { return  m_bbox_max_x; }

    /** Get inclusive bottom border of bounding box.
    */
    virtual double GetMinY() { return m_bbox_min_y; }

    /** Get inclusive top border of bounding box.
    */
    virtual double GetMaxY() { return m_bbox_max_y; }

    virtual void   Plot(wxDC & dc, mpWindow & w);

    /** Set label axis alignment.
      *  @param align alignment (choose between mpALIGN_NE, mpALIGN_NW, mpALIGN_SW, mpALIGN_SE
      */
    void SetAlign(int align) { m_flags = align; };

protected:
    int m_flags; //!< Holds label alignment

    /** The coordinates of the object (orientation "phi" is in radians).
      */
    double m_reference_x,m_reference_y,m_reference_phi;

    /** A method for 2D translation and rotation, using the current transformation stored in m_reference_x,m_reference_y,m_reference_phi.
      */
    void TranslatePoint( double x,double y, double &out_x, double &out_y );

    /** This contains the object points, in local coordinates (to be transformed by the current transformation).
      */
    std::vector<double>  m_shape_xs,m_shape_ys;

    /** The buffer for the translated & rotated points (to avoid recomputing them with each mpWindow refresh).
      *
      */
    std::vector<double>  m_trans_shape_xs,m_trans_shape_ys;

    /** The precomputed bounding box:
      * @sa ShapeUpdated
      */
    double  m_bbox_min_x,m_bbox_max_x,m_bbox_min_y,m_bbox_max_y;

    /** Must be called by the descendent class after updating the shape (m_shape_xs/ys), or when the transformation changes.
      *  This method updates the buffers m_trans_shape_xs/ys, and the precomputed bounding box.
      */
    void ShapeUpdated();

};

//-----------------------------------------------------------------------------
// mpCovarianceEllipse  - provided by Jose Luis Blanco
//-----------------------------------------------------------------------------
/** A 2D ellipse, described by a 2x2 covariance matrix.
  *  The relation between the multivariate Gaussian confidence interval and
  *   the "quantiles" in this class is:
  *     - 1 : 68.27% confidence interval
  *     - 2 : 95.45%
  *     - 3 : 99.73%
  *     - 4 : 99.994%
  * For example, see http://en.wikipedia.org/wiki/Normal_distribution#Standard_deviation_and_confidence_intervals
  *
  * The ellipse will be always centred at the origin. Use mpMovableObject::SetCoordinateBase to move it.
  */
class WXDLLIMPEXP_MATHPLOT mpCovarianceEllipse : public mpMovableObject
{
public:
    /** Default constructor.
      * Initializes to a unity diagonal covariance matrix, a 95% confidence interval (2 sigmas), 32 segments, and a continuous plot (m_continuous=true).
      */
    mpCovarianceEllipse(
        double cov_00 = 1,
        double cov_11 = 1,
        double cov_01 = 0,
        double quantiles = 2,
        int    segments = 32,
        const wxString & layerName = wxT("") ) :
            m_cov_00(cov_00),
            m_cov_11(cov_11),
            m_cov_01(cov_01),
            m_quantiles(quantiles),
            m_segments(segments)
    {
        m_continuous = true;
        m_name = layerName;
        RecalculateShape();
        m_type = mpLAYER_PLOT;
    }

    virtual ~mpCovarianceEllipse() {}

    double GetQuantiles() const { return m_quantiles; }

    /** Set how many "quantiles" to draw, that is, the confidence interval of the ellipse (see above).
      */
    void SetQuantiles(double q)
    {
        m_quantiles=q;
        RecalculateShape();
    }

    void SetSegments( int segments ) { m_segments = segments; }
    int GetSegments( ) const { return m_segments; }

    /** Returns the elements of the current covariance matrix:
      */
    void GetCovarianceMatrix( double &cov_00,double &cov_01,double &cov_11 ) const
    {
        cov_00 = m_cov_00;
        cov_01 = m_cov_01;
        cov_11 = m_cov_11;
    }

    /** Changes the covariance matrix:
      */
    void SetCovarianceMatrix( double cov_00,double cov_01,double cov_11 )
    {
        m_cov_00 = cov_00;
        m_cov_01 = cov_01;
        m_cov_11 = cov_11;
        RecalculateShape();
    }

protected:
    /** The elements of the matrix (only 3 since cov(0,1)=cov(1,0) in any positive definite matrix).
      */
    double m_cov_00,m_cov_11,m_cov_01;
    double m_quantiles;

    /** The number of line segments that build up the ellipse.
      */
    int m_segments;

    /** Called to update the m_shape_xs, m_shape_ys vectors, whenever a parameter changes.
      */
    void RecalculateShape();
};

//-----------------------------------------------------------------------------
// mpPolygon - provided by Jose Luis Blanco
//-----------------------------------------------------------------------------
/** An arbitrary polygon, descendant of mpMovableObject.
  *  Use "setPoints" to set the list of N points. This class also can draw non-closed polygons by
  *   passing the appropriate parameters to "setPoints". To draw a point-cloud, call "SetContinuity(false)".
  */
class WXDLLIMPEXP_MATHPLOT mpPolygon : public mpMovableObject
{
public:
    /** Default constructor.
      */
    mpPolygon( const wxString & layerName = wxT("") )
    {
        m_continuous = true;
        m_name = layerName;
    }

    virtual ~mpPolygon() {}

    /** Set the points in the polygon.
      * @param points_xs  The X coordinates of the points.
      * @param points_ys  The Y coordinates of the points.
      * @param closedShape If set to true, an additional segment will be added from the last to the first point.
      */
    void setPoints(
        const std::vector<double>&  points_xs,
        const std::vector<double>&  points_ys,
        bool                   closedShape=true );



};

//-----------------------------------------------------------------------------
// mpMovableObject  - provided by Jose Luis Blanco
//-----------------------------------------------------------------------------
/** This virtual class represents objects that can be moved to an arbitrary 2D location+rotation.
  *  The current transformation is set through SetCoordinateBase.
  *  To ease the implementation of descendent classes, mpMovableObject will
  *  be in charge of Bounding Box computation and layer render, assuming that
  *  the object updates its shape in m_shape_xs & m_shape_ys.
  */
class WXDLLIMPEXP_MATHPLOT mpBitmapLayer : public mpLayer
{
public:
    /** Default constructor.
      */
    mpBitmapLayer( )
    {
        m_min_x = m_max_x =
        m_min_y = m_max_y = 0;
        m_validImg = false;
        m_type = mpLAYER_BITMAP;
		m_flags = 0;
    }

    virtual ~mpBitmapLayer() {};

    /** Returns a copy of the current bitmap assigned to the layer.
      */
    void GetBitmapCopy( wxImage &outBmp ) const;

    /** Change the bitmap associated with the layer (to update the screen, refresh the mpWindow).
      *  @param inBmp The bitmap to associate. A copy is made, thus it can be released after calling this.
      *  @param x The left corner X coordinate (in plot units).
      *  @param y The top corner Y coordinate (in plot units).
      *  @param lx The width in plot units.
      *  @param ly The height in plot units.
      */
    void SetBitmap( const wxImage &inBmp, double x, double y, double lx, double ly );

    virtual bool HasBBox() { return true; }

    /** Get inclusive left border of bounding box.
    */
    virtual double GetMinX() { return m_min_x; }

    /** Get inclusive right border of bounding box.
    */
    virtual double GetMaxX() { return  m_max_x; }

    /** Get inclusive bottom border of bounding box.
    */
    virtual double GetMinY() { return m_min_y; }

    /** Get inclusive top border of bounding box.
    */
    virtual double GetMaxY() { return m_max_y; }

    virtual void   Plot(wxDC & dc, mpWindow & w);

    /** Set label axis alignment.
      *  @param align alignment (choose between mpALIGN_NE, mpALIGN_NW, mpALIGN_SW, mpALIGN_SE
      */
    void SetAlign(int align) { m_flags = align; };

protected:
    int m_flags; //!< Holds label alignment

    /** The internal copy of the Bitmap:
      */
    wxImage      m_bitmap;
    wxBitmap     m_scaledBitmap;
    wxCoord      m_scaledBitmap_offset_x,m_scaledBitmap_offset_y;


    bool            m_validImg;


    /** The shape of the bitmap:
      */
    double  m_min_x,m_max_x,m_min_y,m_max_y;


};
//-----------------------------------------------------------------------------
// mpPointLayer  - provided by Jussi Vatjus-Anttila
//-----------------------------------------------------------------------------
/** This virtual class represents objects that consist one changeable point (currently only circle).
  *	Circle is placed to another mpLayer spesific place (currently only mpFXY is implemented[implementation can be better..] )
  */


enum mpTaggLine
{
     mpVERTICAL_LINE=1
    ,mpHORIZONTAL_LINE=2
    ,mpCROSS_LINES=4
};
class polygon
{
	private:
		PointList m_polygon;

	public:
		polygon(PointList poly)                 { m_polygon = poly; }
		~polygon()                              { }
		polygon(const polygon& poly)            { m_polygon = poly.GetPolygon(); } //copy constructor
		void SetPolygon(const PointList& poly)  { m_polygon = poly; }
		PointList GetPolygon() const            { return m_polygon; }
		PointList get(double angle=0, double scale=1)
		{
			PointList out;
			PointList::iterator it;
			for(it=m_polygon.begin();it!=m_polygon.end();++it)
			{
				out.Append( rotate(**it, angle, scale) );
			}
			return out;
		}

	private:
		/**
		*             _          _
		*            |  m11  m12  |
		*       [x y]|  m21  m22  | = [m11*x + m21*y, m12*x + m22*y]
		*            |_ 0    0   _|
		*
		*       Rotation Clockwise by Ø, scaling isotropic by s
		*             _                    _
		*            | s * cos Ø    sin Ø   |
		*       [x y]|  -sin Ø    s * cos Ø |=[sx*(cos Ø)*x-(sin Ø)*y,
		*            |_    0          0    _|  (sin Ø)*x+sy*(cos Ø)*y]
		*/
		wxPoint* rotate(wxPoint pt, double angle, double scale, bool angleInRadians=false)
		{
			 wxPoint *point = new wxPoint();
			 double theta = angleInRadians?angle:2*M_PI/360*angle;
			 point->x = (int)(scale * (cos(theta) * pt.x - sin(theta) * pt.y) + 0.5);
			 point->y = (int)(scale * (sin(theta) * pt.x +  cos(theta) * pt.y) +0.5);
			 return point;
		}
};
class mpPointDeriveClass {
    public:
        mpPointDeriveClass(){}
        /** This virtual function must implement to final object */
        virtual void customPlot(wxDC & dc, mpWindow & w, double x0, double y0, double x1, double y1, wxPoint p0, wxPoint p1, int maxYpx) {}
};

class WXDLLIMPEXP_MATHPLOT mpPointLayer : public mpLayer
{
    public:
        /** Default constructor. */
        mpPointLayer(){}
        mpPointLayer( mpLayer *parent );
        /**
            Complete constructor.
            @param mpLayer    	Layer pointer of point
			@param layerName	Layer name
            @param brush 		pointer to a fill brush. Default is transparent
			@param wxWindow		parent window because wan't know where event will be send
			@param int			event type, default
			@param long			event unique id
        */
        mpPointLayer(   mpLayer         *mplayer,
                        wxString        layerName = wxT("P"),
                        const wxBrush   *brush = wxTRANSPARENT_BRUSH,
                        wxWindow        *wxparent = NULL,
                        long int        eventid = wxNewId()
                    );
        /** Destructor */
        virtual ~mpPointLayer();


        virtual bool   HasBBox() { return FALSE; }
        /** Specifies that this is an Info box layer.
            @return true if modify is enabled
            @sa mpLayer::IsPointEnabled */
        bool IsPointModifiable() { return m_EnableModify; }
        bool IsDraggingOnlyFromPoint() { return m_DragOnlyPoint; }
        /** Check if point is defined */
        bool IsPointDefined(){ return m_defined;}
        /** If parent is defined becouse of events
         *  function return event ID for Connect() */
        int  GetEventId(){return eventId;}
		/** Specifies that this is an point layer.
            @return always \a TRUE
            @sa mpLayer::IsPointLayer */
        virtual bool IsPointLayer() { return true; }

        /** Updates point. Shold be overidden by derived classes.
            @param w parent mpWindow from which to obtain information
            @param event The event which called the update. */
        virtual void UpdatePoint(mpWindow& w, wxEvent& event);
		/** Update mouse coordinates
			@param w parent mpWindow from which to obtain information
            @param event The event which called the update. */
        virtual void UpdateMouseCoord(mpWindow& w, wxEvent& event);

        void SetParent(wxWindow *parent){wxParent = parent;}
        void SetSecondPoint(mpPointLayer* layer);

        void SetShape(mpShape shape);
        void SetShapeSize(int diagonal);
        void SetTaggingShape(int shape){m_taggline = shape;}
        void SetTaggingPen( wxPen pen ) { m_taggPen = pen;}

        /** Returns the position of point
			@param	x[out] 	point x-position
			@param 	y[out]	point y-position
			@return true if it spesific */
        bool GetPosition(double &x, double &y);
		/** Set the position of point
			@param	x[in] 	point x-position
			@param 	y[in]	point y-position
			@return true if allowed*/
        bool SetPosition(double &x, double &y);
        /** Undefine point */
        void UnDefine();
		/** Returns the nearest graph position at cursor
			@param	x[out] 	cursor x-position
			@param 	y[out]	graph  y-position
			@return  true if spesific*/
        bool GetCursorPosition(double &x, double &y);

        /**
			Point is possible to move with mouseleft buttom
			@param t	enable modifity */
        void EnableModify(bool t = true){m_EnableModify = t;}

        void DragOnlyPoint(bool t = true){m_DragOnlyPoint = t;}

        /** mpPoint Layer plot handler.
        This implementation will plot point to the visible area. */
        virtual void Plot(wxDC & dc, mpWindow & w);

        virtual bool Inside(const wxPoint& point){ return pointInPolygon(point.x, point.y); }


        bool pointInPolygon(int x, int y);

        /** Get reference to context menu of the plot canvas.
        @return Pointer to menu. The menu can be modified.
        */
        wxMenu* GetPopupMenu() { return &m_popmenu; }

        void popup(wxWindow& win, wxMouseEvent& event)
        {
            win.PopupMenu( &m_popmenu, event.GetX(), event.GetY());
        }

		void ShowShadows(int shadow){ m_ShowShadows = shadow; }
		/**
		 * Set object that generate middle text.
		 * Object must derived form mpPointDeriveClass and there must be
		 * virtual function:
		 *  virtual wxString GenerateString(double x0, double x1, double y0, double y1)
		 */
		void SetExternalObject(mpPointDeriveClass *obj){ m_externalObject = obj; }

    protected:



        //for event using
        wxWindow        *wxParent;
        long int        eventId;

        mpShape         m_shape;            //!< point shape
        int             m_diagonal;			//!< point shape diagonal size

		int             m_taggline;			//!< Tagging line type
		wxPen           m_taggPen;


        //for mp/point layers
        mpLayer         *mpParent;		    //!< parent layer where get the points
        double          m_x,m_y;            //!< The bounding rectangle of the box. It may be resized dynamicaly by the Plot method.



        bool            m_defined;          //!< only true if point is defined
        bool            m_mouseDefined;     //!< only true if mouse is inside graph

        bool            m_EnableModify;		//!< If user wan't change point place with mouse
        bool            m_cursorInside;
        double          mouseX, mouseY;		//!< contains mouse coordinates
        bool            m_DragOnlyPoint;    //!< If true, point modifing is allways possible, but only when mouse draq point

        PointList       m_polygon;

        //second point variables
		mpPointLayer*   m_SecondPointLayer;		//!< pointer to 2. mpPointLayer. ex. Calculate distance of 2 points.
		int				m_ShowShadows;			//!< show shadows betweeb 2 points
		mpPointDeriveClass *m_externalObject;			//!< text shown in middle of 2 points.

        wxTipWindow*    m_tooltip;
        wxMenu          m_popmenu;              //!< user handled popupmenu when clicked the point shape
        //bool            m_showHelpLine;
    DECLARE_DYNAMIC_CLASS(mpPointLayer)
};

/*@}*/

#endif // _MP_MATHPLOT_H_
