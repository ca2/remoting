// Copyright (C) 2012 GlavSoft LLC.
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
#include "ViewerInstance.h"
#include "remoting/rfb_client/remoting.h"
#include "remoting/remoting_rfb/viewer_core/FileTransferCapability.h"
#include "remoting/remoting_rfb/viewer_core/RemoteViewerCore.h"
#include "subsystem/socket/SocketIPv4.h"


namespace remoting_rfb_client
{
   
   
   ViewerInstance::ViewerInstance(subsystem::OperatingSystemApplicationInterface *papplication,
                                  ::remoting_rfb_client::remoting *premoting, ConnectionData *pconnectiondata,
                                  ::remoting_rfb::ConnectionConfig *pconnectionconfig) :
      m_pconnectionconfig(pconnectionconfig), 
      m_pconnectiondata(pconnectiondata),
      m_premoting(premoting)
   {

      m_pvncauthhandler = allocateø ViewerVncAuthHandler(m_pconnectiondata);

      m_pviewercore = allocateø ::remoting_rfb_client::RemoteViewerCore(premoting->m_plogwriter);

      m_pviewerwindow = allocateø ViewerWindow(papplication, premoting, m_pconnectiondata, m_pconnectionconfig,
                                               premoting->m_plogwriter);
   }


   ViewerInstance::ViewerInstance(subsystem::OperatingSystemApplicationInterface *papplication,
                                  ::remoting_rfb_client::remoting *premoting, ConnectionData *pconnectiondata,
                                  ::remoting_rfb::ConnectionConfig *pconnectionconfig,
                                  ::subsystem::SocketIPv4Interface *psocket) :
      m_pconnectionconfig(pconnectionconfig),
      m_pconnectiondata(pconnectiondata), 
      m_psocket(psocket),
      m_premoting(premoting)
   {

      m_pvncauthhandler = allocateø ViewerVncAuthHandler(m_pconnectiondata);

      m_pviewercore = allocateø ::remoting_rfb_client::RemoteViewerCore(premoting->m_plogwriter);

      m_pviewerwindow = allocateø ViewerWindow(papplication, premoting, m_pconnectiondata, m_pconnectionconfig,
                                               premoting->m_plogwriter);
   }


   ViewerInstance::~ViewerInstance()
   {
      if (m_psocket != 0)
      {
         m_psocket->shutdown(::subsystem::e_socket_shutdown_both);
         m_psocket->close();
      }

      m_pviewercore->stop();
      m_pviewercore->waitTermination();

      ::release(m_psocket);
      // if (m_psocket != 0) {
      //     delete m_psocket;
      //     m_psocket = NULL;
      // }
   }

   void ViewerInstance::waitViewer() { m_pviewercore->waitTermination(); }

   bool ViewerInstance::requiresReconnect() const { return m_pviewerwindow->requiresReconnect(); }

   bool ViewerInstance::isStopped() const { return m_pviewerwindow->isStopped(); }

   void ViewerInstance::stop() { m_pviewerwindow->postMessage(ViewerWindow::WM_USER_STOP); }


   void ViewerInstance::start()
   {

      auto plogwriter = m_premoting->m_plogwriter;

      m_pviewerwindow->setRemoteViewerCore(m_pviewercore);

      m_pviewerwindow->setFileTransfer(&m_fileTransfer);

      m_pvncauthhandler->addAuthCapability(m_pviewercore);

      m_fileTransfer.addCapabilities(m_pviewercore);

      if (m_psocket)
      {

         m_pviewercore->start(m_psocket, m_pviewerwindow, m_pconnectionconfig->getSharedFlag());
      }
      else
      {

         //::string strHost;
         auto strHost = m_pconnectiondata->getReducedHost();
         ::u16 portVal = m_pconnectiondata->getPort();
         m_pviewercore->start(strHost, portVal, m_pviewerwindow, m_pconnectionconfig->getSharedFlag());
      }
   }

} // namespace remoting_rfb_client
