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
#include "remoting_node/resource.h"
#include "EditPortMappingDialog.h"

#include "remoting/remoting_common/util/StringParser.h"

#include "remoting/remoting_common/server_config/Configurator.h"
#include "remoting/remoting_common/server_config/PortMappingContainer.h"

EditPortMappingDialog::EditPortMappingDialog(DialogType dlgType)
: BaseDialog(IDD_EDIT_PORT_MAPPING), m_dialogType(dlgType)
{
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
  kill(::subsystem_apex::IDCANCEL);
}

void EditPortMappingDialog::onOkButtonClick()
{
  if (!isUserDataValid())
    return ;

  //
  // Temporary variables
  //

  PortMappingRect rect;
  int port;

  ::string portStringStorage;
  ::string rectStringStorage;

  m_geometryTextBox.getText(&rectStringStorage);
  m_portTextBox.getText(&portStringStorage);

  PortMappingRect::parse(rectStringStorage, &rect);
  main_subsystem()->string_parser()->parseInt(portStringStorage, &port);

  m_mapping->setPort(port);
  m_mapping->setRect(rect);

  kill(::subsystem_apex::IDOK);
}

void EditPortMappingDialog::initControls()
{
  HWND dialogHwnd = m_ctrlThis.operating_system_window();
  m_geometryTextBox.setWindow(GetDlgItem(dialogHwnd, IDC_GEOMETRY_EDIT));
  m_portTextBox.setWindow(GetDlgItem(dialogHwnd, IDC_PORT_EDIT));
}

bool EditPortMappingDialog::isUserDataValid()
{
  ::string rectStringStorage;
  ::string portStringStorage;

  m_geometryTextBox.getText(&rectStringStorage);
  m_portTextBox.getText(&portStringStorage);

  if (!PortMappingRect::tryParse(rectStringStorage)) {
    main_subsystem()->message_box(m_ctrlThis.operating_system_window(),
               main_subsystem()->string_table()->getString(IDS_INVALID_PORT_MAPPING_STRING),
               main_subsystem()->string_table()->getString(IDS_CAPTION_BAD_INPUT),
               ::user::e_message_box_ok | ::user::e_message_box_icon_warning);
    m_geometryTextBox.setFocus();
    return false;
  }

  int port;

  main_subsystem()->string_parser()->parseInt(portStringStorage, &port);

  if ((port < 1) || (port > 65535)) {
    main_subsystem()->message_box(m_ctrlThis.operating_system_window(),
               main_subsystem()->string_table()->getString(IDS_PORT_RANGE_ERROR),
               main_subsystem()->string_table()->getString(IDS_CAPTION_BAD_INPUT),
               ::user::e_message_box_ok | ::user::e_message_box_icon_warning);
    m_portTextBox.setFocus();
    return false;
  }

  PortMappingContainer *extraPorts = Configurator::getInstance()->getServerConfig()->getPortMappingContainer();

  size_t index = extraPorts->findByPort(port);

  if ((index != (size_t)-1) && (extraPorts->at(index) != m_mapping)) {
    main_subsystem()->message_box(m_ctrlThis.operating_system_window(),
               main_subsystem()->string_table()->getString(IDS_PORT_ALREADY_IN_USE),
               main_subsystem()->string_table()->getString(IDS_CAPTION_BAD_INPUT),
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
    m_mapping->getRect().toString(&rectString);

    m_portTextBox.setText(portString);
    m_geometryTextBox.setText(rectString);
  }

  return true;
}

bool EditPortMappingDialog::onCommand(unsigned int cID, unsigned int nID)
{
  switch (cID) {
  case ::subsystem_apex::IDOK:
    onOkButtonClick();
    break;
  case ::subsystem_apex::IDCANCEL:
    onCancelButtonClick();
    break;
  }
  return true;
}
