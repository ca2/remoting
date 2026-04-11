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


#include "remoting/remoting/ViewerWindow.h"
//#include "remoting/remoting_common/network/socket/SocketIPv4.h"
#include "remoting/remoting_common/viewer_core/RemoteViewerCore.h"
#include "ViewerVncAuthHandler.h"


namespace remoting_remoting
{
    class ViewerInstance
    {
    public:
        // creates the viewer instance by using host:port from condata
        ViewerInstance(subsystem::OperatingSystemApplicationInterface *application,
                       ConnectionData & condata,
                       const ::remoting::ConnectionConfig & conConf);

        // creates the viewer instance if we have the socket
        ViewerInstance(subsystem::OperatingSystemApplicationInterface *application,
                       ConnectionData & condata,
                       const ::remoting::ConnectionConfig & conConf,
                       ::subsystem::SocketIPv4Interface *socket);

        virtual ~ViewerInstance();

        bool requiresReconnect() const;
        bool isStopped() const;
        void start();
        void stop();

        // wait while viewer is not terminated
        void waitViewer();

    protected:
        ConnectionData m_condata;
        ::remoting::ConnectionConfig m_pconnectionconfig;

        ViewerWindow m_viewerWnd;
        ::remoting::RemoteViewerCore m_viewerCore;
        ViewerVncAuthHandler m_vncAuthHandler;
        ::remoting::ftp::FileTransferCapability m_fileTransfer;
        ::subsystem::SocketIPv4Interface *m_socket;
    };
} // namespace remoting_remoting
