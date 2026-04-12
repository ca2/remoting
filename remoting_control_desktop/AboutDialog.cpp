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

#include "remoting_node/resource.h"

#include "remoting/remoting_common/win_system/Shell.h"
#include "remoting/remoting_common/win_system/VersionInfo.h"
//#include "remoting/remoting_common/win_system/Environment.h"

#include "innate_subsystem/::subsystem_apex::Control.h"

#include "remoting_node/BuildTime.h"

AboutDialog::AboutDialog()
: BaseDialog(IDD_ABOUT_DIALOG)
{
}

AboutDialog::~AboutDialog()
{
}

void AboutDialog::onCloseButtonClick()
{
  kill(::subsystem_apex::IDCANCEL);
}

void AboutDialog::onOrderSupportButtonClock()
{
  openUrl(main_subsystem()->string_table()->getString(IDS_URL_LICENSING_FSA));
}

void AboutDialog::onVisitSiteButtonClick()
{
  openUrl(main_subsystem()->string_table()->getString(IDS_URL_PRODUCT_FSA));
}

void AboutDialog::openUrl(const ::scoped_string & scopedstrUrl)
{
  try {
    Shell::open(url, 0, 0);
  } catch (SystemException &sysEx) {
    ::string scopedstrMessage;

    scopedstrMessage.format(main_subsystem()->string_table()->getString(IDS_FAILED_TO_OPEN_URL_FORMAT), sysEx.get_message());

    main_subsystem()->message_box(m_ctrlThis.operating_system_window(),
      scopedstrMessage,
      main_subsystem()->string_table()->getString(IDS_MBC_TVNCONTROL),
      ::user::e_message_box_ok|::user::e_message_box_icon_exclamation);
  }
}

bool AboutDialog::onInitDialog()
{
  // Update product version string.
  ::string versionString("unknown");
  try {
    ::string binaryPath;
    Environment::getCurrentModulePath(&binaryPath);
    VersionInfo productInfo(binaryPath);
    versionString= productInfo.getProductVersionString();
  } catch (SystemException &ex) {
    main_subsystem()->message_box(m_ctrlThis.operating_system_window(),
               ex.get_message(),
               main_subsystem()->string_table()->getString(IDS_MBC_TVNCONTROL),
               ::user::e_message_box_ok | ::user::e_message_box_icon_exclamation);
  }

  // Format product version and build time for displaying on the dialog.
  ::string versionText;
  versionText.format(main_subsystem()->string_table()->getString(IDS_PRODUCT_VERSION_FORMAT),
                     versionString,
                     BuildTime::DATE);

  // Show version info on the dialog.
  ::subsystem_apex::Control versionLabel;
  versionLabel.setWindow(GetDlgItem(m_ctrlThis.operating_system_window(), IDC_STATIC_VERSION));
  versionLabel.setText(versionText);

  // Show licensing info and/or special build info.
  ::subsystem_apex::Control licensingLabel;
  licensingLabel.setWindow(GetDlgItem(m_ctrlThis.operating_system_window(), IDC_STATIC_LICENSING));
  licensingLabel.setText(main_subsystem()->string_table()->getString(IDS_LICENSING_INFO));

  return false;
}

bool AboutDialog::onNotify(unsigned int controlID, ::lparam data)
{
  return false;
}

bool AboutDialog::onCommand(unsigned int controlID, unsigned int notificationID)
{
  switch (controlID) {
  case ::subsystem_apex::IDCANCEL:
    onCloseButtonClick();
    break;
  case IDC_ORDER_SUPPORT_BUTTON:
    onOrderSupportButtonClock();
    break;
  case IDC_VISIT_WEB_SITE_BUTTON:
    onVisitSiteButtonClick();
    break;
  }
  return false;
}

bool AboutDialog::onDestroy()
{
  return false;
}
