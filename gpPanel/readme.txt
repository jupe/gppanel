  __  ___ ___  _   _  _  ___  _   
 / _|| o \ o \/ \ | \| || __|| |  
( |_n|  _/  _/ o || \\ || _| | |_ 
 \__/|_| |_| |_n_||_|\_||___||___|
 Author: Jussi Vatjus-Anttila
 (@)  jussiva@gmail.com
 (C)  Jussi Vatjus-Anttila 
      2009

 License: wxWindows Library License v3.1
 
gpPanel is wxWidget library, which purpose is to handle several charts, axis 
and chart mathematical operations. It's developed for my diplom thesis, which 
subject was of an ASIC circuit testing. The library can handle large volumes of
data. Up to several millions of points and the mathematical / statistical data.
Anyway, gpPanel main purpose is to easier use of wxMathPlot, when a needed several 
graphics and several mathematical operations for data.
	 
Library contains two parts, gpPanel and gpLayers. gpPanel derived from mpWindow -class, 
so it's the	basic class where all gpLayers are. gpPanel contains handle gpLayer visibility 
and other basic stuff. gpLayer contains all needed information how graphic is drawed and how 
layer data is handled (x/y-axis + data mathematical functions). One gpLayer contains normally 
at least one mpLayer, but it can handle also several of mpLayers. 


wxMathPlot is modification from original wxMathPlot library:
 new mpLayer:s
 -mpPoint       layer
 -mpXYBar       layer
 -mpFX2Y        layer
 -mpCandleStick layer
 
Usage:
------------------------------
Compile and link them:
-gpPanel.cpp
-baseLayers/gpLayer.cpp
 (all others are in headers)



Example:
------------------------------
// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

//gpPanel widget
#include <gpPanel.h>
#include <gpLineLayer.h>

class MyFrame: public wxFrame
{
public:
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

private:
    void OnHello(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    wxDECLARE_EVENT_TABLE();
};

class MyApp: public wxApp
{
public:
    virtual bool OnInit();
private:
    void gpTest(wxWindow* parentWindow);
};

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(wxID_EXIT,  MyFrame::OnExit)
    EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
wxEND_EVENT_TABLE()
wxIMPLEMENT_APP(MyApp);

void MyApp::gpTest(wxWindow* parentWindow)
{
    //init gpPanel
    gpPanel* graphPanel = new gpPanel(parentWindow,wxNewId(),wxDefaultPosition,wxSize(240,336));

    //create new line layer
    gpLineLayer* lineLayer = new gpLineLayer(_("Line"), _("x-label"), _("y-label"));

    // Create a data series
    gpSeries* series1 = lineLayer->AddSeriesLayer("Random");

    //Push data to gpLayer
    for(int i=0;i<12;i++)
        series1->DataPush( i, (rand()+1)%1024);

    //add gpLayer to gpPanel
    graphPanel->AddLayer( lineLayer, 0 );
    lineLayer->RefreshChart();
    graphPanel->Fit(lineLayer);
}

bool MyApp::OnInit()
{
    MyFrame* frame = new MyFrame( "wxWidgets Demo", wxPoint(50, 50), wxSize(850, 640) );
    gpTest(frame);
    frame->Show( true );
    return true;
}

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(NULL, wxID_ANY, title, pos, size)
{
    wxMenu* menuFile = new wxMenu;
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);
    wxMenu* menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);
    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append( menuFile, "&File" );
    menuBar->Append( menuHelp, "&Help" );
    SetMenuBar( menuBar );
    CreateStatusBar();
    SetStatusText( "gpPanel sample app" );
}

void MyFrame::OnExit(wxCommandEvent& event)
{
    Close( true );
}

void MyFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox( "This is a wxWidgets' gpPanel sample",
        "About gpPanel Sample", wxOK | wxICON_INFORMATION );
}
