// Copyright (C) 2012 GlavSoft LLC.
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


#include "remoting/remoting_common/client_config/ViewerConfig.h"
#include "remoting/remoting_common/client_config/ConnectionConfig.h"
#include "remoting/remoting_common/client_config/ConnectionConfigSM.h"
#include "remoting/remoting_common/client_config/ViewerSettingsManager.h"
#include "subsystem_acme/StringParser.h"
#include "subsystem_acme/node/OperatingSystemApplication.h"
#include "subsystem_apex/Dialog.h"
#include "subsystem_apex/Control.h"
#include "subsystem_apex/TextBox.h"
#include "subsystem_apex/CheckBox.h"
#include "subsystem_apex/SpinControl.h"
#include "subsystem_apex/ComboBox.h"
#include "subsystem_apex/TrackBar.h"
#include "resource.h"

//#include "remoting/remoting_common/win_system/WindowsApplication.h"
namespace remoting_remoting
{
   class ConfigurationDialog : public ::subsystem_apex::Dialog
   {
   public:
      ConfigurationDialog();

      void setListenerOfUpdate(::subsystem::OperatingSystemApplicationInterface *application);

      //protected:
      bool onCommand(unsigned int controlID, unsigned int notificationID);
      void onLogLevelChange();
      void onOpenFolderButtonClick();
      bool onInitDialog();

      ::subsystem_apex::CheckBox  m_checkboxShowToolBars;
      ::subsystem_apex::CheckBox m_checkboxWarnAtSwitching;
      ::subsystem_apex::TextBox m_textboxNumberConn;
      ::subsystem_apex::SpinControl m_spincontrolNumConn;
      ::subsystem_apex::TextBox m_textboxReverseConn;
      ::subsystem_apex::SpinControl  m_spincontrolReverseConn;
      ::subsystem_apex::TextBox  m_textboxVerbLvl;
      ::subsystem_apex::SpinControl  m_spincontrolVerbLvl;
      ::subsystem_apex::TextBox  m_textboxLogging;
      ::subsystem_apex::Control  m_controlOpenLogDir;

      ::subsystem::OperatingSystemApplicationInterface *m_application;

      //private:
      void updateControlValues();
      bool isInputValid();
      bool testNum(::subsystem_apex::TextBoxInterface *tb, const ::scoped_string & scopedstrTbName);
      void onOkPressed();
   };
} // namespace remoting_remoting
