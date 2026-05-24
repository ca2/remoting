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
#include "acme/platform/ProfileLogger.h"


namespace remoting_rfb
{


   static const int BLOCK_SIZE = 32;

   // UpdateFilter::UpdateFilter(ScreenDriver *screenDriver, ::innate_subsystem::Framebuffer *pframebuffer,
   //                            lockable_critical_section *framebufferCriticalSection, ::subsystem::LogWriter * plogwriter) :
   //     m_pscreendriver(screenDriver), m_pframebuffer(pframebuffer), m_pcriticalsectionFramebuffer(framebufferCriticalSection),
   //     m_grabOptimizator(plogwriter), m_plogwriter(plogwriter)
   // {
   // }

   UpdateFilter::UpdateFilter() :
    m_pcriticalsectionFramebuffer(nullptr)
   {
   }

   UpdateFilter::~UpdateFilter() {}


   void UpdateFilter::initialize_update_filter(ScreenDriver *pscreendriver, ::innate_subsystem::Framebuffer *pframebuffer,
       lockable_critical_section * pcriticalsectionFramebuffer, ::subsystem::LogWriter * plogwriter)
   {

      m_pscreendriver = pscreendriver;
      m_pframebuffer = pframebuffer;
      m_pcriticalsectionFramebuffer = pcriticalsectionFramebuffer;
      m_plogwriter = plogwriter;

   }


   void UpdateFilter::filter(UpdateContainer & updatecontainer)
   {
      m_plogwriter->debug("UpdateFilter::filter()");

if (defer_construct_newø(m_pgraboptimizator))
{
   m_pgraboptimizator->initialize_grab_optimizator(m_plogwriter);

}
      critical_section_lock al(m_pcriticalsectionFramebuffer);
      ::innate_subsystem::Framebuffer *pframebufferScreen = m_pscreendriver->getScreenBuffer();

      // Checking for buffers equal
      m_plogwriter->debug("UpdateFilter::filter : Checking for buffers equal");
      if (!pframebufferScreen->isEqualTo(m_pframebuffer))
      {
         return;
      }

      Region toCheck = updatecontainer.m_regionChanged;
      toCheck.add(updatecontainer.m_regionCopied);
      toCheck.add(updatecontainer.m_regionVideo);

      //::int_rectangle_array_base rectanglea;
      //::int_rectangle_array_base::iterator iRect;

      // Reproduce CopyRect operations in m_pframebuffer->
      m_plogwriter->debug("UpdateFilter::filter : Reproduce CopyRect operations in m_pframebuffer");
      auto rectanglea = updatecontainer.m_regionCopied.getRects();
      auto pointSource = updatecontainer.m_pointCopySource;
      //for (iRect = rectanglea.begin(); iRect < rectanglea.end(); iRect++)
      for (auto & rectangle : rectanglea)
      {
         m_pframebuffer->move(rectangle, pointSource.x, pointSource.y);
      }


      rectanglea = toCheck.getRects();
      // Grabbing
      m_plogwriter->debug("grabbing region, {} rectangles", (int)rectanglea.size());
      auto processortimes1 = ProfileLogger().checkPoint("grabbing region");
      try
      {
         m_pgraboptimizator->grab(toCheck, m_pscreendriver);
      }
      catch (...)
      {
         return;
      }
      auto processortimes2 = ProfileLogger().checkPoint("end of grabbing region");

      rectanglea = toCheck.getRects();
      double area = rectanglea.total_area();
      // for (iRect = rectanglea.begin(); iRect < rectanglea.end(); iRect++)
      // {
      //    area += (*iRect).area();
      // }
      area /= 1000000.0; // in millions of pixels
      double dt = processortimes2.m_time.floating_millisecond(); // in milliseconds
      m_plogwriter->debug(
         "Before grabbing region %f processor Mcycles, %f process time, %f kernel time, %f wall clock time ",
         processortimes1.m_cycle / 1000000., processortimes1.m_process, processortimes1.m_kernel, processortimes1.m_time.floating_millisecond());
      m_plogwriter->debug("After grabbing region Mpoint grabbed: %f for %f processor Mcycles. %f ms", area,
                          processortimes2.m_cycle / 1000000., dt);
      m_plogwriter->debug("After grabbing region %f process time, %f kernel time, %f wall clock time", processortimes2.m_process,
                          processortimes2.m_kernel, dt);

      m_plogwriter->debug("end of grabbing region");

      // Filtering
      processortimes1 = ProfileLogger().checkPoint("filtering changed");
      updatecontainer.m_regionChanged.clear();
      //::i32_rectangle rectangle;
      for (auto rectangle : rectanglea)
      {
         //rectangle = &(*iRect);
         getChangedRegion(updatecontainer.m_regionChanged, rectangle);
      }

      // Copy actually changed pixels into m_pframebuffer->
      rectanglea  = updatecontainer.m_regionChanged.getRects();
      for (auto rectangle :rectanglea)
      {
         //rectangle = &(*iRect);
         m_pframebuffer->copyFrom(rectangle, pframebufferScreen, rectangle.left, rectangle.top);
      }
      processortimes2 = ProfileLogger().checkPoint("after filtering changed");
      dt = processortimes2.m_time.floating_millisecond(); // in milliseconds
      m_plogwriter->debug(
         "Before filtering changed %f processor Mcycles, %f process time, %f kernel time, %f wall clock time ",
         processortimes1.m_cycle / 1000000., processortimes1.m_process, processortimes1.m_kernel, processortimes1.m_time.floating_millisecond());
      m_plogwriter->debug("After filtering changed Mpoint filtered: %f for %f processor Mcycles. %f ms", area,
                          processortimes2.m_cycle / 1000000., dt);
      m_plogwriter->debug("After filtering changed %f process time, %f kernel time, %f wall clock time", processortimes2.m_process,
                          processortimes2.m_kernel, dt);
   }

   void UpdateFilter::getChangedRegion(Region & region, const ::i32_rectangle & rectangle)
   {

      const ::u32 bytesPerPixel = m_pframebuffer->getBytesPerPixel();
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
               updateChangedSubRect(region, new_rect);
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
               updateChangedRect(region, new_rect);
               new_rect.top = -1;
            }
         }
         o_ptr += bytesPerRow;
         n_ptr += bytesPerRow;
      }
      if (new_rect.top != -1)
      {
         new_rect.bottom = rectangle.bottom;
         updateChangedRect(region, new_rect);
      }
   }

   void UpdateFilter::updateChangedRect(Region & region, const ::i32_rectangle & rectangle)
   {
      // Pass small rectangles directly to updateChangedSubRect
      if (rectangle.right - rectangle.left <= BLOCK_SIZE && rectangle.bottom - rectangle.top <= BLOCK_SIZE)
      {
         updateChangedSubRect(region, rectangle);
         return;
      }

      const ::u32 bytesPerPixel = m_pframebuffer->getBytesPerPixel();

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

         const int blockbottom = minimum(y + BLOCK_SIZE, rectangle.bottom);
         new_rect.bottom = blockbottom;
         new_rect.left = -1;

         for (x = rectangle.left; x < rectangle.right; x += BLOCK_SIZE)
         {
            // Work our way across the row
            unsigned char *n_block_ptr = n_row_ptr;
            unsigned char *o_block_ptr = o_row_ptr;

            const ::u32 blockright = minimum(x + BLOCK_SIZE, rectangle.right);
            const ::u32 bytesPerBlockRow = (blockright - x) * bytesPerPixel;

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
                  updateChangedSubRect(region, new_rect);
                  new_rect.left = -1;
               }
            }

            o_row_ptr += bytesPerBlockRow;
            n_row_ptr += bytesPerBlockRow;
         }

         if (new_rect.left != -1)
         {
            new_rect.right = rectangle.right;
            updateChangedSubRect(region, new_rect);
         }

         o_topleft_ptr += bytesPerRow * BLOCK_SIZE;
         n_topleft_ptr += bytesPerRow * BLOCK_SIZE;
      }
   }

   void UpdateFilter::updateChangedSubRect(Region & region, const ::i32_rectangle & rectangle)
   {
      const ::u32 bytesPerPixel = m_pframebuffer->getBytesPerPixel();
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
      region.addRect(rectangleFinal);
   }


} // namespace remoting_rfb
