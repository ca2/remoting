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


#include "subsystem_apex/BaseDialog.h"
#include "subsystem_apex/TextBox.h"
#include "subsystem_apex/CheckBox.h"
#include "subsystem_apex/SpinControl.h"
#include "subsystem_apex/BalloonTip.h"

#include "remoting/remoting_common/server_config/ServerConfig.h"
#include "PasswordControl.h"

class ServerConfigDialog : public BaseDialog
{
public:
  ServerConfigDialog();
  virtual ~ServerConfigDialog();

  void setParentDialog(BaseDialog *dialog);

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
  void onPollingIntervalSpinChangePos(LPNMUPDOWN scopedstrMessage);
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
  TextBox m_rfbPort;
  TextBox m_httpPort;
  TextBox m_pollingInterval;
  CheckBox m_useD3D;
  CheckBox m_useMirrorDriver;
  CheckBox m_enableFileTransfers;
  CheckBox m_removeWallpaper;
  CheckBox m_acceptRfbConnections;
  CheckBox m_acceptHttpConnections;
  CheckBox m_showTrayIcon;
  CheckBox m_connectToRdp;
  ::subsystem_apex::Control m_primaryPassword;
  ::subsystem_apex::Control m_readOnlyPassword;
  ::subsystem_apex::Control m_unsetPrimaryPassword;
  ::subsystem_apex::Control m_unsetReadOnlyPassword;
  CheckBox m_useAuthentication;
  SpinControl m_rfbPortSpin;
  SpinControl m_httpPortSpin;
  SpinControl m_pollingIntervalSpin;

  //
  // Begin of input handling members
  //

  CheckBox m_blockRemoteInput;
  CheckBox m_blockLocalInput;
  CheckBox m_localInputPriority;
  TextBox m_localInputPriorityTimeout;
  SpinControl m_inactivityTimeoutSpin;

  //
  // End of input handling members
  //

  // Error notifications
  BaseDialog *m_parentDialog;

  // Primary password control.
  PasswordControl *m_ppControl;
  PasswordControl *m_vpControl;
};


