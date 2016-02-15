/**
 *	@file		gpMenu.cpp
 *	@author     Jussi Vatjus-Anttila
 *	@date		23.04.2012
 *	@version	0.1
 *
 *  Purpose:         Menu group
 *  Original Author: Jussi Vatjus-Anttila
 *  Created:         23/04/2012
 *  Last edit:
 *  Copyright:       (c) Jussi Vatjus-Anttila
 *  Licence:         wxWindows Library License v3.1
 *
 */

 #include "gpMenu.h"

gpMenu::gpMenu()
{
	m_menu = new wxMenu();
	m_submenu = 0;
	m_primaryId = 0;
}

gpMenu::~gpMenu()
{
    delete m_menu;
    delete m_submenu;
}

wxMenuItem*  gpMenu::GetMenuItemById(long id, wxMenu *men)
{
	wxMenuItemList list;
	list = men==0?GetMenuItems():men->GetMenuItems();
	wxMenuItemList::iterator it, it2;
	wxMenuItem* item;
	for(it = list.begin(); it!= list.end(); it++)
	{
		if( (*it)->GetId() == id)return *it;

		if( (*it)->GetSubMenu() != 0)
		{
			if( (item=GetMenuItemById(id, (*it)->GetSubMenu() )) )
			{
				return item;
			}
		}
	}
	return 0;
}
int gpMenu::addMenuItem(wxString label, wxString help, wxItemKind kind, bool checked, int togroup)
{
	int id = wxNewId();
	if(label.Length()==0)return 0;//label cannot be empty
	wxMenuItem* item = new wxMenuItem(m_menu, id ,label , help, kind);
	m_menu->Append( item );


	if(togroup) m_group[togroup].Add(id, item);

	if(item->IsCheckable())item->Check(checked);
	return id;

}
int gpMenu::addSubMenuItem(wxString label, wxString help, wxItemKind kind, bool checked, int togroup)
{
	int id = wxNewId();
	if(label.Length()==0)return 0;//label cannot be empty
	wxMenuItem* item = new wxMenuItem(m_submenu, id ,label , help, kind);
	m_submenu->Append( item );

	if(togroup) m_group[togroup].Add(id, item);

	if(item->IsCheckable())item->Check(checked);
	return id;
}
void gpMenu::newSubMenu(wxString label)
{
	m_submenu = new wxMenu(label);
}
int gpMenu::AppendSubMenu(wxString label , wxString helpString)
{
	int id=wxNewId();
	if(label.Length()==0)return 0; //label cannot be empty
	m_menu->Append( id, label , m_submenu, helpString);
	return id;
}
int gpMenu::AddId(int id)
{
	m_extraIdList.push_back(id);
	return id;
}
int gpMenu::AddNewId(bool primary)
{
	int id =  AddId( wxNewId() );
	if( primary ) m_primaryId = id;
	return id;
}
int gpMenu::GetPrimaryId() const
{
	return m_primaryId;
}
void gpMenu::SetToolbarGroup(wxToolBar* toolbar, int group)
{
	m_group[group].SetToolBar(toolbar);
}
void gpMenu::AddToolBarIdToGroup(int group, int id)
{
	m_group[group].AddToolBarId(id);
	AddId(id);
}
void gpMenu::CheckMenuItem(int id, bool checked)
{
	wxMenuItem * item = GetMenuItemById(id);
	CheckMenuItem(item, checked);
}
void gpMenu::CheckMenuGroup(int group, bool checked)
{
	if (m_group.find(group)==m_group.end()) return;

	wxMenuItem* item;
	m_group[group].RewindMenuItems();
	while( m_group[group].NextItem(item))
		CheckMenuItem(item, checked);

	int id;
	m_group[group].RewindToolBarItems();
	while( m_group[group].NextToolBarItem(id))
		CheckToolBarItem(m_group[group].GetToolBar(), id, checked);
}
bool gpMenu::MenuIdBelongs(int id)
{
	bool found = (GetMenuItemById(id)==0 ? false : true);
	if(! found &&  find(m_extraIdList.begin(), m_extraIdList.end(), id) != m_extraIdList.end() )
		found = true;

	return found;
}
wxMenuItemList&  gpMenu::GetMenuItems()
{
	return m_menu->GetMenuItems();
}

wxMenu* gpMenu::GetMenu()
{
	return m_menu;
}



void gpMenu::CheckMenuItem(wxMenuItem* item, bool checked)
{
	if(item)
	{
		if(item->IsCheckable())
		{
			item->Check(checked);
		}
	}
}
void gpMenu::CheckToolBarItem(wxToolBar* toolbar, int id, bool checked)
{
	if(!toolbar)return;
	toolbar->ToggleTool(id, checked);
}
