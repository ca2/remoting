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
#include "remoting/remoting_common/util/StringParser.h"
#include "apex/innate_subsystem/BaseDialog.h"
#include "apex/innate_subsystem/TextBox.h"
#include "apex/innate_subsystem/CheckBox.h"
#include "apex/innate_subsystem/ComboBox.h"
#include "apex/innate_subsystem/TrackBar.h"
#include "resource.h"


namespace remoting_remoting
{
    class OptionsDialog : public BaseDialog
    {
    public:
        OptionsDialog();

        void setConnectionConfig(ConnectionConfig *conConfig);
        void setConnected();

    protected:
        ComboBox m_useEnc;
        CheckBox m_eightBit;
        CheckBox m_compLvl;
        Trackbar m_tcompLvl;
        ::remoting::Window m_quality;
        CheckBox m_jpeg;
        Trackbar m_tjpeg;
        ::remoting::Window m_quality2;
        CheckBox m_copyrect;
        CheckBox m_viewonly;
        CheckBox m_disclip;
        CheckBox m_sharedses;
        ComboBox m_scale;
        CheckBox m_fullscr;
        CheckBox m_deiconfy;
        CheckBox m_swapmouse;
        CheckBox m_track;       // Track remote control locally
        CheckBox m_cursor;      // Let remote server deal with mouse cursor
        CheckBox m_ncursor;     // Don't show remote cursor
        CheckBox m_dot;
        CheckBox m_smalldot;
        CheckBox m_arrow;
        CheckBox m_nlocal;

        void onMessageReceived(unsigned int uMsg, ::wparam wParam, ::lparam lParam);
        bool onCommand(unsigned int controlID, unsigned int notificationID);
        bool onInitDialog();

        ConnectionConfig *m_conConfig;
        bool m_connected;

    private:
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
