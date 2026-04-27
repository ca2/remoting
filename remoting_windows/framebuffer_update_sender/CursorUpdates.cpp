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
//#include "subsystem/thread/lockable_critical_section.h"

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


   void CursorUpdates::update(const EncodeOptions *encodeOptions, UpdateContainer & updatecontainer, bool fullRegReq,
                              const ::int_rectangle &rectangleViewport, bool shareOnlyApp, const Region & regionShareApp,
                              ::innate_subsystem::Framebuffer *pframebuffer, CursorShape *cursorShape)
   {
      // Check cursor events. If they are outside of shared region then ignore they.
      if (shareOnlyApp)
      {
         bool inside = regionShareApp.isPointInside(updatecontainer.m_pointCursorPos);
         if (!inside)
         {
            updatecontainer.m_bCursorPosChanged = false;
            updatecontainer.m_bCursorShapeChanged = false;
         }
      }

      bool richEnabled = encodeOptions->richCursorEnabled();
      bool posEnabled = encodeOptions->pointerPosEnabled();

      bool posChanged = updatecontainer.m_bCursorPosChanged;
      // The posChanged and updatecontainer.m_bCursorPosChanged flags has different
      // purpose. The posChanged will be always raised if pointPosition real changed
      // but the updatecontainer.m_bCursorPosChanged flag can be raised only cursor pos
      // is not blocked.
      if (posChanged)
      {
         bool cursorPosBlockingIsIgnored = !richEnabled;
         posChanged = checkCursorPos(updatecontainer, rectangleViewport, cursorPosBlockingIsIgnored);
      }

      if (!richEnabled && !posEnabled)
      {
         // Draw the shape on the frame buffer.
         m_plogwriter->debug("Draw the shape of cursor on the frame buffer.");
         drawCursor(updatecontainer, pframebuffer);
      }

      bool initShapeByZeroIsNeeded = false;
      if (richEnabled && !posEnabled)
      {
         bool methodWasChanged = false;
         if (updatecontainer.m_bCursorPosChanged)
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
               restoreFramebuffer(pframebuffer);
               ::int_rectangle backgroundRect = getBackgroundRect();
               updatecontainer.m_regionChanged.addRect(backgroundRect);
            }
         }
         if (m_isDrawCursorMethod)
         {
            m_plogwriter->debug("Draw the shape of cursor on the frame buffer (DrawCursorMethod)");
            drawCursor(updatecontainer, pframebuffer);
            updatecontainer.m_bCursorShapeChanged = methodWasChanged;
         }
         else
         {
            updatecontainer.m_bCursorShapeChanged = updatecontainer.m_bCursorShapeChanged || methodWasChanged;
         }
      }

      if (!richEnabled || !posEnabled)
      {
         m_plogwriter->debug("Clearing m_bCursorPosChanged"
                             " (RichCursor or PointerPos are not requested)");
         updatecontainer.m_bCursorPosChanged = false;
      }
      else if (fullRegReq)
      {
         m_plogwriter->debug("Raising m_bCursorPosChanged (full region requested)");
         // ignore cursor pointPosition changing blocking on full request
         checkCursorPos(updatecontainer, rectangleViewport, true);
         updatecontainer.m_bCursorPosChanged = true;
      }
      if (!richEnabled)
      {
         m_plogwriter->debug("Clearing m_bCursorShapeChanged (RichCursor disabled)");
         updatecontainer.m_bCursorShapeChanged = false;
      }
      else if (fullRegReq)
      {
         m_plogwriter->debug("Raising m_bCursorShapeChanged (RichCursor enabled"
                             " and full region requested)");
         updatecontainer.m_bCursorShapeChanged = true;
      }
      if (updatecontainer.m_bCursorShapeChanged)
      {
         extractCursorShape(cursorShape);
         if (initShapeByZeroIsNeeded)
         {
            cursorShape->resetToEmpty();
         }
      }
   }

   void CursorUpdates::restoreFramebuffer(::innate_subsystem::Framebuffer *pframebuffer)
   {
      critical_section_lock al(&m_criticalsectionCurPosLoc);
      ::int_rectangle rectangleTarget = m_shapeBackground.getDimension();
      rectangleTarget.set_top_left(m_pointBackground.x, m_pointBackground.y);
      pframebuffer->copyFrom(rectangleTarget, &m_shapeBackground, 0, 0);
      // m_shapeBackground.setDimension(&::int_size(0, 0));
   }

   void CursorUpdates::drawCursor(UpdateContainer & updatecontainer, ::innate_subsystem::Framebuffer *pframebuffer)
   {
      critical_section_lock al(&m_criticalsectionCurPosLoc);
      // Add previous background rectangle to the changed region.
      ::int_rectangle rectangle(m_shapeBackground.getDimension());
      rectangle.set_top_left(m_pointBackground.x, m_pointBackground.y);
      updatecontainer.m_regionChanged.addRect(rectangle);
      // Keep the current background rectangle.
      ::int_point pointHotspot = m_cursorShape.getHotSpot();
      m_pointBackground.set(m_cursorPos.x - pointHotspot.x, m_cursorPos.y - pointHotspot.y);
      m_shapeBackground.setProperties(m_cursorShape.getDimension(), m_cursorShape.getPixelFormat());
      // Keep background under cursor shape to can reconstruct full image.
      m_shapeBackground.copyFrom(pframebuffer, m_pointBackground.x, m_pointBackground.y);
      // Draw the cursor shape on the frame buffer
      rectangle.set(m_cursorShape.getDimension());
      rectangle.set_top_left(m_pointBackground.x, m_pointBackground.y);

      pframebuffer->overlay(rectangle, m_cursorShape.getPixels(), 0, 0, m_cursorShape.getMask());
   }

   bool CursorUpdates::checkCursorPos(UpdateContainer & updatecontainer, const ::int_rectangle &rectangleViewport,
                                      bool curPosBlockingIsIgnored)
   {
      critical_section_lock al(&m_criticalsectionCurPosLoc);
      auto cursorPos = updatecontainer.m_pointCursorPos;
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
         updatecontainer.m_bCursorPosChanged = false;
      }
      else
      {
         m_cursorPos = cursorPos;
      }
      m_plogwriter->debug("CursorUpdates::checkCursorPos cursor pointPosition {:pn}, changed:{}", m_cursorPos, positionChanged);
      return positionChanged;
   }

   void CursorUpdates::blockCursorPosSending()
   {
      critical_section_lock al(&m_criticalsectionCurPosLoc);
      // Block cursor pos sending to a time interval.
      m_blockCurPosTime = ::time::now();
   }

   bool CursorUpdates::isCursorPosBlocked()
   {
      critical_section_lock al(&m_criticalsectionCurPosLoc);
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
      critical_section_lock al(&m_criticalsectionCurPosLoc);
      return m_cursorPos;
   }

   ::int_rectangle CursorUpdates::getBackgroundRect()
   {
      critical_section_lock al(&m_criticalsectionCurPosLoc);
      ::int_rectangle rectangle(m_shapeBackground.getDimension());
      rectangle.set_top_left(m_pointBackground.x, m_pointBackground.y);
      return rectangle;
   }

   void CursorUpdates::updateCursorShape(const CursorShape *curShape)
   {
      critical_section_lock al(&m_criticalsectionCurPosLoc);
      m_cursorShape.clone(curShape);
   }

   void CursorUpdates::extractCursorShape(CursorShape *curShape)
   {
      critical_section_lock al(&m_criticalsectionCurPosLoc);
      curShape->clone(&m_cursorShape);
   }


} // namespace remoting
 