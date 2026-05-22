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


#include "subsystem/_common_header.h"
#include "subsystem/platform/CommandLine.h"
#include "subsystem/platform/Exception.h"

namespace remoting_node_desktop
{

   class CLASS_DECL_REMOTING_RTC_NODE_DESKTOP QueryConnectionCommandLine : private ::subsystem::CommandLine
   {
   public:
      static ::string_literal QUERY_CONNECTION;
      static ::string_literal PEER_ADDR;
      static ::string_literal TIMEOUT;
      static ::string_literal ACCEPT;

      ::u32 m_timeout;

   public:
      QueryConnectionCommandLine();
      virtual ~QueryConnectionCommandLine();

      void parse(const ::subsystem::CommandLineArguments *commandLine);

      // Returns true if timeout is specified.
      bool isTimeoutSpecified();

      // Stores ip address value into storage output parameter.
      void getPeerAddress(::string &storage);
      // Returns true if default action is accept.
      bool isDefaultActionAccept();
      // Returns timeout in seconds.
      ::u32 getTimeout();

      // protected:
      // ::u32 m_timeout;
   };


} // namespace remoting_node_desktop
