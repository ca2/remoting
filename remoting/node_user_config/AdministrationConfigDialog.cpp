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
#include "remoting/node_desktop/resource.h"
#include "AdministrationConfigDialog.h"
#include "CommonInputValidation.h"
#include "UIDataAccess.h"
#include "ConfigDialog.h"
#include "acme/constant/user_notification.h"
///#include "file_lib/::file::item.h"
#include "remoting/remoting/node_config/ServerConfig.h"
#include "remoting/remoting/node_config/Configurator.h"
#include "subsystem/_common_header.h"
#include "subsystem/platform/StringParser.h"
#include "remoting/remoting/node_user_config/ChangePasswordDialog.h"
#include "subsystem/platform/StringTable.h"
#include "subsystem/node/Process.h"
#include "remoting/remoting/node/NamingDefs.h"
#include "subsystem/node/OperatingSystem.h"


namespace remoting_node
{

   AdministrationConfigDialog::AdministrationConfigDialog(Configurator * pconfigurator)
   :  m_pdialogParent(NULL),
   m_pconfigurator(pconfigurator)
   {
      initialize_dialog(IDD_CONFIG_ADMINISTRATION_PAGE);
   }

   AdministrationConfigDialog::~AdministrationConfigDialog()
   {
   }

   void AdministrationConfigDialog::setParentDialog(::innate_subsystem::DialogInterface * pdialog)
   {
      m_pdialogParent = pdialog;
   }

   bool AdministrationConfigDialog::onInitDialog()
   {
      m_pserverconfig = m_pconfigurator->getServerConfig();

      initControls();
      updateUI();

      return true;
   }

   bool AdministrationConfigDialog::onCommand(unsigned int controlID, unsigned int notificationID)
   {
      if (notificationID == ::user::e_notification_button_clicked) {
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
      return true;
   }

   bool AdministrationConfigDialog::validateInput()
   {
      if (!CommonInputValidation::validateUINT(
        &m_logLevel,
        MainSubsystem().StringTable().getString(IDS_INVALID_LOG_LEVEL))) {
         return false;
        }

      unsigned int logLevel;

      UIDataAccess::queryValueAsUInt(&m_logLevel, &logLevel);

      if (logLevel > 10) {
         CommonInputValidation::notifyValidationError(
           &m_logLevel,
           MainSubsystem().StringTable().getString(IDS_INVALID_LOG_LEVEL));
         return false;
      }

      bool passwordSpecified = m_cpControl->getState() == PasswordControl::OldPassword ||
                               m_cpControl->getState() == PasswordControl::NewPassword;

      // FIXME: Code duplicate (see ::remoting_node::ServerConfigDialog class).
      if (!passwordSpecified && m_useControlAuth.isChecked()) {
         MainSubsystem().message_box(operating_system_window(),
                    MainSubsystem().StringTable().getString(IDS_SET_CONTROL_PASSWORD_NOTIFICATION),
                    MainSubsystem().StringTable().getString(IDS_CAPTION_BAD_INPUT), MB_ICONSTOP | ::user::e_message_box_ok);
         return false;
      }

      return true;
   }

   void AdministrationConfigDialog::updateUI()
   {
      m_logLevel.setSignedInt(m_pserverconfig->getLogLevel());

      m_useControlAuth.setChecked(m_pserverconfig->isControlAuthEnabled());
      m_repeatControlAuth.setChecked(m_pserverconfig->getControlAuthAlwaysChecking());
      m_repeatControlAuth.enableWindow(m_useControlAuth.isChecked());

      auto pconfigdialog = m_pdialogParent->get_callback<ConfigDialog>();

      ::string logPath;

      m_pserverconfig->getLogFileDir(logPath);

      if (logPath.is_empty()) {
         logPath= MainSubsystem().StringTable().getString(IDS_LOGPATH_UNAVALIABLE);
         m_openLogPathButton.enableWindow(false);
         m_logPathTB.enableWindow(false);
      }

      m_logPathTB.setText(logPath);

      auto folder = file_path_folder(logPath);

      //::string folder;
      //getFolderName(logPath, &folder);

      ::file::path pathFolder(folder);

      //::f
      //::file::item folderFile(folder);

      if (MainSubsystem().OperatingSystem().file_canRead(pathFolder)) {
         m_openLogPathButton.enableWindow(true);
      } else {
         m_openLogPathButton.enableWindow(false);
      }

      for (int i = 0; i < 5; i++) {
         m_shared[0].setChecked(false);
      }

      if (m_pserverconfig->isAlwaysShared() && !m_pserverconfig->isNeverShared() && !m_pserverconfig->isDisconnectingExistingClients()) {
         m_shared[0].setChecked(true);
      }
      if (!m_pserverconfig->isAlwaysShared() && m_pserverconfig->isNeverShared() && !m_pserverconfig->isDisconnectingExistingClients()) {
         m_shared[1].setChecked(true);
      }
      if (!m_pserverconfig->isAlwaysShared() && m_pserverconfig->isNeverShared() && m_pserverconfig->isDisconnectingExistingClients()) {
         m_shared[2].setChecked(true);
      }
      if (!m_pserverconfig->isAlwaysShared() && !m_pserverconfig->isNeverShared() && !m_pserverconfig->isDisconnectingExistingClients()) {
         m_shared[3].setChecked(true);
      }
      if (!m_pserverconfig->isAlwaysShared() && !m_pserverconfig->isNeverShared() && m_pserverconfig->isDisconnectingExistingClients()) {
         m_shared[4].setChecked(true);
      }

      //
      // When last client disconnects
      //

      for (int i = 0; i < 3; i++) {
         m_disconnectAction[i].setChecked(false);
      }
      switch (m_pserverconfig->getDisconnectAction()) {
         case ::remoting_node::ServerConfig::DA_DO_NOTHING:
            m_disconnectAction[0].setChecked(true);
            break;
         case ::remoting_node::ServerConfig::DA_LOCK_WORKSTATION:
            m_disconnectAction[1].setChecked(true);
            break;
         case ::remoting_node::ServerConfig::DA_LOGOUT_WORKSTATION:
            m_disconnectAction[2].setChecked(true);
            break;
      }

      m_logForAllUsers.setChecked(m_pserverconfig->isSaveLogToAllUsersPathFlagEnabled());

      if (m_pserverconfig->hasControlPassword()) {
         unsigned char cryptedPassword[8];

         m_pserverconfig->getControlPassword(cryptedPassword);

         m_cpControl->setCryptedPassword((char *)cryptedPassword);
      }

      m_cpControl->enableWindow(m_pserverconfig->isControlAuthEnabled());
   }

   void AdministrationConfigDialog::apply()
   {
      ::string logLevelStringStorage;
      logLevelStringStorage = m_logLevel.getText();

      int logLevel = 0;

      MainSubsystem().StringParser().parseInt(logLevelStringStorage, &logLevel);

      m_pserverconfig->setLogLevel(logLevel);

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

      m_pserverconfig->useControlAuth(m_useControlAuth.isChecked());
      m_pserverconfig->setControlAuthAlwaysChecking(m_repeatControlAuth.isChecked());

      if (m_cpControl->getState() == PasswordControl::NewPassword) {
         m_pserverconfig->setControlPassword((const unsigned char *)m_cpControl->getCryptedPassword());
      }
      if (m_cpControl->getState() == PasswordControl::ResetPassword) {
         m_pserverconfig->deleteControlPassword();
      }

      if (m_disconnectAction[0].isChecked()) {
         m_pserverconfig->setDisconnectAction(::remoting_node::ServerConfig::DA_DO_NOTHING);
      } else if (m_disconnectAction[1].isChecked()) {
         m_pserverconfig->setDisconnectAction(::remoting_node::ServerConfig::DA_LOCK_WORKSTATION);
      } else if (m_disconnectAction[2].isChecked()) {
         m_pserverconfig->setDisconnectAction(::remoting_node::ServerConfig::DA_LOGOUT_WORKSTATION);
      }

      m_pserverconfig->setAlwaysShared(alwaysShared);
      m_pserverconfig->setNeverShared(neverShared);
      m_pserverconfig->disconnectExistingClients(disconnectClients);
      m_pserverconfig->saveLogToAllUsersPath(m_logForAllUsers.isChecked());
   }

   void AdministrationConfigDialog::initControls()
   {
      //HWND hwnd = operating_system_window();
      dialog_item(m_logLevel, IDC_LOG_LEVEL);
      dialog_item(m_logPathTB, IDC_LOG_FILEPATH_EDIT);

      dialog_item(m_openLogPathButton, IDC_OPEN_LOG_FOLDER_BUTTON);
      dialog_item(m_setControlPasswordButton, IDC_CONTROL_PASSWORD_BUTTON);
      dialog_item(m_unsetControlPasswordButton, IDC_UNSET_CONTROL_PASWORD_BUTTON);
      dialog_item(m_useControlAuth, IDC_USE_CONTROL_AUTH_CHECKBOX);
      dialog_item(m_repeatControlAuth, IDC_REPEAT_CONTROL_AUTH_CHECKBOX);

      dialog_item(m_shared[0], IDC_SHARED_RADIO1);
      dialog_item(m_shared[1], IDC_SHARED_RADIO2);
      dialog_item(m_shared[2], IDC_SHARED_RADIO3);
      dialog_item(m_shared[3], IDC_SHARED_RADIO4);
      dialog_item(m_shared[4], IDC_SHARED_RADIO5);

      dialog_item(m_disconnectAction[0], IDC_DO_NOTHING);
      dialog_item(m_disconnectAction[1], IDC_LOCK_WORKSTATION);
      dialog_item(m_disconnectAction[2], IDC_LOGOFF_WORKSTATION);

      dialog_item(m_logForAllUsers, IDC_LOG_FOR_ALL_USERS);

      dialog_item(m_logSpin, IDC_LOG_LEVEL_SPIN);

      m_logSpin.setBuddy(&m_logLevel);
      m_logSpin.setRange(0, 10);
      m_logSpin.setAccel(0, 1);

      m_cpControl = new PasswordControl(&m_setControlPasswordButton, &m_unsetControlPasswordButton);
   }

   void AdministrationConfigDialog::onShareRadioButtonClick(int number)
   {
      if (!m_shared[number].isChecked()) {
         m_shared[number].setChecked(true);
         for (int i = 0; i < 5; i++) {
            if (i != number) {
               m_shared[i].setChecked(false);
            } // if
         } // for

         auto pconfigdialog = m_pdialogParent->get_callback< ConfigDialog>();
         pconfigdialog->updateApplyButtonState();
      } // if
   }

   void AdministrationConfigDialog::onOpenFolderButtonClick()
   {
      ::string logDir;

      m_pserverconfig->getLogFileDir(logDir);

      ::string command;

      command.format("explorer /select,{}\\{}.log", logDir,
                     LogNames::SERVER_LOG_FILE_STUB_NAME);

      ::subsystem::Process explorer;
      explorer.initialize_process(command);

      try {
         explorer.start();
      } catch (...) {
         // TODO: Place error notification here.
      }
   }

   void AdministrationConfigDialog::onDARadioButtonClick(int number)
   {
      if (!m_disconnectAction[number].isChecked()) {
         m_disconnectAction[number].setChecked(true);
         for (int i = 0; i < 3; i++) {
            if (i != number) {
               m_disconnectAction[i].setChecked(false);
            } // if
         } // for
         auto pconfigdialog = m_pdialogParent->get_callback< ConfigDialog>();
         pconfigdialog->updateApplyButtonState();
      } // if
   }

   void AdministrationConfigDialog::onLogForAllUsersClick()
   {
      auto pconfigdialog = m_pdialogParent->get_callback< ConfigDialog>();
      pconfigdialog->updateApplyButtonState();
   }

   void AdministrationConfigDialog::onUseControlAuthClick()
   {
      m_cpControl->enableWindow(m_useControlAuth.isChecked());
      m_repeatControlAuth.enableWindow(m_useControlAuth.isChecked());

      auto pconfigdialog = m_pdialogParent->get_callback< ConfigDialog>();
      pconfigdialog->updateApplyButtonState();
   }

   void AdministrationConfigDialog::onRepeatControlAuthClick()
   {
      auto pconfigdialog = m_pdialogParent->get_callback< ConfigDialog>();
      pconfigdialog->updateApplyButtonState();
   }

   void AdministrationConfigDialog::onChangeControlPasswordClick()
   {
      if (m_cpControl->showChangePasswordModalDialog(this)) {
         auto pconfigdialog = m_pdialogParent->get_callback< ConfigDialog>();
         pconfigdialog->updateApplyButtonState();
      }
   }

   void AdministrationConfigDialog::onUnsetControlPasswordClick()
   {
      m_cpControl->unsetPassword(true, operating_system_window());

      auto pconfigdialog = m_pdialogParent->get_callback< ConfigDialog>();
      pconfigdialog->updateApplyButtonState();
   }

   void AdministrationConfigDialog::onLogLevelUpdate()
   {
      auto pconfigdialog = m_pdialogParent->get_callback< ConfigDialog>();
      pconfigdialog->updateApplyButtonState();
   }

   // //
   // // FIXME: Dublicating code (see RegistrySettingsManager::getFolderName method)
   // //
   //
   // void AdministrationConfigDialog::getFolderName(const ::scoped_string & scopedstrKey, ::string & folder)
   // {
   //    ::array_base<TCHAR> folderString(_tcslen(key) + 1);
   //    memcpy(&folderString.front(), key, folderString.size() * sizeof(TCHAR));
   //    TCHAR *token = _tcsrchr(&folderString.front(), _T('\\'));
   //    if (token != NULL) {
   //       *token = _T('\0');
   //       folder-= &folderString.front();
   //    } else {
   //       folder-= "";
   //    }
   // }
} // namespace remoting_node



