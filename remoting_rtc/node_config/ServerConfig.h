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


//#include "subsystem/platform/::string_array.h"
#include "subsystem/platform/Exception.h"
#include "PortMappingContainer.h"
#include "IpAccessControl.h"
//#include "subsystem/thread/lockable_critical_section.h"
//#include "subsystem/thread/lockable_critical_section.h"
#include "IpAccessRule.h"
#include "acme/input_output/DataInputStream.h"
#include "acme/input_output/DataOutputStream.h"
#include "acme/exception/io.h"
#include "subsystem/platform/VncPassCrypt.h"
#include "remoting/remoting_rtc/region/RectSerializer.h"
#include "subsystem/thread/Lockable.h"
// #include aaa_<shlobj.h>

namespace remoting_rtc_node
{
   class CLASS_DECL_REMOTING ServerConfig : virtual public ::subsystem::LockableInterface
   {
   public:
      static const ::u32 MINIMAL_POLLING_INTERVAL = 30;
      static const ::u32 MINIMAL_LOCAL_INPUT_PRIORITY_TIMEOUT = 1;
      static const ::u32 MINIMAL_QUERY_TIMEOUT = 1;

      // FIXME: duplicatad at VncPassCrypt
      ///static const int ::subsystem::VncPassCrypt::VNC_PASSWORD_SIZE = 8;

      //
      // Enum defines server action when last client disconnects
      // from the Remoting Node.
      //

      enum DisconnectAction {
         DA_DO_NOTHING = 0,
         DA_LOCK_WORKSTATION = 1,
         DA_LOGOUT_WORKSTATION = 2
       };

         //protected:

      //
      // Server port numbers
      //

      int m_rfbPort;
      int m_httpPort;

      //
      // Other server options members group
      //

      bool m_enableFileTransfers;
      bool m_removeWallpaper;
      bool m_D3DAllowed;
      bool m_mirrorDriverAllowed;
      //
      // Server action when last client disconnects from server
      //

      DisconnectAction m_disconnectAction;

      //
      // Incoming connections options group
      //

      bool m_acceptRfbConnections;
      bool m_acceptHttpConnections;

      unsigned char m_primaryPassword[::subsystem::VncPassCrypt::VNC_PASSWORD_SIZE];
      unsigned char m_readonlyPassword[::subsystem::VncPassCrypt::VNC_PASSWORD_SIZE];
      unsigned char m_controlPassword[::subsystem::VncPassCrypt::VNC_PASSWORD_SIZE];

      //
      // Configurator from Administration tab
      //

      bool m_useAuthentication;
      bool m_onlyLoopbackConnections;
      bool m_enableAppletParamInUrl;
      int m_logLevel;
      bool m_useControlAuth;
      bool m_controlAuthAlwaysChecking;

      //
      // Sharing configuration
      //

      bool m_alwaysShared;
      bool m_neverShared;
      bool m_disconnectClients;

      //
      // Polling configuration
      //

      ::u32 m_pollingInterval;

      //
      // When flag is set server always blocks remote input.
      //

      bool m_blockRemoteInput;
      //
      // When flag is set server always blocks local input.
      //

      bool m_blockLocalInput;

      //
      // When flag is set server blocks remote input
      // on local input activity.
      //

      bool m_localInputPriority;

      //
      // Local input invactivity timeout during that
      // we still blocking remote input(when m_localInputPriority
      // is enabled).
      //

      ::u32 m_localInputPriorityTimeout;

      bool m_defaultActionAccept;

      //
      // Timeout for Query IpAccessControl record
      //

      ::u32 m_queryTimeout;

      //
      // Port mapping config
      //

      PortMappingContainer m_portMappings;

      //
      // Ip access control config
      //

      IpAccessControl m_accessControlContainer;
      bool m_allowLoopbackConnections;

      //
      // Video regions
      //

      // Defined by window class CLASS_DECL_REMOTING name
      ::string_array m_videoClassNames;
      // Defined by rectangle coords in "dXxdY+X0+Y0" format, as in -sharerect command line option
      ::int_rectangle_array_base m_videoRects;

      //
      // Other
      //

      ::u32 m_videoRecognitionInterval;
      bool m_grabTransparentWindows;

      // Socket timeout to disconnect inactive clients, in seconds
      int m_idleTimeout;

      // Flag that determiates where log file directory will be.
      bool m_saveLogToAllUsersPath;
      // Run control interface with Remoting Node or not.
      bool m_showTrayIcon;
      // Connect to existing RDP session or drop it.
      bool m_connectToRdp;

      ::string m_logFilePath;
   //private:

      //
      // Helper methods
      //

      bool m_hasPrimaryPassword;
      bool m_hasReadOnlyPassword;
      bool m_hasControlPassword;

      //
      // Critical section
      //

      lockable_critical_section m_objectCS;


   public:
      ServerConfig();
      virtual ~ServerConfig();

      ServerConfig(ServerConfig& other);
      ServerConfig& operator=(ServerConfig& other);

      /**
       * Serializes server config to output stream as byte stream.
       * Thread-safe method.
       * @throws ::subsystem::Exception on io error.
       * @fixme stub.
       */
      void serialize(DataOutputStream * pdataoutputstream);

      /**
       * Deserializes server config from input stream.
       * Thread-safe method.
       * @throws ::subsystem::Exception on io error.
       * @fixme stub.
       */
      void deserialize(DataInputStream * pinput);

      //
      // Inherited from lockable abstract class.
      //

      virtual ::e_status lock() {
         m_objectCS.lock();
         return ::success;
      }

      virtual void unlock() {
         m_objectCS.unlock();
      }

      //
      // Display (or port numbers) group.
      //

      bool isControlAuthEnabled();
      void useControlAuth(bool useAuth);

      bool getControlAuthAlwaysChecking();
      void setControlAuthAlwaysChecking(bool value);

      void setRfbPort(int port);
      int getRfbPort();

      void setHttpPort(int port);
      int getHttpPort();

      //
      // Other server options access methods
      //

      void enableFileTransfers(bool enabled);
      bool isFileTransfersEnabled();

      void enableRemovingDesktopWallpaper(bool enabled);
      bool isRemovingDesktopWallpaperEnabled();

      void setDisconnectAction(DisconnectAction action);
      DisconnectAction getDisconnectAction();

      bool getD3DIsAllowed();
      void setD3DAllowing(bool value);

      bool getMirrorIsAllowed();
      void setMirrorAllowing(bool value);

      //
      // Incoming connections options group
      //

      bool isAcceptingRfbConnections();
      void acceptRfbConnections(bool accept);

      void getPrimaryPassword(unsigned char *password);
      void setPrimaryPassword(const unsigned char *value);

      void getReadOnlyPassword(unsigned char *password);
      void setReadOnlyPassword(const unsigned char *value);

      void getControlPassword(unsigned char *password);
      void setControlPassword(const unsigned char *password);

      bool hasPrimaryPassword();
      bool hasReadOnlyPassword();
      bool hasControlPassword();

      void deletePrimaryPassword();
      void deleteReadOnlyPassword();
      void deleteControlPassword();

      //
      // Configurator from Administration tab
      //

      bool isUsingAuthentication();

      void useAuthentication(bool enabled);

      bool isOnlyLoopbackConnectionsAllowed();

      void acceptOnlyLoopbackConnections(bool enabled);

      bool isAcceptingHttpConnections();

      void acceptHttpConnections(bool accept);

      bool isAppletParamInUrlEnabled();

      void enableAppletParamInUrl(bool enabled);

      int getLogLevel();

      void setLogLevel(int logLevel);

      //
      // Sharing configuration
      //

      bool isAlwaysShared();

      bool isNeverShared();

      bool isDisconnectingExistingClients();

      void setAlwaysShared(bool enabled);

      void setNeverShared(bool enabled);

      void disconnectExistingClients(bool disconnectExisting);

      void setPollingInterval(::u32 interval);

      ::u32 getPollingInterval();

      //
      // Input handling config
      //

      void blockRemoteInput(bool blockEnabled);

      bool isBlockingRemoteInput();

      void setLocalInputPriority(bool localPriority);

      bool isLocalInputPriorityEnabled();

      ::u32 getLocalInputPriorityTimeout();

      void setLocalInputPriorityTimeout(::u32 value);

      void blockLocalInput(bool enabled);

      bool isBlockingLocalInput();

      //
      // Query config
      //

      ::u32 getQueryTimeout();

      void setQueryTimeout(::u32 timeout);

      bool isDefaultActionAccept();
      void setDefaultActionToAccept(bool accept);

      //
      // Port mapping config
      //

      // Remark: not-thread safe method, use lock / unlock methods of this class
      // to lock and unlock server configuration.
      PortMappingContainer *getPortMappingContainer();

      //
      // Ip access control config
      //

      // Remark: not-thread safe method, use lock / unlock methods of this class
      // to lock and unlock server configuration.
      IpAccessControl *getAccessControl();

      IpAccessRule::ActionType getActionByAddress(unsigned long ip);

      void allowLoopbackConnections(bool allow);

      bool isLoopbackConnectionsAllowed();

      //
      // Video regions
      //

      // FIXME: Deprecated?
      // Remark: not-thread safe method, use lock / unlock methods of this class
      // to lock and unlock server configuration.
      ::string_array *getVideoClassNames();

      ::int_rectangle_array_base *getVideoRects();

      //
      // Other
      //

      ::u32 getVideoRecognitionInterval();
      void setVideoRecognitionInterval(::u32 interval);

      int  getIdleTimeout();
      void setIdleTimeout(int timeout);

      void saveLogToAllUsersPath(bool enabled);
      bool isSaveLogToAllUsersPathFlagEnabled();

      void setGrabTransparentWindowsFlag(bool grab);
      bool getGrabTransparentWindowsFlag();

      bool getShowTrayIconFlag();
      void setShowTrayIconFlag(bool val);

      bool getConnectToRdpFlag();
      void setConnectToRdpFlag(bool val);

      void getLogFileDir(::string & logFileDir);
      void setLogFileDir(const ::scoped_string & scopedstrLogFileDir);

   // //protected:
   //
   //    //
   //    // Server port numbers
   //    //
   //
   //    int m_rfbPort;
   //    int m_httpPort;
   //
   //    //
   //    // Other server options members group
   //    //
   //
   //    bool m_enableFileTransfers;
   //    bool m_removeWallpaper;
   //    bool m_D3DAllowed;
   //    bool m_mirrorDriverAllowed;
   //    //
   //    // Server action when last client disconnects from server
   //    //
   //
   //    DisconnectAction m_disconnectAction;
   //
   //    //
   //    // Incoming connections options group
   //    //
   //
   //    bool m_acceptRfbConnections;
   //    bool m_acceptHttpConnections;
   //
   //    unsigned char m_primaryPassword[::subsystem::VncPassCrypt::VNC_PASSWORD_SIZE];
   //    unsigned char m_readonlyPassword[::subsystem::VncPassCrypt::VNC_PASSWORD_SIZE];
   //    unsigned char m_controlPassword[::subsystem::VncPassCrypt::VNC_PASSWORD_SIZE];
   //
   //    //
   //    // Configurator from Administration tab
   //    //
   //
   //    bool m_useAuthentication;
   //    bool m_onlyLoopbackConnections;
   //    bool m_enableAppletParamInUrl;
   //    int m_logLevel;
   //    bool m_useControlAuth;
   //    bool m_controlAuthAlwaysChecking;
   //
   //    //
   //    // Sharing configuration
   //    //
   //
   //    bool m_alwaysShared;
   //    bool m_neverShared;
   //    bool m_disconnectClients;
   //
   //    //
   //    // Polling configuration
   //    //
   //
   //    ::u32 m_pollingInterval;
   //
   //    //
   //    // When flag is set server always blocks remote input.
   //    //
   //
   //    bool m_blockRemoteInput;
   //    //
   //    // When flag is set server always blocks local input.
   //    //
   //
   //    bool m_blockLocalInput;
   //
   //    //
   //    // When flag is set server blocks remote input
   //    // on local input activity.
   //    //
   //
   //    bool m_localInputPriority;
   //
   //    //
   //    // Local input invactivity timeout during that
   //    // we still blocking remote input(when m_localInputPriority
   //    // is enabled).
   //    //
   //
   //    ::u32 m_localInputPriorityTimeout;
   //
   //    bool m_defaultActionAccept;
   //
   //    //
   //    // Timeout for Query IpAccessControl record
   //    //
   //
   //    ::u32 m_queryTimeout;
   //
   //    //
   //    // Port mapping config
   //    //
   //
   //    PortMappingContainer m_portMappings;
   //
   //    //
   //    // Ip access control config
   //    //
   //
   //    IpAccessControl m_accessControlContainer;
   //    bool m_allowLoopbackConnections;
   //
   //    //
   //    // Video regions
   //    //
   //
   //    // Defined by window class CLASS_DECL_REMOTING name
   //    ::string_array m_videoClassNames;
   //    // Defined by rectangle coords in "dXxdY+X0+Y0" format, as in -sharerect command line option
   //    ::int_rectangle_array_base m_videoRects;
   //
   //    //
   //    // Other
   //    //
   //
   //    ::u32 m_videoRecognitionInterval;
   //    bool m_grabTransparentWindows;
   //
   //    // Socket timeout to disconnect inactive clients, in seconds
   //    int m_idleTimeout;
   //
   //    // Flag that determiates where log file directory will be.
   //    bool m_saveLogToAllUsersPath;
   //    // Run control interface with Remoting Node or not.
   //    bool m_showTrayIcon;
   //    // Connect to existing RDP session or drop it.
   //    bool m_connectToRdp;
   //
   //    ::string m_logFilePath;
   // private:
   //
   //    //
   //    // Helper methods
   //    //
   //
   //    bool m_hasPrimaryPassword;
   //    bool m_hasReadOnlyPassword;
   //    bool m_hasControlPassword;
   //
   //    //
   //    // Critical section
   //    //
   //
   //    lockable_critical_section m_objectCS;
   };
}  //  namespace remoting_rtc_node



