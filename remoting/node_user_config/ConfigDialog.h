// Copyright (C) 2008,2009,2010,2011,2012 GlavSoft LLC.
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

#pragma once


#include "remoting/remoting/config/SettingsManager.h"
#include "remoting/remoting/config/RegistrySettingsManager.h"

#include "innate_subsystem/gui/Dialog.h"
#include "innate_subsystem/gui/Window.h"
#include "innate_subsystem/gui/ListBox.h"
#include "innate_subsystem/gui/TabControl.h"

#include "remoting/remoting/node_config/Configurator.h"

#include "ServerConfigDialog.h"
#include "AdministrationConfigDialog.h"
#include "PortMappingDialog.h"
#include "IpAccessControlDialog.h"
#include "VideoRegionsConfigDialog.h"

#include "remoting/remoting/node/ControlCommand.h"


namespace remoting_node
{
   class CLASS_DECL_REMOTING ConfigDialog : virtual public ::innate_subsystem::Dialog
   {
   public:
      ConfigDialog(Configurator * pconfigurator, ::remoting_node::ControlCommand *reloadConfigCommand);
      ConfigDialog(Configurator * pconfigurator);
      ConfigDialog();
      virtual ~ConfigDialog();

      void updateApplyButtonState();

      void setConfigReloadCommand(::remoting_node::ControlCommand *command);
      void setServiceFlag(bool serviceFlag);

      bool isConfiguringService();

   protected:

      void initControls();
      void loadSettings();
      // Init dialog handler
      bool onInitDialog() override;
      bool onCommand(::u32 controlID, ::u32 notificationID) override;
      //virtual bool onNotify(::u32 controlID, ::lparam data);
      bool onDestroy() override;
      // Button handlers
      void onCancelButtonClick();
      void onOKButtonClick();
      void onApplyButtonClick();
      // Tab handlers
      bool _002OnTabChanged(int iControl) override;
      bool _002OnTabChanging(int iControl, bool & bOk) override;

   private:
      //void moveDialogToTabControl(::innate_subsystem::DialogInterface * pdialog);;
      bool validateInput();
      void updateCaption();
   protected:
      // Controls
      ::innate_subsystem::Control m_ctrlApplyButton;
      ::innate_subsystem::TabControl m_tabControl;
      // Settings
      ::pointer < Configurator > m_pconfigurator;
      // Dialogs for tab control
      ::remoting_node::ServerConfigDialog m_serverConfigDialog;
      PortMappingDialog m_portMappingDialog;
      AdministrationConfigDialog m_administrationConfigDialog;
      IpAccessControlDialog m_ipAccessControlDialog;
      VideoRegionsConfigDialog m_videoRegionsConfigDialog;
      // Other members
      bool m_isConfiguringService;

      ::pointer < ::remoting_node::ControlCommand > m_pcontrolcommandReloadConfig;

      int m_lastSelectedTabIndex;
   };
} // namespace remoting_node


