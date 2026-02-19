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

#include "util/StringParser.h"

#include "server_config_lib/Configurator.h"
#include "server_config_lib/PortMappingContainer.h"

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
  kill(IDCANCEL);
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

  m_geometryTextBox.get_text(&rectStringStorage);
  m_portTextBox.get_text(&portStringStorage);

  PortMappingRect::parse(rectStringStorage, &rect);
  StringParser::parseInt(portStringStorage, &port);

  m_mapping->setPort(port);
  m_mapping->setRect(rect);

  kill(IDOK);
}

void EditPortMappingDialog::initControls()
{
  HWND dialogHwnd = m_ctrlThis.get_hwnd();
  m_geometryTextBox.setWindow(GetDlgItem(dialogHwnd, IDC_GEOMETRY_EDIT));
  m_portTextBox.setWindow(GetDlgItem(dialogHwnd, IDC_PORT_EDIT));
}

bool EditPortMappingDialog::isUserDataValid()
{
  ::string rectStringStorage;
  ::string portStringStorage;

  m_geometryTextBox.get_text(&rectStringStorage);
  m_portTextBox.get_text(&portStringStorage);

  if (!PortMappingRect::tryParse(rectStringStorage)) {
    ::remoting::message_box(m_ctrlThis.get_hwnd(),
               StringTable::getString(IDS_INVALID_PORT_MAPPING_STRING),
               StringTable::getString(IDS_CAPTION_BAD_INPUT),
               MB_OK | MB_ICONWARNING);
    m_geometryTextBox.set_focus();
    return false;
  }

  int port;

  StringParser::parseInt(portStringStorage, &port);

  if ((port < 1) || (port > 65535)) {
    ::remoting::message_box(m_ctrlThis.get_hwnd(),
               StringTable::getString(IDS_PORT_RANGE_ERROR),
               StringTable::getString(IDS_CAPTION_BAD_INPUT),
               MB_OK | MB_ICONWARNING);
    m_portTextBox.set_focus();
    return false;
  }

  PortMappingContainer *extraPorts = Configurator::getInstance()->getServerConfig()->getPortMappingContainer();

  size_t index = extraPorts->findByPort(port);

  if ((index != (size_t)-1) && (extraPorts->at(index) != m_mapping)) {
    ::remoting::message_box(m_ctrlThis.get_hwnd(),
               StringTable::getString(IDS_PORT_ALREADY_IN_USE),
               StringTable::getString(IDS_CAPTION_BAD_INPUT),
               MB_OK | MB_ICONWARNING);
    m_portTextBox.set_focus();
    return false;
  }

  return true;
}

BOOL EditPortMappingDialog::onInitDialog()
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

  return TRUE;
}

BOOL EditPortMappingDialog::onCommand(UINT cID, UINT nID)
{
  switch (cID) {
  case IDOK:
    onOkButtonClick();
    break;
  case IDCANCEL:
    onCancelButtonClick();
    break;
  }
  return TRUE;
}
