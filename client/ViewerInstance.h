// Copyright (C) 2012 GlavSoft LLC.
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


#include "remoting/client/ViewerWindow.h"
//#include "remoting/remoting/network/socket/SocketIPv4.h"
#include "remoting/remoting/viewer_core/RemoteViewerCore.h"
#include "ViewerVncAuthHandler.h"


namespace remoting_client
{
   

   class CLASS_DECL_REMOTING_CLIENT ViewerInstance :
      virtual public ::particle
   {
   public:

      ::pointer<::remoting_client::remoting> m_premoting;
      ::pointer<ConnectionData> m_pconnectiondata;
      ::pointer<::remoting::ConnectionConfig> m_pconnectionconfig;

      ::pointer<ViewerWindow> m_pviewerwindow;
      ::pointer<::remoting_client::RemoteViewerCore> m_pviewercore;
      ::pointer < ViewerVncAuthHandler > m_pvncauthhandler;
      ::remoting_client::file_transfer::FileTransferCapability m_fileTransfer;
      ::pointer<::subsystem::SocketIPv4Interface> m_psocket;


      // creates the viewer instance by using host:port from condata
      ViewerInstance(
         ::subsystem::OperatingSystemApplicationInterface *papplication,
         ::remoting_client::remoting * premoting,
         ::remoting_client::ConnectionData * pconnectiondata,
         ::remoting::ConnectionConfig * pconnectionconfig);

      // creates the viewer instance if we have the socket
      ViewerInstance(
         ::subsystem::OperatingSystemApplicationInterface *papplication,
         ::remoting_client::remoting *premoting, 
         ::remoting_client::ConnectionData *pconnectiondata,
         ::remoting::ConnectionConfig *pconnectionconfig,
         ::subsystem::SocketIPv4Interface *psocket);

      ~ViewerInstance() override;

      bool requiresReconnect() const;
      bool isStopped() const;
      void start();
      void stop();

      // wait while viewer is not terminated
      void waitViewer();

    };


} // namespace remoting_client



