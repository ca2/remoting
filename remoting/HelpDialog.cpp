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
#include "HelpDialog.h"

#include "remoting/remoting_common/util/ResourceLoader.h"
#include "resource.h"

HelpDialog::HelpDialog() 
: BaseDialog(IDD_CMDLINE)
{
}

BOOL HelpDialog::onInitDialog() 
{
  setControlById(m_shelp, IDC_SHELP);

  ::string helpString;
  ResourceLoader *rLoader = ResourceLoader::getInstance();
  rLoader->loadString(IDS_CMDLINE_HELP, helpString);
  m_shelp.setText(helpString);
  return TRUE;
}

BOOL HelpDialog::onCommand(UINT controlID, UINT notificationID)
{
  if (controlID == IDOK) {
    close_dialog(1);
    return TRUE;
  }
  if (controlID == IDCANCEL) {
    close_dialog(0);
    return TRUE;
  }
  return FALSE;
}
