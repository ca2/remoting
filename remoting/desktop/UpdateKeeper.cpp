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
#include "UpdateKeeper.h"

namespace remoting
{


   UpdateKeeper::UpdateKeeper() {}

   UpdateKeeper::UpdateKeeper(const ::i32_rectangle &borderRect) { m_rectangleBorder.set(borderRect); }

   UpdateKeeper::~UpdateKeeper(void) {}

   void UpdateKeeper::addChangedRegion(const Region & regionChanged)
   {
      critical_section_lock al(&m_criticalsectionUpdateContainer);

      // FIXME: Calling subtract() function is correct if use
      // copy region instead of copy rectangle.
      // m_updatecontainer.m_regionCopied.subtract(m_regionChanged);
      m_updatecontainer.m_regionChanged.add(regionChanged);
      m_updatecontainer.m_regionChanged.crop(m_rectangleBorder);
   }

   void UpdateKeeper::addChangedRect(const ::i32_rectangle &rectangleChanged)
   {
      Region region(rectangleChanged);
      addChangedRegion(region);
   }

   void UpdateKeeper::addCopyRect(const ::i32_rectangle &rectangleCopy, const ::i32_point &pointSource)
   {
      critical_section_lock al(&m_criticalsectionUpdateContainer);

      if (rectangleCopy.is_empty())
      {
         return;
      }

      Region & regionChanged = m_updatecontainer.m_regionChanged;
      Region & regionCopied = m_updatecontainer.m_regionCopied;
      ::i32_point & pointCopySource = m_updatecontainer.m_pointCopySource;
      ::i32_rectangle rectangleTargetCopy(rectangleCopy);

      // Create copy of rectangleCopy in the source coordinates.
      ::i32_rectangle rectangleSourceCopy(rectangleCopy);
      rectangleSourceCopy.set_top_left(pointSource.x, pointSource.y);

      // Clipping rectangleTargetCopy
      rectangleTargetCopy = rectangleTargetCopy.intersection(m_rectangleBorder);
      // Correcting source coordinates
      rectangleSourceCopy.left += rectangleTargetCopy.left - rectangleCopy.left;
      rectangleSourceCopy.top += rectangleTargetCopy.top - rectangleCopy.top;
      rectangleSourceCopy.right += rectangleTargetCopy.right - rectangleCopy.right;
      rectangleSourceCopy.bottom += rectangleTargetCopy.bottom - rectangleCopy.bottom;
      // Clipping rectangleSourceCopy
      ::i32_rectangle rectangleDummySourceCopy(rectangleSourceCopy);
      rectangleSourceCopy = rectangleSourceCopy.intersection(m_rectangleBorder);
      // Correcting destination coordinates
      rectangleTargetCopy.left += rectangleSourceCopy.left - rectangleDummySourceCopy.left;
      rectangleTargetCopy.top += rectangleSourceCopy.top - rectangleDummySourceCopy.top;
      rectangleTargetCopy.right += rectangleSourceCopy.right - rectangleDummySourceCopy.right;
      rectangleTargetCopy.bottom += rectangleSourceCopy.bottom - rectangleDummySourceCopy.bottom;

      if (rectangleTargetCopy.is_empty())
      {
         return;
      }

      pointCopySource.x = rectangleSourceCopy.left;
      pointCopySource.y = rectangleSourceCopy.top;

      // Adding difference between clipped rectangleTargetCopy and original rectangleCopy
      // to m_regionChanged. Because without update detectors this information
      // loses irretrievably.
      Region diff(rectangleCopy);
      Region dstCopyRegion(rectangleTargetCopy);
      diff.subtract(dstCopyRegion);
      addChangedRegion(diff);

      // Old m_regionCopied must be added to m_regionChanged - (?)
      if (!regionCopied.is_empty())
      {
         regionChanged.add(regionCopied);
         regionCopied.clear();
         addChangedRect(rectangleCopy);
         return;
      }

      regionCopied.clear();
      regionCopied.addRect(rectangleTargetCopy);

      // m_regionCopied must be substracted from m_regionChanged
      regionChanged.subtract(regionCopied);

      // Create region that is intersection of m_regionChanged and rectangleSourceCopy.
      Region addonChangedRegion(rectangleSourceCopy);
      addonChangedRegion.intersect(regionChanged);

      // Move addonChangedRegion and add it to m_regionChanged.
      addonChangedRegion.translate(rectangleTargetCopy.left - pointCopySource.x, rectangleTargetCopy.top - pointCopySource.y);
      regionChanged.add(addonChangedRegion);

      // Clipping regions
      m_updatecontainer.m_regionChanged.crop(m_rectangleBorder);
      m_updatecontainer.m_regionCopied.crop(m_rectangleBorder);
   }

   void UpdateKeeper::setBorderRect(const ::i32_rectangle &borderRect)
   {
      critical_section_lock al(&m_criticalsectionUpdateContainer);
      m_rectangleBorder = borderRect;
   }

   void UpdateKeeper::setScreenSizeChanged()
   {
      critical_section_lock al(&m_criticalsectionUpdateContainer);
      m_updatecontainer.m_bScreenSizeChanged = true;
   }

   void UpdateKeeper::setCursorPosChanged()
   {
      critical_section_lock al(&m_criticalsectionUpdateContainer);
      m_updatecontainer.m_bCursorPosChanged = true;
   }

   void UpdateKeeper::setCursorPos(const ::i32_point &curPos)
   {
      critical_section_lock al(&m_criticalsectionUpdateContainer);
      m_updatecontainer.m_bCursorPosChanged = true;
      m_updatecontainer.m_pointCursorPos = curPos;
   }

   void UpdateKeeper::setCursorShapeChanged()
   {
      critical_section_lock al(&m_criticalsectionUpdateContainer);
      m_updatecontainer.m_bCursorShapeChanged = true;
   }

   void UpdateKeeper::addUpdateContainer(const UpdateContainer & updatecontainer)
   {
      critical_section_lock al(&m_criticalsectionUpdateContainer);

      // FIXME: Use addCopyRegion instead of addCopyRect
      // Add copied region
      //::int_rectangle_array_base rectanglea;
      ::int_rectangle_array_base::iterator iRect;
      auto rectanglea = updatecontainer.m_regionCopied.getRects();
      size_t numRects = rectanglea.size();
      if (numRects > 0)
      {
         iRect = rectanglea.begin();
         addCopyRect((*iRect), updatecontainer.m_pointCopySource);
      }

      // Add changed region
      addChangedRegion(updatecontainer.m_regionChanged);

      // Add video region
      m_updatecontainer.m_regionVideo.add(updatecontainer.m_regionVideo);

      // Set other properties
      if (updatecontainer.m_bScreenSizeChanged)
      {
         setScreenSizeChanged();
      }
      setCursorPos(updatecontainer.m_pointCursorPos);
      if (updatecontainer.m_bCursorPosChanged)
      {
         setCursorPos(updatecontainer.m_pointCursorPos);
      }
      if (updatecontainer.m_bCursorShapeChanged)
      {
         setCursorShapeChanged();
      }
   }

   UpdateContainer UpdateKeeper::getUpdateContainer()
   {
      critical_section_lock al(&m_criticalsectionUpdateContainer);
      return m_updatecontainer;
   }

   bool UpdateKeeper::checkForUpdates(const Region & region)
   {
      //UpdateContainer updatecontainer;
      auto updatecontainer = getUpdateContainer();

      Region resultRegion = updatecontainer.m_regionChanged;
      resultRegion.add(updatecontainer.m_regionCopied);
      resultRegion.intersect(region);

      bool result = updatecontainer.m_bCursorPosChanged || updatecontainer.m_bCursorShapeChanged ||
                    updatecontainer.m_bScreenSizeChanged || !resultRegion.is_empty();

      return result;
   }

   UpdateContainer UpdateKeeper::extract()
   {
      critical_section_lock al1(&m_criticalsectionUpdateContainer);

      // Clipping regions
      m_updatecontainer.m_regionChanged.crop(m_rectangleBorder);
      m_updatecontainer.m_regionCopied.crop(m_rectangleBorder);

      auto updatecontainer =::transfer(m_updatecontainer);
      //m_updatecontainer.clear();
      al1.unlock();

      critical_section_lock al2(&m_criticalsectionExclRegLoc);
      updatecontainer.m_regionChanged.subtract(m_regionExcluded);
      updatecontainer.m_regionCopied.subtract(m_regionExcluded);

      return ::transfer(updatecontainer);

   }


   void UpdateKeeper::setExcludedRegion(const Region & regionExcluded)
   {
      critical_section_lock al(&m_criticalsectionExclRegLoc);

      m_regionExcluded = regionExcluded;

   }

   void UpdateKeeper::clearExcludedRegion()
   {
      critical_section_lock al(&m_criticalsectionExclRegLoc);

      m_regionExcluded.clear();

   }

} // namespace remoting
 


