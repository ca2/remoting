// Copyright (C) 2011,2012 GlavSoft LLC.
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
#include "apex/innate_subsystem/Dialog.h"
#include "apex/innate_subsystem/TextBox.h"
#include "apex/innate_subsystem/CheckBox.h"
#include "apex/innate_subsystem/ComboBox.h"
#include "apex/innate_subsystem/TrackBar.h"

#include "remoting_impact.h"
#include "resource.h"

namespace remoting_remoting
{
    class remoting_impact;

    class LoginDialog : public ::innate_subsystem::Dialog
    {
    public:
        LoginDialog(remoting_impact *viewer);
        ~LoginDialog();

        // this function returns the host
        ::string getServerHost();
        void setConConf(::remoting::ConnectionConfig *conConf);
        // set listening mode
        void setListening(bool isListening);

        static const int DEFAULT_PORT = 5900;

        // this constant will be returned if user have pressed 'Connect...' button
        static const int CANCEL_MODE = 0;
        // this constant will be returned if user have pressed 'Cancel' button
        static const int CONNECTION_MODE = 1;
        // this constant will be returned if user have pressed 'Listening' button
        static const int LISTENING_MODE = 2;

    protected:
        bool onInitDialog();
        bool onCommand(unsigned int controlID, unsigned int notificationID);
        bool m_isListening;

        ::innate_subsystem::Control m_listening;
        ::innate_subsystem::Control m_ok;
        ::remoting::ConnectionConfig m_connectionConfig;
        ::innate_subsystem::ComboBox m_server;
        ::string m_serverHost;
        remoting_impact *m_viewer;

    private:
        void enableConnect();
        void updateHistory();
        void onConnect();
        void onConfiguration();
        bool onOptions();
        void onOrder();
        void openUrl(const ::scoped_string & scopedstrUrl);
        void onListening();
        void onAbout();
    };
} // namespace remoting_remoting
