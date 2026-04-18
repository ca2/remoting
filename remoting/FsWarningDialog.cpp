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
#include "framework.h"
#include "FsWarningDialog.h"
#include "innate_subsystem/gui/CheckBox.h"
#include "remoting/remoting/remoting.h"
#include "remoting/remoting_common/client_config/ViewerConfig.h"
#include "remoting/remoting_common/client_config/ViewerSettingsManager.h"

namespace remoting_remoting
{
    FsWarningDialog::FsWarningDialog(::remoting_remoting::remoting * premoting) : m_premoting(premoting)
    {
         initialize_dialog(IDD_FS_WARNING);
    }

    bool FsWarningDialog::onInitDialog()
    {
        dialog_item(m_checkboxFsWarning, IDC_CWARN_FS);

        m_checkboxFsWarning.setChecked(false);
        return false;
    }

   
   bool FsWarningDialog::onCommand(unsigned int controlID, unsigned int notificationID)
   {
        
      if (controlID == ::innate_subsystem::e_control_id_ok) 
      {
           
         auto pviewerconfig = m_premoting->m_pviewerconfig;
         bool bPrompt = !m_checkboxFsWarning.isChecked();
         pviewerconfig->promptOnFullscreen(bPrompt);
         pviewerconfig->saveToStorage(::remoting::ViewerSettingsManager::getInstance());
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


} // namespace remoting_remoting


