// Copyright (C) 2008,2009,2010,2011,2012 GlavSoft LLC.
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
#include "EditPortMappingDialog.h"

#include "subsystem/platform/StringParser.h"

#include "remoting/remoting/node_config/Configurator.h"
#include "remoting/remoting/node_config/PortMappingContainer.h"
namespace remoting_node
{
   EditPortMappingDialog::EditPortMappingDialog(::remoting_node::Configurator * pconfigurator, DialogType dlgType)
   :  m_pconfigurator(pconfigurator), m_dialogType(dlgType)
   {
      initialize_dialog(IDD_EDIT_PORT_MAPPING);
   }

   EditPortMappingDialog::~EditPortMappingDialog()
   {
   }

   void EditPortMappingDialog::setMapping(PortMapping *mapping)
   {
      m_mapping = mapping;
   }

   void EditPortMappingDialog::onCancelButtonClick()
   {
      closeDialog(::innate_subsystem::e_control_id_cancel);
   }

   void EditPortMappingDialog::onOkButtonClick()
   {
      if (!isUserDataValid())
         return ;

      //
      // Temporary variables
      //

      PortMappingRect rectangle;
      int port;

      ::string portStringStorage;
      ::string rectStringStorage;

      rectStringStorage = m_geometryTextBox.getText();
      portStringStorage = m_portTextBox.getText();

      rectangle.parse(rectStringStorage);
      MainSubsystem().StringParser().parseInt(portStringStorage, &port);

      m_mapping->setPort(port);
      m_mapping->setRect(rectangle);

      closeDialog(::innate_subsystem::e_control_id_ok);
   }

   void EditPortMappingDialog::initControls()
   {
      //HWND dialogHwnd = operating_system_window();
      dialog_item(m_geometryTextBox, IDC_GEOMETRY_EDIT);
      dialog_item(m_portTextBox, IDC_PORT_EDIT);
   }

   bool EditPortMappingDialog::isUserDataValid()
   {
      ::string rectStringStorage;
      ::string portStringStorage;

      rectStringStorage = m_geometryTextBox.getText();
      portStringStorage = m_portTextBox.getText();

      if (!PortMappingRect::tryParse(rectStringStorage)) {
         MainSubsystem().message_box(operating_system_window(),
                    MainSubsystem().StringTable().getString(IDS_INVALID_PORT_MAPPING_STRING),
                    MainSubsystem().StringTable().getString(IDS_CAPTION_BAD_INPUT),
                    ::user::e_message_box_ok | ::user::e_message_box_icon_warning);
         m_geometryTextBox.setFocus();
         return false;
      }

      int port;

      MainSubsystem().StringParser().parseInt(portStringStorage, &port);

      if ((port < 1) || (port > 65535)) {
         MainSubsystem().message_box(operating_system_window(),
                    MainSubsystem().StringTable().getString(IDS_PORT_RANGE_ERROR),
                    MainSubsystem().StringTable().getString(IDS_CAPTION_BAD_INPUT),
                    ::user::e_message_box_ok | ::user::e_message_box_icon_warning);
         m_portTextBox.setFocus();
         return false;
      }

      PortMappingContainer *extraPorts = m_pconfigurator->getServerConfig()->getPortMappingContainer();

      size_t index = extraPorts->findByPort(port);

      if ((index != (size_t)-1) && (extraPorts->at(index) != m_mapping)) {
         MainSubsystem().message_box(operating_system_window(),
                    MainSubsystem().StringTable().getString(IDS_PORT_ALREADY_IN_USE),
                    MainSubsystem().StringTable().getString(IDS_CAPTION_BAD_INPUT),
                    ::user::e_message_box_ok | ::user::e_message_box_icon_warning);
         m_portTextBox.setFocus();
         return false;
      }

      return true;
   }

   bool EditPortMappingDialog::onInitDialog()
   {
      initControls();

      if (m_dialogType == Add) {
         m_portTextBox.setText("5901");
         m_geometryTextBox.setText("640x480+0+0");
      } else if (m_dialogType == Edit) {
         ::string portString;
         ::string rectString;

         portString.formatf("{}", m_mapping->getPort());
         rectString = m_mapping->getRect().toString();

         m_portTextBox.setText(portString);
         m_geometryTextBox.setText(rectString);
      }

      return true;
   }

   bool EditPortMappingDialog::onCommand(unsigned int cID, unsigned int nID)
   {
      switch (cID) {
         case ::innate_subsystem::e_control_id_ok:
            onOkButtonClick();
            break;
         case ::innate_subsystem::e_control_id_cancel:
            onCancelButtonClick();
            break;
      }
      return true;
   }
} // namespace remoting_node






