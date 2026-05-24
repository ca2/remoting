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
#include "DesktopConfigClient.h"
//#include "subsystem/thread/lockable_critical_section.h"
#include "ReconnectException.h"

namespace remoting_rfb
{


   //DesktopConfigClient::DesktopConfigClient(BlockingGate *pblockinggate) : DesktopServerProto(pblockinggate) {}
   DesktopConfigClient::DesktopConfigClient()
   {

   }

   DesktopConfigClient::~DesktopConfigClient() {}

   void DesktopConfigClient::initialize_desktop_config_client(::remoting_rfb_node::Configurator * pconfigurator, BlockingGate *pblockinggate) {
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
         critical_section_lock al(m_pcontrolgate);
         m_pcontrolgate->writeUInt8(SOFT_INPUT_ENABLING_REQ);
         m_pcontrolgate->writeUInt64(m_lastInputTime.m_iSecond);
         result = m_pcontrolgate->readUInt8() != 0;
         m_lastInputTime.m_iSecond = m_pcontrolgate->readUInt64();
      }
      catch (ReconnectException &)
      {
      }
      return result;
   }


} // namespace remoting_rfb
