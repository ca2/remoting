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
#include "apex/innate_subsystem/Dialog.h"
#include "apex/innate_subsystem/TextBox.h"
#include "apex/innate_subsystem/CheckBox.h"
#include "apex/innate_subsystem/ComboBox.h"
#include "apex/innate_subsystem/TrackBar.h"
#include "resource.h"


namespace remoting_remoting
{


    class OptionsDialog : public ::innate_subsystem::Dialog
    {
    public:

    //protected:
        ::pointer < ::innate_subsystem::ComboBoxInterface >       m_pcomboboxUseEnc;
        ::pointer < ::innate_subsystem::CheckBoxInterface >       m_pcheckboxEightBit;
        ::pointer < ::innate_subsystem::CheckBoxInterface >       m_pcheckboxCompressionLevel;
        ::pointer < ::innate_subsystem::TrackbarInterface >       m_ptrackbarCompressionLevel;
        ::pointer < ::innate_subsystem::ControlInterface >        m_pcontrolQuality;
        ::pointer < ::innate_subsystem::CheckBoxInterface >       m_pcheckboxJpeg;
        ::pointer < ::innate_subsystem::TrackbarInterface >       m_ptrackbarJpeg;
        ::pointer < ::innate_subsystem::ControlInterface >        m_pcontrolQuality2;
        ::pointer < ::innate_subsystem::CheckBoxInterface >       m_pcheckboxCopyrect;
        ::pointer < ::innate_subsystem::CheckBoxInterface >       m_pcheckboxViewonly;
        ::pointer < ::innate_subsystem::CheckBoxInterface >       m_pcheckboxClipboard;
        ::pointer < ::innate_subsystem::CheckBoxInterface >       m_pcheckboxSharedses;
        ::pointer < ::innate_subsystem::ComboBoxInterface >       m_pcheckboxScale;
        ::pointer < ::innate_subsystem::CheckBoxInterface >       m_pcheckboxFullscr;
        ::pointer < ::innate_subsystem::CheckBoxInterface >       m_pcheckboxDeiconfy;
        ::pointer < ::innate_subsystem::CheckBoxInterface >       m_pcheckboxSwapmouse;
        ::pointer < ::innate_subsystem::CheckBoxInterface >       m_pcheckboxTrack;       // Track remote control locally
        ::pointer < ::innate_subsystem::CheckBoxInterface >       m_pcheckboxCursor;      // Let remote server deal with mouse cursor
        ::pointer < ::innate_subsystem::CheckBoxInterface >       m_pcheckboxNcursor;     // Don't show remote cursor
        ::pointer < ::innate_subsystem::CheckBoxInterface >       m_pcheckboxDot;
        ::pointer < ::innate_subsystem::CheckBoxInterface >       m_pcheckboxSmalldot;
        ::pointer < ::innate_subsystem::CheckBoxInterface >       m_pcheckboxArrow;
        ::pointer < ::innate_subsystem::CheckBoxInterface >       m_pcheckboxNlocal;

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
