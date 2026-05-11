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
#include "IpAccessControlDialog.h"
#include "ConfigDialog.h"
#include "CommonInputValidation.h"
#include "UIDataAccess.h"
#include "acme/constant/user_notification.h"
#include "remoting/remoting/node_config/Configurator.h"
#include "remoting/remoting/node_config/IpAccessControl.h"
#include "subsystem/_common_header.h"
#include "subsystem/platform/StringParser.h"
//#include "subsystem/platform/::string.h"
namespace remoting_node
{
   IpAccessControlDialog::IpAccessControlDialog(Configurator * pconfigurator)
   :  m_pdialogParent(NULL)
   , m_pconfigurator(pconfigurator)
   {
      initialize_dialog(IDD_CONFIG_ACCESS_CONTROL_PAGE);
   }

   IpAccessControlDialog::~IpAccessControlDialog()
   {
   }

   void IpAccessControlDialog::setParentDialog(::innate_subsystem::DialogInterface *dialog)
   {
      m_pdialogParent = dialog;
   }

   bool IpAccessControlDialog::onInitDialog()
   {
      m_pserverconfig = m_pconfigurator->getServerConfig();
      m_pipaccesscontrol = m_pserverconfig->getAccessControl();
      m_editDialog.setParent(this);
      initControls();
      updateUI();
      updateButtonsState();
      onIpCheckUpdate();
      return true;
   }

   bool IpAccessControlDialog::onCommand(::u32 controlID, ::u32 notificationID)
   {
      if (notificationID == ::user::e_notification_button_clicked) {
         switch (controlID) {
            case IDC_ADD_BUTTON:
               onAddButtonClick();
               break;
            case IDC_EDIT_BUTTON:
               onEditButtonClick();
               break;
            case IDC_REMOVE_BUTTON:
               onRemoveButtonClick();
               break;
            case IDC_MOVE_UP_BUTTON:
               onMoveUpButtonClick();
               break;
            case IDC_MOVE_DOWN_BUTTON:
               onMoveDownButtonClick();
               break;
            case IDC_ACCEPT:
               onAcceptRadioClick();
               break;
            case IDC_REFUSE:
               onRefuseRadioClick();
               break;
            case IDC_ALLOW_LOOPBACK_CONNECTIONS:
               onAllowLoopbackConnectionsClick();
               break;
            case IDC_ALLOW_ONLY_LOOPBACK_CONNECTIONS:
               onAllowOnlyLoopbackConnectionsClick();
               break;
         }
      } else if (notificationID == ::user::e_notification_edit_update) {
         switch (controlID) {
            case IDC_IP_FOR_CHECK_EDIT:
               onIpCheckUpdate();
               break;
            case IDC_TIMEOUT:
               onQueryTimeoutUpdate();
               break;
         }
      }
      return true;
   }
   //
   // bool IpAccessControlDialog::onNotify(::u32 controlID, ::lparam data)
   // {
   //    if (controlID == IDC_IP_ACCESS_CONTROL_LIST) {
   //       NMHDR* pnmh = (NMHDR*)data;
   //       switch (pnmh->code) {
   //          case LVN_ITEMCHANGED:
   //             onListViewSelChange();
   //             break;
   //          case NM_DBLCLK:
   //             onListViewSelChangeDblClick();
   //             break;
   //       }
   //    }
   //    return true;
   // }
   //

   bool IpAccessControlDialog::_002OnSelectionChange(int iControl)
   {
      if (iControl == IDC_IP_ACCESS_CONTROL_LIST) {
               onListViewSelChange();
      return true;
      }
      return false;
   }

   bool IpAccessControlDialog::_002OnAction(int controlID)
{
   if (controlID == IDC_IP_ACCESS_CONTROL_LIST) {
            onListViewSelChangeDblClick();
   return true;
   }
   return false;
}
   bool IpAccessControlDialog::validateInput()
   {
      if (!CommonInputValidation::validateUINT(
        &m_queryTimeout,
        MainSubsystem().StringTable().getString(IDS_INVALID_QUERY_TIMEOUT))) {
         return false;
        }

      ::u32 queryTimeout;

      UIDataAccess::queryValueAsUInt(&m_queryTimeout, &queryTimeout);

      if (queryTimeout < ::remoting_node::ServerConfig::MINIMAL_QUERY_TIMEOUT) {
         CommonInputValidation::notifyValidationError(
           &m_queryTimeout,
           MainSubsystem().StringTable().getString(IDS_QUERY_TIMEOUT_TOO_SMALL));
         return false;
      }

      return true;
   }

   void IpAccessControlDialog::updateUI()
   {
      m_list.clear();
      for (size_t i = 0; i < m_pipaccesscontrol->size(); i++) {
         IpAccessRule *ip = m_pipaccesscontrol->at(i);
         m_list.addItem(m_list.getCount(), "", (::lparam)ip);
         ASSERT((int)i == i);
         setListViewItemText((int)i, ip);
      }

      m_allowLoopbackConnections.setChecked(m_pserverconfig->isLoopbackConnectionsAllowed());
      m_onlyLoopbackConnections.setChecked(m_pserverconfig->isOnlyLoopbackConnectionsAllowed());
      if (m_pserverconfig->isDefaultActionAccept()) {
         m_defaultActionAccept.setChecked(true);
         m_defaultActionRefuse.setChecked(false);
      } else {
         m_defaultActionAccept.setChecked(false);
         m_defaultActionRefuse.setChecked(true);
      }
      m_queryTimeout.setUnsignedInt(m_pserverconfig->getQueryTimeout());

      updateCheckBoxesState();
   }

   void IpAccessControlDialog::apply()
   {
      // Query timeout string storage
      ::string qtStringStorage;
      qtStringStorage = m_queryTimeout.getText();

      int timeout = 0;
      MainSubsystem().StringParser().parseInt(qtStringStorage, &timeout);

      AutoLock al(m_pserverconfig);

      m_pserverconfig->allowLoopbackConnections(m_allowLoopbackConnections.isChecked());
      m_pserverconfig->acceptOnlyLoopbackConnections(m_onlyLoopbackConnections.isChecked());
      m_pserverconfig->setDefaultActionToAccept(m_defaultActionAccept.isChecked());
      m_pserverconfig->setQueryTimeout(timeout);

      //
      // Put IP access rules container in correct order
      //

      IpAccessControl *ipRules = m_pserverconfig->getAccessControl();
      ipRules->clear();
      for (int i = 0; i < m_list.getCount(); i++) {
         IpAccessRule *rule = m_list.getItemData(i).raw_cast <IpAccessRule *>();
         ipRules->add(rule);
      }
   }

   void IpAccessControlDialog::initControls()
   {
      //HWND hwnd = operating_system_window();
      dialog_item(m_list, IDC_IP_ACCESS_CONTROL_LIST);
      dialog_item(m_addButton, IDC_ADD_BUTTON);
      dialog_item(m_editButton, IDC_EDIT_BUTTON);
      dialog_item(m_removeButton, IDC_REMOVE_BUTTON);
      dialog_item(m_moveUpButton, IDC_MOVE_UP_BUTTON);
      dialog_item(m_moveDownButton, IDC_MOVE_DOWN_BUTTON);
      dialog_item(m_defaultActionAccept, IDC_ACCEPT);
      dialog_item(m_defaultActionRefuse, IDC_REFUSE);
      dialog_item(m_queryTimeout, IDC_TIMEOUT);
      dialog_item(m_allowLoopbackConnections, IDC_ALLOW_LOOPBACK_CONNECTIONS);
      dialog_item(m_onlyLoopbackConnections, IDC_ALLOW_ONLY_LOOPBACK_CONNECTIONS);
      dialog_item(m_queryTimeoutSpin, IDC_QUERY_TIMEOUT_SPIN);
      dialog_item(m_ip, IDC_IP_FOR_CHECK_EDIT);
      dialog_item(m_ipCheckResult, IDC_IP_CHECK_RESULT_LABEL);

      m_list.addColumn(0, MainSubsystem().StringTable().getString(IDS_FIRST_IP_COLUMN), 100);
      m_list.addColumn(1, MainSubsystem().StringTable().getString(IDS_LAST_IP_COLUMN), 100);
      m_list.addColumn(2, MainSubsystem().StringTable().getString(IDS_ACTION_COLUMN), 80);

      m_list.allowMultiSelection(false);
      m_list.setFullRowSelectStyle(true);

      m_queryTimeoutSpin.setBuddy(&m_queryTimeout);
      m_queryTimeoutSpin.setAccel(0, 1);
      m_queryTimeoutSpin.setRange32(0, INT_MAX);
   }

   void IpAccessControlDialog::onAddButtonClick()
   {
      IpAccessRule *ip = new IpAccessRule();
      m_editDialog.setIpAccessControl(ip);
      m_editDialog.setEditFlag(false);
      if (m_editDialog.showModal() == ::innate_subsystem::e_control_id_ok) {
         m_pipaccesscontrol->add(ip);
         m_list.addItem(m_list.getCount(), "", (::lparam)ip);
         setListViewItemText(m_list.getCount() - 1, ip);
         updateButtonsState();
         onIpCheckUpdate();
         auto pconfigdialog = m_pdialogParent->get_callback<ConfigDialog>();
         pconfigdialog->updateApplyButtonState();
      } else {
         delete ip;
      }
      m_list.invalidate();
      this->invalidate();
   }

   void IpAccessControlDialog::onEditButtonClick()
   {
      // If nothing selected
      if (m_list.getSelectedIndex() == -1) {
         return ;
      }
      IpAccessRule *ip = m_list.getItemData(m_list.getSelectedIndex()).raw_cast <IpAccessRule *>();
      m_editDialog.setIpAccessControl(ip);
      m_editDialog.setEditFlag(true);
      if (m_editDialog.showModal() == ::innate_subsystem::e_control_id_ok) {
         setListViewItemText(m_list.getSelectedIndex(), ip);
         updateButtonsState();
         onIpCheckUpdate();
         auto pconfigdialog = m_pdialogParent->get_callback<ConfigDialog>();
         pconfigdialog->updateApplyButtonState();
      } else {
      }
      m_list.invalidate();
      this->invalidate();
   }

   void IpAccessControlDialog::onRemoveButtonClick()
   {
      int si = m_list.getSelectedIndex();
      // If nothing selected
      if (si == -1) {
         return ;
      }
      {
         AutoLock al(m_pserverconfig);
         IpAccessRule *ip = m_list.getItemData(m_list.getSelectedIndex()).raw_cast <IpAccessRule *>();
         for (IpAccessControl::iterator it = m_pipaccesscontrol->begin(); it != m_pipaccesscontrol->end(); it++) {
            IpAccessRule *ip2 = *it;
            if (ip == ip2) {
               m_pipaccesscontrol->erase(it);
               m_list.removeItem(si);
               updateButtonsState();
               onIpCheckUpdate();
               auto pconfigdialog = m_pdialogParent->get_callback<ConfigDialog>();
               pconfigdialog->updateApplyButtonState();
               break;
            }
         }
         delete ip;
      }
      m_list.selectItem(si);
      if (m_list.getSelectedIndex() == -1) {
         m_list.selectItem(si - 1);
      } else if (m_list.getSelectedIndex() == -1) {
         m_list.selectItem(si + 1);
      }
   }

   void IpAccessControlDialog::onMoveUpButtonClick()
   {
      int si = m_list.getSelectedIndex();
      // If nothing selected
      if ((si == -1) || (si == 0)) {
         return ;
      }

      IpAccessRule *ip = (IpAccessRule *)m_list.getItemData(si).raw_cast<IpAccessRule*>();
      IpAccessRule *ipPrev = (IpAccessRule *)m_list.getItemData(si - 1).raw_cast<IpAccessRule*>();

      setListViewItemText(si - 1, ip);
      setListViewItemText(si, ipPrev);
      m_list.selectItem(si - 1);
      onIpCheckUpdate();
      auto pconfigdialog = m_pdialogParent->get_callback<ConfigDialog>();
      pconfigdialog->updateApplyButtonState();
   }

   void IpAccessControlDialog::onMoveDownButtonClick()
   {
      int si = m_list.getSelectedIndex();
      // If nothing selected
      if ((si == -1) || (si == m_list.getCount() - 1)) {
         return ;
      }
      IpAccessRule *ip = (IpAccessRule *)m_list.getItemData(si).raw_cast<IpAccessRule*>();
      IpAccessRule *ipNext = (IpAccessRule *)m_list.getItemData(si + 1).raw_cast<IpAccessRule*>();

      setListViewItemText(si, ipNext);
      setListViewItemText(si + 1, ip);
      m_list.selectItem(si + 1);
      onIpCheckUpdate();
      auto pconfigdialog = m_pdialogParent->get_callback<ConfigDialog>();
      pconfigdialog->updateApplyButtonState();
   }

   void IpAccessControlDialog::onListViewSelChange()
   {
      int si = m_list.getSelectedIndex();
      updateButtonsState();
   }

   void IpAccessControlDialog::onAcceptRadioClick()
   {
      if (!m_defaultActionAccept.isChecked()) {
         m_defaultActionAccept.setChecked(true);
         m_defaultActionRefuse.setChecked(false);
         auto pconfigdialog = m_pdialogParent->get_callback<ConfigDialog>();
         pconfigdialog->updateApplyButtonState();
      }
   }

   void IpAccessControlDialog::onRefuseRadioClick()
   {
      if (!m_defaultActionRefuse.isChecked()) {
         m_defaultActionRefuse.setChecked(true);
         m_defaultActionAccept.setChecked(false);
         auto pconfigdialog = m_pdialogParent->get_callback<ConfigDialog>();
         pconfigdialog->updateApplyButtonState();
      }
   }

   void IpAccessControlDialog::onAllowLoopbackConnectionsClick()
   {
      updateCheckBoxesState();
      updateButtonsState();
      auto pconfigdialog = m_pdialogParent->get_callback<ConfigDialog>();
      pconfigdialog->updateApplyButtonState();
   }

   void IpAccessControlDialog::onAllowOnlyLoopbackConnectionsClick()
   {
      updateCheckBoxesState();
      updateButtonsState();
      auto pconfigdialog = m_pdialogParent->get_callback<ConfigDialog>();
      pconfigdialog->updateApplyButtonState();
   }

   void IpAccessControlDialog::onIpCheckUpdate()
   {
      ::string ipStorage;
      ipStorage = m_ip.getText();

      // Check if ip address is valid.

      if (!IpAccessRule::isIpAddressStringValid(ipStorage)) {
         if (ipStorage.is_empty()) {
            m_ipCheckResult.setText(MainSubsystem().StringTable().getString(IDS_ENTER_IP_HINT));
         } else {
            m_ipCheckResult.setText(MainSubsystem().StringTable().getString(IDS_BAD_IP_HINT));
         }
         return;
      }

      //
      // Convert ip to ansi string
      //

      //::string ansiIpStorage(&ipStorage);
      ::u32 addr = MainSubsystem().internet_address4(ipStorage);

      IpAccessRule::ActionType action = IpAccessRule::ACTION_TYPE_ALLOW;
      int rulesCount = 0;
      if (m_list.getCount() > 0) {
         rulesCount = m_list.getCount();
      }
      for (int i = 0; i < rulesCount; i++) {
         IpAccessRule *rule = (IpAccessRule *)m_list.getItemData(i).raw_cast <IpAccessRule *>();
         if (rule->isIncludingAddress(addr)) {
            action = rule->getAction();
            break;
         }
      }

      ::string actionDescription;
      actionDescription= MainSubsystem().StringTable().getString(IDS_ACTION_UNDEF_HINT);
      switch (action) {
         case IpAccessRule::ACTION_TYPE_ALLOW:
            actionDescription= MainSubsystem().StringTable().getString(IDS_ACTION_ACCEPT_HINT);
            break;
         case IpAccessRule::ACTION_TYPE_DENY:
            actionDescription= MainSubsystem().StringTable().getString(IDS_ACTION_REJECT_HINT);
            break;
         case IpAccessRule::ACTION_TYPE_QUERY:
            actionDescription= MainSubsystem().StringTable().getString(IDS_ACTION_QUERY_HINT);
            break;
      }

      m_ipCheckResult.setText(actionDescription);
   }

   void IpAccessControlDialog::onQueryTimeoutUpdate()
   {
      auto pconfigdialog = m_pdialogParent->get_callback<ConfigDialog>();
      pconfigdialog->updateApplyButtonState();
   }

   void IpAccessControlDialog::updateButtonsState()
   {
      /*if (m_onlyLoopbackConnections.isChecked()) {
        m_list.enableWindow(false);
        m_addButton.enableWindow(false);
        m_editButton.enableWindow(false);
        m_removeButton.enableWindow(false);
        m_moveDownButton.enableWindow(false);
        m_moveUpButton.enableWindow(false);
        return ;
      } else {
        m_list.enableWindow(true);
        m_addButton.enableWindow(true);
      }*/

      int si = m_list.getSelectedIndex();
      if (si == -1) {
         m_editButton.enableWindow(false);
         m_removeButton.enableWindow(false);
         m_moveUpButton.enableWindow(false);
         m_moveDownButton.enableWindow(false);
      } else {
         m_editButton.enableWindow(true);
         m_removeButton.enableWindow(true);
         if (si > 0) {
            m_moveUpButton.enableWindow(true);
         } else {
            m_moveUpButton.enableWindow(false);
         }
         if (si < m_list.getCount() - 1) {
            m_moveDownButton.enableWindow(true);
         } else {
            m_moveDownButton.enableWindow(false);
         }
      }
   }

   void IpAccessControlDialog::updateCheckBoxesState()
   {
      if (m_allowLoopbackConnections.isChecked()) {
         m_onlyLoopbackConnections.enableWindow(true);
      } else {
         m_onlyLoopbackConnections.enableWindow(false);
         m_onlyLoopbackConnections.setChecked(false);
      }
   }

   void IpAccessControlDialog::onListViewSelChangeDblClick()
   {
      int si = m_list.getSelectedIndex();
      if (si == -1) {
         return ;
      } else {
         onEditButtonClick();
      }
   }

   void IpAccessControlDialog::setListViewItemText(int index, IpAccessRule *control)
   {
      ::string firstIp;
      ::string lastIp;

      control->getFirstIp(firstIp);
      control->getLastIp(lastIp);

      m_list.setSubItemText(index, 0, firstIp);
      m_list.setSubItemText(index, 1, lastIp);
      switch (control->getAction()) {
         case IpAccessRule::ACTION_TYPE_ALLOW:
            m_list.setSubItemText(index, 2, MainSubsystem().StringTable().getString(IDS_ACTION_ACCEPT));
            break;
         case IpAccessRule::ACTION_TYPE_DENY:
            m_list.setSubItemText(index, 2, MainSubsystem().StringTable().getString(IDS_ACTION_DENY));
            break;
         case IpAccessRule::ACTION_TYPE_QUERY:
            m_list.setSubItemText(index, 2, MainSubsystem().StringTable().getString(IDS_ACTION_QUERY));
            break;
      }
      m_list.setItemData(index, (::lparam)control);
   }
} // namespace remoting_node



