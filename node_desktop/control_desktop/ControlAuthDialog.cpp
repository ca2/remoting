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
#include "ControlAuthDialog.h"

#include "remoting/node_desktop/resource.h"
#include "remoting/remoting/node_config/ServerConfig.h"
#include "subsystem/platform/VncPassCrypt.h"


namespace remoting_control_desktop
{
   ControlAuthDialog::ControlAuthDialog()
   //: BaseDialog(IDD_CONTROL_AUTH)
   {

      initialize_dialog(IDD_CONTROL_AUTH);
   }

   ControlAuthDialog::~ControlAuthDialog()
   {
   }

   ::string ControlAuthDialog::getPassword() const
   {
      return m_password;
   }

   bool ControlAuthDialog::onInitDialog()
   {
      m_password= "";

      //HWND hwnd = operating_system_window();
      dialog_item(m_passwordTextBox,IDC_PASSWORD_EDIT);
      m_passwordTextBox.setTextLengthLimit(::subsystem::VncPassCrypt::VNC_PASSWORD_SIZE);

      //SetForegroundWindow(hwnd);
      setForegroundWindow();
      m_passwordTextBox.setFocus();

      return true;
   }

   bool ControlAuthDialog::onNotify(::u32 controlID, ::lparam data)
   {
      return true;
   }

   bool ControlAuthDialog::onCommand(::u32 controlID, ::u32 notificationID)
   {
      switch (controlID) {
         case ::innate_subsystem::e_control_id_ok:
            m_password = m_passwordTextBox.getText();
            closeDialog(controlID);
            break;
         case ::innate_subsystem::e_control_id_cancel:
            closeDialog(controlID);
            break;
      }
      return true;
   }

   bool ControlAuthDialog::onDestroy()
   {
      return true;
   }
} // namespace remoting_control_desktop

