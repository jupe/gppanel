/**
 *	@file		gpPopup.h
 *	@author     Jussi Vatjus-Anttila
 *	@date		5.11.2009
 *	@version	0.1
 *
 *  Purpose:         gpPopup handlers
 *  Original Author: Jussi Vatjus-Anttila
 *  Created:         5/11/2009
 *  Last edit:       23/04/2012
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

#define POPUP_FILE      1
#define POPUP_CHART     2
#define POPUP_EDIT      4
#define POPUP_HELP      8
#define POPUP_FIT       16

#include <wx/menu.h>

/**
*  popup:
*   -type
*   -wxMenuItem
*/
class gpPopup
{
    private:
        int         m_type;		//!< popup type
        wxMenuItem* m_item;		//!< menu item
    public:
		/** Default constructor */
        gpPopup();
		/** Other constructor */
        gpPopup(int type, wxMenuItem* item);
		/** Copy  constructor */
        gpPopup(const gpPopup& copy);

        void Check(bool b);
        int GetType() const;
		/** Get item type */
        operator wxMenuItem*() const;
		/** Get item */
        wxMenuItem* const GetItem();
};

#endif
