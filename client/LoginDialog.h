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


#include "remoting/remoting/client_config/ViewerConfig.h"
#include "remoting/remoting/client_config/ConnectionConfig.h"
#include "remoting/remoting/client_config/ConnectionConfigSM.h"
#include "remoting/remoting/client_config/ViewerSettingsManager.h"
#include "subsystem/platform/StringParser.h"
#include "innate_subsystem/gui/Dialog.h"
#include "innate_subsystem/gui/TextBox.h"
#include "innate_subsystem/gui/CheckBox.h"
#include "innate_subsystem/gui/ComboBox.h"
#include "innate_subsystem/gui/Trackbar.h"

#include "remoting_impact.h"
#include "resource.h"

namespace remoting_client
{
    class remoting_impact;
   class remoting;

    class LoginDialog : public ::innate_subsystem::Dialog
    {
    public:

       ::pointer<::remoting_client::remoting> m_premoting;

       LoginDialog(remoting_impact *viewer, ::remoting_client::remoting *premoting);
        ~LoginDialog();

        // this function returns the host
        ::string getServerHost();
        void setConConf(::remoting::ConnectionConfig *pconnectionconfig);
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
        bool onInitDialog() override;
        bool onCommand(::u32 controlID, ::u32 notificationID) override;
        bool m_isListening;

        ::innate_subsystem::Control m_listening;
        ::innate_subsystem::Control m_ok;
        ::pointer < ::remoting::ConnectionConfig > m_pconnectionconfig;
        ::innate_subsystem::ComboBox m_server;
        ::string m_serverHost;
        ::pointer < remoting_impact > m_premotingimpact;

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
} // namespace remoting_client
