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
#include "subsystem/StringParser.h"
#include "subsystem/node/OperatingSystemApplication.h"
#include "innate_subsystem/Dialog.h"
#include "innate_subsystem/Control.h"
#include "innate_subsystem/TextBox.h"
#include "innate_subsystem/CheckBox.h"
#include "innate_subsystem/SpinControl.h"
#include "innate_subsystem/ComboBox.h"
#include "innate_subsystem/TrackBar.h"
#include "resource.h"

//#include "remoting/remoting_common/win_system/WindowsApplication.h"
namespace remoting_remoting
{
   class ConfigurationDialog : public ::innate_subsystem::Dialog
   {
   public:
      ConfigurationDialog();

      void setListenerOfUpdate(::subsystem::OperatingSystemApplicationInterface *application);

      //protected:
      bool onCommand(unsigned int controlID, unsigned int notificationID);
      void onLogLevelChange();
      void onOpenFolderButtonClick();
      bool onInitDialog();

      ::innate_subsystem::CheckBox  m_checkboxShowToolBars;
      ::innate_subsystem::CheckBox m_checkboxWarnAtSwitching;
      ::innate_subsystem::TextBox m_textboxNumberConn;
      ::innate_subsystem::SpinControl m_spincontrolNumConn;
      ::innate_subsystem::TextBox m_textboxReverseConn;
      ::innate_subsystem::SpinControl  m_spincontrolReverseConn;
      ::innate_subsystem::TextBox  m_textboxVerbLvl;
      ::innate_subsystem::SpinControl  m_spincontrolVerbLvl;
      ::innate_subsystem::TextBox  m_textboxLogging;
      ::innate_subsystem::Control  m_controlOpenLogDir;

      ::subsystem::OperatingSystemApplicationInterface *m_application;

      //private:
      void updateControlValues();
      bool isInputValid();
      bool testNum(::innate_subsystem::TextBoxInterface *tb, const ::scoped_string & scopedstrTbName);
      void onOkPressed();
   };
} // namespace remoting_remoting
