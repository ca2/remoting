// Copyright (C) 2008,2009,2010,2011,2012 GlavSoft LLC.
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
#include "remoting/remoting_rfb/desktop/DesktopServerWatcher.h"
#include "remoting/remoting_rfb/desktop_ipc/DesktopConfigClient.h"
#include "remoting/remoting_rfb/desktop_ipc/ReconnectingChannel.h"
#include "remoting/remoting_rfb/desktop_ipc/BlockingGate.h"
#include "remoting/remoting_rfb/desktop_ipc/GateKicker.h"
#include "remoting/remoting_rfb/desktop_ipc/DesktopSrvDispatcher.h"
#include "remoting/remoting_rfb/desktop/DesktopBaseImpl.h"
//#include "log_writer/LogWriter.h"

//Interfaces
//#include "subsystem/platform/AnEventListener.h"
#include "remoting/remoting_rfb/desktop_ipc/ReconnectionListener.h"
#include "remoting/remoting_rfb/desktop/UpdateListener.h"

namespace remoting_macos
{

   //class CLASS_DECL_REMOTING_RFB DesktopClientImpl : public AnEventListener, public ReconnectionListener, public Thread, public DesktopBaseImpl
   class CLASS_DECL_REMOTING_MACOS DesktopClientImpl :
      virtual public ::remoting_rfb::ReconnectionListener,
      virtual public ::subsystem::Thread,
      virtual public ::remoting_rfb::DesktopBaseImpl
   {
   public:

      // Inter process transport
      ::pointer < ::remoting_rfb::ReconnectingChannel > m_pchannelClientToServer;
      ::pointer < ::remoting_rfb::ReconnectingChannel > m_pchannelServerToClient;
      ::pointer < ::remoting_rfb::BlockingGate > m_pgateClientToServer;
      ::pointer < ::remoting_rfb::BlockingGate > m_pgateServerToClient;

      ::pointer < ::remoting_rfb::DesktopServerWatcher > m_pdesktopserverwatcher;
      ::pointer < ::remoting_rfb::DesktopSrvDispatcher > m_pdesktopsrvdispatcher;

      ::pointer < ::remoting_rfb::GateKicker > m_pgatekicker;
      ::pointer < ::remoting_rfb::UserInput > m_puserinput; // It uses for delegation by the SasUserInput.

      ::pointer < ::remoting_rfb::DesktopConfigClient > m_pdesktopconfigclient;

      ::pointer < ::subsystem::LogWriter > m_plogwriter;

      //DesktopClientImpl(ClipboardListener *pclipboardlistenerExternal, UpdateSendingListener *pupdatesendinglistenerExternal,
        //                AbnormDeskTermListener *pdesktermlistenerExternal, ::subsystem::LogWriter * plogwriter);
      DesktopClientImpl();
      ~DesktopClientImpl() override;


      void initialize_desktop(::remoting_rfb_node::Configurator * pconfigurator,
         ::subsystem::ClipboardListener *pclipboardlistenerExternal, ::remoting_rfb::UpdateSendingListener *pupdatesendinglistenerExternal,
                  ::remoting_rfb::AbnormDeskTermListener *pdesktermlistenerExternal, ::subsystem::LogWriter * plogwriter) override;


   //protected:
      virtual void onThreadMain();
      virtual void onTermThread();

   //private:
      // Interface functions
      virtual void onAnObjectEvent();
      virtual void onReconnect(Channel *newChannelTo, Channel *newChannelFrom);

      void freeResource();
      void closeDesktopServerTransport();

      virtual bool isRemoteInputTempBlocked();
      virtual void applyNewConfiguration();

   };


} // namespace remoting_macos








