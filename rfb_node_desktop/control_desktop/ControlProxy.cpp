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
#include "ControlProxy.h"
#include "remoting/rfb_node_desktop/control_desktop/ControlProto.h"
//#include "subsystem/thread/lockable_critical_section.h"

//#include aaa_<crtdbg.h>

namespace remoting_control_desktop
{
   ControlProxy::ControlProxy(ControlGate * pcontrolgate)
   : m_pcontrolgate(pcontrolgate), m_pcontrolmessage(0),
     m_getPassFromConfigEnabled(false),
     m_forService(false)
   {
   }

   ControlProxy::~ControlProxy()
   {
      releaseMessage();
   }

   void ControlProxy::setPasswordProperties(const ::scoped_string & scopedstrPasswordFile,
                                            bool getPassFromConfigEnabled,
                                            bool forService)
   {
      m_passwordFile= scopedstrPasswordFile;
      m_getPassFromConfigEnabled = getPassFromConfigEnabled;
      m_forService = forService;
   }

   ServerInfo ControlProxy::getServerInfo()
   {
      ServerInfo ret;

      critical_section_lock l(m_pcontrolgate);

      createMessage(ControlProto::GET_SERVER_INFO_MSG_ID)->send();

      ret.m_acceptFlag = m_pcontrolgate->readUInt8() == 1;
      ret.m_serviceFlag = m_pcontrolgate->readUInt8() == 1;

      ret.m_statusText = m_pcontrolgate->readUtf8();

      return ret;
   }

   void ControlProxy::reloadServerConfig()
   {
      critical_section_lock l(m_pcontrolgate);

      createMessage(ControlProto::RELOAD_CONFIG_MSG_ID)->send();
   }

   void ControlProxy::disconnectAllClients()
   {
      critical_section_lock l(m_pcontrolgate);

      createMessage(ControlProto::DISCONNECT_ALL_CLIENTS_MSG_ID)->send();
   }

   void ControlProxy::shutdownRemoting()
   {
      critical_section_lock l(m_pcontrolgate);

      createMessage(ControlProto::SHUTDOWN_SERVER_MSG_ID)->send();
   }

   void ControlProxy::getClientsList(::list_base<RfbClientInfo *> *clients)
   {
      critical_section_lock l(m_pcontrolgate);

      createMessage(ControlProto::GET_CLIENT_LIST_MSG_ID)->send();

      ::u32 count = m_pcontrolgate->readUInt32();

      for (::u32 i = 0; i < count; i++) {
         ::string peerAddr;

         ::u32 id = m_pcontrolgate->readUInt32();

         peerAddr = m_pcontrolgate->readUtf8();

         RfbClientInfo *clientInfo = new RfbClientInfo(id, peerAddr);

         clients->add(clientInfo);
      }
   }

   void ControlProxy::makeOutgoingConnection(const ::scoped_string & scopedstrConnectString, bool viewOnly)
   {
      critical_section_lock l(m_pcontrolgate);

      ::pointer < ControlMessage > msg = createMessage(ControlProto::ADD_CLIENT_MSG_ID);

      msg->writeUTF8(scopedstrConnectString);
      msg->writeUInt8(viewOnly);

      msg->send();
   }

   void ControlProxy::makeTcpDispatcherConnection(const ::scoped_string & scopedstrConnectString,
                                                  const ::scoped_string & scopedstrDispatcherName,
                                                  const ::scoped_string & scopedstrKeyword,
                                                  ::u32 connectionId)
   {
      critical_section_lock l(m_pcontrolgate);

      ::pointer < ControlMessage > msg = createMessage(ControlProto::CONNECT_TO_TCPDISP_MSG_ID);

      msg->writeUTF8(scopedstrConnectString);
      msg->writeUTF8(scopedstrDispatcherName);
      msg->writeUTF8(scopedstrKeyword);
      msg->writeUInt32(connectionId);

      msg->send();
   }

   void ControlProxy::sharePrimary()
   {
      critical_section_lock l(m_pcontrolgate);
      ::pointer < ControlMessage > msg = createMessage(ControlProto::SHARE_PRIMARY_MSG_ID);
      msg->send();
   }

   void ControlProxy::shareDisplay(::u8 displayNumber)
   {
      critical_section_lock l(m_pcontrolgate);
      ::pointer < ControlMessage > msg = createMessage(ControlProto::SHARE_DISPLAY_MSG_ID);
      msg->writeUInt8(displayNumber);
      msg->send();
   }

   void ControlProxy::shareRect(const ::i32_rectangle &  shareRect)
   {
      critical_section_lock l(m_pcontrolgate);
      ::pointer < ControlMessage > msg = createMessage(ControlProto::SHARE_RECT_MSG_ID);

      msg->writeInt32(shareRect.left);
      msg->writeInt32(shareRect.top);
      msg->writeInt32(shareRect.right);
      msg->writeInt32(shareRect.bottom);

      msg->send();
   }

   void ControlProxy::shareWindow(const ::scoped_string & scopedstrShareWindowName)
   {
      critical_section_lock l(m_pcontrolgate);
      ::pointer < ControlMessage > msg = createMessage(ControlProto::SHARE_WINDOW_MSG_ID);
      msg->writeUTF8(scopedstrShareWindowName);
      msg->send();
   }

   void ControlProxy::shareFull()
   {
      critical_section_lock l(m_pcontrolgate);
      ::pointer < ControlMessage > msg = createMessage(ControlProto::SHARE_FULL_MSG_ID);
      msg->send();
   }

   void ControlProxy::shareApp(::u32 procId)
   {
      critical_section_lock l(m_pcontrolgate);
      ::pointer < ControlMessage > msg = createMessage(ControlProto::SHARE_APP_MSG_ID);
      msg->writeUInt32(procId);
      msg->send();
   }

   void ControlProxy::setServerConfig(::remoting_rfb_node::ServerConfig * pserverconfig)
   {
      critical_section_lock l(m_pcontrolgate);

      ::pointer < ControlMessage > msg = createMessage(ControlProto::SET_CONFIG_MSG_ID);

      pserverconfig->serialize(msg);

      msg->send();
   }

   void ControlProxy::getServerConfig(::remoting_rfb_node::ServerConfig * pserverconfig)
   {
      critical_section_lock l(m_pcontrolgate);

      createMessage(ControlProto::GET_CONFIG_MSG_ID)->send();

      pserverconfig->deserialize(m_pcontrolgate);
   }

   bool ControlProxy::getShowTrayIconFlag()
   {
      critical_section_lock l(m_pcontrolgate);

      auto pmessage = createMessage(ControlProto::GET_SHOW_TRAY_ICON_FLAG);
      pmessage->send();

      return m_pcontrolgate->readUInt8() == 1;
   }

   void ControlProxy::updateTvnControlProcessId(DWORD processId)
   {
      critical_section_lock l(m_pcontrolgate);

      ::pointer < ControlMessage > msg = createMessage(ControlProto::UPDATE_TVNCONTROL_PROCESS_ID_MSG_ID);

      msg->writeUInt32(processId);

      msg->send();
   }

   ::pointer < ControlMessage > ControlProxy::createMessage(DWORD messageId)
   {
      releaseMessage();

      m_pcontrolmessage = allocateø ControlMessage(messageId, m_pcontrolgate, m_passwordFile,
                                     m_getPassFromConfigEnabled, m_forService);

      return m_pcontrolmessage;
   }

   void ControlProxy::releaseMessage()
   {
      m_pcontrolmessage.release();
//      if (m_pcontrolmessage != 0) {
//         delete m_pcontrolmessage;
//
//         m_pcontrolmessage = 0;
//      }
   }
} // namespace remoting_control_desktop

