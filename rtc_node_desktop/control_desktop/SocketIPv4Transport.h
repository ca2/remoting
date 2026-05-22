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

//#include "remoting/remoting_rtc/network/socket/SocketIPv4.h"

namespace remoting_control_desktop
{
   /**
    * Transport that uses IPv4 socket.
    */
   class SocketIPv4Transport : public Transport
   {
   public:
      /**
       * Creates transport and takes ownership over existing socket.
       */
      SocketIPv4Transport(subsystem::SocketIPv4Interface *socket);
      /**
       * Deletes transport and frees resources.
       */
      virtual ~SocketIPv4Transport();

      /**
       * Returns transport's IO Stream.
       */
      virtual ::pointer < Channel > getIOStream();

      /**
       * Accepts new connection.
       * @return transport for accepted connection.
       * @throws SocketException on fail.
       */
      virtual ::pointer < Transport > accept();

      /**
       * Destroys transport(closes socket).
       */
      virtual void close();

   private:
      /**
       * Real transport.
       */
      ::pointer < subsystem::SocketIPv4Interface > m_psocket;
      /**
       * Stream.
       */
      ::pointer < Channel > m_pchannel;
   };
} // namespace remoting_control_desktop

