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
#include "acme/_operating_system.h"
#include "RfbClient.h"
#include "remoting/remoting_common/thread/AutoLock.h"
#include "RfbCodeRegistrator.h"
#include "ft_server_lib/FileTransferRequestHandler.h"
#include "EchoExtensionRequestHandler.h"
#include "remoting/remoting_common/network/socket/SocketStream.h"
#include "RfbInitializer.h"
#include "ClientAuthListener.h"
#include "remoting/remoting_common/server_config/Configurator.h"
#include "remoting/remoting_common/io/BufferedInputStream.h"
#include "remoting/remoting_common/util/MemUsage.h"

RfbClient::RfbClient(NewConnectionEvents *newConnectionEvents,
                     SocketIPv4 *socket,
                     ClientTerminationListener *extTermListener,
                     ClientAuthListener *extAuthListener, bool viewOnly,
                     bool isOutgoing, unsigned int id,
                     const ViewPortState *constViewPort,
                     const ViewPortState *dynViewPort,
                     int idleTimeout,
                     LogWriter *log)
: m_socket(socket), // now we own the socket
  m_newConnectionEvents(newConnectionEvents),
  m_viewOnly(viewOnly),
  m_isOutgoing(isOutgoing),
  m_shared(false),
  m_viewOnlyAuth(true),
  m_clientState(IN_NONAUTH),
  m_isMarkedOk(false),
  m_extTermListener(extTermListener),
  m_extAuthListener(extAuthListener),
  m_updateSender(0),
  m_clipboardExchange(0),
  m_clientInputHandler(0),
  m_id(id),
  m_desktop(0),
  m_constViewPort(constViewPort, log),
  m_dynamicViewPort(dynViewPort, log),
  m_idleTimer(idleTimeout), m_idleTimeout(idleTimeout),
  m_log(log)
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
  m_log->debug("Connection from {} has been closed for client #{}", peerStr, m_id);
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
    _ASSERT(FALSE);

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
    _ASSERT(FALSE);

    address-= "unknown";
  }
}

void RfbClient::getSocketAddr(SocketAddressIPv4 *addr) const
{
  m_socket->getPeerAddr(addr);
}

void RfbClient::setClientState(ClientState newState)
{
  AutoLock al(&m_clientStateMut);
  if (newState > m_clientState) {
    m_clientState = newState;
  }
}

ClientState RfbClient::getClientState()
{
  AutoLock al(&m_clientStateMut);
  return m_clientState;
}

void RfbClient::setViewOnlyFlag(bool value)
{
  if (getClientState() < IN_NORMAL_PHASE) {
    throw ::remoting::Exception("Irrelevant call to RfbClient::setViewOnlyFlag()");
  }
  m_viewOnly = value || m_viewOnlyAuth;
  m_clientInputHandler->setViewOnlyFlag(m_viewOnly);
}

void RfbClient::changeDynViewPort(const ViewPortState *dynViewPort)
{
  AutoLock al(&m_viewPortMutex);
  m_dynamicViewPort.changeState(dynViewPort);
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

  ServerConfig *config = Configurator::getInstance()->getServerConfig();

  SocketStream sockStream(m_socket);

  RfbOutputGate output(&sockStream);
  BufferedInputStream bufInput(&sockStream);
  RfbInputGate input(&bufInput);

  FileTransferRequestHandler *fileTransfer = 0;
  EchoExtensionRequestHandler *echoExtension = 0;

  RfbInitializer rfbInitializer(&sockStream, m_extAuthListener, this,
                                !m_isOutgoing);

  try {
    // First initialization phase
    try {
      m_log->information("Entering RFB initialization phase 1");
      rfbInitializer.authPhase();
      setClientState(IN_AUTH);
      m_log->debug("RFB initialization phase 1 completed");

      m_shared = rfbInitializer.getSharedFlag();
      m_log->debug("Shared flag = {}", (int)m_shared);
      m_viewOnlyAuth = rfbInitializer.getViewOnlyAuth();
      m_log->debug("Initial view-only state = {}", (int)m_viewOnly);
      m_log->debug("Authenticated with view-only password = {}", (int)m_viewOnlyAuth);
      m_viewOnly = m_viewOnly || m_viewOnlyAuth;

      // Let RfbClientManager handle new authenticated connection.
      m_desktop = m_extAuthListener->onClientAuth(this);

      m_log->information("View only = {}", (int)m_viewOnly);
    } catch (::exception &e) {
      m_log->error("Error during RFB initialization: {}", e.get_message());
      throw;
    }
    _ASSERT(m_desktop != 0);

    m_constViewPort.initDesktopInterface(m_desktop);
    m_dynamicViewPort.initDesktopInterface(m_desktop);

    RfbDispatcher dispatcher(&input, &m_connClosingEvent);
    m_log->debug("Dispatcher has been created");
    CapContainer srvToClCaps, clToSrvCaps, encCaps;
    RfbCodeRegistrator codeRegtor(&dispatcher, &srvToClCaps, &clToSrvCaps,
                                  &encCaps);
    // Init modules
    // UpdateSender initialization
    m_updateSender = new UpdateSender(&codeRegtor, m_desktop, this,
                                      &output, m_id, m_desktop, m_log);
    m_log->debug("UpdateSender has been created for client #{}", m_id);
    PixelFormat pf;
    ::int_size fbDim;
    m_desktop->getFrameBufferProperties(&fbDim, &pf);
    ::int_rectangle viewPort = getViewPortRect(fbDim);
    m_updateSender->init(::int_size(viewPort.size()), pf);
    m_log->debug("UpdateSender has been initialized");
    // ClientInputHandler initialization
    m_clientInputHandler = new ClientInputHandler(&codeRegtor, this,
                                                  m_viewOnly);
    m_log->debug("ClientInputHandler has been created");
    // ClipboardExchange initialization
    m_clipboardExchange = new ClipboardExchange(&codeRegtor, m_desktop, &output,
                                                m_viewOnly, m_log);
    m_log->debug("ClipboardExchange has been created");

    // FileTransfers initialization
    if (config->isFileTransfersEnabled() &&
        rfbInitializer.getTightEnabledFlag()) {
      fileTransfer = new FileTransferRequestHandler(&codeRegtor, &output, m_desktop, m_log, !m_viewOnly);
      m_log->debug("::file::item transfer has been created");
    } else {
      m_log->information("::file::item transfer is not allowed");
    }
    // echo extension initialization
    echoExtension = new EchoExtensionRequestHandler(&codeRegtor, &output, m_log);
    m_log->debug("Echo extension handler has been created");

    // Second initialization phase
    // Send and receive initialization information between server and viewer
    m_log->debug("View port: ({},{}) (%dx{})", viewPort.left,
                                                 viewPort.top,
                                                 viewPort.width(),
                                                 viewPort.height());
    m_log->information("Entering RFB initialization phase 2");
    rfbInitializer.afterAuthPhase(&srvToClCaps, &clToSrvCaps,
                                  &encCaps, &::int_size(&viewPort), &pf);
    m_log->debug("RFB initialization phase 2 completed");

    // Start normal phase
    setClientState(IN_NORMAL_PHASE);

    m_log->information("Entering normal phase of the RFB protocol");
    dispatcher.resume();

    m_connClosingEvent.waitForEvent();
  } catch (::exception &e) {
    m_log->error("Connection will be closed: {}", e.get_message());
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
  if (m_clipboardExchange)  delete m_clipboardExchange;
  if (m_clientInputHandler) delete m_clientInputHandler;
  if (m_updateSender)       delete m_updateSender;

  // Let the client manager remove us from the client lists.
  notifyAbStateChanging(IN_READY_TO_REMOVE);
  m_log->debug("End of RfbClient, process memory usage: {} ", MemUsage::getCurrentMemUsage());
}

void RfbClient::sendUpdate(const UpdateContainer *updateContainer,
                           const CursorShape *cursorShape)
{
  m_updateSender->newUpdates(updateContainer, cursorShape);

  if (m_idleTimeout != 0  && m_idleTimer.isElapsed()) {
    m_log->error("Connection will be closed due to client inactivity. IdleTimeout = {} ms", m_idleTimeout);
    m_connClosingEvent.notify();
  }
}

void RfbClient::sendClipboard(const ::scoped_string & newClipboard)
{
  m_clipboardExchange->sendClipboard(newClipboard);
}

void RfbClient::onKeyboardEvent(unsigned int keySym, bool down)
{
  // FIXME: How to deal with the situations when we inject a "key down" event, then foreground
  //        window changes and is no longer owned by the shared app so we cannot pass "key up"
  //        to the desktop?

  bool mayPass = true;
  bool shareApp = m_dynamicViewPort.getOnlyApplication();
  if (shareApp) {
    unsigned int pid = m_dynamicViewPort.getApplicationId();
    mayPass = m_desktop->isApplicationInFocus(pid);
  }

  if (mayPass) {
    m_desktop->setKeyboardEvent(keySym, down);
    m_idleTimer.reset();
  }
}

void RfbClient::onMouseEvent(unsigned short x, unsigned short y, unsigned char buttonMask)
{
  // FIXME: Too much extra work. Typically we would share the whole desktop and would not need
  //        to compute regions on each mouse move.

  PixelFormat pfStub;
  ::int_size fbDim;
  m_desktop->getFrameBufferProperties(&fbDim, &pfStub);

  ::int_rectangle vp;
  bool shareApp;
  Region sharedRegion;
  getViewPortInfo(&fbDim, &vp, &shareApp, &sharedRegion);

  if (!shareApp) {
    sharedRegion.clear();
    sharedRegion.addRect(&vp);
  }
  bool pointInside = sharedRegion.isPointInside(x + vp.left, y + vp.top);

  if (pointInside) {
    m_updateSender->blockCursorPosSending();
    m_desktop->setMouseEvent(x + vp.left, y + vp.top, buttonMask);
    m_idleTimer.reset();
  }
}

::int_rectangle RfbClient::getViewPortRect(const ::int_size & fbDimension)
{
  AutoLock al(&m_viewPortMutex);
  m_constViewPort.update(fbDimension);
  m_dynamicViewPort.update(fbDimension);

  return m_constViewPort.getViewPortRect().intersection(
    &m_dynamicViewPort.getViewPortRect());
}

void RfbClient::getViewPortInfo(const ::int_size & fbDimension, ::int_rectangle *resultRect,
                                bool *shareApp, Region *shareAppRegion)
{
  AutoLock al(&m_viewPortMutex);

  *resultRect = getViewPortRect(fbDimension);
  *shareApp = m_dynamicViewPort.getOnlyApplication();
  if (*shareApp) {
    m_dynamicViewPort.getApplicationRegion(shareAppRegion);
  }
}

void RfbClient::onGetViewPort(::int_rectangle *viewRect, bool *shareApp, Region *shareAppRegion)
{
  PixelFormat pfStub;
  ::int_size fbDim;
  m_desktop->getFrameBufferProperties(&fbDim, &pfStub);
  getViewPortInfo(&fbDim, viewRect, shareApp, shareAppRegion);
}
