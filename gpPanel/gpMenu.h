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
        std::map<int, wxMenuItem*> idMapMenuItem;
		/** Map iterator for menuitems map*/
        std::map<int, wxMenuItem*>::iterator itMenu;

		/** ToolBar where menuitems is connected */
        wxToolBar*      m_toolbar;

		/** Vector for toolbaritems id */
        std::vector<int> idMapToolBarItem;
		/** iterator for toolbaritem vector */
        std::vector<int>::iterator itTool;

    public:
		/** Default constructor. */
        menuGroup()
        {
            itMenu=idMapMenuItem.begin();
            itTool=idMapToolBarItem.begin();
            m_toolbar=0;
        }
		/** Copy Constructor. */
        menuGroup(const menuGroup& copy)
        {
            idMapMenuItem = copy.GetMap();
            itMenu =idMapMenuItem.begin();
            idMapToolBarItem = copy.GetToolBarIdList();
            itTool = idMapToolBarItem.begin();
            m_toolbar = copy.GetToolBar();
        }

		/** @return Get menuitem's map. */
        std::map<int, wxMenuItem*> GetMap() const {return idMapMenuItem;}
		/** Add new item's to map */
        void Add(int id, wxMenuItem* item){ idMapMenuItem[id] = item; }

		/** Reset menuitems map iterator */
        void RewindMenuItems(){itMenu=idMapMenuItem.begin();}
		/** Get next item ref.pointer.
			@param[out] item	menuitem
			@return true if any
		*/
        bool NextItem(wxMenuItem*& item)
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
        wxToolBar* GetToolBar() const {return m_toolbar;}
		/** @param toolbar Set toolbar pointer */
        void SetToolBar(wxToolBar* toolbar){m_toolbar = toolbar;}
		/** @param id Add extra id to id map */
        void AddToolBarId(int id){ idMapToolBarItem.push_back(id);}
		/** @return Get id vector */
        std::vector<int> GetToolBarIdList() const {return idMapToolBarItem;}
		/** Reset id map iterator */
        void RewindToolBarItems(){itTool=idMapToolBarItem.begin();}
		/**
			Get next id
			@param[out]		item	Next item
			@return true if any
		*/
        bool NextToolBarItem(int& item)
        {
            if(itTool!=idMapToolBarItem.end())
            {
                item = *itTool;
                itTool++;
                return true;
            }
            return false;
        }
};

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
        gpMenu()
        {
            menu = new wxMenu();
            submenu = 0;
            primaryId = 0;
        }
		/** Default destructor */
        virtual ~gpMenu(){}
        /**
         *   Function to get wxMenuItem* by id
         *   @param      id              id of wxMenuItem
         *   @return     wxMenuItem*     pointer to wxMenuItem
         */
        wxMenuItem*  GetMenuItemById(long id, wxMenu *men=0)
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
        /**
         *  Add new menu item to gpLayer
         *  @param label    label of menu
         *  @param kind     menu item type, default: wxITEM_NORMAL
         *  @param checked  if menuitem type is wxITEM_CHECK
         *  @return item id for example using connect(), see example.
         */
        int addMenuItem(wxString label, wxString help = wxEmptyString, wxItemKind kind = wxITEM_NORMAL, bool checked = true, int togroup=0)
        {
            int id = wxNewId();
            if(label.Length()==0)return 0;//label cannot be empty
            wxMenuItem* item = new wxMenuItem(menu, id ,label , help, kind);
            menu->Append( item );


            if(togroup) m_group[togroup].Add(id, item);

            if(item->IsCheckable())item->Check(checked);
            return id;

        }
		/**
		 * Add new submenu item to menu structure
		 *	@param label	menu label
		 *	@param help		help string
		 *	@param kind		item kind
		 *	@param checked 	true possible only if checkable item and
		 *	@param togrou	group name to attach
		 *	@return return created wxMenuItem id
		 */
        int addSubMenuItem(wxString label, wxString help = wxEmptyString, wxItemKind kind = wxITEM_NORMAL, bool checked = true, int togroup=0)
        {
            int id = wxNewId();
            if(label.Length()==0)return 0;//label cannot be empty
            wxMenuItem* item = new wxMenuItem(submenu, id ,label , help, kind);
            submenu->Append( item );

            if(togroup) m_group[togroup].Add(id, item);

            if(item->IsCheckable())item->Check(checked);
            return id;
        }
		/**
			@param label	submenu label
		*/
        void newSubMenu(wxString label = wxEmptyString)
        {
            submenu = new wxMenu(label);
        }
		/** Append new submenu item
		*	@param label	submenu label
		*	@param helpString	help string for item.
		*	@return return item id
		*/
        int AppendSubMenu(wxString label , wxString helpString=wxEmptyString)
        {
            int id=wxNewId();
            if(label.Length()==0)return 0; //label cannot be empty
            menu->Append( id, label , submenu, helpString);
            return id;
        }
        /**
         *   There is also possible to add extra id's,
         *   which not belong to menu's
         */
        int AddId(int id){ extraIdList.push_back(id); return id;}
		/**
		*	Push new id to map
		*	@return Create new id and return it.
		*/
        int AddNewId(bool primary=false)
        {
            int id =  AddId( wxNewId() );
            if( primary ) primaryId = id;
            return id;
        }
        int GetPrimaryId() const { return primaryId; }
		/**
		*	Set toolbar to group
		*	@param toolbar	toolbar to attach
		*	@param group	group where want to put.
		*/
        void SetToolbarGroup(wxToolBar* toolbar, int group)
		{
			m_group[group].SetToolBar(toolbar);
		}
		/**
		*	Add toolbar id to group
		*	@param group	group for toolbar
		*	@param id		id to attach
		*/
        void AddToolBarIdToGroup(int group, int id)
        {
            m_group[group].AddToolBarId(id);
            AddId(id);
        }

		/**
		*	Check menu item if checkable
		*	@param id		id of item
		*	@param checked	true if want check
		*/
        void CheckMenuItem(int id, bool checked = true)
        {
            wxMenuItem * item = GetMenuItemById(id);
            CheckMenuItem(item, checked);
        }
		/**
		*	Check whole group
		*	@param group	group to be check
		*	@param checked	true if want all to be checked
		*/
        void CheckMenuGroup(int group, bool checked = true)
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

        /**
         * Check if id belongs to menu list
         */
        bool MenuIdBelongs(int id)
        {
            bool found = (GetMenuItemById(id)==0 ? false : true);
            if(! found &&  find(extraIdList.begin(), extraIdList.end(), id) != extraIdList.end() )
                found = true;

            return found;
        }
        /**
         *  Get menu list
         *  @return wxMenuItemList&
         */
        wxMenuItemList&     GetMenuItems(){return menu->GetMenuItems();}
        /**
         *  Get whole wxMenu pointer
         *  @return wxMenu*
         */
        wxMenu*             GetMenu(){return menu;}

    private:

		/**
		*	Check item
		*	@param item	item to be change check state
		*	@param checked true if want check
		*/
        void CheckMenuItem(wxMenuItem* item, bool checked = true)
        {
            if(item)
            {
                if(item->IsCheckable())
                {
                    item->Check(checked);
                }
            }
        }
		/**
		*	Check toolbaritem item
		*	@param toolbar	toolbar to be change check state
		*	@param id		id of menuitem
		*	@param checked	true if want check
		*/
        void CheckToolBarItem(wxToolBar* toolbar, int id, bool checked = true)
        {
            if(!toolbar)return;
            toolbar->ToggleTool(id, checked);
        }


        wxMenu              *menu;      	//!< wxMenu for handling
        wxMenu              *submenu;   	//!< submenu
        std::vector<int>     extraIdList;  //!< Other id's to belong to this
        int                 primaryId;      //!< Primary ID

        std::map<int, menuGroup> m_group;	//!< Map for menu groups

};



#endif
