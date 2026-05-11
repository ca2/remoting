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
#include "NamedPipeTransport.h"
#include "subsystem/node/NamedPipe.h"

namespace remoting_control_desktop
{
   NamedPipeTransport::NamedPipeTransport(::subsystem::NamedPipeInterface *pnamedpipeClient)
   : m_pnamedpipeClient(pnamedpipeClient), m_ppipeserver(0)
   {
   }

   NamedPipeTransport::NamedPipeTransport(::subsystem::PipeServer *server)
   : m_pnamedpipeClient(0), m_ppipeserver(server)
   {
   }

   NamedPipeTransport::~NamedPipeTransport()
   {
      if (m_pnamedpipeClient != 0) {
         delete m_pnamedpipeClient;
      }
      if (m_ppipeserver != 0) {
         delete m_ppipeserver;
      }
   }

   Channel *NamedPipeTransport::getIOStream()
   {
      ASSERT(m_pnamedpipeClient != 0);

      return m_pnamedpipeClient;
   }

   Transport *NamedPipeTransport::accept()
   {
      ASSERT(m_ppipeserver != 0);

      return new NamedPipeTransport(m_ppipeserver->accept());
   }

   void NamedPipeTransport::close()
   {
      if (m_pnamedpipeClient != 0) {
         m_pnamedpipeClient->close();
      }
      if (m_ppipeserver != 0) {
         m_ppipeserver->close();
      }
   }
} // namespace remoting_control_desktop

