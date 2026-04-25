// Copyright (C) 2008,2009,2010,2011,2012 GlavSoft LLC.
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
#include "ServerConfig.h"

#include "subsystem/node/OperatingSystem.h"
#include "remoting/remoting/region/RectSerializer.h"
//#include "file_lib/::file::item.h"
#include "acme/input_output/DataCopy.h"
namespace remoting
{
   ServerConfig::ServerConfig()
   : m_rfbPort(5900), m_httpPort(5800),
     m_disconnectAction(DA_DO_NOTHING), m_logLevel(0), m_useControlAuth(false),
     m_controlAuthAlwaysChecking(false),
     m_acceptRfbConnections(true), m_useAuthentication(true),
     m_onlyLoopbackConnections(false), m_acceptHttpConnections(true),
     m_enableAppletParamInUrl(true), m_enableFileTransfers(true),
     m_D3DAllowed(true),
     m_mirrorDriverAllowed(true),
     m_removeWallpaper(true), m_hasReadOnlyPassword(false),
     m_hasPrimaryPassword(false), m_alwaysShared(false), m_neverShared(false),
     m_disconnectClients(true), m_pollingInterval(1000), m_localInputPriorityTimeout(3),
     m_blockLocalInput(false), m_blockRemoteInput(false), m_localInputPriority(false),
     m_defaultActionAccept(false), m_queryTimeout(30),
     m_allowLoopbackConnections(false),
     m_videoRecognitionInterval(3000), m_grabTransparentWindows(true),
     m_saveLogToAllUsersPath(false), m_hasControlPassword(false),
     m_showTrayIcon(true),
     m_connectToRdp(false),
     m_idleTimeout(0)
   {
      memset(m_primaryPassword,  0, sizeof(m_primaryPassword));
      memset(m_readonlyPassword, 0, sizeof(m_readonlyPassword));
      memset(m_controlPassword,  0, sizeof(m_controlPassword));
   }

   ServerConfig::~ServerConfig()
   {
   }

   ServerConfig::ServerConfig(ServerConfig& other)
   {
      DataCopy datacopy;
      DataOutputStream outputstream(&datacopy);
      other.serialize(&outputstream);
      DataInputStream inputstream(&datacopy);
      this->deserialize(&inputstream);
   }

   ServerConfig& ServerConfig::operator=(ServerConfig& other) {
      if (this != &other) {
         DataCopy datacopy;
         DataOutputStream outputstream(&datacopy);
         other.serialize(&outputstream);
         DataInputStream inputstream(&datacopy);
         this->deserialize(&inputstream);
      }
      return *this;
   }

   void ServerConfig::serialize(DataOutputStream * pdataoutputstream)
   {
      AutoLock l(this);

      pdataoutputstream->writeInt32(m_rfbPort);
      pdataoutputstream->writeInt32(m_httpPort);
      pdataoutputstream->writeInt8(m_enableFileTransfers ? 1 : 0);
      pdataoutputstream->writeInt8(m_removeWallpaper ? 1 : 0);
      pdataoutputstream->writeInt8(m_D3DAllowed ? 1 : 0);
      pdataoutputstream->writeInt8(m_mirrorDriverAllowed ? 1 : 0);
      pdataoutputstream->writeInt32(m_disconnectAction);
      pdataoutputstream->writeInt8(m_acceptRfbConnections ? 1 : 0);
      pdataoutputstream->writeInt8(m_acceptHttpConnections ? 1 : 0);

      pdataoutputstream->writeInt8(m_hasPrimaryPassword ? 1 : 0);
      pdataoutputstream->write(m_primaryPassword, VNC_PASSWORD_SIZE);
      pdataoutputstream->writeInt8(m_hasReadOnlyPassword ? 1 : 0);
      pdataoutputstream->write(m_readonlyPassword, VNC_PASSWORD_SIZE);
      pdataoutputstream->writeInt8(m_hasControlPassword ? 1 : 0);
      pdataoutputstream->write(m_controlPassword, VNC_PASSWORD_SIZE);
      pdataoutputstream->writeInt8(m_useAuthentication ? 1 : 0);
      pdataoutputstream->writeInt8(m_onlyLoopbackConnections ? 1 : 0);
      pdataoutputstream->writeInt8(m_enableAppletParamInUrl ? 1 : 0);
      pdataoutputstream->writeInt32(m_logLevel);
      pdataoutputstream->writeInt8(m_useControlAuth ? 1 : 0);
      pdataoutputstream->writeInt8(m_controlAuthAlwaysChecking ? 1 : 0);
      pdataoutputstream->writeInt8(m_alwaysShared ? 1 : 0);
      pdataoutputstream->writeInt8(m_neverShared ? 1 : 0);
      pdataoutputstream->writeInt8(m_disconnectClients ? 1 : 0);
      pdataoutputstream->writeUInt32(m_pollingInterval);
      pdataoutputstream->writeInt8(m_blockRemoteInput ? 1 : 0);
      pdataoutputstream->writeInt8(m_blockLocalInput ? 1 : 0);
      pdataoutputstream->writeInt8(m_localInputPriority ? 1 : 0);
      pdataoutputstream->writeUInt32(m_localInputPriorityTimeout);
      pdataoutputstream->writeInt8(m_defaultActionAccept ? 1 : 0);
      pdataoutputstream->writeUInt32(m_queryTimeout);
      pdataoutputstream->writeInt8(m_connectToRdp ? 1 : 0);


      m_portMappings.serialize(pdataoutputstream);

      m_accessControlContainer.serialize(pdataoutputstream);

      pdataoutputstream->writeInt8(m_allowLoopbackConnections ? 1 : 0);

      _ASSERT((unsigned int)m_videoClassNames.size() == m_videoClassNames.size());

      pdataoutputstream->writeUInt32((unsigned int)m_videoClassNames.size());
      for (size_t i = 0; i < m_videoClassNames.size(); i++) {
         pdataoutputstream->writeUTF8(m_videoClassNames.at(i));
      }

      pdataoutputstream->writeUInt32(m_videoRecognitionInterval);

      pdataoutputstream->writeUInt32(m_idleTimeout);
      _ASSERT((unsigned int)m_videoRects.size() == m_videoRects.size());
      pdataoutputstream->writeUInt32((unsigned int)m_videoRects.size());
      for (size_t i = 0; i < m_videoRects.size(); i++) {
         ::string s;
         ::remoting::RectSerializer::toString(m_videoRects[i], s);
         pdataoutputstream->writeUTF8(s);
      }

      pdataoutputstream->writeInt8(m_grabTransparentWindows ? 1 : 0);

      pdataoutputstream->writeInt8(m_saveLogToAllUsersPath ? 1 : 0);
      pdataoutputstream->writeInt8(m_hasPrimaryPassword ? 1 : 0);
      pdataoutputstream->writeInt8(m_hasReadOnlyPassword ? 1 : 0);
      pdataoutputstream->writeInt8(m_hasControlPassword ? 1 : 0);
      pdataoutputstream->writeInt8(m_showTrayIcon ? 1 : 0);

      pdataoutputstream->writeUTF8(m_logFilePath);
   }

   void ServerConfig::deserialize(DataInputStream * pdatainputstream)
   {
      AutoLock l(this);

      m_rfbPort = pdatainputstream->readInt32();
      m_httpPort = pdatainputstream->readInt32();

      m_enableFileTransfers = pdatainputstream->readInt8() == 1;
      m_removeWallpaper = pdatainputstream->readInt8() == 1;
      m_D3DAllowed = pdatainputstream->readInt8() != 0;
      m_mirrorDriverAllowed = pdatainputstream->readInt8() != 0;
      m_disconnectAction = (ServerConfig::DisconnectAction)pdatainputstream->readInt32();
      m_acceptRfbConnections = pdatainputstream->readInt8() == 1;
      m_acceptHttpConnections = pdatainputstream->readInt8() == 1;

      m_hasPrimaryPassword = pdatainputstream->readInt8() == 1;
      pdatainputstream->readFully(m_primaryPassword, VNC_PASSWORD_SIZE);
      m_hasReadOnlyPassword = pdatainputstream->readInt8() == 1;
      pdatainputstream->readFully(m_readonlyPassword, VNC_PASSWORD_SIZE);
      m_hasControlPassword = pdatainputstream->readInt8() == 1;
      pdatainputstream->readFully(m_controlPassword, VNC_PASSWORD_SIZE);
      m_useAuthentication = pdatainputstream->readInt8() == 1;

      m_onlyLoopbackConnections = pdatainputstream->readInt8() == 1;
      m_enableAppletParamInUrl = pdatainputstream->readInt8() == 1;
      m_logLevel = pdatainputstream->readInt32();
      m_useControlAuth = pdatainputstream->readInt8() == 1;
      m_controlAuthAlwaysChecking = pdatainputstream->readInt8() != 0;
      m_alwaysShared = pdatainputstream->readInt8() == 1;
      m_neverShared = pdatainputstream->readInt8() == 1;
      m_disconnectClients = pdatainputstream->readInt8() == 1;
      m_pollingInterval = pdatainputstream->readUInt32();
      m_blockRemoteInput = pdatainputstream->readInt8() == 1;
      m_blockLocalInput = pdatainputstream->readInt8() == 1;
      m_localInputPriority = pdatainputstream->readInt8() == 1;
      m_localInputPriorityTimeout = pdatainputstream->readUInt32();
      m_defaultActionAccept = pdatainputstream->readInt8() == 1;
      m_queryTimeout = pdatainputstream->readUInt32();
      m_connectToRdp = pdatainputstream->readInt8() == 1;

      m_portMappings.deserialize(pdatainputstream);

      m_accessControlContainer.deserialize(pdatainputstream);

      m_allowLoopbackConnections = pdatainputstream->readInt8() == 1;

      m_videoClassNames.clear();
      size_t count = pdatainputstream->readUInt32();
      ::string videoClass;
      for (size_t i = 0; i < count; i++) {
         videoClass = pdatainputstream->readUtf8();
         m_videoClassNames.add(videoClass);
      }

      m_videoRecognitionInterval = pdatainputstream->readUInt32();

      m_idleTimeout = pdatainputstream->readUInt32();
      m_videoRects.clear();
      count = pdatainputstream->readUInt32();
      ::string strVideoRect;
      for (size_t i = 0; i < count; i++) {
         strVideoRect = pdatainputstream->readUtf8();
         m_videoRects.add(::remoting::RectSerializer::toRect(strVideoRect));
      }

      m_grabTransparentWindows = pdatainputstream->readInt8() == 1;

      m_saveLogToAllUsersPath = pdatainputstream->readInt8() == 1;
      m_hasPrimaryPassword = pdatainputstream->readInt8() == 1;
      m_hasReadOnlyPassword = pdatainputstream->readInt8() == 1;
      m_hasControlPassword = pdatainputstream->readInt8() == 1;
      m_showTrayIcon = pdatainputstream->readInt8() == 1;

      m_logFilePath = pdatainputstream->readUtf8();
   }

   bool ServerConfig::getShowTrayIconFlag()
   {
      AutoLock l(this);

      return m_showTrayIcon;
   }

   void ServerConfig::setShowTrayIconFlag(bool val)
   {
      AutoLock l(this);

      m_showTrayIcon = val;
   }

   bool ServerConfig::getConnectToRdpFlag()
   {
      AutoLock l(this);

      return m_connectToRdp;
   }

   void ServerConfig::setConnectToRdpFlag(bool val)
   {
      AutoLock l(this);

      m_connectToRdp = val;
   }

   void ServerConfig::getLogFileDir(::string & logFilePath)
   {
      AutoLock l(this);

      logFilePath = m_logFilePath;
   }

   void ServerConfig::setLogFileDir(const ::scoped_string & scopedstrLogFilePath)
   {
      AutoLock l(this);

      m_logFilePath = scopedstrLogFilePath;
   }

   IpAccessRule::ActionType ServerConfig::getActionByAddress(unsigned long ip)
   {
      AutoLock l(this);

      IpAccessControl *rules = &m_accessControlContainer;

      size_t rulesCount = rules->size();

      for (size_t i = 0; i < rulesCount; i++) {
         IpAccessRule *rule = rules->at(i);
         if (rule->isIncludingAddress(ip)) {
            return rule->getAction();
         }
      }

      return IpAccessRule::ACTION_TYPE_ALLOW;
   }

   bool ServerConfig::isControlAuthEnabled()
   {
      AutoLock l(&m_objectCS);

      return m_useControlAuth;
   }

   void ServerConfig::useControlAuth(bool useAuth)
   {
      AutoLock l(&m_objectCS);

      m_useControlAuth = useAuth;
   }

   bool ServerConfig::getControlAuthAlwaysChecking()
   {
      AutoLock l(&m_objectCS);

      return m_controlAuthAlwaysChecking;
   }

   void ServerConfig::setControlAuthAlwaysChecking(bool value)
   {
      AutoLock l(&m_objectCS);

      m_controlAuthAlwaysChecking = value;
   }

   void ServerConfig::setRfbPort(int port)
   {
      AutoLock lock(&m_objectCS);
      if (port > 65535) {
         m_rfbPort = 65535;
      } else if (port <= 0) {
         m_rfbPort = 1;
      } else {
         m_rfbPort = port;
      }
   }

   int ServerConfig::getRfbPort()
   {
      AutoLock lock(&m_objectCS);
      return m_rfbPort;
   }

   void ServerConfig::setHttpPort(int port)
   {
      AutoLock lock(&m_objectCS);
      if (port > 65535) {
         m_httpPort = 65535;
      } else if (port < 0) {
         m_httpPort = 1;
      } else {
         m_httpPort = port;
      }
   }

   int ServerConfig::getHttpPort()
   {
      AutoLock lock(&m_objectCS);
      return m_httpPort;
   }

   void ServerConfig::enableFileTransfers(bool enabled)
   {
      AutoLock lock(&m_objectCS);
      m_enableFileTransfers = enabled;
   }

   bool ServerConfig::isFileTransfersEnabled()
   {
      AutoLock lock(&m_objectCS);
      return m_enableFileTransfers;
   }

   void ServerConfig::enableRemovingDesktopWallpaper(bool enabled)
   {
      AutoLock lock(&m_objectCS);
      m_removeWallpaper = enabled;
   }

   bool ServerConfig::isRemovingDesktopWallpaperEnabled()
   {
      AutoLock lock(&m_objectCS);
      return m_removeWallpaper;
   }

   void ServerConfig::setDisconnectAction(DisconnectAction action)
   {
      AutoLock lock(&m_objectCS);
      m_disconnectAction = action;
   }

   ServerConfig::DisconnectAction ServerConfig::getDisconnectAction()
   {
      AutoLock lock(&m_objectCS);
      return m_disconnectAction;
   }

   bool ServerConfig::getD3DIsAllowed()
   {
      AutoLock lock(&m_objectCS);
      return m_D3DAllowed;
   }

   void ServerConfig::setD3DAllowing(bool value)
   {
      AutoLock lock(&m_objectCS);
      m_D3DAllowed = value;
   }

   bool ServerConfig::getMirrorIsAllowed()
   {
      AutoLock lock(&m_objectCS);
      return m_mirrorDriverAllowed;
   }

   void ServerConfig::setMirrorAllowing(bool value)
   {
      AutoLock lock(&m_objectCS);
      m_mirrorDriverAllowed = value;
   }

   bool ServerConfig::isAcceptingRfbConnections()
   {
      AutoLock lock(&m_objectCS);
      return m_acceptRfbConnections;
   }

   void ServerConfig::acceptRfbConnections(bool accept)
   {
      AutoLock lock(&m_objectCS);
      m_acceptRfbConnections = accept;
   }

   void ServerConfig::getPrimaryPassword(unsigned char *password)
   {
      AutoLock lock(&m_objectCS);

      memcpy(password, m_primaryPassword, VNC_PASSWORD_SIZE);
   }

   void ServerConfig::setPrimaryPassword(const unsigned char *value)
   {
      AutoLock lock(&m_objectCS);

      m_hasPrimaryPassword = true;

      memcpy((void *)&m_primaryPassword[0], (void *)value, VNC_PASSWORD_SIZE);
   }

   void ServerConfig::getReadOnlyPassword(unsigned char *password)
   {
      AutoLock lock(&m_objectCS);

      memcpy(password, m_readonlyPassword, VNC_PASSWORD_SIZE);
   }

   void ServerConfig::setReadOnlyPassword(const unsigned char *value)
   {
      AutoLock lock(&m_objectCS);

      m_hasReadOnlyPassword = true;

      memcpy((void *)&m_readonlyPassword[0], (void *)value, VNC_PASSWORD_SIZE);
   }

   void ServerConfig::getControlPassword(unsigned char *password)
   {
      AutoLock lock(&m_objectCS);

      memcpy(password, m_controlPassword, VNC_PASSWORD_SIZE);
   }

   void ServerConfig::setControlPassword(const unsigned char *password)
   {
      AutoLock lock(&m_objectCS);

      memcpy((void *)&m_controlPassword[0], (const void *)password, VNC_PASSWORD_SIZE);

      m_hasControlPassword = true;
   }

   bool ServerConfig::hasPrimaryPassword()
   {
      AutoLock lock(&m_objectCS);

      return m_hasPrimaryPassword;
   }

   bool ServerConfig::hasReadOnlyPassword()
   {
      AutoLock lock(&m_objectCS);

      return m_hasReadOnlyPassword;
   }

   bool ServerConfig::hasControlPassword()
   {
      AutoLock lock(&m_objectCS);

      return m_hasControlPassword;
   }

   void ServerConfig::deletePrimaryPassword()
   {
      AutoLock lock(&m_objectCS);

      m_hasPrimaryPassword = false;
   }

   void ServerConfig::deleteReadOnlyPassword()
   {
      AutoLock lock(&m_objectCS);

      m_hasReadOnlyPassword = false;
   }

   void ServerConfig::deleteControlPassword()
   {
      AutoLock lock(&m_objectCS);

      m_hasControlPassword = false;
   }

   bool ServerConfig::isUsingAuthentication()
   {
      AutoLock lock(&m_objectCS);
      return m_useAuthentication;
   }

   void ServerConfig::useAuthentication(bool enabled)
   {
      AutoLock lock(&m_objectCS);
      m_useAuthentication = enabled;
   }

   bool ServerConfig::isOnlyLoopbackConnectionsAllowed()
   {
      AutoLock lock(&m_objectCS);
      return m_onlyLoopbackConnections;
   }

   void ServerConfig::acceptOnlyLoopbackConnections(bool enabled)
   {
      AutoLock lock(&m_objectCS);
      m_onlyLoopbackConnections = enabled;
   }

   bool ServerConfig::isAcceptingHttpConnections()
   {
      AutoLock lock(&m_objectCS);
      return m_acceptHttpConnections;
   }

   void ServerConfig::acceptHttpConnections(bool accept)
   {
      AutoLock lock(&m_objectCS);
      m_acceptHttpConnections = accept;
   }

   bool ServerConfig::isAppletParamInUrlEnabled()
   {
      AutoLock lock(&m_objectCS);
      return m_enableAppletParamInUrl;
   }

   void ServerConfig::enableAppletParamInUrl(bool enabled)
   {
      AutoLock lock(&m_objectCS);
      m_enableAppletParamInUrl = enabled;
   }

   int ServerConfig::getLogLevel()
   {
      AutoLock lock(&m_objectCS);
      return m_logLevel;
   }

   void ServerConfig::setLogLevel(int logLevel)
   {
      AutoLock lock(&m_objectCS);
      if (logLevel < 0) {
         m_logLevel = 0;
      } else if (logLevel > 10) {
         m_logLevel = 10;
      } else {
         m_logLevel = logLevel;
      }
   }

   bool ServerConfig::isAlwaysShared()
   {
      AutoLock lock(&m_objectCS);
      return m_alwaysShared;
   }

   bool ServerConfig::isNeverShared()
   {
      AutoLock lock(&m_objectCS);
      return m_neverShared;
   }

   bool ServerConfig::isDisconnectingExistingClients()
   {
      AutoLock lock(&m_objectCS);
      return m_disconnectClients;
   }

   void ServerConfig::setAlwaysShared(bool enabled)
   {
      AutoLock lock(&m_objectCS);
      m_alwaysShared = enabled;
   }

   void ServerConfig::setNeverShared(bool enabled)
   {
      AutoLock lock(&m_objectCS);
      m_neverShared = enabled;
   }

   void ServerConfig::disconnectExistingClients(bool disconnectExisting)
   {
      AutoLock lock(&m_objectCS);
      m_disconnectClients = disconnectExisting;
   }

   void ServerConfig::setPollingInterval(unsigned int interval)
   {
      AutoLock lock(&m_objectCS);
      if (interval < MINIMAL_POLLING_INTERVAL) {
         m_pollingInterval = MINIMAL_POLLING_INTERVAL;
      } else {
         m_pollingInterval = interval;
      }
   }

   unsigned int ServerConfig::getPollingInterval()
   {
      AutoLock lock(&m_objectCS);
      return m_pollingInterval;
   }

   void ServerConfig::blockRemoteInput(bool blockEnabled)
   {
      AutoLock lock(&m_objectCS);
      m_blockRemoteInput = blockEnabled;
   }

   bool ServerConfig::isBlockingRemoteInput()
   {
      AutoLock lock(&m_objectCS);
      return m_blockRemoteInput;
   }

   void ServerConfig::setLocalInputPriority(bool localPriority)
   {
      AutoLock lock(&m_objectCS);
      m_localInputPriority = localPriority;
   }

   bool ServerConfig::isLocalInputPriorityEnabled()
   {
      AutoLock lock(&m_objectCS);
      return m_localInputPriority;
   }

   unsigned int ServerConfig::getLocalInputPriorityTimeout()
   {
      AutoLock lock(&m_objectCS);
      return m_localInputPriorityTimeout;
   }

   void ServerConfig::setLocalInputPriorityTimeout(unsigned int value)
   {
      AutoLock lock(&m_objectCS);
      if (value < MINIMAL_LOCAL_INPUT_PRIORITY_TIMEOUT) {
         m_localInputPriorityTimeout = MINIMAL_LOCAL_INPUT_PRIORITY_TIMEOUT;
      } else {
         m_localInputPriorityTimeout = value;
      }
   }

   void ServerConfig::blockLocalInput(bool enabled)
   {
      AutoLock lock(&m_objectCS);
      m_blockLocalInput = enabled;
   }

   bool ServerConfig::isBlockingLocalInput()
   {
      AutoLock lock(&m_objectCS);
      return m_blockLocalInput;
   }

   unsigned int ServerConfig::getQueryTimeout()
   {
      AutoLock lock(&m_objectCS);
      return m_queryTimeout;
   }

   void ServerConfig::setQueryTimeout(unsigned int timeout)
   {
      AutoLock lock(&m_objectCS);
      if (timeout < MINIMAL_QUERY_TIMEOUT) {
         m_queryTimeout = MINIMAL_QUERY_TIMEOUT;
      } else {
         m_queryTimeout = timeout;
      }
   }

   bool ServerConfig::isDefaultActionAccept()
   {
      AutoLock lock(&m_objectCS);
      return m_defaultActionAccept;
   }

   void ServerConfig::setDefaultActionToAccept(bool accept)
   {
      AutoLock lock(&m_objectCS);
      m_defaultActionAccept = accept;
   }

   PortMappingContainer *ServerConfig::getPortMappingContainer()
   {
      return &m_portMappings;
   }

   //
   // Ip access control config
   //

   IpAccessControl *ServerConfig::getAccessControl()
   {
      return &m_accessControlContainer;
   }

   void ServerConfig::allowLoopbackConnections(bool allow)
   {
      AutoLock lock(&m_objectCS);
      m_allowLoopbackConnections = allow;
   }

   bool ServerConfig::isLoopbackConnectionsAllowed()
   {
      AutoLock l(&m_objectCS);

      return m_allowLoopbackConnections;
   }

   ::string_array *ServerConfig::getVideoClassNames()
   {
      return &m_videoClassNames;
   }

   unsigned int ServerConfig::getVideoRecognitionInterval()
   {
      AutoLock lock(&m_objectCS);
      return m_videoRecognitionInterval;
   }

   void ServerConfig::setVideoRecognitionInterval(unsigned int interval)
   {
      AutoLock lock(&m_objectCS);

      m_videoRecognitionInterval = interval;
   }

   ::int_rectangle_array_base *ServerConfig::getVideoRects()
   {
      return &m_videoRects;
   }

   int ServerConfig::getIdleTimeout()
   {
      AutoLock lock(&m_objectCS);
      return m_idleTimeout;
   }

   void ServerConfig::setIdleTimeout(int timeout)
   {
      AutoLock lock(&m_objectCS);
      m_idleTimeout = timeout;
   }

   void ServerConfig::saveLogToAllUsersPath(bool enabled)
   {
      AutoLock lock(&m_objectCS);

      m_saveLogToAllUsersPath = enabled;
   }

   bool ServerConfig::isSaveLogToAllUsersPathFlagEnabled()
   {
      AutoLock l(&m_objectCS);

      return m_saveLogToAllUsersPath;
   }

   void ServerConfig::setGrabTransparentWindowsFlag(bool grab)
   {
      AutoLock lock(&m_objectCS);
      m_grabTransparentWindows = grab;
   }

   bool ServerConfig::getGrabTransparentWindowsFlag()
   {
      AutoLock lock(&m_objectCS);
      return m_grabTransparentWindows;
   }
} // namespace remoting