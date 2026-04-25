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


   // UpdateSender::UpdateSender(RfbCodeRegistrator *codeRegtor, UpdateRequestListener *updReqListener,
   //                            SenderControlInformationInterface *senderControlInformation, RfbOutputGate *output,
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
   //    codeRegtor->addEncCap(EncodingDefs::COPYRECT, VendorDefs::STANDARD, EncodingDefs::SIG_COPYRECT);
   //    codeRegtor->addEncCap(EncodingDefs::HEXTILE, VendorDefs::STANDARD, EncodingDefs::SIG_HEXTILE);
   //    codeRegtor->addEncCap(EncodingDefs::TIGHT, VendorDefs::TIGHTVNC, EncodingDefs::SIG_TIGHT);
   //    codeRegtor->addEncCap(PseudoEncDefs::COMPR_LEVEL_0, VendorDefs::TIGHTVNC, PseudoEncDefs::SIG_COMPR_LEVEL);
   //    codeRegtor->addEncCap(PseudoEncDefs::QUALITY_LEVEL_0, VendorDefs::TIGHTVNC, PseudoEncDefs::SIG_QUALITY_LEVEL);
   //    codeRegtor->addEncCap(PseudoEncDefs::RICH_CURSOR, VendorDefs::TIGHTVNC, PseudoEncDefs::SIG_RICH_CURSOR);
   //    codeRegtor->addEncCap(PseudoEncDefs::POINTER_POS, VendorDefs::TIGHTVNC, PseudoEncDefs::SIG_POINTER_POS);
   //    codeRegtor->addEncCap(PseudoEncDefs::DESKTOP_SIZE, VendorDefs::TIGHTVNC, PseudoEncDefs::SIG_DESKTOP_SIZE);
   //    codeRegtor->addEncCap(PseudoEncDefs::DESKTOP_CONFIGURATION, VendorDefs::TIGHTVNC,
   //                          PseudoEncDefs::SIG_DESKTOP_CONFIGURATION);
   //
   //    codeRegtor->addClToSrvCap(UpdSenderClientMsgDefs::RFB_VIDEO_FREEZE, VendorDefs::TIGHTVNC,
   //                              UpdSenderClientMsgDefs::RFB_VIDEO_FREEZE_SIG);
   //
   //    // Request codes
   //    codeRegtor->regCode(UpdSenderClientMsgDefs::RFB_VIDEO_FREEZE, this);
   //    codeRegtor->regCode(ClientMsgDefs::FB_UPDATE_REQUEST, this);
   //    codeRegtor->regCode(ClientMsgDefs::SET_PIXEL_FORMAT, this);
   //    codeRegtor->regCode(ClientMsgDefs::SET_ENCODINGS, this);
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


    void  UpdateSender::initialize_update_sender(RfbCodeRegistrator *codeRegtor, UpdateRequestListener *updReqListener,
                              SenderControlInformationInterface *senderControlInformation, RfbOutputGate *prfboutputgate,
                              int id, Desktop *desktop, ::subsystem::LogWriter * plogwriter) //:
       // , , ,
       // ,
       // , m_id(id), m_videoFrozen(false), m_shareOnlyApp(false), m_plogwriter = plogwriter;,
       // m_pcursorupdates(plogwriter)
   {
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
      codeRegtor->addEncCap(EncodingDefs::COPYRECT, VendorDefs::STANDARD, EncodingDefs::SIG_COPYRECT);
      codeRegtor->addEncCap(EncodingDefs::HEXTILE, VendorDefs::STANDARD, EncodingDefs::SIG_HEXTILE);
      codeRegtor->addEncCap(EncodingDefs::TIGHT, VendorDefs::TIGHTVNC, EncodingDefs::SIG_TIGHT);
      codeRegtor->addEncCap(PseudoEncDefs::COMPR_LEVEL_0, VendorDefs::TIGHTVNC, PseudoEncDefs::SIG_COMPR_LEVEL);
      codeRegtor->addEncCap(PseudoEncDefs::QUALITY_LEVEL_0, VendorDefs::TIGHTVNC, PseudoEncDefs::SIG_QUALITY_LEVEL);
      codeRegtor->addEncCap(PseudoEncDefs::RICH_CURSOR, VendorDefs::TIGHTVNC, PseudoEncDefs::SIG_RICH_CURSOR);
      codeRegtor->addEncCap(PseudoEncDefs::POINTER_POS, VendorDefs::TIGHTVNC, PseudoEncDefs::SIG_POINTER_POS);
      codeRegtor->addEncCap(PseudoEncDefs::DESKTOP_SIZE, VendorDefs::TIGHTVNC, PseudoEncDefs::SIG_DESKTOP_SIZE);
      codeRegtor->addEncCap(PseudoEncDefs::DESKTOP_CONFIGURATION, VendorDefs::TIGHTVNC,
                            PseudoEncDefs::SIG_DESKTOP_CONFIGURATION);

      codeRegtor->addClToSrvCap(UpdSenderClientMsgDefs::RFB_VIDEO_FREEZE, VendorDefs::TIGHTVNC,
                                UpdSenderClientMsgDefs::RFB_VIDEO_FREEZE_SIG);

      // Request codes
      codeRegtor->regCode(UpdSenderClientMsgDefs::RFB_VIDEO_FREEZE, this);
      codeRegtor->regCode(ClientMsgDefs::FB_UPDATE_REQUEST, this);
      codeRegtor->regCode(ClientMsgDefs::SET_PIXEL_FORMAT, this);
      codeRegtor->regCode(ClientMsgDefs::SET_ENCODINGS, this);

      resume();
   }

   void UpdateSender::onTerminate() { m_newUpdatesEvent.set_happening(); }

   void UpdateSender::onRequest(unsigned int reqCode, RfbInputGate *input)
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
            errMess.formatf("Unknown {} protocol code received", (int)reqCode);
            throw ::subsystem::Exception(errMess);
            break;
      }
   }

   void UpdateSender::init(const ::int_size &viewPortDimension, const ::innate_subsystem::PixelFormat &pf)
   {
      setClientPixelFormat(pf, false);
      {
         critical_section_lock al(&m_criticalsectionViewport);
         m_sizeClient = viewPortDimension;
      }
      m_sizeLastViewport = viewPortDimension;
      m_pupdatekeeper->setBorderRect(viewPortDimension);
   }

   void UpdateSender::newUpdates(const UpdateContainer *updateContainer, const CursorShape *cursorShape)
   {
      m_plogwriter->debug("New updates passed to client #{}", m_id);
      addUpdateContainer(updateContainer);

      m_pcursorupdates->updateCursorShape(cursorShape);
      {
         critical_section_lock al(&m_criticalsectionRectLoc);
         m_busy = true;
         m_newUpdatesEvent.set_happening();
      }
      m_plogwriter->debug("Client #{} is waking up", m_id);
   }

   void UpdateSender::addUpdateContainer(const UpdateContainer *updateContainer)
   {
      UpdateContainer updCont = *updateContainer;

      ::int_rectangle rectangleViewport = getViewPort();

      updCont.m_regionVideo-= rectangleViewport.top_left();
      updCont.m_regionChanged-= rectangleViewport.top_left();
      updCont.m_regionCopied-= rectangleViewport.top_left();
      updCont.m_pointCopySource -= rectangleViewport.top_left();

      m_pupdatekeeper->addUpdateContainer(&updCont);
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

   void UpdateSender::sendRectHeader(const ::int_rectangle &rect, int encodingType)
   {
      // FIXME: Why no warnings on passing bigger integer types?
      m_prfboutputgate->writeUInt16(rect.left);
      m_prfboutputgate->writeUInt16(rect.top);
      m_prfboutputgate->writeUInt16(rect.width());
      m_prfboutputgate->writeUInt16(rect.height());
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

   void UpdateSender::sendNewFBSize(::int_size *size, bool extended)
   {
      // Header
      m_prfboutputgate->writeUInt8(ServerMsgDefs::FB_UPDATE); // scopedstrMessage type
      m_prfboutputgate->writeUInt8(0); // padding
      m_prfboutputgate->writeUInt16(1); // one rectangle

      ::int_rectangle r(*size);
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
            ::int_rectangle rect = screens[i];
            m_prfboutputgate->writeUInt16(rect.left);
            m_prfboutputgate->writeUInt16(rect.top);
            m_prfboutputgate->writeUInt16(rect.width());
            m_prfboutputgate->writeUInt16(rect.height());
         }
      }
   }

   void UpdateSender::sendFbInClientDim(const EncodeOptions *encodeOptions, const ::innate_subsystem::FrameBuffer *pframebuffer,
                                        const ::int_size &size, const ::innate_subsystem::PixelFormat &pf)
   {
      // On the black frame buffer will be overlayed the current framebuffer.
      // This is needed to combine the server frame buffer with a client frame
      // buffer when the dimensions are not equal.
      ::innate_subsystem::FrameBuffer blankFrameBuffer;
      blankFrameBuffer.setProperties(size, pf);
      blankFrameBuffer.setColor(0, 0, 0);
      blankFrameBuffer.copyFrom(pframebuffer, 0, 0);

      ::int_rectangle rectangleForRegion(size);
      Region region(rectangleForRegion);
      ::int_rectangle_array_base rects;
      splitRegion(m_pencoderstore->getEncoder(), &region, &rects, &blankFrameBuffer, encodeOptions);

      // Header
      m_prfboutputgate->writeUInt8(0); // scopedstrMessage type
      m_prfboutputgate->writeUInt8(0); // padding
      unsigned short numRects = (unsigned short)rects.size();
      _ASSERT(numRects == rects.size());
      m_prfboutputgate->writeUInt16(numRects);
      sendRectangles(m_pencoderstore->getEncoder(), &rects, &blankFrameBuffer, encodeOptions);
   }

   void UpdateSender::sendCursorShapeUpdate(const ::innate_subsystem::PixelFormat &fmt, const CursorShape *cursorShape)
   {
      // Send pseudo-rectangle.
      ::int_point hotSpot = cursorShape->getHotSpot();
      ::int_size size = cursorShape->getDimension();
      sendRectHeader(hotSpot.x, hotSpot.y, size.cx, size.cy, PseudoEncDefs::RICH_CURSOR);

      ::innate_subsystem::FrameBuffer fbConverted;
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
      m_plogwriter->debug("Sending cursor position update: ({},{})", pos.x, pos.y);
      sendRectHeader(pos.x, pos.y, 0, 0, PseudoEncDefs::POINTER_POS);
   }

   void UpdateSender::sendCopyRect(const ::int_rectangle_array_base *rects, const ::int_point *source)
   {
      ::int_rectangle_array_base::const_iterator iRect;

      for (iRect = rects->begin(); iRect != rects->end(); iRect++)
      {
         const ::int_rectangle &rect = *iRect;

         sendRectHeader(rect, EncodingDefs::COPYRECT);

         // Send copyRect data
         // FIXME: Each dest rect should have own source point
         m_prfboutputgate->writeUInt16(source->x);
         m_prfboutputgate->writeUInt16(source->y);
      }
   }

   void UpdateSender::sendPalette(::innate_subsystem::PixelFormat *pf)
   {
      m_prfboutputgate->writeUInt8(1); // type
      m_prfboutputgate->writeUInt8(0); // pad
      m_prfboutputgate->writeUInt16(0); // first color
      m_prfboutputgate->writeUInt16(256); // number of colors
      for (unsigned int i = 0; i < 256; i++)
      {
         m_prfboutputgate->writeUInt16(((i >> pf->redShift) & pf->redMax) * 65535 / pf->redMax); // red
         m_prfboutputgate->writeUInt16(((i >> pf->greenShift) & pf->greenMax) * 65535 / pf->greenMax); // green
         m_prfboutputgate->writeUInt16(((i >> pf->blueShift) & pf->blueMax) * 65535 / pf->blueMax); // blue
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
      if (!extractReqRegions(&regionRequestedIncremental, &regionRequestedFull, &bIncrementalUpdateRequest, &bFullUpdateRequest, &timeReqPoint))
      {
         m_plogwriter->debug("No request, exiting from the sendUpdate()");
         return;
      }
      m_plogwriter->debug("Time between request and a point after extractReqRegions (in milliseconds): %u",
                          (unsigned int)timeReqPoint.elapsed().integral_millisecond());
      m_plogwriter->debug("A request has been made, continuing");
      m_plogwriter->debug("The incremental region has {} rectangles", (int)regionRequestedIncremental.getCount());
      m_plogwriter->debug("The full region has {} rectangles", (int)regionRequestedFull.getCount());

      UpdateContainer updCont;
      extractUpdates(&updCont);

      EncodeOptions encodeOptions;
      selectEncoder(&encodeOptions);
      EncodeOptions losslessEncodeOptions;
      bool losslessEnabled = encodeOptions.jpegEnabled();
      if (losslessEnabled)
      {
         selectEncoder(&losslessEncodeOptions);
         losslessEncodeOptions.disableJpeg();
      }

      // Viewport calculating
      ::int_rectangle rectangleViewport;
      bool shareOnlyApp;
      Region prevShareAppRegion;
      Region shareAppRegion;
      bool viewPortChanged = updateViewPort(&rectangleViewport, &shareOnlyApp, &prevShareAppRegion, &shareAppRegion);

      updateFrameBuffer(&updCont, shareOnlyApp, &prevShareAppRegion, &shareAppRegion);
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
         updCont.m_bScreenSizeChanged = true;
      }
      if (dimensionChanged || viewPortChanged)
      {
         updCont.m_regionCopied.clear();

         critical_section_lock al(&m_criticalsectionViewport);
         m_sizeLastViewport = rectangleViewport.size();
         sizeLastViewport = m_sizeLastViewport;
         if (encodeOptions.desktopSizeEnabled() || encodeOptions.desktopConfigurationEnabled())
         {
            m_sizeClient = m_sizeLastViewport;
            sizeClient = m_sizeClient;
         }
         m_pupdatekeeper->setBorderRect(sizeLastViewport);
         updCont.m_regionChanged.crop(sizeLastViewport);
         // Dazzle m_regionChanged
         updCont.m_regionChanged.addRect(sizeLastViewport);
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
         sendPalette(&pixelformatClient);
      }
      m_ppixelconverter->setPixelFormats(pixelformatClient, serverPixelFormat);

      // Send updates
      if (updCont.m_bScreenSizeChanged || (!regionRequestedFull.is_empty() && !encodeOptions.desktopSizeEnabled() &&
                                        !encodeOptions.desktopConfigurationEnabled()))
      {
         m_plogwriter->debug("Screen size changed or full region requested");
         if (encodeOptions.desktopSizeEnabled() || encodeOptions.desktopConfigurationEnabled())
         {
            m_plogwriter->debug("Desktop resize is enabled, sending NewFBSize %dx{}", sizeLastViewport.cx,
                                sizeLastViewport.cy);
            sendNewFBSize(&sizeLastViewport, encodeOptions.desktopConfigurationEnabled());
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
         m_pcursorupdates->update(&encodeOptions, &updCont, !regionRequestedFull.is_empty(), rectangleViewport, shareOnlyApp,
                                &shareAppRegion, pframebuffer, &cursorShape);

         if (!encodeOptions.copyRectEnabled() || getVideoFrozen())
         {
            m_plogwriter->debug("CopyRect is disabled, converting to normal updates");
            updCont.m_regionChanged.add(&updCont.m_regionCopied);
            updCont.m_regionCopied.clear();
         }

         updCont.m_regionChanged.add(&m_regionOldVideoRegion); // This line updates rid video places when
                                                        // video is frozen.
         updCont.m_regionVideo.subtract(&regionRequestedFull);
         updCont.m_regionChanged.subtract(&updCont.m_regionVideo);
         m_regionOldVideoRegion = updCont.m_regionVideo;
         if (getVideoFrozen())
         {
            updCont.m_regionVideo.clear();
         }
         updCont.m_regionChanged.add(&regionRequestedFull);

         // FIXME: Are these two lines really needed? Check that carefully.
         ::int_rectangle frameBufferRect = pframebuffer->getDimension();
         updCont.m_regionVideo.crop(frameBufferRect);
         updCont.m_regionChanged.crop(frameBufferRect);
         shareAppRegion.crop(frameBufferRect);
         prevShareAppRegion.crop(frameBufferRect);
         m_regionLosslessClean.crop(frameBufferRect);

         // If Tight encoding is not supported by the client, convert video updates
         // to normal updates so that the preferred encoding will be used.
         if (!encodeOptions.encodingEnabled(EncodingDefs::TIGHT))
         {
            updCont.m_regionChanged.add(&updCont.m_regionVideo);
            updCont.m_regionVideo.clear();
         }

         // Crop changed and video region by requested regions.
         cropUpdContForReqRegions(&updCont, &regionRequestedIncremental, &regionRequestedFull);

         Region m_regionVideo = updCont.m_regionVideo;
         Region m_regionChanged = updCont.m_regionChanged;

         if (shareOnlyApp)
         {
            Region newOpeningAppRegion = shareAppRegion;
            newOpeningAppRegion.subtract(&prevShareAppRegion);

            Region blackRegion = m_regionChanged;
            blackRegion.add(&m_regionVideo);
            blackRegion.add(&prevShareAppRegion);
            blackRegion.subtract(&shareAppRegion);
            m_regionChanged.add(&blackRegion);
            m_regionChanged.add(&newOpeningAppRegion);
            // Paint black in the framebuffer for the black region.
            paintBlack(pframebuffer, &blackRegion);
         }

         if (losslessEnabled)
         {
            m_regionLosslessDirty.add(m_regionChanged);
            m_regionLosslessClean.subtract(&m_regionChanged);
         }
         // Add some lossless data to every update but no more than 1/100 part of framebuffer.
         // So at 20 fps full screen will be sent in 5 sec.
         int screenArea = frameBufferRect.area();
         Region losslessRegion = takePartFromRegion(&m_regionLosslessClean, screenArea / 100);
         if (losslessEnabled)
         {
            if (losslessRegion.is_empty())
            {
               m_regionLosslessClean.set(&m_regionLosslessDirty);
               m_regionLosslessDirty.clear();
            }
         }

         //
         // At this point, we've got final regions in m_regionChanged and m_regionVideo.
         //

         // Convert m_regionChanged to the final ::list_base of rectangles.
         m_plogwriter->debug("Number of normal rectangles before splitting: {}", m_regionChanged.getCount());
         ::int_rectangle_array_base rectangleaNormal;
         splitRegion(m_pencoderstore->getEncoder(), &m_regionChanged, &rectangleaNormal, pframebuffer, &encodeOptions);

         // Convert losslessRegion to the final ::list_base of rectangles.
         ::int_rectangle_array_base rectangleaLossless;
         if (losslessEnabled && !losslessRegion.is_empty())
         {
            m_plogwriter->debug("Number of lossless rectangles before splitting: {}", losslessRegion.getCount());
            splitRegion(m_pencoderstore->getEncoder(), &losslessRegion, &rectangleaLossless, pframebuffer, &losslessEncodeOptions);
         }
         // Do the same for the m_regionVideo.
         ::int_rectangle_array_base rectangleaVideo;
         if (!m_regionVideo.is_empty())
         {
            m_plogwriter->debug("Video region is not empty");
            m_pencoderstore->validateJpegEncoder(); // make sure JpegEncoder is allocated
            splitRegion(m_pencoderstore->getJpegEncoder(), &m_regionVideo, &rectangleaVideo, pframebuffer, &encodeOptions);
         }

         // Get the final ::list_base of CopyRect rectangles.
         ::int_rectangle_array_base rectangleaCopy;
         updCont.m_regionCopied.getRectVector(&rectangleaCopy);

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

         if (updCont.m_bCursorPosChanged)
         {
            numTotalRects++;
            m_plogwriter->debug("Adding a pseudo-rectangle for cursor position update");
         }
         if (updCont.m_bCursorShapeChanged)
         {
            numTotalRects++;
            m_plogwriter->debug("Adding a pseudo-rectangle for cursor shape update");
         }
         m_plogwriter->debug("Total number of rectangles and pseudo-rectangles: {}", numTotalRects);

         // FIXME: Handle this better, e.g. send first 65534 rectangles.
         _ASSERT(numTotalRects <= 65534);

         if (numTotalRects != 0)
         {
            m_plogwriter->debug("Sending FramebufferUpdate scopedstrMessage header");
            m_prfboutputgate->writeUInt8(ServerMsgDefs::FB_UPDATE); // scopedstrMessage type
            m_prfboutputgate->writeUInt8(0); // padding
            m_prfboutputgate->writeUInt16((unsigned short)numTotalRects);

            if (updCont.m_bCursorPosChanged)
            {
               sendCursorPosUpdate();
            }
            if (updCont.m_bCursorShapeChanged)
            {
               m_plogwriter->debug("Sending cursor shape update");
               sendCursorShapeUpdate(pixelformatClient, &cursorShape);
            }
            if (rectangleaCopy.size() > 0)
            {
               m_plogwriter->debug("Sending CopyRect rectangles");
               sendCopyRect(&rectangleaCopy, &updCont.m_pointCopySource);
            }

            m_plogwriter->debug("Time between request and a point before send and coding (in milliseconds): %u",
                                (unsigned int) timeReqPoint.elapsed().integral_millisecond());
            m_plogwriter->debug("Sending video rectangles");
            sendRectangles(m_pencoderstore->getJpegEncoder(), &rectangleaVideo, pframebuffer, &encodeOptions);
            m_plogwriter->debug("Sending normal rectangles");
            double area = rectangleaNormal.total_area() / 1'000'000.; // in millions of pixels
            auto processortimes1 = ProfileLogger().checkPoint("Before Sending normal rectangles");

            sendRectangles(m_pencoderstore->getEncoder(), &rectangleaNormal, pframebuffer, &encodeOptions);

            sendRectangles(m_pencoderstore->getEncoder(), &rectangleaLossless, pframebuffer, &losslessEncodeOptions);

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
            m_regionRequestedFull.add(&regionRequestedFull);
            m_regionRequestedIncremental.add(&regionRequestedIncremental);
            m_incrUpdIsReq = bIncrementalUpdateRequest;
            m_fullUpdIsReq = bFullUpdateRequest;
         }
         m_pcursorupdates->restoreFrameBuffer(pframebuffer);
      }

      m_plogwriter->debug("Flushing output");
      //  m_plogwriter->checkPoint("4 before flush");
      m_prfboutputgate->flush();
      //  m_plogwriter->checkPoint("5 sendUpdate() end");
   }

   void UpdateSender::paintBlack(::innate_subsystem::FrameBuffer *pframebuffer, const Region *blackRegion)
   {
      ::int_rectangle_array_base blackRects;
      blackRegion->getRectVector(&blackRects);
      for (size_t i = 0; i < blackRects.size(); i++)
      {
         pframebuffer->fillRect(&blackRects[i], 0);
      }
   }

   void UpdateSender::splitRegion(Encoder *encoder, const Region *region, ::int_rectangle_array_base *rects,
                                  const ::innate_subsystem::FrameBuffer *pframebuffer,
                                  const EncodeOptions *encodeOptions)
   {
      ::int_rectangle_array_base baseRects;
      region->getRectVector(&baseRects);
      ::int_rectangle_array_base::iterator i;
      for (i = baseRects.begin(); i != baseRects.end(); i++)
      {
         encoder->splitRectangle(&*i, rects, pframebuffer, encodeOptions);
      }
   }

   void UpdateSender::sendRectangles(Encoder *encoder, const ::int_rectangle_array_base *rects,
                                     const ::innate_subsystem::FrameBuffer *pframebuffer,
                                     const EncodeOptions *encodeOptions)
   {
      ::int_rectangle_array_base::const_iterator i;
      for (i = rects->begin(); i != rects->end(); i++)
      {
         sendRectHeader(&*i, encoder->getCode());
         encoder->sendRectangle(&*i, pframebuffer, encodeOptions);
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
               m_plogwriter->interror("The update sender thread caught an error and will"
                                      " be terminated: {}",
                                      e.get_message());
               Thread::terminate();
            }
         }
      }
   }

   void UpdateSender::readUpdateRequest(RfbInputGate *io)
   {
      // Read the rest of the scopedstrMessage:
      bool incremental = io->readUInt8() != 0;
      ::int_rectangle reqRect;
      reqRect.left = io->readUInt16();
      reqRect.top = io->readUInt16();
      reqRect.set_width(io->readUInt16());
      reqRect.set_height(io->readUInt16());

      Region combinedReqRegions;
      {
         critical_section_lock al(&m_criticalsectionRectLoc);
         if (incremental)
         {
            m_regionRequestedIncremental.addRect(&reqRect);
            m_incrUpdIsReq = true;
         }
         else
         {
            m_regionRequestedFull.addRect(&reqRect);
            m_fullUpdIsReq = true;
         }
         m_requestTimePoint = class ::time::now();
         combinedReqRegions.add(&m_regionRequestedIncremental);
         combinedReqRegions.add(&m_regionRequestedFull);
      }

      m_plogwriter->information("update requested ({}, {}, %dx{}, incremental = {})"
                                " by client (client #{})",
                                reqRect.left, reqRect.top, reqRect.width(), reqRect.height(), (int)incremental, m_id);

      _ASSERT(m_pupdaterequestlistener != 0);

      bool alreadyHasUpdates = m_pupdatekeeper->checkForUpdates(&combinedReqRegions);
      if (alreadyHasUpdates)
      {
         // We should initiaite send update to avoid it skipping on no updates from a desktop
         // FIXME: Code duplication, see the newUpdates() function.
         m_newUpdatesEvent.set_happening();
         m_plogwriter->debug("Client #{} is waking up", m_id);
      }

      m_pupdaterequestlistener->onUpdateRequest(&reqRect, incremental);
   }

   void UpdateSender::readSetPixelFormat(RfbInputGate *io)
   {
      ::innate_subsystem::PixelFormat pf;
      // Read padding
      io->readUInt16();
      io->readUInt8();

      // Read pixel format
      int bpp = io->readUInt8();
      if (bpp == 8 || bpp == 16 || bpp == 32)
      {
         pf.bitsPerPixel = bpp;
      }
      else
      {
         throw ::subsystem::Exception("Only 8, 16 or 32 bits per pixel supported!");
      }
      pf.colorDepth = io->readUInt8();
      pf.bigEndian = io->readUInt8() != 0;
      bool setColorMapEntr = io->readUInt8() == 0;
      if (setColorMapEntr && bpp != 8)
      {
         throw ::subsystem::Exception("Only 8 bits per pixel supported with set color ::map "
                                      "entries request.");
      }
      pf.redMax = io->readUInt16();
      pf.greenMax = io->readUInt16();
      pf.blueMax = io->readUInt16();
      pf.redShift = io->readUInt8();
      pf.greenShift = io->readUInt8();
      pf.blueShift = io->readUInt8();

      // Read padding
      io->readUInt16();
      io->readUInt8();

      // If palette rewuested fill the pixel format own values
      if (setColorMapEntr)
      {
         pf.redMax = 7;
         pf.greenMax = 7;
         pf.blueMax = 3;
         pf.redShift = 0;
         pf.greenShift = 3;
         pf.blueShift = 6;
      }
      setClientPixelFormat(&pf, setColorMapEntr);
   }

   void UpdateSender::setClientPixelFormat(const ::innate_subsystem::PixelFormat &pf, bool clrMapEntries)
   {
      critical_section_lock al(&m_criticalsectionNewPixelFormat);
      m_ppixelformatNew = *pf;
      m_setColorMapEntr = clrMapEntries;
   }

   void UpdateSender::readSetEncodings(RfbInputGate *io)
   {
      io->readUInt8(); // padding
      int numCodes = io->readUInt16();

      ::array_base<int>::list_base;
      ::list_base.reserve(numCodes);
      for (int i = 0; i < numCodes; i++)
      {
         int code = (int)io->readUInt32();
         ::list_base.add(code);
      }

      critical_section_lock lock(&m_criticalsectionNewEncodeOptions);
      m_pencodeoptionsNew.setEncodings(&::list_base);
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

   void UpdateSender::readVideoFreeze(RfbInputGate *io) { setVideoFrozen(io->readUInt8() != 0); }

   bool UpdateSender::extractReqRegions(Region *incrReqReg, Region *fullReqReg, bool *bIncrementalUpdateRequest, bool *bFullUpdateRequest,
                                        class ::time *timeReqPoint)
   {
      critical_section_lock al(&m_criticalsectionRectLoc);

      *incrReqReg = m_regionRequestedIncremental;
      *fullReqReg = m_regionRequestedFull;
      *bIncrementalUpdateRequest = m_incrUpdIsReq;
      *bFullUpdateRequest = m_fullUpdIsReq;

      m_regionRequestedFull.clear();
      m_regionRequestedIncremental.clear();
      m_incrUpdIsReq = false;
      m_fullUpdIsReq = false;

      *timeReqPoint = m_requestTimePoint;

      return *bIncrementalUpdateRequest || *bFullUpdateRequest;
   }

   void UpdateSender::extractUpdates(UpdateContainer *updCont) { m_pupdatekeeper->extract(updCont); }

   void UpdateSender::cropUpdContForReqRegions(UpdateContainer *updCont, const Region *incrReqReg,
                                               const Region *fullReqReg)
   {
      // Crop by requested region
      Region backRegion = updCont->m_regionChanged;
      backRegion.add(&updCont->m_regionVideo);

      Region combinedReqRegion = *incrReqReg;
      combinedReqRegion.add(fullReqReg);

      inscribeCopiedRegionToReqRegion(updCont, &combinedReqRegion);

      backRegion.subtract(&combinedReqRegion);
      updCont->m_regionChanged.intersect(&combinedReqRegion);
      updCont->m_regionVideo.intersect(&combinedReqRegion);

      // Return back the out region to the update keeper.
      m_pupdatekeeper->addChangedRegion(&backRegion);
   }

   void UpdateSender::inscribeCopiedRegionToReqRegion(UpdateContainer *updCont, const Region *requestRegion)
   {
      // Test copied region. If it is fully inside in the requested region then
      // there is no to change. Otherwise, simply decline copied region.
      if (!updCont->m_regionCopied.is_empty())
      {
         Region dstCopiedRegion = updCont->m_regionCopied;
         dstCopiedRegion.subtract(requestRegion);

         bool copiedRegionFullyInscribed = dstCopiedRegion.is_empty();
         if (copiedRegionFullyInscribed)
         {
            // Then see the same at source coordinates.
            Region m_regionCopied = updCont->m_regionCopied;
            ::int_rectangle dstBounds = m_regionCopied.getBounds();
            int dx = updCont->m_pointCopySource.x - dstBounds.left;
            int dy = updCont->m_pointCopySource.y - dstBounds.top;
            m_regionCopied.translate(dx, dy);
            m_regionCopied.subtract(requestRegion);
            copiedRegionFullyInscribed = m_regionCopied.is_empty();
         }
         if (!copiedRegionFullyInscribed)
         {
            // Convert copied region to changed region.
            updCont->m_regionChanged.add(&updCont->m_regionCopied);
            updCont->m_regionCopied.clear();
         }
      }
   }

   void UpdateSender::selectEncoder(EncodeOptions *encodeOptions)
   {
      // Make new encode options take effect. They might have been changed on
      // receiving SetEncodings client scopedstrMessage.
      {
         critical_section_lock lock(&m_criticalsectionNewEncodeOptions);
         *encodeOptions = m_pencodeoptionsNew;
      }
      // Make sure the encoder object corresponds to the preferred encoding
      // requested in the most recent SetEncodings client scopedstrMessage.
      m_pencoderstore->selectEncoder(encodeOptions->getPreferredEncoding());
   }

   void UpdateSender::updateFrameBuffer(UpdateContainer *updCont, bool shareOnlyApp, const Region *prevSharedRegion,
                                        const Region *shareAppRegion)
   {
      ::int_rectangle rectangleViewport = getViewPort();

      Region newOpeningPixels;
      if (shareOnlyApp)
      {
         updCont->m_regionChanged.add(&updCont->m_regionCopied);
         updCont->m_regionCopied.clear();
         m_regionApp = *shareAppRegion;
         newOpeningPixels = m_regionApp;
         newOpeningPixels.subtract(&m_regionAppOld);
      }

      // Also, if shareapp checked then just opening application pixels can be already painted
      // to black and does not include to changed region (when prev pixel value = current value).
      // Thereby, new opening pixels must be updated in the framebuffer too. New opening pixels,
      // for example, appears when alien application creep on the shared application.
      updCont->m_regionChanged.add(&newOpeningPixels);

      // Frame buffers synchronizing
      Region changedAndCopyRgns = updCont->m_regionChanged;
      changedAndCopyRgns.add(&updCont->m_regionCopied);
      changedAndCopyRgns.add(&updCont->m_regionVideo);
      changedAndCopyRgns.addRect(&m_pcursorupdates.getBackgroundRect());
      {
         critical_section_lock al(&m_criticalsectionRectLoc);
         changedAndCopyRgns.add(&m_regionRequestedFull);
      }

      updCont->m_bScreenSizeChanged =
         !m_pdesktop->updateExternalFrameBuffer(&m_pframebuffer, &changedAndCopyRgns, &rectangleViewport) ||
         updCont->m_bScreenSizeChanged;
   }

   bool UpdateSender::updateViewPort(::int_rectangle *outNewViewPort, bool *shareApp, Region *prevShareAppRegion,
                                     Region *newShareAppRegion)
   {
      ::int_rectangle newViewPort;
      m_senderControlInformation->onGetViewPort(&newViewPort, shareApp, newShareAppRegion);

      critical_section_lock al(&m_criticalsectionViewport);
      bool viewPortChanged = !m_rectangleViewport.isEqualTo(&newViewPort);
      if (viewPortChanged)
      {
         m_rectangleViewport = newViewPort;
      }

      bool shareAppModeChanged = *shareApp != m_shareOnlyApp;
      // Emulating share app mode changes as view port changes.
      viewPortChanged = viewPortChanged || shareAppModeChanged;

      *outNewViewPort = newViewPort;
      return viewPortChanged;
   }

   // Returns part of region with total area no more than area
   // and removes this part form source reg
   Region UpdateSender::takePartFromRegion(Region *reg, int area)
   {
      Region out;
      ::int_rectangle_array_base rects;
      reg->getRectVector(&rects);
      // process region rects form last one, I hope it can reduce allocation number for region structure
      for (int i = rects.size() - 1; i >= 0 && area > 0; i--)
      {
         ::int_rectangle r = rects[i];
         int a = r.area();
         if (a > area)
         {
            r.set_width(r.width() * area / a + 1);
            a = r.area();
         }
         area -= a;
         Region t(r);
         reg->subtract(&t);
         out.add(&t);
      }
      return out;
   }

   int UpdateSender::calcAreas(::int_rectangle_array_base rects)
   {
      int sum = 0;
      for (size_t i = 0; i < rects.size(); i++)
      {
         sum += rects[i].area();
      }
      return sum;
   }


} // namespace remoting
