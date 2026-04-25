// Copyright (C) 2008,2009,2010,2011,2012 GlavSoft LLC.
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
#include "UpdateKeeper.h"

namespace remoting
{


   UpdateKeeper::UpdateKeeper() {}

   UpdateKeeper::UpdateKeeper(const ::int_rectangle &borderRect) { m_borderRect.set(borderRect); }

   UpdateKeeper::~UpdateKeeper(void) {}

   void UpdateKeeper::addChangedRegion(const Region *m_regionChanged)
   {
      critical_section_lock al(&m_updContLocMut);

      // FIXME: Calling subtract() function is correct if use
      // copy region instead of copy rectangle.
      // m_updateContainer.m_regionCopied.subtract(m_regionChanged);
      m_updateContainer.m_regionChanged.add(m_regionChanged);
      m_updateContainer.m_regionChanged.crop(m_borderRect);
   }

   void UpdateKeeper::addChangedRect(const ::int_rectangle &changedRect)
   {
      Region region(changedRect);
      addChangedRegion(&region);
   }

   void UpdateKeeper::addCopyRect(const ::int_rectangle &copyRect, const ::int_point &pointSource)
   {
      critical_section_lock al(&m_updContLocMut);

      if (copyRect.is_empty())
      {
         return;
      }

      Region *m_regionChanged = &m_updateContainer.m_regionChanged;
      Region *m_regionCopied = &m_updateContainer.m_regionCopied;
      ::int_point *m_pointCopySource = &m_updateContainer.m_pointCopySource;
      ::int_rectangle dstCopyRect(copyRect);

      // Create copy of copyRect in the source coordinates.
      ::int_rectangle srcCopyRect(copyRect);
      srcCopyRect.set_top_left(pointSource.x, pointSource.y);

      // Clipping dstCopyRect
      dstCopyRect = dstCopyRect.intersection(m_borderRect);
      // Correcting source coordinates
      srcCopyRect.left += dstCopyRect.left - copyRect.left;
      srcCopyRect.top += dstCopyRect.top - copyRect.top;
      srcCopyRect.right += dstCopyRect.right - copyRect.right;
      srcCopyRect.bottom += dstCopyRect.bottom - copyRect.bottom;
      // Clipping srcCopyRect
      ::int_rectangle dummySrcCopyRect(srcCopyRect);
      srcCopyRect = srcCopyRect.intersection(m_borderRect);
      // Correcting destination coordinates
      dstCopyRect.left += srcCopyRect.left - dummySrcCopyRect.left;
      dstCopyRect.top += srcCopyRect.top - dummySrcCopyRect.top;
      dstCopyRect.right += srcCopyRect.right - dummySrcCopyRect.right;
      dstCopyRect.bottom += srcCopyRect.bottom - dummySrcCopyRect.bottom;

      if (dstCopyRect.is_empty())
      {
         return;
      }

      m_pointCopySource->x = srcCopyRect.left;
      m_pointCopySource->y = srcCopyRect.top;

      // Adding difference between clipped dstCopyRect and original copyRect
      // to m_regionChanged. Because without update detectors this information
      // loses irretrievably.
      Region diff(copyRect);
      Region dstCopyRegion(dstCopyRect);
      diff.subtract(&dstCopyRegion);
      addChangedRegion(&diff);

      // Old m_regionCopied must be added to m_regionChanged - (?)
      if (!m_regionCopied->is_empty())
      {
         m_regionChanged->add(m_regionCopied);
         m_regionCopied->clear();
         addChangedRect(copyRect);
         return;
      }

      m_regionCopied->clear();
      m_regionCopied->addRect(dstCopyRect);

      // m_regionCopied must be substracted from m_regionChanged
      m_regionChanged->subtract(m_regionCopied);

      // Create region that is intersection of m_regionChanged and srcCopyRect.
      Region addonChangedRegion(srcCopyRect);
      addonChangedRegion.intersect(m_regionChanged);

      // Move addonChangedRegion and add it to m_regionChanged.
      addonChangedRegion.translate(dstCopyRect.left - m_pointCopySource->x, dstCopyRect.top - m_pointCopySource->y);
      m_regionChanged->add(&addonChangedRegion);

      // Clipping regions
      m_updateContainer.m_regionChanged.crop(m_borderRect);
      m_updateContainer.m_regionCopied.crop(m_borderRect);
   }

   void UpdateKeeper::setBorderRect(const ::int_rectangle &borderRect)
   {
      critical_section_lock al(&m_updContLocMut);
      m_borderRect = borderRect;
   }

   void UpdateKeeper::setScreenSizeChanged()
   {
      critical_section_lock al(&m_updContLocMut);
      m_updateContainer.m_bScreenSizeChanged = true;
   }

   void UpdateKeeper::setCursorPosChanged()
   {
      critical_section_lock al(&m_updContLocMut);
      m_updateContainer.m_bCursorPosChanged = true;
   }

   void UpdateKeeper::setCursorPos(const ::int_point &curPos)
   {
      critical_section_lock al(&m_updContLocMut);
      m_updateContainer.m_bCursorPosChanged = true;
      m_updateContainer.m_pointCursorPos = curPos;
   }

   void UpdateKeeper::setCursorShapeChanged()
   {
      critical_section_lock al(&m_updContLocMut);
      m_updateContainer.m_bCursorShapeChanged = true;
   }

   void UpdateKeeper::addUpdateContainer(const UpdateContainer *updateContainer)
   {
      critical_section_lock al(&m_updContLocMut);

      // FIXME: Use addCopyRegion instead of addCopyRect
      // Add copied region
      ::int_rectangle_array_base rects;
      ::int_rectangle_array_base::iterator iRect;
      updateContainer->m_regionCopied.getRectVector(&rects);
      size_t numRects = rects.size();
      if (numRects > 0)
      {
         iRect = rects.begin();
         addCopyRect((*iRect), updateContainer->m_pointCopySource);
      }

      // Add changed region
      addChangedRegion(&updateContainer->m_regionChanged);

      // Add video region
      m_updateContainer.m_regionVideo.add(&updateContainer->m_regionVideo);

      // Set other properties
      if (updateContainer->m_bScreenSizeChanged)
      {
         setScreenSizeChanged();
      }
      setCursorPos(updateContainer->m_pointCursorPos);
      if (updateContainer->m_bCursorPosChanged)
      {
         setCursorPos(updateContainer->m_pointCursorPos);
      }
      if (updateContainer->m_bCursorShapeChanged)
      {
         setCursorShapeChanged();
      }
   }

   void UpdateKeeper::getUpdateContainer(UpdateContainer *updCont)
   {
      critical_section_lock al(&m_updContLocMut);
      *updCont = m_updateContainer;
   }

   bool UpdateKeeper::checkForUpdates(const Region *region)
   {
      UpdateContainer updateContainer;
      getUpdateContainer(&updateContainer);

      Region resultRegion = updateContainer.m_regionChanged;
      resultRegion.add(&updateContainer.m_regionCopied);
      resultRegion.intersect(region);

      bool result = updateContainer.m_bCursorPosChanged || updateContainer.m_bCursorShapeChanged ||
                    updateContainer.m_bScreenSizeChanged || !resultRegion.is_empty();

      return result;
   }

   void UpdateKeeper::extract(UpdateContainer *updateContainer)
   {
      {
         critical_section_lock al(&m_updContLocMut);

         // Clipping regions
         m_updateContainer.m_regionChanged.crop(m_borderRect);
         m_updateContainer.m_regionCopied.crop(m_borderRect);

         *updateContainer = m_updateContainer;
         m_updateContainer.clear();
      }
      {
         critical_section_lock al(&m_exclRegLocMut);
         updateContainer->m_regionChanged.subtract(&m_excludedRegion);
         updateContainer->m_regionCopied.subtract(&m_excludedRegion);
      }
   }

   void UpdateKeeper::setExcludedRegion(const Region *excludedRegion)
   {
      critical_section_lock al(&m_exclRegLocMut);

      if (excludedRegion == 0)
      {
         m_excludedRegion.clear();
      }
      else
      {
         m_excludedRegion = *excludedRegion;
      }
   }


} // namespace remoting
 


