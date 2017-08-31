/**
 *	@file		gpPanel.h
 *	@author     Jussi Vatjus-Anttila
 *	@date		10.09.2009
 *	@version	0.2

 *
 *	@note
 *  There is decrlarations all the gpPanel functions

 *  @mainpage

   Main purpose of gpPanel library is to provide a graphical
   presentation of information a general-purpose interface (API).<br><br>
   Library is designed for using with wxWidget. It is based on a
   custom wxMathplot library. Library, the changes made in Section
   wxMathPlot for gpPanel.<br>

   This document presents the library API functions, as well as the
   structure of the library.


   Original Author: Jussi Vatjus-Anttila<br>
   Created:         10/09/2009<br>
   Last edit:       8/2/2010<br>
   Copyright:       (c) Jussi Vatjus-Anttila<br>

    @section homepage Project homepage
    <a href="http://code.google.com/p/gppanel/">Go to the project google-page.</a>

    @section overview Overview
    Class section declares all classes.<br>
	Class Hierarchy presents ingeritance of classes.<br>

	Section Examples, presented the library's use of.


    @section coding Coding conventions
	Main class, gpPanel, is inherited from wxPanel-class.
	It creates an instance from wxWindow inherited class (for example from wxFrame).



    @section author Author and license
    gpPanel is published under the terms of the wxWidget license.<br>
    The original author is Jussi Vatjus-Anttila <jussiva@gmail.com>.<br>


	@page  API API description


	This is short API description, shows how to use class.


	\section gpPanel gpPanel

	gpPanel is used to handle gpLayers. It's main class of library.<br>
	the user must create an instance and connect it charged to wxWindow class by constructor.<br>
	When user has implemented own layer (inherited from gpLayer) correctly, it can be attached to gpPanel.
	There's little code example:
	<pre>
	myFrame::myFrame(wxWindow* parent):wxFrame(parent){
		...
		gpPanel *panel = new gpPanel(this); //note gpPanel must be class variable (not like now)

		panel->addLayer(myLayer1::gpLayer);
		panel->addLayer(myLayer2::gpLayer);
	}
	</pre>


	\section gpLayer gpLayer

	gpLayer is abstract class, which must be implement by you self.
	There's couple examples for bar and line charts, but you probably need
	implement own for your project. Implement need at least:
	-virtual functions (see gpLayer)
	-.... @note commentation is not finished

	@}

 *
 */
#ifndef GPPANEL_H
#define GPPANEL_H

#ifndef WX_PRECOMP
	//(*HeadersPCH(gpPanel)
	#include <wx/panel.h>
	//*)
#endif
//(*Headers(gpPanel)
//*)

//Drag and drop
#include <wx/dnd.h>
#include <wx/clipbrd.h>
#include <wx/sizer.h>

#include <deque>
#include <map>
#include <string>


#include "mathplot.h"   //used drawing library
#include "gpLayer.h"

#define STDSTRING2WXSTRING(x)   (wxString(x.c_str(), wxConvUTF8))

#include "gpMenu.h"
#include "gpPopup.h"
#include "gpSizer.h"

/**
*   gpLayer_s stucture
*
*/
struct gpLayer_s
{
    /**
    *   constructor
    *   @param plot     mpWindow*
    *   @param l        gpLayer*
    *   @param Name     layer name
    */
    gpLayer_s(mpWindow* plot, gpLayer *l): m_plot(plot), layer(l){}
    mpWindow    *m_plot;    /// mpWindow
    gpLayer     *layer;     /// gpLayer
};
/** typedef for gpLayer deque*/
typedef std::deque< gpLayer_s* >   gpLayerList_t;

/**
 * gpLayers handler class:
 *
 * window is divided to several
 * gpLayers by Vertical wxBoxSizers
 *<pre>
 * ------------------------
 * |                      |
 * |   gpLayer            |
 * |                      |
 * |                      |
 * |----------------------|
 * |                      |
 * |   gpLayer            |
 * |                      |
 * |                      |
 * ------------------------
 *
 * New gpLayer can be attached by AddLayer() functions
 *</pre>
 *
 */
class gpPanel : public wxPanel, public gpSizer//, private wxFileDropTarget,
{


    private:
        /**
        *   Private member variables
        */
        wxBoxSizer	   *m_BoxSizer;       //!< Vertical sizer
		wxWindow 	    *m_wxParent;      //!< wxParent window (example gpPanel)
		gpLayerList_t 	m_gpLayerList;    //!< this contains all gpLayers.
        int             m_PanelWidth      //!< Panel width, used by (re)storePanelSize()
                        ,m_PanelHeight;   //!< Panel height, used by (re)stoPanelSize()
        wxClipboard     m_clipboard;
	public:

        /**
         *  \group interface    gpPanel interface
         *  This function is used for end users.
         */
        //! @{

        /**
         * gpPanel constructor
         *  @param parenet  wxWindow parent
         *  @param id       window id
         *  @param pos      window position
         *  @param size     window size
         */
        gpPanel(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
        /** Destructor
         *  -delete all gpLayer attached to gpPanel (AddLayer)
         */
		virtual ~gpPanel();


		//virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames);

		//(*Declarations(gpPanel)
		//*)


		/**
		*   Add layer to vertically
        *   @param layer    gpLayer*
        *   @return true if ok
        */
		int AddLayer( gpLayer* layer, int MenuMask = 0);

		/**
		*   Add layer to horizontalize
		*   @param id       vertical index
        *   @param layer    gpLayer*
        *   @return true if ok
        */
		bool AddLayer(unsigned int id, gpLayer* layer, int MenuMask,
                int proportion = 1 );


		/**
		*   Realize Added layers
		*/
		void RealizeLayers();

		/**
        *   @param layer    gpLayer*
        *   @param alsoDeleteObject     if want delete also gpLayer
        *   @return true if ok
        */
		bool DelLayer(gpLayer* layer, bool alsoDeleteObject = false);
		/**
		*   Delete all layers
		*   @param alsoDeleteObject     if want delete also gpLayers
		*/
		void DelAllLayers(bool alsoDeleteObjects);

		/**
		 *  Change Layer position
		 *  @param layer    layer to be moved
		 *  @param newPosition  New position of layer
		 *  @return true if ok
		 */
		//bool ChangeLayerPosition(gpLayer_s *layer, int newPosition);


        /**
        * Fit all mpWindow's
        */
		void FitAll();
		/**
		*   Fit layer
		*   @param layer    layer to Fit
		*/
		void Fit(gpLayer* layer);
		/**
		*   Fit layer with parameters
		*   @param layer    layer to be Fit
		*   @param xMin     x value to begin x-scale fitt
		*   @param xMax     x value to end x-scale fitt
		*   @param yMin     y value to begin y-scale fitt
		*   @param yMax     y value to end y-scale fitt
		*   @param printSizeX   print size, used for printing
		*   @param printSizeY   print size, used for printing
		*/
		void Fit(gpLayer* layer, double xMin, double xMax, double yMin, double yMax, wxCoord *printSizeX=0,wxCoord *printSizeY=0);
		/**
		*   Lock all mpWindow graphs
		*   @param b  lock or not to lock
		*/
		void LockAll(bool t = true);
        /**
        *   Lock aspect from layer
        *   @param layer    layer to be lock
        *   @param lock     lock boolean.
        */
		void LockAspect(gpLayer *layer, bool lock=true);
        /**
        *   Lock X axis
        *   @param layer    layer to be lock
        *   @param t        lock boolean
        */
        void LockXAxis(gpLayer *layer, bool t=true);
        /**
        *   Lock Y axis
        *   @param layer    layer to be lock
        *   @param t        lock boolean
        */
        void LockYAxis(gpLayer *layer, bool t=true);


        /**
        *   Set layer visibility
        *   @param layer  gpLayer
        *   @param viewable visibility
        *   @return true if layer found and ok
        */
        bool SetLayerVisible(gpLayer* layer, bool viewable);
        bool SetLayerVisible(gpLayer_s* layer, bool viewable);
        /**
        *   Set layer visibility
        *   @param name name of gpLayer
        *   @param viewable visibility
        *   @return true if layer found and ok
        */
		bool SetLayerVisible(const wxString &name, bool viewable);
		/**
        *   Set layer visibility
        *   @param id id of gpLayer
        *   @param viewable visibility
        *   @return true if layer found and ok
        */
		bool SetLayerVisible(int id, bool viewable);

		/**
		*   Is layer visible
		*   @param  name layer name
		*   @return true if visible
		*/
		bool IsLayerVisible(const wxString &name);

		/**
		*   Is layer visible
		*   @param  position position of layer
		*   @return true if visible
		*/
		bool IsLayerVisible(int position);
		bool IsLayerVisible(gpLayer* layer);
		/**
        *   Get Layer by menuId
        *   @param id
        *   @return gpLayer_s*
        *   Useful for example when using gpMenu
		*/
		gpLayer_s*  GetLayerByMenuId(int id);
		mpWindow*   GetWindowByMenuId(int id);
		mpWindow*   GetWindowByLayer(gpLayer*);


        /**
        * Refresh all the graphs
        */
        void Refresh();
        /**
        * if graph is uptodate eq "don't needed update",
        * tell it by return false
        */
        bool Refresh(gpLayer* layer);

        //! @}



        /**
        *   Menu-items event handlers.
        *  \ingroup eventHandlers
        *   @{
        */

		/** Graph Enable event */
		void OnMenuItemGraphEnableEvent(wxCommandEvent& event);
		/** Graph Lock Aspect event */
		void OnMenuItemGraphLockAspectEvent(wxCommandEvent& event);
		/** Graph Lock event */
		void OnMenuItemGraphLockEvent(wxCommandEvent& event);
		/** Graph Grid event */
		void OnMenuItemGridEvent(wxCommandEvent& event);
		/** Graph Markers event */
		void OnMenuItemMarkEvent(wxCommandEvent& event);
		/** Show Info Layer event*/
		void OnMenuItemShowInfo(wxCommandEvent& event);
		/** Graph Continous line event */
		void OnMenuItemContinousLine(wxCommandEvent& event);
		/** Graph Tooltip event */
		void OnMenuItemTooltip(wxCommandEvent& event);
		/** Graph Fit event */
		void OnMenuItemFit(wxCommandEvent& event);
		/** Graph Print event */
		void OnMenuItemPrint(wxCommandEvent& event);
		/** Graph Print Preview event */
		void OnMenuItemPrintPreview(wxCommandEvent& event);
		/** Graph Save screenshot event */
		void OnMenuItemSaveScreenshot(wxCommandEvent& event);
		/** Graph Export SVS event */
		void OnMenuItemExportCVS(wxCommandEvent& event);
		/** Graph Export cliboard event */
		void OnMenuItemExportClipboard(wxCommandEvent& event);
		/** Graph Change draw mode event*/
		void OnMenuItemChangeAxis(wxCommandEvent &event);
		/** Graph Change Y Axis event*/
		void OnMenuItemChangeAxisY(wxCommandEvent &event);
		/** Graph Change X Axis event*/
		void OnMenuItemChangeAxisX(wxCommandEvent &event);

        /** Key event*/
		void OnKeyDown(wxKeyEvent &event);

		//! @}

		/** Graph Export CVS */
		void OnExportCVS(gpLayer_s *glayer = 0);
		/** Graph Export Clipboard */
		void OnExportClipboard(gpLayer_s *glayer = 0);




	protected:

		//(*Identifiers(gpPanel)
		//*)

	private:


        bool AddLayer( gpLayer* layer, mpWindow*& window, int MenuMask);

        void AddPopupMenus(gpLayer_s*, int MenuMask);
        void OnMenuItemEnableSelected(wxCommandEvent& event);
        void OnMenuItemLockAllSelected(wxCommandEvent& event);
        void OnMenuItemFitAllSelected(wxCommandEvent& event);
        void OnMenuItemGrid(wxCommandEvent& event);

        void OnMenuItemFft(wxCommandEvent& event);

        void OnMenuItemChangeAxisDefault(wxCommandEvent& event);
        void OnMenuItemChangeAxisINL(wxCommandEvent& event);
        void OnMenuItemChangeAxisDNL(wxCommandEvent& event);
        void OnMenuItemChangeAxisFFT(wxCommandEvent& event);
        void OnMenuItemChangeAxisAccumulation(wxCommandEvent& event);
        void OnMenuItemChangeAxisCustom(wxCommandEvent& event);

        void OnMenuItemChangeYAxisDefault(wxCommandEvent& event);
        void OnMenuItemChangeYAxisProcent(wxCommandEvent& event);
        void OnMenuItemChangeYAxisTime(wxCommandEvent& event);
        void OnMenuItemChangeYAxisLength(wxCommandEvent& event);
        void OnMenuItemChangeYAxisN(wxCommandEvent& event);
        void OnMenuItemChangeYAxisCustom(wxCommandEvent& event);

        void OnMenuItemChangeXAxisDefault(wxCommandEvent& event);
        void OnMenuItemChangeXAxisProcent(wxCommandEvent& event);
        void OnMenuItemChangeXAxisTime(wxCommandEvent& event);
        void OnMenuItemChangeXAxisLength(wxCommandEvent& event);
        void OnMenuItemChangeXAxisN(wxCommandEvent& event);
        void OnMenuItemChangeXAxisCustom(wxCommandEvent& event);

        void OnMenuHighlight(wxCommandEvent& event);

        bool QuestString(wxString label, wxString title, wxString& out);



		gpLayer_s*	GetLayerByPosition(int position);		/// Get layer by position
		gpLayer_s*	GetLayerById(long id);					/// Get layer by id
		gpLayer_s*	GetLayerByName(const wxString name);	/// Get layer by name
		gpLayer_s*  GetLayer(gpLayer* layer);

		bool        LayerExist(gpLayer_s*);

		void        storePanelSize(){GetSize(&m_PanelWidth, &m_PanelHeight);}
		void        restorePanelSize(){wxPanel::Fit();SetSize(m_PanelWidth, m_PanelHeight);}



		//(*Handlers(gpPanel)
		//*)




		DECLARE_EVENT_TABLE()
};

#endif
