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
#include "acme/subsystem/StringParser.h"
#include "apex/innate_subsystem/BaseDialog.h"
#include "apex/innate_subsystem/Control.h"
#include "apex/innate_subsystem/TextBox.h"
#include "apex/innate_subsystem/CheckBox.h"
#include "apex/innate_subsystem/SpinControl.h"
#include "apex/innate_subsystem/ComboBox.h"
#include "apex/innate_subsystem/TrackBar.h"
#include "resource.h"

//#include "remoting/remoting_common/win_system/WindowsApplication.h"
namespace remoting_remoting
{
   class ConfigurationDialog : public ::innate_subsystem::BaseDialog
   {
   public:
      ConfigurationDialog();

      void setListenerOfUpdate(::subsystem::OperatingSystemApplicationInterface *application);

      //protected:
      bool onCommand(unsigned int controlID, unsigned int notificationID);
      void onLogLevelChange();
      void onOpenFolderButtonClick();
      bool onInitDialog();

      ::pointer < ::innate_subsystem::CheckBox> m_pcheckboxShowToolBars;
      ::pointer < ::innate_subsystem::CheckBox m_pcheckboxWarnAtSwitching;
      ::pointer < ::innate_subsystem::TextBox m_ptextboxNumberConn;
      ::pointer < ::innate_subsystem::SpinControl m_pspincontrolNumConn;
      ::pointer < ::innate_subsystem::TextBox m_ptextboxReverseConn;
      ::pointer < ::innate_subsystem::SpinControl > m_pspincontrolReverseConn;
      ::pointer < ::innate_subsystem::TextBox > m_ptextboxVerbLvl;
      ::pointer < ::innate_subsystem::SpinControl > m_pspincontrolVerbLvl;
      ::pointer < ::innate_subsystem::TextBox > m_ptextboxLogging;
      ::pointer < ::innate_subsystem::Control > m_pcontrolOpenLogDir;

      ::subsystem::OperatingSystemApplicationInterface *m_application;

      //private:
      void updateControlValues();
      bool isInputValid();
      bool testNum(::innate_subsystem::TextBox *tb, const ::scoped_string & scopedstrTbName);
      void onOkPressed();
   };
} // namespace remoting_remoting
