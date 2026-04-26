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
#include "UpdateHandlerServer.h"

namespace remoting
{


   UpdateHandlerServer::UpdateHandlerServer(BlockingGate *pblockinggate, DesktopSrvDispatcher * pdispatcher,
                                            AnEventListener *extTerminationListener, ::subsystem::LogWriter * plogwriter) :
       DesktopServerProto(pblockinggate), m_extTerminationListener(extTerminationListener), m_plogwriter = plogwriter;,
       m_pscreendriverfactory(m_pconfigurator->getServerConfig())
   {
      m_pupdatehandler = new UpdateHandlerImpl(this, &m_pscreendriverfactory, log);

      dispatcher->registerNewHandle(EXTRACT_REQ, this);
      dispatcher->registerNewHandle(SCREEN_PROP_REQ, this);
      dispatcher->registerNewHandle(SET_FULL_UPD_REQ_REGION, this);
      dispatcher->registerNewHandle(SET_EXCLUDING_REGION, this);
      dispatcher->registerNewHandle(FRAME_BUFFER_INIT, this);
      m_plogwriter->debug("UpdateHandlerServer created");
   }

   UpdateHandlerServer::~UpdateHandlerServer() { delete m_pupdatehandler; }

   void UpdateHandlerServer::onUpdate()
   {
      critical_section_lock al(m_pblockinggate);
      try
      {
         m_pblockinggate->writeUInt8(UPDATE_DETECTED);
      }
      catch (::exception &e)
      {
         m_plogwriter->error("An error has been occurred while sending the"
                             " UPDATE_DETECTED scopedstrMessage from UpdateHandlerServer: {}",
                             e.get_message());
         m_extTerminationListener->onAnObjectEvent();
      }
   }

   void UpdateHandlerServer::onRequest(unsigned char reqCode, BlockingGate *pblockinggate)
   {
      switch (reqCode)
      {
         case EXTRACT_REQ:
            m_plogwriter->debug("UpdateHandlerServer, EXTRACT_REQ recieved");
            extractReply(pblockinggate);
            break;
         case SCREEN_PROP_REQ:
            m_plogwriter->debug("UpdateHandlerServer, SCREEN_PROP_REQ recieved");
            screenPropReply(pblockinggate);
            break;
         case SET_FULL_UPD_REQ_REGION:
            m_plogwriter->debug("UpdateHandlerServer, SET_FULL_UPD_REQ_REGION recieved");
            receiveFullReqReg(pblockinggate);
            break;
         case SET_EXCLUDING_REGION:
            m_plogwriter->debug("UpdateHandlerServer, SET_EXCLUDING_REGION recieved");
            receiveExcludingReg(pblockinggate);
            break;
         case FRAME_BUFFER_INIT:
            m_plogwriter->debug("UpdateHandlerServer, FRAME_BUFFER_INIT recieved");
            // Init from client
            serverInit(pblockinggate);
            break;
         default:
            ::string errMess;
            errMess..formatf("Unknown {} protocol code received from a pipe client", (int)reqCode);
            throw ::subsystem::Exception(errMess);
            break;
      }
   }

   void UpdateHandlerServer::extractReply(BlockingGate *pblockinggate)
   {
      m_plogwriter->debug("UpdateHandlerServer: extract updates");
      UpdateContainer updatecontainer;
      m_pupdatehandler->extract(&updatecontainer);
      m_plogwriter->debug("UpdateHandlerServer: %u changed rectangles", updatecontainer.m_regionChanged.getCount());

      const ::innate_subsystem::Framebuffer *pframebuffer = m_pupdatehandler->getFramebuffer();

      ::innate_subsystem::PixelFormat pixelformatNew = pframebuffer->getPixelFormat();

      if (!m_pixelformatOld.isEqualTo(&pixelformatNew))
      {
         updatecontainer.m_bScreenSizeChanged = true;
         m_pixelformatOld = pixelformatNew;
      }

      pblockinggate->writeUInt8(updatecontainer.m_bScreenSizeChanged);
      if (updatecontainer.m_bScreenSizeChanged)
      {
         // Send new screen properties
         m_plogwriter->debug("UpdateHandlerServer: Send new screen properties");
         sendPixelFormat(&pixelformatNew, pblockinggate);
         ::int_size sizeFramebuffer = pframebuffer->getDimension();
         ::int_rectangle rectangleFramebuffer = sizeFramebuffer;
         sendDimension(&sizeFramebuffer, pblockinggate);
         sendFramebuffer(pframebuffer, &rectangleFramebuffer, pblockinggate);
      }

      // Send video region
      m_plogwriter->debug("UpdateHandlerServer: Send video region");
      sendRegion(&updatecontainer.m_regionVideo, pblockinggate);
      // Send changed region
      ::int_rectangle_array_base rectanglea;
      ::int_rectangle_array_base::iterator iRect;
      updatecontainer.m_regionChanged.getRects(&rectanglea);
      unsigned int countChangedRect = (unsigned int)rectanglea.size();
      _ASSERT(countChangedRect == rectanglea.size());
      m_plogwriter->debug("UpdateHandlerServer: send %u changed rectangles", countChangedRect);
      pblockinggate->writeUInt32(countChangedRect);

      for (iRect = rectanglea.begin(); iRect < rectanglea.end(); iRect++)
      {
         ::int_rectangle rectangle = &(*iRect);
         sendRect(rectangle, pblockinggate);
         sendFramebuffer(pframebuffer, rectangle, pblockinggate);
      }

      // Send "copyrect"
      m_plogwriter->debug("UpdateHandlerServer: Send copyrect");
      bool hasCopyRect = !updatecontainer.m_regionCopied.is_empty();
      pblockinggate->writeUInt8(hasCopyRect);
      if (hasCopyRect)
      {
         sendPoint(&updatecontainer.m_pointCopySource, pblockinggate);
         updatecontainer.m_regionCopied.getRects(&rectanglea);
         iRect = rectanglea.begin();
         sendRect(&(*iRect), pblockinggate);
         sendFramebuffer(pframebuffer, &(*iRect), pblockinggate);
      }

      // Send cursor pointPosition if it has been changed.
      m_plogwriter->debug("UpdateHandlerServer: Send cursor pointPosition");
      pblockinggate->writeUInt8(updatecontainer.m_bCursorPosChanged);
      sendPoint(&updatecontainer.m_pointCursorPos, pblockinggate);

      // Send cursor shape if it has been changed.
      m_plogwriter->debug("UpdateHandlerServer: Send cursor shape");
      pblockinggate->writeUInt8(updatecontainer.m_bCursorShapeChanged);
      if (updatecontainer.m_bCursorShapeChanged)
      {
         const CursorShape *pcursorshape = m_pupdatehandler->getCursorShape();
         sendDimension(&pcursorshape->getDimension(), pblockinggate);
         sendPoint(&pcursorshape->getHotSpot(), pblockinggate);

         // Send pixels
         pblockinggate->writeFully(pcursorshape->getPixels()->getBuffer(), pcursorshape->getPixelsSize());
         // Send mask
         if (pcursorshape->getMaskSize())
         {
            pblockinggate->writeFully((void *)pcursorshape->getMask(), pcursorshape->getMaskSize());
         }
      }
      m_plogwriter->debug("UpdateHandlerServer::extractReply finished");
   }

   void UpdateHandlerServer::screenPropReply(BlockingGate *pblockinggate)
   {
      const ::innate_subsystem::Framebuffer *pframebuffer = m_pupdatehandler->getFramebuffer();
      sendPixelFormat(&pframebuffer->getPixelFormat(), pblockinggate);
      sendDimension(&pframebuffer->getDimension(), pblockinggate);
   }

   void UpdateHandlerServer::receiveFullReqReg(BlockingGate *pblockinggate)
   {
      Region region;
      readRegion(&region, pblockinggate);
      m_pupdatehandler->setFullUpdateRequested(&region);
   }

   void UpdateHandlerServer::receiveExcludingReg(BlockingGate *pblockinggate)
   {
      Region region;
      readRegion(&region, pblockinggate);
      m_pupdatehandler->setExcludedRegion(&region);
   }

   void UpdateHandlerServer::serverInit(BlockingGate *pblockinggate)
   {
      // FIXME: Use another method to initialize m_pframebufferBackup
      // because this method use a lot of memory.
      auto pframebuffer = createø<::innate_subsystem::Framebuffer>();
      readPixelFormat(&m_pixelformatOld, pblockinggate);
      ::int_size size = readDimension(pblockinggate);
      pframebuffer->setProperties(&size, &m_pixelformatOld);

      readFramebuffer(&pframebuffer, &size, pblockinggate);
      m_pupdatehandler->initFramebuffer(&pframebuffer);
   }


} // namespace remoting



