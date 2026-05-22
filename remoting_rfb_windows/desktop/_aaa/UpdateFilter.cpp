// Copyright (C) 2008,2009,2010,2011,2012 GlavSoft LLC.
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
#include "UpdateFilter.h"
#include "subsystem/_common_header.h"

namespace remoting_windows
{


   static const int BLOCK_SIZE = 32;

   UpdateFilter::UpdateFilter(::remoting_rfb::ScreenDriver *screenDriver, ::innate_subsystem::Framebuffer *pframebuffer,
                              lockable_critical_section *framebufferCriticalSection, ::subsystem::LogWriter * plogwriter) :
       m_pscreendriver(screenDriver), m_pframebuffer(pframebuffer), m_pcriticalsectionFramebuffer(framebufferCriticalSection),
       m_grabOptimizator(plogwriter), m_plogwriter = plogwriter;
   {
   }

   UpdateFilter::~UpdateFilter() {}

   void UpdateFilter::filter(UpdateContainer & updatecontainer)
   {
      m_plogwriter->debug("UpdateFilter::filter()");
      critical_section_lock al(m_pcriticalsectionFramebuffer);

      ::innate_subsystem::Framebuffer *pframebufferScreen = m_pscreendriver->getScreenBuffer();

      // Checking for buffers equal
      m_plogwriter->debug("UpdateFilter::filter : Checking for buffers equal");
      if (!pframebufferScreen->isEqualTo(m_pframebuffer))
      {
         return;
      }

      Region toCheck = updatecontainer.m_regionChanged;
      toCheck.add(&updatecontainer.m_regionCopied);
      toCheck.add(&updatecontainer.m_regionVideo);

      ::int_rectangle_array_base rectanglea;
      ::int_rectangle_array_base::iterator iRect;

      // Reproduce CopyRect operations in m_pframebuffer->
      m_plogwriter->debug("UpdateFilter::filter : Reproduce CopyRect operations in m_pframebuffer");
      updatecontainer.m_regionCopied.getRects(&rectanglea);
      ::i32_point *src = &updatecontainer.m_pointCopySource;
      for (iRect = rectanglea.begin(); iRect < rectanglea.end(); iRect++)
      {
         m_pframebuffer->move(&(*iRect), src->x, src->y);
      }


      toCheck.getRects(&rectanglea);
      // Grabbing
      m_plogwriter->debug("grabbing region, {} rectangles", (int)rectanglea.size());
      ProcessorTimes pt1 = m_plogwriter->checkPoint("grabbing region");
      try
      {
         m_grabOptimizator.grab(&toCheck, m_pscreendriver);
      }
      catch (...)
      {
         return;
      }
      ProcessorTimes pt2 = m_plogwriter->checkPoint("end of grabbing region");

      toCheck.getRects(&rectanglea);
      double area = 0.0;
      for (iRect = rectanglea.begin(); iRect < rectanglea.end(); iRect++)
      {
         area += (*iRect).area();
      }
      area /= 1000000.0; // in millions of pixels
      double dt = pt2.wall.getTime(); // in milliseconds
      m_plogwriter->debug(
         "Before grabbing region %f processor Mcycles, %f process time, %f kernel time, %f wall clock time ",
         pt1.cycle / 1000000., pt1.process, pt1.kernel, pt1.wall.getTime());
      m_plogwriter->debug("After grabbing region Mpoint grabbed: %f for %f processor Mcycles. %f ms", area,
                          pt2.cycle / 1000000., dt);
      m_plogwriter->debug("After grabbing region %f process time, %f kernel time, %f wall clock time", pt2.process,
                          pt2.kernel, dt);

      m_plogwriter->debug("end of grabbing region");

      // Filtering
      pt1 = m_plogwriter->checkPoint("filtering changed");
      updatecontainer.m_regionChanged.clear();
      ::i32_rectangle rectangle;
      for (iRect = rectanglea.begin(); iRect < rectanglea.end(); iRect++)
      {
         rectangle = &(*iRect);
         getChangedRegion(&updatecontainer.m_regionChanged, rectangle);
      }

      // Copy actually changed pixels into m_pframebuffer->
      updatecontainer.m_regionChanged.getRects(&rectanglea);
      for (iRect = rectanglea.begin(); iRect < rectanglea.end(); iRect++)
      {
         rectangle = &(*iRect);
         m_pframebuffer->copyFrom(rectangle, pframebufferScreen, rectangle.left, rectangle.top);
      }
      pt2 = m_plogwriter->checkPoint("after filtering changed");
      dt = pt2.wall.getTime(); // in milliseconds
      m_plogwriter->debug(
         "Before filtering changed %f processor Mcycles, %f process time, %f kernel time, %f wall clock time ",
         pt1.cycle / 1000000., pt1.process, pt1.kernel, pt1.wall.getTime());
      m_plogwriter->debug("After filtering changed Mpoint filtered: %f for %f processor Mcycles. %f ms", area,
                          pt2.cycle / 1000000., dt);
      m_plogwriter->debug("After filtering changed %f process time, %f kernel time, %f wall clock time", pt2.process,
                          pt2.kernel, dt);
   }

   void UpdateFilter::getChangedRegion(Region & rgn, const ::i32_rectangle & rectangle)
   {

      const unsigned int bytesPerPixel = m_pframebuffer->getBytesPerPixel();
      const int bytes_per_scanline = (rectangle.right - rectangle.left) * bytesPerPixel;

      const int bytesPerRow = m_pframebuffer->getBytesPerRow();
      const int offset = rectangle.top * bytesPerRow + rectangle.left * bytesPerPixel;
      unsigned char *o_ptr = (unsigned char *)m_pframebuffer->getBuffer() + offset;
      unsigned char *n_ptr = (unsigned char *)m_pscreendriver->getScreenBuffer()->getBuffer() + offset;

      ::i32_rectangle new_rect = rectangle;

      // Fast processing for small rectangles
      if (rectangle.right - rectangle.left <= BLOCK_SIZE && rectangle.bottom - rectangle.top <= BLOCK_SIZE)
      {
         for (int y = rectangle.top; y < rectangle.bottom; y++)
         {
            if (memcmp(o_ptr, n_ptr, bytes_per_scanline) != 0)
            {
               new_rect.top = y;
               updateChangedSubRect(rgn, &new_rect);
               break;
            }
            o_ptr += bytesPerRow;
            n_ptr += bytesPerRow;
         }
         return;
      }

      // Process bigger rectangles
      new_rect.top = -1;
      for (int y = rectangle.top; y < rectangle.bottom; y++)
      {
         if (memcmp(o_ptr, n_ptr, bytes_per_scanline) != 0)
         {
            if (new_rect.top == -1)
            {
               new_rect.top = y;
            }
            // Skip a number of lines after a non-matched one
            int n = BLOCK_SIZE / 2 - 1;
            y += n;
            o_ptr += n * bytesPerRow;
            n_ptr += n * bytesPerRow;
         }
         else
         {
            if (new_rect.top != -1)
            {
               new_rect.bottom = y;
               updateChangedRect(rgn, &new_rect);
               new_rect.top = -1;
            }
         }
         o_ptr += bytesPerRow;
         n_ptr += bytesPerRow;
      }
      if (new_rect.top != -1)
      {
         new_rect.bottom = rectangle.bottom;
         updateChangedRect(rgn, &new_rect);
      }
   }

   void UpdateFilter::updateChangedRect(Region & rgn, const ::i32_rectangle & rectangle)
   {
      // Pass small rectangles directly to updateChangedSubRect
      if (rectangle.right - rectangle.left <= BLOCK_SIZE && rectangle.bottom - rectangle.top <= BLOCK_SIZE)
      {
         updateChangedSubRect(rgn, rectangle);
         return;
      }

      const unsigned int bytesPerPixel = m_pframebuffer->getBytesPerPixel();

      ::i32_rectangle new_rect;
      int x, y, ay;

      // Scan down the rectangle
      const int bytesPerRow = m_pframebuffer->getBytesPerRow();
      const int offset = rectangle.top * bytesPerRow + rectangle.left * bytesPerPixel;
      unsigned char *o_topleft_ptr = (unsigned char *)m_pframebuffer->getBuffer() + offset;
      unsigned char *n_topleft_ptr = (unsigned char *)m_pscreendriver->getScreenBuffer()->getBuffer() + offset;

      for (y = rectangle.top; y < rectangle.bottom; y += BLOCK_SIZE)
      {
         // Work out way down the bitmap
         unsigned char *o_row_ptr = o_topleft_ptr;
         unsigned char *n_row_ptr = n_topleft_ptr;

         const int blockbottom = min(y + BLOCK_SIZE, rectangle.bottom);
         new_rect.bottom = blockbottom;
         new_rect.left = -1;

         for (x = rectangle.left; x < rectangle.right; x += BLOCK_SIZE)
         {
            // Work our way across the row
            unsigned char *n_block_ptr = n_row_ptr;
            unsigned char *o_block_ptr = o_row_ptr;

            const unsigned int blockright = min(x + BLOCK_SIZE, rectangle.right);
            const unsigned int bytesPerBlockRow = (blockright - x) * bytesPerPixel;

            // Scan this block
            for (ay = y; ay < blockbottom; ay++)
            {
               if (memcmp(n_block_ptr, o_block_ptr, bytesPerBlockRow) != 0)
                  break;
               n_block_ptr += bytesPerRow;
               o_block_ptr += bytesPerRow;
            }
            if (ay < blockbottom)
            {
               // There were changes, so this block will need to be updated
               if (new_rect.left == -1)
               {
                  new_rect.left = x;
                  new_rect.top = ay;
               }
               else if (ay < new_rect.top)
               {
                  new_rect.top = ay;
               }
            }
            else
            {
               // No changes in this block, process previous changed blocks if any
               if (new_rect.left != -1)
               {
                  new_rect.right = x;
                  updateChangedSubRect(rgn, &new_rect);
                  new_rect.left = -1;
               }
            }

            o_row_ptr += bytesPerBlockRow;
            n_row_ptr += bytesPerBlockRow;
         }

         if (new_rect.left != -1)
         {
            new_rect.right = rectangle.right;
            updateChangedSubRect(rgn, &new_rect);
         }

         o_topleft_ptr += bytesPerRow * BLOCK_SIZE;
         n_topleft_ptr += bytesPerRow * BLOCK_SIZE;
      }
   }

   void UpdateFilter::updateChangedSubRect(Region & rgn, const ::i32_rectangle & rectangle)
   {
      const unsigned int bytesPerPixel = m_pframebuffer->getBytesPerPixel();
      int bytes_in_row = (rectangle.right - rectangle.left) * bytesPerPixel;
      int y, i;

      // Exclude unchanged scan lines at the bottom
      const int bytesPerRow = m_pframebuffer->getBytesPerRow();
      int offset = (rectangle.bottom - 1) * bytesPerRow + rectangle.left * bytesPerPixel;
      unsigned char *o_ptr = (unsigned char *)m_pframebuffer->getBuffer() + offset;
      unsigned char *n_ptr = (unsigned char *)m_pscreendriver->getScreenBuffer()->getBuffer() + offset;
      ::i32_rectangle rectangleFinal = rectangle;
      rectangleFinal.bottom = rectangle.top + 1;
      for (y = rectangle.bottom - 1; y > rectangle.top; y--)
      {
         if (memcmp(o_ptr, n_ptr, bytes_in_row) != 0)
         {
            rectangleFinal.bottom = y + 1;
            break;
         }
         n_ptr -= bytesPerRow;
         o_ptr -= bytesPerRow;
      }

      // Exclude unchanged pixels at left and right sides
      offset = rectangleFinal.top * bytesPerRow + rectangleFinal.left * bytesPerPixel;
      o_ptr = (unsigned char *)m_pframebuffer->getBuffer() + offset;
      n_ptr = (unsigned char *)m_pscreendriver->getScreenBuffer()->getBuffer() + offset;
      int left_delta = bytes_in_row - 1;
      int right_delta = 0;
      for (y = rectangleFinal.top; y < rectangleFinal.bottom; y++)
      {
         for (i = 0; i < bytes_in_row - 1; i++)
         {
            if (n_ptr[i] != o_ptr[i])
            {
               if (i < left_delta)
                  left_delta = i;
               break;
            }
         }
         for (i = bytes_in_row - 1; i > 0; i--)
         {
            if (n_ptr[i] != o_ptr[i])
            {
               if (i > right_delta)
                  right_delta = i;
               break;
            }
         }
         n_ptr += bytesPerRow;
         o_ptr += bytesPerRow;
      }
      rectangleFinal.right = rectangleFinal.left + right_delta / bytesPerPixel + 1;
      rectangleFinal.left += left_delta / bytesPerPixel;

      // Update the rectangle
      rgn->addRect(&rectangleFinal);
   }


} // namespace remoting_windows
namespace remoting_windows
{
