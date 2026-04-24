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


//#include "remoting/remoting/win_system/WindowsProcessCommandLine.h"
#include "remoting/remoting/client_config/ViewerConfig.h"
#include "subsystem/platform/CommandLineFormatException.h"
#include "subsystem/platform/CommandLineFormatHelp.h"
#include "HelpDialog.h"
#include "ConnectionData.h"

namespace remoting_client
{
    class ViewerCmdLine {
    public:
        ViewerCmdLine(ConnectionData *pconnectiondata,
                      ::remoting::ConnectionConfig *pconnectionconfig,
                      ::remoting::ViewerConfig *config,
                      bool *isListening);

        // this function parse the command line
        void parse();

        // this function creates the modal dialog with text how to use it
        void onHelp();

    protected:
        void parseHostArg();
        bool parseHostOptions();
        bool isPresent(const ::scoped_string & scopedstrKeyName);

        static ::string_literal HELP;
        static ::string_literal HELP_SHORT;
        static ::string_literal HELP_ARG;
        static ::string_literal HELP_ARG_SHORT;
        static ::string_literal HELP_ARG_QUESTION;
        static ::string_literal OPTIONS_FILE;
        static ::string_literal LISTEN;
        static ::string_literal HOST;
        static ::string_literal PORT;
        static ::string_literal PASSWORD;
        static ::string_literal DISPATCH_ID;
        static ::string_literal SHOW_CONTROLS;
        static ::string_literal VIEW_ONLY;
        static ::string_literal USE_CLIPBOARD;
        static ::string_literal SCALE;
        static ::string_literal FULL_SCREEN;
        static ::string_literal WARN_FULL_SCREEN;
        static ::string_literal ENCODING;
        static ::string_literal COPY_RECT;
        static ::string_literal MOUSE_CURSOR;
        static ::string_literal MOUSE_LOCAL;
        static ::string_literal MOUSE_SWAP;
        static ::string_literal JPEG_IMAGE_QUALITY;
        static ::string_literal COMPRESSION_LEVEL;
        static ::string_literal LOG_PATH;


        static ::string_literal YES;
        static ::string_literal NO;
        static ::string_literal AUTO;
        static ::string_literal LOCAL;
        static ::string_literal SMALLDOT;
        static ::string_literal NORMAL;
        static ::string_literal HEXTILE;
        static ::string_literal TIGHT;
        static ::string_literal RRE;
        static ::string_literal ZRLE;

        //::pointer < ::windows::WindowsProcessCommandLine > m_pwindowsprocesscommandline;
       ::pointer < ::subsystem::ProcessCommandLineInterface > m_pprocesscommandlineOperatingSystem;
        ConnectionData *m_pconnectiondata;
        ::remoting::ConnectionConfig *m_pconnectionconfig;
        ::remoting::ViewerConfig *m_config;
        bool *m_isListening;

        ::map<::string, ::string> m_options;

    private:
        struct CmdLineOption
        {
            const_char_pointer keyName;
        };

        bool processCmdLine(const CmdLineOption * cmdLines,
                            size_t lenCmdLineOption);

        bool isHelpPresent();
        void parseOptionsFile();
        void parsePassword();
        void parseDispatchId();
        void parseEncoding();
        void parseMouseShape();
        void parseMouseCursor();
        void parseScale();
        void parseFullScreen();
        void parseCompressionLevel();
        void parseWarnFullScr();
        void parseMouseSwap();
        void parseUseClipboard();
        void parseShowControl();
        void parseCopyRect();
        void parseViewOnly();
        void parseJpegImageQuality();
        bool parseHost();
        void parseLogPath();
    };
} // namespace remoting_client
