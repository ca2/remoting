// Copyright (C) 2009,2010,2011,2012 GlavSoft LLC.
// All rights reserved.
//
//-------------------------------------------------------------------------
// This file is part of the TightVNC software.  Please visit our Web site:
//
//                       http://www.tightvnc.com/
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, w_rite to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//

#pragma once


#include "BaseDialog.h"
#include "remoting/gui/Window.h"
//////#include "util/::string.h"

class Tab :
virtual public ::remoting::Window
{
public:
  Tab();
  Tab(BaseDialog *dialog, const ::scoped_string & scopedstrCaption);

  //
  // Access methods to protected members
  //

  void setCaption(const ::scoped_string & scopedstrCaption) { m_strCaption= scopedstrCaption; }

  ::string get_caption() {
    return m_strCaption;
  }

  void setDialog(BaseDialog *dialog) { m_pdialog = dialog; }
  BaseDialog *getDialog() { return m_pdialog; }

  //
  // Method return true if tab has dialog
  //

  bool isOk() { return m_pdialog != NULL; }

  //
  // Changes visible state of dialog donates by this tab
  //

  void set_visible(bool visible);

//protected:

  BaseDialog *m_pdialog;
  ::string m_strCaption;
};


