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
#include "framework.h"
#include "NewFolderDialog.h"
#include "resource.h"
#include "remoting/common/remoting.h"

NewFolderDialog::NewFolderDialog()
{
  setResourceId(ftclient_createFolderDialog);
  m_strFileName= "";
}

NewFolderDialog::NewFolderDialog(::remoting::Window *parent)
{
  setResourceId(ftclient_createFolderDialog);
  set_parent(parent);
  m_strFileName= "";
}

NewFolderDialog::~NewFolderDialog()
{
}

void NewFolderDialog::setFileName(const ::scoped_string & scopedstrFilename)
{
  m_strFileName= scopedstrFilename;
}

::string NewFolderDialog::getFileName()
{
  return m_strFileName;
}

BOOL NewFolderDialog::onInitDialog()
{
  initControls();
  m_fileNameTextBox.setText(m_strFileName);
  return TRUE;
}

BOOL NewFolderDialog::onNotify(UINT controlID, LPARAM data)
{
  return TRUE;
}

BOOL NewFolderDialog::onCommand(UINT controlID, UINT notificationID)
{
  switch (controlID) {
  case IDOK:
    onOkButtonClick();
    break;
  case IDCANCEL:
    onCancelButtonClick();
    break;
  }
  return TRUE;
}

BOOL NewFolderDialog::onDestroy()
{
  return TRUE;
}

void NewFolderDialog::onOkButtonClick()
{
  ::string fileName;

  fileName = m_fileNameTextBox.get_text();

  if (fileName.is_empty() || fileName.contains_any_character_in("\\/"))
     {
    ::remoting::message_box(m_hwnd,
               L"::file::item name cannot be empty and cannot contain '/' or '\\' characters.",
               L"Incorrect ::file::item Name",
               MB_OK | MB_ICONWARNING);
    m_fileNameTextBox.set_focus();
    return ;
  }

  m_strFileName = fileName;

  close_dialog(IDOK);
}

void NewFolderDialog::onCancelButtonClick()
{
  close_dialog(IDCANCEL);
}

void NewFolderDialog::initControls()
{
  HWND hwnd = m_hwnd;

  m_label.setWindow(GetDlgItem(hwnd, IDC_LABEL));
  m_fileNameTextBox.setWindow(GetDlgItem(hwnd, IDC_FILENAME_EDIT));
}
