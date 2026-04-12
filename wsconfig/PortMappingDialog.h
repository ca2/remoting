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

#pragma once


#include "innate_subsystem/BaseDialog.h"
#include "innate_subsystem/Window.h"
#include "innate_subsystem/ListBox.h"

#include "remoting/remoting_common/server_config/Configurator.h"

class PortMappingDialog : public BaseDialog
{
public:
  PortMappingDialog();
  virtual ~PortMappingDialog();

  void setParentDialog(BaseDialog *dialog);

protected:

  //
  // Inherited from BaseDialog.
  //

  virtual bool onInitDialog();
  virtual bool onCommand(unsigned int controlID, unsigned int notificationID);
  virtual bool onNotify(unsigned int controlID, ::lparam data) { return true; }
  virtual bool onDestroy() { return true; }

  //
  // Controls event handlers.
  //

  void onAddButtonClick();
  void onEditButtonClick();
  void onRemoveButtonClick();
  void onExPortsListBoxSelChange();
  void onExPortsListBoxDoubleClick();

private:
  void initControls();

protected:
  ListBox m_exPortsListBox;
  ::subsystem_apex::Control m_editButton;
  ::subsystem_apex::Control m_removeButton;

  PortMappingContainer *m_extraPorts;

  BaseDialog *m_parent;
};


