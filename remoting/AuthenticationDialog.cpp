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
#include "AuthenticationDialog.h"
//#include "acme/_operating_system.h"


namespace remoting_remoting
{


    AuthenticationDialog::AuthenticationDialog()
    {
        initialize_dialog(IDD_DAUTH);
    }


    bool AuthenticationDialog::onInitDialog()
    {
        subclassControlById(m_ptextboxHostname, IDC_EHOST);
        m_ptextboxHostname->setText(m_strHost);
        subclassControlById(m_ptextboxPassword, IDC_EPASSW);
        m_ptextboxPassword->setFocus();
        return false;
    }


    void AuthenticationDialog::setHostName(const ::scoped_string & hostname)
    {
        m_strHost = hostname;
    }

    bool AuthenticationDialog::onCommand(unsigned int controlID, unsigned int notificationID)
    {
        if (controlID == ::innate_subsystem::IDOK) {
            m_strPassword = m_ptextboxPassword->getText();
            closeDialog(1);
            return true;
        }
        if (controlID == ::innate_subsystem::IDCANCEL) {
            closeDialog(0);
            return true;
        }
        return false;
    }

    string AuthenticationDialog::getPassword()
    {
        return m_strPassword;
    }


} // namespace remoting_remoting


