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

#pragma once


//#include aaa_<list>
//#include "remoting/remoting/network/socket/SocketIPv4.h"
//#include "acme/parallelization/happening.h"
#include "subsystem/thread/Thread.h"
#include "remoting/remoting/network/RfbOutputGate.h"
#include "remoting/remoting/desktop/Desktop.h"
#include "remoting/remoting/framebuffer_update_sender/UpdateSender.h"
//#include "log_writer/LogWriter.h"

#include "RfbDispatcher.h"
#include "ClipboardExchange.h"
#include "ClientInputHandler.h"
#include "ClientTerminationListener.h"
#include "ClientInputEventListener.h"
//#include "remoting/remnode_desktop/NewConnectionEvents.h"
#include "remoting/remoting/node_config/Configurator.h"
#include "subsystem/platform/DemandTimer.h"

namespace remoting
{
   class ClientAuthListener;

   // FIXME: Document it.
   enum ClientState
   {
      IN_NONAUTH,
      IN_AUTH,
      IN_NORMAL_PHASE,
      IN_PENDING_TO_REMOVE,
      IN_READY_TO_REMOVE
    };

   class rfb_client_run;

   // FIXME: Document it, i understand nothing from such kind of description.
   class CLASS_DECL_REMOTING RfbClient:
      virtual public ::subsystem::Thread,
      virtual public ClientInputEventListener,
      virtual public SenderControlInformationInterface
   {
   public:



      ::pointer < rfb_client_run > m_prun;
      ClientState m_clientState;
      bool m_isMarkedOk;
      lockable_critical_section m_criticalsectionClientState;
      ::pointer < ClientTerminationListener > m_pclientterminationlistener;
      //::happening m_connClosingEvent;
      ::happening m_connClosingEvent;

      ::pointer < ::subsystem::SocketIPv4Interface > m_psocket;

      ::pointer < ClientAuthListener > m_pclientauthlistener;

      ::pointer < Viewport > m_pviewportConst;
      ::pointer < Viewport > m_pviewportDynamic;
      lockable_critical_section m_criticalsectionViewport;

      ::pointer < UpdateSender > m_pupdatesender;
      ::pointer < ClipboardExchange > m_pclipboardexchange;
      ::pointer < ClientInputHandler > m_pclientinputhandler;
      ::pointer < Desktop > m_pdesktop;

      bool m_viewOnly;
      bool m_isOutgoing;
      bool m_viewOnlyAuth;
      bool m_shared;

      ::pointer < ::remoting_node::Configurator > m_pconfigurator;
      ::pointer < ::subsystem::LogWriter > m_plogwriter;

      // Information
      unsigned int m_id;

      ::pointer < ::remoting_node::NewConnectionEvents>m_pnewconnectionevents;
      // This timer sets by IdleTimeout value from server config
      // and resets on mouse or keyboard event
      ::subsystem::DemandTimer m_demandtimerIdle;
      int m_idleTimeout;


      RfbClient(::remoting_node::Configurator * pconfigurator, ::remoting_node::NewConnectionEvents *newConnectionEvents, ::subsystem::SocketIPv4Interface *socket,
                ClientTerminationListener *pclientterminationlistener,
                ClientAuthListener *pclientauthlistener, bool viewOnly,
                bool isOutgoing, unsigned int id,
                const ViewPortState & viewportstateConst,
                const ViewPortState & viewportstateDynamic,
                int idleTimeout,
                ::subsystem::LogWriter * plogwriter);

      virtual ~RfbClient();

      void disconnect();

      ClientState getClientState();

      unsigned int getId() const;
      void getPeerHost(::string & address);
      void getLocalIpAddress(::string & address);
      ::pointer < ::subsystem::SocketAddressIPv4Interface > getSocketAddr() const;

      // Return true if connection has been initialised from the server to a client
      // else return false.
      bool isOutgoing() const;

      bool getSharedFlag() const { return m_shared; }
      bool getViewOnlyAuth() const { return m_viewOnlyAuth; }

      void setViewOnlyFlag(bool value);

      // Changes current dynViewPort value by new.
      void changeDynViewPort(const ViewPortState *dynViewPort);

      bool clientIsReady() const { return m_pupdatesender->clientIsReady(); }
      void sendUpdate(const UpdateContainer & updatecontainer,
                      const ::remoting::CursorShape *cursorShape);
      void sendClipboard(const ::scoped_string & newClipboard);

   protected:
      virtual void execute();
      virtual void onTerminate();

   private:
      // Calling this function makes the client manager enter (and leave) the
      // mutex associated with the client ::list_base, so it will have to wait until
      // other threads stop working with our object (such operations should be
      // protected with the same mutex as well). If we call this function to
      // change the state to IN_PENDING_TO_REMOVE or IN_READY_TO_REMOVE, we can
      // guarantee that our object will not be used by the client manager after
      // this call.
      void notifyAbStateChanging(ClientState state);

      // This class is layer between WinDesktop and ClientInputHandler.
      virtual void onKeyboardEvent(unsigned int keySym, bool down);
      virtual void onMouseEvent(unsigned short x, unsigned short y, unsigned char buttonMask);

      void setClientState(ClientState newState);

      ::int_rectangle getViewport(const ::int_size & sizeFramebuffer);
      virtual void onGetViewPort(::int_rectangle &viewRect, bool *shareApp, ::remoting::Region & regionShareApp);
      void getViewPortInfo(const ::int_size & sizeFramebuffer, ::int_rectangle &rectangleResult,
                           bool *shareApp, ::remoting::Region & regionShareApp);

   };



} // namespace remoting





