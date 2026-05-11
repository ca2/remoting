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
#include "TransportFactory.h"
#include "SocketIPv4Transport.h"
#include "NamedPipeTransport.h"

#include "subsystem/node/security/SecurityAttributes.h"
#include "subsystem/socket/SocketIPv4.h"


namespace remoting_control_desktop
{
   ::pointer < Transport >TransportFactory::createSocketClientTransport(const ::scoped_string & scopedstrConnectHost,
                                                            ::u32 connectPort)
   {
      auto psocket = ::system()->createø<::subsystem::SocketIPv4Interface>();

      try {
         psocket->connect(scopedstrConnectHost, connectPort);
      } catch (::subsystem::SocketException &) {
         //delete socket;
         throw;
      }

      return allocateø SocketIPv4Transport(psocket);
   }

   ::pointer < Transport >TransportFactory::createSocketServerTransport(const ::scoped_string & scopedstrBindHost,
                                                            ::u32 bindPort)
   {
      auto psocket = ::system()->createø<::subsystem::SocketIPv4Interface>();

      try {
         psocket->bind(scopedstrBindHost, bindPort);
         psocket->listen(15);
      } catch (::subsystem::SocketException &) {
         //delete socket;
         throw;
      }

      return allocateø SocketIPv4Transport(psocket);
   }

   ::pointer < Transport > TransportFactory::createPipeClientTransport(const ::scoped_string & scopedstrName)
   {
      return allocateø NamedPipeTransport(MainSubsystem().PipeClient().connect(scopedstrName, 0));
   }

   ::pointer < Transport > TransportFactory::createPipeServerTransport(const ::scoped_string & scopedstrName)
   {
      // FIXME: Memory leak.
      auto pipeSecurity = ::system()->createø<::subsystem:: SecurityAttributesInterface>();

      pipeSecurity->setInheritable();
      pipeSecurity->shareToAllUsers();

      auto ppipeserver = ::system()->createø<::subsystem:: PipeServer>();

      ppipeserver->initialize_pipe_server(scopedstrName, 0, pipeSecurity);

      return allocateø NamedPipeTransport(ppipeserver);
   }
} // namespace remoting_control_desktop

