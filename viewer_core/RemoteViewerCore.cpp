// Copyright (C) 2011,2012 GlavSoft LLC.
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
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//
#include "framework.h"
#include "RemoteViewerCore.h"

#include "remoting/ftp_common/FTMessage.h"
#include "rfb/AuthDefs.h"
#include "rfb/TunnelDefs.h"
#include "rfb/MsgDefs.h"
#include "rfb/EncodingDefs.h"
#include "rfb/VendorDefs.h"
//#include "util/::string.h"
////#include "util/::string.h"

#include "AuthHandler.h"
#include "RichCursorDecoder.h"
#include "RfbFramebufferUpdateRequestClientMessage.h"
#include "RfbCutTextEventClientMessage.h"
#include "RfbKeyEventClientMessage.h"
#include "RfbPointerEventClientMessage.h"
#include "RfbSetEncodingsClientMessage.h"
#include "RfbSetPixelFormatClientMessage.h"
#include "WatermarksController.h"

#include "RawDecoder.h"
#include "CopyRectDecoder.h"
#include "RreDecoder.h"
#include "HexTileDecoder.h"
#include "TightDecoder.h"
#include "ZrleDecoder.h"

#include "JpegQualityLevel.h"
#include "CompressionLevel.h"

#include "DesktopSizeDecoder.h"
#include "LastRectDecoder.h"
#include "PointerPosDecoder.h"
#include "RichCursorDecoder.h"

#include <algorithm>

RemoteViewerCore::RemoteViewerCore(LogWriter *LogWriter)
: m_logWriter(LogWriter),
  m_tcpConnection(m_logWriter),
  m_fbUpdateNotifier(&m_frameBuffer, &m_fbLock, m_logWriter, &m_watermarksController),
  m_decoderStore(m_logWriter),
  m_updateRequestSender(&m_fbLock, &m_frameBuffer, m_logWriter),
  m_dispatchDataProvider(0),
  m_isTightEnabled(true),
  m_isUtf8ClipboardEnabled(false)
{
  init();
}

RemoteViewerCore::RemoteViewerCore(const ::scoped_string & scopedstrHost, unsigned short port,
                                   CoreEventsAdapter *adapter,
                                   LogWriter *LogWriter,
                                   bool sharedFlag)
: m_logWriter(LogWriter),
  m_tcpConnection(m_logWriter),
  m_fbUpdateNotifier(&m_frameBuffer, &m_fbLock, m_logWriter, &m_watermarksController),
  m_decoderStore(m_logWriter),
  m_updateRequestSender(&m_fbLock, &m_frameBuffer, m_logWriter),
  m_dispatchDataProvider(0),
  m_isTightEnabled(true),
  m_isUtf8ClipboardEnabled(false)
{
  init();

  start(scopedstrHost, port, adapter, sharedFlag);
}

RemoteViewerCore::RemoteViewerCore(SocketIPv4 *socket,
                                   CoreEventsAdapter *adapter,
                                   LogWriter *LogWriter,
                                   bool sharedFlag)
: m_logWriter(LogWriter),
  m_tcpConnection(m_logWriter),
  m_fbUpdateNotifier(&m_frameBuffer, &m_fbLock, m_logWriter, &m_watermarksController),
  m_decoderStore(m_logWriter),
  m_updateRequestSender(&m_fbLock, &m_frameBuffer, m_logWriter),
  m_dispatchDataProvider(0),
  m_isTightEnabled(true),
  m_isUtf8ClipboardEnabled(false)
{
  init();

  start(socket, adapter, sharedFlag);
}

RemoteViewerCore::RemoteViewerCore(RfbInputGate *input, RfbOutputGate *output,
                                   CoreEventsAdapter *adapter,
                                   LogWriter *LogWriter,
                                   bool sharedFlag)
: m_logWriter(LogWriter),
  m_tcpConnection(m_logWriter),
  m_fbUpdateNotifier(&m_frameBuffer, &m_fbLock, m_logWriter, &m_watermarksController),
  m_decoderStore(m_logWriter),
  m_updateRequestSender(&m_fbLock, &m_frameBuffer, m_logWriter),
  m_dispatchDataProvider(0),
  m_isTightEnabled(true),
  m_isUtf8ClipboardEnabled(false)
{
  init();

  start(input, output, adapter, sharedFlag);
}

void RemoteViewerCore::init()
{
  m_decoderStore.addDecoder(new RawDecoder(m_logWriter), 0);
  m_decoderStore.addDecoder(new CopyRectDecoder(m_logWriter), 10);
  m_decoderStore.addDecoder(new RreDecoder(m_logWriter), 1);
  m_decoderStore.addDecoder(new HexTileDecoder(m_logWriter), 4);
  m_decoderStore.addDecoder(new TightDecoder(m_logWriter), 9);
  m_decoderStore.addDecoder(new ZrleDecoder(m_logWriter), 9);

  m_decoderStore.addDecoder(new DesktopSizeDecoder(m_logWriter), -1);
  m_decoderStore.addDecoder(new LastRectDecoder(m_logWriter), -1);
  m_decoderStore.addDecoder(new PointerPosDecoder(m_logWriter), -1);
  m_decoderStore.addDecoder(new RichCursorDecoder(m_logWriter), -1);
  m_input = 0;
  m_output = 0;

  m_wasStarted = false;
  m_wasConnected = false;
  m_isNewPixelFormat = false;
  m_isFreeze = false;
  m_isNeedRequestUpdate = true;
  m_forceFullUpdate = false;

  m_updateTimeout = 0;

  addClientMsgCapability(ClientMsgDefs::CLIENT_CUT_TEXT_UTF8,
    VendorDefs::TIGHTVNC,
    Utf8CutTextDefs::CLIENT_CUT_TEXT_UTF8_SIG, 
    "UTF-8 clipboard");

  addClientMsgCapability(ClientMsgDefs::ENABLE_CUT_TEXT_UTF8,
    VendorDefs::TIGHTVNC,
    Utf8CutTextDefs::ENABLE_CUT_TEXT_UTF8_SIG,
    "enable UTF-8 clipboard");
}

RemoteViewerCore::~RemoteViewerCore()
{
  try {
    // Stop all threads.
    stop();

    // If core isn't started, then this thread isn't execute,
    // wait only thead of FbUpdateNotifier.
    if (wasStarted()) {
      waitTermination();
    } else {
      m_fbUpdateNotifier.wait();
	  m_updateRequestSender.wait();
    }
  } catch (...) {
  }
}

void RemoteViewerCore::start(CoreEventsAdapter *adapter,
                             bool sharedFlag)
{
  m_logWriter->debug("Starting remote viewer core...");

  if (adapter == 0) {
    throw ::remoting::Exception("Remote viewer core is not started: adapter is 0");
  }

  // Set flag "wasStarted".
  // This flag protects of second call start().
  {
    AutoLock al(&m_startLock);
    if (m_wasStarted) {
      throw ::remoting::Exception("Remote viewer core is already started");
    }
    m_wasStarted = true;
  }

  m_sharedFlag = sharedFlag;
  m_adapter = adapter;

  m_fbUpdateNotifier.setAdapter(adapter);

  // Start thread.
  resume();
  m_logWriter->debug("Remote viewer core is started");
}

void RemoteViewerCore::start(const ::scoped_string & scopedstrHost,
                             unsigned short port,
                             CoreEventsAdapter *adapter,
                             bool sharedFlag)
{
  m_tcpConnection.bind(scopedstrHost, port);
  start(adapter, sharedFlag);
}
void RemoteViewerCore::start(SocketIPv4 *socket,
                             CoreEventsAdapter *adapter,
                             bool sharedFlag)
{
  m_tcpConnection.bind(socket);
  start(adapter, sharedFlag);
}

void RemoteViewerCore::start(RfbInputGate *input, RfbOutputGate *output,
                             CoreEventsAdapter *adapter,
                             bool sharedFlag)
{
  m_tcpConnection.bind(input, output);
  start(adapter, sharedFlag);
}

bool RemoteViewerCore::wasStarted() const
{
  AutoLock al(&m_startLock);
  return m_wasStarted;
}

bool RemoteViewerCore::wasConnected() const
{
  AutoLock al(&m_connectLock);
  return m_wasConnected;
}

void RemoteViewerCore::stop()
{
  {
    // We should use locking to prevent simultaneous reading and writing of
    // the m_dispatchDataProvider pointer.
    AutoLock al(&m_dispatchDataProviderLock);
    m_dispatchDataProvider = 0;
  }

  m_updateRequestSender.terminate();

  m_tcpConnection.close();
  m_fbUpdateNotifier.terminate();
  terminate();
}

void RemoteViewerCore::waitTermination()
{
  m_fbUpdateNotifier.wait();
  m_updateRequestSender.wait();
  wait();
}

void RemoteViewerCore::setPixelFormat(const PixelFormat & pixelFormat)
{
  m_logWriter->debug("Pixel format will changed");
  AutoLock al(&m_pixelFormatLock);
  m_isNewPixelFormat = true;
  m_viewerPixelFormat = pixelFormat;
}

void RemoteViewerCore::enableDispatching(DispatchDataProvider *src)
{
  AutoLock al(&m_startLock);
  if (!m_wasStarted) {
    // In other places, we use locking to access this pointer
    // (see m_dispatchDataProviderLock), but not here, as we assume there is
    // no concurrent access to this variable prior to calling start().
    m_dispatchDataProvider = src;
  }
}

bool RemoteViewerCore::updatePixelFormat()
{
  PixelFormat pxFormat;
  m_logWriter->debug("Check pixel format change...");
  {
    AutoLock al(&m_pixelFormatLock);
    if (!m_isNewPixelFormat)
      return false;
    m_isNewPixelFormat = false;
    pxFormat = m_viewerPixelFormat;
  }

  int bitsPerPixel = m_viewerPixelFormat.bitsPerPixel;
  if (bitsPerPixel != 8 && bitsPerPixel != 16 && bitsPerPixel != 32) {
    throw ::remoting::Exception("Only 8, 16 or 32 bits per pixel supported!");
  }

  {
    AutoLock al(&m_fbLock);
    // FIXME: here isn't accept true-colour flag.
    // PixelFormats may be equal, if isn't.
    if (pxFormat == m_frameBuffer.getPixelFormat() ){
      return false;
    }
    if (m_frameBuffer.getBuffer() != 0)
      setFbProperties(m_frameBuffer.getDimension(), pxFormat);
  }

  RfbSetPixelFormatClientMessage pixelFormatMessage(pxFormat);
  pixelFormatMessage.send(m_output);

  return true;
}

void RemoteViewerCore::refreshFrameBuffer()
{
  m_logWriter->debug("Frame buffer will refreshed");
  AutoLock al(&m_refreshingLock);
  m_isRefreshing = true;
}

void RemoteViewerCore::forceFullUpdateRequests(const bool& forceUpdate)
{
	m_forceFullUpdate = forceUpdate;
	m_updateRequestSender.setIsIncremental(!forceUpdate);
}

void RemoteViewerCore::deferUpdateRequests(const int& milliseconds)
{
	m_updateTimeout = milliseconds;
	m_updateRequestSender.setTimeout(milliseconds);
}

void RemoteViewerCore::sendFbUpdateRequest(bool incremental)
{
  {
    AutoLock al(&m_requestUpdateLock);
    bool requestUpdate = m_isNeedRequestUpdate;
    m_isNeedRequestUpdate = false;
    if (!requestUpdate)
      return;
  }

  bool isRefresh = false;
  bool isUpdateFbProperties = false;
  if (updatePixelFormat()) {
    isUpdateFbProperties = true;
  }

  {
    AutoLock al(&m_refreshingLock);
    if (m_isRefreshing) {
      m_isRefreshing= false;
      isRefresh = true;
    }
  }

  if (isRefresh || isUpdateFbProperties || m_updateRequestSender.getTimeout() <= 0)
  {
	bool isIncremental = incremental && !isRefresh && !isUpdateFbProperties;
	::int_rectangle updateRect;
	{
	  AutoLock al(&m_fbLock);
	  updateRect = m_frameBuffer.getDimension();
	}

	if (isIncremental) {
	  m_logWriter->debug("Sending frame buffer incremental update request [%dx{}]...",
	                    updateRect.width(), updateRect.height());
	} else {
	  m_logWriter->debug("Sending frame buffer full update request [%dx{}]...",
	                    updateRect.width(), updateRect.height());
  }

	RfbFramebufferUpdateRequestClientMessage fbUpdReq(isIncremental, updateRect);
	fbUpdReq.send(m_output);
	m_logWriter->debug("Frame buffer update request is sent");
  }
  else
  {
	  m_updateRequestSender.setWasUpdated();
  }
}

void RemoteViewerCore::sendKeyboardEvent(bool downFlag, unsigned int key)
{
  // If core isn't connected, then m_output may be isn't initialized.
  // Exit from function, if it is.
  if (!wasConnected()) {
    return;
  }

  m_logWriter->debug("Sending key event: {}, {}...", downFlag, key);
  RfbKeyEventClientMessage keyMessage(downFlag, key);
  keyMessage.send(m_output);
  m_logWriter->debug("Key event: {}, {} is sent", downFlag, key);
}

void RemoteViewerCore::sendPointerEvent(unsigned char buttonMask,
                                        const Point *position)
{
  // If core isn't connected, then m_output may be isn't initialized.
  // Exit from function, if it is.
  if (!wasConnected()) {
    return;
  }

  m_logWriter->debug("Sending pointer event 0x%X, ({}, {})...",
                     static_cast<int>(buttonMask), position->x, position->y);
  // send position to server
  RfbPointerEventClientMessage pointerMessage(buttonMask, position);
  pointerMessage.send(m_output);
  // update position
  m_fbUpdateNotifier.updatePointerPos(position);

  m_logWriter->debug("Pointer event: 0x%X, ({}, {}) is sent",
                    static_cast<int>(buttonMask), position->x, position->y);
}

void RemoteViewerCore::sendCutTextEvent(const ::scoped_string & cutText)
{
  // If core isn't connected, then m_output may be isn't initialized.
  // Exit from function, if it is.

  if (!wasConnected()) {
    return;
  }

  RfbCutTextEventClientMessage cutTextMessage(cutText);
  if (m_isUtf8ClipboardEnabled) {
    m_logWriter->debug("Sending UTF-8 clipboard cut text: \"{}\"...", cutText);
    cutTextMessage.sendUtf8(m_output);
  }
  else {
    m_logWriter->debug("Sending clipboard cut text: \"{}\"...", cutText);
    cutTextMessage.send(m_output);
  }
  m_logWriter->debug("Clipboard cut text: \"{}\" is sent", cutText);
}

void RemoteViewerCore::setPreferredEncoding(int encodingType)
{
  m_decoderStore.setPreferredEncoding(encodingType);
  sendEncodings();
}

void RemoteViewerCore::allowCopyRect(bool allow)
{
  m_decoderStore.allowCopyRect(allow);
  sendEncodings();
}

void RemoteViewerCore::allowUtf8Clipboard()
{
  m_isUtf8ClipboardEnabled = m_clientMsgCaps.isEnabled(ClientMsgDefs::CLIENT_CUT_TEXT_UTF8);
  if (m_isUtf8ClipboardEnabled) {
    m_logWriter->debug("Server supports Utf8 Clipboard recieving.");
  }
  else {
    m_logWriter->debug("Server does not support Utf8 Clipboard recieving, ClientCutTextUtf8 messages disabled.");
  }
  if (m_clientMsgCaps.isEnabled(ClientMsgDefs::ENABLE_CUT_TEXT_UTF8)) {
    m_logWriter->debug("Sending EnableCutTextUtf8 scopedstrMessage.");
    AutoLock al(m_output);
    m_output->writeUInt32(ClientMsgDefs::ENABLE_CUT_TEXT_UTF8);
    m_output->flush();
  }
}

void RemoteViewerCore::setCompressionLevel(int newLevel)
{
  bool needUpdate = false;
  for (int level = CompressionLevel::COMPRESSION_LEVEL_MIN;
       level <= CompressionLevel::COMPRESSION_LEVEL_MAX;
       level++)
    if (level != newLevel)
      needUpdate |= m_decoderStore.removeDecoder(CompressionLevel(m_logWriter, level).getCode());

  // new compression level is valid?
  if (newLevel < CompressionLevel::COMPRESSION_LEVEL_MIN ||
      newLevel > CompressionLevel::COMPRESSION_LEVEL_MAX)
    return;

  needUpdate |= m_decoderStore.addDecoder(new CompressionLevel(m_logWriter, newLevel), -1);
  if (needUpdate) {
    sendEncodings();
  }
}

void RemoteViewerCore::setJpegQualityLevel(int newLevel)
{
  bool needUpdate = false;
  for (int level = JpegQualityLevel::JPEG_QUALITY_LEVEL_MIN;
       level <= JpegQualityLevel::JPEG_QUALITY_LEVEL_MAX;
       level++)
    if (level != newLevel)
      needUpdate |= m_decoderStore.removeDecoder(JpegQualityLevel(m_logWriter, level).getCode());

  // new jpeg quality level is valid?
  if (newLevel < JpegQualityLevel::JPEG_QUALITY_LEVEL_MIN ||
      newLevel > JpegQualityLevel::JPEG_QUALITY_LEVEL_MAX)
    return;

  needUpdate |= m_decoderStore.addDecoder(new JpegQualityLevel(m_logWriter, newLevel), -1);
  if (needUpdate) {
    sendEncodings();
  }
}

void RemoteViewerCore::enableCursorShapeUpdates(bool enabled)
{
  bool needUpdate = false;
  if (enabled) {
    needUpdate |= m_decoderStore.addDecoder(new RichCursorDecoder(m_logWriter), -1);
    needUpdate |= m_decoderStore.addDecoder(new PointerPosDecoder(m_logWriter), -1);
  } else {
    needUpdate |= m_decoderStore.removeDecoder(PseudoEncDefs::RICH_CURSOR);
    needUpdate |= m_decoderStore.removeDecoder(PseudoEncDefs::POINTER_POS);
  }
  
  if (needUpdate) {
    sendEncodings();
  }
}

void RemoteViewerCore::ignoreCursorShapeUpdates(bool ignored)
{
  m_fbUpdateNotifier.setIgnoreShapeUpdates(ignored);
}

void RemoteViewerCore::stopUpdating(bool isStopped)
{
  {
    AutoLock al(&m_freezeLock);
    if (isStopped == m_isFreeze)
      return;
    m_isFreeze = isStopped;
  }
  if (!isStopped) {
    m_logWriter->debug("Sending of frame buffer update request...");
    sendFbUpdateRequest(!m_forceFullUpdate);
  }
}

PixelFormat RemoteViewerCore::readPixelFormat()
{
  PixelFormat pixelFormat;
  pixelFormat.bitsPerPixel = m_input->readUInt8();
  pixelFormat.colorDepth = m_input->readUInt8();
  pixelFormat.bigEndian = !!m_input->readUInt8();
  // now, supported only true color
  int trueColour = m_input->readUInt8();
  if (trueColour == false) {
    m_logWriter->error("Not supported palette. Flag \"True colour\" is not set.");
  }
  pixelFormat.redMax = m_input->readUInt16();
  pixelFormat.greenMax = m_input->readUInt16();
  pixelFormat.blueMax = m_input->readUInt16();
  pixelFormat.redShift = m_input->readUInt8();
  pixelFormat.greenShift = m_input->readUInt8();
  pixelFormat.blueShift = m_input->readUInt8();
  m_input->readUInt8(); // padding bytes (3)
  m_input->readUInt8();
  m_input->readUInt8();
  return pixelFormat;
}

void RemoteViewerCore::connectToHost()
{
  m_tcpConnection.connect();
  m_input = m_tcpConnection.getInput();
  m_output = m_tcpConnection.getOutput();

  m_updateRequestSender.setOutput(m_output);

  m_logWriter->debug("Connection is established");
  try {
    m_adapter->onEstablished();
  } catch (const ::remoting::Exception &ex) {
    m_logWriter->error("Error in CoreEventsAdapter::onEstablished(): {}", ex.get_message());
  } catch (...) {
    m_logWriter->error("Unknown error in CoreEventsAdapter::onEstablished()");
  }
}

void RemoteViewerCore::authenticate()
{
  m_logWriter->debug("Negotiating security type...");
  int authenticationType = negotiateSecurityType();
  m_logWriter->information("Authentication type accepted: {} ({})",
                   getAuthenticationTypeName(authenticationType),
                   authenticationType);

  if (authenticationType != 0) {
    m_logWriter->debug("Authentication...");
    if (m_authHandlers.find(authenticationType) != m_authHandlers.end()) {
      m_authHandlers[authenticationType]->authenticate(m_input, m_output);
    } else {
      // Security type is added automatic, but not by user.
      if (authenticationType != SecurityDefs::NONE) {
        m_logWriter->error("Isn't exist authentication handler for selected security type {}",
                          authenticationType);
        throw AuthException("Isn't exist authentication handler "
                            "for selected type of authentication");
      }
    }
  }

  // get authentication result, if version 3.8 or authentication isn't None
  if (m_minor >= 8 || authenticationType != SecurityDefs::NONE) {
    unsigned int authResult = 0;
    if (authenticationType) {
      authResult = m_input->readUInt32();
      m_logWriter->debug("Auth result is {}", authResult);
    }
    static const unsigned int AUTH_RESULT_OK = 0;
    if (!authenticationType || authResult != AUTH_RESULT_OK) {
      // if version 3.3 or 3.7 then server connection closed
      m_logWriter->error("Authentication failure");
      if (m_minor < 8) {
        throw AuthException("Authentication failure");
      }
      // if version 3.8 then try read reasonAuth.
      ::string reasonAuth;
      reasonAuth = m_input->read_utf8_string();
      ::string errorMessage = "Authentication reason: ";
      errorMessage+=reasonAuth;
      m_logWriter->error("{}", errorMessage);
      throw AuthException(errorMessage);
    }
  }
}

void RemoteViewerCore::enableTightSecurityType(bool enabled)
{
  m_isTightEnabled = enabled;	
}

int RemoteViewerCore::negotiateSecurityType()
{
  m_logWriter->debug("Reading ::list_base of security types...");
  // read ::list_base of security types
  ::array_base<unsigned int> secTypes;
  readSecurityTypeList(&secTypes);
  m_logWriter->debug("List of security type is read");
  if (secTypes.size() == 0) {
    m_logWriter->warning("Error in negotiate about of security: only security type is 0");
    return 0;
  }

  // log information about security ::list_base
  ::string secTypeString;
  for (::array_base<unsigned int>::iterator i = secTypes.begin(); i != secTypes.end(); i++) {
    if(i != secTypes.begin())
      secTypeString += ", ";
    ::string nameType;
    nameType.format("{} ({})", getSecurityTypeName(*i), *i);
    secTypeString+=nameType;
  }
  m_logWriter->debug("Security Types received ({}): {}",
                   secTypes.size(), secTypeString); 

  // select type security
  m_logWriter->debug("Selecting auth-handler");
  int typeSelected = selectSecurityType(&secTypes, &m_authHandlers, m_isTightEnabled);
  m_logWriter->information("Security type is selected: {}", typeSelected);
  if (typeSelected == SecurityDefs::TIGHT) {
    m_logWriter->information("Tight capabilities is enable");
    m_isTight = true;

    m_output->writeUInt8(typeSelected);
    m_output->flush();

    initTunnelling();
    return initAuthentication();
  }

  if (m_minor >= 7) {
    m_output->writeUInt8(typeSelected);
    m_output->flush();
  }

  return typeSelected;
}

void RemoteViewerCore::readSecurityTypeList(::array_base<unsigned int> *secTypes)
{
  if (m_minor < 7) {
    unsigned int type = m_input->readUInt32();
    if (type != 0)
      secTypes->add(type);
  } else { // m_minor >= 7
    int secTypesNumber = m_input->readUInt8();
    if (secTypesNumber != 0) {
      secTypes->resize(secTypesNumber);
      for (::array_base<unsigned int>::iterator i = secTypes->begin(); i != secTypes->end(); i++)
        *i = m_input->readUInt8();
    }
  }
}

::string RemoteViewerCore::getSecurityTypeName(unsigned int securityType) const
{
  switch (securityType) {
  case SecurityDefs::NONE:
    return "None";
  case SecurityDefs::VNC:
    return "VNC";
  case SecurityDefs::TIGHT:
    return "Tight";
  }
  return "Unknown type";
}

::string RemoteViewerCore::getAuthenticationTypeName(unsigned int authenticationType) const
{
  switch (authenticationType) {
  case AuthDefs::NONE:
    return "None";
  case AuthDefs::VNC:
    return "VNC";
  }
  return "Unknown type";
}

int RemoteViewerCore::selectSecurityType(const ::array_base<unsigned int> *secTypes,
                                         const ::map<unsigned int, AuthHandler *> *authHandlers,
										 const bool isTightEnabled) const
{
  // Typedef const iterator.
  typedef ::array_base<unsigned int>::const_iterator SecTypesIterator;

  if (isTightEnabled) {
    // If server supports security type "Tight", select it.
    SecTypesIterator tightSecType = std::find(secTypes->begin(),
                                              secTypes->end(),
                                              SecurityDefs::TIGHT);
    if (tightSecType != secTypes->end()) {
      return *tightSecType;
    }
  }
  // If server doesn't support security type "Tight", then search first type, which is supported.
  for (SecTypesIterator i = secTypes->begin();
       i != secTypes->end();
       i++) {
    if (authHandlers->find(*i) != authHandlers->end() ||
        *i == SecurityDefs::NONE)
      return *i;
  }

  throw ::remoting::Exception("No security types supported. "
                  "Server sent security types, "
                  "but we do not support any of them.");
}

void RemoteViewerCore::initTunnelling()
{
  m_logWriter->debug("Initialization of tight-tunneling...");
  unsigned int tunnelCount = m_input->readUInt32();
  if (tunnelCount > 0) {
    bool hasNoTunnel = false;
    for (unsigned int i = 0; i < tunnelCount; i++) {
      RfbCapabilityInfo cap = readCapability();
      if (cap.code == TunnelDefs::NOTUNNEL) {
        hasNoTunnel = true;
      }
      // Special case for VNC server of Siemense PLC . It supports NOTUNNEL while there is no it in the ::list_base.
	  if (cap.isEqual("SICR", "SCHANNEL")) {
        hasNoTunnel = true;
      }
    }
    if (hasNoTunnel) {
      m_output->writeUInt32(TunnelDefs::NOTUNNEL);
      m_output->flush();
    } else {
      m_logWriter->error("Viewer support only default tunneling tight-authentication");
      throw ::remoting::Exception("Viewer support only default tunneling tight-authentication");
    }
  }
  m_logWriter->debug("Tunneling is init");
}

int RemoteViewerCore::initAuthentication()
{
  m_logWriter->debug("Initialization of tight-authentication...");
  unsigned int authTypesNumber = m_input->readUInt32();

  m_logWriter->information("Number of auth-types is {}", authTypesNumber);
  if (authTypesNumber == 0) {
    return AuthDefs::NONE;
  }

  m_logWriter->debug("Reading authentication capability...");
  for (unsigned int i = 0; i < authTypesNumber; i++) {
    RfbCapabilityInfo cap = readCapability();
    m_authCaps.enable(&cap);
  }
  size_t numEnabled = m_authCaps.numEnabled();
  ::array_base<unsigned int> authTypes(numEnabled);
  for (size_t i = 0; i < numEnabled; ++i) {
    authTypes[i] = m_authCaps.getByOrder(i);
  }

  m_logWriter->debug("Authentication capability is read");

  m_logWriter->debug("Select authentication capability");
  if (numEnabled == 0) {
    throw ::remoting::Exception("No security types supported. "
                    "Server sent security types, but we do not support any of their.");
  }
  int typeSelected = authTypes[0];
  m_logWriter->debug("Selected type of authentication: {}", typeSelected);

  m_output->writeUInt32(typeSelected);
  m_output->flush();

  m_logWriter->debug("Tight authentication is init");
  return typeSelected;
}

void RemoteViewerCore::setFbProperties(const ::int_size & fbDimension,
                                       const PixelFormat & fbPixelFormat)
{
#ifdef _DEMO_VERSION_
	m_watermarksController.setNewFbProperties(&fbDimension->getRect(), fbPixelFormat);
#endif

  const PixelFormat &pxFormat = fbPixelFormat;
  ::string pxString;
  pxString.formatf("[bits-per-pixel: {}, depth: {}, big-endian-flag: {}, "
                  "true-color-flag: is set, " // true color always is set
                  "red-max: {}, green-max: {}, blue-max: {}, "
                  "red-shift: {}, green-shift: {}, blue-shift: {}]",
                  pxFormat.bitsPerPixel, pxFormat.colorDepth, pxFormat.bigEndian,
                  pxFormat.redMax, pxFormat.greenMax, pxFormat.blueMax,
                  pxFormat.redShift, pxFormat.greenShift, pxFormat.blueShift);

  m_logWriter->debug("Setting frame buffer properties...");
  m_logWriter->information("Frame buffer dimension: ({}, {})",
                   fbDimension.cx, fbDimension.cy);
  m_logWriter->information("Frame buffer pixel format: {}", pxString);

  if (!m_frameBuffer.setProperties(fbDimension, fbPixelFormat) ||
      !m_rectangleFb.setProperties(fbDimension, fbPixelFormat)) {
    ::string error;
    error.formatf("Failed to set property frame buffer. "
                 "::int_size: ({}, {}), Pixel format: {}",
                 fbDimension.cx, fbDimension.cy,
                 pxString);
    throw ::remoting::Exception(error);
  }
  m_rectangleFb.setColor(0, 0, 0);
  m_frameBuffer.setColor(0, 0, 0);
  refreshFrameBuffer();
  m_fbUpdateNotifier.onPropertiesFb();
  m_logWriter->debug("Frame buffer properties set");
}

::string RemoteViewerCore::getProtocolString() const
{
  ::string protocolString;
  protocolString.formatf("RFB %03d.%03d\n", m_major, m_minor);
  return protocolString;
}

::string RemoteViewerCore::getRemoteDesktopName() const
{
  return m_remoteDesktopName;
}

void RemoteViewerCore::execute()
{
  try {
    // connect to host and create RfbInputGate/RfbOutputGate
    // if already connected, then function do nothing
    m_logWriter->information("Protocol stage is \"Connection establishing\".");
    connectToHost();

    // get server version and set client version
    m_logWriter->information("Protocol stage is \"Handshake\".");
    handshake();

    // negotiaty about security type and authenticate
    m_logWriter->information("Protocol stage is \"Authentication\".");
    authenticate();

    // set shared flag, get server dimension, pixel format and hostname
    // send client pixel format and set him.
    m_logWriter->information("Protocol stage is \"Initialization\".");
    clientAndServerInit();

    // is connected
    m_logWriter->information("Protocol stage is \"Is connected\".");

    try {
      m_adapter->onConnected(m_output);
    } catch (const ::remoting::Exception &ex) {
      m_logWriter->error("Error in CoreEventsAdapter::onConnected(): {}", ex.get_message());
    } catch (...) {
      m_logWriter->error("Unknown error in CoreEventsAdapter::onConnected()");
    }

    {
      AutoLock al(&m_connectLock);
      m_wasConnected = true;
    }

    // send supporting encoding
    m_logWriter->information("Protocol stage is \"Encoding select\".");
    sendEncodings();

    // send ENABLE_CUT_TEXT_UTF8 if server has the capability
    allowUtf8Clipboard();

    // send request of frame buffer update
    m_logWriter->information("Protocol stage is \"Working phase\".");
    sendFbUpdateRequest(false);

    // received server messages
    while (!isTerminating()) {
      unsigned int msgType = receiveServerMessageType();

      switch (msgType) {
      case ServerMsgDefs::FB_UPDATE:
        m_logWriter->debug("Received scopedstrMessage: FB_UPDATE");
        receiveFbUpdate();
        break;

      case ServerMsgDefs::SET_COLOR_MAP_ENTRIES:
        m_logWriter->debug("Received scopedstrMessage: SET_COLOR_MAP_ENTRIES");
        receiveSetColorMapEntries();
        break;

      case ServerMsgDefs::BELL:
        m_logWriter->debug("Received scopedstrMessage: BELL");
        receiveBell();
        break;

      case ServerMsgDefs::SERVER_CUT_TEXT:
        m_logWriter->debug("Received scopedstrMessage: SERVER_CUT_TEXT");
        receiveServerCutText();
        break;
      case ServerMsgDefs::SERVER_CUT_TEXT_UTF8:
        m_logWriter->debug("Received scopedstrMessage: SERVER_CUT_TEXT_UTF8");
        receiveServerCutTextUtf8();
        break;

      default:
        if (m_serverMsgHandlers.find(msgType) != m_serverMsgHandlers.end()) {
          m_logWriter->debug("Received scopedstrMessage ({}) transmit to capability handler", msgType);
          try {
            m_serverMsgHandlers[msgType]->onServerMessage(msgType, m_input);
          } catch (const ::remoting::Exception &ex) {
            m_logWriter->error("Error in onServerMessage(): {}", ex.get_message());
          } catch (...) {
            m_logWriter->error("Unknown error in onServerMessage()");
          }
        } else {
          m_logWriter->error("Server to client scopedstrMessage: {} is not supported", msgType);
        }
      }
    }
    ::string scopedstrMessage("Remote viewer's core thread terminated");
    try {
      m_adapter->onDisconnect(scopedstrMessage);
    } catch (const ::remoting::Exception &ex) {
      m_logWriter->error("Error in CoreEventsAdapter::onDisconnect(): {}", ex.get_message());
    } catch (...) {
      m_logWriter->error("Unknown error in CoreEventsAdapter::onDisconnect()");
    }

  } catch (const AuthException &ex) {
    m_logWriter->error("RemoteVewerCore. Auth exception: {}", ex.get_message());
    try {
      m_adapter->onAuthError(&ex);
    } catch (const ::remoting::Exception &ex) {
      m_logWriter->error("Error in CoreEventsAdapter::onAuthError(): {}", ex.get_message());
    } catch (...) {
      m_logWriter->error("Unknown error in CoreEventsAdapter::onAuthError()");
    }
  } catch (const ::io_exception &ex) {
    try {
      ::string disconnectMessage(ex.get_message());
      m_adapter->onDisconnect(disconnectMessage);
    } catch (const ::remoting::Exception &ex) {
      m_logWriter->error("Error in CoreEventsAdapter::onDisconnect(): {}", ex.get_message());
    } catch (...) {
      m_logWriter->error("Unknown error in CoreEventsAdapter::onDisconnect()");
    }
  } catch (const ::remoting::Exception &ex) {
    m_logWriter->error("RemoteViewerCore. ::remoting::Exception: {}", ex.get_message());
    try {
      m_adapter->onError(&ex);
    } catch (...) {
      m_logWriter->error("Unknown error in CoreEventsAdapter::onError()");
    }
  } catch (...) {
    ::string error;
    error.formatf("RemoteViewerCore. Unknown exception");
    m_logWriter->error("{}", error);
    ::remoting::Exception ex(error);
    try {
      m_adapter->onError(&ex);
    } catch (...) {
      m_logWriter->error("Unknown error in CoreEventsAdapter::onError()");
    }
  }
}

unsigned int RemoteViewerCore::receiveServerMessageType()
{
  // Viewer in common case read first byte (unsigned char) as scopedstrMessage id,
  // but if first byte is equal to 0xFC then it's TightVNC extension scopedstrMessage and
  // must read next 3 bytes and create unsigned int scopedstrMessage id for processing.

  static const unsigned short SERVER_MSG_SPECIAL_TIGHT_CODE = 0xFC;

  unsigned int msgType = m_input->readUInt8();
  if (msgType == SERVER_MSG_SPECIAL_TIGHT_CODE) {
    for (int i = 0; i < 3; i++) {
      msgType <<= 8;
      msgType += m_input->readUInt8();
    }
  }
  return msgType;
}

void RemoteViewerCore::receiveFbUpdate()
{
  // scopedstrMessage type is already known: 0

  // read padding: one byte
  m_input->readUInt8();

  unsigned short numberOfRectangles = m_input->readUInt16();
  m_logWriter->debug("number of rectangles: {}", numberOfRectangles);

  bool isLastRect = false;
  for (int rectangle = 0; rectangle < numberOfRectangles && !isLastRect; rectangle++) {
    m_logWriter->debug("Receiving rectangle #{}...", rectangle);
    isLastRect = receiveFbUpdateRectangle();
  }

  {
    AutoLock al(&m_requestUpdateLock);
    m_isNeedRequestUpdate = true;
  }
  {
    AutoLock al(&m_freezeLock);
    if (m_isFreeze)
      return;
  }
  m_logWriter->debug("Sending of frame buffer update request...");
  sendFbUpdateRequest(!m_forceFullUpdate);
}

bool RemoteViewerCore::receiveFbUpdateRectangle()
{
  ::int_rectangle rect;
  rect.left = m_input->readUInt16();
  rect.top = m_input->readUInt16();
  rect.set_width(m_input->readUInt16());
  rect.set_height(m_input->readUInt16());

  int encodingType = m_input->readInt32();

  m_logWriter->debug("Rectangle: ({}, {}), ({}, {}). Type is {}", 
                    rect.left, rect.top, rect.right, rect.bottom, encodingType);

  if (encodingType == PseudoEncDefs::LAST_RECT)
    return true;
  if (!Decoder::isPseudo(encodingType)) {
    if (::int_rectangle(m_frameBuffer.getDimension()).intersection(rect) != rect) {
      throw ::remoting::Exception("Error in protocol: incorrect size of rectangle");
    }

    Decoder *decoder = m_decoderStore.getDecoder(encodingType);
    if (decoder != 0) {
      m_logWriter->debug("Decoding...");

      DecoderOfRectangle *rectangleDecoder = dynamic_cast<DecoderOfRectangle *>(decoder);
      rectangleDecoder->process(m_input,
                                &m_frameBuffer, &m_rectangleFb, rect, &m_fbLock,
                                &m_fbUpdateNotifier);

      m_logWriter->debug("Decoded");
    } else { // decoder is 0
      ::string errorString;
      errorString.formatf("Decoder \"{}\" isn't exist", encodingType);
      m_logWriter->error("{}", errorString);
      throw ::remoting::Exception(errorString);
    } 
  } else { // it's pseudo encoding
    m_logWriter->debug("It's pseudo encoding");
    processPseudoEncoding(rect, encodingType);
  }
  return false;
}

void RemoteViewerCore::processPseudoEncoding(const ::int_rectangle &  rect,
                                             int encodingType)
{
  switch (encodingType) {
  case PseudoEncDefs::DESKTOP_SIZE:
    m_logWriter->information("Changed size of desktop");
    {
      AutoLock al(&m_fbLock);
      setFbProperties(rect.size(), m_frameBuffer.getPixelFormat());
    }
    break;
    
  case PseudoEncDefs::RICH_CURSOR:
    {
      m_logWriter->debug("New rich cursor");

      unsigned short width = rect.width();
      unsigned short height = rect.height();
      unsigned char bytesPerPixel = m_frameBuffer.getBytesPerPixel();

      ::array_base<unsigned char> cursor;
      ::array_base<unsigned char> bitmask;

      size_t cursorLen = width * height * bytesPerPixel;
      if (cursorLen != 0) {
        cursor.resize(cursorLen);
        m_input->readFully(cursor.data(), cursorLen);

        size_t bitmaskLen = ((width + 7) / 8) * height;
        bitmask.resize(bitmaskLen);
        m_input->readFully(bitmask.data(), bitmaskLen);
      }
      Point hotSpot(rect.left, rect.top);

      m_logWriter->debug("Setting new rich cursor...");
      m_fbUpdateNotifier.setNewCursor(&hotSpot, width, height,
                                      &cursor, &bitmask);
    }
    break;

  case PseudoEncDefs::POINTER_POS:
    {
      m_logWriter->debug("Updating pointer position: [{}, {}]", rect.left, rect.top);
      Point position(rect.left, rect.top);
      m_fbUpdateNotifier.updatePointerPos(&position);
    }
    break;

  default:
    ::string errorString;
    errorString.formatf("Pseudo encoding {} is not supported", encodingType);
    m_logWriter->error("{}", errorString);
    throw ::remoting::Exception(errorString);
  }
}

void RemoteViewerCore::receiveSetColorMapEntries()
{
  // scopedstrMessage type is already known: 1

  // read padding: 1 byte
  m_input->readUInt8();

  unsigned short firstColour = m_input->readUInt16();
  unsigned short numberOfColours = m_input->readUInt16();
  for (size_t i = 0; i < numberOfColours; i++) {
    unsigned short red = m_input->readUInt16();
    unsigned short green = m_input->readUInt16();
    unsigned short blue = m_input->readUInt16();
  }
}

void RemoteViewerCore::receiveBell()
{
  // scopedstrMessage is already readed. Message type: 2

  m_logWriter->information("Bell!");
  try {
    m_adapter->onBell();
  } catch (const ::remoting::Exception &ex) {
    m_logWriter->error("Error in CoreEventsAdapter::onBell(): {}", ex.get_message());
  } catch (...) {
    m_logWriter->error("Unknown error in CoreEventsAdapter::onBell()");
  }
}

void RemoteViewerCore::receiveServerCutText()
{
  // scopedstrMessage type is already known: 3

  // read padding: three byte
  m_input->readUInt16();
  m_input->readUInt8();

  unsigned int length = m_input->readUInt32();
  ::array_base<char> buffer(length + 1);
  m_input->readFully(buffer.data(), length);
  buffer[length] = '\0';
  ::string cutText;
  ::string cutTextAnsi(buffer.data());
  cutText = cutTextAnsi;

  m_logWriter->debug("Cut text: {}", cutText);
  try {
    m_adapter->onCutText(cutText);
  } catch (const ::remoting::Exception &ex) {
    m_logWriter->error("Error in CoreEventsAdapter::onCutText(): {}", ex.get_message());
  } catch (...) {
    m_logWriter->error("Unknown error in CoreEventsAdapter::onCutText()");
  }
}

void RemoteViewerCore::receiveServerCutTextUtf8()
{
  unsigned int length = m_input->readUInt32();
  ::array_base<char> buffer(length + 1);
  m_input->readFully(buffer.data(), length);
  buffer[length] = '\0';
  ::string cutText;
  ::string cutTextUtf8(buffer.data());
  cutText=cutTextUtf8;

  m_logWriter->debug("Cut text: {}", cutText);
  try {
    m_adapter->onCutText(cutText);
  }
  catch (const ::remoting::Exception &ex) {
    m_logWriter->error("Error in CoreEventsAdapter::onCutText(): {}", ex.get_message());
  }
  catch (...) {
    m_logWriter->error("Unknown error in CoreEventsAdapter::onCutText()");
  }
}

bool RemoteViewerCore::isRfbProtocolString(const char protocol[12]) const
{
  // Format protocol version "RFB XXX.YYY\n"
  // where XXX is major version and YYY is minor version of protocol
  // TODO: replace "if" to regexp

  if (strncmp(protocol, "RFB ", 4) != 0)
    return false;

  if (!isdigit(protocol[4]) || !isdigit(protocol[5]) || !isdigit(protocol[6]))
    return false;

  if (protocol[7] != '.')
    return false;

  if (!isdigit(protocol[8]) || !isdigit(protocol[9]) || !isdigit(protocol[10]))
    return false;

  if (protocol[11] != '\n')
    return false;

  return true;
}

void RemoteViewerCore::handshake()
{
  char serverProtocol[13];
  serverProtocol[12] = 0;
  m_input->readFully(serverProtocol, 12);

  m_major = strtol(&serverProtocol[4], 0, 10);
  m_minor = strtol(&serverProtocol[8], 0, 10);
  m_isTight = false;

  m_logWriter->information("Server sent protocol version: {}", getProtocolString());
  if (!isRfbProtocolString(serverProtocol) || 
      m_major < 3 ||
      (m_major == 3 && m_minor < 3)) {
    ::string error;
    ::string protocolAnsi(serverProtocol);
    ::string protocol;
    protocol = protocolAnsi;
    error.format("Unsupported protocol: {}", protocol);
    m_logWriter->error("{}", error);
    throw ::remoting::Exception(error);
  }

  // if version is 4.0 or later, then set version 3.8.
  if (m_major > 3) {
    m_major = 3;
    m_minor = 8;
  } else { // else set version from ::list_base 3.3, 3.7, 3.8.
    // select minor version:
    if (m_minor < 7) {
      m_minor = 3;
    }
    if (m_minor >= 8) {
     m_minor = 8;
    }
  }

  m_logWriter->information("Send to server protocol version: {}", getProtocolString());

  ::string clientProtocolAnsi;
  clientProtocolAnsi= getProtocolString();
  m_output->write(clientProtocolAnsi, 12);
  m_output->flush();
}

/**
 * Client send:
 * 2           - U8          - shared flag
 *
 * Server send:
 * 2           - U16         - framebuffer-width
 * 2           - U16         - framebuffer-height
 * 16          - PixelFormat - server-pixel-format
 * 4           - U32         - name-length
 * name-length - U8 array    - name-string
 */
void RemoteViewerCore::clientAndServerInit()
{
  if (m_sharedFlag) {
    m_logWriter->information("Setting share flag in on...");
  } else {
    m_logWriter->information("Setting share flag is off...");
  }
  m_output->writeUInt8(m_sharedFlag);
  m_output->flush();
  m_logWriter->debug("Shared flag is set");

  unsigned short width = m_input->readUInt16();
  unsigned short height = m_input->readUInt16();
  ::int_size screenDimension(width, height);
  PixelFormat serverPixelFormat = readPixelFormat();
  
  {
    AutoLock al(&m_fbLock);
    setFbProperties(screenDimension, serverPixelFormat);
  }

  unsigned int sizeInBytes = m_input->readUInt32();
  ::array_base<char> buffer(sizeInBytes + 1);
  m_input->read(buffer.data(), sizeInBytes);
  buffer[sizeInBytes] = '\0';
  ::string ansiStr;
  ansiStr= buffer.data();
  m_remoteDesktopName = ansiStr;
  m_logWriter->information("Server remote name: {}", m_remoteDesktopName);

  if (m_isTight) {
    m_logWriter->debug("Reading tight capabilities");
    readCapabilities();
  }
}

/**
 * Struct filled from the Capabilities scopedstrMessage
 * 2  - U16         - nServerMessageTypes
 * 2  - U16         - nClientMessageTypes
 * 2  - U16         - nEncodingTypes
 * 2  - U16         - padding. Reserved, must be 0
 * followed by nServerMessageTypes * rfbCapabilityInfo structures
 * followed by nClientMessageTypes * rfbCapabilityInfo structures
 * followed by       nEncodingType * rfbCapabilityInfo structures
 */
void RemoteViewerCore::readCapabilities()
{
  int nServerMessageTypes = m_input->readUInt16();
  int nClientMessageTypes = m_input->readUInt16();
  int nEncodingTypes = m_input->readUInt16();
  m_input->readUInt16(); //padding

  m_logWriter->debug("Server scopedstrMessage types (capability):");
  while (nServerMessageTypes--) {
    RfbCapabilityInfo cap = readCapability();
    m_serverMsgCaps.enable(&cap);
  }

  m_logWriter->debug("Client scopedstrMessage types (capability):");
  while (nClientMessageTypes--) {
    RfbCapabilityInfo cap = readCapability();
    m_clientMsgCaps.enable(&cap);
  }

  m_logWriter->debug("Encodings (capability):");
  while (nEncodingTypes--) {
    RfbCapabilityInfo cap = readCapability();
    m_encodingCaps.enable(&cap);
  }

  // Add active encoding-capabilities in DecoderStore.
  for (size_t i = 0; i < m_encodingCaps.numEnabled(); i++) {
    m_decoderStore.addDecoder(m_decoderHandlers[m_encodingCaps.getByOrder(i)],
                              m_decoderPriority[m_encodingCaps.getByOrder(i)]);
  }
}

RfbCapabilityInfo RemoteViewerCore::readCapability()
{
  // read capability
  RfbCapabilityInfo cap;
  cap.code = m_input->readUInt32();
  m_input->readFully(cap.vendorSignature, RfbCapabilityInfo::vendorSigSize);
  m_input->readFully(cap.nameSignature, RfbCapabilityInfo::nameSigSize);

  // transform information for the log and log him
  char vendorSignature[RfbCapabilityInfo::vendorSigSize + 1];
  memcpy(vendorSignature, cap.vendorSignature, RfbCapabilityInfo::vendorSigSize);
  vendorSignature[RfbCapabilityInfo::vendorSigSize] = 0;
  ::string vendorSignatureAnsiString(vendorSignature);
  ::string vendorSignatureString;
  vendorSignatureString = vendorSignatureAnsiString;

  char nameSignature[RfbCapabilityInfo::nameSigSize + 1];
  memcpy(nameSignature, cap.nameSignature, RfbCapabilityInfo::nameSigSize);
  nameSignature[RfbCapabilityInfo::nameSigSize] = 0;
  ::string nameSignatureAnsiString(nameSignature);
  ::string nameSignatureString;
  nameSignatureString = nameSignatureAnsiString;

  m_logWriter->debug("code: {}, vendor: {}, signature: {}",
                     cap.code, vendorSignatureString, nameSignatureString);

  // return capability info
  return cap;
}

void RemoteViewerCore::addAuthCapability(AuthHandler *authHandler,
                                         unsigned int code,
                                         const char *vendorSignature,
                                         const char *nameSignature,
                                         const ::string description)
{
  m_authCaps.add(code, vendorSignature, nameSignature, description);
  registerAuthHandler(code, authHandler);
}

void RemoteViewerCore::addServerMsgCapability(ServerMessageListener *listener,
                                              unsigned int code,
                                              const char *vendorSignature,
                                              const char *nameSignature,
                                              const ::string description)
{
  m_serverMsgCaps.add(code, vendorSignature, nameSignature, description);
  registerMessageListener(code, listener);
}

void RemoteViewerCore::addClientMsgCapability(unsigned int code,
                                              const char *vendorSignature,
                                              const char *nameSignature,
                                              const ::string description)
{
  m_clientMsgCaps.add(code, vendorSignature, nameSignature, description);
}

void RemoteViewerCore::addEncodingCapability(Decoder *decoder,
                                             int priorityEncoding,
                                             unsigned int code,
                                             const char *vendorSignature,
                                             const char *nameSignature,
                                             const ::string description)
{
  m_encodingCaps.add(code, vendorSignature, nameSignature, description);
  registerDecoderHandler(code, decoder, priorityEncoding);
}

void RemoteViewerCore::getEnabledClientMsgCapabilities(::array_base<unsigned int> *codes) const
{
  m_clientMsgCaps.getEnabledCapabilities(*codes);
}

void RemoteViewerCore::getEnabledServerMsgCapabilities(::array_base<unsigned int> *codes) const
{
  m_serverMsgCaps.getEnabledCapabilities(*codes);
}

void RemoteViewerCore::getEnabledEncodingCapabilities(::array_base<unsigned int> *codes) const
{
  m_encodingCaps.getEnabledCapabilities(*codes);
}

void RemoteViewerCore::registerAuthHandler(unsigned int code, AuthHandler *handler)
{
  m_authHandlers[code] = handler;
}

void RemoteViewerCore::registerMessageListener(unsigned int code, ServerMessageListener *listener)
{
  m_serverMsgHandlers[code] = listener;
}

void RemoteViewerCore::registerDecoderHandler(unsigned int code, Decoder *decoder, int priority)
{
  m_decoderHandlers[code] = decoder;
  m_decoderPriority[code] = priority;
}

void RemoteViewerCore::sendEncodings()
{
  // If core isn't connected, then m_output may be isn't initialized.
  // Exit from function, if it is.
  if (!wasConnected()) {
    return;
  }

  RfbSetEncodingsClientMessage encodingsMessage(m_decoderStore.getDecoderIds());
  encodingsMessage.send(m_output);
}

::array_base<::int_rectangle> RemoteViewerCore::getDesktops()
{
  return m_desktops;
}

::int_size RemoteViewerCore::getDesktopSize()
{
  return m_desktopSize;
}
