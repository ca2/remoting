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

#include "innate_subsystem/gui/Dialog.h"
#include "innate_subsystem/gui/CheckBox.h"
#include "innate_subsystem/gui/TextBox.h"
#include "innate_subsystem/gui/SpinControl.h"
#include "innate_subsystem/gui/BalloonTip.h"
#include "remoting/remoting/node_config/ServerConfig.h"
#include "PasswordControl.h"


namespace remoting_node
{


   class CLASS_DECL_REMOTING AdministrationConfigDialog:
   virtual public ::innate_subsystem::Dialog
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
      // ::innate_subsystem::Control event handlers
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
      ::innate_subsystem::Control m_openLogPathButton;
      ::innate_subsystem::Control m_setControlPasswordButton;
      ::innate_subsystem::Control m_unsetControlPasswordButton;
      ::innate_subsystem::CheckBox m_disconnectAction[3];
      ::innate_subsystem::::innate_subsystem::TextBox m_logLevel;
      ::innate_subsystem::TextBox m_logPathTB;
      ::innate_subsystem::CheckBox m_useControlAuth;
      ::innate_subsystem::CheckBox m_repeatControlAuth;
      ::innate_subsystem::CheckBox m_shared[5];
      ::innate_subsystem::CheckBox m_logForAllUsers;
      SpinControl m_logSpin;
      BaseDialog *m_parentDialog;

      PasswordControl *m_cpControl;

      friend class ConfigDialog;
   };
} // namespace remoting_node
