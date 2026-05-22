// Copyright (C) 2009,2010,2011,2012 GlavSoft LLC.
// All rights reserved.
//
//-------------------------------------------------------------------------
// This file is part of the T i g h t V N C software.  Please visit our Web site:
//
//                       http://www.t i g h t v n c.com/
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
#include "ChangePasswordDialog.h"
#include "remoting/node_desktop/resource.h"
#include "remoting/remoting_rtc/platform/remoting.h"
#include "remoting/remoting_rtc/node_config/ServerConfig.h"
#include "subsystem/platform/VncPassCrypt.h"
#include "acme/constant/user_notification.h"
//#include "subsystem/platform/::string.h"
namespace remoting_rtc_node
{
   ChangePasswordDialog::ChangePasswordDialog(::innate_subsystem::WindowInterface *pwindowParent, bool isNewPassword)
   : m_allowEmptyPassword(false), m_newPassword(isNewPassword)
   {
       initialize_dialog(IDD_CHANGE_PASSWORD);
      setParent(pwindowParent);

      m_passwordEmptyTooltip.setText(MainSubsystem().StringTable().getString(IDS_PASSWORD_IS_EMPTY));
      m_passwordEmptyTooltip.setTitle(MainSubsystem().StringTable().getString(IDS_MBC_TVNCONTROL));

      m_passwordsNotMatchTooltip.setText(MainSubsystem().StringTable().getString(IDS_PASSWORDS_NOT_MATCH));
      m_passwordsNotMatchTooltip.setTitle(MainSubsystem().StringTable().getString(IDS_MBC_TVNCONTROL));

      m_passwordWeakTooltip.setText(MainSubsystem().StringTable().getString(IDS_BAD_PASSWORD));
      m_passwordWeakTooltip.setTitle(MainSubsystem().StringTable().getString(IDS_MBC_BAD_PASSWORD));
   }

   ChangePasswordDialog::~ChangePasswordDialog()
   {
   }

   ::string ChangePasswordDialog::getPasswordInPlainText() const
   {
      return m_passwordText;
   }

   bool ChangePasswordDialog::onInitDialog()
   {
      initControls();
      if (m_newPassword) {
         setText(MainSubsystem().StringTable().getString(IDS_NEW_PASSWORD));
      } else {
         setText(MainSubsystem().StringTable().getString(IDS_CHANGE_PASSWORD));
      }
      return true;
   }

   bool ChangePasswordDialog::onCommand(::u32 controlID, ::u32 notificationID)
   {
      if (notificationID == ::user::e_notification_button_clicked) {
         switch (controlID) {
            case ::innate_subsystem::e_control_id_ok:
               onOkButtonClick();
               break;
            case ::innate_subsystem::e_control_id_cancel:
               onCancelButtonClick();
               break;
         }
      }
      return true;
   }

   void ChangePasswordDialog::onOkButtonClick()
   {
      ::string password1;
      ::string password2;
      password1 = m_password1.getText();
      password2 = m_password2.getText();

      if (password1.is_empty() && !m_allowEmptyPassword) {
         m_password1.showBalloonTip(&m_passwordEmptyTooltip);
         m_password1.setFocus();
         return ;
      }
      if (password1 != password2) {
         m_password2.showBalloonTip(&m_passwordsNotMatchTooltip);
         m_password2.setFocus();
         return ;
      }

      if (!::str::checkAnsiConversion(password1)) {
         m_password1.showBalloonTip(&m_passwordWeakTooltip);
         m_password1.setFocus();
         return;
      }

      m_passwordText= password1;

      closeDialog(::innate_subsystem::e_control_id_ok);
   }

   void ChangePasswordDialog::onCancelButtonClick()
   {
      closeDialog(::innate_subsystem::e_control_id_ok);
   }

   void ChangePasswordDialog::initControls()
   {
      //HWND hwnd = operating_system_window();
      dialog_item(m_password1, IDC_PASSWORD);
      dialog_item(m_password2, IDC_PASSWORD2);

      m_password1.setTextLengthLimit(::subsystem::VncPassCrypt::VNC_PASSWORD_SIZE);
      m_password2.setTextLengthLimit(::subsystem::VncPassCrypt::VNC_PASSWORD_SIZE);
   }
} // namespace remoting_rtc_node



