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

//#ifndef _ADMINISTRATION_CONFIG_DIALOG_H
#pragma once
//#define _ADMINISTRATION_CONFIG_DIALOG_H

#include "innate_subsystem/BaseDialog.h"
#include "innate_subsystem/CheckBox.h"
#include "innate_subsystem/TextBox.h"
#include "innate_subsystem/SpinControl.h"
#include "innate_subsystem/BalloonTip.h"
#include "remoting/remoting_common/server_config/ServerConfig.h"
#include "PasswordControl.h"

class AdministrationConfigDialog: public BaseDialog
{
public:
  AdministrationConfigDialog();
  virtual ~AdministrationConfigDialog();
public:

  void setParentDialog(BaseDialog *dialog);

  //
  // BaseDialog overrided methods
  //

  virtual bool onInitDialog();
  virtual bool onCommand(unsigned int controlID, unsigned int notificationID);
  virtual bool onDestroy() { return true; }
  virtual bool onNotify(unsigned int controlID, ::lparam data) { return true; }

  //
  // Helper methods
  //

  bool validateInput();
  void updateUI();
  void apply();

private:
  void initControls();

  //
  // ::subsystem_apex::Control event handlers
  //

  void onShareRadioButtonClick(int number);
  void onOpenFolderButtonClick();
  void onLogLevelUpdate();
  void onDARadioButtonClick(int number);
  void onLogForAllUsersClick();
  void onUseControlAuthClick();
  void onRepeatControlAuthClick();
  void onChangeControlPasswordClick();
  void onUnsetControlPasswordClick();

  //
  // Helper methods
  //

  void getFolderName(const ::scoped_string & scopedstrKey, ::string & folder);

protected:
  // Configuration
  ServerConfig *m_config;
  // Controls
  ::subsystem_apex::Control m_openLogPathButton;
  ::subsystem_apex::Control m_setControlPasswordButton;
  ::subsystem_apex::Control m_unsetControlPasswordButton;
  CheckBox m_disconnectAction[3];
  TextBox m_logLevel;
  TextBox m_logPathTB;
  CheckBox m_useControlAuth;
  CheckBox m_repeatControlAuth;
  CheckBox m_shared[5];
  CheckBox m_logForAllUsers;
  SpinControl m_logSpin;
  BaseDialog *m_parentDialog;

  PasswordControl *m_cpControl;

  friend class ConfigDialog;
};


