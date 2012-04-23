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

 #include "gpMenuGroup.h"

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