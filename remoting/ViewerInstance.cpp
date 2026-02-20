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
#include "framework.h"
#include "ViewerInstance.h"

#include "remoting/remoting_common/viewer_core/RemoteViewerCore.h"
#include "remoting/remoting_common/viewer_core/FileTransferCapability.h"

ViewerInstance::ViewerInstance(WindowsApplication *application,
                               ConnectionData & conData,
                               const ConnectionConfig & conConf)
: m_conConf(conConf),
  m_condata(conData),
  m_socket(0),
  m_viewerWnd(application,
              &m_condata,
              &m_conConf,
              ::remoting::ViewerConfig::getInstance()->getLogWriter()),
  m_vncAuthHandler(&m_condata),
  m_viewerCore(::remoting::ViewerConfig::getInstance()->getLogWriter())
{
}

ViewerInstance::ViewerInstance(WindowsApplication *application,
                               ConnectionData & conData,
                               const ConnectionConfig & conConf,
                               SocketIPv4 *socket)
: m_conConf(conConf),
  m_condata(conData),
  m_socket(socket),
  m_viewerWnd(application,
              &m_condata,
              &m_conConf,
              ::remoting::ViewerConfig::getInstance()->getLogWriter()),
  m_vncAuthHandler(&m_condata),
  m_viewerCore(::remoting::ViewerConfig::getInstance()->getLogWriter())
{
}


ViewerInstance::~ViewerInstance()
{
  if (m_socket != 0) {
    m_socket->shutdown(SD_BOTH);
    m_socket->close();
  }

  m_viewerCore.stop();
  m_viewerCore.waitTermination();

  if (m_socket != 0) {
    delete m_socket;
    m_socket = NULL;
  }
}

void ViewerInstance::waitViewer()
{
  m_viewerCore.waitTermination();
}

bool ViewerInstance::requiresReconnect() const
{
  return m_viewerWnd.requiresReconnect();
}

bool ViewerInstance::isStopped() const
{
  return m_viewerWnd.isStopped();
}

void ViewerInstance::stop()
{
  m_viewerWnd.postMessage(ViewerWindow::WM_USER_STOP);
}

void ViewerInstance::start()
{
  auto LogWriter = ::remoting::ViewerConfig::getInstance()->getLogWriter();
  m_viewerWnd.setRemoteViewerCore(&m_viewerCore);


  m_viewerWnd.setFileTransfer(&m_fileTransfer);

  m_vncAuthHandler.addAuthCapability(&m_viewerCore);

  m_fileTransfer.addCapabilities(&m_viewerCore);

  if (m_socket) {
    m_viewerCore.start(m_socket,
                       &m_viewerWnd, m_conConf.getSharedFlag());
  } else {
    //::string strHost;
    auto strHost = m_condata.getReducedHost();
    unsigned short portVal = m_condata.getPort();
    m_viewerCore.start(strHost, portVal,
                       &m_viewerWnd, m_conConf.getSharedFlag());
  }
}
