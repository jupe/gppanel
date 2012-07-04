/**
 *	@file		gpSizer.h
 *	@author     Jussi Vatjus-Anttila
 *	@date		04.07.2012
 *	@version	0.1
 *
 *  Purpose:         gpSizer handlers
 *  Original Author: Jussi Vatjus-Anttila
 *  Created:         04/07/2012
 *  Last edit:       04/07/2012
 *  Copyright:       (c) Jussi Vatjus-Anttila
 *  Licence:         wxWindows Library License v3.1
 *
 *	@note
 *  There is declarations for gpSizer
 *
 */
#ifndef GP_SIZER_H
#define GP_SIZER_H

#include <wx/window.h>
#include <wx/sizer.h>

#include <deque>

class gpSizerWin
{
    private:
        wxWindow    *m_window;
        int         m_proportion;
    public:
        gpSizerWin();
        gpSizerWin(wxWindow* window, int proportion=1);
        gpSizerWin(const gpSizerWin& copy);
        operator wxWindow*();
        wxWindow* GetWindow() const{ return m_window; }
        int GetProportion() const { return m_proportion; }
};

class gpHorizontalWindows
{
    private:
        std::deque< gpSizerWin > m_windowList;
        std::deque< gpSizerWin >::iterator m_it;

    public:
        gpHorizontalWindows();
        gpHorizontalWindows(wxWindow* window);
        gpHorizontalWindows(const gpHorizontalWindows& copy);
        bool AddWindow(wxWindow* window, int proportion = 1);
        void Clear();
        bool Exist(wxWindow *window);
        std::deque< gpSizerWin > GetWindowList() const;
        void Rewind();
        bool NextWindow(gpSizerWin& window);
};
class gpSizer
{
    private:
        std::deque< gpHorizontalWindows > m_windowList;
        wxBoxSizer  *m_topSizer;
        wxWindow    *m_parent;
        int         m_border;

    protected:
        gpSizer();
        void SetParent(wxWindow* parent);
        void AttachGpSizer();
        void AddNullWindow();
        int AddWindow(wxWindow* window);
        bool AddWindow(unsigned int id, wxWindow* window, int prop);
        wxBoxSizer* GetSizer();
        void Fit();
        void Realize();
};


#endif
