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

#pragma once


#include "innate_subsystem/gui/Dialog.h"
#include "innate_subsystem/gui/ListView.h"
#include "innate_subsystem/gui/SpinControl.h"
#include "innate_subsystem/gui/BalloonTip.h"

#include "remoting/remoting_rtc/node_config/Configurator.h"

#include "EditIpAccessRuleDialog.h"

namespace remoting_rtc_node
{
   class IpAccessControlDialog : virtual public ::innate_subsystem::Dialog
   {
   public:


      IpAccessControlDialog(Configurator * pconfigurator);
      ~IpAccessControlDialog() override;


      void setParentDialog(::innate_subsystem::DialogInterface * pdialog);;

      //
      // BaseDialog overrided methods
      //

      virtual bool onInitDialog() override;
      virtual bool onCommand(::u32 controlID, ::u32 notificationID) override;
      //virtual bool onNotify(::u32 controlID, ::lparam data);
      virtual bool onDestroy() override { return true; }


      bool _002OnSelectionChange(int iControl) override;
      bool _002OnAction(int iControl) override;
      //
      // Helper methods
      //

      bool validateInput();
      void updateUI();
      void apply();

   private:
      void initControls();

      //
      // ::innate_subsystem::Control event handlers
      //

      void onAddButtonClick();
      void onEditButtonClick();
      void onRemoveButtonClick();
      void onMoveUpButtonClick();
      void onMoveDownButtonClick();
      void onListViewSelChange();
      void onListViewSelChangeDblClick();
      void onAcceptRadioClick();
      void onRefuseRadioClick();
      void onAllowLoopbackConnectionsClick();
      void onAllowOnlyLoopbackConnectionsClick();
      void onIpCheckUpdate();
      void onQueryTimeoutUpdate();

      //
      // Private helper methods
      //

      void updateButtonsState();
      void updateCheckBoxesState();
      void setListViewItemText(int index, IpAccessRule *control);

   private:
      // Configuration
      ::pointer < IpAccessControl > m_pipaccesscontrol;
      ::pointer < ::remoting_rtc_node::Configurator > m_pconfigurator;
      ::pointer < ::remoting_rtc_node::ServerConfig > m_pserverconfig;
      // Child dialog
      EditIpAccessRuleDialog m_editDialog;
      // Controls
      ::innate_subsystem::ListView m_list;
      ::innate_subsystem::Control m_addButton;
      ::innate_subsystem::Control m_editButton;
      ::innate_subsystem::Control m_removeButton;
      ::innate_subsystem::Control m_moveUpButton;
      ::innate_subsystem::Control m_moveDownButton;
      ::innate_subsystem::CheckBox m_defaultActionAccept;
      ::innate_subsystem::CheckBox m_defaultActionRefuse;
      ::innate_subsystem::CheckBox m_allowLoopbackConnections;
      ::innate_subsystem::CheckBox m_onlyLoopbackConnections;
      ::innate_subsystem::TextBox m_queryTimeout;
      ::innate_subsystem::TextBox m_ip;
      ::innate_subsystem::Control m_ipCheckResult;
      ::innate_subsystem::SpinControl m_queryTimeoutSpin;
      ::pointer < ::innate_subsystem::DialogInterface > m_pdialogParent;
   };
} // namespace remoting_rtc_node



