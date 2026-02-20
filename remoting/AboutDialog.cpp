// Copyright (C) 2010,2011,2012 GlavSoft LLC.
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
#include "AboutDialog.h"
#include "remoting/remoting_common/win_system/Shell.h"
#include "remoting/remoting_common/win_system/VersionInfo.h"
//#include "remoting/remoting_common/win_system/Environment.h"
//#include "remoting/remoting_common/gui/::remoting::Window.h"
#include "BuildTime.h"
#include "resource.h"
#include "acme/filesystem/filesystem/file_context.h"
#include "remoting/remoting_common/remoting.h"

AboutDialog::AboutDialog()
: BaseDialog(IDD_ABOUT_DIALOG)
{
}

AboutDialog::~AboutDialog()
{
}

void AboutDialog::onCloseButtonClick()
{
  close_dialog(IDCANCEL);
}

void AboutDialog::onOrderSupportButtonClock()
{
  openUrl(StringTable::getString(IDS_URL_LICENSING_FVA));
}

void AboutDialog::onVisitSiteButtonClick()
{
  openUrl(StringTable::getString(IDS_URL_PRODUCT_FVA));
}

void AboutDialog::openUrl(const ::scoped_string & scopedstrUrl)
{
  try {
    Shell::open(scopedstrUrl, 0, 0);
  } catch (SystemException &sysEx) {
    ::string strMessage;

    strMessage.formatf(StringTable::getString(IDS_FAILED_TO_OPEN_URL_FORMAT).c_str(), sysEx.get_message());

    ::remoting::message_box(m_hwnd,
               wstring(strMessage),
               wstring(StringTable::getString(IDS_MBC_TVNVIEWER)),
               MB_OK | MB_ICONEXCLAMATION);
  }
}

BOOL AboutDialog::onInitDialog()
{
  // Update product version string.
  ::string versionString("unknown");
  try {
    ::string binaryPath;
binaryPath = ::system()->file()->module();
    VersionInfo productInfo(binaryPath);
    versionString= productInfo.getProductVersionString();
  } catch (SystemException &ex) {
    ::remoting::message_box(m_hwnd,
               ::wstring(ex.get_message()),
               ::wstring(StringTable::getString(IDS_MBC_TVNVIEWER)),
               MB_OK | MB_ICONEXCLAMATION);
  }

  // Format product version and build time for displaying on the dialog.
  ::string versionText;
  versionText.formatf(StringTable::getString(IDS_PRODUCT_VERSION_FORMAT).c_str(),
                     ::string(versionString).c_str(),
                     BuildTime::DATE);

  // Show version info on the dialog.
  ::remoting::Window versionLabel;
  versionLabel.setWindow(GetDlgItem(m_hwnd, IDC_STATIC_VERSION));
  versionLabel.setText(versionText);

  // Show licensing info and/or special build info.
  ::remoting::Window licensingLabel;
  licensingLabel.setWindow(GetDlgItem(m_hwnd, IDC_STATIC_LICENSING));
  licensingLabel.setText(StringTable::getString(IDS_LICENSING_INFO));

  return FALSE;
}

BOOL AboutDialog::onNotify(UINT controlID, LPARAM data)
{
  return FALSE;
}

BOOL AboutDialog::onCommand(UINT controlID, UINT notificationID)
{
  switch (controlID) {
  case IDCANCEL:
    onCloseButtonClick();
    break;
  case IDC_ORDER_SUPPORT_BUTTON:
    onOrderSupportButtonClock();
    break;
  case IDC_VISIT_WEB_SITE_BUTTON:
    onVisitSiteButtonClick();
    break;
  }
  return FALSE;
}

BOOL AboutDialog::onDestroy()
{
  return FALSE;
}
