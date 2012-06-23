/**
 *	@file		gpMenu.h
 *	@author     Jussi Vatjus-Anttila
 *	@date		23.04.2012
 *	@version	0.1
 *
 *  Purpose:         Menu group
 *  Original Author: Jussi Vatjus-Anttila
 *  Created:         23/04/2012
 *  Last edit:       23/04/2012
 *  Copyright:       (c) Jussi Vatjus-Anttila
 *  Licence:         wxWindows Library License v3.1
 *
 */

#ifndef GPMENU_GROUP_H
#define GPMENU_GROUP_H

#define gpMENU_GROUP_VISIBLE 1

#include <wx/menu.h>
#include <wx/toolbar.h>
#include <wx/hashmap.h>

//#include <wx/vector.h>
#include <vector>
#include <algorithm>

// hash map with int keys and wxMenuItem* values
WX_DECLARE_HASH_MAP( int, wxMenuItem*, wxIntegerHash, wxIntegerEqual, MenuHashMap );

/**
*
*	Class for Menu Group
*	This contains
*	-one group of wxMenuItem's.
*	-one wxToolBar
*	-id list for toolbar's
*	Possible to loop wxMenuItem's by calling
*	RewindMenuItems(), and NextItem()
*
*/
class menuGroup
{
    private:
		/** Map for wxMenuItem's*/
        MenuHashMap idMapMenuItem;
		/** Map iterator for menuitems map*/
        MenuHashMap::iterator itMenu;

		/** ToolBar where menuitems is connected */
        wxToolBar*      m_toolbar;

		/** Vector for toolbaritems id */
        std::vector<int> idMapToolBarItem;
		/** iterator for toolbaritem vector */
        std::vector<int>::iterator itTool;

    public:
		/** Default constructor. */
        menuGroup();
		/** Copy Constructor. */
        menuGroup(const menuGroup& copy);

		/** @return Get menuitem's map. */
        MenuHashMap GetMap() const;
		/** Add new item's to map */
        void Add(int id, wxMenuItem* item);

		/** Reset menuitems map iterator */
        void RewindMenuItems();
		/** Get next item ref.pointer.
			@param[out] item	menuitem
			@return true if any
		*/
        bool NextItem(wxMenuItem*& item);
		/** @return Get toolbar pointer */
        wxToolBar* GetToolBar() const;
		/** @param toolbar Set toolbar pointer */
        void SetToolBar(wxToolBar* toolbar);
		/** @param id Add extra id to id map */
        void AddToolBarId(int id);
		/** @return Get id vector */
        std::vector<int> GetToolBarIdList() const ;
		/** Reset id map iterator */
        void RewindToolBarItems();
		/**
			Get next id
			@param[out]		item	Next item
			@return true if any
		*/
        bool NextToolBarItem(int& item);
};


WX_DECLARE_HASH_MAP( int, menuGroup, wxIntegerHash, wxIntegerEqual, MenuGroupHashMap );

#endif
