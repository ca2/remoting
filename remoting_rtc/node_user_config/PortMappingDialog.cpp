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
#include "PortMappingDialog.h"
#include "EditPortMappingDialog.h"
#include "ConfigDialog.h"
#include "remoting/node_desktop/resource.h"
#include "acme/constant/user_notification.h"


namespace remoting_rtc_node
{
   PortMappingDialog::PortMappingDialog(::remoting_rtc_node::Configurator * pconfigurator)
   : m_pconfigurator(pconfigurator), m_pdialogParent(NULL)
   {
      initialize_dialog(IDD_CONFIG_PORT_MAPPING_PAGE);
   }

   PortMappingDialog::~PortMappingDialog()
   {
   }

   void PortMappingDialog::setParentDialog(::innate_subsystem::DialogInterface * pdialog)
   {
      m_pdialogParent = pdialog;
   }

   void PortMappingDialog::initControls()
      //HWND dialogHwnd = operating_sy//tem_window();
   {

      dialog_item(m_exPortsListBox, IDC_MAPPINGS);
      dialog_item(m_editButton, IDC_EDIT_PORT);
      dialog_item(m_removeButton, IDC_REMOVE_PORT);
   }

   bool PortMappingDialog::onCommand(::u32 controlID, ::u32 notificationID)
   {
      switch (controlID) {
         case IDC_ADD_PORT:
            onAddButtonClick();
            break;
         case IDC_EDIT_PORT:
            onEditButtonClick();
            break;
         case IDC_REMOVE_PORT:
            onRemoveButtonClick();
            break;
         case IDC_MAPPINGS:
            switch (notificationID) {
               case ::user::e_notification_list_box_selection_change:
                  onExPortsListBoxSelChange();
                  break;
               case ::user::e_notification_list_box_double_click:
                  onExPortsListBoxDoubleClick();
                  break;
            }
            break;
      }
      return true;
   }

   bool PortMappingDialog::onInitDialog()
   {
      initControls();

      m_extraPorts = m_pconfigurator->getServerConfig()->getPortMappingContainer();

      //
      // Fill listbox.
      //

      ::string mappingString;

      for (size_t i = 0; i < m_extraPorts->count(); i++) {
         mappingString = m_extraPorts->at(i)->toString();
         ASSERT((::i32)i == i);
         m_exPortsListBox.insertString((::i32)i, mappingString);
      }

      return true;
   }

   void PortMappingDialog::onExPortsListBoxSelChange()
   {
      ::i32 selectedIndex = m_exPortsListBox.getSelectedIndex();

      m_editButton.enableWindow(selectedIndex >= 0);
      m_removeButton.enableWindow(selectedIndex >= 0);
   }

   void PortMappingDialog::onAddButtonClick()
   {


      EditPortMappingDialog addDialog(m_pconfigurator, EditPortMappingDialog::Add);

      PortMapping newPM;

      addDialog.setMapping(&newPM);
      addDialog.setParent(this);

      if (addDialog.showModal() == ::innate_subsystem::e_control_id_ok) {
         {
            ::string mappingString;
            mappingString = newPM.toString();
            m_exPortsListBox.addString(mappingString);
         }

         m_extraPorts->pushBack(newPM);

         auto pconfigdialog = m_pdialogParent->get_callback<ConfigDialog>();

         pconfigdialog->updateApplyButtonState();
      }
   }

   void PortMappingDialog::onEditButtonClick()
   {
      ::i32 selectedIndex = m_exPortsListBox.getSelectedIndex();

      if (selectedIndex == -1) {
         return ;
      }

      PortMapping *pPM = m_extraPorts->at(selectedIndex);

      EditPortMappingDialog editDialog(m_pconfigurator, EditPortMappingDialog::Edit);

      editDialog.setParent(this);
      editDialog.setMapping(pPM);

      if (editDialog.showModal() == ::innate_subsystem::e_control_id_ok) {
         ::string mappingString;
         mappingString = pPM->toString();
         m_exPortsListBox.setItemText(selectedIndex, mappingString);

         auto pconfigdialog = m_pdialogParent->get_callback<ConfigDialog>();

         pconfigdialog->updateApplyButtonState();
      }
   }

   void PortMappingDialog::onRemoveButtonClick()
   {
      ::i32 selectedIndex = m_exPortsListBox.getSelectedIndex();

      if (selectedIndex == -1) {
         return ;
      }

      m_exPortsListBox.removeString(selectedIndex);
      m_extraPorts->remove(selectedIndex);

      auto pconfigdialog = m_pdialogParent->get_callback<ConfigDialog>();

      pconfigdialog->updateApplyButtonState();

      //
      // Restore selection after item is removed.
      //

      if (m_exPortsListBox.getCount() > 0) {
         m_exPortsListBox.setSelectedIndex(selectedIndex);
         if (m_exPortsListBox.getSelectedIndex() == -1) {
            m_exPortsListBox.setSelectedIndex(selectedIndex - 1);
         }
         if (m_exPortsListBox.getSelectedIndex() == -1) {
            m_exPortsListBox.setSelectedIndex(selectedIndex + 1);
         }
      } else {
         m_removeButton.enableWindow(false);
         m_editButton.enableWindow(false);
      }
   }

   void PortMappingDialog::onExPortsListBoxDoubleClick()
   {
      if (m_exPortsListBox.getSelectedIndex() != -1)
         onEditButtonClick();
   }
} // namespace remoting_rtc_node



