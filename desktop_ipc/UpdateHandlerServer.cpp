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
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//
#include "framework.h"
#include "UpdateHandlerServer.h"

UpdateHandlerServer::UpdateHandlerServer(BlockingGate *forwGate,
                                         DesktopSrvDispatcher *dispatcher,
                                         AnEventListener *extTerminationListener,
                                         LogWriter *log)
: DesktopServerProto(forwGate),
  m_extTerminationListener(extTerminationListener),
  m_log(log),
  m_scrDriverFactory(Configurator::getInstance()->getServerConfig())
{
  m_updateHandler = new UpdateHandlerImpl(this, &m_scrDriverFactory, log);

  dispatcher->registerNewHandle(EXTRACT_REQ, this);
  dispatcher->registerNewHandle(SCREEN_PROP_REQ, this);
  dispatcher->registerNewHandle(SET_FULL_UPD_REQ_REGION, this);
  dispatcher->registerNewHandle(SET_EXCLUDING_REGION, this);
  dispatcher->registerNewHandle(FRAME_BUFFER_INIT, this);
  m_log->debug("UpdateHandlerServer created");
}

UpdateHandlerServer::~UpdateHandlerServer()
{
  delete m_updateHandler;
}

void UpdateHandlerServer::onUpdate()
{
  AutoLock al(m_forwGate);
  try {
    m_forwGate->writeUInt8(UPDATE_DETECTED);
  } catch (::remoting::Exception &e) {
    m_log->error("An error has been occurred while sending the"
                 " UPDATE_DETECTED message from UpdateHandlerServer: {}",
               e.getMessage());
    m_extTerminationListener->onAnObjectEvent();
  }
}

void UpdateHandlerServer::onRequest(unsigned char reqCode, BlockingGate *backGate)
{
  switch (reqCode) {
  case EXTRACT_REQ:
    m_log->debug("UpdateHandlerServer, EXTRACT_REQ recieved");
    extractReply(backGate);
    break;
  case SCREEN_PROP_REQ:
    m_log->debug("UpdateHandlerServer, SCREEN_PROP_REQ recieved");
    screenPropReply(backGate);
    break;
  case SET_FULL_UPD_REQ_REGION:
    m_log->debug("UpdateHandlerServer, SET_FULL_UPD_REQ_REGION recieved");
    receiveFullReqReg(backGate);
    break;
  case SET_EXCLUDING_REGION:
    m_log->debug("UpdateHandlerServer, SET_EXCLUDING_REGION recieved");
    receiveExcludingReg(backGate);
    break;
  case FRAME_BUFFER_INIT:
    m_log->debug("UpdateHandlerServer, FRAME_BUFFER_INIT recieved");
    // Init from client
    serverInit(backGate);
    break;
  default:
    ::string errMess;
    errMess..formatf("Unknown {} protocol code received from a pipe client",
                   (int)reqCode);
    throw ::remoting::Exception(errMess);
    break;
  }
}

void UpdateHandlerServer::extractReply(BlockingGate *backGate)
{
  m_log->debug("UpdateHandlerServer: extract updates");
  UpdateContainer updCont;
  m_updateHandler->extract(&updCont);
  m_log->debug("UpdateHandlerServer: %u changed rectangles", updCont.changedRegion.getCount());

  const FrameBuffer *fb = m_updateHandler->getFrameBuffer();

  PixelFormat newPf = fb->getPixelFormat();

  if (!m_oldPf.isEqualTo(&newPf)) {
    updCont.screenSizeChanged = true;
    m_oldPf = newPf;
  }

  backGate->writeUInt8(updCont.screenSizeChanged);
  if (updCont.screenSizeChanged) {
    // Send new screen properties
    m_log->debug("UpdateHandlerServer: Send new screen properties");
    sendPixelFormat(&newPf, backGate);
    ::int_size fbDim = fb->getDimension();
    ::int_rectangle fbRect = fbDim;
    sendDimension(&fbDim, backGate);
    sendFrameBuffer(fb, &fbRect, backGate);
  }

  // Send video region
  m_log->debug("UpdateHandlerServer: Send video region");
  sendRegion(&updCont.videoRegion, backGate);
  // Send changed region
  ::array_base<::int_rectangle> rects;
  ::array_base<::int_rectangle>::iterator iRect;
  updCont.changedRegion.getRectVector(&rects);
  unsigned int countChangedRect = (unsigned int)rects.size();
  _ASSERT(countChangedRect == rects.size());
  m_log->debug("UpdateHandlerServer: send %u changed rectangles", countChangedRect);
  backGate->writeUInt32(countChangedRect);

  for (iRect = rects.begin(); iRect < rects.end(); iRect++) {
    ::int_rectangle *rect = &(*iRect);
    sendRect(rect, backGate);
    sendFrameBuffer(fb, rect, backGate);
  }

  // Send "copyrect"
  m_log->debug("UpdateHandlerServer: Send copyrect");
  bool hasCopyRect = !updCont.copiedRegion.is_empty();
  backGate->writeUInt8(hasCopyRect);
  if (hasCopyRect) {
    sendPoint(&updCont.copySrc, backGate);
    updCont.copiedRegion.getRectVector(&rects);
    iRect = rects.begin();
    sendRect(&(*iRect), backGate);
    sendFrameBuffer(fb, &(*iRect), backGate);
  }

  // Send cursor position if it has been changed.
  m_log->debug("UpdateHandlerServer: Send cursor position");
  backGate->writeUInt8(updCont.cursorPosChanged);
  sendPoint(&updCont.cursorPos, backGate);

  // Send cursor shape if it has been changed.
  m_log->debug("UpdateHandlerServer: Send cursor shape");
  backGate->writeUInt8(updCont.cursorShapeChanged);
  if (updCont.cursorShapeChanged) {
    const CursorShape *curSh = m_updateHandler->getCursorShape();
    sendDimension(&curSh->getDimension(), backGate);
    sendPoint(&curSh->getHotSpot(), backGate);

    // Send pixels
    backGate->writeFully(curSh->getPixels()->getBuffer(), curSh->getPixelsSize());
    // Send mask
    if (curSh->getMaskSize()) {
      backGate->writeFully((void *)curSh->getMask(), curSh->getMaskSize());
    }
  }
  m_log->debug("UpdateHandlerServer::extractReply finished");
}

void UpdateHandlerServer::screenPropReply(BlockingGate *backGate)
{
  const FrameBuffer *fb = m_updateHandler->getFrameBuffer();
  sendPixelFormat(&fb->getPixelFormat(), backGate);
  sendDimension(&fb->getDimension(), backGate);
}

void UpdateHandlerServer::receiveFullReqReg(BlockingGate *backGate)
{
  Region region;
  readRegion(&region, backGate);
  m_updateHandler->setFullUpdateRequested(&region);
}

void UpdateHandlerServer::receiveExcludingReg(BlockingGate *backGate)
{
  Region region;
  readRegion(&region, backGate);
  m_updateHandler->setExcludedRegion(&region);
}

void UpdateHandlerServer::serverInit(BlockingGate *backGate)
{
  // FIXME: Use another method to initialize m_backupFrameBuffer
  // because this method use a lot of memory.
  FrameBuffer fb;
  readPixelFormat(&m_oldPf, backGate);
  ::int_size dim = readDimension(backGate);
  fb.setProperties(&dim, &m_oldPf);

  readFrameBuffer(&fb, &dim, backGate);
  m_updateHandler->initFrameBuffer(&fb);
}
