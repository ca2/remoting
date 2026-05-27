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

#pragma once


#include "subsystem/platform/CommandLine.h"
#include "subsystem/platform/Exception.h"


namespace remoting_node_desktop
{

   /**
    * Desktop server command line parser.
    * @valid command line is '-desktopserver -logdir pathToLogDir' where
    * pathToLogDir arg is full path to directory where desktop server log
    * will be created.
    */
   class CLASS_DECL_REMOTING_RTC_NODE_DESKTOP DesktopServerCommandLine : private ::subsystem::CommandLine
   {
   public:
      DesktopServerCommandLine();
      virtual ~DesktopServerCommandLine();

      /**
       * Parses command line.
       * @param cmdLine command line to parse.
       * @throws ::subsystem::Exception on fail.
       */
      virtual void parse(const ::subsystem::CommandLineArguments *cmdArgs);

      /**
       * Puts specified in command line path to log directory into output parameter.
       * @param [out] logDir output parameter for log directory.
       * @remark if parse method wasn't before this will return 0 as log dir.
       */
      void getLogDir(::string &logDir);

      /**
       * Returns specified log level.
       * @return specified log level.
       */
      ::i32 getLogLevel();

      /**
       * Puts specified shared memory name into output parameter.
       */
      void getSharedMemName(::string &shMemName);

   public:
      static ::string_literal DESKTOP_SERVER_KEY;

   private:
      static ::string_literal LOG_DIR_KEY;
      static ::string_literal LOG_LEVEL_KEY;
      static ::string_literal SHARED_MEMORY_NAME_KEY;
   };


} // namespace remoting_node_desktop



