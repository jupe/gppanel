/**
 *	@file		gpPopupGroup.cpp
 *	@author     Jussi Vatjus-Anttila
 *	@date		5.11.2009
 *	@version	0.1
 *
 *  Purpose:         gpPopup handlers
 *  Original Author: Jussi Vatjus-Anttila
 *  Created:         5/11/2009
 *  Last edit:       5/11/2009
 *  Copyright:       (c) Jussi Vatjus-Anttila
 *  Licence:         wxWindows Library License v3.1
 *
 *	@note
 *  There is decrlarations all the gpPopup menu functions
 *  and wxMenuItem*s
 *
 */

#include "gpPopupGroup.h"


gpPopupGroup::gpPopupGroup()
{
	m_submenu=0;
	m_type=0;
}

gpPopupGroup::gpPopupGroup(int type, wxString label, wxString help)
{
	m_submenu = new wxMenu();
	m_groupLabel = label;
	m_groupHelp = help;
	m_type = type;
}
gpPopupGroup::gpPopupGroup(const gpPopupGroup& copy)
{
	m_groupLabel = copy.GetLabel();
	m_groupHelp = copy.GetHelp();
	m_popupList = copy.GetPopupList();
	m_submenu = copy;
	m_type = copy.m_type;
}

gpPopupGroup::operator wxMenu*() const
{ return m_submenu; }

void gpPopupGroup::Enable(int popup, bool b)
{
	gpPopup *item = Find(popup);
	if(item){
		((wxMenuItem*)(*item))->Enable(b);

		/// @todo find way to hide menuitem, which not enabled. Now those are only not clickable items..
		//wxMenu* menu = ((wxMenuItem*)(*item))->GetMenu();
		/*if(!b) menu->Delete( ((wxMenuItem*)(*item)) );
		else{
			menu->Append( ((wxMenuItem*)(*item)) );
		}*/

	}
}
void gpPopupGroup::Enable(int kind, int popup, bool b)
{ m_popupEnableList[kind][popup] = b; Select(kind); }

void gpPopupGroup::DisableAllItems( )
{
	if( !m_submenu )return;
	wxMenuItemList itemlist = m_submenu->GetMenuItems();
	wxMenuItemList::iterator it;
	for(it=itemlist.begin(); it!= itemlist.end(); ++it)
	{
		(*it)->Enable(false);
	}
	m_popupEnableList.clear();

}
bool gpPopupGroup::Select(int kind)
{
	bool kindFound=false;
	if(m_popupEnableList.find(kind)!= m_popupEnableList.end() )
		kindFound = true;
	gpPopup item;
	bool en;
	rewind();
	while(nextItem(item))
	{
		en = false;
		if(kindFound)
		{
			if( m_popupEnableList[kind].find( item.GetType() ) != m_popupEnableList[kind].end() )
			{  en = m_popupEnableList[kind][ item.GetType() ]; }
		}
		if(en){
			//if user has marked this to belong to chart kind group then enable it
			((wxMenuItem*)item)->Enable(true);
		}
		else ((wxMenuItem*)item)->Enable(false);
	}
	return true;
}
bool gpPopupGroup::IsEnabled(int type)
{
	gpPopup* item = Find(type);
	if(!item)return false; //cannot find
	return item->GetItem()->IsEnabled();
}
void gpPopupGroup::push(gpPopup popup)
{ m_popupList.push_back(popup); }

void gpPopupGroup::clear()
{ m_popupList.clear(); }

//wxString GetLabel() const { return m_submenu->GetLabel(); }
wxString gpPopupGroup::GetLabel() const
{ return m_groupLabel; }

wxString gpPopupGroup::GetHelp() const
{ return m_groupHelp; }

gpPopupList_t gpPopupGroup::GetPopupList() const
{ return m_popupList; }

void gpPopupGroup::rewind()
{ it = m_popupList.begin(); }

bool gpPopupGroup::nextItem(gpPopup& item)
{
	 if(it != m_popupList.end())
	{ item=*it; it++; return true; }
	return false;
}
bool gpPopupGroup::IdBelong(int id)
{
	rewind();
	gpPopup item;
	while(nextItem(item))
	{
		if( ((wxMenuItem*)item)->GetId() == id ) return true;
	}
	return false;
}

bool gpPopupGroup::SetLabel(int type, wxString label, wxString help)
{
	gpPopup* pop = Find(type);
	if(pop){
		((wxMenuItem*)*pop)->SetText(label);
		((wxMenuItem*)*pop)->SetHelp(help);
		return true;
	}	return false;
}

/* PRIVATE */

gpPopup* gpPopupGroup::Find(int type)
{
	gpPopupList_t::iterator it;
	for(it=m_popupList.begin(); it!=m_popupList.end(); ++it)
	{ if(it->GetType() == type)return &(*it); }
	return 0;
}
