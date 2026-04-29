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
//#include "subsystem/platform/winhdr.h"
//#include "acme/_operating_system.h"

#include "remoting/remoting/node/NamingDefs.h"

#include "subsystem/platform/Registry.h"

#include "OutgoingConnectionDialog.h"

#include "remoting/node_desktop/resource.h"

namespace remoting_control_desktop
{
   OutgoingConnectionDialog::OutgoingConnectionDialog()
   //:
   // m_connHistoryKey(MainSubsystem().Registry().getCurrentUserKey(),
   //                    ::remoting_node::RegistryPaths::SERVER_REVERSE_CONN_HISTORY_PATH,
   //                    true),
     //m_connHistory(&m_connHistoryKey, 16)
//   m_connHistory("app://remoting/node_desktop/recent_connections", 16)
   {
      initialize_dialog(IDD_OUTGOING_CONN);
      m_connHistory.initialize_connection_history("app://remoting/node_desktop/recent_connections", 16);
   }

   OutgoingConnectionDialog::~OutgoingConnectionDialog()
   {
   }

   ::string OutgoingConnectionDialog::getConnectString() const
   {
      return m_connectString;
   }

   bool OutgoingConnectionDialog::isViewOnly() const
   {
      return m_isViewOnly;
   }

   void OutgoingConnectionDialog::initControls()
   {
      //HWND window = m_ctrlThis.operating_system_window();

      dialog_item(m_connectStringCB, IDC_HOSTNAME_COMBO);
      dialog_item(m_viewOnlyCB, IDC_VIEW_ONLY_CHECKBOX);
   }

   bool OutgoingConnectionDialog::onInitDialog()
   {
      initControls();

      m_viewOnlyCB.setChecked(false);

      // Load connection history.

      m_connHistory.load();

      for (size_t i = 0; i < m_connHistory.getHostCount(); i++) {
         m_connectStringCB.addItem(m_connHistory.getHost(i));
      }

      m_connectStringCB.setSelectedItem(0);
      m_connectStringCB.setFocus();

      return false;
   }

   bool OutgoingConnectionDialog::onNotify(unsigned int controlID, ::lparam data)
   {
      return false;
   }

   bool OutgoingConnectionDialog::onCommand(unsigned int controlID, unsigned int notificationID)
   {
      switch (controlID) {
         case ::innate_subsystem::e_control_id_ok:
            onOkButtonClick();
            break;
         case ::innate_subsystem::e_control_id_cancel:
            onCancelButtonClick();
            break;
      }
      return false;
   }

   bool OutgoingConnectionDialog::onDestroy()
   {
      return false;
   }

   void OutgoingConnectionDialog::onOkButtonClick()
   {

      m_connectString = m_connectStringCB.getText();

      m_isViewOnly = m_viewOnlyCB.isChecked();

      // Modify connection history.

      m_connHistory.addHost(m_connectString);
      m_connHistory.save();
      m_connHistory.truncate();

      closeDialog(::innate_subsystem::e_control_id_ok);
   }

   void OutgoingConnectionDialog::onCancelButtonClick()
   {
      closeDialog(::innate_subsystem::e_control_id_cancel);
   }
} // namespace remoting_control_desktop

