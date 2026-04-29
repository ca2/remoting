// Copyright (C) 2009,2010,2011,2012 GlavSoft LLC.
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
#include "DesktopConfigClient.h"
//#include "subsystem/thread/lockable_critical_section.h"
#include "ReconnectException.h"

namespace remoting
{


   //DesktopConfigClient::DesktopConfigClient(BlockingGate *pblockinggate) : DesktopServerProto(pblockinggate) {}
   DesktopConfigClient::DesktopConfigClient()
   {

   }

   DesktopConfigClient::~DesktopConfigClient() {}

   void DesktopConfigClient::initialize_desktop_config_client(::remoting_node::Configurator * pconfigurator, BlockingGate *pblockinggate) {
      initialize_desktop_server_proto(pconfigurator, pblockinggate);
   }
   void DesktopConfigClient::updateByNewSettings(BlockingGate *pblockinggate)
   {
      critical_section_lock al(pblockinggate);
      pblockinggate->writeUInt8(CONFIG_RELOAD_REQ);
      sendConfigSettings(pblockinggate);
   }

   bool DesktopConfigClient::isRemoteInputAllowed()
   {
      bool result = false;
      try
      {
         critical_section_lock al(m_pblockinggate);
         m_pblockinggate->writeUInt8(SOFT_INPUT_ENABLING_REQ);
         m_pblockinggate->writeUInt64(m_lastInputTime.m_iSecond);
         result = m_pblockinggate->readUInt8() != 0;
         m_lastInputTime.m_iSecond = m_pblockinggate->readUInt64();
      }
      catch (ReconnectException &)
      {
      }
      return result;
   }


} // namespace remoting
