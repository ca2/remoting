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
#include "subsystem/socket/SocketIPv4.h"
//#include "remoting/remoting/network/socket/SocketStream.h"
#include "RfbInitializer.h"
#include "ClientAuthListener.h"
#include "remoting/remoting/node_config/Configurator.h"
#include "remoting/remoting/node/NewConnectionEvents.h"
#include "acme/input_output/BufferedInputStream.h"
#include "subsystem/platform/MemUsage.h"
#include "subsystem/socket/SocketStream.h"


namespace remoting
{
   RfbClient::RfbClient(::remoting_node::Configurator * pconfigurator,
      ::remoting_node::NewConnectionEvents *pnewconnectionevents,
                        ::subsystem::SocketIPv4Interface *psocket,
                        ClientTerminationListener *pclientterminationlistener,
                        ClientAuthListener *pclientauthlistener,
                        bool viewOnly,
                        bool isOutgoing, ::u32 id,
                        const ViewPortState & viewportstateConst,
                        const ViewPortState & viewportstateDynamic,
                        int idleTimeout,
                        ::subsystem::LogWriter * plogwriter)
   :m_pconfigurator(pconfigurator),
   m_psocket(psocket), // now we own the socket
     m_pnewconnectionevents(pnewconnectionevents),
     m_viewOnly(viewOnly),
     m_isOutgoing(isOutgoing),
     m_shared(false),
     m_viewOnlyAuth(true),
     m_clientState(IN_NONAUTH),
     m_isMarkedOk(false),
     m_pclientterminationlistener(pclientterminationlistener),
     m_pclientauthlistener(pclientauthlistener),
     //m_pupdatesender(0),
     //m_pclipboardexchange(0),
     //m_pclientinputhandler(0),
     m_id(id),
     m_pdesktop(0),
     m_demandtimerIdle(idleTimeout), m_idleTimeout(idleTimeout),
     m_plogwriter(plogwriter)
   {

      construct_newø(m_pviewportConst);
      m_pviewportConst->initialize_viewport(viewportstateConst, plogwriter);

      construct_newø(m_pviewportDynamic);
      m_pviewportDynamic->initialize_viewport(viewportstateDynamic, plogwriter);

      resumeThread();
   }

   RfbClient::~RfbClient()
   {
      //setThreadToFinish();
      //waitThreadToFinish();
      //delete m_psocket;
   }


void RfbClient::destroy()
{
   ::subsystem::Thread::destroy();
   // terminateThread();
   // waitThreadToFinish();
   //delete m_psocket;
}
   void RfbClient::disconnect()
   {
      ::string peerStr;
      getPeerHost(peerStr);
      // Shutdown and close socket.
      try { m_psocket->shutdown(::subsystem::e_socket_shutdown_both); } catch (...) { }
      try { m_psocket->close(); } catch (...) { }
      m_plogwriter->debug("Connection from {} has been closed for client #{}", peerStr, m_id);
   }

   ::u32 RfbClient::getId() const
   {
      return m_id;
   }

   bool RfbClient::isOutgoing() const
   {
      return m_isOutgoing;
   }


   void RfbClient::getPeerHost(::string & host)
   {

      auto paddrPeer = m_psocket->getPeerAddr();

      if (paddrPeer)
      {
         host = paddrPeer->toString();
      }
      else
      {
         // FIXME: This may occur if the close() function has been called.
         ASSERT(false);

         host = "(unknown)";

      }

   }


   void RfbClient::getLocalIpAddress(::string & address)
   {

      auto paddrLocal = m_psocket->getLocalAddr();


      if (paddrLocal) {
         address = paddrLocal->toString();
      } else {
         // FIXME: This may occur if the close() function has been called.
         ASSERT(false);

         address = "(unknown)";
      }
   }

   ::pointer < ::subsystem::SocketAddressIPv4Interface > RfbClient::getSocketAddr() const
   {
      auto paddr = m_psocket->getPeerAddr();

      return paddr;
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
      m_pclientterminationlistener->onClientTerminate();
   }

   void RfbClient::onTermThread()
   {
      disconnect();
   }

   class rfb_client_run :
      virtual public ::particle
   {
   public:

      ::pointer < RfbClient > m_prfbclient;

      ::pointer < ::subsystem::SocketStream >  m_psockstream;

      ::pointer < ::remoting::RfbOutputGate > m_prfboutputgate;
      ::pointer < BufferedInputStream > m_pbufferedinput;
      ::pointer < ::remoting::RfbInputGate > m_prfbinputgate;

      ::pointer < file_transfer::FileTransferRequestHandler > m_pfiletransfer;
      ::pointer < file_transfer::EchoExtensionRequestHandler > m_pechoextension;

      ::pointer < RfbInitializer > m_prfbinitializer;

      CapContainer srvToClCaps, clToSrvCaps, encCaps;

      ::pointer < RfbDispatcher > m_prfbdispatcher;
      auto_pointer < RfbCodeRegistrator > m_prfbcoderegistrator;


      ::innate_subsystem::PixelFormat m_pixelformat;
      ::i32_size m_sizeFramebuffer;
      ::i32_rectangle m_rectangleViewport;

      ::pointer < file_transfer::FileTransferRequestHandler > m_pfiletransferrequesthandler;
      ::pointer < file_transfer::EchoExtensionRequestHandler > m_pechoextensionrequesthandler;


      rfb_client_run(RfbClient * prfbclient):
      m_prfbclient(prfbclient)
      {



         initialize(m_prfbclient);

         raw_construct_newø(m_psockstream, m_prfbclient->m_psocket);
         raw_construct_newø(m_prfboutputgate, m_psockstream);
         raw_construct_newø(m_pbufferedinput, m_psockstream);
         raw_construct_newø(m_prfbinputgate, m_pbufferedinput);
         raw_construct_newø(m_prfbinitializer);

         m_prfbinitializer->initialize_rfb_initializer(prfbclient->m_pconfigurator, m_psockstream, m_prfbclient->m_pclientauthlistener, m_prfbclient, !m_prfbclient->m_isOutgoing);

      }


   };

   void RfbClient::onThreadMain()
   {
      // Initialized by default scopedstrMessage that will be logged on normal way
      // of disconnection.
      ::string peerStr;
      getPeerHost(peerStr);
      ::string sysLogMessage;
      sysLogMessage.format("The client {} has disconnected",
                           peerStr);

      auto pserverconfig = m_pconfigurator->getServerConfig();

      raw_construct_newø(m_prun, this);

      // ::subsystem::SocketStream sockStream(m_psocket);
      //
      // construct_newø(m_prun->m_prfboutputgate);
      //
      // ::remoting::RfbOutputGate m_prfpoutputgate(&sockStream);
      // BufferedInputStream bufInput(&sockStream);
      // ::remoting::RfbInputGate input(&bufInput);

      // ::pointer < FileTransferRequestHandler > m_pfiletransferrequesthandler;
      // ::pointer < EchoExtensionRequestHandler > m_pechoextensionrequesthandler;

//      RfbInitializer rfbInitializer(&sockStream, m_pclientauthlistener, this,
  //                                  !m_isOutgoing);

      try {
         // First initialization phase
         try {
            m_plogwriter->information("Entering RFB initialization phase 1");
            m_prun->m_prfbinitializer->authPhase();
            setClientState(IN_AUTH);
            m_plogwriter->debug("RFB initialization phase 1 completed");

            m_shared = m_prun->m_prfbinitializer->getSharedFlag();
            m_plogwriter->debug("Shared flag = {}", (int)m_shared);
            m_viewOnlyAuth = m_prun->m_prfbinitializer->getViewOnlyAuth();
            m_plogwriter->debug("Initial view-only state = {}", (int)m_viewOnly);
            m_plogwriter->debug("Authenticated with view-only password = {}", (int)m_viewOnlyAuth);
            m_viewOnly = m_viewOnly || m_viewOnlyAuth;

            // Let RfbClientManager handle new authenticated connection.
            m_pdesktop = m_pclientauthlistener->onClientAuth(this);

            m_plogwriter->information("View only = {}", (int)m_viewOnly);
         } catch (::exception &e) {
            m_plogwriter->error("Error during RFB initialization: {}", e.get_message());
            throw;
         }
         ASSERT(m_pdesktop != 0);

         m_pviewportConst->initDesktopInterface(m_pdesktop);
         m_pviewportDynamic->initDesktopInterface(m_pdesktop);

         raw_construct_newø(m_prun->m_prfbdispatcher, m_prun->m_prfbinputgate, &m_connClosingEvent);
         m_plogwriter->debug("Dispatcher has been created");
         raw_construct_newø(m_prun->m_prfbcoderegistrator,m_prun->m_prfbdispatcher,
            &m_prun->srvToClCaps, &m_prun->clToSrvCaps,
                                       &m_prun->encCaps);
         // Init modules
         // UpdateSender initialization
         m_pupdatesender = allocateø UpdateSender();
         m_pupdatesender->initialize_update_sender(m_prun->m_prfbcoderegistrator, m_pdesktop, this,
                                           m_prun->m_prfboutputgate, m_id, m_pdesktop, m_plogwriter);
         m_plogwriter->debug("UpdateSender has been created for client #{}", m_id);
         //::innate_subsystem::PixelFormat pixelformat;
         //::i32_size sizeFramebuffer;
         m_pdesktop->getFramebufferProperties(m_prun->m_sizeFramebuffer, m_prun->m_pixelformat);
         ::i32_rectangle rectangleViewport = getViewport(m_prun->m_sizeFramebuffer);
         m_pupdatesender->init(::i32_size(rectangleViewport.size()), m_prun->m_pixelformat);
         m_plogwriter->debug("UpdateSender has been initialized");
         // ClientInputHandler initialization
         m_pclientinputhandler = new ClientInputHandler(m_prun->m_prfbcoderegistrator, this,
                                                       m_viewOnly);
         m_plogwriter->debug("ClientInputHandler has been created");
         // ClipboardExchange initialization
         m_pclipboardexchange = new ClipboardExchange(m_prun->m_prfbcoderegistrator, m_pdesktop, m_prun->m_prfboutputgate,
                                                     m_viewOnly, m_plogwriter);
         m_plogwriter->debug("ClipboardExchange has been created");

         // FileTransfers initialization
         if (m_pconfigurator->getServerConfig()->isFileTransfersEnabled() &&
             m_prun->m_prfbinitializer->getTightEnabledFlag())
         {
            m_prun->m_pfiletransferrequesthandler = allocateø file_transfer::FileTransferRequestHandler(m_pconfigurator, m_prun->m_prfbcoderegistrator, m_prun->m_prfboutputgate, m_pdesktop, m_plogwriter, !m_viewOnly);
            m_plogwriter->debug("::file::item transfer has been created");
         }
         else
         {
            m_plogwriter->information("::file::item transfer is not allowed");
         }
         // echo extension initialization
         m_prun->m_pechoextensionrequesthandler = allocateø file_transfer::EchoExtensionRequestHandler(m_prun->m_prfbcoderegistrator, m_prun->m_prfboutputgate, m_plogwriter);
         m_plogwriter->debug("Echo extension handler has been created");

         // Second initialization phase
         // Send and receive initialization information between server and viewer
         m_plogwriter->debug("View port: ({},{}) (%dx{})", rectangleViewport.left,
                                                      rectangleViewport.top,
                                                      rectangleViewport.width(),
                                                      rectangleViewport.height());
         m_plogwriter->information("Entering RFB initialization phase 2");
         m_prun->m_prfbinitializer->afterAuthPhase(&m_prun->srvToClCaps, &m_prun->clToSrvCaps,
                                       &m_prun->encCaps, rectangleViewport.size(), m_prun->m_pixelformat);
         m_plogwriter->debug("RFB initialization phase 2 completed");

         // Start normal phase
         setClientState(IN_NORMAL_PHASE);

         m_plogwriter->information("Entering normal phase of the RFB protocol");
         m_prun->m_prfbdispatcher->resumeThread();

         m_connClosingEvent.wait();
      } catch (::exception &e) {
         m_plogwriter->error("Connection will be closed: {}", e.get_message());
         sysLogMessage.format("The client {} #{} has been"
                              " disconnected for the reason: {}",
                              peerStr, m_id, e.get_message());
      }

      disconnect();
      m_pnewconnectionevents->onDisconnect(sysLogMessage);

      // After this call, we are guaranteed not to be used by other threads.
      notifyAbStateChanging(IN_PENDING_TO_REMOVE);

      if (m_prun->m_pfiletransferrequesthandler) m_prun->m_pfiletransferrequesthandler.defer_destroy_and_release();
      if (m_prun->m_pechoextensionrequesthandler) m_prun->m_pechoextensionrequesthandler.defer_destroy_and_release();
      if (m_pclipboardexchange)  delete m_pclipboardexchange;
      if (m_pclientinputhandler) delete m_pclientinputhandler;
      if (m_pupdatesender)       delete m_pupdatesender;

      // Let the client manager remove us from the client lists.
      notifyAbStateChanging(IN_READY_TO_REMOVE);
      m_plogwriter->debug("End of RfbClient, process memory usage: {} ", MainSubsystem().getCurrentMemoryUsage());
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

   void RfbClient::onKeyboardEvent(::u32 keySym, bool down)
   {
      // FIXME: How to deal with the situations when we inject a "key down" event, then foreground
      //        window changes and is no longer owned by the shared app so we cannot pass "key up"
      //        to the desktop?

      bool mayPass = true;
      bool shareApp = m_pviewportDynamic->getOnlyApplication();
      if (shareApp) {
         ::u32 pid = m_pviewportDynamic->getApplicationId();
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
      ::i32_size sizeFramebuffer;
      m_pdesktop->getFramebufferProperties(sizeFramebuffer, pixelformatStub);

      ::i32_rectangle rectangleViewport;
      bool shareApp;
      Region sharedRegion;
      getViewPortInfo(sizeFramebuffer, rectangleViewport, &shareApp, sharedRegion);

      if (!shareApp) {
         sharedRegion.clear();
         sharedRegion.addRect(rectangleViewport);
      }
      bool pointInside = sharedRegion.isPointInside(x + rectangleViewport.left, y + rectangleViewport.top);

      if (pointInside) {
         m_pupdatesender->blockCursorPosSending();
         m_pdesktop->setMouseEvent(x + rectangleViewport.left, y + rectangleViewport.top, buttonMask);
         m_demandtimerIdle.reset();
      }
   }

   ::i32_rectangle RfbClient::getViewport(const ::i32_size & sizeFramebuffer)
   {
      critical_section_lock al(&m_criticalsectionViewport);
      m_pviewportConst->update(sizeFramebuffer);
      m_pviewportDynamic->update(sizeFramebuffer);

      return m_pviewportConst->getViewport().intersection(
        m_pviewportDynamic->getViewport());
   }

   void RfbClient::getViewPortInfo(const ::i32_size & sizeFramebuffer, ::i32_rectangle &rectangleResult,
                                   bool *shareApp, Region & regionShareApp)
   {
      critical_section_lock al(&m_criticalsectionViewport);

      rectangleResult = getViewport(sizeFramebuffer);
      *shareApp = m_pviewportDynamic->getOnlyApplication();
      if (*shareApp) {
         m_pviewportDynamic->getApplicationRegion(regionShareApp);
      }
   }

   void RfbClient::onGetViewPort(::i32_rectangle &viewRect, bool *shareApp, Region & regionShareApp)
   {
      ::innate_subsystem::PixelFormat pixelformatStub;
      ::i32_size sizeFramebuffer;
      m_pdesktop->getFramebufferProperties(sizeFramebuffer, pixelformatStub);
      getViewPortInfo(sizeFramebuffer, viewRect, shareApp, regionShareApp);
   }
} // namespace remoting


