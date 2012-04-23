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

menuGroup::menuGroup()
{
	itMenu=idMapMenuItem.begin();
	itTool=idMapToolBarItem.begin();
	m_toolbar=0;
}
menuGroup::menuGroup(const menuGroup& copy)
{
	idMapMenuItem = copy.GetMap();
	itMenu =idMapMenuItem.begin();
	idMapToolBarItem = copy.GetToolBarIdList();
	itTool = idMapToolBarItem.begin();
	m_toolbar = copy.GetToolBar();
}
MenuHashMap menuGroup::GetMap() const
{	return idMapMenuItem;	}

void menuGroup::Add(int id, wxMenuItem* item)
{ idMapMenuItem[id] = item; }

void menuGroup::RewindMenuItems()
{itMenu=idMapMenuItem.begin();}

bool menuGroup::NextItem(wxMenuItem*& item)
{
	if(itMenu!=idMapMenuItem.end())
	{
		item = itMenu->second;
		itMenu++;
		return true;
	}
	return false;
}
/** @return Get toolbar pointer */
wxToolBar* menuGroup::GetToolBar() const
{ return m_toolbar; }

void menuGroup::SetToolBar(wxToolBar* toolbar)
{ m_toolbar = toolbar;}

void menuGroup::AddToolBarId(int id)
{ idMapToolBarItem.push_back(id);}

std::vector<int> menuGroup::GetToolBarIdList() const
{ return idMapToolBarItem; }

void menuGroup::RewindToolBarItems()
{ itTool=idMapToolBarItem.begin(); }

bool menuGroup::NextToolBarItem(int& item)
{
	if(itTool!=idMapToolBarItem.end())
	{
		item = *itTool;
		itTool++;
		return true;
	}
	return false;
}



gpMenu::gpMenu()
{
	menu = new wxMenu();
	submenu = 0;
	primaryId = 0;
}
gpMenu::~gpMenu(){}
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
	wxMenuItem* item = new wxMenuItem(menu, id ,label , help, kind);
	menu->Append( item );


	if(togroup) m_group[togroup].Add(id, item);

	if(item->IsCheckable())item->Check(checked);
	return id;

}
int gpMenu::addSubMenuItem(wxString label, wxString help, wxItemKind kind, bool checked, int togroup)
{
	int id = wxNewId();
	if(label.Length()==0)return 0;//label cannot be empty
	wxMenuItem* item = new wxMenuItem(submenu, id ,label , help, kind);
	submenu->Append( item );

	if(togroup) m_group[togroup].Add(id, item);

	if(item->IsCheckable())item->Check(checked);
	return id;
}
void gpMenu::newSubMenu(wxString label)
{
	submenu = new wxMenu(label);
}
int gpMenu::AppendSubMenu(wxString label , wxString helpString)
{
	int id=wxNewId();
	if(label.Length()==0)return 0; //label cannot be empty
	menu->Append( id, label , submenu, helpString);
	return id;
}
int gpMenu::AddId(int id)
{
	extraIdList.push_back(id);
	return id;
}
int gpMenu::AddNewId(bool primary)
{
	int id =  AddId( wxNewId() );
	if( primary ) primaryId = id;
	return id;
}
int gpMenu::GetPrimaryId() const
{
	return primaryId;
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
	if(! found &&  find(extraIdList.begin(), extraIdList.end(), id) != extraIdList.end() )
		found = true;

	return found;
}
wxMenuItemList&  gpMenu::GetMenuItems()
{
	return menu->GetMenuItems();
}

wxMenu* gpMenu::GetMenu()
{
	return menu;
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
