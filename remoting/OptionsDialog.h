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
#include "innate_subsystem/gui/Dialog.h"
#include "innate_subsystem/gui/TextBox.h"
#include "innate_subsystem/gui/CheckBox.h"
#include "innate_subsystem/gui/ComboBox.h"
#include "innate_subsystem/gui/TrackBar.h"
#include "resource.h"


namespace remoting_remoting
{


    class OptionsDialog : public ::innate_subsystem::Dialog
    {
    public:

    //protected:
        ::innate_subsystem::ComboBox       m_comboboxUseEnc;
        ::innate_subsystem::CheckBox       m_checkboxEightBit;
        ::innate_subsystem::CheckBox       m_checkboxCompressionLevel;
        ::innate_subsystem::Trackbar       m_trackbarCompressionLevel;
        ::innate_subsystem::Control        m_controlQuality;
        ::innate_subsystem::CheckBox       m_checkboxJpeg;
        ::innate_subsystem::Trackbar       m_trackbarJpeg;
        ::innate_subsystem::Control        m_controlQuality2;
        ::innate_subsystem::CheckBox       m_checkboxCopyrect;
        ::innate_subsystem::CheckBox       m_checkboxViewonly;
        ::innate_subsystem::CheckBox       m_checkboxClipboard;
        ::innate_subsystem::CheckBox       m_checkboxSharedses;
        ::innate_subsystem::ComboBox       m_checkboxScale;
        ::innate_subsystem::CheckBox       m_checkboxFullscr;
        ::innate_subsystem::CheckBox       m_checkboxDeiconfy;
        ::innate_subsystem::CheckBox       m_checkboxSwapmouse;
        ::innate_subsystem::CheckBox       m_checkboxTrack;       // Track remote control locally
        ::innate_subsystem::CheckBox       m_checkboxCursor;      // Let remote server deal with mouse cursor
        ::innate_subsystem::CheckBox       m_checkboxNcursor;     // Don't show remote cursor
        ::innate_subsystem::CheckBox       m_checkboxDot;
        ::innate_subsystem::CheckBox       m_checkboxSmalldot;
        ::innate_subsystem::CheckBox       m_checkboxArrow;
        ::innate_subsystem::CheckBox       m_checkboxNlocal;

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
