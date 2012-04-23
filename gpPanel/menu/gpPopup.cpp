/**
 *	@file		gpPopup.cpp
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

#include "gpPopup.h"

gpPopup::gpPopup()
{m_item=0;m_type=0;}

gpPopup::gpPopup(int type, wxMenuItem* item) :
	m_type(type), m_item(item)
{ }

gpPopup::gpPopup(const gpPopup& copy):
	m_type(copy.GetType()), m_item( copy )
{ }

void gpPopup::Check(bool b)
{ if(m_item->IsCheckable() )m_item->Check(b); }

int gpPopup::GetType() const
{ return m_type; }

gpPopup::operator wxMenuItem*() const
{ return m_item; }

wxMenuItem* const gpPopup::GetItem()
{return m_item;}