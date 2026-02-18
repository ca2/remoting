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
#include "framework.h"
#include "ViewerCmdLine.h"
#include "config_lib/IniFileSettingsManager.h"
#include "win_system/SystemException.h"

const char ViewerCmdLine::HELP[] = "help";
const char ViewerCmdLine::HELP_SHORT[] = "h";
const char ViewerCmdLine::HELP_ARG[] = "/help";
const char ViewerCmdLine::HELP_ARG_SHORT[] = "/h";
const char ViewerCmdLine::HELP_ARG_QUESTION[] = "/?";

const char ViewerCmdLine::OPTIONS_FILE[] = "optionsfile";
const char ViewerCmdLine::LISTEN[] = "listen";
const char ViewerCmdLine::HOST[] = "host";
const char ViewerCmdLine::PORT[] = "port";
const char ViewerCmdLine::PASSWORD[] = "password";
const char ViewerCmdLine::SHOW_CONTROLS[] = "showcontrols";
const char ViewerCmdLine::VIEW_ONLY[] = "viewonly";
const char ViewerCmdLine::USE_CLIPBOARD[] = "useclipboard";
const char ViewerCmdLine::SCALE[] = "scale";
const char ViewerCmdLine::FULL_SCREEN[] = "fullscreen";
const char ViewerCmdLine::WARN_FULL_SCREEN[] = "warnfullscr";
const char ViewerCmdLine::ENCODING[] = "encoding";
const char ViewerCmdLine::COPY_RECT[] = "copyrect";
const char ViewerCmdLine::MOUSE_CURSOR[] = "mousecursor";
const char ViewerCmdLine::MOUSE_LOCAL[] = "mouselocal";
const char ViewerCmdLine::MOUSE_SWAP[] = "mouseswap";
const char ViewerCmdLine::JPEG_IMAGE_QUALITY[] = "jpegimagequality";
const char ViewerCmdLine::COMPRESSION_LEVEL[] = "compressionlevel";
const char ViewerCmdLine::LOG_PATH[] = "logpath";


const char ViewerCmdLine::YES[] = "yes";
const char ViewerCmdLine::NO[] = "no";
const char ViewerCmdLine::AUTO[] = "auto";
const char ViewerCmdLine::LOCAL[] = "local";
const char ViewerCmdLine::SMALLDOT[] = "smalldot";
const char ViewerCmdLine::NORMAL[] = "normal";
const char ViewerCmdLine::HEXTILE[] = "hextile";
const char ViewerCmdLine::TIGHT[] = "tight";
const char ViewerCmdLine::RRE[] = "rre";
const char ViewerCmdLine::ZRLE[] = "zrle";

ViewerCmdLine::ViewerCmdLine(ConnectionData *conData,
                             ConnectionConfig *conConf,
                             ::remoting::ViewerConfig *config,
                             bool *isListening)
: m_conData(conData),
  m_conConf(conConf),
  m_config(config),
  m_isListening(isListening)
{
}

bool ViewerCmdLine::processCmdLine(const CmdLineOption *cmdLines, size_t lenCmdLineOption)
{
  if (m_wpcl.getOptionsCount()) {
    int countRecog = 0;

    for (size_t i = 0; i < lenCmdLineOption; i++) {
       ::string strOut;

       for (size_t j = 0; j < m_wpcl.getOptionsCount(); j++) {
         m_wpcl.getOption(j , strOut);

         if (strOut == cmdLines[i].keyName) {
           countRecog++;
         }
       }
       if (m_wpcl.findOptionValue(cmdLines[i].keyName, strOut)) {
         m_options[cmdLines[i].keyName] = strOut; 
       }
    }
    if (countRecog != m_wpcl.getOptionsCount()) {
      return false;
    }
  }
  return true;
}

void ViewerCmdLine::parse()
{
  const CmdLineOption options[] = {
    HELP,
    HELP_SHORT,
    OPTIONS_FILE,
    LISTEN,
    HOST,
    PORT,
    PASSWORD,
    SHOW_CONTROLS,
    VIEW_ONLY,
    USE_CLIPBOARD,
    SCALE,
    FULL_SCREEN,
    WARN_FULL_SCREEN,
    ENCODING,
    COPY_RECT,
    MOUSE_CURSOR,
    MOUSE_LOCAL,
    MOUSE_SWAP,
    JPEG_IMAGE_QUALITY,
    COMPRESSION_LEVEL,
    LOG_PATH
  };

  if (!processCmdLine(&options[0], sizeof(options) / sizeof(CmdLineOption))) {
    throw CommandLineFormatException(StringTable::getString(IDS_ERROR_COMMAND_LINE));
  }

  // If options "help" is present, then show "Help dialog" and exit.
  if (isHelpPresent()) {
    throw CommandLineFormatHelp();
  }


  if (m_wpcl.getArgumentsCount() > 2) {
    throw CommandLineFormatException(StringTable::getString(IDS_ERROR_COMMAND_LINE));
  }

  if (m_wpcl.getArgumentsCount() > 1) {
    if (isPresent(ViewerCmdLine::HOST)) {
      throw CommandLineFormatException(StringTable::getString(IDS_ERROR_COMMAND_LINE));
    }
  }

  if (isPresent(ViewerCmdLine::OPTIONS_FILE)) {
    parseOptionsFile();
  } else if (isPresent(ViewerCmdLine::LISTEN)) {
      *m_isListening = true;
  } else if (isPresent(ViewerCmdLine::LOG_PATH)) {
    parseLogPath();
  } else if (!parseHost()) {
      throw CommandLineFormatException(StringTable::getString(IDS_ERROR_COMMAND_LINE));
  }
  parsePassword();
  parseEncoding();
  parseMouseShape();
  parseMouseCursor();
  parseScale();
  parseFullScreen();
  parseCompressionLevel();
  parseWarnFullScr();
  parseMouseSwap();
  parseUseClipboard();
  parseShowControl();
  parseCopyRect();
  parseViewOnly();
  parseJpegImageQuality();
}

void ViewerCmdLine::onHelp()
{
  HelpDialog hlpdlg;

  hlpdlg.showModal();
}

bool ViewerCmdLine::isHelpPresent()
{
  for (size_t i = 0; i < m_wpcl.getArgumentsCount(); i++) {
    ::string argument;
    if (m_wpcl.getArgument(i, argument)) {
      if (argument == HELP_ARG)
        return true;
      if (argument == HELP_ARG_SHORT)
        return true;
      if (argument == HELP_ARG_QUESTION)
        return true;
    }
  }

  if (isPresent(HELP))
    return true;
  if (isPresent(HELP_SHORT))
    return true;
  return false;
}

bool ViewerCmdLine::isPresent(const ::scoped_string & scopedstrKeyName)
{
  return m_options.find(scopedstrKeyName) != m_options.end();
}

void ViewerCmdLine::parseOptionsFile()
{
  ::string pathToIniFile = m_options[OPTIONS_FILE];
  if (::not_found(pathToIniFile.find_first_character('\\'))) {
    ::string newPathToIniFile;
    newPathToIniFile.format(".\\{}", pathToIniFile);
    pathToIniFile = newPathToIniFile;
  }
  IniFileSettingsManager sm(pathToIniFile);
  sm.setApplicationName("connection");

  ::string host;
  if (!sm.getString("host", host)) {
    throw CommandLineFormatException(StringTable::getString(IDS_ERROR_PARSE_OPTIONS_FILE));
  }
  ::string port;
  if (sm.getString("port", port)) {
    ::string hostString;
    hostString.format("{}::{}", host, port);
    m_conData->setHost(hostString);
  } else {
    m_conData->setHost(host);
  }

  ::string password;
  sm.getString("password", password);
  if (!password.is_empty()) {
    m_conData->setCryptedPassword(password);
  } else {
    parsePassword();
  }
  
  sm.setApplicationName("options");
  m_conConf->loadFromStorage(&sm);
}

void ViewerCmdLine::parsePassword()
{
  if (isPresent(PASSWORD)) {
    m_conData->setPlainPassword(m_options[PASSWORD]);
  }
}

void ViewerCmdLine::parseEncoding()
{
  if (isPresent(ENCODING)) {
    int iEncoding = EncodingDefs::RAW;

    if (m_options[ENCODING] == HEXTILE) {
      iEncoding = EncodingDefs::HEXTILE;
    }
    if (m_options[ENCODING] == TIGHT) {
      iEncoding = EncodingDefs::TIGHT;
    }
    if (m_options[ENCODING] == RRE) {
      iEncoding = EncodingDefs::RRE;
    }
    if (m_options[ENCODING] == ZRLE) {
      iEncoding = EncodingDefs::ZRLE;
    }
    m_conConf->setPreferredEncoding(iEncoding);
  }
}

void ViewerCmdLine::parseMouseShape() 
{
  if (isPresent(MOUSE_LOCAL)) {
    int localCursorShape = ConnectionConfig::DOT_CURSOR;

    if (m_options[MOUSE_LOCAL] == NO) {
      localCursorShape = ConnectionConfig::NO_CURSOR;
    }
    if (m_options[MOUSE_LOCAL] == SMALLDOT) {
      localCursorShape = ConnectionConfig::SMALL_CURSOR;
    }
    if (m_options[MOUSE_LOCAL] == NORMAL) {
      localCursorShape = ConnectionConfig::NORMAL_CURSOR;
    }
    m_conConf->setLocalCursorShape(localCursorShape);
  }
}

void ViewerCmdLine::parseMouseCursor()
{
  if (isPresent(MOUSE_CURSOR)) {
    m_conConf->requestShapeUpdates(false);
    m_conConf->ignoreShapeUpdates(false);
    if (m_options[MOUSE_CURSOR] == NO) {
      m_conConf->requestShapeUpdates(true);
      m_conConf->ignoreShapeUpdates(true);
    } else {
      if (m_options[MOUSE_CURSOR] == LOCAL) {
        m_conConf->requestShapeUpdates(true);
      }
    }
  }
}

void ViewerCmdLine::parseScale()
{
  if (isPresent(SCALE)) {
    if (m_options[SCALE] == AUTO) {
      m_conConf->fitWindow(true);
    } else {
      int scale = atoi(m_options[SCALE]);

      if (scale < 1) {
        scale = 1;
      }
      if (scale > 400) {
        scale = 400;
      }
      m_conConf->setScale(scale, 100);
      m_conConf->fitWindow(false);
    }
  }
}

void ViewerCmdLine::parseFullScreen()
{
  if (isPresent(FULL_SCREEN)) {
    bool isFullScreen = false;

    if (m_options[FULL_SCREEN] == YES) {
      isFullScreen = true;
    }
    m_conConf->enableFullscreen(isFullScreen);
  }
}

void ViewerCmdLine::parseWarnFullScr()
{
  if (isPresent(WARN_FULL_SCREEN)) {
    bool isWarn = false;

    if (m_options[WARN_FULL_SCREEN] == YES) {
      isWarn = true;
    }
    m_config->promptOnFullscreen(isWarn);
  }
}

void ViewerCmdLine::parseMouseSwap()
{
  if (isPresent(MOUSE_SWAP)) {
    bool isMouseSwap = false;

    if (m_options[MOUSE_SWAP] == YES) {
      isMouseSwap = true;
    }
    m_conConf->swapMouse(isMouseSwap);
  }
}

void ViewerCmdLine::parseUseClipboard()
{
  if (isPresent(USE_CLIPBOARD)) {
    bool isUseClipboard = false;

    if (m_options[USE_CLIPBOARD] == YES) {
      isUseClipboard = true;
    }
    m_conConf->enableClipboard(isUseClipboard);
  }
}

void ViewerCmdLine::parseShowControl()
{
  if (isPresent(SHOW_CONTROLS)) {
    bool isShowControl = false;

    if (m_options[SHOW_CONTROLS] == YES) {
      isShowControl = true;
    }
    m_config->showToolbar(isShowControl);
  }
}

void ViewerCmdLine::parseCopyRect()
{
  if (isPresent(COPY_RECT)) {
    bool isCopyRect = false;

    if (m_options[COPY_RECT] == YES) {
      isCopyRect = true;
    }
    m_conConf->allowCopyRect(isCopyRect);
  }
}

void ViewerCmdLine::parseViewOnly()
{
  if (isPresent(VIEW_ONLY)) {
    bool isViewOnly = false;

    if (m_options[VIEW_ONLY] == ViewerCmdLine::YES) {
      isViewOnly = true;
    }
    m_conConf->setViewOnly(isViewOnly);
  }
}

void ViewerCmdLine::parseJpegImageQuality()
{
  if (isPresent(JPEG_IMAGE_QUALITY)) {
    int iJpegQuality = atoi(m_options[JPEG_IMAGE_QUALITY]);
    m_conConf->setJpegCompressionLevel(iJpegQuality);
  }
}

void ViewerCmdLine::parseCompressionLevel()
{
  if (isPresent(COMPRESSION_LEVEL)) {
    int iCompLvl = atoi(m_options[COMPRESSION_LEVEL]);
    m_conConf->setCustomCompressionLevel(iCompLvl);
  }
}

void ViewerCmdLine::parseHostArg()
{
  ::string host;
  m_wpcl.getArgument(1, host);

  if (::not_found(host.find_first_character(':'))) {
    m_conData->setHost(host);
  } else {
   if (isPresent(PORT)) {
     ::string hostPort;

     hostPort.format("{}::{}", host,
                                  m_options[PORT]);
     m_conData->setHost(hostPort);
   }
   m_conData->setHost(host);
  }
}

bool ViewerCmdLine::parseHostOptions()
{
  if (!isPresent(HOST)) {
    if (m_wpcl.getOptionsCount()) {
      return false;
    }
    return true;
  }
  ::string strHost, strPort;

  if (!isPresent(PORT)) {
    strHost = m_options[HOST];
  } else {
    strHost.formatf("{}::{}", m_options[HOST],
                                m_options[PORT]);
  }
  m_conData->setHost(strHost);
  return true;
}

bool ViewerCmdLine::parseHost()
{
  if (m_wpcl.getArgumentsCount() > 1) {
    parseHostArg();
  } else {
    return parseHostOptions();
  }
  return true;
}

void ViewerCmdLine::parseLogPath()
{
  if (isPresent(LOG_PATH)) {

     throw todo;
    //m_config->setLogDir(m_options[LOG_PATH]);
  }
}
