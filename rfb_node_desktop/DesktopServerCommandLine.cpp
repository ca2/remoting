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
#include "DesktopServerCommandLine.h"

#include "subsystem/platform/StringParser.h"

namespace remoting_rfb_node_desktop
{


   ::string_literal DesktopServerCommandLine::DESKTOP_SERVER_KEY = "-desktopserver";
   ::string_literal DesktopServerCommandLine::LOG_DIR_KEY = "-logdir";
   ::string_literal DesktopServerCommandLine::LOG_LEVEL_KEY = "-loglevel";
   ::string_literal DesktopServerCommandLine::SHARED_MEMORY_NAME_KEY = "-shmemname";

   DesktopServerCommandLine::DesktopServerCommandLine() {}

   DesktopServerCommandLine::~DesktopServerCommandLine() {}

   void DesktopServerCommandLine::parse(const ::subsystem::CommandLineArguments *cmdArgs)
   {
      ::subsystem::CommandLineFormat format[] = {{DESKTOP_SERVER_KEY, ::subsystem::NO_ARG},
                                                 {LOG_DIR_KEY, ::subsystem::NEEDS_ARG},
                                                 {LOG_LEVEL_KEY, ::subsystem::NEEDS_ARG},
                                                 {SHARED_MEMORY_NAME_KEY, ::subsystem::NEEDS_ARG}};

      if (!CommandLine::parse(format, sizeof(format) / sizeof(::subsystem::CommandLineFormat), cmdArgs) ||
          !optionSpecified(DESKTOP_SERVER_KEY) || !optionSpecified(LOG_DIR_KEY) || !optionSpecified(LOG_LEVEL_KEY) ||
          !optionSpecified(SHARED_MEMORY_NAME_KEY))
      {
         throw ::subsystem::Exception("Command line is invalid");
      }
   }

   void DesktopServerCommandLine::getLogDir(::string &logDir)
   {
      if (!optionSpecified(LOG_DIR_KEY, &logDir))
      {
         ASSERT(false);

         logDir.clear();
      }
   }

   int DesktopServerCommandLine::getLogLevel()
   {
      ::string logLevelKeyArg;

      if (!optionSpecified(LOG_LEVEL_KEY, &logLevelKeyArg))
      {
         ASSERT(false);
      }

      int ret;

      if (!MainSubsystem().StringParser().parseInt(logLevelKeyArg, &ret))
      {
         ASSERT(false);
      }

      return ret;
   }
   void DesktopServerCommandLine::getSharedMemName(::string &shMemName)
   {
      if (!optionSpecified(SHARED_MEMORY_NAME_KEY, &shMemName))
      {
         ASSERT(false);

         shMemName.clear();
      }
   }


} // namespace remoting_rfb_node_desktop




