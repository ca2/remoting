// Copyright (C) 2012 GlavSoft LLC.
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
#include "AuthenticationDialog.h"

AuthenticationDialog::AuthenticationDialog()
: BaseDialog(IDD_DAUTH)
{
}


BOOL AuthenticationDialog::onInitDialog() 
{
  setControlById(m_hostname, IDC_EHOST);
  m_hostname.setText(m_strHost);
  setControlById(m_password, IDC_EPASSW);
  m_password.set_focus();
  return FALSE;
}

void AuthenticationDialog::setHostName(const ::scoped_string & hostname)
{
  m_strHost = hostname;
}

BOOL AuthenticationDialog::onCommand(UINT controlID, UINT notificationID)
{
  if (controlID == IDOK) {
    m_strPassword = m_password.get_text();
    close_dialog(1);
    return TRUE;
  }
  if (controlID == IDCANCEL) {
    close_dialog(0);
    return TRUE;
  }
  return FALSE;
}

string AuthenticationDialog::getPassword()
{
  return m_strPassword;
}
