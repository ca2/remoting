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
#include "SocketIPv4Transport.h"
#include "subsystem/socket/SocketIPv4.h"
#include "subsystem/socket/SocketStream.h"

//#include "remoting/remoting/network/socket/SocketStream.h"

namespace remoting_control_desktop
{
   SocketIPv4Transport::SocketIPv4Transport(subsystem::SocketIPv4Interface *psocket)
   : m_psocket(psocket)
   {
      m_pchannel = new ::subsystem::SocketStream(m_psocket);
   }

   SocketIPv4Transport::~SocketIPv4Transport()
   {
      try { close(); } catch (...) { }

      //delete m_stream;
      //delete m_socket;
   }

   Channel* SocketIPv4Transport::getIOStream()
   {
      if (m_psocket->isBound()) {
         _ASSERT(false);

         return 0;
      }

      return m_pchannel;
   }

   Transport *SocketIPv4Transport::accept()
   {
      if (!m_psocket->isBound()) {
         _ASSERT(false);

         return 0;
      }

      return new SocketIPv4Transport(m_psocket->accept());
   }

   void SocketIPv4Transport::close()
   {
      try { m_psocket->shutdown(::subsystem::e_socket_shutdown_both); } catch (...) { }

      m_psocket->close();
   }
} // namespace remoting_control_desktop

