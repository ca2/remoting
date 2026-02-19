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


#include "gui/BaseDialog.h"
//#include "gui/::remoting::Window.h"
#include "gui/TextBox.h"

class NewFolderDialog : public BaseDialog
{
public:
  NewFolderDialog();
  NewFolderDialog(::remoting::Window *parent);
  ~NewFolderDialog();

  void setFileName(const ::scoped_string & scopedstrFilename);
  ::string getFileName();

//protected:

  //
  // Inherited from BaseDialog
  //

  virtual BOOL onInitDialog();
  virtual BOOL onNotify(UINT controlID, LPARAM data);
  virtual BOOL onCommand(UINT controlID, UINT notificationID);
  virtual BOOL onDestroy();

  //
  // Button event handlers
  //

  void onOkButtonClick();
  void onCancelButtonClick();

//private:

  void initControls();

//protected:

  //
  // Controls
  //

  ::remoting::Window m_label;
  TextBox m_fileNameTextBox;

  ::string m_strFileName;
};


