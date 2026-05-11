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
#include "UpdateSender.h"
#include "remoting/remoting/rfb/VendorDefs.h"
#include "remoting/remoting/rfb/EncodingDefs.h"
#include "remoting/remoting/rfb/MsgDefs.h"
//#include aaa_<vector>
//#include aaa_<algorithm>
//#include "subsystem/platform/inttypes.h"
#include "subsystem/platform/Exception.h"
#include "UpdSenderMsgDefs.h"
#include "remoting/remoting/rfb_sconn/ClipboardExchange.h"
#include "acme/platform/ProfileLogger.h"
namespace remoting
{


   // UpdateSender::UpdateSender(RfbCodeRegistrator *m_prfbcoderegistrator, UpdateRequestListener *updReqListener,
   //                            SenderControlInformationInterface *senderControlInformation, ::remoting::RfbOutputGate *output,
   //                            int id, Desktop *desktop, ::subsystem::LogWriter * plogwriter) :
   //     m_pupdaterequestlistener(updReqListener), m_pdesktop(desktop), m_senderControlInformation(senderControlInformation),
   //     m_busy(false), m_incrUpdIsReq(false), m_fullUpdIsReq(false), m_setColorMapEntr(false), m_prfboutputgate(output),
   //     m_enbox(&m_ppixelconverter, m_prfboutputgate), m_id(id), m_videoFrozen(false), m_shareOnlyApp(false), m_plogwriter = plogwriter;,
   //     m_pcursorupdates(plogwriter)
   // {
   //    // FIXME: argument must be defined
   //    m_pupdatekeeper = new UpdateKeeper(::int_rectangle());
   //
   //    // Capabilities
   //    m_prfbcoderegistrator->addEncCap(EncodingDefs::COPYRECT, VendorDefs::STANDARD, EncodingDefs::SIG_COPYRECT);
   //    m_prfbcoderegistrator->addEncCap(EncodingDefs::HEXTILE, VendorDefs::STANDARD, EncodingDefs::SIG_HEXTILE);
   //    m_prfbcoderegistrator->addEncCap(EncodingDefs::TIGHT, VendorDefs::TIGHTVNC, EncodingDefs::SIG_TIGHT);
   //    m_prfbcoderegistrator->addEncCap(PseudoEncDefs::COMPR_LEVEL_0, VendorDefs::TIGHTVNC, PseudoEncDefs::SIG_COMPR_LEVEL);
   //    m_prfbcoderegistrator->addEncCap(PseudoEncDefs::QUALITY_LEVEL_0, VendorDefs::TIGHTVNC, PseudoEncDefs::SIG_QUALITY_LEVEL);
   //    m_prfbcoderegistrator->addEncCap(PseudoEncDefs::RICH_CURSOR, VendorDefs::TIGHTVNC, PseudoEncDefs::SIG_RICH_CURSOR);
   //    m_prfbcoderegistrator->addEncCap(PseudoEncDefs::POINTER_POS, VendorDefs::TIGHTVNC, PseudoEncDefs::SIG_POINTER_POS);
   //    m_prfbcoderegistrator->addEncCap(PseudoEncDefs::DESKTOP_SIZE, VendorDefs::TIGHTVNC, PseudoEncDefs::SIG_DESKTOP_SIZE);
   //    m_prfbcoderegistrator->addEncCap(PseudoEncDefs::DESKTOP_CONFIGURATION, VendorDefs::TIGHTVNC,
   //                          PseudoEncDefs::SIG_DESKTOP_CONFIGURATION);
   //
   //    m_prfbcoderegistrator->addClToSrvCap(UpdSenderClientMsgDefs::RFB_VIDEO_FREEZE, VendorDefs::TIGHTVNC,
   //                              UpdSenderClientMsgDefs::RFB_VIDEO_FREEZE_SIG);
   //
   //    // Request codes
   //    m_prfbcoderegistrator->regCode(UpdSenderClientMsgDefs::RFB_VIDEO_FREEZE, this);
   //    m_prfbcoderegistrator->regCode(ClientMsgDefs::FB_UPDATE_REQUEST, this);
   //    m_prfbcoderegistrator->regCode(ClientMsgDefs::SET_PIXEL_FORMAT, this);
   //    m_prfbcoderegistrator->regCode(ClientMsgDefs::SET_ENCODINGS, this);
   //
   //    resume();
   // }
UpdateSender::UpdateSender() :
       m_pupdaterequestlistener(nullptr), m_pdesktop(nullptr), m_senderControlInformation(nullptr),
       m_busy(false), m_incrUpdIsReq(false), m_fullUpdIsReq(false), m_setColorMapEntr(false),// m_prfboutputgate(nullptr),
       //m_enbox(&m_ppixelconverter, m_prfboutputgate), m_id(0), m_videoFrozen(false), m_shareOnlyApp(false)
   m_id(0), m_videoFrozen(false), m_shareOnlyApp(false)
       //m_pcursorupdates(plogwriter)
   {

   }
   UpdateSender::~UpdateSender()
   {
      terminate();
      wait();
      delete m_pupdatekeeper;

   }


    void  UpdateSender::initialize_update_sender(RfbCodeRegistrator *m_prfbcoderegistrator, UpdateRequestListener *updReqListener,
                              SenderControlInformationInterface *senderControlInformation, ::remoting::RfbOutputGate *prfboutputgate,
                              int id, Desktop *desktop, ::subsystem::LogWriter * plogwriter) //:
       // , , ,
       // ,
       // , m_id(id), m_videoFrozen(false), m_shareOnlyApp(false), m_plogwriter = plogwriter;,
       // m_pcursorupdates(plogwriter)
   {
   construct_newø(m_ppixelconverter);
          m_pupdaterequestlistener = updReqListener;
          m_pdesktop = desktop;
          m_senderControlInformation = senderControlInformation;
          m_prfboutputgate = prfboutputgate;
      // FIXME: argument must be defined
          construct_newø(m_pencoderstore);
          m_pencoderstore->initialize_encoder_store(m_ppixelconverter, m_prfboutputgate);
          m_id = id;
          m_plogwriter = plogwriter;
          construct_newø(m_pcursorupdates);
   m_pcursorupdates->initialize_cursor_updates(plogwriter);
      m_pupdatekeeper = new UpdateKeeper(::int_rectangle());

      // Capabilities
      m_prfbcoderegistrator->addEncCap(EncodingDefs::COPYRECT, VendorDefs::STANDARD, EncodingDefs::SIG_COPYRECT);
      m_prfbcoderegistrator->addEncCap(EncodingDefs::HEXTILE, VendorDefs::STANDARD, EncodingDefs::SIG_HEXTILE);
      m_prfbcoderegistrator->addEncCap(EncodingDefs::TIGHT, VendorDefs::TIGHTVNC, EncodingDefs::SIG_TIGHT);
      m_prfbcoderegistrator->addEncCap(PseudoEncDefs::COMPR_LEVEL_0, VendorDefs::TIGHTVNC, PseudoEncDefs::SIG_COMPR_LEVEL);
      m_prfbcoderegistrator->addEncCap(PseudoEncDefs::QUALITY_LEVEL_0, VendorDefs::TIGHTVNC, PseudoEncDefs::SIG_QUALITY_LEVEL);
      m_prfbcoderegistrator->addEncCap(PseudoEncDefs::RICH_CURSOR, VendorDefs::TIGHTVNC, PseudoEncDefs::SIG_RICH_CURSOR);
      m_prfbcoderegistrator->addEncCap(PseudoEncDefs::POINTER_POS, VendorDefs::TIGHTVNC, PseudoEncDefs::SIG_POINTER_POS);
      m_prfbcoderegistrator->addEncCap(PseudoEncDefs::DESKTOP_SIZE, VendorDefs::TIGHTVNC, PseudoEncDefs::SIG_DESKTOP_SIZE);
      m_prfbcoderegistrator->addEncCap(PseudoEncDefs::DESKTOP_CONFIGURATION, VendorDefs::TIGHTVNC,
                            PseudoEncDefs::SIG_DESKTOP_CONFIGURATION);

      m_prfbcoderegistrator->addClToSrvCap(UpdSenderClientMsgDefs::RFB_VIDEO_FREEZE, VendorDefs::TIGHTVNC,
                                UpdSenderClientMsgDefs::RFB_VIDEO_FREEZE_SIG);

      // Request codes
      m_prfbcoderegistrator->regCode(UpdSenderClientMsgDefs::RFB_VIDEO_FREEZE, this);
      m_prfbcoderegistrator->regCode(ClientMsgDefs::FB_UPDATE_REQUEST, this);
      m_prfbcoderegistrator->regCode(ClientMsgDefs::SET_PIXEL_FORMAT, this);
      m_prfbcoderegistrator->regCode(ClientMsgDefs::SET_ENCODINGS, this);

      resume();
   }

   void UpdateSender::onTerminate() { m_newUpdatesEvent.set_happening(); }

   void UpdateSender::onRequest(unsigned int reqCode, ::remoting::RfbInputGate *input)
   {
      // UpdateSender internal dispatcher
      switch (reqCode)
      {
         case ClientMsgDefs::FB_UPDATE_REQUEST:
            readUpdateRequest(input);
            break;
         case ClientMsgDefs::SET_PIXEL_FORMAT:
            readSetPixelFormat(input);
            break;
         case ClientMsgDefs::SET_ENCODINGS:
            readSetEncodings(input);
            break;
         case UpdSenderClientMsgDefs::RFB_VIDEO_FREEZE:
            readVideoFreeze(input);
            break;
         default:
            ::string errMess;
            errMess.format("Unknown {} protocol code received", (int)reqCode);
            throw ::subsystem::Exception(errMess);
            break;
      }
   }

   void UpdateSender::init(const ::int_size &viewPortDimension, const ::innate_subsystem::PixelFormat & pixelformat)
   {
      setClientPixelFormat(pixelformat, false);
      {
         critical_section_lock al(&m_criticalsectionViewport);
         m_sizeClient = viewPortDimension;
      }
      m_sizeLastViewport = viewPortDimension;
      m_pupdatekeeper->setBorderRect(viewPortDimension);
   }

   void UpdateSender::newUpdates(const UpdateContainer & updatecontainer, const CursorShape *cursorShape)
   {
      m_plogwriter->debug("New updates passed to client #{}", m_id);
      addUpdateContainer(updatecontainer);

      m_pcursorupdates->updateCursorShape(cursorShape);
      {
         critical_section_lock al(&m_criticalsectionRectLoc);
         m_busy = true;
         m_newUpdatesEvent.set_happening();
      }
      m_plogwriter->debug("Client #{} is waking up", m_id);
   }

   void UpdateSender::addUpdateContainer(const UpdateContainer & updatecontainer)
   {
      UpdateContainer updatecontainerNew = updatecontainer;

      ::int_rectangle rectangleViewport = getViewPort();

      updatecontainerNew.m_regionVideo-= rectangleViewport.top_left();
      updatecontainerNew.m_regionChanged-= rectangleViewport.top_left();
      updatecontainerNew.m_regionCopied-= rectangleViewport.top_left();
      updatecontainerNew.m_pointCopySource -= rectangleViewport.top_left();

      m_pupdatekeeper->addUpdateContainer(updatecontainerNew);
   }

   void UpdateSender::blockCursorPosSending() { m_pcursorupdates->blockCursorPosSending(); }

   ::int_rectangle UpdateSender::getViewPort()
   {
      critical_section_lock al(&m_criticalsectionViewport);
      return m_rectangleViewport;
   }

   bool UpdateSender::clientIsReady()
   {
      critical_section_lock al(&m_criticalsectionRectLoc);
      return (m_incrUpdIsReq || m_fullUpdIsReq) && !m_busy;
   }

   void UpdateSender::sendRectHeader(const ::int_rectangle & rectangle, int encodingType)
   {
      // FIXME: Why no warnings on passing bigger integer types?
      m_prfboutputgate->writeUInt16(rectangle.left);
      m_prfboutputgate->writeUInt16(rectangle.top);
      m_prfboutputgate->writeUInt16(rectangle.width());
      m_prfboutputgate->writeUInt16(rectangle.height());
      m_prfboutputgate->writeInt32(encodingType);
   }

   void UpdateSender::sendRectHeader(unsigned short x, unsigned short y, unsigned short w, unsigned short h,
                                     int encodingType)
   {
      m_prfboutputgate->writeUInt16(x);
      m_prfboutputgate->writeUInt16(y);
      m_prfboutputgate->writeUInt16(w);
      m_prfboutputgate->writeUInt16(h);
      m_prfboutputgate->writeInt32(encodingType);
   }

   void UpdateSender::sendNewFBSize(::int_size & size, bool extended)
   {
      // Header
      m_prfboutputgate->writeUInt8(ServerMsgDefs::FB_UPDATE); // scopedstrMessage type
      m_prfboutputgate->writeUInt8(0); // padding
      m_prfboutputgate->writeUInt16(1); // one rectangle

      ::int_rectangle r(size);
      if (!extended)
      {
         sendRectHeader(r, PseudoEncDefs::DESKTOP_SIZE);
      }
      else
      {
         ::int_rectangle_array_base screens = m_pdesktop->getDisplaysCoords();
         if (screens.size() > 255)
         {
            screens.resize(255);
         }
         sendRectHeader(r, PseudoEncDefs::DESKTOP_CONFIGURATION);

         m_prfboutputgate->writeUInt8((unsigned char)screens.size()); // number-of-screens
         m_prfboutputgate->writeUInt8(0); // padding
         m_prfboutputgate->writeUInt16(0); // padding

         for (size_t i = 0; i < screens.size(); i++)
         {
            ::int_rectangle rectangle = screens[i];
            m_prfboutputgate->writeUInt16(rectangle.left);
            m_prfboutputgate->writeUInt16(rectangle.top);
            m_prfboutputgate->writeUInt16(rectangle.width());
            m_prfboutputgate->writeUInt16(rectangle.height());
         }
      }
   }

   void UpdateSender::sendFbInClientDim(const EncodeOptions *encodeOptions, const ::innate_subsystem::Framebuffer *pframebuffer,
                                        const ::int_size &size, const ::innate_subsystem::PixelFormat & pixelformat)
   {
      // On the black frame buffer will be overlayed the current pframebuffer->
      // This is needed to combine the server frame buffer with a client frame
      // buffer when the dimensions are not equal.
      ::innate_subsystem::Framebuffer blankFramebuffer;
      blankFramebuffer.setProperties(size, pixelformat);
      blankFramebuffer.setColor(0, 0, 0);
      blankFramebuffer.copyFrom(pframebuffer, 0, 0);

      ::int_rectangle rectangleForRegion(size);
      Region region(rectangleForRegion);
      ::int_rectangle_array_base rectanglea;
      splitRegion(m_pencoderstore->getEncoder(), region, rectanglea, &blankFramebuffer, encodeOptions);

      // Header
      m_prfboutputgate->writeUInt8(0); // scopedstrMessage type
      m_prfboutputgate->writeUInt8(0); // padding
      unsigned short numRects = (unsigned short)rectanglea.size();
      ASSERT(numRects == rectanglea.size());
      m_prfboutputgate->writeUInt16(numRects);
      sendRectangles(m_pencoderstore->getEncoder(), rectanglea, &blankFramebuffer, encodeOptions);
   }

   void UpdateSender::sendCursorShapeUpdate(const ::innate_subsystem::PixelFormat &fmt, const CursorShape *cursorShape)
   {
      // Send pseudo-rectangle.
      ::int_point pointHotspot = cursorShape->getHotSpot();
      ::int_size size = cursorShape->getDimension();
      sendRectHeader(pointHotspot.x, pointHotspot.y, size.cx, size.cy, PseudoEncDefs::RICH_CURSOR);

      ::innate_subsystem::Framebuffer fbConverted;
      fbConverted.setProperties(size, fmt);
      ::int_rectangle rectangleFromDimension(size);
      m_ppixelconverter->convert(rectangleFromDimension, &fbConverted, cursorShape->getPixels());

      if (fbConverted.getBufferSize())
      {
         m_prfboutputgate->write(fbConverted.getBuffer(), fbConverted.getBufferSize());
      }
      if (cursorShape->getMaskSize())
      {
         m_prfboutputgate->write(cursorShape->getMask(), cursorShape->getMaskSize());
      }
   }

   void UpdateSender::sendCursorPosUpdate()
   {
      ::int_point pos = m_pcursorupdates->getCurPos();
      m_plogwriter->debug("Sending cursor pointPosition update: ({},{})", pos.x, pos.y);
      sendRectHeader(pos.x, pos.y, 0, 0, PseudoEncDefs::POINTER_POS);
   }


   void UpdateSender::sendCopyRect(const ::int_rectangle_array_base & rectanglea, const ::int_point & pointSource)
   {

      for (auto rectangle : rectanglea)
      {

         sendRectHeader(rectangle, EncodingDefs::COPYRECT);

         // Send rectangleCopy data
         // FIXME: Each dest rectangle should have own source point
         m_prfboutputgate->writeUInt16(pointSource.x);
         m_prfboutputgate->writeUInt16(pointSource.y);

      }

   }


   void UpdateSender::sendPalette(::innate_subsystem::PixelFormat & pixelformat)
   {
      m_prfboutputgate->writeUInt8(1); // type
      m_prfboutputgate->writeUInt8(0); // pad
      m_prfboutputgate->writeUInt16(0); // first color
      m_prfboutputgate->writeUInt16(256); // number of colors
      for (unsigned int i = 0; i < 256; i++)
      {
         m_prfboutputgate->writeUInt16(((i >> pixelformat.redShift) & pixelformat.redMax) * 65535 / pixelformat.redMax); // red
         m_prfboutputgate->writeUInt16(((i >> pixelformat.greenShift) & pixelformat.greenMax) * 65535 / pixelformat.greenMax); // green
         m_prfboutputgate->writeUInt16(((i >> pixelformat.blueShift) & pixelformat.blueMax) * 65535 / pixelformat.blueMax); // blue
      }
   }

   void UpdateSender::sendUpdate()
   {
      m_plogwriter->debug("Entered to the sendUpdate() function");

      //  m_plogwriter->checkPoint("1 sendUpdate() begins");

      // Check requested regions and immediately return if the client did not
      // request anything.
      Region regionRequestedFull, regionRequestedIncremental;
      bool bIncrementalUpdateRequest, bFullUpdateRequest;
      class ::time timeReqPoint;
      if (!extractReqRegions(regionRequestedIncremental, regionRequestedFull, &bIncrementalUpdateRequest, &bFullUpdateRequest, &timeReqPoint))
      {
         m_plogwriter->debug("No request, exiting from the sendUpdate()");
         return;
      }
      m_plogwriter->debug("Time between request and a point after extractReqRegions (in milliseconds): %u",
                          (unsigned int)timeReqPoint.elapsed().integral_millisecond());
      m_plogwriter->debug("A request has been made, continuing");
      m_plogwriter->debug("The incremental region has {} rectangles", (int)regionRequestedIncremental.getCount());
      m_plogwriter->debug("The full region has {} rectangles", (int)regionRequestedFull.getCount());

      //UpdateContainer updatecontainer;
      auto updatecontainer = ::transfer(extractUpdates());

      EncodeOptions encodeOptions;
      selectEncoder(encodeOptions);
      EncodeOptions losslessEncodeOptions;
      bool losslessEnabled = encodeOptions.jpegEnabled();
      if (losslessEnabled)
      {
         selectEncoder(losslessEncodeOptions);
         losslessEncodeOptions.disableJpeg();
      }

      // Viewport calculating
      ::int_rectangle rectangleViewport;
      bool shareOnlyApp;
      Region regionOldShareApp;
      Region regionShareApp;
      bool viewPortChanged = updateViewPort(rectangleViewport, &shareOnlyApp, regionOldShareApp, regionShareApp);

      updateFramebuffer(updatecontainer, shareOnlyApp, regionOldShareApp, regionShareApp);
      auto pframebuffer = m_pframebuffer;

      critical_section_lock l(m_prfboutputgate);

      ::int_size sizeClient, sizeLastViewport;
      {
         critical_section_lock al(&m_criticalsectionViewport);
         sizeClient = m_sizeClient;
         sizeLastViewport = m_sizeLastViewport;
      }

      // If client does not support the desktop resizing then view port dimension
      // must be no more than client dimension.
      if (!encodeOptions.desktopSizeEnabled() && !encodeOptions.desktopConfigurationEnabled())
      {
         ::int_rectangle clientRect = sizeClient;
         clientRect.set_top_left(rectangleViewport.left, rectangleViewport.top);
         rectangleViewport = rectangleViewport.intersection(clientRect);
      }

      // Checking for screen size changing
      bool dimensionChanged = sizeLastViewport != rectangleViewport.size();
      if (dimensionChanged)
      {
         updatecontainer.m_bScreenSizeChanged = true;
      }
      if (dimensionChanged || viewPortChanged)
      {
         updatecontainer.m_regionCopied.clear();

         critical_section_lock al(&m_criticalsectionViewport);
         m_sizeLastViewport = rectangleViewport.size();
         sizeLastViewport = m_sizeLastViewport;
         if (encodeOptions.desktopSizeEnabled() || encodeOptions.desktopConfigurationEnabled())
         {
            m_sizeClient = m_sizeLastViewport;
            sizeClient = m_sizeClient;
         }
         m_pupdatekeeper->setBorderRect(sizeLastViewport);
         updatecontainer.m_regionChanged.crop(sizeLastViewport);
         // Dazzle m_regionChanged
         updatecontainer.m_regionChanged.addRect(sizeLastViewport);
         m_pupdatekeeper->dazzleChangedReg();
         m_pupdatekeeper->setCursorPosChanged();
         m_pupdatekeeper->setCursorShapeChanged();
      }

      // Update pixel converter for effective pixel formats. We must do this
      // before using encoders.
      const ::innate_subsystem::PixelFormat serverPixelFormat = pframebuffer->getPixelFormat();
      bool setColorMapEntr;
      ::innate_subsystem::PixelFormat pixelformatClient;
      {
         critical_section_lock lock(&m_criticalsectionNewPixelFormat);
         pixelformatClient = m_pixelformatNew;
         setColorMapEntr = m_setColorMapEntr;
         m_setColorMapEntr = false;
      }
      if (setColorMapEntr)
      {
         sendPalette(pixelformatClient);
      }
      m_ppixelconverter->setPixelFormats(pixelformatClient, serverPixelFormat);

      // Send updates
      if (updatecontainer.m_bScreenSizeChanged || (!regionRequestedFull.is_empty() && !encodeOptions.desktopSizeEnabled() &&
                                        !encodeOptions.desktopConfigurationEnabled()))
      {
         m_plogwriter->debug("Screen size changed or full region requested");
         if (encodeOptions.desktopSizeEnabled() || encodeOptions.desktopConfigurationEnabled())
         {
            m_plogwriter->debug("Desktop resize is enabled, sending NewFBSize %dx{}", sizeLastViewport.cx,
                                sizeLastViewport.cy);
            sendNewFBSize(sizeLastViewport, encodeOptions.desktopConfigurationEnabled());
         }
         else
         {
            m_plogwriter->debug("Desktop resize is disabled, sending blank screen");
            sendFbInClientDim(&encodeOptions, pframebuffer, sizeClient, pframebuffer->getPixelFormat());
         }
         // FIXME: "Dazzle" does not seem like a good word here.
         m_plogwriter->debug("Dazzle changed region");
         m_pupdatekeeper->dazzleChangedReg();
         m_pupdatekeeper->setCursorPosChanged();
      }
      else
      {
         m_plogwriter->debug("Processing normal updates");
         CursorShape cursorShape;
         m_pcursorupdates->update(&encodeOptions, updatecontainer, !regionRequestedFull.is_empty(), rectangleViewport, shareOnlyApp,
                                regionShareApp, pframebuffer, &cursorShape);

         if (!encodeOptions.copyRectEnabled() || getVideoFrozen())
         {
            m_plogwriter->debug("CopyRect is disabled, converting to normal updates");
            updatecontainer.m_regionChanged.add(updatecontainer.m_regionCopied);
            updatecontainer.m_regionCopied.clear();
         }

         updatecontainer.m_regionChanged.add(m_regionOldVideoRegion); // This line updates rid video places when
                                                        // video is frozen.
         updatecontainer.m_regionVideo.subtract(regionRequestedFull);
         updatecontainer.m_regionChanged.subtract(updatecontainer.m_regionVideo);
         m_regionOldVideoRegion = updatecontainer.m_regionVideo;
         if (getVideoFrozen())
         {
            updatecontainer.m_regionVideo.clear();
         }
         updatecontainer.m_regionChanged.add(regionRequestedFull);

         // FIXME: Are these two lines really needed? Check that carefully.
         ::int_rectangle framebufferRect = pframebuffer->getDimension();
         updatecontainer.m_regionVideo.crop(framebufferRect);
         updatecontainer.m_regionChanged.crop(framebufferRect);
         regionShareApp.crop(framebufferRect);
         regionOldShareApp.crop(framebufferRect);
         m_regionLosslessClean.crop(framebufferRect);

         // If Tight encoding is not supported by the client, convert video updates
         // to normal updates so that the preferred encoding will be used.
         if (!encodeOptions.encodingEnabled(EncodingDefs::TIGHT))
         {
            updatecontainer.m_regionChanged.add(updatecontainer.m_regionVideo);
            updatecontainer.m_regionVideo.clear();
         }

         // Crop changed and video region by requested regions.
         cropUpdContForReqRegions(updatecontainer, regionRequestedIncremental, regionRequestedFull);

         Region m_regionVideo = updatecontainer.m_regionVideo;
         Region m_regionChanged = updatecontainer.m_regionChanged;

         if (shareOnlyApp)
         {
            Region newOpeningAppRegion = regionShareApp;
            newOpeningAppRegion.subtract(regionOldShareApp);

            Region regionBlack = m_regionChanged;
            regionBlack.add(m_regionVideo);
            regionBlack.add(regionOldShareApp);
            regionBlack.subtract(regionShareApp);
            m_regionChanged.add(regionBlack);
            m_regionChanged.add(newOpeningAppRegion);
            // Paint black in the pframebuffer for the black region.
            paintBlack(pframebuffer, regionBlack);
         }

         if (losslessEnabled)
         {
            m_regionLosslessDirty.add(m_regionChanged);
            m_regionLosslessClean.subtract(m_regionChanged);
         }
         // Add some lossless data to every update but no more than 1/100 part of pframebuffer->
         // So at 20 fps full screen will be sent in 5 sec.
         int screenArea = framebufferRect.area();
         Region regionLossless = takePartFromRegion(m_regionLosslessClean, screenArea / 100);
         if (losslessEnabled)
         {
            if (regionLossless.is_empty())
            {
               m_regionLosslessClean.set(m_regionLosslessDirty);
               m_regionLosslessDirty.clear();
            }
         }

         //
         // At this point, we've got final regions in m_regionChanged and m_regionVideo.
         //

         // Convert m_regionChanged to the final ::list_base of rectangles.
         m_plogwriter->debug("Number of normal rectangles before splitting: {}", m_regionChanged.getCount());
         ::int_rectangle_array_base rectangleaNormal;
         splitRegion(m_pencoderstore->getEncoder(), m_regionChanged, rectangleaNormal, pframebuffer, &encodeOptions);

         // Convert regionLossless to the final ::list_base of rectangles.
         ::int_rectangle_array_base rectangleaLossless;
         if (losslessEnabled && !regionLossless.is_empty())
         {
            m_plogwriter->debug("Number of lossless rectangles before splitting: {}", regionLossless.getCount());
            splitRegion(m_pencoderstore->getEncoder(), regionLossless, rectangleaLossless, pframebuffer, &losslessEncodeOptions);
         }
         // Do the same for the m_regionVideo.
         ::int_rectangle_array_base rectangleaVideo;
         if (!m_regionVideo.is_empty())
         {
            m_plogwriter->debug("Video region is not empty");
            m_pencoderstore->validateJpegEncoder(); // make sure JpegEncoder is allocated
            splitRegion(m_pencoderstore->getJpegEncoder(), m_regionVideo, rectangleaVideo, pframebuffer, &encodeOptions);
         }

         // Get the final ::list_base of CopyRect rectangles.
         //::int_rectangle_array_base rectangleaCopy;
         auto rectangleaCopy = updatecontainer.m_regionCopied.getRects();

         m_plogwriter->debug("Number of normal rectangles: {}", rectangleaNormal.size());
         m_plogwriter->debug("Number of lossless rectangles: {}", rectangleaLossless.size());
         m_plogwriter->debug("Number of video rectangles: {}", rectangleaVideo.size());
         m_plogwriter->debug("Number of CopyRect rectangles: {}", rectangleaCopy.size());

         // calculate regions areas
         if (m_plogwriter->is_debug_trace_level())
         {
            m_plogwriter->debug("Area of normal rectangles: {}", calcAreas(rectangleaNormal));
            m_plogwriter->debug("Area of lossless rectangles: {}", calcAreas(rectangleaLossless));
            m_plogwriter->debug("Area of video rectangles: {}", calcAreas(rectangleaVideo));
            m_plogwriter->debug("Area of CopyRect rectangles: {}", calcAreas(rectangleaCopy));
         }

         // Calculate the total number of rectangles and pseudo-rectangles.
         size_t numTotalRects = rectangleaNormal.size() + rectangleaLossless.size() + rectangleaVideo.size() + rectangleaCopy.size();

         if (updatecontainer.m_bCursorPosChanged)
         {
            numTotalRects++;
            m_plogwriter->debug("Adding a pseudo-rectangle for cursor pointPosition update");
         }
         if (updatecontainer.m_bCursorShapeChanged)
         {
            numTotalRects++;
            m_plogwriter->debug("Adding a pseudo-rectangle for cursor shape update");
         }
         m_plogwriter->debug("Total number of rectangles and pseudo-rectangles: {}", numTotalRects);

         // FIXME: Handle this better, e.g. send first 65534 rectangles.
         ASSERT(numTotalRects <= 65534);

         if (numTotalRects != 0)
         {
            m_plogwriter->debug("Sending FramebufferUpdate scopedstrMessage header");
            m_prfboutputgate->writeUInt8(ServerMsgDefs::FB_UPDATE); // scopedstrMessage type
            m_prfboutputgate->writeUInt8(0); // padding
            m_prfboutputgate->writeUInt16((unsigned short)numTotalRects);

            if (updatecontainer.m_bCursorPosChanged)
            {
               sendCursorPosUpdate();
            }
            if (updatecontainer.m_bCursorShapeChanged)
            {
               m_plogwriter->debug("Sending cursor shape update");
               sendCursorShapeUpdate(pixelformatClient, &cursorShape);
            }
            if (rectangleaCopy.size() > 0)
            {
               m_plogwriter->debug("Sending CopyRect rectangles");
               sendCopyRect(rectangleaCopy, updatecontainer.m_pointCopySource);
            }

            m_plogwriter->debug("Time between request and a point before send and coding (in milliseconds): %u",
                                (unsigned int) timeReqPoint.elapsed().integral_millisecond());
            m_plogwriter->debug("Sending video rectangles");
            sendRectangles(m_pencoderstore->getJpegEncoder(), rectangleaVideo, pframebuffer, &encodeOptions);
            m_plogwriter->debug("Sending normal rectangles");
            double area = rectangleaNormal.total_area() / 1'000'000.; // in millions of pixels
            auto processortimes1 = ProfileLogger().checkPoint("Before Sending normal rectangles");

            sendRectangles(m_pencoderstore->getEncoder(), rectangleaNormal, pframebuffer, &encodeOptions);

            sendRectangles(m_pencoderstore->getEncoder(), rectangleaLossless, pframebuffer, &losslessEncodeOptions);

            auto processortimes2 = ProfileLogger().checkPoint("After Sending normal rectangles");
            m_plogwriter->debug("Before Sending normal rectangles %f processor Mcycles, %f process time, %f kernel "
                                "time, %f wall clock time",
                                processortimes1.m_cycle / 1000000., processortimes1.m_process, processortimes1.m_kernel, (double)(processortimes1.m_time.floating_second()));
            double dt = (double)(processortimes2.m_time.floating_second());
            m_plogwriter->debug("After Sending normal rectangles Mpoint encoded and send: %f for %f processor Mcycles",
                                area, processortimes2.m_cycle / 1000000.);
            m_plogwriter->debug("After Sending normal rectangles %f process time, %f kernel time, %f wall clock time",
                                processortimes2.m_process, processortimes2.m_kernel, dt);

            m_plogwriter->information("Time between request and answer is (in milliseconds): %u",
                                      (unsigned int) timeReqPoint.elapsed().integral_millisecond());
         }
         else
         {
            m_plogwriter->debug("Nothing to send, restoring requested regions");
            critical_section_lock al(&m_criticalsectionRectLoc);
            m_regionRequestedFull.add(regionRequestedFull);
            m_regionRequestedIncremental.add(regionRequestedIncremental);
            m_incrUpdIsReq = bIncrementalUpdateRequest;
            m_fullUpdIsReq = bFullUpdateRequest;
         }
         m_pcursorupdates->restoreFramebuffer(pframebuffer);
      }

      m_plogwriter->debug("Flushing output");
      //  m_plogwriter->checkPoint("4 before flush");
      m_prfboutputgate->flush();
      //  m_plogwriter->checkPoint("5 sendUpdate() end");
   }

   void UpdateSender::paintBlack(::innate_subsystem::Framebuffer *pframebuffer, const Region & regionBlack)
   {
      //::int_rectangle_array_base rectangleaBlack;
      auto rectangleaBlack = regionBlack.getRects();
      for (size_t i = 0; i < rectangleaBlack.size(); i++)
      {
         pframebuffer->fillRect(rectangleaBlack[i], 0);
      }
   }

   void UpdateSender::splitRegion(Encoder *encoder, const Region & region, ::int_rectangle_array_base & rectanglea,
                                  const ::innate_subsystem::Framebuffer *pframebuffer,
                                  const EncodeOptions *encodeOptions)
   {
      //::int_rectangle_array_base rectangleaBase;
      auto rectangleaBase = region.getRects();
      ::int_rectangle_array_base::iterator i;
      for (i = rectangleaBase.begin(); i != rectangleaBase.end(); i++)
      {
         encoder->splitRectangle(*i, rectanglea, pframebuffer, encodeOptions);
      }
   }

   void UpdateSender::sendRectangles(Encoder *encoder, const ::int_rectangle_array_base & rectanglea,
                                     const ::innate_subsystem::Framebuffer *pframebuffer,
                                     const EncodeOptions *encodeOptions)
   {
      //::int_rectangle_array_base::const_iterator i;
      for (auto & rectangle : rectanglea)
      {
         sendRectHeader(rectangle, encoder->getCode());
         encoder->sendRectangle(rectangle, pframebuffer, encodeOptions);
      }
   }

   void UpdateSender::execute()
   {
      m_plogwriter->information("Starting update sender thread for client #{}", m_id);

      while (!isTerminating())
      {
         m_newUpdatesEvent.wait();
         {
            critical_section_lock al(&m_criticalsectionRectLoc);
            m_busy = true;
         }
         m_plogwriter->debug("Update sender thread of client #{} is awake", m_id);
         if (!isTerminating())
         {
            try
            {
               m_plogwriter->debug("UpdateSender::Trying to call the sendUpdate() function");
               sendUpdate();
               m_plogwriter->debug("The sendUpdate() function has finished");
               {
                  critical_section_lock al(&m_criticalsectionRectLoc);
                  m_busy = false;
               }
            }
            catch (::exception &e)
            {
               m_plogwriter->error("The update sender thread caught an error and will"
                                      " be terminated: {}",
                                      e.get_message());
               Thread::terminate();
            }
         }
      }
   }

   void UpdateSender::readUpdateRequest(::remoting::RfbInputGate *io)
   {
      // Read the rest of the scopedstrMessage:
      bool incremental = io->readUInt8() != 0;
      ::int_rectangle reqRect;
      reqRect.left = io->readUInt16();
      reqRect.top = io->readUInt16();
      reqRect.set_width(io->readUInt16());
      reqRect.set_height(io->readUInt16());

      Region regionCombinedReq;
      {
         critical_section_lock al(&m_criticalsectionRectLoc);
         if (incremental)
         {
            m_regionRequestedIncremental.addRect(reqRect);
            m_incrUpdIsReq = true;
         }
         else
         {
            m_regionRequestedFull.addRect(reqRect);
            m_fullUpdIsReq = true;
         }
         m_requestTimePoint.Now();
         regionCombinedReq.add(m_regionRequestedIncremental);
         regionCombinedReq.add(m_regionRequestedFull);
      }

      m_plogwriter->information("update requested ({}, {}, %dx{}, incremental = {})"
                                " by client (client #{})",
                                reqRect.left, reqRect.top, reqRect.width(), reqRect.height(), (int)incremental, m_id);

      ASSERT(m_pupdaterequestlistener != 0);

      bool alreadyHasUpdates = m_pupdatekeeper->checkForUpdates(regionCombinedReq);
      if (alreadyHasUpdates)
      {
         // We should initiaite send update to avoid it skipping on no updates from a desktop
         // FIXME: Code duplication, see the newUpdates() function.
         m_newUpdatesEvent.set_happening();
         m_plogwriter->debug("Client #{} is waking up", m_id);
      }

      m_pupdaterequestlistener->onUpdateRequest(reqRect, incremental);
   }

   void UpdateSender::readSetPixelFormat(::remoting::RfbInputGate *io)
   {
      ::innate_subsystem::PixelFormat pixelformat;
      // Read padding
      io->readUInt16();
      io->readUInt8();

      // Read pixel format
      int bpp = io->readUInt8();
      if (bpp == 8 || bpp == 16 || bpp == 32)
      {
         pixelformat.bitsPerPixel = bpp;
      }
      else
      {
         throw ::subsystem::Exception("Only 8, 16 or 32 bits per pixel supported!");
      }
      pixelformat.colorDepth = io->readUInt8();
      pixelformat.bigEndian = io->readUInt8() != 0;
      bool setColorMapEntr = io->readUInt8() == 0;
      if (setColorMapEntr && bpp != 8)
      {
         throw ::subsystem::Exception("Only 8 bits per pixel supported with set color ::map "
                                      "entries request.");
      }
      pixelformat.redMax = io->readUInt16();
      pixelformat.greenMax = io->readUInt16();
      pixelformat.blueMax = io->readUInt16();
      pixelformat.redShift = io->readUInt8();
      pixelformat.greenShift = io->readUInt8();
      pixelformat.blueShift = io->readUInt8();

      // Read padding
      io->readUInt16();
      io->readUInt8();

      // If palette rewuested fill the pixel format own values
      if (setColorMapEntr)
      {
         pixelformat.redMax = 7;
         pixelformat.greenMax = 7;
         pixelformat.blueMax = 3;
         pixelformat.redShift = 0;
         pixelformat.greenShift = 3;
         pixelformat.blueShift = 6;
      }
      setClientPixelFormat(pixelformat, setColorMapEntr);
   }

   void UpdateSender::setClientPixelFormat(const ::innate_subsystem::PixelFormat & pixelformat, bool clrMapEntries)
   {
      critical_section_lock al(&m_criticalsectionNewPixelFormat);
      m_pixelformatNew = pixelformat;
      m_setColorMapEntr = clrMapEntries;
   }

   void UpdateSender::readSetEncodings(::remoting::RfbInputGate *io)
   {
      io->readUInt8(); // padding
      int numCodes = io->readUInt16();

      int_array ia;
      ia.set_size(numCodes);
      for (int i = 0; i < numCodes; i++)
      {
         int code = (int)io->readUInt32();
         ia[i] = code;
      }

      critical_section_lock lock(&m_criticalsectionNewEncodeOptions);
      m_encodeoptionsNew.setEncodings(ia);
   }

   void UpdateSender::setVideoFrozen(bool value)
   {
      critical_section_lock al(&m_criticalsectionVidFreezeLoc);
      m_videoFrozen = value;
   }

   bool UpdateSender::getVideoFrozen()
   {
      critical_section_lock al(&m_criticalsectionVidFreezeLoc);
      return m_videoFrozen;
   }

   void UpdateSender::readVideoFreeze(::remoting::RfbInputGate *io) { setVideoFrozen(io->readUInt8() != 0); }

   bool UpdateSender::extractReqRegions(Region & incrReqReg, Region & fullReqReg, bool *bIncrementalUpdateRequest, bool *bFullUpdateRequest,
                                        class ::time *timeReqPoint)
   {
      critical_section_lock al(&m_criticalsectionRectLoc);

      incrReqReg = m_regionRequestedIncremental;
      fullReqReg = m_regionRequestedFull;
      *bIncrementalUpdateRequest = m_incrUpdIsReq;
      *bFullUpdateRequest = m_fullUpdIsReq;

      m_regionRequestedFull.clear();
      m_regionRequestedIncremental.clear();
      m_incrUpdIsReq = false;
      m_fullUpdIsReq = false;

      *timeReqPoint = m_requestTimePoint;

      return *bIncrementalUpdateRequest || *bFullUpdateRequest;
   }


   UpdateContainer UpdateSender::extractUpdates()
   {

      return ::transfer(m_pupdatekeeper->extract());

   }


   void UpdateSender::cropUpdContForReqRegions(UpdateContainer & updatecontainer, const Region & incrReqReg,
                                               const Region & fullReqReg)
   {
      // Crop by requested region
      Region regionBack = updatecontainer.m_regionChanged;
      regionBack.add(updatecontainer.m_regionVideo);

      Region regionCombinedReq = incrReqReg;
      regionCombinedReq.add(fullReqReg);

      inscribeCopiedRegionToReqRegion(updatecontainer, regionCombinedReq);

      regionBack.subtract(regionCombinedReq);
      updatecontainer.m_regionChanged.intersect(regionCombinedReq);
      updatecontainer.m_regionVideo.intersect(regionCombinedReq);

      // Return back the out region to the update keeper.
      m_pupdatekeeper->addChangedRegion(regionBack);
   }

   void UpdateSender::inscribeCopiedRegionToReqRegion(UpdateContainer & updatecontainer, const Region & requestRegion)
   {
      // Test copied region. If it is fully inside in the requested region then
      // there is no to change. Otherwise, simply decline copied region.
      if (!updatecontainer.m_regionCopied.is_empty())
      {
         Region dstCopiedRegion = updatecontainer.m_regionCopied;
         dstCopiedRegion.subtract(requestRegion);

         bool copiedRegionFullyInscribed = dstCopiedRegion.is_empty();
         if (copiedRegionFullyInscribed)
         {
            // Then see the same at source coordinates.
            Region m_regionCopied = updatecontainer.m_regionCopied;
            ::int_rectangle dstBounds = m_regionCopied.getBounds();
            int dx = updatecontainer.m_pointCopySource.x - dstBounds.left;
            int dy = updatecontainer.m_pointCopySource.y - dstBounds.top;
            m_regionCopied.translate(dx, dy);
            m_regionCopied.subtract(requestRegion);
            copiedRegionFullyInscribed = m_regionCopied.is_empty();
         }
         if (!copiedRegionFullyInscribed)
         {
            // Convert copied region to changed region.
            updatecontainer.m_regionChanged.add(updatecontainer.m_regionCopied);
            updatecontainer.m_regionCopied.clear();
         }
      }
   }

   void UpdateSender::selectEncoder(EncodeOptions & encodeoptions)
   {
      // Make new encode options take effect. They might have been changed on
      // receiving SetEncodings client scopedstrMessage.
      {
         critical_section_lock lock(&m_criticalsectionNewEncodeOptions);
         encodeoptions = m_encodeoptionsNew;
      }
      // Make sure the encoder object corresponds to the preferred encoding
      // requested in the most recent SetEncodings client scopedstrMessage.
      m_pencoderstore->selectEncoder(encodeoptions.getPreferredEncoding());
   }

   void UpdateSender::updateFramebuffer(UpdateContainer & updatecontainer, bool shareOnlyApp, const Region & prevSharedRegion,
                                        const Region & regionShareApp)
   {
      ::int_rectangle rectangleViewport = getViewPort();

      Region newOpeningPixels;
      if (shareOnlyApp)
      {
         updatecontainer.m_regionChanged.add(updatecontainer.m_regionCopied);
         updatecontainer.m_regionCopied.clear();
         m_regionApp = regionShareApp;
         newOpeningPixels = m_regionApp;
         newOpeningPixels.subtract(m_regionAppOld);
      }

      // Also, if shareapp checked then just opening application pixels can be already painted
      // to black and does not include to changed region (when prev pixel value = current value).
      // Thereby, new opening pixels must be updated in the pframebuffer too. New opening pixels,
      // for example, appears when alien application creep on the shared application.
      updatecontainer.m_regionChanged.add(newOpeningPixels);

      // Frame buffers synchronizing
      Region regionChangedAndCopied = updatecontainer.m_regionChanged;
      regionChangedAndCopied.add(updatecontainer.m_regionCopied);
      regionChangedAndCopied.add(updatecontainer.m_regionVideo);
      regionChangedAndCopied.addRect(m_pcursorupdates->getBackgroundRect());
      {
         critical_section_lock al(&m_criticalsectionRectLoc);
         regionChangedAndCopied.add(m_regionRequestedFull);
      }
defer_construct_newø(m_pframebuffer);
      updatecontainer.m_bScreenSizeChanged =
         !m_pdesktop->updateExternalFramebuffer(m_pframebuffer,regionChangedAndCopied, rectangleViewport) ||
         updatecontainer.m_bScreenSizeChanged;
   }

   bool UpdateSender::updateViewPort(::int_rectangle &rectangleOutNewViewport, bool *shareApp, Region & regionOldShareApp,
                                     Region & regionNewShareApp)
   {
      ::int_rectangle rectangleNewViewport;
      m_senderControlInformation->onGetViewPort(rectangleNewViewport, shareApp, regionNewShareApp);

      critical_section_lock al(&m_criticalsectionViewport);
      bool viewPortChanged = m_rectangleViewport != rectangleNewViewport;
      if (viewPortChanged)
      {
         m_rectangleViewport = rectangleNewViewport;
      }

      bool shareAppModeChanged = *shareApp != m_shareOnlyApp;
      // Emulating share app mode changes as view port changes.
      viewPortChanged = viewPortChanged || shareAppModeChanged;

      rectangleOutNewViewport = rectangleNewViewport;
      return viewPortChanged;
   }

   // Returns part of region with total area no more than area
   // and removes this part form source reg
   Region UpdateSender::takePartFromRegion(Region & region, int area)
   {
      Region out;
      //::int_rectangle_array_base rectanglea;
      auto rectanglea = region.getRects();
      // process region rectanglea form last one, I hope it can reduce allocation number for region structure
      for (int i = rectanglea.size() - 1; i >= 0 && area > 0; i--)
      {
         ::int_rectangle r = rectanglea[i];
         int a = r.area();
         if (a > area)
         {
            r.set_width(r.width() * area / a + 1);
            a = r.area();
         }
         area -= a;
         Region regionTime(r);
         region.subtract(regionTime);
         out.add(regionTime);
      }
      return out;
   }

   int UpdateSender::calcAreas(::int_rectangle_array_base rectanglea)
   {
      int sum = 0;
      for (size_t i = 0; i < rectanglea.size(); i++)
      {
         sum += rectanglea[i].area();
      }
      return sum;
   }


} // namespace remoting
