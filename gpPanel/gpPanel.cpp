/**
 *	@file		gpPanel.cpp
 *	@author     Jussi Vatjus-Anttila
 *	@date		10.09.2009
 *	@version	0.1
 *
 *  Purpose:         gpPanel implementation file.
 *  Original Author: Jussi Vatjus-Anttila
 *  Created:         10/09/2009
 *  Last edit:       21/04/2012
 *  Copyright:       (c) Jussi Vatjus-Anttila
 *  Licence:         wxWindows Library License v3.1
 *
 *	@note
 *  All gpPanel class function Implementations.
 *
 */

#include "gpPanel.h"

#ifndef WX_PRECOMP
	//(*InternalHeadersPCH(gpPanel)
	#include <wx/intl.h>
	#include <wx/string.h>
	//*)
#endif
//(*InternalHeaders(gpPanel)
//*)
#include <wx/filename.h>
#include <wx/choicdlg.h>
#include <wx/arrstr.h>
#include <wx/log.h>
#include <wx/filedlg.h>
#include <wx/textdlg.h>
//(*IdInit(gpPanel)
//*)

#include <wx/file.h>
#include "baseData.h"

#include <wx/tipwin.h>
#include "menu/gpPopup.h"


BEGIN_EVENT_TABLE(gpPanel,wxPanel)
	//(*EventTable(gpPanel)
    EVT_KEY_DOWN( gpPanel::OnKeyDown )
	//*)
END_EVENT_TABLE()

gpPanel::gpPanel(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
    //(*Initialize(gpPanel)
    Create(parent, id, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("id"));
    //*)
    //m_BoxSizer = new wxBoxSizer(wxVERTICAL);

    //SetAutoLayout( TRUE );
    //SetSizer(m_BoxSizer);
    gpSizer::SetParent(this);
    //AttachGpSizer();
    SetSizerAndFit( gpSizer::GetSizer() );
             //AttachGpSizer(this);

    // BoxSizer destructor is called when gpPanel destroyed
    // http://docs.wxwidgets.org/2.6.3/wx_wxsizer.html#wxsizerremove

	m_wxParent = parent;

	//this->SetDropTarget(this);
}

gpPanel::~gpPanel()
{
	//(*Destroy(gpPanel)
	//*)

	// BoxSizer destructor delete all its child, like mpWindow's,
    // and that destroy all mpLayer's.
    // That's why don't need delete than gpLayer_s structures..
	DelAllLayers(true);
}
/*bool gpPanel::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames)
{
    wxLogMessage(_("Dropped file. There is no handler yet."));
}*/
void gpPanel::OnKeyDown(wxKeyEvent &event)
{
    char keycode = event.m_keyCode;
    if( event.m_controlDown && keycode)
    {
        //keycode -= -0x30;
        //SetLayerVisible( GetLayerByPosition(keycode) , !IsLayerVisible(keycode) );
    }
    event.Skip();
}

void gpPanel::Refresh()
{
    gpLayerList_t::iterator it;
    for(it = m_gpLayerList.begin(); it != m_gpLayerList.end(); it++)
    {
        (*it)->layer->Refresh();    //refresh gpLayer data to mpLayers
        (*it)->m_plot->UpdateAll(); //udpate all mpLayers to display
        (*it)->m_plot->Fit();       //Fit graph

    }
}
bool gpPanel::Refresh(gpLayer* layer)
{
    gpLayer_s* l = GetLayer(layer);
    if(l)
    {
        if( l->layer->RefreshNeeded() )
        {
            l->layer->Refresh();    //refresh gpLayer data to mpLayers
            l->m_plot->UpdateAll(); //update mpLayers to display
            return true;
        }
    }
    return false;
}


int gpPanel::AddLayer(gpLayer *layer)
{
    if(layer==NULL)return false;

    //storePanelSize();
    mpWindow* newMp;
    /*bool ok = */AddLayer(layer, newMp);
    int id = AddWindow(newMp);
    //gpSizer::Realize();

    //restorePanelSize();

    return id;
}
bool gpPanel::AddLayer(unsigned int id, gpLayer* layer, int proportion)
{
    if(layer==NULL)return false;

    //storePanelSize();

    mpWindow* newMp;
    bool ok = AddLayer(layer, newMp);
    AddWindow(id, newMp, proportion);

    //restorePanelSize();
    return ok;
}
void gpPanel::RealizeLayers()
{
    gpSizer::Realize();
}
bool gpPanel::DelLayer(gpLayer *layer, bool alsoDeleteObject)
{
    if(layer == 0) return false;
    gpLayerList_t::iterator it;
    for(it = m_gpLayerList.begin(); it != m_gpLayerList.end(); it++)
    {
        if(layer == (*it)->layer)
        {

            //(*it)->layer->DelAllLayers(alsoDeleteObject);   //gplayers
            wxDELETE( (*it)->m_plot );      //this call destructor and delete layers
            wxDELETE( *it );
            m_gpLayerList.erase(it);
            return true;
        }
    }
    return false;
}
void gpPanel::DelAllLayers(bool alsoDeleteObjects)
{
    gpLayerList_t::iterator it;
    for(it = m_gpLayerList.begin(); it != m_gpLayerList.end(); it++)
    {

        wxDELETE( *it );
        //this call destructor and delete all mpLayers
        //wxDELETE( (*it)->layer );   //delete gpLayer
        //wxDELETE( (*it)->m_plot );  //delete mpWindow with all attached mpLayers
    }
    m_gpLayerList.clear();
}


//PRIVATE FUNCTIONS:

bool gpPanel::AddLayer(gpLayer *layer, mpWindow*& newMp)
{
    if(layer==NULL)return false;

    newMp = new mpWindow(this,wxNewId(), wxDefaultPosition,wxDefaultSize,0);
    gpLayer_s *slayer;

    newMp->SetToolTip(layer);

    m_gpLayerList.push_back( (slayer = new gpLayer_s( newMp, layer ) ) );
    AddPopupMenus(slayer);

    newMp->SetMargins(0, 0, 50, 100);
    newMp->EnableDoubleBuffer(true);
    newMp->SetMPScrollbars( layer->IsWindowScrollbarsEnabled() );
    newMp->zoomIncrementalFactor= layer->GetZoomIncrementFactory();

    //newMp->SetGradienBackColour(false);
    //newMp->SetColourTheme( *wxWHITE, *wxBLACK, *wxBLACK );
    //newMp->SetColourTheme( *wxBLACK, *wxWHITE, *wxWHITE );


    //newMp->SetMouseZoomMode(mpZOOM_X);
    bool xmin,xmax,ymin,ymax;
    layer->GetBoundLocks(xmin,xmax,ymin,ymax);
    newMp->SetBoundLockMinY(ymin);
    newMp->SetBoundLockMaxY(ymax);
    newMp->SetBoundLockMinX(xmin);
    newMp->SetBoundLockMaxX(xmax);

    mpLayerList_t *list = layer->GetLayerList();
    if(list) // should be defined
    {
        mpLayerList_t::iterator it;
        for(it = list->begin(); it != list->end(); it++)
            newMp->AddLayer( *it, false );
    }
    return true;
}


void gpPanel::AddPopupMenus(gpLayer_s *slayer)
{
    mpWindow* window = slayer->m_plot;
    gpLayer* layer = slayer->layer;

    int idd;
    wxMenu *popup, *submenu;

    gpPopupGroup group;
    gpPopup gppop;

    Connect(wxID_ANY, wxEVT_MENU_HIGHLIGHT, (wxObjectEventFunction)&gpPanel::OnMenuHighlight);


    if( layer->GetPopupGroup( gpYAXIS, group) )
    {

        popup = layer->GetYAxis()->GetPopupMenu();
        popup->Append( wxNewId(), group.GetLabel(), group, group.GetHelp() );

        group.rewind();
        while(group.nextItem(gppop))
        {
            idd = ((wxMenuItem*)gppop)->GetId();
            int type = gppop.GetType();
            switch(type)
            {
                case(gpAXIS_DEFAULT):  Connect( idd, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&gpPanel::OnMenuItemChangeYAxisDefault); break;
                case(gpAXIS_DISTANCE):   Connect( idd, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&gpPanel::OnMenuItemChangeYAxisLength); break;
                case(gpAXIS_N):        Connect( idd, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&gpPanel::OnMenuItemChangeYAxisN); break;
                case(gpAXIS_PROCENT):  Connect( idd, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&gpPanel::OnMenuItemChangeYAxisProcent); break;
                case(gpAXIS_TIME):     Connect( idd, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&gpPanel::OnMenuItemChangeYAxisTime); break;
                case(gpAXIS_CUSTOM):   Connect( idd, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&gpPanel::OnMenuItemChangeYAxisCustom); break;
                default: wxLogMessage(_("There is no popup handler!")); break;
            }
            Connect(idd, wxEVT_MENU_HIGHLIGHT, (wxObjectEventFunction)&gpPanel::OnMenuHighlight);
        }
    }
    if( layer->GetPopupGroup( gpXAXIS, group) )
    {

        popup = layer->GetXAxis()->GetPopupMenu();
        popup->Append(  wxNewId(), group.GetLabel(), group, group.GetHelp() );
        group.rewind();
        gpPopup gppop;
        while(group.nextItem(gppop))
        {
            idd = ((wxMenuItem*)gppop)->GetId();
            int type = gppop.GetType();
            switch(type)
            {
                case(gpAXIS_DEFAULT):  Connect( idd, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&gpPanel::OnMenuItemChangeXAxisDefault); break;
                case(gpAXIS_DISTANCE):   Connect( idd, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&gpPanel::OnMenuItemChangeXAxisLength); break;
                case(gpAXIS_N):        Connect( idd, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&gpPanel::OnMenuItemChangeXAxisN); break;
                case(gpAXIS_PROCENT):  Connect( idd, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&gpPanel::OnMenuItemChangeXAxisProcent); break;
                case(gpAXIS_TIME):     Connect( idd, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&gpPanel::OnMenuItemChangeXAxisTime); break;
                case(gpAXIS_CUSTOM):   Connect( idd, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&gpPanel::OnMenuItemChangeXAxisCustom); break;
                default:    wxLogMessage(_("There is no popup handler!")); break;
            }
        }
    }

    popup = window->GetPopupMenu();
    if( layer->GetPopupGroup( gpWINDOW, group) )
    {
        popup->Insert( 0, wxNewId(), group.GetLabel(), group, group.GetHelp() );
        group.rewind();
        gpPopup gppop;
        while(group.nextItem(gppop))
        {
            idd = ((wxMenuItem*)gppop)->GetId();
            int type = gppop.GetType();
            switch(type)
            {
                case(gpCHART_DEFAULT):      Connect( idd, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&gpPanel::OnMenuItemChangeAxisDefault); break;
                case(gpCHART_ACCUMULATION): Connect( idd, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&gpPanel::OnMenuItemChangeAxisAccumulation); break;
                case(gpCHART_CUSTOM):       Connect( idd, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&gpPanel::OnMenuItemChangeAxisCustom); break;
                case(gpCHART_DNL):          Connect( idd, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&gpPanel::OnMenuItemChangeAxisDNL); break;
                case(gpCHART_INL):          Connect( idd, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&gpPanel::OnMenuItemChangeAxisINL); break;
                case(gpCHART_FFT):          Connect( idd, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&gpPanel::OnMenuItemChangeAxisFFT); break;
                default:  wxLogMessage(_("There is no popup handler!")); break;
            }
        }
    }

    // Default Window popup menu

    // File
    submenu = new wxMenu();
        submenu->Append( idd=wxNewId(), _("Print preview"), _("chart Print preview"));
        Connect(idd, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&gpPanel::OnMenuItemPrintPreview);
        layer->AddId(idd);

        submenu->Append( idd=wxNewId(), _("Screenshot\tCtrl+W"), _("Export screenshot from chart"));
        Connect(idd, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&gpPanel::OnMenuItemSaveScreenshot);
        layer->AddId(idd);

        submenu->Append( idd=wxNewId(), _("Export CVS\tCtrl+E"), _("Export chart to CVS ascii table"));
        Connect(idd, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&gpPanel::OnMenuItemExportCVS);
        layer->AddId(idd);

        submenu->Append( idd=wxNewId(), _("Export Clipboard"), _("Export chart to clipboard"));
        Connect(idd, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&gpPanel::OnMenuItemExportClipboard);
        layer->AddId(idd);


    popup->Insert(0, wxNewId(),  _("File"), submenu, _("Print, screenshots and exports") );

    // Edit
    wxMenuItem* item;
    submenu = new wxMenu();
        submenu->AppendCheckItem( idd=wxNewId(),  _("Lock"),  _("Lock chart") );
        Connect( idd,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&gpPanel::OnMenuItemGraphLockEvent);
        layer->AddId(idd);

        submenu->AppendCheckItem( idd=wxNewId(), wxString::Format(_("Show Grids\tCtrl+G")), _("Enable / disable grids") );
        Connect(idd,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&gpPanel::OnMenuItemGridEvent);
        layer->AddId(idd);


        if( layer->IsThereMarkCorner() ){
            item = submenu->AppendCheckItem( idd=wxNewId(), wxString::Format(_("Mark Corners")), _("Show corner marks") );
            Connect(idd,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&gpPanel::OnMenuItemMarkEvent);
            layer->AddId(idd);
            item->Check( layer->GetDefaultMarkCorners() );
        }
        if( layer->IsThereInfoLayer() ){
            item = submenu->AppendCheckItem( idd=wxNewId(), wxString::Format(_("Show Info Layer")), _("Show Info Layer") );
            Connect(idd,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&gpPanel::OnMenuItemShowInfo);
            layer->AddId(idd);
            item->Check( layer->GetDefaultShowInfoLayer() );
        }

        if( layer->IsThereContinousLines() )
        {
            item = submenu->AppendCheckItem( idd=wxNewId(), wxString::Format(_("Continous line")), _("Continous line") );
            Connect(idd,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&gpPanel::OnMenuItemContinousLine);
            layer->AddId(idd);
            item->Check( layer->GetDefaultContinousLines() );
        }


        /* This doesnt need to be here, because mpWindow has Fit already.
        submenu->AppendCheckItem( idd=wxNewId(), wxString::Format(_("Fit")), _("Fit chart") );
        Connect(idd,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&gpPanel::OnMenuItemFit);
        layer->AddId(idd);*/

        //move Center to Edit submenu
        item = popup->FindItemByPosition(2);
        submenu->Append( popup->Remove(item) );

        //move Zoom in to Edit submenu
        item = popup->FindItemByPosition(3);
        submenu->Append( popup->Remove(item) );

        //move Zoom out to Edit submenu
        item = popup->FindItemByPosition(3);
        submenu->Append( popup->Remove(item) );

        //move Lock acpect to Edit submenu
        item = popup->FindItemByPosition(3);
        submenu->Append( popup->Remove(item) );

    popup->Insert(2, wxNewId(),  _("Edit"), submenu, _("Edit") );

    //Help
    submenu = new wxMenu();
        //Show mouse commands
        item = popup->FindItemByPosition(4);
        submenu->Append( popup->Remove(item) );
    popup->Insert(3, wxNewId(),  _("Help"), submenu, _("Help") );


}

gpLayer_s*	gpPanel::GetLayerByPosition(int position)
{
    if(position >= 0 && (int)m_gpLayerList.size() > position )
    {
        return m_gpLayerList[position];
    }
    return 0;
}
gpLayer_s*	gpPanel::GetLayerByName(const wxString name)
{
    gpLayerList_t::iterator it;
    for( it = m_gpLayerList.begin(); it!= m_gpLayerList.end() ; it++)
    {
        if( (*it)->layer->GetName() == name)return (*it);
    }
    return 0;
}
gpLayer_s*  gpPanel::GetLayerByMenuId(int id)
{
    gpLayerList_t::iterator it;
    for( it = m_gpLayerList.begin(); it!= m_gpLayerList.end() ; it++)
    {
        if( (*it)->layer->IdBelong(id))return (*it);
        if( (*it)->layer->MenuIdBelongs(id))return (*it);
    }
    return 0;

}
gpLayer_s*  gpPanel::GetLayer(gpLayer* layer)
{
    gpLayerList_t::iterator it;
    if(layer==NULL)return 0;
    for( it = m_gpLayerList.begin(); it!= m_gpLayerList.end() ; it++)
    {
        if( (*it)->layer == layer ) return *it;
    }
    return 0;
}
mpWindow*  gpPanel::GetWindowByLayer(gpLayer* layer)
{
    gpLayer_s *li = GetLayer(layer);
    if(li) return li->m_plot;
    return 0;
}
bool gpPanel::SetLayerVisible(gpLayer* layer, bool viewable)
{
    bool ok=false;
    if(layer==0)return false;

    gpLayer_s* l = GetLayer(layer);

    if( l )
    {
        layer->SetVisibility(viewable);
        storePanelSize();



        //if not Realized there is no sizer yet!
        wxSizer* sizer = l->m_plot->GetContainingSizer();
        if(sizer)
            sizer->Show( viewable );


        //m_BoxSizer->Fit(this);
        //m_BoxSizer->SetSizeHints(this);

        //I think, this will be called from BoxSizer internal functions..it's true?
        //l->m_plot->Enable(viewable);

        if(viewable){
            //only need update when viewable is enabled
            l->m_plot->UpdateAll();
        }
        restorePanelSize();

    }
    return ok;
}
bool gpPanel::SetLayerVisible(gpLayer_s* layer, bool viewable)
{
    if(!layer)return false;
    return SetLayerVisible(layer->layer, viewable);
}
bool gpPanel::IsLayerVisible(gpLayer* layer)
{
    gpLayer_s* l = GetLayer(layer);
    if(l)
    {
        return l->m_plot->GetContainingSizer()->IsShown(l->m_plot);
        //return m_BoxSizer->IsShown(l->m_plot);
    }
    return false;
}
bool gpPanel::IsLayerVisible(int position)
{
    gpLayer_s* l = GetLayerByPosition(position);
    if(l) return IsLayerVisible(l->layer);
    return false;
}
bool gpPanel::SetLayerVisible(const wxString &name, bool viewable)
{
    gpLayer_s *l = GetLayerByName(name);
    return SetLayerVisible(l, viewable);

}
bool gpPanel::SetLayerVisible(int id, bool viewable)
{
    gpLayer_s* layer = GetLayerByMenuId(id);
    return SetLayerVisible(layer, viewable);
}

void gpPanel::FitAll()
{
    gpLayerList_t::iterator it;
    for(it = m_gpLayerList.begin(); it != m_gpLayerList.end(); it++)
    {
        (*it)->m_plot->Fit();
    }
}
void gpPanel::Fit(gpLayer* layer)
{
    gpLayer_s* l = GetLayer(layer);
    if(l)l->m_plot->Fit();

}
void gpPanel::Fit(gpLayer* layer, double xMin, double xMax, double yMin, double yMax, wxCoord *printSizeX,wxCoord *printSizeY)
{
    gpLayer_s* l = GetLayer(layer);
    if(l)l->m_plot->Fit(xMin, xMax, yMin, yMax, printSizeX, printSizeY);
}
void gpPanel::LockAll(bool t)
{
    gpLayerList_t::iterator it;
    for(it = m_gpLayerList.begin(); it != m_gpLayerList.end(); it++)
    {
        (*it)->m_plot->EnableMousePanZoom(!t);
    }
}
void gpPanel::LockXAxis(gpLayer *layer, bool t)
{

}
void gpPanel::LockYAxis(gpLayer *layer, bool t)
{
    gpLayer_s* l = GetLayer(layer);
    if(l){
        if(t)l->m_plot->SetMouseZoomMode(mpZOOM_X);
        else l->m_plot->SetMouseZoomMode(mpZOOM_XY);
    }
}
bool gpPanel::LayerExist(gpLayer_s* l)
{
    gpLayerList_t::iterator it;
    for(it = m_gpLayerList.begin(); it != m_gpLayerList.end(); it++)
    {
        if( *it == l)return true;
    }
    return false;
}


/*
void gpWindow::LockAspect(gpLayer *layer, bool lock)
{
    //layer->LockAspect(event.GetSelection());
}

void gpWindow::LockScreen(gpLayer *layer, bool lock)
{
    //layer->EnableMousePanZoom(!t);
}
*/
void gpPanel::OnMenuHighlight(wxCommandEvent& event)
{
    wxString str = event.GetString();
    str = _("");
}
void gpPanel::OnMenuItemGraphEnableEvent(wxCommandEvent& event)
{
    bool visible = event.GetSelection();
    int id = event.GetId();
    SetLayerVisible( id, visible); //set layer visibility

    //If used both, menuitem and toolbar buttons to change visibility:
    //if pressed menuitem, must change also toolbar button!
    gpLayer_s* slayer = GetLayerByMenuId( id );
    if(slayer==0)return;
    slayer->layer->CheckMenuGroup(gpMENU_GROUP_VISIBLE, visible);
}
void gpPanel::OnMenuItemGraphLockAspectEvent(wxCommandEvent& event)
{
    gpLayer_s *gp = GetLayerByMenuId(event.GetId() );
    if(gp==0)return;
    gp->m_plot->LockAspect(event.GetSelection());
    gp->m_plot->UpdateAll();

}
void gpPanel::OnMenuItemGraphLockEvent(wxCommandEvent& event)
{
    bool enable = event.GetSelection();
    gpLayer_s *gp = GetLayerByMenuId(event.GetId() );
    if(gp==0)return;

    if(enable)  gp->m_plot->SetMouseZoomMode(mpZOOM_X);
    else        gp->m_plot->SetMouseZoomMode(mpZOOM_XY);

    //gp->m_plot->EnableMousePanZoom( ! enable );
    //gp->m_plot->EnableMousePopup( ! enable );
    //gp->m_plot->UpdateAll();
}
void gpPanel::OnMenuItemGridEvent(wxCommandEvent& event)
{
    gpLayer_s *gp = GetLayerByMenuId(event.GetId() );
    if(gp==0)return;
    gp->layer->ShowGrid(!event.GetSelection());
    gp->m_plot->UpdateAll();
}
void gpPanel::OnMenuItemTooltip(wxCommandEvent& event)
{
    gpLayer_s *gp = GetLayerByMenuId(event.GetId() );
    if(gp==0)return;
    gp->layer->wxToolTip::Enable( event.GetSelection() );
}
void gpPanel::OnMenuItemMarkEvent(wxCommandEvent& event)
{
    gpLayer_s *gp = GetLayerByMenuId(event.GetId() );
    if(gp==0)return;
    gp->layer->ShowCornerMarkers(event.GetSelection());
    gp->m_plot->UpdateAll();
}
void gpPanel::OnMenuItemShowInfo(wxCommandEvent& event)
{
    gpLayer_s *gp = GetLayerByMenuId(event.GetId() );
    if(gp==0)return;
    gp->layer->ShowInfoLayer(event.GetSelection());
    gp->m_plot->UpdateAll();
}
void gpPanel::OnMenuItemContinousLine(wxCommandEvent& event)
{
    gpLayer_s *gp = GetLayerByMenuId( event.GetId() );
    if(gp==0)return;
    gp->layer->SetContinousLine(event.GetSelection());
    gp->m_plot->UpdateAll();
}
void gpPanel::OnMenuItemFit(wxCommandEvent& event)
{
    gpLayer_s *gp = GetLayerByMenuId(event.GetId() );
    if(gp==0)return;
    gp->m_plot->Fit();
    gp->m_plot->UpdateAll();
}
void gpPanel::OnMenuItemPrint(wxCommandEvent& event)
{
    gpLayer_s *gp = GetLayerByMenuId(event.GetId() );
    if(gp==NULL)return;

    wxPrinter printer;
    mpPrintout printout(gp->m_plot, wxT("Plot print"));
    printer.Print(this, &printout, true);

}
void gpPanel::OnMenuItemPrintPreview(wxCommandEvent& event)
{
    gpLayer_s *gp = GetLayerByMenuId(event.GetId() );
    if(gp==NULL)return;

    mpPrintout *plotPrint = new mpPrintout(gp->m_plot, gp->layer->GetName());
    mpPrintout *plotPrintPreview = new mpPrintout(gp->m_plot, gp->layer->GetName());

    wxPrintPreview *preview = new wxPrintPreview(plotPrintPreview, plotPrint);

    wxPreviewFrame *frame = new wxPreviewFrame(preview, this, wxT("Print Plot"), wxPoint(100, 100), wxSize(600, 650));

    frame->Centre(wxBOTH);
    frame->Initialize();
    frame->Show(true);
}
void gpPanel::OnMenuItemSaveScreenshot(wxCommandEvent& event)
{
    gpLayer_s *gp = GetLayerByMenuId(event.GetId() );
    if(gp==NULL)return;

    wxFileDialog fileDialog(this, _("Save a screenshot"), wxT(""), wxT(""),
                                 wxT("PNG image (*.png)|*.png|JPEG image (*.jpg)|*.jpeg;*.jpg|BMP image (*.bmp)|*.bmp"),
                                wxFD_SAVE);

    if(fileDialog.ShowModal() == wxID_OK)
    {
        wxFileName namePath(fileDialog.GetPath());
        int fileType = wxBITMAP_TYPE_BMP;
        if( namePath.GetExt().CmpNoCase(wxT("jpeg")) == 0 ) fileType = wxBITMAP_TYPE_JPEG;
        else if( namePath.GetExt().CmpNoCase(wxT("jpg")) == 0 )  fileType = wxBITMAP_TYPE_JPEG;
        else if( namePath.GetExt().CmpNoCase(wxT("png")) == 0 )  fileType = wxBITMAP_TYPE_PNG;

        wxArrayString aChoices;
        long width = gp->m_plot->GetXScreen(),
             height = gp->m_plot->GetYScreen();
        aChoices.Add(wxString::Format(_("%ix%i"), width, height)); //current window size
        aChoices.Add(wxString::Format(_("%ix%i"), 640, (int)(640.0/(double)width*(double)height)) ); //current window size  ( 640 x ??)
        aChoices.Add(wxString::Format(_("%ix%i"), 1024, (int)(1024.0/(double)width*(double)height)) ); //current window size ( 1024 x ??)
        aChoices.Add(wxString::Format(_("%ix%i"), 1920, (int)(1920.0/(double)width*(double)height)) ); //current window size ( 1920 x ??)
        aChoices.Add(_("640x480"));
        aChoices.Add(_("1024x768"));
        aChoices.Add(_("1280x720"));
        aChoices.Add(_("1920x1080"));
        aChoices.Add(_("custom"));
        int  index = wxGetSingleChoiceIndex(_("Select image resolution"), _("caption"), aChoices, this);

        if(index!=-1)
        {
            wxString reso = aChoices.Item(index);
            if( reso.CompareTo(_("custom")) == 0 ){
                reso = wxGetTextFromUser(_("Write image resolution (format: [width]x[height]"), _("Custom resolution for image"), _("640x480"), this);
            }
            if(reso.Find('x') != -1)
            {
                reso.Mid(0, reso.Find('x')).ToLong(&width);
                reso.Mid(reso.Find('x')+1).ToLong(&height);

                wxSize imgSize((int)width,(int)height);
                gp->m_plot->SaveScreenshot(fileDialog.GetPath(), fileType, imgSize, false);
            }
        }
    }
    event.Skip();
}
void gpPanel::OnMenuItemExportCVS(wxCommandEvent& event)
{
    gpLayer_s *gp = GetLayerByMenuId(event.GetId() );
    if(gp==NULL)return;
    OnExportCVS(gp);
}
void gpPanel::OnMenuItemExportClipboard(wxCommandEvent& event)
{
    gpLayer_s *gp = GetLayerByMenuId(event.GetId() );
    if(gp==NULL)return;
    OnExportClipboard(gp);
}
void gpPanel::OnExportCVS(gpLayer_s *glayer)
{
    wxString content;
    gpLayerList_t::iterator it;
    double x,y;
    wxFile output;
    wxString formatStr = _("%.0f\t%.0f");

    wxFileDialog fileDialog(this, _("Save CVS table"), wxT(""), wxT(""), wxT("CVS table (*.cvs) | *.cvs|txt table (*.txt) | *.txt"), wxFD_SAVE);
    if(fileDialog.ShowModal() != wxID_OK) return;
    wxFileName namePath(fileDialog.GetPath());


    if( !QuestString(_("Set table format"), _("Table format"), formatStr) ) return;
    formatStr += _("\n");

    if(glayer == 0)  //export all layers to cvs table
    {
        for(it = m_gpLayerList.begin(); it != m_gpLayerList.end(); it++)
        {
            int type = (*it)->layer->gpType();
            if(type==0)continue;
            content.Append(  _("\t\t") );
            content.Append( (*it)->layer->GetName() );
            content.Append( _("\n") );
            if(type == gpLAYER_HISTOGRAM)
            {
                wxString label;
                barChartLayer *layer = dynamic_cast<barChartLayer*>( (*it)->layer->GetLayer() );
                layer->Rewind();
                while(layer->GetNextXY(x,y, label))
                {
                    content.Append( wxString::Format( formatStr, x,y) );
                }
            }
            else if(type == gpLAYER_TDCRESULT)
            {
                lineChartLayer *layer = dynamic_cast<lineChartLayer*>( (*it)->layer->GetLayer() );
                layer->Rewind();
                while(layer->GetNextXY(x,y))
                {
                    content.Append( wxString::Format( formatStr, x,y ) );
                }
            }

        }
    }
    else
    {
        //export one layer to csv table
        int type = glayer->layer->gpType();
        if(type==0)return;
        content.Append(  _("\t\t") );
        content.Append( glayer->layer->GetName() );
        content.Append( _("\n") );
        if(type == gpLAYER_HISTOGRAM)
        {
            wxString label;
            barChartLayer *layer = dynamic_cast<barChartLayer*>( glayer->layer->GetLayer() );
            layer->Rewind();
            while(layer->GetNextXY(x,y, label))
            {
                content.Append( wxString::Format( formatStr, x,y) );
            }
        }
        else if(type == gpLAYER_TDCRESULT || type == gpLAYER_LINE )
        {
            lineChartLayer *layer = dynamic_cast<lineChartLayer*>( glayer->layer->GetLayer() );
            layer->Rewind();
            while(layer->GetNextXY(x,y))
            {
                content.Append( wxString::Format( formatStr, x,y) );
            }
        }
    }
    content.Replace(_("."), _(","));
    output.Open( namePath.GetFullPath(), wxFile::write);
    output.Write( content );

}
void gpPanel::OnExportClipboard(gpLayer_s *glayer)
{
    wxString content;
    gpLayerList_t::iterator it;
    double x,y;
    wxFile output;
    wxString formatStr = _("%.3f\t%.3f\n");
    if(glayer == 0)  //export all layers to cvs table
    {
        for(it = m_gpLayerList.begin(); it != m_gpLayerList.end(); it++)
        {
            int type = (*it)->layer->gpType();
            if(type==0)continue;
            content.Append(  _("\t\t") );
            content.Append( (*it)->layer->GetName() );
            content.Append( _("\n") );
            if(type == gpLAYER_HISTOGRAM)
            {
                wxString label;
                barChartLayer *layer = dynamic_cast<barChartLayer*>( (*it)->layer->GetLayer() );
                layer->Rewind();
                while(layer->GetNextXY(x,y, label))
                {
                    content.Append( wxString::Format( formatStr, x,y) );
                }
            }

        }
    }
    else
    {
        //export one layer to csv table
        int type = glayer->layer->gpType();
        if(type==0)return;
        content.Append(  _("\t\t") );
        content.Append( glayer->layer->GetName() );
        content.Append( _("\n") );
        if(type == gpLAYER_HISTOGRAM)
        {
            wxString label;
            barChartLayer *layer = dynamic_cast<barChartLayer*>( glayer->layer->GetLayer() );
            layer->Rewind();
            while(layer->GetNextXY(x,y, label))
            {
                content.Append( wxString::Format( formatStr, x,y) );
            }
        }
        else if(type == gpLAYER_LINE )
        {
            lineChartLayer *layer = dynamic_cast<lineChartLayer*>( glayer->layer->GetLayer() );
            layer->Rewind();
            while(layer->GetNextXY(x,y))
            {
                content.Append( wxString::Format( formatStr, x,y) );
            }
        }
    }
    // Write some text to the clipboard
    if (m_clipboard.Open())
    {

        content.Replace(_("."), _(","));

        // This data objects are held by the clipboard,
        // so do not delete them in the app.
        m_clipboard.SetData( new wxTextDataObject(content) );
        m_clipboard.Close();
    }

}

void gpPanel::OnMenuItemChangeAxis(wxCommandEvent &event)
{
    //get gpLayer_s pointer by menu id (also detect custom id (gpMenu::AddId)
    wxString formula;
    gpLayer_s *gp = GetLayerByMenuId( event.GetId() );
    if(gp==NULL)return;

    if( (gpCHART_KIND)event.GetInt() == gpCHART_CUSTOM )
    {
        if(!gp->layer->IsXFormula()){
            formula = gp->layer->GetYXFormula();
            if( QuestString(_("Insert custom formula Y(X)=. Example: sin(2PI*X/30)"), _("Custom formula"), formula) )
                gp->layer->SetYXFormula(formula);
        }
    }
    //convert chart to different kind
    gp->layer->SetChartKind( (gpCHART_KIND)event.GetInt() );
    gp->m_plot->Fit();
    gp->m_plot->UpdateAll();
}
void gpPanel::OnMenuItemChangeAxisY(wxCommandEvent &event)
{
    //get gpLayer_s pointer by menu id (also detect custom id (gpMenu::AddId)
    gpLayer_s *gp = GetLayerByMenuId( event.GetId() );
    if(gp==NULL)return;

    if((gpAXIS_SCALE)event.GetInt() == gpAXIS_CUSTOM )
    {
        wxString formula = gp->layer->GetYFormula();
        if( QuestString(_("Insert custom formula for Y axis"), _("Custom formula"), formula) )
            gp->layer->SetYFormula(formula);
    }

    //convert chart to different kind
    gp->layer->SelectYAxisScale( (gpAXIS_SCALE)event.GetInt() );
    gp->m_plot->Fit();
    gp->m_plot->UpdateAll();
}
void gpPanel::OnMenuItemChangeAxisX(wxCommandEvent &event)
{
    //get gpLayer_s pointer by menu id (also detect custom id (gpMenu::AddId)
    gpLayer_s *gp = GetLayerByMenuId( event.GetId() );
    if(gp==NULL)return;

    if((gpAXIS_SCALE)event.GetInt() == gpAXIS_CUSTOM )
    {
        wxString formula = gp->layer->GetXFormula();
        if( QuestString(_("Insert custom formula for X axis"), _("Custom formula"), formula) )
            gp->layer->SetXFormula(formula);
    }

    //convert chart to different kind
    gp->layer->SelectXAxisScale( (gpAXIS_SCALE)event.GetInt() );
    gp->m_plot->Fit();
    gp->m_plot->UpdateAll();
}
bool gpPanel::QuestString(wxString label, wxString title, wxString& out)
{
    wxTextEntryDialog formatDialog(this, label, title, out, wxOK | wxCANCEL);
    if( formatDialog.ShowModal() != wxID_OK ) return false;
    out = formatDialog.GetValue();
    return out.IsEmpty() ? false : true;
}

/* VARIOUS POPUP EVENTS HANDLER */
void gpPanel::OnMenuItemLockAllSelected(wxCommandEvent& event)
{
    //LockAll( event.GetSelection() );
}


// chart Kind change events
void gpPanel::OnMenuItemChangeAxisDefault(wxCommandEvent& event)
{ event.SetInt( gpCHART_DEFAULT); OnMenuItemChangeAxis(event); }

void gpPanel::OnMenuItemChangeAxisINL(wxCommandEvent& event)
{ event.SetInt( gpCHART_INL); OnMenuItemChangeAxis(event); }

void gpPanel::OnMenuItemChangeAxisDNL(wxCommandEvent& event)
{ event.SetInt( gpCHART_DNL); OnMenuItemChangeAxis(event); }

void gpPanel::OnMenuItemChangeAxisFFT(wxCommandEvent& event)
{ event.SetInt( gpCHART_FFT); OnMenuItemChangeAxis(event); }

void gpPanel::OnMenuItemChangeAxisAccumulation(wxCommandEvent& event)
{ event.SetInt( gpCHART_ACCUMULATION); OnMenuItemChangeAxis(event); }

void gpPanel::OnMenuItemChangeAxisCustom(wxCommandEvent& event)
{ event.SetInt( gpCHART_CUSTOM); OnMenuItemChangeAxis(event); }

//Y AXIS CHANGE EVENTS
void gpPanel::OnMenuItemChangeYAxisDefault(wxCommandEvent& event)
{ event.SetInt( gpAXIS_DEFAULT ); OnMenuItemChangeAxisY(event); }

void gpPanel::OnMenuItemChangeYAxisProcent(wxCommandEvent& event)
{ event.SetInt( gpAXIS_PROCENT ); OnMenuItemChangeAxisY(event); }

void gpPanel::OnMenuItemChangeYAxisLength(wxCommandEvent& event)
{ event.SetInt( gpAXIS_DISTANCE ); OnMenuItemChangeAxisY(event); }

void gpPanel::OnMenuItemChangeYAxisTime(wxCommandEvent& event)
{ event.SetInt( gpAXIS_TIME ); OnMenuItemChangeAxisY(event); }

void gpPanel::OnMenuItemChangeYAxisN(wxCommandEvent& event)
{ event.SetInt( gpAXIS_N ); OnMenuItemChangeAxisY(event); }

void gpPanel::OnMenuItemChangeYAxisCustom(wxCommandEvent& event)
{ event.SetInt( gpAXIS_CUSTOM ); OnMenuItemChangeAxisY(event); }

//X AXIS CHANGE EVENTS
void gpPanel::OnMenuItemChangeXAxisDefault(wxCommandEvent& event)
{ event.SetInt( gpAXIS_DEFAULT ); OnMenuItemChangeAxisX(event); }

void gpPanel::OnMenuItemChangeXAxisProcent(wxCommandEvent& event)
{ event.SetInt( gpAXIS_PROCENT ); OnMenuItemChangeAxisX(event); }

void gpPanel::OnMenuItemChangeXAxisLength(wxCommandEvent& event)
{ event.SetInt( gpAXIS_DISTANCE ); OnMenuItemChangeAxisX(event); }

void gpPanel::OnMenuItemChangeXAxisTime(wxCommandEvent& event)
{ event.SetInt( gpAXIS_TIME ); OnMenuItemChangeAxisX(event); }

void gpPanel::OnMenuItemChangeXAxisN(wxCommandEvent& event)
{ event.SetInt( gpAXIS_N ); OnMenuItemChangeAxisX(event); }

void gpPanel::OnMenuItemChangeXAxisCustom(wxCommandEvent& event)
{ event.SetInt( gpAXIS_CUSTOM ); OnMenuItemChangeAxisX(event); }

//http://www.parashift.com/c++-faq-lite/
