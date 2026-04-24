// Copyright (C) 2010,2011,2012 GlavSoft LLC.
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
#include "AdditionalActionApplication.h"
#include "ServerHelp.h"

#include "subsystem/platform/CommandLine.h"

#include "subsystem/node/Workstation.h"
#include "subsystem/platform/CommandLineArguments.h"

namespace remoting_node_desktop
{

const char AdditionalActionApplication::LOCK_WORKSTATION_KEY[] = "-lockworkstation";
const char AdditionalActionApplication::LOGOUT_KEY[] = "-logout";

////AdditionalActionApplication::AdditionalActionApplication(::hinstance hInstance,
////                                                         const ::scoped_string & scopedstrwindowClassName,
////                                                         const ::scoped_string & scopedstrCommandLine)// :
////    //LocalOperatingSystemApplication(hInstance, scopedstrwindowClassName), m_commandLine(scopedstrCommandLine)
//AdditionalActionApplication::AdditionalActionApplication(::hinstance hInstance,
////                                                          const ::scoped_string & scopedstrwindowClassName,
////                                                          const ::scoped_string & scopedstrCommandLine)// :
////     //LocalOperatingSystemApplication(hInstance, scopedstrwindowClassName), m_commandLine(scopedstrCommandLine)
//{
//   
//   initialize_operating_system_application();
//   
//   m_commandLine = scopedstrCommandLine;
//
//}

   AdditionalActionApplication::AdditionalActionApplication()
   {

   }

   AdditionalActionApplication::~AdditionalActionApplication()
   {
   }


   //void AdditionalActionApplication::initialize_additional_action_application(::hinstance hInstance,
   //                                                          const ::scoped_string & scopedstrwindowClassName,
   //                                                          const ::scoped_string & scopedstrCommandLine)// :
   //     //LocalOperatingSystemApplication(hInstance, scopedstrwindowClassName), m_commandLine(scopedstrCommandLine)
   void AdditionalActionApplication::initialize_additional_action_application(const ::scoped_string & scopedstrCommandLine)
   {

      initialize_operating_system_application();

      m_strCommandLine = scopedstrCommandLine;

   }


   void AdditionalActionApplication::run()
   {
      ::subsystem::CommandLine args;

      ::subsystem::CommandLineFormat format[] = {
         { LOCK_WORKSTATION_KEY, ::subsystem::NO_ARG },
                                                  {LOGOUT_KEY, ::subsystem::NO_ARG}
      };

      try {
         auto pcommandlinearguments = MainSubsystem().getCommandLineArguments(m_strCommandLine);
         //WinCommandLineArgs cmdArgs(m_commandLine);
         if (!args.parse(format,
                         sizeof(format) / sizeof(::subsystem::CommandLineFormat),
                         pcommandlinearguments)) {
                         }
      } catch (...) {
         ServerHelp::showUsage();
         return;
      }

      //try {
      if (args.optionSpecified(LOCK_WORKSTATION_KEY)) {
         MainSubsystem().Workstation().workstation_lock();
      } else if (args.optionSpecified(LOGOUT_KEY)) {
         MainSubsystem().Workstation().workstation_logOff();
      }
      //} catch (SystemException &sysEx) {
      //return sysEx.getErrorCode();
      //}
      //return 0;
   }
} // namespace remoting_node_desktop