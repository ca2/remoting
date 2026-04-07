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
//#include "remoting/remoting_common/util/winhdr.h"
//#include "acme/_operating_system.h"

#include "remoting_node_desktop/NamingDefs.h"

#include "acme/acme/Registry.h"

#include "SetPasswordsDialog.h"

#include "remoting_node/resource.h"
//#include "remoting/remoting_common/util/::string.h"

SetPasswordsDialog::SetPasswordsDialog(bool initStateOfUseRfbAuth,
                                       bool initStateOfUseAdminAuth)
: BaseDialog(IDD_SET_PASSWORDS),
  m_dontChangeRfbAuth(!initStateOfUseRfbAuth),
  m_useRfbAuth(initStateOfUseRfbAuth),
  m_dontChangeAdmAuth(!initStateOfUseAdminAuth),
  m_protectControlInterface(initStateOfUseAdminAuth)
{
  m_passwordEmptyTooltip.setText(main_subsystem()->string_table()->getString(IDS_PASSWORD_IS_EMPTY));
  m_passwordEmptyTooltip.setTitle(main_subsystem()->string_table()->getString(IDS_MBC_TVNCONTROL));

  m_passwordsNotMatchTooltip.setText(main_subsystem()->string_table()->getString(IDS_PASSWORDS_NOT_MATCH));
  m_passwordsNotMatchTooltip.setTitle(main_subsystem()->string_table()->getString(IDS_MBC_TVNCONTROL));

  m_passwordWeakTooltip.setText(main_subsystem()->string_table()->getString(IDS_BAD_PASSWORD));
  m_passwordWeakTooltip.setTitle(main_subsystem()->string_table()->getString(IDS_MBC_BAD_PASSWORD));
}

SetPasswordsDialog::~SetPasswordsDialog()
{
}

void SetPasswordsDialog::getRfbPass(::string & pass)
{
  *pass = m_rfbPass;
}

bool SetPasswordsDialog::getUseRfbPass()
{
  return m_useRfbAuth;
}

bool SetPasswordsDialog::getRfbPassForClear()
{
  return !m_useRfbAuth && !m_dontChangeRfbAuth;
}

bool SetPasswordsDialog::getUseAdmPass()
{
  return m_protectControlInterface;
}

bool SetPasswordsDialog::getAdmPassForClear()
{
  return !m_protectControlInterface && !m_dontChangeAdmAuth;
}

void SetPasswordsDialog::getAdmPass(::string & pass)
{
  *pass = m_admPass;
}

void SetPasswordsDialog::initControls()
{
  HWND window = m_ctrlThis.operating_system_window();

  m_dontChangeRfbAuthSettingsRadio.setWindow(GetDlgItem(window,
    IDC_DONT_CHANGE_RFB_AUTH_SETTINGS_RADIO));
  m_dontUseRfbAuthRadio.setWindow(GetDlgItem(window, IDC_DONT_USE_RFB_AUTH_RADIO));
  m_useRfbAuthRadio.setWindow(GetDlgItem(window, IDC_USE_RFB_AUTH_RADIO));
  m_rfbPassEdit1.setWindow(GetDlgItem(window, IDC_ENTER_RFB_PASSWORD));
  m_rfbPassEdit2.setWindow(GetDlgItem(window, IDC_CONFIRM_RFB_PASSWORD));

  m_dontChangeAdminAuthSettingsRadio.setWindow(GetDlgItem(window,
    IDC_DONT_CHANGE_CONTROL_PROTECTION_SETTINGS_RADIO));
  m_dontUseAdminAuthRadio.setWindow(GetDlgItem(window, IDC_DONT_USE_CONTROL_PROTECTION_RADIO));
  m_useAdminAuthRadio.setWindow(GetDlgItem(window, IDC_PROTECT_CONTROL_INTERFACE_RADIO));
  m_admPassEdit1.setWindow(GetDlgItem(window, IDC_ENTER_ADMIN_PASSWORD));
  m_admPassEdit2.setWindow(GetDlgItem(window, IDC_CONFIRM_ADMIN_PASSWORD));

  m_rfbPassEdit1.setTextLengthLimit(8);
  m_rfbPassEdit2.setTextLengthLimit(8);
  m_admPassEdit1.setTextLengthLimit(8);
  m_admPassEdit2.setTextLengthLimit(8);
}

bool SetPasswordsDialog::onInitDialog()
{
  initControls();

  if (m_useRfbAuth) {
    m_useRfbAuthRadio.check(true);
  } else if (m_dontChangeRfbAuth) {
    m_dontChangeRfbAuthSettingsRadio.check(true);
  }
  if (m_protectControlInterface) {
    m_useAdminAuthRadio.check(true);
  } else if (m_dontChangeAdmAuth) {
    m_dontChangeAdminAuthSettingsRadio.check(true);
  }

  updateEditControls();

  return false;
}

bool SetPasswordsDialog::onNotify(unsigned int controlID, ::lparam data)
{
  return false;
}

bool SetPasswordsDialog::onCommand(unsigned int controlID, unsigned int notificationID)
{
  if (notificationID == ::user::e_notification_button_clicked) {
    readRadio();
    updateEditControls();
  }
  if (controlID == ::innate_subsystem::IDOK) {
    onOkButtonClick();
  }
  return false;
}

bool SetPasswordsDialog::onDestroy()
{
  return false;
}

void SetPasswordsDialog::onOkButtonClick()
{
  ::string rfbPass1;
  ::string rfbPass2;
  ::string admPass1;
  ::string admPass2;

  m_rfbPassEdit1.getText(&rfbPass1);
  m_rfbPassEdit2.getText(&rfbPass2);
  m_admPassEdit1.getText(&admPass1);
  m_admPassEdit2.getText(&admPass2);

  if (m_useRfbAuth) {
    if (rfbPass1.is_empty()) {
      m_rfbPassEdit1.showBalloonTip(&m_passwordEmptyTooltip);
      m_rfbPassEdit1.setFocus();
      return;
    }
    if (!rfbPass1.isEqualTo(&rfbPass2)) {
      m_rfbPassEdit2.showBalloonTip(&m_passwordsNotMatchTooltip);
      m_rfbPassEdit2.setFocus();
      return;
    }
    // shows scopedstrMessage box if the password can't be converted to ANSI with no data lost
    if (!::string::checkAnsiConversion(rfbPass1)) {
      m_rfbPassEdit1.showBalloonTip(&m_passwordWeakTooltip);
      m_rfbPassEdit1.setFocus();
      return;
    }
    m_rfbPass= rfbPass1;
  }
  if (m_protectControlInterface) {
    if (admPass1.is_empty()) {
      m_admPassEdit1.showBalloonTip(&m_passwordEmptyTooltip);
      m_admPassEdit1.setFocus();
      return;
    }
    if (!admPass1.isEqualTo(&admPass2)) {
      m_admPassEdit2.showBalloonTip(&m_passwordsNotMatchTooltip);
      m_admPassEdit2.setFocus();
      return;
    }
    if (!::string::checkAnsiConversion(admPass1)) {
      m_admPassEdit1.showBalloonTip(&m_passwordWeakTooltip);
      m_admPassEdit1.setFocus();
      return;
    }

    m_admPass= admPass1;
  }
  kill(::innate_subsystem::IDOK);
}

void SetPasswordsDialog::readRadio()
{
  m_dontChangeRfbAuth = m_dontChangeRfbAuthSettingsRadio.isChecked();
  m_useRfbAuth = m_useRfbAuthRadio.isChecked();
  m_dontChangeAdmAuth = m_dontChangeAdminAuthSettingsRadio.isChecked();
  m_protectControlInterface = m_useAdminAuthRadio.isChecked();
}

void SetPasswordsDialog::updateEditControls()
{
  m_rfbPassEdit1.enable_window(m_useRfbAuth);
  m_rfbPassEdit2.enable_window(m_useRfbAuth);
  m_admPassEdit1.enable_window(m_protectControlInterface);
  m_admPassEdit2.enable_window(m_protectControlInterface);
}

bool SetPasswordsDialog::onClose()
{
  kill(::innate_subsystem::IDCANCEL);
  return false;
}
