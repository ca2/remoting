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
#include "ConfigServer.h"
#include "DesktopSrvDispatcher.h"
#include "desktop/DesktopConfigLocal.h"
#include "remoting/remoting/node_config/Configurator.h"

namespace remoting
{


   // ConfigServer::ConfigServer(DesktopSrvDispatcher * pdispatcher, ::subsystem::LogWriter * plogwriter) :
   //     DesktopServerProto(0), m_pdesktopconfigclient(plogwriter)
   // {
   //    dispatcher->registerNewHandle(CONFIG_RELOAD_REQ, this);
   //    dispatcher->registerNewHandle(SOFT_INPUT_ENABLING_REQ, this);
   // }

   ConfigServer::ConfigServer() {}
   ConfigServer::~ConfigServer() {}


   void ConfigServer::initialize_config_server(::remoting_node::Configurator * pconfigurator, DesktopSrvDispatcher *pdispatcher, ::subsystem::LogWriter * plogwriter)
   {
      initialize_desktop_server_proto(pconfigurator, nullptr);
      m_pconfigurator = pconfigurator;
      construct_newø(m_pdesktopconfiglocal);
      m_pdesktopconfiglocal->initialize_desktop_config_local(pconfigurator, plogwriter);
      pdispatcher->registerNewHandle(CONFIG_RELOAD_REQ, this);
      pdispatcher->registerNewHandle(SOFT_INPUT_ENABLING_REQ, this);
   }

   void ConfigServer::onRequest(unsigned char reqCode, BlockingGate *pblockinggate)
   {
      switch (reqCode)
      {
         case CONFIG_RELOAD_REQ:
            reloadSettings(pblockinggate);
            break;
         case SOFT_INPUT_ENABLING_REQ:
            answerOnSoftInputEnablingReq(pblockinggate);
            break;
         default:
            ::string errMess;
            errMess.format("Unknown {} protocol code received from a pipe client", (int)reqCode);
            throw ::subsystem::Exception(errMess);
            break;
      }
   }

   void ConfigServer::reloadSettings(BlockingGate *pblockinggate)
   {
      readConfigSettings(pblockinggate);
      m_pconfigurator->notifyReload();
      m_pdesktopconfiglocal->updateByNewSettings();
   }

   void ConfigServer::answerOnSoftInputEnablingReq(BlockingGate *pblockinggate)
   {
      m_pdesktopconfiglocal->correctLastTime(pblockinggate->readUInt64() * 1_s);

      bool allowed = m_pdesktopconfiglocal->isRemoteInputAllowed();
      pblockinggate->writeUInt8(allowed);
      pblockinggate->writeUInt64(m_pdesktopconfiglocal->getLastInputTime().m_iSecond);
   }


} // namespace remoting
