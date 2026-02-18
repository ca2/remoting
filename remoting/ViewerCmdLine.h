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
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//

#pragma once


#include "win_system/WinProcessCommandLine.h"
#include "client_config_lib/ViewerConfig.h"
#include "util/CommandLineFormatException.h"
#include "util/CommandLineFormatHelp.h"
#include "HelpDialog.h"
#include "ConnectionData.h"

class ViewerCmdLine {
public:
  ViewerCmdLine(ConnectionData *conData,
                ConnectionConfig *conConf,
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

  static const char HELP[];
  static const char HELP_SHORT[];
  static const char HELP_ARG[];
  static const char HELP_ARG_SHORT[];
  static const char HELP_ARG_QUESTION[];
  static const char OPTIONS_FILE[];
  static const char LISTEN[];
  static const char HOST[];
  static const char PORT[];
  static const char PASSWORD[];
  static const char DISPATCH_ID[];
  static const char SHOW_CONTROLS[];
  static const char VIEW_ONLY[];
  static const char USE_CLIPBOARD[];
  static const char SCALE[];
  static const char FULL_SCREEN[];
  static const char WARN_FULL_SCREEN[];
  static const char ENCODING[];
  static const char COPY_RECT[];
  static const char MOUSE_CURSOR[];
  static const char MOUSE_LOCAL[];
  static const char MOUSE_SWAP[];
  static const char JPEG_IMAGE_QUALITY[];
  static const char COMPRESSION_LEVEL[];
  static const char LOG_PATH[];


  static const char YES[];
  static const char NO[];
  static const char AUTO[];
  static const char LOCAL[];
  static const char SMALLDOT[];
  static const char NORMAL[];
  static const char HEXTILE[];
  static const char TIGHT[];
  static const char RRE[];
  static const char ZRLE[];

  WinProcessCommandLine m_wpcl;
  ConnectionData *m_conData;
  ConnectionConfig *m_conConf;
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


