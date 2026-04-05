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
#include "remoting/remoting_common/client_config/ViewerConfig.h"
#include "remoting/remoting_common/client_config/ViewerSettingsManager.h"

namespace remoting_remoting
{
    FsWarningDialog::FsWarningDialog()
    {
         initialize_base_dialog(IDD_FS_WARNING);
    }

    bool FsWarningDialog::onInitDialog()
    {
        setControlById(m_pcheckboxFsWarning, IDC_CWARN_FS);

        m_pcheckboxFsWarning->check(false);
        return false;
    }

    bool FsWarningDialog::onCommand(unsigned int controlID, unsigned int notificationID)
    {
        if (controlID == ::innate_subsystem::IDOK) {
            ::remoting::ViewerConfig *config = ::remoting::ViewerConfig::getInstance();
            bool promt = !m_pcheckboxFsWarning->isChecked();
            config->promptOnFullscreen(promt);
            config->saveToStorage(::remoting::ViewerSettingsManager::getInstance());
            closeDialog(1);
            return true;
        }
        if (controlID == ::innate_subsystem::IDCANCEL) {
            closeDialog(0);
            return true;
        }
        return false;
    }
} // namespace remoting_remoting


