#include "gpSizer.h"

/* gpSizerWin implementation */
gpSizerWin::gpSizerWin(){}
gpSizerWin::gpSizerWin(wxWindow* window, int proportion)
{
	m_window = window;
	m_proportion = proportion;
}
gpSizerWin::gpSizerWin(const gpSizerWin& copy)
{
	m_window = copy.GetWindow();
	m_proportion = copy.GetProportion();
}
gpSizerWin::operator wxWindow*(){ return m_window; }

/* gpHorizontalWindows implementation */
gpHorizontalWindows::gpHorizontalWindows(){}
gpHorizontalWindows::gpHorizontalWindows(wxWindow* window)
{
	AddWindow(window);
}
gpHorizontalWindows::gpHorizontalWindows(const gpHorizontalWindows& copy)
{
	m_windowList = copy.GetWindowList();
}
bool gpHorizontalWindows::AddWindow(wxWindow* window, int proportion)
{
	if(Exist(window))return false;
	m_windowList.push_back( gpSizerWin(window, proportion) );
	return true;
}
void gpHorizontalWindows::Clear()
{
	m_windowList.clear();
}
bool gpHorizontalWindows::Exist(wxWindow *window)
{
	Rewind();
	gpSizerWin win;
	while( NextWindow(win) )
	{
		if(win == window)return true;
	}
	return false;
}
std::deque< gpSizerWin > gpHorizontalWindows::GetWindowList() const
{
	return m_windowList;
}
void gpHorizontalWindows::Rewind(){ m_it = m_windowList.begin(); }
bool gpHorizontalWindows::NextWindow(gpSizerWin& window)
{
	if(m_it!=m_windowList.end())
	{
		window = *m_it;
		m_it++;
		return true;
	}
	return false;
}

/* gpSizer implementation */

gpSizer::gpSizer()
{
	m_topSizer = new wxBoxSizer(wxVERTICAL);
	m_border = 2;
}
void gpSizer::SetParent(wxWindow* parent)
{
	m_parent = parent;
}
void gpSizer::AttachGpSizer()
{
	m_parent->SetSizer(m_topSizer);
}
void gpSizer::AddNullWindow()
{
	m_windowList.push_back( gpHorizontalWindows() );
}
int gpSizer::AddWindow(wxWindow* window)
{
	m_windowList.push_back( gpHorizontalWindows(window) );
	return m_windowList.size()-1;
}
bool gpSizer::DeleteWindow(wxWindow* window)
{
    std::deque< gpHorizontalWindows >::iterator it;
    for(it = m_windowList.begin(); it!= m_windowList.end(); it++)
	{
	    if( (*it).Exist(window) )
	    {
	        m_windowList.erase(it);
	        return true;
	    }

	}
    return false;
}
bool gpSizer::AddWindow(unsigned int id, wxWindow* window, int prop)
{
	if( m_windowList.size() < id+1 )
		return false;
	return m_windowList[id].AddWindow(window, prop);
}
wxBoxSizer* gpSizer::GetSizer()
{
	return m_topSizer;
}
void gpSizer::Fit()
{
	m_topSizer->SetSizeHints(m_parent);
}
void gpSizer::Realize()
{
	m_topSizer->Clear();
	std::deque< gpHorizontalWindows >::iterator it;
	gpSizerWin window;
	for(it = m_windowList.begin(); it!= m_windowList.end(); it++)
	{

		wxBoxSizer *newsizer = new wxBoxSizer(wxHORIZONTAL);
		it->Rewind();
		while( it->NextWindow(window) )
		{
			newsizer->Add( window.GetWindow(),
						   window.GetProportion(),
						   wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, m_border);

		}
		m_topSizer->Add(newsizer, 1, wxALL|wxEXPAND|wxALIGN_TOP|wxALIGN_BOTTOM, 0);
	}
	int x,y;
	m_parent->GetPosition(&x, &y);
	m_topSizer->SetDimension( x,y,m_parent->GetClientSize().GetWidth(), m_parent->GetClientSize().GetHeight() );
}
