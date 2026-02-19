// Copyright (C) 2009,2010,2011,2012 GlavSoft LLC.
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
#include "framework.h"
#include "ViewerConfig.h"

//#include "win_system/Environment.h"
#include "win_system/RegistryKey.h"
#include "win_system/Registry.h"
#include "acme/filesystem/filesystem/directory_context.h"

//#include "file_lib/::file::item.h"


namespace remoting
{
   // FIXME: Duplicate macro, see ConnectionConfig.cpp file
#define TEST_FAIL(C,R) if (!C) { R = false; }

   ViewerConfig::ViewerConfig(const ::scoped_string & scopedstrRegistryPath)
   : m_etracelevel(e_trace_level_information), m_listenPort(5500), m_historyLimit(32),
     m_showToolbar(true), m_promptOnFullscreen(true),
     m_conHistory(&m_conHistoryKey, m_historyLimit)
   //,    m_LogWriter(0)
   {
      ::string registryKey;
      registryKey.format("{}\\History",::string(scopedstrRegistryPath).c_str());
      m_conHistoryKey.open(Registry::getCurrentUserKey(),
                           registryKey,
                           true);
   }

   ViewerConfig::~ViewerConfig()
   {
      // if (m_LogWriter != 0) {
      //    try {
      //       delete m_LogWriter;
      //    } catch (...) {
      //    }
      // }
   }

   bool ViewerConfig::loadFromStorage(SettingsManager *storage)
   {
      bool loadAllOk = true;

      int iTrace;
      TEST_FAIL(storage->getInt("LogLevel", &iTrace), loadAllOk);
      m_etracelevel = (enum_trace_level) iTrace;
      //setLogLevel(m_logLevel);
      TEST_FAIL(storage->getInt("ListenPort", &m_listenPort), loadAllOk);
      TEST_FAIL(storage->getInt("HistoryLimit", &m_historyLimit), loadAllOk);
      setHistoryLimit(m_historyLimit);
      //
      // FIXME: Why registry entry has "NoToolbar" name but
      // meaning of code in what, that is this flag is set then
      // toolbar become visible.
      //

      TEST_FAIL(storage->getBoolean("NoToolbar", &m_showToolbar), loadAllOk);

      if (storage->getBoolean("SkipFullScreenPrompt", &m_promptOnFullscreen)) {
         m_promptOnFullscreen = !m_promptOnFullscreen;
      } else {
         loadAllOk = false;
      }

      return loadAllOk;
   }

   bool ViewerConfig::saveToStorage(SettingsManager *storage) const
   {
      bool saveAllOk = true;

      TEST_FAIL(storage->setInt("LogLevel",  (int) m_etracelevel), saveAllOk);
      TEST_FAIL(storage->setInt("ListenPort", m_listenPort), saveAllOk);
      TEST_FAIL(storage->setInt("HistoryLimit", m_historyLimit), saveAllOk);
      TEST_FAIL(storage->setBoolean("NoToolbar", m_showToolbar), saveAllOk);
      TEST_FAIL(storage->setBoolean("SkipFullScreenPrompt", !m_promptOnFullscreen), saveAllOk);

      return saveAllOk;
   }

   void ViewerConfig::setListenPort(int listenPort)
   {
      AutoLock l(&m_cs);

      if (listenPort < 0) {
         listenPort = 0;
      } else if (listenPort > 65535) {
         listenPort = 65535;
      }

      m_listenPort = listenPort;
   }

   int ViewerConfig::getListenPort() const
   {
      AutoLock l(&m_cs);
      return m_listenPort;
   }

    void ViewerConfig::setLogLevel(enum_trace_level etracelevel)
    {
   AutoLock l(&m_cs);

   if (m_etracelevel < e_trace_level_none) {
      m_etracelevel = e_trace_level_none;
   } else if (m_etracelevel > e_trace_level_last){
      m_etracelevel = e_trace_level_last;
   }

      ;//::system()->m_plogger->set_
   //m_logLevel = logLevel;
   //if (m_LogWriter != 0) {
     // m_LogWriter->changeLogProps(m_pathToLogFile, m_logLevel);
   //}
    }

    enum_trace_level ViewerConfig::getLogLevel() const
    {
   //    AutoLock l(&m_cs);
       return m_etracelevel;
    }

    ::file::path ViewerConfig::getLogDir() const
    {
       AutoLock l(&m_cs);
       return m_pathToLogFile;
    }

    void ViewerConfig::setLogDir(const ::file::path &logDir)
    {
       AutoLock l(&m_cs);
       m_pathToLogFile = logDir;
   //    if (m_LogWriter != 0) {
   //       m_LogWriter->changeLogProps(m_pathToLogFile, m_logLevel);
   //    }
    }

   void ViewerConfig::setHistoryLimit(int historyLimit)
   {
      AutoLock l(&m_cs);

      if (historyLimit < 0) {
         historyLimit = 0;
      } else if (historyLimit > 1024) {
         historyLimit = 1024;
      }

      m_historyLimit = historyLimit;

      m_conHistory.setLimit(m_historyLimit);
   }

   int ViewerConfig::getHistoryLimit() const
   {
      AutoLock l(&m_cs);
      return m_historyLimit;
   }

   void ViewerConfig::showToolbar(bool show)
   {
      AutoLock l(&m_cs);
      m_showToolbar = show;
   }

   bool ViewerConfig::isToolbarShown() const
   {
      AutoLock l(&m_cs);
      return m_showToolbar;
   }

   void ViewerConfig::promptOnFullscreen(bool prompt)
   {
      AutoLock l(&m_cs);
      m_promptOnFullscreen = prompt;
   }

   bool ViewerConfig::isPromptOnFullscreenEnabled() const
   {
      AutoLock l(&m_cs);
      return m_promptOnFullscreen;
   }

   ::string ViewerConfig::getPathToLogFile() const
   {
      AutoLock l(&m_cs);
      return m_pathToLogFile;
   }

   ConnectionHistory *ViewerConfig::getConnectionHistory()
   {
      AutoLock l(&m_cs);
      return &m_conHistory;
   }
   //
    LogWriter *ViewerConfig::initLog(const ::file::path & pathLogDir, const ::scoped_string & scopedstrLogName, bool useSpecialFolder)
    {
   //    m_logName = scopedstrLogName;
   //    ::string logFileFolderPath;
   //    ::string appDataPath;
   //
   //    // After that logFilePath variable will contain path to folder
   //    // where remoting_impact.log must be located
   //    if (Environment::getSpecialFolderPath(Environment::APPLICATION_DATA_SPECIAL_FOLDER, appDataPath) && useSpecialFolder) {
   //       logFileFolderPath.format("{}\\{}", appDataPath.c_str(), ::string(scopedstrLogDir).c_str());
   //    } else {
   //       logFileFolderPath.format("{}", ::string(scopedstrLogDir).c_str());
   //    }
   //
   //    // Create TightVNC folder
   //    {
   //       //::file::item folder(logFileFolderPath);
   //       //folder.mkdir();
   //       directory()->create(logFileFolderPath);
   //    }
   //
   //    // Path to log file
   //    AutoLock l(&m_cs);
   //    m_pathToLogFile = logFileFolderPath;
   //
   //    if (m_LogWriter != 0) {
   //       delete m_LogWriter;
   //    }
   //    m_LogWriter = new FileLogWriter(m_pathToLogFile, scopedstrLogName, m_logLevel, false);
       return this;
    }

    LogWriter *ViewerConfig::getLogWriter()
    {
   //    return m_LogWriter;
      return this;
    }
} // namespace remoting


