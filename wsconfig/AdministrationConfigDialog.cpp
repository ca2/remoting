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
#include "remoting_node/resource.h"
#include "AdministrationConfigDialog.h"
#include "CommonInputValidation.h"
#include "UIDataAccess.h"
#include "ConfigDialog.h"
#include "file_lib/::file::item.h"
#include "remoting/remoting_common/server_config/ServerConfig.h"
#include "remoting/remoting_common/server_config/Configurator.h"
#include "remoting/remoting_common/util/CommonHeader.h"
#include "remoting/remoting_common/util/StringParser.h"
#include "remoting/remoting_common/wsconfig/ChangePasswordDialog.h"
#include "remoting/remoting_common/util/StringTable.h"
#include "remoting/remoting_common/win_system/Process.h"
#include "remoting_node_desktop/NamingDefs.h"

AdministrationConfigDialog::AdministrationConfigDialog()
: BaseDialog(IDD_CONFIG_ADMINISTRATION_PAGE), m_parentDialog(NULL)
{
}

AdministrationConfigDialog::~AdministrationConfigDialog()
{
}

void AdministrationConfigDialog::setParentDialog(BaseDialog *dialog)
{
  m_parentDialog = dialog;
}

BOOL AdministrationConfigDialog::onInitDialog()
{
  m_config = Configurator::getInstance()->getServerConfig();

  initControls();
  updateUI();

  return TRUE;
}

BOOL AdministrationConfigDialog::onCommand(UINT controlID, UINT notificationID)
{
  if (notificationID == BN_CLICKED) {
    if (controlID == IDC_SHARED_RADIO1) {
      onShareRadioButtonClick(0);
    } else if (controlID == IDC_SHARED_RADIO2) {
      onShareRadioButtonClick(1);
    } else if (controlID == IDC_SHARED_RADIO3) {
      onShareRadioButtonClick(2);
    } else if (controlID == IDC_SHARED_RADIO4) {
      onShareRadioButtonClick(3);
    } else if (controlID == IDC_SHARED_RADIO5) {
      onShareRadioButtonClick(4);
    } else if (controlID == IDC_OPEN_LOG_FOLDER_BUTTON) {
      onOpenFolderButtonClick();
    } else if (controlID == IDC_DO_NOTHING) {
      onDARadioButtonClick(0);
    } else if (controlID == IDC_LOCK_WORKSTATION) {
      onDARadioButtonClick(1);
    } else if (controlID == IDC_LOGOFF_WORKSTATION) {
      onDARadioButtonClick(2);
    } else if (controlID == IDC_LOG_FOR_ALL_USERS) {
      onLogForAllUsersClick();
    } else if (controlID == IDC_USE_CONTROL_AUTH_CHECKBOX) {
      onUseControlAuthClick();
    } else if (controlID == IDC_REPEAT_CONTROL_AUTH_CHECKBOX) {
      onRepeatControlAuthClick();
    } else if (controlID == IDC_CONTROL_PASSWORD_BUTTON) {
      onChangeControlPasswordClick();
    } else if (controlID == IDC_UNSET_CONTROL_PASWORD_BUTTON) {
      onUnsetControlPasswordClick();
    }

  } else if (notificationID == EN_UPDATE) {
    if (controlID == IDC_LOG_LEVEL) {
      onLogLevelUpdate();
    }
  }
  return TRUE;
}

bool AdministrationConfigDialog::validateInput()
{
  if (!CommonInputValidation::validateUINT(
    &m_logLevel,
    StringTable::getString(IDS_INVALID_LOG_LEVEL))) {
    return false;
  }

  unsigned int logLevel;

  UIDataAccess::queryValueAsUInt(&m_logLevel, &logLevel);

  if (logLevel > 10) {
    CommonInputValidation::notifyValidationError(
      &m_logLevel,
      StringTable::getString(IDS_INVALID_LOG_LEVEL));
    return false;
  }

  bool passwordSpecified = m_cpControl->getState() == PasswordControl::OldPassword ||
                           m_cpControl->getState() == PasswordControl::NewPassword;

  // FIXME: Code duplicate (see ServerConfigDialog class).
  if (!passwordSpecified && m_useControlAuth.isChecked()) {
    ::remoting::message_box(m_ctrlThis.get_hwnd(),
               StringTable::getString(IDS_SET_CONTROL_PASSWORD_NOTIFICATION),
               StringTable::getString(IDS_CAPTION_BAD_INPUT), MB_ICONSTOP | MB_OK);
    return false;
  }

  return true;
}

void AdministrationConfigDialog::updateUI()
{
  m_logLevel.setSignedInt(m_config->getLogLevel());

  m_useControlAuth.check(m_config->isControlAuthEnabled());
  m_repeatControlAuth.check(m_config->getControlAuthAlwaysChecking());
  m_repeatControlAuth.enable_window(m_useControlAuth.isChecked());

  ConfigDialog *configDialog = (ConfigDialog *)m_parentDialog;

  ::string logPath;

  m_config->getLogFileDir(&logPath);

  if (logPath.is_empty()) {
    logPath= StringTable::getString(IDS_LOGPATH_UNAVALIABLE);
    m_openLogPathButton.enable_window(false);
    m_logPathTB.enable_window(false);
  }

  m_logPathTB.setText(logPath);

  ::string folder;
  getFolderName(logPath, &folder);

  ::file::item folderFile(folder);

  if (folderFile.canRead()) {
    m_openLogPathButton.enable_window(true);
  } else {
    m_openLogPathButton.enable_window(false);
  }

  for (int i = 0; i < 5; i++) {
    m_shared[0].check(false);
  }

  if (m_config->isAlwaysShared() && !m_config->isNeverShared() && !m_config->isDisconnectingExistingClients()) {
    m_shared[0].check(true);
  }
  if (!m_config->isAlwaysShared() && m_config->isNeverShared() && !m_config->isDisconnectingExistingClients()) {
    m_shared[1].check(true);
  }
  if (!m_config->isAlwaysShared() && m_config->isNeverShared() && m_config->isDisconnectingExistingClients()) {
    m_shared[2].check(true);
  }
  if (!m_config->isAlwaysShared() && !m_config->isNeverShared() && !m_config->isDisconnectingExistingClients()) {
    m_shared[3].check(true);
  }
  if (!m_config->isAlwaysShared() && !m_config->isNeverShared() && m_config->isDisconnectingExistingClients()) {
    m_shared[4].check(true);
  }

  //
  // When last client disconnects
  //

  for (int i = 0; i < 3; i++) {
    m_disconnectAction[i].check(false);
  }
  switch (m_config->getDisconnectAction()) {
  case ServerConfig::DA_DO_NOTHING:
    m_disconnectAction[0].check(true);
    break;
  case ServerConfig::DA_LOCK_WORKSTATION:
    m_disconnectAction[1].check(true);
    break;
  case ServerConfig::DA_LOGOUT_WORKSTATION:
    m_disconnectAction[2].check(true);
    break;
  }

  m_logForAllUsers.check(m_config->isSaveLogToAllUsersPathFlagEnabled());

  if (m_config->hasControlPassword()) {
    unsigned char cryptedPassword[8];

    m_config->getControlPassword(cryptedPassword);

    m_cpControl->setCryptedPassword((char *)cryptedPassword);
  }

  m_cpControl->enable_window(m_config->isControlAuthEnabled());
}

void AdministrationConfigDialog::apply()
{
  ::string logLevelStringStorage;
  m_logLevel.get_text(&logLevelStringStorage);

  int logLevel = 0;

  StringParser::parseInt(logLevelStringStorage, &logLevel);

  m_config->setLogLevel(logLevel);

  bool alwaysShared = false;
  bool neverShared = false;
  bool disconnectClients = false;

  if (m_shared[0].isChecked()) {
    alwaysShared = true;
    neverShared = false;
    disconnectClients = false;
  } else if (m_shared[1].isChecked()) {
    alwaysShared = false;
    neverShared = true;
    disconnectClients = false;
  } else if (m_shared[2].isChecked()) {
    alwaysShared = false;
    neverShared = true;
    disconnectClients = true;
  } else if (m_shared[3].isChecked()) {
    alwaysShared = false;
    neverShared = false;
    disconnectClients = false;
  } else if (m_shared[4].isChecked()) {
    alwaysShared = false;
    neverShared = false;
    disconnectClients = true;
  }

  m_config->useControlAuth(m_useControlAuth.isChecked());
  m_config->setControlAuthAlwaysChecking(m_repeatControlAuth.isChecked());

  if (m_cpControl->getState() == PasswordControl::NewPassword) {
    m_config->setControlPassword((const unsigned char *)m_cpControl->getCryptedPassword());
  } 
  if (m_cpControl->getState() == PasswordControl::ResetPassword) {
    m_config->deleteControlPassword();
  }

  if (m_disconnectAction[0].isChecked()) {
    m_config->setDisconnectAction(ServerConfig::DA_DO_NOTHING);
  } else if (m_disconnectAction[1].isChecked()) {
    m_config->setDisconnectAction(ServerConfig::DA_LOCK_WORKSTATION);
  } else if (m_disconnectAction[2].isChecked()) {
    m_config->setDisconnectAction(ServerConfig::DA_LOGOUT_WORKSTATION);
  }

  m_config->setAlwaysShared(alwaysShared);
  m_config->setNeverShared(neverShared);
  m_config->disconnectExistingClients(disconnectClients);
  m_config->saveLogToAllUsersPath(m_logForAllUsers.isChecked());
}

void AdministrationConfigDialog::initControls()
{
  HWND hwnd = m_ctrlThis.get_hwnd();
  m_logLevel.setWindow(GetDlgItem(hwnd, IDC_LOG_LEVEL));
  m_logPathTB.setWindow(GetDlgItem(hwnd, IDC_LOG_FILEPATH_EDIT));

  m_openLogPathButton.setWindow(GetDlgItem(hwnd, IDC_OPEN_LOG_FOLDER_BUTTON));
  m_setControlPasswordButton.setWindow(GetDlgItem(hwnd, IDC_CONTROL_PASSWORD_BUTTON));
  m_unsetControlPasswordButton.setWindow(GetDlgItem(hwnd, IDC_UNSET_CONTROL_PASWORD_BUTTON));
  m_useControlAuth.setWindow(GetDlgItem(hwnd, IDC_USE_CONTROL_AUTH_CHECKBOX));
  m_repeatControlAuth.setWindow(GetDlgItem(hwnd, IDC_REPEAT_CONTROL_AUTH_CHECKBOX));

  m_shared[0].setWindow(GetDlgItem(hwnd, IDC_SHARED_RADIO1));
  m_shared[1].setWindow(GetDlgItem(hwnd, IDC_SHARED_RADIO2));
  m_shared[2].setWindow(GetDlgItem(hwnd, IDC_SHARED_RADIO3));
  m_shared[3].setWindow(GetDlgItem(hwnd, IDC_SHARED_RADIO4));
  m_shared[4].setWindow(GetDlgItem(hwnd, IDC_SHARED_RADIO5));

  m_disconnectAction[0].setWindow(GetDlgItem(hwnd, IDC_DO_NOTHING));
  m_disconnectAction[1].setWindow(GetDlgItem(hwnd, IDC_LOCK_WORKSTATION));
  m_disconnectAction[2].setWindow(GetDlgItem(hwnd, IDC_LOGOFF_WORKSTATION));

  m_logForAllUsers.setWindow(GetDlgItem(hwnd, IDC_LOG_FOR_ALL_USERS));

  m_logSpin.setWindow(GetDlgItem(hwnd, IDC_LOG_LEVEL_SPIN));

  m_logSpin.setBuddy(&m_logLevel);
  m_logSpin.setRange(0, 10);
  m_logSpin.setAccel(0, 1);

  m_cpControl = new PasswordControl(&m_setControlPasswordButton, &m_unsetControlPasswordButton);
}

void AdministrationConfigDialog::onShareRadioButtonClick(int number)
{
  if (!m_shared[number].isChecked()) {
    m_shared[number].check(true);
    for (int i = 0; i < 5; i++) {
      if (i != number) {
        m_shared[i].check(false);
      } // if
    } // for
    ((ConfigDialog *)m_parentDialog)->updateApplyButtonState();
  } // if
}

void AdministrationConfigDialog::onOpenFolderButtonClick()
{
  ::string logDir;

  m_config->getLogFileDir(&logDir);

  ::string command;

  command.formatf("explorer /select,{}\\{}.log", logDir,
                 LogNames::SERVER_LOG_FILE_STUB_NAME);

  Process explorer(command);

  try {
    explorer.start();
  } catch (...) {
    // TODO: Place error notification here.
  }
}

void AdministrationConfigDialog::onDARadioButtonClick(int number)
{
  if (!m_disconnectAction[number].isChecked()) {
    m_disconnectAction[number].check(true);
    for (int i = 0; i < 3; i++) {
      if (i != number) {
        m_disconnectAction[i].check(false);
      } // if
    } // for
    ((ConfigDialog *)m_parentDialog)->updateApplyButtonState();
  } // if
}

void AdministrationConfigDialog::onLogForAllUsersClick()
{
  ((ConfigDialog *)m_parentDialog)->updateApplyButtonState();
}

void AdministrationConfigDialog::onUseControlAuthClick()
{
  m_cpControl->enable_window(m_useControlAuth.isChecked());
  m_repeatControlAuth.enable_window(m_useControlAuth.isChecked());

  ((ConfigDialog *)m_parentDialog)->updateApplyButtonState();
}

void AdministrationConfigDialog::onRepeatControlAuthClick()
{
  ((ConfigDialog *)m_parentDialog)->updateApplyButtonState();
}

void AdministrationConfigDialog::onChangeControlPasswordClick()
{
  if (m_cpControl->showChangePasswordModalDialog(&m_ctrlThis)) {
    ((ConfigDialog *)m_parentDialog)->updateApplyButtonState();
  }
}

void AdministrationConfigDialog::onUnsetControlPasswordClick()
{
  m_cpControl->unsetPassword(true, m_ctrlThis.get_hwnd());

  ((ConfigDialog *)m_parentDialog)->updateApplyButtonState();
}

void AdministrationConfigDialog::onLogLevelUpdate()
{
  ((ConfigDialog *)m_parentDialog)->updateApplyButtonState();
}

//
// FIXME: Dublicating code (see RegistrySettingsManager::getFolderName method)
//

void AdministrationConfigDialog::getFolderName(const ::scoped_string & scopedstrKey, ::string & folder)
{
  ::array_base<TCHAR> folderString(_tcslen(key) + 1);
  memcpy(&folderString.front(), key, folderString.size() * sizeof(TCHAR));
  TCHAR *token = _tcsrchr(&folderString.front(), _T('\\'));
  if (token != NULL) {
    *token = _T('\0');
    folder-= &folderString.front();
  } else {
    folder-= "";
  }
}
