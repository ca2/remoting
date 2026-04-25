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
       m_scrDriverFactory(m_pconfigurator->getServerConfig())
   {
      m_updateHandler = new UpdateHandlerImpl(this, &m_scrDriverFactory, log);

      dispatcher->registerNewHandle(EXTRACT_REQ, this);
      dispatcher->registerNewHandle(SCREEN_PROP_REQ, this);
      dispatcher->registerNewHandle(SET_FULL_UPD_REQ_REGION, this);
      dispatcher->registerNewHandle(SET_EXCLUDING_REGION, this);
      dispatcher->registerNewHandle(FRAME_BUFFER_INIT, this);
      m_plogwriter->debug("UpdateHandlerServer created");
   }

   UpdateHandlerServer::~UpdateHandlerServer() { delete m_updateHandler; }

   void UpdateHandlerServer::onUpdate()
   {
      critical_section_lock al(m_forwGate);
      try
      {
         m_forwGate->writeUInt8(UPDATE_DETECTED);
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
      UpdateContainer updCont;
      m_updateHandler->extract(&updCont);
      m_plogwriter->debug("UpdateHandlerServer: %u changed rectangles", updCont.m_regionChanged.getCount());

      const ::innate_subsystem::FrameBuffer *pframebuffer = m_updateHandler->getFrameBuffer();

      ::innate_subsystem::PixelFormat newPf = pframebuffer->getPixelFormat();

      if (!m_oldPf.isEqualTo(&newPf))
      {
         updCont.m_bScreenSizeChanged = true;
         m_oldPf = newPf;
      }

      pblockinggate->writeUInt8(updCont.m_bScreenSizeChanged);
      if (updCont.m_bScreenSizeChanged)
      {
         // Send new screen properties
         m_plogwriter->debug("UpdateHandlerServer: Send new screen properties");
         sendPixelFormat(&newPf, pblockinggate);
         ::int_size fbDim = pframebuffer->getDimension();
         ::int_rectangle fbRect = fbDim;
         sendDimension(&fbDim, pblockinggate);
         sendFrameBuffer(pframebuffer, &fbRect, pblockinggate);
      }

      // Send video region
      m_plogwriter->debug("UpdateHandlerServer: Send video region");
      sendRegion(&updCont.m_regionVideo, pblockinggate);
      // Send changed region
      ::int_rectangle_array_base rects;
      ::int_rectangle_array_base::iterator iRect;
      updCont.m_regionChanged.getRectVector(&rects);
      unsigned int countChangedRect = (unsigned int)rects.size();
      _ASSERT(countChangedRect == rects.size());
      m_plogwriter->debug("UpdateHandlerServer: send %u changed rectangles", countChangedRect);
      pblockinggate->writeUInt32(countChangedRect);

      for (iRect = rects.begin(); iRect < rects.end(); iRect++)
      {
         ::int_rectangle *rect = &(*iRect);
         sendRect(rect, pblockinggate);
         sendFrameBuffer(pframebuffer, rect, pblockinggate);
      }

      // Send "copyrect"
      m_plogwriter->debug("UpdateHandlerServer: Send copyrect");
      bool hasCopyRect = !updCont.m_regionCopied.is_empty();
      pblockinggate->writeUInt8(hasCopyRect);
      if (hasCopyRect)
      {
         sendPoint(&updCont.m_pointCopySource, pblockinggate);
         updCont.m_regionCopied.getRectVector(&rects);
         iRect = rects.begin();
         sendRect(&(*iRect), pblockinggate);
         sendFrameBuffer(pframebuffer, &(*iRect), pblockinggate);
      }

      // Send cursor position if it has been changed.
      m_plogwriter->debug("UpdateHandlerServer: Send cursor position");
      pblockinggate->writeUInt8(updCont.m_bCursorPosChanged);
      sendPoint(&updCont.cursorPos, pblockinggate);

      // Send cursor shape if it has been changed.
      m_plogwriter->debug("UpdateHandlerServer: Send cursor shape");
      pblockinggate->writeUInt8(updCont.m_bCursorShapeChanged);
      if (updCont.m_bCursorShapeChanged)
      {
         const CursorShape *curSh = m_updateHandler->getCursorShape();
         sendDimension(&curSh->getDimension(), pblockinggate);
         sendPoint(&curSh->getHotSpot(), pblockinggate);

         // Send pixels
         pblockinggate->writeFully(curSh->getPixels()->getBuffer(), curSh->getPixelsSize());
         // Send mask
         if (curSh->getMaskSize())
         {
            pblockinggate->writeFully((void *)curSh->getMask(), curSh->getMaskSize());
         }
      }
      m_plogwriter->debug("UpdateHandlerServer::extractReply finished");
   }

   void UpdateHandlerServer::screenPropReply(BlockingGate *pblockinggate)
   {
      const ::innate_subsystem::FrameBuffer *pframebuffer = m_updateHandler->getFrameBuffer();
      sendPixelFormat(&pframebuffer->getPixelFormat(), pblockinggate);
      sendDimension(&pframebuffer->getDimension(), pblockinggate);
   }

   void UpdateHandlerServer::receiveFullReqReg(BlockingGate *pblockinggate)
   {
      Region region;
      readRegion(&region, pblockinggate);
      m_updateHandler->setFullUpdateRequested(&region);
   }

   void UpdateHandlerServer::receiveExcludingReg(BlockingGate *pblockinggate)
   {
      Region region;
      readRegion(&region, pblockinggate);
      m_updateHandler->setExcludedRegion(&region);
   }

   void UpdateHandlerServer::serverInit(BlockingGate *pblockinggate)
   {
      // FIXME: Use another method to initialize m_backupFrameBuffer
      // because this method use a lot of memory.
      ::innate_subsystem::FrameBuffer pframebuffer;
      readPixelFormat(&m_oldPf, pblockinggate);
      ::int_size size = readDimension(pblockinggate);
      pframebuffer.setProperties(&size, &m_oldPf);

      readFrameBuffer(&pframebuffer, &size, pblockinggate);
      m_updateHandler->initFrameBuffer(&pframebuffer);
   }


} // namespace remoting



