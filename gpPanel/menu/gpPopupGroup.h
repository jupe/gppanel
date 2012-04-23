/**
 *	@file		gpPopupGroup.h
 *	@author     Jussi Vatjus-Anttila
 *	@date		23.04.2012
 *	@version	0.1
 *
 *  Purpose:         gpPopup handlers
 *  Original Author: Jussi Vatjus-Anttila
 *  Created:         23.04.2012
 *  Last edit:       23.04.2012
 *  Copyright:       (c) Jussi Vatjus-Anttila
 *  Licence:         wxWindows Library License v3.1
 *
 *	@note
 *  There is decrlarations all the gpPopup menu functions
 *  and wxMenuItem*s
 *
 */
#ifndef _GPPOPUP_GROUP_H
#define _GPPOPUP_GROUP_H

#include "gpPopup.h"

#include <vector>
#include <map>

typedef std::vector<gpPopup>            gpPopupList_t;
typedef std::map<int, bool>  			gpEnableMap_t;
typedef std::map<int, gpEnableMap_t >  	gpEnableList_t;

/**
*
*  group:
*
*  -label  (m_groupLabel)
*  -popups (m_popupList)
*  -menu  wich contains all popups (m_submenu)
*  -enablelist (m_popupEnableList)
*  gpYAXIS_DEFAULT = true
*
*/
class gpPopupGroup
{
    private:
        wxString    m_groupLabel,
                    m_groupHelp;
        int         m_type;
        gpPopupList_t m_popupList;
		/*
		[kind][popup][bool]
		gpCHART_KIND
		*/
        gpEnableList_t 	m_popupEnableList;
        gpPopupList_t::iterator it;
        wxMenu*     	m_submenu;

    public:
        gpPopupGroup();
        gpPopupGroup(int type, wxString label, wxString help= wxEmptyString);
        gpPopupGroup(const gpPopupGroup& copy);
        operator wxMenu*() const;


        void Enable(int popup, bool b=true);
        void Enable(int kind, int popup, bool b=true);

        void DisableAllItems( );
        bool Select(int kind);
        bool IsEnabled(int type);
        void push(gpPopup popup);
        void clear();
        //wxString GetLabel() const { return m_submenu->GetLabel(); }
        wxString GetLabel() const;
        wxString GetHelp() const;

        gpPopupList_t GetPopupList() const;
        void rewind();
        bool nextItem(gpPopup& item);
        bool IdBelong(int id);
		bool SetLabel(int type, wxString label, wxString help=wxEmptyString);

	private:
        gpPopup* Find(int type);
};

typedef std::map<int, gpPopupGroup> gpPopupGroupList_t;


/*

Examples:

gpPopupGroupList_t list;
wxMenuItem* item;
wxMenu *popup = mpWindow->GetPopupMenu();

gpPopupGroup group(_("Y vs X"));
item = ((wxMenu*)group)->Append(wxNewId, others) //submenu append
group->pushYpopup( gpPopup( gpYAXIS_DEFAULT, _("LSB"), true, item ) );
list[gpCHART_KIND] = group;

popup->Append( wxNewId(), group.GetLabel(), group, group.GetHelp() );


change chart event
int type = gpCHART_CUSTOM
list[type].Enable(bool t);

//if( list[type].IsEnabled(gpYAXIS_DEFAULT) ) not necessary
  layer->ChangeYaxis(gpYAXIS_DEFAULT);
*/




#endif
