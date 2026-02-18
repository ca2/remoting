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

#include "WsConfigRunner.h"
#include "win_system/CurrentConsoleProcess.h"
//#include "win_system/Environment.h"
#include "server_config_lib/Configurator.h"
#include "remoting_control_desktop/ControlCommandLine.h"

WsConfigRunner::WsConfigRunner(LogWriter *LogWriter, bool serviceMode)
: m_serviceMode(serviceMode),
  m_log(LogWriter)
{
  resume();
}

WsConfigRunner::~WsConfigRunner()
{
  terminate();
  wait();
}

void WsConfigRunner::execute()
{
  Process *process = 0;

  try {
     // Prepare path to executable.
    ::string pathToBin;
    Environment::getCurrentModulePath(&pathToBin);
    pathToBin.quoteSelf();
    // Prepare arguments.
    ::string args;
    args.formatf("{} {}",
      m_serviceMode ? ControlCommandLine::CONTROL_SERVICE :
                      ControlCommandLine::CONTROL_APPLICATION,
      ControlCommandLine::SLAVE_MODE);
    // Start process.
    process = new Process(pathToBin, args);
    process->start();
  } catch (::exception &e) {
    m_log.error("Cannot start the WsControl process ({})", e.get_message());
  }

  if (process != 0) {
    delete process;
  }
}
