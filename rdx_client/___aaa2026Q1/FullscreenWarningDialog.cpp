// Copyright (C) 2012 GlavSoft LLC.
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
#include "../FullscreenWarningDialog.h"
#include "../framework.h"
#include "innate_subsystem/gui/CheckBox.h"
#include "remoting/client/remoting.h"
#include "remoting/remoting_rtc/client_config/ViewerConfig.h"
#include "remoting/remoting_rtc/client_config/ViewerSettingsManager.h"

namespace remoting_client
{
    FullscreenWarningDialog::FullscreenWarningDialog(::remoting_client::remoting * premoting) : m_premoting(premoting)
    {
         initialize_dialog(IDD_FS_WARNING);
    }

FullscreenWarningDialog::~FullscreenWarningDialog()
{
   
   
}

    bool FullscreenWarningDialog::onInitDialog()
    {
        dialog_item(m_checkboxFullScreenWarning, IDC_CWARN_FS);

        m_checkboxFullScreenWarning.setChecked(false);
        return false;
    }

   
   bool FullscreenWarningDialog::onCommand(::u32 controlID, ::u32 notificationID)
   {
        
      if (controlID == ::innate_subsystem::e_control_id_ok) 
      {
           
         auto pviewerconfig = m_premoting->m_pviewerconfig;
         bool bPrompt = !m_checkboxFullScreenWarning.isChecked();
         pviewerconfig->promptOnFullscreen(bPrompt);
         pviewerconfig->saveToStorage(::remoting_rtc::ViewerSettingsManager::getInstance());
         closeDialog(1);
         return true;

      }
      
      if (controlID == ::innate_subsystem::e_control_id_cancel) 
      {
      
         closeDialog(0);
         return true;

      }

      return false;

    }


} // namespace remoting_client


