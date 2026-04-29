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
#include "SetPasswordsDialog.h"
//#include "subsystem/platform/winhdr.h"
//#include "acme/_operating_system.h"

//#include "remoting/node_desktop/NamingDefs.h"

#include "subsystem/platform/Registry.h"

#include "remoting/remoting/platform/remoting.h"

//#include "port/base/archive/archive/libarchive/archive_windows.h"
#include "remoting/node_desktop/resource.h"
//#include "subsystem/platform/::string.h"
#include "acme/constant/user_notification.h"


namespace remoting_control_desktop
{


   SetPasswordsDialog::SetPasswordsDialog(bool initStateOfUseRfbAuth,
                                          bool initStateOfUseAdminAuth)
   :
     m_dontChangeRfbAuth(!initStateOfUseRfbAuth),
     m_useRfbAuth(initStateOfUseRfbAuth),
     m_dontChangeAdmAuth(!initStateOfUseAdminAuth),
     m_protectControlInterface(initStateOfUseAdminAuth)
   {
      initialize_dialog(IDD_SET_PASSWORDS),
      m_passwordEmptyTooltip.setText(MainSubsystem().StringTable().getString(IDS_PASSWORD_IS_EMPTY));
      m_passwordEmptyTooltip.setTitle(MainSubsystem().StringTable().getString(IDS_MBC_TVNCONTROL));

      m_passwordsNotMatchTooltip.setText(MainSubsystem().StringTable().getString(IDS_PASSWORDS_NOT_MATCH));
      m_passwordsNotMatchTooltip.setTitle(MainSubsystem().StringTable().getString(IDS_MBC_TVNCONTROL));

      m_passwordWeakTooltip.setText(MainSubsystem().StringTable().getString(IDS_BAD_PASSWORD));
      m_passwordWeakTooltip.setTitle(MainSubsystem().StringTable().getString(IDS_MBC_BAD_PASSWORD));
   }

   SetPasswordsDialog::~SetPasswordsDialog()
   {
   }

   void SetPasswordsDialog::getRfbPass(::string & pass)
   {
      pass = m_rfbPass;
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
      pass = m_admPass;
   }

   void SetPasswordsDialog::initControls()
   {
      //HWND window = m_ctrlThis.operating_system_window();

      dialog_item(m_dontChangeRfbAuthSettingsRadio, IDC_DONT_CHANGE_RFB_AUTH_SETTINGS_RADIO);
      dialog_item(m_dontUseRfbAuthRadio, IDC_DONT_USE_RFB_AUTH_RADIO);
      dialog_item(m_useRfbAuthRadio, IDC_USE_RFB_AUTH_RADIO);
      dialog_item(m_rfbPassEdit1, IDC_ENTER_RFB_PASSWORD);
      dialog_item(m_rfbPassEdit2, IDC_CONFIRM_RFB_PASSWORD);

      dialog_item(m_dontChangeAdminAuthSettingsRadio, IDC_DONT_CHANGE_CONTROL_PROTECTION_SETTINGS_RADIO);
      dialog_item(m_dontUseAdminAuthRadio, IDC_DONT_USE_CONTROL_PROTECTION_RADIO);
      dialog_item(m_useAdminAuthRadio, IDC_PROTECT_CONTROL_INTERFACE_RADIO);
      dialog_item(m_admPassEdit1, IDC_ENTER_ADMIN_PASSWORD);
      dialog_item(m_admPassEdit2, IDC_CONFIRM_ADMIN_PASSWORD);

      m_rfbPassEdit1.setTextLengthLimit(8);
      m_rfbPassEdit2.setTextLengthLimit(8);
      m_admPassEdit1.setTextLengthLimit(8);
      m_admPassEdit2.setTextLengthLimit(8);
   }

   bool SetPasswordsDialog::onInitDialog()
   {
      initControls();

      if (m_useRfbAuth) {
         m_useRfbAuthRadio.setChecked(true);
      } else if (m_dontChangeRfbAuth) {
         m_dontChangeRfbAuthSettingsRadio.setChecked(true);
      }
      if (m_protectControlInterface) {
         m_useAdminAuthRadio.setChecked(true);
      } else if (m_dontChangeAdmAuth) {
         m_dontChangeAdminAuthSettingsRadio.setChecked(true);
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
      if (controlID == ::innate_subsystem::e_control_id_ok) {
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

      rfbPass1 = m_rfbPassEdit1.getText();
      rfbPass2 = m_rfbPassEdit2.getText();
      admPass1 = m_admPassEdit1.getText();
      admPass2 = m_admPassEdit2.getText();

      if (m_useRfbAuth) {
         if (rfbPass1.is_empty()) {
            m_rfbPassEdit1.showBalloonTip(&m_passwordEmptyTooltip);
            m_rfbPassEdit1.setFocus();
            return;
         }
         if (rfbPass1 != rfbPass2) {
            m_rfbPassEdit2.showBalloonTip(&m_passwordsNotMatchTooltip);
            m_rfbPassEdit2.setFocus();
            return;
         }
         // shows scopedstrMessage box if the password can't be converted to ANSI with no data lost
         if (!::str::checkAnsiConversion(rfbPass1)) {
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
         if (admPass1 != admPass2) {
            m_admPassEdit2.showBalloonTip(&m_passwordsNotMatchTooltip);
            m_admPassEdit2.setFocus();
            return;
         }
         if (!::str::checkAnsiConversion(admPass1)) {
            m_admPassEdit1.showBalloonTip(&m_passwordWeakTooltip);
            m_admPassEdit1.setFocus();
            return;
         }

         m_admPass= admPass1;
      }
      closeDialog(::innate_subsystem::e_control_id_ok);
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
      m_rfbPassEdit1.enableWindow(m_useRfbAuth);
      m_rfbPassEdit2.enableWindow(m_useRfbAuth);
      m_admPassEdit1.enableWindow(m_protectControlInterface);
      m_admPassEdit2.enableWindow(m_protectControlInterface);
   }

   bool SetPasswordsDialog::onClose()
   {
      closeDialog(::innate_subsystem::e_control_id_cancel);
      return false;
   }
} // namespace remoting_control_desktop


