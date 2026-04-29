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
#include "PasswordControl.h"

#include "subsystem/platform/VncPassCrypt.h"
#include "subsystem/platform/StringTable.h"
//#include "subsystem/platform/::string.h"

#include "ChangePasswordDialog.h"
#include "subsystem/platform/VncPassCrypt.h"
#include "remoting/node_desktop/resource.h"



namespace remoting_node
{
   PasswordControl::PasswordControl(::innate_subsystem::Control *changeButton, ::innate_subsystem::Control *unsetButton)
   : m_enabled(true),
     m_changeButton(changeButton),
     m_unsetButton(unsetButton),
     m_state(NoPassword)
   {
      updateControlsState();
   }

   PasswordControl::~PasswordControl()
   {
      releaseCryptedPassword();
   }


   void PasswordControl::enableWindow(bool enabled)
   {
      m_enabled = enabled;

      updateControlsState();
   }

   void PasswordControl::unsetPassword(bool promtUser, const ::operating_system::window & operatingsystemwindowParent)
   {
      if (promtUser) {
         if (MainSubsystem().message_box(operatingsystemwindowParent,
           MainSubsystem().StringTable().getString(IDS_UNSET_PASSWORD_PROMT),
           MainSubsystem().StringTable().getString(IDS_MBC_TVNCONTROL),
           ::user::e_message_box_yes_no | ::user::e_message_box_icon_question) ==
           ::e_dialog_result_no) {
            return;
           }
      }

      releaseCryptedPassword();
      m_state = ResetPassword;
      updateControlsState();
   }

   void PasswordControl::setPassword(const ::scoped_string & scopedstrPlainText)
   {
      char plainTextInANSI[9];
      memset(plainTextInANSI, 0, sizeof(plainTextInANSI));
      ::string plainTextInUTF16(scopedstrPlainText);
      ::string ansiPlainTextStorage(&plainTextInUTF16);
      memcpy(plainTextInANSI, ansiPlainTextStorage,
             minimum(ansiPlainTextStorage.length(), sizeof(plainTextInANSI)));

      unsigned char cryptedPassword[8];
      memset(cryptedPassword, 0, 8);

      ::subsystem::VncPassCrypt::getEncryptedPass(cryptedPassword, (const unsigned char *)plainTextInANSI);

      setCryptedPassword((char *)cryptedPassword);

      updateControlsState();
   }

   void PasswordControl::setCryptedPassword(const char *cryptedPass)
   {
      m_cryptedPassword.resize(8);
      memcpy(m_cryptedPassword.data(), cryptedPass, m_cryptedPassword.size());
      m_state = NewPassword;
   }

   const char *PasswordControl::getCryptedPassword() const
   {
      if (m_cryptedPassword.empty()) {
         return 0;
      }

      return m_cryptedPassword.data();
   }

   bool PasswordControl::showChangePasswordModalDialog(::innate_subsystem::Control *parent)
   {
      ChangePasswordDialog changePasswordDialog(parent, m_state != NewPassword && m_state != ResetPassword);

      if (changePasswordDialog.showModal() != ::innate_subsystem::e_control_id_ok) {
         return false;
      }

      setPassword(changePasswordDialog.getPasswordInPlainText());

      return true;
   }

   void PasswordControl::updateControlsState()
   {
      if (m_changeButton != 0) {
         if (m_state == OldPassword || m_state == NewPassword) {
            m_changeButton->setText(MainSubsystem().StringTable().getString(IDS_CHANGE_PASSWORD_CAPTION));
         } else {
            m_changeButton->setText(MainSubsystem().StringTable().getString(IDS_SET_PASSWORD_CAPTION));
         }
         m_changeButton->enableWindow(m_enabled);
      }
      if (m_unsetButton != 0) {
         m_unsetButton->enableWindow(m_enabled && (m_state == OldPassword || m_state == NewPassword));
      }
   }

   void PasswordControl::releaseCryptedPassword()
   {
      m_cryptedPassword.resize(0);
   }
} // namespace remoting_node



