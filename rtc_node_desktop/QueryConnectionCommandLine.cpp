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
#include "QueryConnectionCommandLine.h"

#include "subsystem/platform/StringParser.h"

namespace remoting_node_desktop
{


   ::string_literal QueryConnectionCommandLine::QUERY_CONNECTION = "-queryconnection";
   ::string_literal QueryConnectionCommandLine::PEER_ADDR = "-peer";
   ::string_literal QueryConnectionCommandLine::TIMEOUT = "-timeout";
   ::string_literal QueryConnectionCommandLine::ACCEPT = "-accept";

   QueryConnectionCommandLine::QueryConnectionCommandLine() {}

   QueryConnectionCommandLine::~QueryConnectionCommandLine() {}

   void QueryConnectionCommandLine::parse(const ::subsystem::CommandLineArguments *commandLine)
   {
      ::subsystem::CommandLineFormat format[] = {{QUERY_CONNECTION, ::subsystem::NO_ARG},
                                                 {PEER_ADDR, ::subsystem::NEEDS_ARG},
                                                 {TIMEOUT, ::subsystem::NEEDS_ARG},
                                                 {ACCEPT, ::subsystem::NO_ARG}};

      if (!CommandLine::parse(format, sizeof(format) / sizeof(::subsystem::CommandLineFormat), commandLine))
      {
         throw ::subsystem::Exception("Wrong command line format");
      }

      if (!optionSpecified(QUERY_CONNECTION))
      {
         throw ::subsystem::Exception("-queryconnection flag is not specified");
      }

      ::string timeoutStr;

      if (optionSpecified(TIMEOUT, &timeoutStr) &&
          !MainSubsystem().StringParser().parseInt(timeoutStr, (::i32 *)&m_timeout))
      {
         throw ::subsystem::Exception("Invalid timeout");
      }
   }

   bool QueryConnectionCommandLine::isTimeoutSpecified() { return optionSpecified(TIMEOUT); }

   void QueryConnectionCommandLine::getPeerAddress(::string &storage) { optionSpecified(PEER_ADDR, &storage); }

   bool QueryConnectionCommandLine::isDefaultActionAccept() { return optionSpecified(ACCEPT); }

   ::u32 QueryConnectionCommandLine::getTimeout() { return m_timeout; }


} // namespace remoting_node_desktop
