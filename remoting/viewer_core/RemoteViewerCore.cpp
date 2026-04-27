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
// with this program; if not, w_rite to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//
#include "framework.h"
#include "RemoteViewerCore.h"

#include "remoting/remoting/file_transfer_common/FTMessage.h"
#include "remoting/remoting/rfb/AuthDefs.h"
#include "remoting/remoting/rfb/TunnelDefs.h"
#include "remoting/remoting/rfb/MsgDefs.h"
#include "remoting/remoting/rfb/EncodingDefs.h"
#include "remoting/remoting/rfb/VendorDefs.h"
//#include "subsystem/platform/::string.h"
////#include "subsystem/platform/::string.h"

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

//#include aaa_<algorithm>

namespace remoting_client
{
   RemoteViewerCore::RemoteViewerCore(::subsystem::LogWriter * plogwriter)
   : m_plogwriter (plogwriter),
     //m_tcpConnection(m_plogwriter),
     //m_pfbupdatenotifier(m_pframebuffer, &m_criticalsectionFramebuffer, m_plogwriter, &m_pwatermarkscontroller),
     m_decoderStore(m_plogwriter),
     //m_updateRequestSender(&m_criticalsectionFramebuffer, m_pframebuffer, m_plogwriter),
     m_dispatchDataProvider(0),
     m_isTightEnabled(true),
     m_isUtf8ClipboardEnabled(false)
   {

      raw_construct_newø(m_pfbupdatenotifier, m_pframebuffer, &m_criticalsectionFramebuffer, m_plogwriter, &m_pwatermarkscontroller);
      raw_construct_newø(m_pupdaterequestsenderProperty, &m_criticalsectionFramebuffer, m_pframebuffer, m_plogwriter);
      m_tcpConnection.initialize_tcp_connection(m_plogwriter);
      init();
   }

   RemoteViewerCore::RemoteViewerCore(const ::scoped_string & scopedstrHost, unsigned short port,
                                      CoreEventsAdapter *adapter,
                                      ::subsystem::LogWriter * plogwriter,
                                      bool sharedFlag)
   : m_plogwriter(plogwriter),
     //m_tcpConnection(m_plogwriter),
     //m_pfbupdatenotifier(m_pframebuffer, &m_criticalsectionFramebuffer, m_plogwriter, &m_pwatermarkscontroller),
     m_decoderStore(m_plogwriter),
     //m_updateRequestSender(&m_criticalsectionFramebuffer, m_pframebuffer, m_plogwriter),
     m_dispatchDataProvider(0),
     m_isTightEnabled(true),
     m_isUtf8ClipboardEnabled(false)
   {

      raw_construct_newø(m_pfbupdatenotifier, m_pframebuffer, &m_criticalsectionFramebuffer, m_plogwriter, &m_pwatermarkscontroller);
      raw_construct_newø(m_pupdaterequestsenderProperty, &m_criticalsectionFramebuffer, m_pframebuffer, m_plogwriter);

      m_tcpConnection.initialize_tcp_connection(m_plogwriter);
      init();

      start(scopedstrHost, port, adapter, sharedFlag);
   }

   RemoteViewerCore::RemoteViewerCore(::subsystem::SocketIPv4Interface * psocket,
                                      CoreEventsAdapter *adapter,
                                      ::subsystem::LogWriter * plogwriter,
                                      bool sharedFlag)
   : m_plogwriter(plogwriter),
     //m_tcpConnection(m_plogwriter),
     //m_pfbupdatenotifier(m_pframebuffer, &m_criticalsectionFramebuffer, m_plogwriter, &m_pwatermarkscontroller),
     m_decoderStore(m_plogwriter),
     //m_updateRequestSender(&m_criticalsectionFramebuffer, m_pframebuffer, m_plogwriter),
     m_dispatchDataProvider(0),
     m_isTightEnabled(true),
     m_isUtf8ClipboardEnabled(false)
   {

      raw_construct_newø(m_pfbupdatenotifier, m_pframebuffer, &m_criticalsectionFramebuffer, m_plogwriter, &m_pwatermarkscontroller);
      raw_construct_newø(m_pupdaterequestsenderProperty, &m_criticalsectionFramebuffer, m_pframebuffer, m_plogwriter);

      m_tcpConnection.initialize_tcp_connection(m_plogwriter);
      init();

      start(psocket, adapter, sharedFlag);
   }

   RemoteViewerCore::RemoteViewerCore(::remoting::RfbInputGate *input, ::remoting::RfbOutputGate *output,
                                      CoreEventsAdapter *adapter,
                                      ::subsystem::LogWriter * plogwriter,
                                      bool sharedFlag)
   : m_plogwriter (plogwriter),
     //m_tcpConnection(m_plogwriter),
     //m_pfbupdatenotifier(m_pframebuffer, &m_criticalsectionFramebuffer, m_plogwriter, &m_pwatermarkscontroller),
     m_decoderStore(m_plogwriter),
     //m_updateRequestSender(&m_criticalsectionFramebuffer, m_pframebuffer, m_plogwriter),
     m_dispatchDataProvider(0),
     m_isTightEnabled(true),
     m_isUtf8ClipboardEnabled(false)
   {

      raw_construct_newø(m_pfbupdatenotifier, m_pframebuffer, &m_criticalsectionFramebuffer, m_plogwriter, &m_pwatermarkscontroller);
      raw_construct_newø(m_pupdaterequestsenderProperty, &m_criticalsectionFramebuffer, m_pframebuffer, m_plogwriter);

      m_tcpConnection.initialize_tcp_connection(m_plogwriter);
      init();

      start(input, output, adapter, sharedFlag);
   }

   void RemoteViewerCore::init()
   {
      m_decoderStore.addDecoder(new RawDecoder(m_plogwriter), 0);
      m_decoderStore.addDecoder(new CopyRectDecoder(m_plogwriter), 10);
      m_decoderStore.addDecoder(new RreDecoder(m_plogwriter), 1);
      m_decoderStore.addDecoder(new HexTileDecoder(m_plogwriter), 4);
      m_decoderStore.addDecoder(new TightDecoder(m_plogwriter), 9);
      m_decoderStore.addDecoder(new ZrleDecoder(m_plogwriter), 9);

      m_decoderStore.addDecoder(new DesktopSizeDecoder(m_plogwriter), -1);
      m_decoderStore.addDecoder(new LastRectDecoder(m_plogwriter), -1);
      m_decoderStore.addDecoder(new PointerPosDecoder(m_plogwriter), -1);
      m_decoderStore.addDecoder(new RichCursorDecoder(m_plogwriter), -1);
      m_input = 0;
      m_output = 0;

      m_wasStarted = false;
      m_wasConnected = false;
      m_bNewPixelFormat = false;
      m_bFreeze = false;
      m_bNeedRequestUpdate = true;
      m_forceFullUpdate = false;

      m_updateTimeout = 0;

      addClientMsgCapability(::remoting::ClientMsgDefs::CLIENT_CUT_TEXT_UTF8,
        ::remoting::VendorDefs::TIGHTVNC,
        ::remoting::Utf8CutTextDefs::CLIENT_CUT_TEXT_UTF8_SIG,
        "UTF-8 clipboard");

      addClientMsgCapability(::remoting::ClientMsgDefs::ENABLE_CUT_TEXT_UTF8,
        ::remoting::VendorDefs::TIGHTVNC,
        ::remoting::Utf8CutTextDefs::ENABLE_CUT_TEXT_UTF8_SIG,
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
            m_pfbupdatenotifier->wait();
            m_pupdaterequestsenderProperty->wait();
         }
      } catch (...) {
      }
   }

   void RemoteViewerCore::start(CoreEventsAdapter *adapter,
                                bool sharedFlag)
   {
      m_plogwriter->debug("Starting remote viewer core...");

      if (adapter == 0) {
         throw ::subsystem::Exception("Remote viewer core is not started: adapter is 0");
      }

      // Set flag "wasStarted".
      // This flag protects of second call start().
      {
         critical_section_lock al(&m_startLock);
         if (m_wasStarted) {
            throw ::subsystem::Exception("Remote viewer core is already started");
         }
         m_wasStarted = true;
      }

      m_bShared = sharedFlag;
      m_pcoreeventsadapter = adapter;

      m_pfbupdatenotifier->setAdapter(adapter);

      // Start thread.
      resume();
      m_plogwriter->debug("Remote viewer core is started");
   }

   void RemoteViewerCore::start(const ::scoped_string & scopedstrHost,
                                unsigned short port,
                                CoreEventsAdapter *adapter,
                                bool sharedFlag)
   {
      m_tcpConnection.bind(scopedstrHost, port);
      start(adapter, sharedFlag);
   }
   void RemoteViewerCore::start(::subsystem::SocketIPv4Interface *socket,
                                CoreEventsAdapter *adapter,
                                bool sharedFlag)
   {
      m_tcpConnection.bind(socket);
      start(adapter, sharedFlag);
   }

   void RemoteViewerCore::start(::remoting::RfbInputGate *input, ::remoting::RfbOutputGate *output,
                                CoreEventsAdapter *adapter,
                                bool sharedFlag)
   {
      m_tcpConnection.bind(input, output);
      start(adapter, sharedFlag);
   }

   bool RemoteViewerCore::wasStarted() const
   {
      critical_section_lock al(&m_startLock);
      return m_wasStarted;
   }

   bool RemoteViewerCore::wasConnected() const
   {
      critical_section_lock al(&m_connectLock);
      return m_wasConnected;
   }

   void RemoteViewerCore::stop()
   {
      {
         // We should use locking to prevent simultaneous reading and writing of
         // the m_dispatchDataProvider pointer.
         critical_section_lock al(&m_dispatchDataProviderLock);
         m_dispatchDataProvider = 0;
      }

      m_pupdaterequestsenderProperty->terminate();

      m_tcpConnection.close();
      m_pfbupdatenotifier->terminate();
      terminate();
   }

   void RemoteViewerCore::waitTermination()
   {
      m_pfbupdatenotifier->wait();
      m_pupdaterequestsenderProperty->wait();
      wait();
   }

   void RemoteViewerCore::setPixelFormat(const ::innate_subsystem::PixelFormat & pixelFormat)
   {
      m_plogwriter->debug("Pixel format will changed");
      critical_section_lock al(&m_criticalsectionPixelFormat);
      m_bNewPixelFormat = true;
      m_pixelformatViewer = pixelFormat;
   }

   void RemoteViewerCore::enableDispatching(DispatchDataProvider *src)
   {
      critical_section_lock al(&m_startLock);
      if (!m_wasStarted) {
         // In other places, we use locking to access this pointer
         // (see m_dispatchDataProviderLock), but not here, as we assume there is
         // no concurrent access to this variable prior to calling start().
         m_dispatchDataProvider = src;
      }
   }

   bool RemoteViewerCore::updatePixelFormat()
   {
      ::innate_subsystem::PixelFormat pixelformat;
      m_plogwriter->debug("Check pixel format change...");
      {
         critical_section_lock al(&m_criticalsectionPixelFormat);
         if (!m_bNewPixelFormat)
            return false;
         m_bNewPixelFormat = false;
         pixelformat = m_pixelformatViewer;
      }

      int bitsPerPixel = m_pixelformatViewer.bitsPerPixel;
      if (bitsPerPixel != 8 && bitsPerPixel != 16 && bitsPerPixel != 32) {
         throw ::subsystem::Exception("Only 8, 16 or 32 bits per pixel supported!");
      }

      {
         critical_section_lock al(&m_criticalsectionFramebuffer);
         // FIXME: here isn't accept true-colour flag.
         // PixelFormats may be equal, if isn't.
         if (pixelformat == m_pframebuffer->getPixelFormat() ){
            return false;
         }
         if (m_pframebuffer->getBuffer() != 0)
            setFbProperties(m_pframebuffer->getDimension(), pixelformat);
      }

      RfbSetPixelFormatClientMessage pixelFormatMessage(pixelformat);
      pixelFormatMessage.send(m_output);

      return true;
   }

   void RemoteViewerCore::refreshFramebuffer()
   {
      m_plogwriter->debug("Frame buffer will refreshed");
      critical_section_lock al(&m_criticalsectionLock);
      m_bRefreshing = true;
   }

   void RemoteViewerCore::forceFullUpdateRequests(const bool& forceUpdate)
   {
      m_forceFullUpdate = forceUpdate;
      m_pupdaterequestsenderProperty->setIsIncremental(!forceUpdate);
   }

   void RemoteViewerCore::deferUpdateRequests(const int& milliseconds)
   {
      m_updateTimeout = milliseconds;
      m_pupdaterequestsenderProperty->setTimeout(milliseconds);
   }

   void RemoteViewerCore::sendFbUpdateRequest(bool incremental)
   {
      {
         critical_section_lock al(&m_criticalsectionRequestUpdate);
         bool requestUpdate = m_bNeedRequestUpdate;
         m_bNeedRequestUpdate = false;
         if (!requestUpdate)
            return;
      }

      bool isRefresh = false;
      bool isUpdateFbProperties = false;
      if (updatePixelFormat()) {
         isUpdateFbProperties = true;
      }

      {
         critical_section_lock al(&m_criticalsectionLock);
         if (m_bRefreshing) {
            m_bRefreshing= false;
            isRefresh = true;
         }
      }

      if (isRefresh || isUpdateFbProperties || m_pupdaterequestsenderProperty->getTimeout() <= 0)
      {
         bool isIncremental = incremental && !isRefresh && !isUpdateFbProperties;
         ::int_rectangle updateRect;
         {
            critical_section_lock al(&m_criticalsectionFramebuffer);
            updateRect = m_pframebuffer->getDimension();
         }

         if (isIncremental) {
            m_plogwriter->debug("Sending frame buffer incremental update request [%dx{}]...",
                              updateRect.width(), updateRect.height());
         } else {
            m_plogwriter->debug("Sending frame buffer full update request [%dx{}]...",
                              updateRect.width(), updateRect.height());
         }

         RfbFramebufferUpdateRequestClientMessage fbUpdReq(isIncremental, updateRect);
         fbUpdReq.send(m_output);
         m_plogwriter->debug("Frame buffer update request is sent");
      }
      else
      {
         m_pupdaterequestsenderProperty->setWasUpdated();
      }
   }

   void RemoteViewerCore::sendKeyboardEvent(bool downFlag, unsigned int key)
   {
      // If core isn't connected, then m_output may be isn't initialized.
      // Exit from function, if it is.
      if (!wasConnected()) {
         return;
      }

      m_plogwriter->debug("Sending key event: {}, {}...", downFlag, key);
      RfbKeyEventClientMessage keyMessage(downFlag, key);
      keyMessage.send(m_output);
      m_plogwriter->debug("Key event: {}, {} is sent", downFlag, key);
   }

   void RemoteViewerCore::sendPointerEvent(unsigned char buttonMask,
                                           const ::int_point &pointPosition)
   {
      // If core isn't connected, then m_output may be isn't initialized.
      // Exit from function, if it is.
      if (!wasConnected()) {
         return;
      }

      m_plogwriter->debug("Sending pointer event 0x%X, ({}, {})...",
                         static_cast<int>(buttonMask), pointPosition.x, pointPosition.y);
      // send pointPosition to server
      RfbPointerEventClientMessage pointerMessage(buttonMask, pointPosition);
      pointerMessage.send(m_output);
      // update pointPosition
      m_pfbupdatenotifier->updatePointerPos(pointPosition);

      m_plogwriter->debug("Pointer event: 0x%X, ({}, {}) is sent",
                        static_cast<int>(buttonMask), pointPosition.x, pointPosition.y);
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
         m_plogwriter->debug("Sending UTF-8 clipboard cut text: \"{}\"...", cutText);
         cutTextMessage.sendUtf8(m_output);
      }
      else {
         m_plogwriter->debug("Sending clipboard cut text: \"{}\"...", cutText);
         cutTextMessage.send(m_output);
      }
      m_plogwriter->debug("Clipboard cut text: \"{}\" is sent", cutText);
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
      m_isUtf8ClipboardEnabled = m_clientMsgCaps.isEnabled(::remoting::ClientMsgDefs::CLIENT_CUT_TEXT_UTF8);
      if (m_isUtf8ClipboardEnabled) {
         m_plogwriter->debug("Server supports Utf8 Clipboard recieving.");
      }
      else {
         m_plogwriter->debug("Server does not support Utf8 Clipboard recieving, ClientCutTextUtf8 messages disabled.");
      }
      if (m_clientMsgCaps.isEnabled(::remoting::ClientMsgDefs::ENABLE_CUT_TEXT_UTF8)) {
         m_plogwriter->debug("Sending EnableCutTextUtf8 scopedstrMessage.");
         critical_section_lock al(m_output);
         m_output->writeUInt32(::remoting::ClientMsgDefs::ENABLE_CUT_TEXT_UTF8);
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
            needUpdate |= m_decoderStore.removeDecoder(CompressionLevel(m_plogwriter, level).getCode());

      // new compression level is valid?
      if (newLevel < CompressionLevel::COMPRESSION_LEVEL_MIN ||
          newLevel > CompressionLevel::COMPRESSION_LEVEL_MAX)
         return;

      needUpdate |= m_decoderStore.addDecoder(new CompressionLevel(m_plogwriter, newLevel), -1);
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
            needUpdate |= m_decoderStore.removeDecoder(JpegQualityLevel(m_plogwriter, level).getCode());

      // new jpeg quality level is valid?
      if (newLevel < JpegQualityLevel::JPEG_QUALITY_LEVEL_MIN ||
          newLevel > JpegQualityLevel::JPEG_QUALITY_LEVEL_MAX)
         return;

      needUpdate |= m_decoderStore.addDecoder(new JpegQualityLevel(m_plogwriter, newLevel), -1);
      if (needUpdate) {
         sendEncodings();
      }
   }

   void RemoteViewerCore::enableCursorShapeUpdates(bool enabled)
   {
      bool needUpdate = false;
      if (enabled) {
         needUpdate |= m_decoderStore.addDecoder(new RichCursorDecoder(m_plogwriter), -1);
         needUpdate |= m_decoderStore.addDecoder(new PointerPosDecoder(m_plogwriter), -1);
      } else {
         needUpdate |= m_decoderStore.removeDecoder(::remoting::PseudoEncDefs::RICH_CURSOR);
         needUpdate |= m_decoderStore.removeDecoder(::remoting::PseudoEncDefs::POINTER_POS);
      }

      if (needUpdate) {
         sendEncodings();
      }
   }

   void RemoteViewerCore::ignoreCursorShapeUpdates(bool ignored)
   {
      m_pfbupdatenotifier->setIgnoreShapeUpdates(ignored);
   }

   void RemoteViewerCore::stopUpdating(bool isStopped)
   {
      {
         critical_section_lock al(&m_criticalsectionFreeze);
         if (isStopped == m_bFreeze)
            return;
         m_bFreeze = isStopped;
      }
      if (!isStopped) {
         m_plogwriter->debug("Sending of frame buffer update request...");
         sendFbUpdateRequest(!m_forceFullUpdate);
      }
   }

   ::innate_subsystem::PixelFormat RemoteViewerCore::readPixelFormat()
   {
      ::innate_subsystem::PixelFormat pixelFormat;
      pixelFormat.bitsPerPixel = m_input->readUInt8();
      pixelFormat.colorDepth = m_input->readUInt8();
      pixelFormat.bigEndian = !!m_input->readUInt8();
      // now, supported only true color
      int trueColour = m_input->readUInt8();
      if (trueColour == false) {
         m_plogwriter->error("Not supported palette. Flag \"True colour\" is not set.");
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

      m_pupdaterequestsenderProperty->setOutput(m_output);

      m_plogwriter->debug("Connection is established");
      try {
         m_pcoreeventsadapter->onEstablished();
      } catch (const ::subsystem::Exception &ex) {
         m_plogwriter->error("Error in CoreEventsAdapter::onEstablished(): {}", ex.get_message());
      } catch (...) {
         m_plogwriter->error("Unknown error in CoreEventsAdapter::onEstablished()");
      }
   }

   void RemoteViewerCore::authenticate()
   {
      m_plogwriter->debug("Negotiating security type...");
      int authenticationType = negotiateSecurityType();
      m_plogwriter->information("Authentication type accepted: {} ({})",
                       getAuthenticationTypeName(authenticationType),
                       authenticationType);

      if (authenticationType != 0) {
         m_plogwriter->debug("Authentication...");
         if (m_authHandlers.find(authenticationType) != m_authHandlers.end()) {
            m_authHandlers[authenticationType]->authenticate(m_input, m_output);
         } else {
            // Security type is added automatic, but not by user.
            if (authenticationType != ::remoting::SecurityDefs::NONE) {
               m_plogwriter->error("Isn't exist authentication handler for selected security type {}",
                                 authenticationType);
               throw AuthException("Isn't exist authentication handler "
                                   "for selected type of authentication");
            }
         }
      }

      // get authentication result, if version 3.8 or authentication isn't None
      if (m_minor >= 8 || authenticationType != ::remoting::SecurityDefs::NONE) {
         unsigned int authResult = 0;
         if (authenticationType) {
            authResult = m_input->readUInt32();
            m_plogwriter->debug("Auth result is {}", authResult);
         }
         static const unsigned int AUTH_RESULT_OK = 0;
         if (!authenticationType || authResult != AUTH_RESULT_OK) {
            // if version 3.3 or 3.7 then server connection closed
            m_plogwriter->error("Authentication failure");
            if (m_minor < 8) {
               throw AuthException("Authentication failure");
            }
            // if version 3.8 then try read reasonAuth.
            ::string reasonAuth;
            reasonAuth = m_input->readUtf8();
            ::string errorMessage = "Authentication reason: ";
            errorMessage+=reasonAuth;
            m_plogwriter->error("{}", errorMessage);
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
      m_plogwriter->debug("Reading ::list_base of security types...");
      // read ::list_base of security types
      ::array_base<unsigned int> secTypes;
      readSecurityTypeList(&secTypes);
      m_plogwriter->debug("List of security type is read");
      if (secTypes.size() == 0) {
         m_plogwriter->warning("Error in negotiate about of security: only security type is 0");
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
      m_plogwriter->debug("Security Types received ({}): {}",
                       secTypes.size(), secTypeString);

      // select type security
      m_plogwriter->debug("Selecting auth-handler");
      int typeSelected = selectSecurityType(&secTypes, &m_authHandlers, m_isTightEnabled);
      m_plogwriter->information("Security type is selected: {}", typeSelected);
      if (typeSelected == ::remoting::SecurityDefs::TIGHT) {
         m_plogwriter->information("Tight pcapabilitiesmanager is enable");
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
         case ::remoting::SecurityDefs::NONE:
            return "None";
         case ::remoting::SecurityDefs::VNC:
            return "VNC";
         case ::remoting::SecurityDefs::TIGHT:
            return "Tight";
      }
      return "Unknown type";
   }

   ::string RemoteViewerCore::getAuthenticationTypeName(unsigned int authenticationType) const
   {
      switch (authenticationType) {
         case ::remoting::AuthDefs::NONE:
            return "None";
         case ::remoting::AuthDefs::VNC:
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
                                                   ::remoting::SecurityDefs::TIGHT);
         if (tightSecType != secTypes->end()) {
            return *tightSecType;
         }
      }
      // If server doesn't support security type "Tight", then search first type, which is supported.
      for (SecTypesIterator i = secTypes->begin();
           i != secTypes->end();
           i++) {
         if (authHandlers->find(*i) != authHandlers->end() ||
             *i == ::remoting::SecurityDefs::NONE)
            return *i;
           }

      throw ::subsystem::Exception("No security types supported. "
                      "Server sent security types, "
                      "but we do not support any of them.");
   }

   void RemoteViewerCore::initTunnelling()
   {
      m_plogwriter->debug("Initialization of tight-tunneling...");
      unsigned int tunnelCount = m_input->readUInt32();
      if (tunnelCount > 0) {
         bool hasNoTunnel = false;
         for (unsigned int i = 0; i < tunnelCount; i++) {
            RfbCapabilityInfo cap = readCapability();
            if (cap.code == ::remoting::TunnelDefs::NOTUNNEL) {
               hasNoTunnel = true;
            }
            // Special case for VNC server of Siemense PLC . It supports NOTUNNEL while there is no it in the ::list_base.
            if (cap.isEqual("SICR", "SCHANNEL")) {
               hasNoTunnel = true;
            }
         }
         if (hasNoTunnel) {
            m_output->writeUInt32(::remoting::TunnelDefs::NOTUNNEL);
            m_output->flush();
         } else {
            m_plogwriter->error("Viewer support only default tunneling tight-authentication");
            throw ::subsystem::Exception("Viewer support only default tunneling tight-authentication");
         }
      }
      m_plogwriter->debug("Tunneling is init");
   }

   int RemoteViewerCore::initAuthentication()
   {
      m_plogwriter->debug("Initialization of tight-authentication...");
      unsigned int authTypesNumber = m_input->readUInt32();

      m_plogwriter->information("Number of auth-types is {}", authTypesNumber);
      if (authTypesNumber == 0) {
         return ::remoting::AuthDefs::NONE;
      }

      m_plogwriter->debug("Reading authentication capability...");
      for (unsigned int i = 0; i < authTypesNumber; i++) {
         RfbCapabilityInfo cap = readCapability();
         m_authCaps.enable(&cap);
      }
      size_t numEnabled = m_authCaps.numEnabled();
      ::array_base<unsigned int> authTypes(numEnabled);
      for (size_t i = 0; i < numEnabled; ++i) {
         authTypes[i] = m_authCaps.getByOrder(i);
      }

      m_plogwriter->debug("Authentication capability is read");

      m_plogwriter->debug("Select authentication capability");
      if (numEnabled == 0) {
         throw ::subsystem::Exception("No security types supported. "
                         "Server sent security types, but we do not support any of their.");
      }
      int typeSelected = authTypes[0];
      m_plogwriter->debug("Selected type of authentication: {}", typeSelected);

      m_output->writeUInt32(typeSelected);
      m_output->flush();

      m_plogwriter->debug("Tight authentication is init");
      return typeSelected;
   }

   void RemoteViewerCore::setFbProperties(const ::int_size & sizeFramebuffer,
                                          const ::innate_subsystem::PixelFormat & pixelformatFramebuffer)
   {
#ifdef _DEMO_VERSION_
      m_pwatermarkscontroller.setNewFbProperties(&sizeFramebuffer->getRect(), pixelformatFramebuffer);
#endif

      const ::innate_subsystem::PixelFormat &pixelformat = pixelformatFramebuffer;
      ::string pxString;
      pxString.formatf("[bits-per-pixel: {}, depth: {}, big-endian-flag: {}, "
                      "true-color-flag: is set, " // true color always is set
                      "red-max: {}, green-max: {}, blue-max: {}, "
                      "red-shift: {}, green-shift: {}, blue-shift: {}]",
                      pixelformat.bitsPerPixel, pixelformat.colorDepth, pixelformat.bigEndian,
                      pixelformat.redMax, pixelformat.greenMax, pixelformat.blueMax,
                      pixelformat.redShift, pixelformat.greenShift, pixelformat.blueShift);

      m_plogwriter->debug("Setting frame buffer properties...");
      m_plogwriter->information("Frame buffer dimension: ({}, {})",
                       sizeFramebuffer.cx, sizeFramebuffer.cy);
      m_plogwriter->information("Frame buffer pixel format: {}", pxString);

      if (!m_pframebuffer->setProperties(sizeFramebuffer, pixelformatFramebuffer) ||
          !m_pframebufferRectangle->setProperties(sizeFramebuffer, pixelformatFramebuffer)) {
         ::string error;
         error.formatf("Failed to set property frame buffer. "
                      "::int_size: ({}, {}), Pixel format: {}",
                      sizeFramebuffer.cx, sizeFramebuffer.cy,
                      pxString);
         throw ::subsystem::Exception(error);
          }
      m_pframebufferRectangle->setColor(0, 0, 0);
      m_pframebuffer->setColor(0, 0, 0);
      refreshFramebuffer();
      m_pfbupdatenotifier->onPropertiesFb();
      m_plogwriter->debug("Frame buffer properties set");
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
         // connect to host and create ::remoting::RfbInputGate/::remoting::RfbOutputGate
         // if already connected, then function do nothing
         m_plogwriter->information("Protocol stage is \"Connection establishing\".");
         connectToHost();

         // get server version and set client version
         m_plogwriter->information("Protocol stage is \"Handshake\".");
         handshake();

         // negotiaty about security type and authenticate
         m_plogwriter->information("Protocol stage is \"Authentication\".");
         try {
            m_pcoreeventsadapter->onConnecting(1);
         } catch (const ::subsystem::Exception &ex) {
            m_plogwriter->error("Error in CoreEventsAdapter::onConnecting(): {}", ex.get_message());
         } catch (...) {
            m_plogwriter->error("Unknown error in CoreEventsAdapter::onConnecting()");
         }

         authenticate();

         // set shared flag, get server dimension, pixel format and hostname
         // send client pixel format and set him.
         m_plogwriter->information("Protocol stage is \"Initialization\".");
         clientAndServerInit();

         // is connected
         m_plogwriter->information("Protocol stage is \"Is connected\".");

         try {
            m_pcoreeventsadapter->onConnected(m_output);
         } catch (const ::subsystem::Exception &ex) {
            m_plogwriter->error("Error in CoreEventsAdapter::onConnected(): {}", ex.get_message());
         } catch (...) {
            m_plogwriter->error("Unknown error in CoreEventsAdapter::onConnected()");
         }

         {
            critical_section_lock al(&m_connectLock);
            m_wasConnected = true;
         }

         // send supporting encoding
         m_plogwriter->information("Protocol stage is \"Encoding select\".");
         sendEncodings();

         // send ENABLE_CUT_TEXT_UTF8 if server has the capability
         allowUtf8Clipboard();

         // send request of frame buffer update
         m_plogwriter->information("Protocol stage is \"Working phase\".");
         sendFbUpdateRequest(false);

         // received server messages
         while (!isTerminating()) {
            unsigned int msgType = receiveServerMessageType();

            switch (msgType) {
               case ::remoting::ServerMsgDefs::FB_UPDATE:
                  m_plogwriter->debug("Received scopedstrMessage: FB_UPDATE");
                  receiveFbUpdate();
                  break;

               case ::remoting::ServerMsgDefs::SET_COLOR_MAP_ENTRIES:
                  m_plogwriter->debug("Received scopedstrMessage: SET_COLOR_MAP_ENTRIES");
                  receiveSetColorMapEntries();
                  break;

               case ::remoting::ServerMsgDefs::BELL:
                  m_plogwriter->debug("Received scopedstrMessage: BELL");
                  receiveBell();
                  break;

               case ::remoting::ServerMsgDefs::SERVER_CUT_TEXT:
                  m_plogwriter->debug("Received scopedstrMessage: SERVER_CUT_TEXT");
                  receiveServerCutText();
                  break;
               case ::remoting::ServerMsgDefs::SERVER_CUT_TEXT_UTF8:
                  m_plogwriter->debug("Received scopedstrMessage: SERVER_CUT_TEXT_UTF8");
                  receiveServerCutTextUtf8();
                  break;

               default:
                  if (m_serverMsgHandlers.find(msgType) != m_serverMsgHandlers.end()) {
                     m_plogwriter->debug("Received scopedstrMessage ({}) transmit to capability handler", msgType);
                     try {
                        m_serverMsgHandlers[msgType]->onServerMessage(msgType, m_input);
                     } catch (const ::subsystem::Exception &ex) {
                        m_plogwriter->error("Error in onServerMessage(): {}", ex.get_message());
                     } catch (...) {
                        m_plogwriter->error("Unknown error in onServerMessage()");
                     }
                  } else {
                     m_plogwriter->error("Server to client scopedstrMessage: {} is not supported", msgType);
                  }
            }
         }
         ::string scopedstrMessage("Remote viewer's core thread terminated");
         try {
            m_pcoreeventsadapter->onDisconnect(scopedstrMessage);
         } catch (const ::subsystem::Exception &ex) {
            m_plogwriter->error("Error in CoreEventsAdapter::onDisconnect(): {}", ex.get_message());
         } catch (...) {
            m_plogwriter->error("Unknown error in CoreEventsAdapter::onDisconnect()");
         }

      } catch (const AuthException &ex) {
         m_plogwriter->error("RemoteVewerCore. Auth exception: {}", ex.get_message());
         try {
            m_pcoreeventsadapter->onAuthError(&ex);
         } catch (const ::subsystem::Exception &ex) {
            m_plogwriter->error("Error in CoreEventsAdapter::onAuthError(): {}", ex.get_message());
         } catch (...) {
            m_plogwriter->error("Unknown error in CoreEventsAdapter::onAuthError()");
         }
      } catch (const ::io_exception &ex) {
         try {
            ::string disconnectMessage(ex.get_message());
            m_pcoreeventsadapter->onDisconnect(disconnectMessage);
         } catch (const ::subsystem::Exception &ex) {
            m_plogwriter->error("Error in CoreEventsAdapter::onDisconnect(): {}", ex.get_message());
         } catch (...) {
            m_plogwriter->error("Unknown error in CoreEventsAdapter::onDisconnect()");
         }
      } catch (const ::subsystem::Exception &ex) {
         m_plogwriter->error("RemoteViewerCore. ::subsystem::Exception: {}", ex.get_message());
         try {
            m_pcoreeventsadapter->onError(&ex);
         } catch (...) {
            m_plogwriter->error("Unknown error in CoreEventsAdapter::onError()");
         }
      } catch (...) {
         ::string error;
         error.formatf("RemoteViewerCore. Unknown exception");
         m_plogwriter->error("{}", error);
         ::subsystem::Exception ex(error);
         try {
            m_pcoreeventsadapter->onError(&ex);
         } catch (...) {
            m_plogwriter->error("Unknown error in CoreEventsAdapter::onError()");
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
      m_plogwriter->debug("number of rectangles: {}", numberOfRectangles);

      bool isLastRect = false;
      for (int rectangle = 0; rectangle < numberOfRectangles && !isLastRect; rectangle++) {
         m_plogwriter->debug("Receiving rectangle #{}...", rectangle);
         isLastRect = receiveFbUpdateRectangle();
      }

      {
         critical_section_lock al(&m_criticalsectionRequestUpdate);
         m_bNeedRequestUpdate = true;
      }
      {
         critical_section_lock al(&m_criticalsectionFreeze);
         if (m_bFreeze)
            return;
      }
      m_plogwriter->debug("Sending of frame buffer update request...");
      sendFbUpdateRequest(!m_forceFullUpdate);
   }

   bool RemoteViewerCore::receiveFbUpdateRectangle()
   {
      ::int_rectangle rectangle;
      rectangle.left = m_input->readUInt16();
      rectangle.top = m_input->readUInt16();
      rectangle.set_width(m_input->readUInt16());
      rectangle.set_height(m_input->readUInt16());

      int encodingType = m_input->readInt32();

      m_plogwriter->debug("Rectangle: ({}, {}), ({}, {}). Type is {}",
                        rectangle.left, rectangle.top, rectangle.right, rectangle.bottom, encodingType);

      if (encodingType == ::remoting::PseudoEncDefs::LAST_RECT)
         return true;
      if (!Decoder::isPseudo(encodingType)) {
         if (::int_rectangle(m_pframebuffer->getDimension()).intersection(rectangle) != rectangle) {
            throw ::subsystem::Exception("Error in protocol: incorrect size of rectangle");
         }

         Decoder *decoder = m_decoderStore.getDecoder(encodingType);
         if (decoder != 0) {
            m_plogwriter->debug("Decoding...");

            DecoderOfRectangle *rectangleDecoder = dynamic_cast<DecoderOfRectangle *>(decoder);
            rectangleDecoder->process(m_input,
                                      m_pframebuffer, m_pframebufferRectangle, rectangle, &m_criticalsectionFramebuffer,
                                      m_pfbupdatenotifier);

            m_plogwriter->debug("Decoded");
         } else { // decoder is 0
            ::string errorString;
            errorString.formatf("Decoder \"{}\" isn't exist", encodingType);
            m_plogwriter->error("{}", errorString);
            throw ::subsystem::Exception(errorString);
         }
      } else { // it's pseudo encoding
         m_plogwriter->debug("It's pseudo encoding");
         processPseudoEncoding(rectangle, encodingType);
      }
      return false;
   }

   void RemoteViewerCore::processPseudoEncoding(const ::int_rectangle &  rectangle,
                                                int encodingType)
   {
      switch (encodingType) {
         case ::remoting::PseudoEncDefs::DESKTOP_SIZE:
            m_plogwriter->information("Changed size of desktop");
         {
            critical_section_lock al(&m_criticalsectionFramebuffer);
            setFbProperties(rectangle.size(), m_pframebuffer->getPixelFormat());
         }
            break;

         case ::remoting::PseudoEncDefs::RICH_CURSOR:
         {
            m_plogwriter->debug("New rich cursor");

            unsigned short width = rectangle.width();
            unsigned short height = rectangle.height();
            unsigned char bytesPerPixel = m_pframebuffer->getBytesPerPixel();

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
            ::int_point pointHotspot(rectangle.left, rectangle.top);

            m_plogwriter->debug("Setting new rich cursor...");
            m_pfbupdatenotifier->setNewCursor(pointHotspot, width, height,
                                            &cursor, &bitmask);
         }
            break;

         case ::remoting::PseudoEncDefs::POINTER_POS:
         {
            m_plogwriter->debug("Updating pointer pointPosition: [{}, {}]", rectangle.left, rectangle.top);
            ::int_point pointPosition(rectangle.left, rectangle.top);
            m_pfbupdatenotifier->updatePointerPos(pointPosition);
         }
            break;

         default:
            ::string errorString;
            errorString.formatf("Pseudo encoding {} is not supported", encodingType);
            m_plogwriter->error("{}", errorString);
            throw ::subsystem::Exception(errorString);
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

      m_plogwriter->information("Bell!");
      try {
         m_pcoreeventsadapter->onBell();
      } catch (const ::subsystem::Exception &ex) {
         m_plogwriter->error("Error in CoreEventsAdapter::onBell(): {}", ex.get_message());
      } catch (...) {
         m_plogwriter->error("Unknown error in CoreEventsAdapter::onBell()");
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

      m_plogwriter->debug("Cut text: {}", cutText);
      try {
         m_pcoreeventsadapter->onCutText(cutText);
      } catch (const ::subsystem::Exception &ex) {
         m_plogwriter->error("Error in CoreEventsAdapter::onCutText(): {}", ex.get_message());
      } catch (...) {
         m_plogwriter->error("Unknown error in CoreEventsAdapter::onCutText()");
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

      m_plogwriter->debug("Cut text: {}", cutText);
      try {
         m_pcoreeventsadapter->onCutText(cutText);
      }
      catch (const ::subsystem::Exception &ex) {
         m_plogwriter->error("Error in CoreEventsAdapter::onCutText(): {}", ex.get_message());
      }
      catch (...) {
         m_plogwriter->error("Unknown error in CoreEventsAdapter::onCutText()");
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

      m_plogwriter->information("Server sent protocol version: {}", getProtocolString());
      if (!isRfbProtocolString(serverProtocol) ||
          m_major < 3 ||
          (m_major == 3 && m_minor < 3)) {
         ::string error;
         ::string protocolAnsi(serverProtocol);
         ::string protocol;
         protocol = protocolAnsi;
         error.format("Unsupported protocol: {}", protocol);
         m_plogwriter->error("{}", error);
         throw ::subsystem::Exception(error);
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

      m_plogwriter->information("Send to server protocol version: {}", getProtocolString());

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
    * 2           - U16         - pframebuffer-width
    * 2           - U16         - pframebuffer-height
    * 16          - ::innate_subsystem::PixelFormat - server-pixel-format
    * 4           - U32         - name-length
    * name-length - U8 array    - name-string
    */
   void RemoteViewerCore::clientAndServerInit()
   {
      if (m_bShared) {
         m_plogwriter->information("Setting share flag in on...");
      } else {
         m_plogwriter->information("Setting share flag is off...");
      }
      m_output->writeUInt8(m_bShared);
      m_output->flush();
      m_plogwriter->debug("Shared flag is set");

      unsigned short width = m_input->readUInt16();
      unsigned short height = m_input->readUInt16();
      ::int_size screenDimension(width, height);
      ::innate_subsystem::PixelFormat serverPixelFormat = readPixelFormat();

      {
         critical_section_lock al(&m_criticalsectionFramebuffer);
         setFbProperties(screenDimension, serverPixelFormat);
      }

      unsigned int sizeInBytes = m_input->readUInt32();
      ::array_base<char> buffer(sizeInBytes + 1);
      m_input->read(buffer.data(), sizeInBytes);
      buffer[sizeInBytes] = '\0';
      ::string ansiStr;
      ansiStr= buffer.data();
      m_remoteDesktopName = ansiStr;
      m_plogwriter->information("Server remote name: {}", m_remoteDesktopName);

      if (m_isTight) {
         m_plogwriter->debug("Reading tight pcapabilitiesmanager");
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

      m_plogwriter->debug("Server scopedstrMessage types (capability):");
      while (nServerMessageTypes--) {
         RfbCapabilityInfo cap = readCapability();
         m_serverMsgCaps.enable(&cap);
      }

      m_plogwriter->debug("Client scopedstrMessage types (capability):");
      while (nClientMessageTypes--) {
         RfbCapabilityInfo cap = readCapability();
         m_clientMsgCaps.enable(&cap);
      }

      m_plogwriter->debug("Encodings (capability):");
      while (nEncodingTypes--) {
         RfbCapabilityInfo cap = readCapability();
         m_encodingCaps.enable(&cap);
      }

      // Add active encoding-pcapabilitiesmanager in DecoderStore.
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

      m_plogwriter->debug("code: {}, vendor: {}, signature: {}",
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

   ::int_rectangle_array_base RemoteViewerCore::getDesktops()
   {
      return m_desktops;
   }

   ::int_size RemoteViewerCore::getDesktopSize()
   {
      return m_desktopSize;
   }
} // namespace remoting_client