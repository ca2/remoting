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
//#include "subsystem/thread/critical_section.h"

namespace remoting
{


   CursorUpdates::CursorUpdates() :
       //m_blockCurPosTime(0), 
       m_isDrawCursorMethod(false)
   {
   }

   CursorUpdates::~CursorUpdates() {}


   void CursorUpdates::initialize_cursor_updates(subsystem::LogWriter *plogwriter)
   {

      m_plogwriter = plogwriter;
   }


   void CursorUpdates::update(const EncodeOptions *encodeOptions, UpdateContainer *pupdatecontainer, bool fullRegReq,
                              const ::int_rectangle &rectangleViewport, bool shareOnlyApp, const Region *shareAppRegion,
                              ::innate_subsystem::FrameBuffer *pframebuffer, CursorShape *cursorShape)
   {
      // Check cursor events. If they are outside of shared region then ignore they.
      if (shareOnlyApp)
      {
         bool inside = shareAppRegion->isPointInside(pupdatecontainer->m_pointCursorPos);
         if (!inside)
         {
            pupdatecontainer->m_bCursorPosChanged = false;
            pupdatecontainer->m_bCursorShapeChanged = false;
         }
      }

      bool richEnabled = encodeOptions->richCursorEnabled();
      bool posEnabled = encodeOptions->pointerPosEnabled();

      bool posChanged = pupdatecontainer->m_bCursorPosChanged;
      // The posChanged and pupdatecontainer->m_bCursorPosChanged flags has different
      // purpose. The posChanged will be always raised if position real changed
      // but the pupdatecontainer->m_bCursorPosChanged flag can be raised only cursor pos
      // is not blocked.
      if (posChanged)
      {
         bool cursorPosBlockingIsIgnored = !richEnabled;
         posChanged = checkCursorPos(pupdatecontainer, rectangleViewport, cursorPosBlockingIsIgnored);
      }

      if (!richEnabled && !posEnabled)
      {
         // Draw the shape on the frame buffer.
         m_plogwriter->debug("Draw the shape of cursor on the frame buffer.");
         drawCursor(pupdatecontainer, pframebuffer);
      }

      bool initShapeByZeroIsNeeded = false;
      if (richEnabled && !posEnabled)
      {
         bool methodWasChanged = false;
         if (pupdatecontainer->m_bCursorPosChanged)
         { // The move by the server
            methodWasChanged = !m_isDrawCursorMethod;
            if (methodWasChanged)
            {
               m_isDrawCursorMethod = true;
               // By cursor shape method will be sended zero shape.
               initShapeByZeroIsNeeded = true;
            }
         }
         else if (posChanged)
         { // The move by the client.
            methodWasChanged = m_isDrawCursorMethod;
            if (methodWasChanged)
            {
               m_isDrawCursorMethod = false;
               // Restore background under the cursor shape
               m_plogwriter->debug("Restore background under the cursor shape");
               restoreFrameBuffer(pframebuffer);
               ::int_rectangle backgroundRect = getBackgroundRect();
               pupdatecontainer->m_regionChanged.addRect(backgroundRect);
            }
         }
         if (m_isDrawCursorMethod)
         {
            m_plogwriter->debug("Draw the shape of cursor on the frame buffer (DrawCursorMethod)");
            drawCursor(pupdatecontainer, pframebuffer);
            pupdatecontainer->m_bCursorShapeChanged = methodWasChanged;
         }
         else
         {
            pupdatecontainer->m_bCursorShapeChanged = pupdatecontainer->m_bCursorShapeChanged || methodWasChanged;
         }
      }

      if (!richEnabled || !posEnabled)
      {
         m_plogwriter->debug("Clearing m_bCursorPosChanged"
                             " (RichCursor or PointerPos are not requested)");
         pupdatecontainer->m_bCursorPosChanged = false;
      }
      else if (fullRegReq)
      {
         m_plogwriter->debug("Raising m_bCursorPosChanged (full region requested)");
         // ignore cursor position changing blocking on full request
         checkCursorPos(pupdatecontainer, rectangleViewport, true);
         pupdatecontainer->m_bCursorPosChanged = true;
      }
      if (!richEnabled)
      {
         m_plogwriter->debug("Clearing m_bCursorShapeChanged (RichCursor disabled)");
         pupdatecontainer->m_bCursorShapeChanged = false;
      }
      else if (fullRegReq)
      {
         m_plogwriter->debug("Raising m_bCursorShapeChanged (RichCursor enabled"
                             " and full region requested)");
         pupdatecontainer->m_bCursorShapeChanged = true;
      }
      if (pupdatecontainer->m_bCursorShapeChanged)
      {
         extractCursorShape(cursorShape);
         if (initShapeByZeroIsNeeded)
         {
            cursorShape->resetToEmpty();
         }
      }
   }

   void CursorUpdates::restoreFrameBuffer(::innate_subsystem::FrameBuffer *pframebuffer)
   {
      critical_section_lock al(&m_curPosLocMut);
      ::int_rectangle dstRect = m_shapeBackground.getDimension();
      dstRect.set_top_left(m_pointBackground.x, m_pointBackground.y);
      pframebuffer->copyFrom(dstRect, &m_shapeBackground, 0, 0);
      // m_shapeBackground.setDimension(&::int_size(0, 0));
   }

   void CursorUpdates::drawCursor(UpdateContainer *pupdatecontainer, ::innate_subsystem::FrameBuffer *pframebuffer)
   {
      critical_section_lock al(&m_curPosLocMut);
      // Add previous background rectangle to the changed region.
      ::int_rectangle rect(m_shapeBackground.getDimension());
      rect.set_top_left(m_pointBackground.x, m_pointBackground.y);
      pupdatecontainer->m_regionChanged.addRect(rect);
      // Keep the current background rectangle.
      ::int_point hotSpot = m_cursorShape.getHotSpot();
      m_pointBackground.set(m_cursorPos.x - hotSpot.x, m_cursorPos.y - hotSpot.y);
      m_shapeBackground.setProperties(m_cursorShape.getDimension(), m_cursorShape.getPixelFormat());
      // Keep background under cursor shape to can reconstruct full image.
      m_shapeBackground.copyFrom(pframebuffer, m_pointBackground.x, m_pointBackground.y);
      // Draw the cursor shape on the frame buffer
      rect.set(m_cursorShape.getDimension());
      rect.set_top_left(m_pointBackground.x, m_pointBackground.y);

      pframebuffer->overlay(rect, m_cursorShape.getPixels(), 0, 0, m_cursorShape.getMask());
   }

   bool CursorUpdates::checkCursorPos(UpdateContainer *pupdatecontainer, const ::int_rectangle &rectangleViewport,
                                      bool curPosBlockingIsIgnored)
   {
      critical_section_lock al(&m_curPosLocMut);
      auto cursorPos = pupdatecontainer->m_pointCursorPos;
      cursorPos -= rectangleViewport.top_left();
      //cursorPos.y -= rectangleViewport.top;

      if (cursorPos.x < 0)
      {
         cursorPos.x = 0;
      }
      else if (cursorPos.x >= rectangleViewport.width())
      {
         cursorPos.x = rectangleViewport.width() - 1;
      }
      if (cursorPos.y < 0)
      {
         cursorPos.y = 0;
      }
      else if (cursorPos.y >= rectangleViewport.height())
      {
         cursorPos.y = rectangleViewport.height() - 1;
      }

      bool isBlocked = isCursorPosBlocked() && !curPosBlockingIsIgnored;
      bool positionChanged = cursorPos!= m_cursorPos;
      if (!positionChanged || isBlocked)
      {
         pupdatecontainer->m_bCursorPosChanged = false;
      }
      else
      {
         m_cursorPos = cursorPos;
      }
      m_plogwriter->debug("CursorUpdates::checkCursorPos cursor position {:pn}, changed:{}", m_cursorPos, positionChanged);
      return positionChanged;
   }

   void CursorUpdates::blockCursorPosSending()
   {
      critical_section_lock al(&m_curPosLocMut);
      // Block cursor pos sending to a time interval.
      m_blockCurPosTime = ::time::now();
   }

   bool CursorUpdates::isCursorPosBlocked()
   {
      critical_section_lock al(&m_curPosLocMut);
      if (m_blockCurPosTime.elapsed() > 1_s)
      {
         return false; // Unblocked
      }
      else
      {
         return true; // Blocked
      }
   }

   ::int_point CursorUpdates::getCurPos()
   {
      critical_section_lock al(&m_curPosLocMut);
      return m_cursorPos;
   }

   ::int_rectangle CursorUpdates::getBackgroundRect()
   {
      critical_section_lock al(&m_curPosLocMut);
      ::int_rectangle rect(m_shapeBackground.getDimension());
      rect.set_top_left(m_pointBackground.x, m_pointBackground.y);
      return rect;
   }

   void CursorUpdates::updateCursorShape(const CursorShape *curShape)
   {
      critical_section_lock al(&m_curPosLocMut);
      m_cursorShape.clone(curShape);
   }

   void CursorUpdates::extractCursorShape(CursorShape *curShape)
   {
      critical_section_lock al(&m_curPosLocMut);
      curShape->clone(&m_cursorShape);
   }


} // namespace remoting
 