// Copyright (C) 2009,2010,2011,2012 GlavSoft LLC.
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

#pragma once


//#include "subsystem/thread/lockable_critical_section.h"
#include "subsystem/thread/Thread.h"
#include "remoting/remoting_rtc/desktop/UpdateKeeper.h"
#include "UpdateRequestListener.h"
#include "innate_subsystem/framebuffer/Framebuffer.h"
#include "Viewport.h"
#include "remoting/remoting_rtc/network/RfbOutputGate.h"
#include "remoting/remoting_rtc/network/RfbInputGate.h"
#include "acme/parallelization/happening.h"
#include "remoting/remoting_rtc/rfb_sconn/Encoder.h"
#include "remoting/remoting_rtc/rfb_sconn/HextileEncoder.h"
#include "remoting/remoting_rtc/rfb_sconn/JpegEncoder.h"
#include "remoting/remoting_rtc/rfb_sconn/EncoderStore.h"
#include "remoting/remoting_rtc/rfb_sconn/RfbCodeRegistrator.h"
//#include "subsystem/platform/class ::time.h"
#include "CursorUpdates.h"
#include "SenderControlInformationInterface.h"
//#include "log_writer/LogWriter.h"

namespace remoting
{

   class CLASS_DECL_REMOTING  UpdateSender : public ::subsystem::Thread, public RfbDispatcherListener
   {
   public:


      ::pointer < ::subsystem::LogWriter > m_plogwriter;

      // ::happening m_newUpdatesEvent;

      ::happening m_newUpdatesEvent;

      ::pointer < UpdateRequestListener > m_pupdaterequestlistener;
      ::remoting_rtc::Region m_regionRequestedIncremental;
      ::remoting_rtc::Region m_regionRequestedFull;
      bool m_incrUpdIsReq;
      bool m_fullUpdIsReq;
      bool m_busy;
      // Property for perfomance measurements. It uses with the regions mutex.
      class ::time m_requestTimePoint;
      lockable_critical_section m_criticalsectionRectLoc;

      SenderControlInformationInterface *m_senderControlInformation;

      ::i32_rectangle m_rectangleViewport;
      ::i32_size m_sizeClient;
      ::i32_size m_sizeLastViewport;
      bool m_shareOnlyApp;
      ::remoting_rtc::Region m_regionApp;
      ::remoting_rtc::Region m_regionAppOld;
      lockable_critical_section m_criticalsectionViewport;

      ::pointer < UpdateKeeper > m_pupdatekeeper;

      ::pointer < ::innate_subsystem::Framebuffer > m_pframebuffer;
      ::pointer < Desktop > m_pdesktop;

      ::pointer < CursorUpdates >  m_pcursorupdates;

      // EncodeOptions class CLASS_DECL_REMOTING  maintain the configuration of encoders and
      // pseudo-encoders read from the SetEncodings client scopedstrMessage.
      // m_pencodeoptionsNew may be changed at any time but all change and read
      // operations must be synchronized with m_criticalsectionNewEncodeOptions.
      // In the beginning of each pframebuffer update, m_pencodeoptionsNew will be
      // safely copied to a local variable which then will be used while encoding
      // data. Thus, changes to m_pencodeoptionsNew will take effect on next
      // pframebuffer update.
      EncodeOptions m_encodeoptionsNew;
      lockable_critical_section m_criticalsectionNewEncodeOptions;

      // Pixel format requested by the RFB client. It may be changed at any time
      // but all change and read operations must be synchronized with
      // m_criticalsectionNewPixelFormat. In the beginning of each pframebuffer update, it
      // will be safely copied to a local variable which then will be used while
      // encoding data. Thus, changes to m_ppixelformatNew will take effect on next
      // pframebuffer update.
      ::innate_subsystem::PixelFormat m_pixelformatNew;
      lockable_critical_section m_criticalsectionNewPixelFormat;

      // This flag indicates that color ::map entries requested. If this flag is true
      // then before send the updates updateSender must to send the color ::map
      // entries
      bool m_setColorMapEntr;

      // This flag indicates that video is frozen or not.
      bool m_videoFrozen;
      // This region constains a video region which was sent at previous time.
      ::remoting_rtc::Region m_regionOldVideoRegion;
      lockable_critical_section m_criticalsectionVidFreezeLoc;


      ::remoting_rtc::Region m_regionLosslessDirty;
      ::remoting_rtc::Region m_regionLosslessClean;

      // Output stream.
      ::pointer < ::remoting_rtc::RfbOutputGate > m_prfboutputgate;

      // PixelConverter can convert from one pixel format to another using fast
      // table lookups. It should be used only in the sender thread.
      ::pointer < ::remoting_rtc::PixelConverter > m_ppixelconverter;

      // All encoders are encapsulated in EncoderStore. It allocates new encoders
      // on request and maintains a pointer to the preferred encoder. This object
      // should be used only by the sender thread.
      ::pointer < EncoderStore > m_pencoderstore;

      // Information
      // FIXME: Document this properly.
      ::i32 m_id;

      // updReqListener - pointer to the out listener for retranslate
      // update reqest to out.
      // FIXME: Document all the arguments properly.
      // UpdateSender(RfbCodeRegistrator *m_prfbcoderegistrator, UpdateRequestListener *updReqListener,
      //              SenderControlInformationInterface *senderControlInformation, ::remoting_rtc::RfbOutputGate *output,
      //              ::i32 id, Desktop *desktop, ::subsystem::LogWriter * plogwriter);
      UpdateSender();
       ~UpdateSender() override;

      void destroy() override;

      virtual void initialize_update_sender(RfbCodeRegistrator *m_prfbcoderegistrator, UpdateRequestListener *updReqListener,
             SenderControlInformationInterface *senderControlInformation, ::remoting_rtc::RfbOutputGate *output,
             ::i32 id, Desktop *desktop, ::subsystem::LogWriter * plogwriter);

      // The sendServerInit() function sends first rfb init scopedstrMessage to a client
      // FIXME: The comment does not seem to be relevant.
      void init(const ::i32_size &viewPortDimension, const ::innate_subsystem::PixelFormat & pixelformat);

      // The triggerUpdate() function adds updatecontainer to the UpdateKeeper,
      // gives new frame buffer properties if needed.
      // Also, if client is ready by now (m_reqRect is not empty), triggerUpdate()
      // notifies to thread.
      // FIXME: The comment does not seem to be relevant.
      void newUpdates(const UpdateContainer & updatecontainer, const ::remoting_rtc::CursorShape *cursorShape);

      // Block cursor pos sending by this connection to a client. Unblocking will
      // be automaticly for a time.
      void blockCursorPosSending();

      // Returns a rectangle for current Viewport
      ::i32_rectangle getViewPort();

      // Check requsted regions to determine if the client is ready.
      // Return true if the client is ready, false otherwise.
      bool clientIsReady();

   //protected:
      // Listener function which implements RfbDispatcherListener. It will be
      // called on receiving client messages if we registered as a handler for
      // corresponding RFB scopedstrMessage types.
      virtual void onRequest(::u32 reqCode, ::remoting_rtc::RfbInputGate *input) override;

      // Handlers for individual RFB client messages. Called by onRequest().
      void readUpdateRequest(::remoting_rtc::RfbInputGate *io);
      void readSetPixelFormat(::remoting_rtc::RfbInputGate *io);
      void readSetEncodings(::remoting_rtc::RfbInputGate *io);
      void readVideoFreeze(::remoting_rtc::RfbInputGate *io);

      // The addUpdateContainer() function adds all updates from the first
      // updatecontainer parameter to the own UpdateContainer object.
      // This function may asynchronously be called from any threads.
      void addUpdateContainer(const UpdateContainer & updatecontainer);

      // The sender thread.
      void onThreadMain() override;
      void onTermThread() override;

      // Check cursor pointPosition for changing and store it to the m_cursorPos.
      // Return true value if cursor pointPosition has been changed.
      void checkCursorPos(UpdateContainer & updatecontainer, const ::i32_rectangle &rectangleViewport);

      // Thread safed get and set of the m_videFrozen flag.
      void setVideoFrozen(bool value);
      bool getVideoFrozen();

      // The sendUpdate() function sends all stored updates to the client.
      // Access to a ::innate_subsystem::Framebuffer data passes through the pframebuffer pointer
      // of the function parameter.
      void sendUpdate();

      // sendUpdate() auxiliary functions.
      // Returns true if an update has been requested.
      bool extractReqRegions(::remoting_rtc::Region & incrReqReg, ::remoting_rtc::Region & fullReqReg, bool *bIncrementalUpdateRequest,
                             bool *bFullUpdateRequest, class ::time *timeReqPoint);
      UpdateContainer extractUpdates();
      void cropUpdContForReqRegions(UpdateContainer & updatecontainer, const ::remoting_rtc::Region & incrReqReg,
                                    const ::remoting_rtc::Region & fullReqReg);
      void inscribeCopiedRegionToReqRegion(UpdateContainer & updatecontainer, const ::remoting_rtc::Region & requestRegion);

      void selectEncoder(EncodeOptions & encodeoptions);

      // Updates pixels in the internal frame buffer.
      void updateFramebuffer(UpdateContainer & updatecontainer, bool shareOnlyApp, const ::remoting_rtc::Region & prevSharedRegion,
                             const ::remoting_rtc::Region & regionShareApp);
      // Updates internal view port rectangle.
      // Returns true if view port has been changed during the operation.
      bool updateViewPort(::i32_rectangle &rectangleOutNewViewport, bool *shareApp, ::remoting_rtc::Region & regionOldShareApp,
                          ::remoting_rtc::Region & regionNewShareApp);

      // The sendPalette() function sends pallete after a set color ::map request
      // by a client.
      void sendPalette(::innate_subsystem::PixelFormat & pixelformat);

      // Set new client pixel format. This function may be called from any thread.
      // New pixel format will take effect on sending next frame buffer update.
      void setClientPixelFormat(const ::innate_subsystem::PixelFormat & pixelformat, bool clrMapEntries);

      void sendRectHeader(const ::i32_rectangle & rectangle, ::i32 encodingType);
      void sendRectHeader(::u16 x, ::u16 y, ::u16 w, ::u16 h, ::i32 encodingType);
      void sendNewFBSize(::i32_size & size, bool extended);
      void sendFbInClientDim(const EncodeOptions *encodeOptions, const ::innate_subsystem::Framebuffer *pframebuffer,
                             const ::i32_size &size, const ::innate_subsystem::PixelFormat & pixelformat);
      void sendCursorShapeUpdate(const ::innate_subsystem::PixelFormat &fmt,
                                 const ::remoting_rtc::CursorShape *cursorShape);
      void sendCursorPosUpdate();
      void sendCopyRect(const ::i32_rectangle_array_base & rectanglea, const ::i32_point & pointSource);

      // Encode and send a ::list_base of rectangles via the specified encoder.
      void sendRectangles(Encoder *encoder, const ::i32_rectangle_array_base & rectanglea,
                          const ::innate_subsystem::Framebuffer *pframebuffer, const EncodeOptions *encodeOptions);

      // This function paints black region in pframebuffer->
      void paintBlack(::innate_subsystem::Framebuffer *pframebuffer, const ::remoting_rtc::Region & regionBlack);

      // This function is used to split a region into a ::list_base of rectangles,
      // where actual splitting is performed by the specified encoder object.
      // We do not use m_pencoder because this function may be used for the video
      // encoder as well.
      void splitRegion(Encoder *encoder, const ::remoting_rtc::Region & region, ::i32_rectangle_array_base & rectanglea,
                       const ::innate_subsystem::Framebuffer *pframebuffer, const EncodeOptions *encodeOptions);

      // Returns part of region with total area not much more than area
      // and removes this part form source reg
      ::remoting_rtc::Region takePartFromRegion(::remoting_rtc::Region & region, ::i32 area);
      // calculate total area of rectanglea in pixels
      ::i32 calcAreas(::i32_rectangle_array_base rectanglea);

   };


} //  namespace remoting




