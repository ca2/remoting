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

#pragma once


#include "subsystem/platform/ListenerContainer.h"
#include "remoting/remoting_rfb/rfb_sconn/RfbClient.h"
//#include "subsystem/thread/lockable_critical_section.h"
#include "subsystem/thread/Thread.h"
//#include "subsystem/thread/lockable_critical_section.h"
#include "acme/parallelization/happening.h"
#include "remoting/remoting_rfb/desktop/Desktop.h"
#include "remoting/remoting_rfb/desktop/DesktopFactory.h"
//#include "log_writer/LogWriter.h"

// Listener interfaces
#include "RfbClientManager.h"
#include "RfbClientManagerEventListener.h"
#include "remoting/remoting_rfb/rfb_sconn/ClientTerminationListener.h"
#include "subsystem/node/ClipboardListener.h"
#include "remoting/remoting_rfb/desktop/AbnormDeskTermListener.h"
#include "remoting/remoting_rfb/desktop/UpdateSendingListener.h"
#include "remoting/remoting_rfb/rfb_sconn/ClientAuthListener.h"
#include "remoting/rfb_node_desktop/control_desktop/RfbClientInfo.h"
#include "remoting/remoting_rfb/node/NewConnectionEvents.h"

namespace remoting_rfb_node_desktop
{

   using ClientList =  ::ø<::list_base<::pointer < ::remoting_rfb::RfbClient >> >;
   using ClientListIter = typename ClientList::iterator;

   struct BanProp
   {
      ::u32 count;
      class ::time banLastTime;
   };
   typedef ::map<::string, BanProp> BanList;
   typedef BanList::iterator BanListIter;

   //
   // FIXME: RfbClientManager don't care about configuration changes when
   // application is running. Is this fixme deprecated?
   //
   // FIXME: No documentation for problem which this class solves.
   class CLASS_DECL_REMOTING_RFB_NODE_DESKTOP RfbClientManager : public ::remoting_rfb::ClientTerminationListener,
                            public ::subsystem::ClipboardListener,
                            public ::remoting_rfb::UpdateSendingListener,
                            public ::remoting_rfb::ClientAuthListener,
                            public ::remoting_rfb::AbnormDeskTermListener,
                            public ::subsystem::ListenerContainer<RfbClientManagerEventListener *>
   {
   public:
      // FIXME: parameter is not used.
      RfbClientManager(const ::scoped_string &scopedstrServerName, ::remoting_rfb_node::Configurator *pconfigurator,
                       ::remoting_rfb_node::NewConnectionEvents *newConnectionEvents, ::subsystem::LogWriter * plogwriter,
                       ::remoting_rfb::DesktopFactory *desktopFactory);
      virtual ~RfbClientManager();

      // Adds rfb clients info to specified rfb client info ::list_base.
      // FIXME: This method needed only for control server.
      void getClientsInfo(::remoting_control_desktop::RfbClientInfoList *plist);

      // Disconnects all connected clients.
      virtual void disconnectAllClients();
      virtual void disconnectNonAuthClients();
      virtual void disconnectAuthClients();

      // Sets a view port value to all client that already onThreadMain and
      // will be onThreadMain.
      void setDynViewPort(const ::remoting_rfb::ViewPortState *dynViewPort);

      // FIXME: Place comment for this method here.
      void addNewConnection(::subsystem::SocketIPv4Interface *socket, ::remoting_rfb::ViewPortState *constViewPort, bool viewOnly,
                            bool isOutgoing);

      // returns ::list_base of bans.
      BanList getBanList()
      {
         critical_section_lock al(&m_banListMutex);
         return m_banList;
      };
      ::string getBanListString();

   protected:
      // Listen functions
      virtual void onClientTerminate();
      virtual ::remoting_rfb::Desktop *onClientAuth(::remoting_rfb::RfbClient *client);
      virtual bool onCheckForBan(::remoting_rfb::RfbClient *client);
      // This function only adds the client to the ban ::list_base.
      virtual void onAuthFailed(::remoting_rfb::RfbClient *client);
      virtual void onCheckAccessControl(::remoting_rfb::RfbClient *client);
      virtual void onClipboardUpdate(const ::scoped_string &newClipboard);
      virtual void onSendUpdate(const ::remoting_rfb::UpdateContainer & updatecontainer, const ::remoting_rfb::CursorShape *cursorShape);
      virtual bool isReadyToSend();
      // If an error occured RfbClientManager closes all current connections
      // (authorized and not authorized) that bring to closing the belonged desktop
      // object.
      virtual void onAbnormalDesktopTerminate();

      void waitUntilAllClientAreBeenDestroyed();

   private:
      void validateClientList();

      // Checks the ip to ban.
      // Returns true if client is banned.
      bool checkForBan(const ::scoped_string &ip);
      // If the success param is true the belonged ip entry will be removed
      // from the ban ::list_base. Else the ip will be added to the ban or will be
      // increased it count.
      void updateIpInBan(const ::scoped_string &ip, bool success);

      ClientList m_nonAuthClientList;
      ClientList m_clientList;
      lockable_critical_section m_clientListLocker;
      // m_dynViewPort is a client view port that can be changed during a
      // client work. Now, the dynViewPort has the same value for all clients.
      // By this field initilizes new clients.
      // Acces to the viewport must be covered by the m_clientListLocker mutex.
      ::remoting_rfb::ViewPortState m_dynViewPort;

      BanList m_banList;
      // ::happening m_banTimer;
      ::happening m_banTimer;
      //;
      lockable_critical_section m_banListMutex;

      // ::happening m_listUnderflowingEvent;

      ::happening m_listUnderflowingEvent;

      // Creating and destroying this object must be with the locked
      // m_clientListLocker
      ::pointer < ::remoting_rfb::Desktop > m_pdesktop;
      ::remoting_rfb::DesktopFactory *m_desktopFactory;

      // Inforamtion
      ::u32 m_nextClientId;

      ::pointer<::remoting_rfb_node::Configurator> m_pconfigurator;
      ::pointer<::remoting_rfb_node::NewConnectionEvents> m_pnewconnectionevents;

      ::pointer < ::subsystem::LogWriter > m_plogwriter;
   };


} // namespace remoting_rfb_node_desktop
 