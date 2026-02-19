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

#pragma once


#include "config_lib/SettingsManager.h"
#include "config_lib/RegistrySettingsManager.h"

#include "gui/BaseDialog.h"
#include "gui/::remoting::Window.h"
#include "gui/ListBox.h"
#include "gui/TabControl.h"

#include "server_config_lib/Configurator.h"

#include "ServerConfigDialog.h"
#include "AdministrationConfigDialog.h"
#include "PortMappingDialog.h"
#include "IpAccessControlDialog.h"
#include "VideoRegionsConfigDialog.h"

#include "remoting_control_desktop/ControlCommand.h"

class ConfigDialog : public BaseDialog
{
public:
  ConfigDialog(bool forService, ControlCommand *reloadConfigCommand);
  ConfigDialog(bool forService);
  ConfigDialog();
  virtual ~ConfigDialog();

  void updateApplyButtonState();

  void setConfigReloadCommand(ControlCommand *command);
  void setServiceFlag(bool serviceFlag);

  bool isConfiguringService();

protected:

  void initControls();
  void loadSettings();
  // Init dialog handler
  virtual BOOL onInitDialog();
  virtual BOOL onCommand(UINT controlID, UINT notificationID);
  virtual BOOL onNotify(UINT controlID, LPARAM data);
  virtual BOOL onDestroy();
  // Button handlers
  void onCancelButtonClick();
  void onOKButtonClick();
  void onApplyButtonClick();
  // Tab handlers
  void onTabChange();
  void onTabChanging();
private:
  void moveDialogToTabControl(BaseDialog *dialog);
  bool validateInput();
  void updateCaption();
protected:
  // Controls
  ::remoting::Window m_ctrlApplyButton;
  TabControl m_tabControl;
  // Settings
  Configurator *m_config;
  // Dialogs for tab control
  ServerConfigDialog m_serverConfigDialog;
  PortMappingDialog m_portMappingDialog;
  AdministrationConfigDialog m_administrationConfigDialog;
  IpAccessControlDialog m_ipAccessControlDialog;
  VideoRegionsConfigDialog m_videoRegionsConfigDialog;
  // Other members
  bool m_isConfiguringService;

  ControlCommand *m_reloadConfigCommand;

  int m_lastSelectedTabIndex;
};


