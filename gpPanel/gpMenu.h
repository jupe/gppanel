/**
 *	@file		gpMenu.h
 *	@author     Jussi Vatjus-Anttila
 *	@date		13.10.2009
 *	@version	0.1
 *
 *  Purpose:         Menu group
 *  Original Author: Jussi Vatjus-Anttila
 *  Created:         10/09/2009
 *  Last edit:       21/04/2012
 *  Copyright:       (c) Jussi Vatjus-Anttila
 *  Licence:         wxWindows Library License v3.1
 *
 */

#ifndef GPMENU_H
#define GPMENU_H

#define gpMENU_GROUP_VISIBLE 1

#include <algorithm>


#include <wx/menu.h>
#include <wx/toolbar.h>
#include <wx/hashmap.h>

//#include <wx/vector.h>
#include <vector>

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
/**
* Class for handling gpLayer menu.
* User can add menu items to gpLayer and get
* MenuItemList for adding main menu.
*
* example:
*   int id = histogram->addMenuItem(_("Enable"), wxITEM_CHECK, true);
*   MyMenu->Append( wxNewId(), _("HISTOGRAM"), histogram->GetMenu(), wxEmptyString);
*   Connect(idd,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&gpSampleFrame::OnMenuEnableSelected);
*/
class gpMenu
{
    public:
        /**
         *   Constructor
         *   Create new wxMenu.
         *
         */
        gpMenu();
		/** Default destructor */
        virtual ~gpMenu();
        /**
         *   Function to get wxMenuItem* by id
         *   @param      id              id of wxMenuItem
         *   @return     wxMenuItem*     pointer to wxMenuItem
         */
        wxMenuItem*  GetMenuItemById(long id, wxMenu *men=0);
        /**
         *  Add new menu item to gpLayer
         *  @param label    label of menu
         *  @param kind     menu item type, default: wxITEM_NORMAL
         *  @param checked  if menuitem type is wxITEM_CHECK
         *  @return item id for example using connect(), see example.
         */
        int addMenuItem(wxString label,
						wxString help = wxEmptyString,
						wxItemKind kind = wxITEM_NORMAL,
						bool checked = true,
						int togroup=0);
		/**
		 * Add new submenu item to menu structure
		 *	@param label	menu label
		 *	@param help		help string
		 *	@param kind		item kind
		 *	@param checked 	true possible only if checkable item and
		 *	@param togrou	group name to attach
		 *	@return return created wxMenuItem id
		 */
        int addSubMenuItem(	wxString label,
							wxString help = wxEmptyString,
							wxItemKind kind = wxITEM_NORMAL,
							bool checked = true,
							int togroup=0);
		/**
			@param label	submenu label
		*/
        void newSubMenu(wxString label = wxEmptyString);
		/** Append new submenu item
		*	@param label	submenu label
		*	@param helpString	help string for item.
		*	@return return item id
		*/
        int AppendSubMenu(wxString label, wxString helpString=wxEmptyString);
        /**
         *   There is also possible to add extra id's,
         *   which not belong to menu's
         */
        int AddId(int id);
		/**
		*	Push new id to map
		*	@return Create new id and return it.
		*/
        int AddNewId(bool primary=false);

        int GetPrimaryId() const;
		/**
		*	Set toolbar to group
		*	@param toolbar	toolbar to attach
		*	@param group	group where want to put.
		*/
        void SetToolbarGroup(wxToolBar* toolbar, int group);
		/**
		*	Add toolbar id to group
		*	@param group	group for toolbar
		*	@param id		id to attach
		*/
        void AddToolBarIdToGroup(int group, int id);

		/**
		*	Check menu item if checkable
		*	@param id		id of item
		*	@param checked	true if want check
		*/
        void CheckMenuItem(int id, bool checked = true);
		/**
		*	Check whole group
		*	@param group	group to be check
		*	@param checked	true if want all to be checked
		*/
        void CheckMenuGroup(int group, bool checked = true);

        /**
         * Check if id belongs to menu list
         */
        bool MenuIdBelongs(int id);
        /**
         *  Get menu list
         *  @return wxMenuItemList&
         */
        wxMenuItemList&     GetMenuItems();
        /**
         *  Get whole wxMenu pointer
         *  @return wxMenu*
         */
        wxMenu*             GetMenu();

    private:

		/**
		*	Check item
		*	@param item	item to be change check state
		*	@param checked true if want check
		*/
        void CheckMenuItem(wxMenuItem* item, bool checked = true);
		/**
		*	Check toolbaritem item
		*	@param toolbar	toolbar to be change check state
		*	@param id		id of menuitem
		*	@param checked	true if want check
		*/
        void CheckToolBarItem(wxToolBar* toolbar, int id, bool checked = true);


        wxMenu              *menu;      	//!< wxMenu for handling
        wxMenu              *submenu;   	//!< submenu
        std::vector<int>    extraIdList;  	//!< Other id's to belong to this
        int                 primaryId;      //!< Primary ID

        MenuGroupHashMap    m_group;	//!< Map for menu groups

};



#endif
