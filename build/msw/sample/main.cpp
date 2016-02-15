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
    // A panel is a container for one or more charts
    gpPanel* graphPanel = new gpPanel( parentWindow, wxNewId(), wxDefaultPosition,wxSize(240,336) );

    //create new line layer for each chart with its own axes and titles
    gpLineLayer* lli = new gpLineLayer(_("No Line :("), _("sample number"), _("I component"));
    gpLineLayer* llq = new gpLineLayer(_("No Line :("), _("sample number"), _("Q component"));
    gpLineLayer* lliq = new gpLineLayer(_("No Line:("), _("sample number"), _("IQ Plot"));

    // Multiple data series can be plotted on a single graph
    gpSeries* s1 = lliq->AddSeriesLayer("I");
    gpSeries* s2 = lliq->AddSeriesLayer("Q");
    // Or on separate graphs
    gpSeries* si = lli->AddSeriesLayer("I");
    gpSeries* sq = llq->AddSeriesLayer("Q");

    // Series colors can be selected via SetPen()
    si->SetPen((wxPen&) *wxRED_PEN);
    sq->SetPen((wxPen&) *wxBLUE_PEN);
    s1->SetPen((wxPen&) *wxRED_PEN);
    s2->SetPen((wxPen&) *wxBLUE_PEN);

    //Push data to each series
    for(int i = 0; i < 250; i++)
    {
        // Generate a random angle between 0 and 2PI
        double theta =((double)rand()/(double)RAND_MAX) * 2 * M_PI;
        // Get vector components of the angle
        double ph_i = cos(theta);
        double ph_q = sin(theta);
        
        // Add the components to the data series for the separate plots
        si->DataPush( i, ph_i );
        sq->DataPush( i, ph_q );
        // Add the componenets to the data series for the combined plots
        s1->DataPush( i, ph_i );
        s2->DataPush( i, ph_q );
    }

    // Each layer must be added to the panel.
    // Popup menu options can be specified in the second mask parameter, or 0 for none.
    graphPanel->AddLayer( lli, POPUP_FILE | POPUP_CHART | POPUP_EDIT | POPUP_HELP | POPUP_FIT );
    graphPanel->AddLayer( llq, POPUP_FILE | POPUP_CHART | POPUP_EDIT | POPUP_HELP | POPUP_FIT );
    graphPanel->AddLayer( lliq, POPUP_FILE | POPUP_CHART | POPUP_EDIT | POPUP_HELP | POPUP_FIT );

    // Refresh the layers with the series data
    lli->RefreshChart();
    llq->RefreshChart();
    lliq->RefreshChart();
    // Fit the charts to the panel size
    graphPanel->Fit(lli);
    graphPanel->Fit(llq);
    graphPanel->Fit(lliq);
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
