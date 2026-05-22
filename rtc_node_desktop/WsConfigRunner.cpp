// Copyright (C) 2009,2010,2011,2012 GlavSoft LLC.
// All rights reserved.
//
//-------------------------------------------------------------------------
// This file is part of the T i g h t V N C software.  Please visit our Web site:
//
//                       http://www.t i g h t v n c.com/
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
#include "WsConfigRunner.h"
#include "subsystem/node/CurrentConsoleProcess.h"
#include "subsystem/node/OperatingSystem.h"
#include "remoting/remoting_rtc/node_config/Configurator.h"
#include "remoting/node_desktop/control_desktop/ControlCommandLine.h"

namespace remoting_node_desktop
{

   WsConfigRunner::WsConfigRunner() 
   {
   }

   WsConfigRunner::~WsConfigRunner()
   {
//      setThreadToFinish();
//      waitThreadToFinish();
   }


   void WsConfigRunner::destroy()
   {
   //   setThreadToFinish();
   //   waitThreadToFinish();
   }


   void WsConfigRunner::initialize_ws_config_runner(::subsystem::LogWriter* plogwriter, bool serviceMode)
   {

      m_serviceMode = serviceMode;
      m_plogwriter = plogwriter;

      resumeThread();
      
   }


   void WsConfigRunner::onThreadMain()
   {
      ::pointer<::subsystem::ProcessInterface> process;

      try
      {
         // Prepare path to executable.
         ::string pathToBin;
         pathToBin = MainSubsystem().OperatingSystem().getCurrentModulePath();
         pathToBin.double_quote();
         // Prepare arguments.
         ::string args;
         args.format("{} {}",
                      m_serviceMode ? ::remoting_control_desktop::ControlCommandLine::CONTROL_SERVICE : ::remoting_control_desktop::ControlCommandLine::CONTROL_APPLICATION,
                      ::remoting_control_desktop::ControlCommandLine::SLAVE_MODE);
         // Start process.
         process = createø<::subsystem::ProcessInterface>();

         process->initialize_process(pathToBin, args);
         process->start();
      }
      catch (::exception &e)
      {
         m_plogwriter->error("Cannot start the WsControl process ({})", e.get_message());
      }

      // if (process != 0) {
      //   delete process;
      // }
   }


} // namespace remoting_node_desktop
