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
#include "framework.h"
//#include "acme/_operating_system.h"
#include "RfbClient.h"
//#include "subsystem/thread/lockable_critical_section.h"
#include "RfbCodeRegistrator.h"
#include "remoting/remoting/file_transfer_node/FileTransferRequestHandler.h"
#include "EchoExtensionRequestHandler.h"
//#include "remoting/remoting/network/socket/SocketStream.h"
#include "RfbInitializer.h"
#include "ClientAuthListener.h"
#include "remoting/remoting/node_config/Configurator.h"
#include "remoting/remoting/node/NewConnectionEvents.h"
#include "acme/input_output/BufferedInputStream.h"
#include "subsystem/platform/MemUsage.h"

namespace remoting
{
   RfbClient::RfbClient(::remoting_node::NewConnectionEvents *newConnectionEvents,
                        SocketIPv4 *socket,
                        ClientTerminationListener *extTermListener,
                        ClientAuthListener *pclientauthlistener, bool viewOnly,
                        bool isOutgoing, unsigned int id,
                        const ViewPortState *constViewPort,
                        const ViewPortState *dynViewPort,
                        int idleTimeout,
                        ::subsystem::LogWriter * plogwriter)
   : m_socket(socket), // now we own the socket
     m_newConnectionEvents(newConnectionEvents),
     m_viewOnly(viewOnly),
     m_isOutgoing(isOutgoing),
     m_shared(false),
     m_viewOnlyAuth(true),
     m_clientState(IN_NONAUTH),
     m_isMarkedOk(false),
     m_extTermListener(extTermListener),
     m_extAuthListener(pclientauthlistener),
     m_pupdatesender(0),
     m_pclipboardexchange(0),
     m_pclientinputhandler(0),
     m_id(id),
     m_pdesktop(0),
     m_pviewportConst(constViewPort, log),
     m_pviewportDynamic(dynViewPort, log),
     m_demandtimerIdle(idleTimeout), m_idleTimeout(idleTimeout),
     m_plogwriter = plogwriter;
   {
      resume();
   }

   RfbClient::~RfbClient()
   {
      terminate();
      wait();
      delete m_socket;
   }

   void RfbClient::disconnect()
   {
      ::string peerStr;
      getPeerHost(&peerStr);
      // Shutdown and close socket.
      try { m_socket->shutdown(SD_BOTH); } catch (...) { }
      try { m_socket->close(); } catch (...) { }
      m_plogwriter->debug("Connection from {} has been closed for client #{}", peerStr, m_id);
   }

   unsigned int RfbClient::getId() const
   {
      return m_id;
   }

   bool RfbClient::isOutgoing() const
   {
      return m_isOutgoing;
   }

   void RfbClient::getPeerHost(::string & host)
   {
      SocketAddressIPv4 addr;

      if (m_socket->getPeerAddr(&addr)) {
         addr.toString(host);
      } else {
         // FIXME: This may occur if the close() function has been called.
         _ASSERT(false);

         host-= "unknown";
      }
   }

   void RfbClient::getLocalIpAddress(::string & address)
   {
      SocketAddressIPv4 addr;

      if (m_socket->getLocalAddr(&addr)) {
         addr.toString(address);
      } else {
         // FIXME: This may occur if the close() function has been called.
         _ASSERT(false);

         address-= "unknown";
      }
   }

   void RfbClient::getSocketAddr(SocketAddressIPv4 *addr) const
   {
      m_socket->getPeerAddr(addr);
   }

   void RfbClient::setClientState(ClientState newState)
   {
      critical_section_lock al(&m_criticalsectionClientState);
      if (newState > m_clientState) {
         m_clientState = newState;
      }
   }

   ClientState RfbClient::getClientState()
   {
      critical_section_lock al(&m_criticalsectionClientState);
      return m_clientState;
   }

   void RfbClient::setViewOnlyFlag(bool value)
   {
      if (getClientState() < IN_NORMAL_PHASE) {
         throw ::subsystem::Exception("Irrelevant call to RfbClient::setViewOnlyFlag()");
      }
      m_viewOnly = value || m_viewOnlyAuth;
      m_pclientinputhandler->setViewOnlyFlag(m_viewOnly);
   }

   void RfbClient::changeDynViewPort(const ViewPortState *dynViewPort)
   {
      critical_section_lock al(&m_criticalsectionViewport);
      m_pviewportDynamic->changeState(dynViewPort);
   }

   void RfbClient::notifyAbStateChanging(ClientState state)
   {
      setClientState(state);
      m_extTermListener->onClientTerminate();
   }

   void RfbClient::onTerminate()
   {
      disconnect();
   }

   void RfbClient::execute()
   {
      // Initialized by default scopedstrMessage that will be logged on normal way
      // of disconnection.
      ::string peerStr;
      getPeerHost(&peerStr);
      ::string sysLogMessage;
      sysLogMessage.formatf("The client {} has disconnected",
                           peerStr);

      ::remoting_node::ServerConfig * pserverconfig = m_pconfigurator->getServerConfig();

      SocketStream sockStream(m_socket);

      ::remoting::RfbOutputGate output(&sockStream);
      BufferedInputStream bufInput(&sockStream);
      ::remoting::RfbInputGate input(&bufInput);

      FileTransferRequestHandler *fileTransfer = 0;
      EchoExtensionRequestHandler *echoExtension = 0;

      RfbInitializer rfbInitializer(&sockStream, m_extAuthListener, this,
                                    !m_isOutgoing);

      try {
         // First initialization phase
         try {
            m_plogwriter->information("Entering RFB initialization phase 1");
            rfbInitializer.authPhase();
            setClientState(IN_AUTH);
            m_plogwriter->debug("RFB initialization phase 1 completed");

            m_shared = rfbInitializer.getSharedFlag();
            m_plogwriter->debug("Shared flag = {}", (int)m_shared);
            m_viewOnlyAuth = rfbInitializer.getViewOnlyAuth();
            m_plogwriter->debug("Initial view-only state = {}", (int)m_viewOnly);
            m_plogwriter->debug("Authenticated with view-only password = {}", (int)m_viewOnlyAuth);
            m_viewOnly = m_viewOnly || m_viewOnlyAuth;

            // Let RfbClientManager handle new authenticated connection.
            m_pdesktop = m_extAuthListener->onClientAuth(this);

            m_plogwriter->information("View only = {}", (int)m_viewOnly);
         } catch (::exception &e) {
            m_plogwriter->error("Error during RFB initialization: {}", e.get_message());
            throw;
         }
         _ASSERT(m_pdesktop != 0);

         m_pviewportConst->initDesktopInterface(m_pdesktop);
         m_pviewportDynamic->initDesktopInterface(m_pdesktop);

         RfbDispatcher dispatcher(&input, &m_connClosingEvent);
         m_plogwriter->debug("Dispatcher has been created");
         CapContainer srvToClCaps, clToSrvCaps, encCaps;
         RfbCodeRegistrator m_prfbcoderegistrator(&dispatcher, &srvToClCaps, &clToSrvCaps,
                                       &encCaps);
         // Init modules
         // UpdateSender initialization
         m_pupdatesender = new UpdateSender(&m_prfbcoderegistrator, m_pdesktop, this,
                                           &output, m_id, m_pdesktop, m_plogwriter);
         m_plogwriter->debug("UpdateSender has been created for client #{}", m_id);
         ::innate_subsystem::PixelFormat pixelformat;
         ::int_size sizeFramebuffer;
         m_pdesktop->getFramebufferProperties(&sizeFramebuffer, &pixelformat);
         ::int_rectangle rectangleViewport = getViewport(sizeFramebuffer);
         m_pupdatesender->init(::int_size(rectangleViewport.size()), pixelformat);
         m_plogwriter->debug("UpdateSender has been initialized");
         // ClientInputHandler initialization
         m_pclientinputhandler = new ClientInputHandler(&m_prfbcoderegistrator, this,
                                                       m_viewOnly);
         m_plogwriter->debug("ClientInputHandler has been created");
         // ClipboardExchange initialization
         m_pclipboardexchange = new ClipboardExchange(&m_prfbcoderegistrator, m_pdesktop, &output,
                                                     m_viewOnly, m_plogwriter);
         m_plogwriter->debug("ClipboardExchange has been created");

         // FileTransfers initialization
         if (config->isFileTransfersEnabled() &&
             rfbInitializer.getTightEnabledFlag()) {
            fileTransfer = new FileTransferRequestHandler(&m_prfbcoderegistrator, &output, m_pdesktop, m_plogwriter, !m_viewOnly);
            m_plogwriter->debug("::file::item transfer has been created");
             } else {
                m_plogwriter->information("::file::item transfer is not allowed");
             }
         // echo extension initialization
         echoExtension = new EchoExtensionRequestHandler(&m_prfbcoderegistrator, &output, m_plogwriter);
         m_plogwriter->debug("Echo extension handler has been created");

         // Second initialization phase
         // Send and receive initialization information between server and viewer
         m_plogwriter->debug("View port: ({},{}) (%dx{})", rectangleViewport.left,
                                                      rectangleViewport.top,
                                                      rectangleViewport.width(),
                                                      rectangleViewport.height());
         m_plogwriter->information("Entering RFB initialization phase 2");
         rfbInitializer.afterAuthPhase(&srvToClCaps, &clToSrvCaps,
                                       &encCaps, &::int_size(&rectangleViewport), &pixelformat);
         m_plogwriter->debug("RFB initialization phase 2 completed");

         // Start normal phase
         setClientState(IN_NORMAL_PHASE);

         m_plogwriter->information("Entering normal phase of the RFB protocol");
         dispatcher.resume();

         m_connClosingEvent.wait();
      } catch (::exception &e) {
         m_plogwriter->error("Connection will be closed: {}", e.get_message());
         sysLogMessage.formatf("The client {} #{} has been"
                              " disconnected for the reason: {}",
                              peerStr, m_id, e.get_message());
      }

      disconnect();
      m_newConnectionEvents->onDisconnect(&sysLogMessage);

      // After this call, we are guaranteed not to be used by other threads.
      notifyAbStateChanging(IN_PENDING_TO_REMOVE);

      if (fileTransfer)         delete fileTransfer;
      if (echoExtension)        delete echoExtension;
      if (m_pclipboardexchange)  delete m_pclipboardexchange;
      if (m_pclientinputhandler) delete m_pclientinputhandler;
      if (m_pupdatesender)       delete m_pupdatesender;

      // Let the client manager remove us from the client lists.
      notifyAbStateChanging(IN_READY_TO_REMOVE);
      m_plogwriter->debug("End of RfbClient, process memory usage: {} ", MemUsage::getCurrentMemUsage());
   }

   void RfbClient::sendUpdate(const UpdateContainer & updatecontainer,
                              const CursorShape *cursorShape)
   {
      m_pupdatesender->newUpdates(updatecontainer, cursorShape);

      if (m_idleTimeout != 0  && m_demandtimerIdle.isElapsed()) {
         m_plogwriter->error("Connection will be closed due to client inactivity. IdleTimeout = {} ms", m_idleTimeout);
         m_connClosingEvent.set_happening();
      }
   }

   void RfbClient::sendClipboard(const ::scoped_string & newClipboard)
   {
      m_pclipboardexchange->sendClipboard(newClipboard);
   }

   void RfbClient::onKeyboardEvent(unsigned int keySym, bool down)
   {
      // FIXME: How to deal with the situations when we inject a "key down" event, then foreground
      //        window changes and is no longer owned by the shared app so we cannot pass "key up"
      //        to the desktop?

      bool mayPass = true;
      bool shareApp = m_pviewportDynamic->getOnlyApplication();
      if (shareApp) {
         unsigned int pid = m_pviewportDynamic->getApplicationId();
         mayPass = m_pdesktop->isApplicationInFocus(pid);
      }

      if (mayPass) {
         m_pdesktop->setKeyboardEvent(keySym, down);
         m_demandtimerIdle.reset();
      }
   }

   void RfbClient::onMouseEvent(unsigned short x, unsigned short y, unsigned char buttonMask)
   {
      // FIXME: Too much extra work. Typically we would share the whole desktop and would not need
      //        to compute regions on each mouse move.

      ::innate_subsystem::PixelFormat pixelformatStub;
      ::int_size sizeFramebuffer;
      m_pdesktop->getFramebufferProperties(&sizeFramebuffer, &pixelformatStub);

      ::int_rectangle rectangleViewport;
      bool shareApp;
      Region sharedRegion;
      getViewPortInfo(&sizeFramebuffer, &rectangleViewport, &shareApp, &sharedRegion);

      if (!shareApp) {
         sharedRegion.clear();
         sharedRegion.addRect(&rectangleViewport);
      }
      bool pointInside = sharedRegion.isPointInside(x + rectangleViewport.left, y + rectangleViewport.top);

      if (pointInside) {
         m_pupdatesender->blockCursorPosSending();
         m_pdesktop->setMouseEvent(x + rectangleViewport.left, y + rectangleViewport.top, buttonMask);
         m_demandtimerIdle.reset();
      }
   }

   ::int_rectangle RfbClient::getViewport(const ::int_size & sizeFramebuffer)
   {
      critical_section_lock al(&m_criticalsectionViewport);
      m_pviewportConst->update(sizeFramebuffer);
      m_pviewportDynamic->update(sizeFramebuffer);

      return m_pviewportConst->getViewport().intersection(
        m_pviewportDynamic->getViewport());
   }

   void RfbClient::getViewPortInfo(const ::int_size & sizeFramebuffer, ::int_rectangle &rectangleResult,
                                   bool *shareApp, Region & regionShareApp)
   {
      critical_section_lock al(&m_criticalsectionViewport);

      *rectangleResult = getViewport(sizeFramebuffer);
      *shareApp = m_pviewportDynamic->getOnlyApplication();
      if (*shareApp) {
         m_pviewportDynamic->getApplicationRegion(regionShareApp);
      }
   }

   void RfbClient::onGetViewPort(::int_rectangle &viewRect, bool *shareApp, Region & regionShareApp)
   {
      ::innate_subsystem::PixelFormat pixelformatStub;
      ::int_size sizeFramebuffer;
      m_pdesktop->getFramebufferProperties(&sizeFramebuffer, &pixelformatStub);
      getViewPortInfo(&sizeFramebuffer, viewRect, shareApp, regionShareApp);
   }
} // namespace remoting


