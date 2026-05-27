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
#include "framework.h"
#include "subsystem/node/PipeClient.h"
#include "UpdateHandlerClient.h"
#include "ReconnectException.h"
#include "subsystem/platform/Exception.h"


namespace remoting_rfb
{


   // UpdateHandlerClient::UpdateHandlerClient(BlockingGate *pblockinggate, DesktopSrvDispatcher * pdispatcher,
   //                                          UpdateListener *pupdatelistenerExternal, ::subsystem::LogWriter * plogwriter) :
   //     DesktopServerProto(pblockinggate), m_pupdatelistenerExternal(pupdatelistenerExternal), m_plogwriter = plogwriter;
   // {
   //    dispatcher->registerNewHandle(UPDATE_DETECTED, this);
   //
   //    // m_pframebufferBackup building
   //    ::innate_subsystem::PixelFormat pixelformatTerm;
   //    ::i32_size sizeTerm;
   //    getScreenProperties(&pixelformatTerm, &sizeTerm);
   //
   //    m_pframebufferBackup->setProperties(&sizeTerm, &pixelformatTerm);
   //
   //    // Synchronize our ::innate_subsystem::Framebuffer and the server ::innate_subsystem::Framebuffer
   //    sendInit(m_pcontrolgate);
   // }
   //

   UpdateHandlerClient::UpdateHandlerClient()
   {
   }


   UpdateHandlerClient::UpdateHandlerClient(::remoting_rfb_node::Configurator * pconfigurator, BlockingGate *pblockinggate, DesktopSrvDispatcher * pdispatcher,
                     UpdateListener *pupdatelistenerExternal, ::subsystem::LogWriter * plogwriter)
   {

      initialize_update_handler_client(pconfigurator, pblockinggate, pdispatcher, pupdatelistenerExternal, plogwriter);

   }


   UpdateHandlerClient::~UpdateHandlerClient() {}



    void UpdateHandlerClient::initialize_update_handler_client(::remoting_rfb_node::Configurator * pconfigurator, BlockingGate *pblockinggate, DesktopSrvDispatcher * pdispatcher,
                    UpdateListener *pupdatelistenerExternal, ::subsystem::LogWriter * plogwriter)
   {

      initialize_desktop_server_proto(pconfigurator, pblockinggate);
      m_pupdatelistenerExternal = pupdatelistenerExternal;
      m_plogwriter = plogwriter;
      //{
         pdispatcher->registerNewHandle(UPDATE_DETECTED, this);

         // m_pframebufferBackup building
         ::innate_subsystem::PixelFormat pixelformatTerm;
         ::i32_size sizeTerm;
         getScreenProperties(pixelformatTerm, sizeTerm);

         m_pframebufferBackup->setProperties(sizeTerm, pixelformatTerm);

         // Synchronize our ::innate_subsystem::Framebuffer and the server ::innate_subsystem::Framebuffer
         sendInit(m_pcontrolgate);
      //}
   }


   void UpdateHandlerClient::onRequest(::u8 reqCode, BlockingGate *pblockinggate)
   {
      switch (reqCode)
      {
         case UPDATE_DETECTED:
            m_pupdatelistenerExternal->onUpdate();
            break;
         default:
            ::string errMess;
            errMess.format("Unknown {} protocol code received from a pipe "
                             "update detector",
                             (::i32)reqCode);
            throw ::subsystem::Exception(errMess);
            break;
      }
   }

   UpdateContainer UpdateHandlerClient::extract()
   {
      //updatecontainer.clear();

      critical_section_lock al(m_pcontrolgate);

      //UpdateContainer updatecontainer;
      try
      {
         UpdateContainer updatecontainer;
         m_plogwriter->debug("UpdateHandlerClient: send EXTRACT_REQ");
         m_pcontrolgate->writeUInt8(EXTRACT_REQ); // query for extract

         // Get screen size changed
         m_plogwriter->debug("UpdateHandlerClient: Get screen size changed");
         updatecontainer.m_bScreenSizeChanged = m_pcontrolgate->readUInt8() != 0;

         if (updatecontainer.m_bScreenSizeChanged)
         {
            m_plogwriter->information("UpdateHandlerClient: screen size changed");
            // Store old screen properties
            ::innate_subsystem::PixelFormat pixelformatOld = m_pframebufferBackup->getPixelFormat();
            ::i32_size oldDim = m_pframebufferBackup->getDimension();
            // Get new screen properties
            ::innate_subsystem::PixelFormat pixelformatNew;
            readPixelFormat(pixelformatNew, m_pcontrolgate);
            ::i32_size sizeNew = readDimension(m_pcontrolgate);
            ;
            if (pixelformatNew!= pixelformatOld || sizeNew!=oldDim)
            {
               m_plogwriter->information("UpdateHandlerClient: new screen size: %dx{}", sizeNew.cx, sizeNew.cy);
               m_plogwriter->information("UpdateHandlerClient: new pixel format: "
                                         "{}, {}, {}, {}, {}, {}, {}, {}",
                                         (::i32)pixelformatNew.bigEndian, (::i32)pixelformatNew.bitsPerPixel, (::i32)pixelformatNew.redMax,
                                         (::i32)pixelformatNew.greenMax, (::i32)pixelformatNew.blueMax, (::i32)pixelformatNew.redShift,
                                         (::i32)pixelformatNew.greenShift, (::i32)pixelformatNew.blueShift);
               critical_section_lock al(&m_criticalsectionFramebuffer);
               m_pframebufferBackup->setProperties(sizeNew, pixelformatNew);
            }
            // Equalizing this frame buffer by other side frame buffer.
            ::i32_rectangle rectangleFramebuffer = m_pframebufferBackup->getDimension();
            readFramebuffer(m_pframebufferBackup,rectangleFramebuffer, m_pcontrolgate);
         }

         // Get video region
         m_plogwriter->debug("UpdateHandlerClient: Get video region");
         readRegion(updatecontainer.m_regionVideo, m_pcontrolgate);
         // Get changed region
         ::u32 countChangedRect = m_pcontrolgate->readUInt32();
         m_plogwriter->information("UpdateHandlerClient: count changed rectangles = %u", countChangedRect);
         for (::u32 i = 0; i < countChangedRect; i++)
         {
            ::i32_rectangle r = readRect(m_pcontrolgate);
            updatecontainer.m_regionChanged.addRect(r);
            readFramebuffer(m_pframebufferBackup, r, m_pcontrolgate);
         }

         // Get "copyrect"
         ::u8 hasCopyRect = m_pcontrolgate->readUInt8();
         if (hasCopyRect)
         {
            m_plogwriter->information("UpdateHandlerClient: has \"CopyRect\"");
            updatecontainer.m_pointCopySource = readPoint(m_pcontrolgate);
            ::i32_rectangle r = readRect(m_pcontrolgate);
            updatecontainer.m_regionCopied.addRect(r);
            readFramebuffer(m_pframebufferBackup, r, m_pcontrolgate);
         }

         // Get cursor pointPosition if it has been changed.
         updatecontainer.m_bCursorPosChanged = m_pcontrolgate->readUInt8() != 0;
         if (updatecontainer.m_bCursorPosChanged)
         {
            m_plogwriter->information("UpdateHandlerClient: cursor pos changed");
         }
         updatecontainer.m_pointCursorPos = readPoint(m_pcontrolgate);

         // Get cursor shape if it has been changed.
         updatecontainer.m_bCursorShapeChanged = m_pcontrolgate->readUInt8() != 0;
         if (updatecontainer.m_bCursorShapeChanged)
         {
            m_plogwriter->information("UpdateHandlerClient: cursor shape changed");
            ::innate_subsystem::PixelFormat pixelformatNew = m_pframebufferBackup->getPixelFormat();
            ::i32_size sizeNew = readDimension(m_pcontrolgate);
            ::i32_point newHotSpot = readPoint(m_pcontrolgate);

            m_cursorshape.setProperties(sizeNew, pixelformatNew);
            m_cursorshape.setHotSpot(newHotSpot.x, newHotSpot.y);

            // Get pixels
            m_pcontrolgate->readFully(m_cursorshape.getPixels()->getBuffer(), m_cursorshape.getPixelsSize());
            // Get mask
            if (m_cursorshape.getMaskSize())
            {
               m_pcontrolgate->readFully((void *)m_cursorshape.getMask(), m_cursorshape.getMaskSize());
            }
         }
         return updatecontainer;
      }
      catch (ReconnectException &)
      {
         m_plogwriter->information("UpdateHandlerClient: ReconnectException catching in the extract function");
      }
      //updatecontainer = updatecontainer;
      return {};
   }

   void UpdateHandlerClient::setFullUpdateRequested(const Region & region)
   {
      critical_section_lock al(m_pcontrolgate);

      try
      {
         m_pcontrolgate->writeUInt8(SET_FULL_UPD_REQ_REGION);
         sendRegion(region, m_pcontrolgate);
      }
      catch (ReconnectException &)
      {
      }
   }

   void UpdateHandlerClient::setExcludedRegion(const Region & regionExcluded)
   {
      critical_section_lock al(m_pcontrolgate);

      try
      {
         m_pcontrolgate->writeUInt8(SET_EXCLUDING_REGION);
         sendRegion(regionExcluded, m_pcontrolgate);
      }
      catch (ReconnectException &)
      {
      }
   }

   bool UpdateHandlerClient::checkForUpdates(Region & region) { return false; }

   void UpdateHandlerClient::getScreenProperties(::innate_subsystem::PixelFormat & pixelformat, ::i32_size & size)
   {
      critical_section_lock al(m_pcontrolgate);

      m_pcontrolgate->writeUInt8(SCREEN_PROP_REQ);
      readPixelFormat(pixelformat, m_pcontrolgate);
      size = readDimension(m_pcontrolgate);
   }

   void UpdateHandlerClient::sendInit(BlockingGate *pblockinggate)
   {
      critical_section_lock al(pblockinggate);
      pblockinggate->writeUInt8(FRAME_BUFFER_INIT);

      sendPixelFormat(m_pframebufferBackup->getPixelFormat(), pblockinggate);
      ::i32_size size = m_pframebufferBackup->getDimension();
      sendDimension(size, pblockinggate);
      sendFramebuffer(m_pframebufferBackup, size, pblockinggate);
   }


} // namespace remoting_rfb



