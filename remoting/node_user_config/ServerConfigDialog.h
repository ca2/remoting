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


#include "innate_subsystem/gui/Dialog.h"
#include "innate_subsystem/gui/TextBox.h"
#include "innate_subsystem/gui/CheckBox.h"
#include "innate_subsystem/gui/SpinControl.h"
#include "innate_subsystem/gui/BalloonTip.h"

#include "remoting/remoting/node_config/ServerConfig.h"
#include "PasswordControl.h"

namespace remoting_node
{
   class ServerConfigDialog : virtual public ::innate_subsystem::Dialog
   {
   public:
      ServerConfigDialog();
      virtual ~ServerConfigDialog();

      void setParentDialog(::innate_subsystem::Dialog *dialog);

   public:

      //
      // BaseDialog overrided methods
      //

      virtual bool onInitDialog();
      virtual bool onNotify(unsigned int controlID, ::lparam data);
      virtual bool onCommand(unsigned int controlID, unsigned int notificationID);
      virtual bool onDestroy() { return true; }

      //
      // Helper methods
      //

      bool validateInput();
      void updateUI();
      void apply();

   private:
      void initControls();
      void updateControlDependencies();

      //
      // Contol event handlers
      //

      void onAcceptRfbConnectionsClick();
      void onAcceptHttpConnectionsClick();
      void onAuthenticationClick();
      void onPrimaryPasswordChange();
      void onReadOnlyPasswordChange();
      void onUnsetPrimaryPasswordClick();
      void onUnsetReadOnlyPasswordClick();
      void onShowTrayIconCheckBoxClick();
      void onConnectToRdpCheckBoxClick();
      void onPollingIntervalSpinChangePos(int iControlId, int iPos, int iDelta);
      void onRfbPortUpdate();
      void onHttpPortUpdate();
      void onUrlParamsClick();
      void onPollingIntervalUpdate();
      void onFileTransferCheckBoxClick();
      void onRemoveWallpaperCheckBoxClick();
      void onGrabTransparentWindowsChanged();
      void onUseD3DChanged();

      //
      // Input handling
      //

      void onBlockLocalInputChanged();
      void onBlockRemoteInputChanged();
      void onLocalInputPriorityChanged();
      void onInactivityTimeoutUpdate();
      void updateCheckboxesState();

   protected:
      // Configuration
      ServerConfig *m_config;
      // Controls
      ::innate_subsystem::TextBox m_rfbPort;
      ::innate_subsystem::TextBox m_httpPort;
      ::innate_subsystem::TextBox m_pollingInterval;
      ::innate_subsystem::CheckBox m_useD3D;
      ::innate_subsystem::CheckBox m_useMirrorDriver;
      ::innate_subsystem::CheckBox m_enableFileTransfers;
      ::innate_subsystem::CheckBox m_removeWallpaper;
      ::innate_subsystem::CheckBox m_acceptRfbConnections;
      ::innate_subsystem::CheckBox m_acceptHttpConnections;
      ::innate_subsystem::CheckBox m_showTrayIcon;
      ::innate_subsystem::CheckBox m_connectToRdp;
      ::innate_subsystem::Control m_primaryPassword;
      ::innate_subsystem::Control m_readOnlyPassword;
      ::innate_subsystem::Control m_unsetPrimaryPassword;
      ::innate_subsystem::Control m_unsetReadOnlyPassword;
      ::innate_subsystem::CheckBox m_useAuthentication;
      ::innate_subsystem::SpinControl m_rfbPortSpin;
      ::innate_subsystem::SpinControl m_httpPortSpin;
      ::innate_subsystem::SpinControl m_pollingIntervalSpin;

      //
      // Begin of input handling members
      //

      ::innate_subsystem::CheckBox m_blockRemoteInput;
      ::innate_subsystem::CheckBox m_blockLocalInput;
      ::innate_subsystem::CheckBox m_localInputPriority;
      ::innate_subsystem::TextBox m_localInputPriorityTimeout;
      ::innate_subsystem::SpinControl m_inactivityTimeoutSpin;

      //
      // End of input handling members
      //

      // Error notifications
      ::innate_subsystem::Dialog *m_pdialogParent;

      // Primary password control.
      PasswordControl *m_ppControl;
      PasswordControl *m_vpControl;
   };
} // namespace remoting_node



