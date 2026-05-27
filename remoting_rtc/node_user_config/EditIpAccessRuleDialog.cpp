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
#include "remoting/node_desktop/resource.h"
#include "EditIpAccessRuleDialog.h"
#include "subsystem/platform/StringParser.h"
#include "acme/constant/user_notification.h"
//#include "subsystem/platform/::string.h"
namespace remoting_rtc_node
{
   EditIpAccessRuleDialog::EditIpAccessRuleDialog()
   :
   //BaseDialog(IDD_EDIT_IP_ACESS_CONTROL),
   m_data(NULL), m_isOpenedForEdit(false)
   {

      initialize_dialog(IDD_EDIT_IP_ACESS_CONTROL);
      m_warningBalloonTip.setIconType(::innate_subsystem::e_tooltip_icon_warning);
      m_warningBalloonTip.setText(MainSubsystem().StringTable().getString(IDS_IP_ADDRESS_HINT));
      m_warningBalloonTip.setTitle(MainSubsystem().StringTable().getString(IDS_INVALID_IP_TITLE));

      m_lastIpLessThanFirstBT.setIconType(::innate_subsystem::e_tooltip_icon_warning);
      m_lastIpLessThanFirstBT.setText(MainSubsystem().StringTable().getString(IDS_LAST_IP_MUST_BE_MORE_THAN_FIRST));
      m_lastIpLessThanFirstBT.setTitle(MainSubsystem().StringTable().getString(IDS_CAPTION_BAD_INPUT));
   }

   EditIpAccessRuleDialog::~EditIpAccessRuleDialog()
   {
   }

   void EditIpAccessRuleDialog::setEditFlag(bool flagEnabled)
   {
      m_isOpenedForEdit = flagEnabled;
   }

   bool EditIpAccessRuleDialog::onInitDialog()
   {
      initControls();

      for (::i32 i = 0; i < 3; i++) {
         m_access[i].setChecked(false);
      }

      if (m_data == NULL) {
         return true;
      }

      switch (m_data->getAction()) {
         case IpAccessRule::ACTION_TYPE_ALLOW:
            m_access[0].setChecked(true);
            break;
         case IpAccessRule::ACTION_TYPE_DENY:
            m_access[1].setChecked(true);
            break;
         case IpAccessRule::ACTION_TYPE_QUERY:
            m_access[2].setChecked(true);
            break;
      }

      if (m_isOpenedForEdit) {
         ::string firstIp;
         ::string lastIp;

         m_data->getFirstIp(firstIp);
         m_data->getLastIp(lastIp);

         m_firstIp.setText(firstIp);
         m_lastIp.setText(lastIp);

         setText(MainSubsystem().StringTable().getString(IDS_EDIT_IP_ACCESS_RULE_DIALOG_CAPTION));
      } else {
         setText(MainSubsystem().StringTable().getString(IDS_NEW_IP_ACCESS_RULE_DIALOG_CAPTION));
      }

      return false;
   }

   bool EditIpAccessRuleDialog::onCommand(::u32 controlID, ::u32 notificationID)
   {
      if (notificationID == ::user::e_notification_button_clicked) {
         switch (controlID) {
            case ::innate_subsystem::e_control_id_ok:
               onOkButtonClick();
               break;
            case ::innate_subsystem::e_control_id_cancel:
               onCancelButtonClick();
               break;
            case IDC_ALLOW:
               onAccessTypeRadioClick(0);
               break;
            case IDC_DENY:
               onAccessTypeRadioClick(1);
               break;
            case IDC_QUERY:
               onAccessTypeRadioClick(2);
               break;
         }
      }
      return true;
   }

   void EditIpAccessRuleDialog::onOkButtonClick()
   {
      if (!validateInput()) {
         return ;
      }
      if (m_data != NULL) {
         if (m_access[0].isChecked()) {
            m_data->setAction(IpAccessRule::ACTION_TYPE_ALLOW);
         } else if (m_access[1].isChecked()) {
            m_data->setAction(IpAccessRule::ACTION_TYPE_DENY);
         } else if (m_access[2].isChecked()) {
            m_data->setAction(IpAccessRule::ACTION_TYPE_QUERY);
         }

         ::string firstIp;
         ::string lastIp;

         firstIp = m_firstIp.getText();
         lastIp=m_lastIp.getText();

         m_data->setFirstIp(firstIp);
         m_data->setLastIp(lastIp);

      } // if
      closeDialog(::innate_subsystem::e_control_id_ok);
   }

   void EditIpAccessRuleDialog::onCancelButtonClick()
   {
      closeDialog(::innate_subsystem::e_control_id_cancel);
   }

   void EditIpAccessRuleDialog::onAccessTypeRadioClick(::i32 num)
   {
      if (!m_access[num].isChecked()) {
         m_access[num].setChecked(true);
         for (::i32 i = 0; i < 3; i++) {
            if (i != num) {
               m_access[i].setChecked(false);
            } // if
         } // for
      } // if
   } // void

   void EditIpAccessRuleDialog::initControls()
   {
      //HWND hwnd = operating_system_window();

      dialog_item(m_firstIp, IDC_FIRST_IP);
      dialog_item(m_lastIp, IDC_LAST_IP);

      dialog_item(m_access[0], IDC_ALLOW);
      dialog_item(m_access[1], IDC_DENY);
      dialog_item(m_access[2], IDC_QUERY);
   }

   bool EditIpAccessRuleDialog::validateInput()
   {
      ::string firstIp;
      ::string lastIp;

      firstIp = m_firstIp.getText();
      lastIp = m_lastIp.getText();

      if (!IpAccessRule::isIpAddressStringValid(firstIp)) {
         m_firstIp.setFocus();
         m_firstIp.showBalloonTip(&m_warningBalloonTip);
         return false;
      } // if

      if (lastIp.is_empty()) {
         return true;
      }

      if (!IpAccessRule::isIpAddressStringValid(lastIp)) {
         m_lastIp.setFocus();
         m_lastIp.showBalloonTip(&m_warningBalloonTip);
         return false;
      } // if

      ::string firstIpAnsi(&firstIp);
      ::string lastIpAnsi(&lastIp);

      ulong firstIpAddr = MainSubsystem().internet_address4(firstIpAnsi);
      ulong lastIpAddr = MainSubsystem().internet_address4(lastIpAnsi);

      if (IpAccessRule::compareIp(firstIpAddr, lastIpAddr) == 1) {
         m_lastIp.setFocus();
         m_lastIp.showBalloonTip(&m_lastIpLessThanFirstBT);
         return false;
      }

      return true;
   }
} // namespace remoting_rtc_node



