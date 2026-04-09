// Copyright (C) 2013 GlavSoft LLC.
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
#include "ViewerVncAuthHandler.h"
#include "AuthenticationDialog.h"
//// #include aaa_<windows.h>
//// #include aaa_<wincrypt.h>

#include "acme/filesystem/filesystem/directory_context.h"
#include "acme/filesystem/filesystem/file_context.h"

//#pragma comment (lib, "Crypt32.lib")
namespace remoting_remoting
{

    ViewerVncAuthHandler::ViewerVncAuthHandler(ConnectionData *connectionData)
    : m_connectionData(connectionData)
    {
    }

    ViewerVncAuthHandler::~ViewerVncAuthHandler()
    {
    }



    ::string ViewerVncAuthHandler::getPassword()
    {
        ::string passString;

        // get password from ConnectionData or User Interface
        if (!m_connectionData->isSetPassword()) {
            AuthenticationDialog authDialog;
            ::string hostname = m_connectionData->getHost();
            authDialog.setHostName(hostname);
            auto m = ::system()->file()->safe_get_memory(::system()->directory()->appdata()/::string(hostname));
            if (m.has_data())
            {
                ::string str;
                main_subsystem()->DecryptData(m, str);
                m_connectionData->setPlainPassword(::wstring(str).c_str());
            }
            else
            {
                if (authDialog.showModal()) {
                    m_connectionData->setPlainPassword(authDialog.getPassword());
                    memory m2;
                    ::string str(authDialog.getPassword());
                    main_subsystem()->EncryptData(str, m2);
                    if (m2.has_data())
                    {

                        ::system()->file()->put_memory(::system()->directory()->appdata()/::string(hostname), m2);
                    }
                } else {
                    throw ::remoting::AuthCanceledException();
                }
            }
        }
        passString = m_connectionData->getPlainPassword();

        return passString;
    }
} // namespace remoting_remoting
