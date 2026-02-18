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
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//

#pragma once


////////#include "util/::string.h"
#include "util/Singleton.h"
#include "config_lib/ConnectionHistory.h"
//#include "log_writer/FileLogWriter.h"
#include "thread/LocalMutex.h"
#include "thread/AutoLock.h"

#include "ConnectionConfig.h"

namespace remoting
{
   //
   // Contains base set of viewer configuration options.
   //

   class ViewerConfig : public Singleton<ViewerConfig>
   {
   public:
      ViewerConfig(const ::scoped_string & scopedstrRegistryPath);
      ~ViewerConfig();

      // Deserializes client configuration from settings storage.
      // Returns true if was no errors during deserialization,
      // false, otherwise.
      bool loadFromStorage(SettingsManager *storage);

      // Serializes client configuration from settings storage.
      // Returns true if was no errors during serialization,
      // false, otherwise.
      bool saveToStorage(SettingsManager *storage) const;

      // Sets port for incoming connection when client runs in daemoon mode
      void setListenPort(int listenPort);
      // Returns listen port value
      int getListenPort() const;

      // Changes log level in 0 - 9 range
      void setLogLevel(enum_trace_level etracelevel);
      // Returns log level
      enum_trace_level getLogLevel() const;

      // Puts log directory to the logDir argument.
      ::file::path getLogDir() const;
      // Changes log directory
      void setLogDir(const ::file::path &path);

      // Creates path to log file and place value to m_pathToLogFile member
      // creates LogWriter and return pointer to him
      LogWriter *initLog(const ::file::path &path, const ::scoped_string & scopedstrLogName, bool useSpecialFolder = true);

      // function return pointer to LogWriter
      LogWriter *getLogWriter();

      // Sets number of connections to remember
      void setHistoryLimit(int historyLimit);
      // Returns number of connections to remember
      int getHistoryLimit() const;

      // Sets "show toolbar" flag
      void showToolbar(bool show);
      // Returns "show toolbar" flag
      bool isToolbarShown() const;

      // Sets "prompt on fullscreen flag"
      void promptOnFullscreen(bool promt);
      // Returns "prompt on fullscreen flag"
      bool isPromptOnFullscreenEnabled() const;

      // Returns path to log file if file is avaliable to write,
      // returns NULL otherwise
      ::string getPathToLogFile() const;

      // Returns connection history
      ConnectionHistory *getConnectionHistory();

      //protected:
      // TCP port for accepting incoming connection
      // when client runs in daemon mode
      int m_listenPort;
      // Current level of logging
      enum_trace_level m_etracelevel;
      // Number of connections to remember
      int m_historyLimit;
      // If set then toolbar is shown, otherwise not shown
      bool m_showToolbar;
      // If set then app must show promt dialog when viewer window
      // become fullscreen
      bool m_promptOnFullscreen;
      // Log file
      ::string m_pathToLogFile;
      ::string m_logName;
      //FileLogWriter *m_LogWriter;
      // Connection history
      RegistryKey m_conHistoryKey; // Used by m_conHistory
      ConnectionHistory m_conHistory;
      //private:
      // Critical section for synchronization
      mutable LocalMutex m_cs;
   };
} // namespace remoting
