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
#include "ConfigurationDialog.h"
#include "NamingDefs.h"
#include "remoting_impact.h"
#include "resource.h"
#include "acme/filesystem/file/item.h"
//#include "file_lib/::file::item.h"
#include "win_system/Process.h"
#include "remoting/common/remoting.h"
ConfigurationDialog::ConfigurationDialog()
: BaseDialog(IDD_CONFIGURATION),
  m_application(0)
{
}

void ConfigurationDialog::setListenerOfUpdate(WindowsApplication *application)
{
  m_application = application;
}

BOOL ConfigurationDialog::onCommand(UINT controlID, UINT notificationID)
{
  if (controlID == IDC_EVERBLVL) {
    if (notificationID == EN_CHANGE) {
      onLogLevelChange();
    }
  }
  if (controlID == IDOK) {
    onOkPressed();
    if (m_application != 0) {
      m_application->postMessage(remoting_impact::WM_USER_CONFIGURATION_RELOAD);
    }
    close_dialog(1);
    return TRUE;
  }
  if (controlID == IDCANCEL) {
    close_dialog(0);
    return TRUE;
  }
  if (controlID == IDC_BCLEAR_LIST) {
    ::remoting::ViewerConfig::getInstance()->getConnectionHistory()->clear();
  }
  if (controlID == IDC_OPEN_LOG_FOLDER_BUTTON) {
    onOpenFolderButtonClick();
  }
  return FALSE;
}

void ConfigurationDialog::onLogLevelChange()
{
  ::string text;
  int logLevel;
  text = m_verbLvl.get_text();
  StringParser::parseInt(text, &logLevel);
  if (logLevel != 0) {
    m_logging.enable_window(true);

    // If log-file is exist, then enable button "Locate...", else disable him.
    //::string logDir;
    auto logDir = ::remoting::ViewerConfig::getInstance()->getLogDir();
    auto logFileName = logDir / (::string(LogNames::VIEWER_LOG_FILE_STUB_NAME) + ".log");

    auto  logFile=file_item(logFileName);
    if (logFile->exists()) {
      m_openLogDir.enable_window(true);
    } else {
      m_openLogDir.enable_window(false);
    }
  } else {
    m_logging.enable_window(false);
    m_openLogDir.enable_window(false);
  }
}
void ConfigurationDialog::onOpenFolderButtonClick()
{
  //::string logDir;
  
  auto logDir = ::remoting::ViewerConfig::getInstance()->getLogDir();

   auto pathLog = logDir / (::string(LogNames::VIEWER_LOG_FILE_STUB_NAME) + ".log");

  ::string command;

  command.format("explorer /select,{}", pathLog);

  Process explorer(command);

  try {
    explorer.start();
  } catch (...) {
    // TODO: Place error notification here.
  }
}

BOOL ConfigurationDialog::onInitDialog()
{
  setControlById(m_showToolBars, IDC_CSHOWTOOLBARS); 
  setControlById(m_warnAtSwitching, IDC_CWARNATSW);
  setControlById(m_numberConn, IDC_ENUMCON);
  setControlById(m_snumConn, IDC_SNUMCON);
  setControlById(m_reverseConn, IDC_EREVCON);
  setControlById(m_sreverseConn, IDC_SREVCON);
  setControlById(m_verbLvl, IDC_EVERBLVL);
  setControlById(m_sverbLvl, IDC_SVERBLVL);
  setControlById(m_logging, IDC_ELOGGING);
  setControlById(m_openLogDir, IDC_OPEN_LOG_FOLDER_BUTTON);

  m_snumConn.setRange(0, 1024);
  m_snumConn.setBuddy(&m_numberConn);

  m_sreverseConn.setRange32(1, 65535);
  m_sreverseConn.setBuddy(&m_reverseConn);

  m_sverbLvl.setRange(0, 9);
  m_sverbLvl.setBuddy(&m_verbLvl);

  updateControlValues();

  return FALSE;
}

void ConfigurationDialog::updateControlValues()
{
  auto config = ::remoting::ViewerConfig::getInstance();

  ::string txt;

  txt.formatf("{}", config->getListenPort());
  m_reverseConn.setText(txt);

  txt.formatf("{}", config->getLogLevel());
  m_verbLvl.setText(txt);

  txt.formatf("{}", config->getHistoryLimit());
  m_numberConn.setText(txt);

  m_showToolBars.check(config->isToolbarShown());
  m_warnAtSwitching.check(config->isPromptOnFullscreenEnabled());

  ::string logFileName;
  logFileName.formatf("{}\\{}.log", config->getPathToLogFile(), LogNames::VIEWER_LOG_FILE_STUB_NAME);
  m_logging.setText(logFileName);
}

bool ConfigurationDialog::isInputValid()
{
  if (!testNum(&m_reverseConn, StringTable::getString(IDS_CONFIGURATION_LISTEN_PORT))) {
    return false;
  }
  if (!testNum(&m_verbLvl, StringTable::getString(IDS_CONFIGURATION_LOG_LEVEL))) {
    return false;
  }
  if (!testNum(&m_numberConn, StringTable::getString(IDS_CONFIGURATION_HISTORY_LIMIT))) {
    return false;
  }
  return true;
}

bool ConfigurationDialog::testNum(TextBox *tb, const ::scoped_string & scopedstrTbName)
{
  //::string text;
  auto text = tb->get_text();

  if (StringParser::tryParseInt(text)) {
    return true;
  }

  ::string scopedstrMessage;
  scopedstrMessage.formatf(StringTable::getString(IDS_ERROR_VALUE_FIELD_ONLY_NUMERIC).c_str(), scopedstrTbName.c_str());

  ::remoting::message_box(m_hwnd, scopedstrMessage,
             StringTable::getString(IDS_CONFIGURATION_CAPTION), MB_OK | MB_ICONWARNING);

  tb->set_focus();

  return false;
}

void ConfigurationDialog::onOkPressed()
{
  if (!isInputValid()) {
    return ;
  }

  auto config = ::remoting::ViewerConfig::getInstance();

  ::string text;
  int intVal;

  text = m_reverseConn.get_text();
  StringParser::parseInt(text, &intVal);
  config->setListenPort(intVal);

  text = m_verbLvl.get_text();
  StringParser::parseInt(text, &intVal);
  config->setLogLevel((enum_trace_level)intVal);

  int oldLimit = config->getHistoryLimit();
  text=m_numberConn.get_text();
  StringParser::parseInt(text, &intVal);
  config->setHistoryLimit(intVal);

  if (config->getHistoryLimit() < oldLimit) {
    config->getConnectionHistory()->truncate();
  }

  config->showToolbar(m_showToolBars.isChecked());
  config->promptOnFullscreen(m_warnAtSwitching.isChecked());

  SettingsManager *sm = ViewerSettingsManager::getInstance();
  config->saveToStorage(sm);
}
