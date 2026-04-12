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
#include "innate_subsystem/Dialog.h"
#include "innate_subsystem/TextBox.h"
#include "innate_subsystem/CheckBox.h"
#include "innate_subsystem/ComboBox.h"
#include "innate_subsystem/TrackBar.h"
#include "resource.h"


namespace remoting_remoting
{


    class OptionsDialog : public ::subsystem_apex::Dialog
    {
    public:

    //protected:
        ::pointer < ::subsystem_apex::ComboBoxInterface >       m_pcomboboxUseEnc;
        ::pointer < ::subsystem_apex::CheckBoxInterface >       m_pcheckboxEightBit;
        ::pointer < ::subsystem_apex::CheckBoxInterface >       m_pcheckboxCompressionLevel;
        ::pointer < ::subsystem_apex::TrackbarInterface >       m_ptrackbarCompressionLevel;
        ::pointer < ::subsystem_apex::ControlInterface >        m_pcontrolQuality;
        ::pointer < ::subsystem_apex::CheckBoxInterface >       m_pcheckboxJpeg;
        ::pointer < ::subsystem_apex::TrackbarInterface >       m_ptrackbarJpeg;
        ::pointer < ::subsystem_apex::ControlInterface >        m_pcontrolQuality2;
        ::pointer < ::subsystem_apex::CheckBoxInterface >       m_pcheckboxCopyrect;
        ::pointer < ::subsystem_apex::CheckBoxInterface >       m_pcheckboxViewonly;
        ::pointer < ::subsystem_apex::CheckBoxInterface >       m_pcheckboxClipboard;
        ::pointer < ::subsystem_apex::CheckBoxInterface >       m_pcheckboxSharedses;
        ::pointer < ::subsystem_apex::ComboBoxInterface >       m_pcheckboxScale;
        ::pointer < ::subsystem_apex::CheckBoxInterface >       m_pcheckboxFullscr;
        ::pointer < ::subsystem_apex::CheckBoxInterface >       m_pcheckboxDeiconfy;
        ::pointer < ::subsystem_apex::CheckBoxInterface >       m_pcheckboxSwapmouse;
        ::pointer < ::subsystem_apex::CheckBoxInterface >       m_pcheckboxTrack;       // Track remote control locally
        ::pointer < ::subsystem_apex::CheckBoxInterface >       m_pcheckboxCursor;      // Let remote server deal with mouse cursor
        ::pointer < ::subsystem_apex::CheckBoxInterface >       m_pcheckboxNcursor;     // Don't show remote cursor
        ::pointer < ::subsystem_apex::CheckBoxInterface >       m_pcheckboxDot;
        ::pointer < ::subsystem_apex::CheckBoxInterface >       m_pcheckboxSmalldot;
        ::pointer < ::subsystem_apex::CheckBoxInterface >       m_pcheckboxArrow;
        ::pointer < ::subsystem_apex::CheckBoxInterface >       m_pcheckboxNlocal;

        void onMessageReceived(unsigned int uMsg, ::wparam wParam, ::lparam lParam);
        bool onCommand(unsigned int controlID, unsigned int notificationID);
        bool onInitDialog();

        ::pointer < ::remoting::ConnectionConfig  >m_pconnectionconfig;



        bool m_connected;



               OptionsDialog();

        void setConnectionConfig(::remoting::ConnectionConfig *conConfig);
        void setConnected();

    //private:
        void updateControlValues();
        void onViewOnlyClick();
        void on8BitColorClick();
        void enableJpegCompression(bool enable);
        void onAllowCustomCompressionClick();
        void enableCustomCompression(bool enable);
        void onAllowJpegCompressionClick();
        void onPreferredEncodingSelectionChange();
        void onCustomCompressionLevelScroll();
        void onJpegCompressionLevelScroll();
        void onScaleKillFocus();
        bool isInputValid();
        bool onOkPressed();
        void apply();

    };
} // namespace remoting_remoting
