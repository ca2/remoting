// Copyright (C) 2010,2011,2012 GlavSoft LLC.
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


#include "Transport.h"

//#include "remoting/remoting_rfb/network/socket/SocketIPv4.h"

#include "subsystem/node/Pipe.h"
#include "subsystem/node/PipeClient.h"
#include "subsystem/node/PipeServer.h"


namespace remoting_control_desktop
{
   /**
    * Transport factory.
    *
    * Can create various types of transport.
    */
   class TransportFactory
   {
   public:
      /**
       * Creates client transport that uses client socket.
       *
       * Creates socket, connects to host, creates transport using connected socket
       * and returns transport.
       *
       * @param connectHost host to connect for socket.
       * @param connectPort port to connect.
       * @return created transport.
       *
       * @throws SocketException on socket error.
       */
      static ::pointer < Transport > createSocketClientTransport(const ::scoped_string & scopedstrConnectHost,
                                                    ::u32 connectPort);
      /**
       * Creates server transport that uses bound socket.
       *
       * Creates socket, bound it, creates transport using socket
       * and returns transport.
       *
       * @param bindHost host to bind.
       * @param bindPort port to bind.
       * @return created transport.
       *
       * @throws SocketException on socket error.
       */
      static ::pointer < Transport > createSocketServerTransport(const ::scoped_string & scopedstrBindHost,
                                                    ::u32 bindPort);

      /**
       * Creates client transport that uses named pipe.
       *
       * @param name name of pipe.
       * @return created transport.
       *
       * @throws ::subsystem::Exception on socket error.
       */
      static ::pointer < Transport > createPipeClientTransport(const ::scoped_string & scopedstrName);

      /**
       * Creates server transport that uses named pipe.
       *
       * @param name name of pipe.
       * @return created transport.
       *
       * @throws ::subsystem::Exception on socket error.
       */
      static ::pointer < Transport > createPipeServerTransport(const ::scoped_string & scopedstrName);

   private:
      /**
       * Don't allow instanizing of factory.
       */
      TransportFactory() { };
   };
} // namespace remoting_control_desktop

