/**
 *	@file		gpPopup.h
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
#ifndef GP_POPUP_H
#define GP_POPUP_H

/**
*  popup:
*   -type
*   -wxMenuItem
*/
class gpPopup
{
    private:
        int         m_type;
        wxMenuItem* m_item;
    public:
        gpPopup(){m_item=0;m_type=0;}
        gpPopup(int type, wxMenuItem* item) :
            m_type(type), m_item(item)
        { }
        gpPopup(const gpPopup& copy):
            m_type(copy.GetType()), m_item( copy )
        { }

        void Check(bool b) { if(m_item->IsCheckable() )m_item->Check(b); }
        int GetType() const { return m_type; }
        operator wxMenuItem*() const { return m_item; }
        wxMenuItem* const GetItem(){return m_item;}
};
typedef std::vector<gpPopup>  			gpPopupList_t;
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
        gpPopupGroup(){m_submenu=0;m_type=0;}
        gpPopupGroup(int type, wxString label, wxString help= wxEmptyString)
        {   m_submenu = new wxMenu();
            m_groupLabel = label;
            m_groupHelp = help;
            m_type = type;
        }
        gpPopupGroup(const gpPopupGroup& copy)
        {
            m_groupLabel = copy.GetLabel();
            m_groupHelp = copy.GetHelp();
            m_popupList = copy.GetPopupList();
            m_submenu = copy;
            m_type = copy.m_type;
        }
        operator wxMenu*() const { return m_submenu; }


        void Enable(int popup, bool b=true)
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
        void Enable(int kind, int popup, bool b=true)
        { m_popupEnableList[kind][popup] = b; Select(kind); }

        void DisableAllItems( )
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
        bool Select(int kind)
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
        bool IsEnabled(int type)
        {
            gpPopup* item = Find(type);
            if(!item)return false; //cannot find
            return item->GetItem()->IsEnabled();
        }
        void push(gpPopup popup){ m_popupList.push_back(popup); }
        void clear(){ m_popupList.clear(); }
        //wxString GetLabel() const { return m_submenu->GetLabel(); }
        wxString GetLabel() const { return m_groupLabel; }
        wxString GetHelp() const { return m_groupHelp; }

        gpPopupList_t GetPopupList() const { return m_popupList; }
        void rewind() { it = m_popupList.begin(); }
        bool nextItem(gpPopup& item)
        {
             if(it != m_popupList.end())
            { item=*it; it++; return true; }
            return false;
        }
        bool IdBelong(int id)
        {
            rewind();
            gpPopup item;
            while(nextItem(item))
            {
                if( ((wxMenuItem*)item)->GetId() == id ) return true;
            }
            return false;
        }

		bool SetLabel(int type, wxString label, wxString help=wxEmptyString)
		{
			gpPopup* pop = Find(type);
			if(pop){
				((wxMenuItem*)*pop)->SetText(label);
				((wxMenuItem*)*pop)->SetHelp(help);
				return true;
			}	return false;
		}

	private:
        gpPopup* Find(int type)
        {
            gpPopupList_t::iterator it;
            for(it=m_popupList.begin(); it!=m_popupList.end(); ++it)
            { if(it->GetType() == type)return &(*it); }
            return 0;
        }
};
typedef std::map<int, gpPopupGroup> gpPopupGroupList_t;


/*
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
