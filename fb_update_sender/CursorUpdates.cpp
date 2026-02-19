// Copyright (C) 2010,2011,2012 GlavSoft LLC.
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
#include "CursorUpdates.h"
#include "thread/AutoLock.h"

CursorUpdates::CursorUpdates(LogWriter *log)
: m_blockCurPosTime(0),
  m_isDrawCursorMethod(false),
  m_log(log)
{
}

CursorUpdates::~CursorUpdates()
{
}

void CursorUpdates::update(const EncodeOptions *encodeOptions,
                           UpdateContainer *updCont,
                           bool fullRegReq,
                           const ::int_rectangle & viewPort,
                           bool shareOnlyApp,
                           const Region *shareAppRegion,
                           FrameBuffer *fb,
                           CursorShape *cursorShape)
{
  // Check cursor events. If they are outside of shared region then ignore they.
  if (shareOnlyApp) {
    bool inside = shareAppRegion->isPointInside(updCont->cursorPos.x, updCont->cursorPos.y);
    if (!inside) {
      updCont->cursorPosChanged = false;
      updCont->cursorShapeChanged = false;
    }
  }

  bool richEnabled = encodeOptions->richCursorEnabled();
  bool posEnabled = encodeOptions->pointerPosEnabled();

  bool posChanged = updCont->cursorPosChanged;
  // The posChanged and updCont->cursorPosChanged flags has different
  // purpose. The posChanged will be always raised if position real changed
  // but the updCont->cursorPosChanged flag can be raised only cursor pos
  // is not blocked.
  if (posChanged) {
    bool cursorPosBlockingIsIgnored = !richEnabled;
    posChanged = checkCursorPos(updCont, viewPort, cursorPosBlockingIsIgnored);
  }

  if (!richEnabled && !posEnabled) {
    // Draw the shape on the frame buffer.
	m_log->debug("Draw the shape of cursor on the frame buffer.");
    drawCursor(updCont, fb);
  }

  bool initShapeByZeroIsNeeded = false;
  if (richEnabled && !posEnabled) {
    bool methodWasChanged = false;
    if (updCont->cursorPosChanged) { // The move by the server
      methodWasChanged = !m_isDrawCursorMethod;
      if (methodWasChanged) {
        m_isDrawCursorMethod = true;
        // By cursor shape method will be sended zero shape.
        initShapeByZeroIsNeeded = true;
      }
    } else if (posChanged) { // The move by the client.
      methodWasChanged = m_isDrawCursorMethod;
      if (methodWasChanged) {
        m_isDrawCursorMethod = false;
        // Restore background under the cursor shape
		m_log->debug("Restore background under the cursor shape");
		restoreFrameBuffer(fb);
        ::int_rectangle backgroundRect = getBackgroundRect();
        updCont->changedRegion.addRect(backgroundRect);
      }
    }
    if (m_isDrawCursorMethod) {
  	  m_log->debug("Draw the shape of cursor on the frame buffer (DrawCursorMethod)");
      drawCursor(updCont, fb);
      updCont->cursorShapeChanged = methodWasChanged;
    } else {
      updCont->cursorShapeChanged = updCont->cursorShapeChanged ||
                                    methodWasChanged;
    }
  }

  if (!richEnabled || !posEnabled) {
    m_log->debug("Clearing cursorPosChanged"
               " (RichCursor or PointerPos are not requested)");
    updCont->cursorPosChanged = false;
  } else if (fullRegReq) {
    m_log->debug("Raising cursorPosChanged (full region requested)");
    // ignore cursor position changing blocking on full request
    checkCursorPos(updCont, viewPort, true);
    updCont->cursorPosChanged = true;
  }
  if (!richEnabled) {
    m_log->debug("Clearing cursorShapeChanged (RichCursor disabled)");
    updCont->cursorShapeChanged = false;
  } else if (fullRegReq) {
    m_log->debug("Raising cursorShapeChanged (RichCursor enabled"
               " and full region requested)");
    updCont->cursorShapeChanged = true;
  }
  if (updCont->cursorShapeChanged) {
    extractCursorShape(cursorShape);
    if (initShapeByZeroIsNeeded) {
      cursorShape->resetToEmpty();
    }
  }
}

void CursorUpdates::restoreFrameBuffer(FrameBuffer *fb)
{
  AutoLock al(&m_curPosLocMut);
  ::int_rectangle dstRect = m_shapeBackground.getDimension();
  dstRect.set_top_left(m_backgroundPos.x, m_backgroundPos.y);
  fb->copyFrom(dstRect, &m_shapeBackground, 0, 0);
  // m_shapeBackground.setDimension(&::int_size(0, 0));
}

void CursorUpdates::drawCursor(UpdateContainer *updCont, FrameBuffer *fb)
{
  AutoLock al(&m_curPosLocMut);
  // Add previous background rectangle to the changed region.
  ::int_rectangle rect(m_shapeBackground.getDimension());
  rect.set_top_left(m_backgroundPos.x, m_backgroundPos.y);
  updCont->changedRegion.addRect(rect);
  // Keep the current background rectangle.
  Point hotSpot = m_cursorShape.getHotSpot();
  m_backgroundPos.setPoint(m_cursorPos.x - hotSpot.x,
                           m_cursorPos.y - hotSpot.y);
  m_shapeBackground.setProperties(m_cursorShape.getDimension(),
                                  m_cursorShape.getPixelFormat());
  // Keep background under cursor shape to can reconstruct full image.
  m_shapeBackground.copyFrom(fb,
                             m_backgroundPos.x,
                             m_backgroundPos.y);
  // Draw the cursor shape on the frame buffer
  rect.set(m_cursorShape.getDimension());
  rect.set_top_left(m_backgroundPos.x, m_backgroundPos.y);

  fb->overlay(rect,
              m_cursorShape.getPixels(), 0, 0,
              m_cursorShape.getMask());
}

bool CursorUpdates::checkCursorPos(UpdateContainer *updCont,
                                   const ::int_rectangle & viewPort,
                                   bool curPosBlockingIsIgnored)
{
  AutoLock al(&m_curPosLocMut);
  Point cursorPos = updCont->cursorPos;
  cursorPos.x -= viewPort.left;
  cursorPos.y -= viewPort.top;

  if (cursorPos.x < 0) {
    cursorPos.x = 0;
  } else if (cursorPos.x >= viewPort.width()) {
    cursorPos.x = viewPort.width() - 1;
  }
  if (cursorPos.y < 0) {
    cursorPos.y = 0;
  } else if (cursorPos.y >= viewPort.height()) {
    cursorPos.y = viewPort.height() - 1;
  }

  bool isBlocked = isCursorPosBlocked() && !curPosBlockingIsIgnored;
  bool positionChanged = cursorPos.x != m_cursorPos.x ||
                         cursorPos.y != m_cursorPos.y;
  if (!positionChanged || isBlocked) {
    updCont->cursorPosChanged = false;
  } else {
    m_cursorPos.x = cursorPos.x;
    m_cursorPos.y = cursorPos.y;
  }
  m_log->debug("CursorUpdates::checkCursorPos cursor position ({},{}), changed:{}",
    m_cursorPos.x, m_cursorPos.y, positionChanged);
  return positionChanged;
}

void CursorUpdates::blockCursorPosSending()
{
  AutoLock al(&m_curPosLocMut);
  // Block cursor pos sending to a time interval.
  m_blockCurPosTime = ::earth::time::now();
}

bool CursorUpdates::isCursorPosBlocked()
{
  AutoLock al(&m_curPosLocMut);
  if ((::earth::time::now() - m_blockCurPosTime).getTime() > 1000) {
    return false; // Unblocked
  } else {
    return true; // Blocked
  }
}

Point CursorUpdates::getCurPos()
{
  AutoLock al(&m_curPosLocMut);
  return m_cursorPos;
}

::int_rectangle CursorUpdates::getBackgroundRect()
{
  AutoLock al(&m_curPosLocMut);
  ::int_rectangle rect(m_shapeBackground.getDimension());
  rect.set_top_left(m_backgroundPos.x, m_backgroundPos.y);
  return rect;
}

void CursorUpdates::updateCursorShape(const CursorShape *curShape)
{
  AutoLock al(&m_curPosLocMut);
  m_cursorShape.clone(curShape);
}

void CursorUpdates::extractCursorShape(CursorShape *curShape)
{
  AutoLock al(&m_curPosLocMut);
  curShape->clone(&m_cursorShape);
}
