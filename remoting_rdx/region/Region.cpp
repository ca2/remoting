// Copyright (C) 2010,2011,2012 GlavSoft LLC.
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
#include "Region.h"
#include "acme/prototype/geometry2d/rectangle.h"
#include "acme/prototype/geometry2d/rectangle_array.h"

namespace remoting_rfb
{
   Region::Region()
   {
      miRegionInit(&m_reg, NullBox, 0);
   }

   // FIXME: Make BoxRec and ::i32_rectangle identical to get rid of conversions.
   Region::Region(const ::i32_rectangle & rectangle)
   {
      if (!rectangle.is_empty()) {
         BoxRec box;
         box.x1 = rectangle.left;
         box.x2 = rectangle.right;
         box.y1 = rectangle.top;
         box.y2 = rectangle.bottom;
         miRegionInit(&m_reg, &box, 0);
      } else {
         miRegionInit(&m_reg, NullBox, 0);
      }
   }

   Region::Region(const Region &src)
   {
      miRegionInit(&m_reg, NullBox, 0);
      set(src);
   }
   Region::Region(Region &&src)
   {
      __miRegionInitTransfer(&m_reg, &src.m_reg);

   }

   Region::~Region()
   {
      miRegionUninit(&m_reg);
   }

   void Region::clear()
   {
      miRegionEmpty(&m_reg);
   }

   void Region::set(const Region & src)
   {
      miRegionCopy(&m_reg, (RegionPtr)&src.m_reg);
   }

   Region & Region::operator=(const Region &src)
   {
      if (this != &src)
      {
         set(src);
      }
      return *this;
   }

   Region & Region::operator=(Region &&src)
   {

      if (this != &src)
      {
         __miRegionTransfer(&m_reg, &src.m_reg);
      }

      return *this;
   }

   void Region::addRect(const ::i32_rectangle &  rectangle)
   {
      if (!rectangle.is_empty()) {
         Region temp(rectangle);
         add(temp);
      }
   }

   void Region::translate(int dx, int dy)
   {
      miTranslateRegion(&m_reg, dx, dy);
   }

   // void Region::add(const Region & other)
   // {
   //    miUnion(&m_reg, &m_reg, (RegionPtr)&other.m_reg);
   // }

   void Region::add(const Region &other)
   {
      miUnion(&m_reg, &m_reg, (RegionPtr)&other.m_reg);
   }

   void Region::subtract(const Region & other)
   {
      miSubtract(&m_reg, &m_reg, (RegionPtr)&other.m_reg);
   }

   void Region::intersect(const Region & other)
   {
      miIntersect(&m_reg, &m_reg, (RegionPtr)&other.m_reg);
   }

   void Region::crop(const ::i32_rectangle &  rectangle)
   {
      Region temp(rectangle);
      intersect(temp);
   }

   bool Region::is_empty() const
   {
      return (miRegionNotEmpty((RegionPtr)&m_reg) == false);
   }

   bool Region::isPointInside(int x, int y) const
   {
      BoxRec stubBox; // Ignore returning rectangle.
      return !!miPointInRegion((RegionPtr)&m_reg, x, y, &stubBox);
   }

   bool Region::_equals(const Region & other) const
   {
      // Handle a special case when both regions are empty.
      // Such regions may be considered different by miRegionsEqual().
      if (this->is_empty() && other.is_empty()) {
         return true;
      }

      return (miRegionsEqual((RegionPtr)&m_reg,
                             (RegionPtr)&other.m_reg) == true);
   }

   // FIXME: Optimize, make BoxRec and ::i32_rectangle identical to get rid of conversions.
   void Region::_getRects(::int_rectangle_array_base & rectanglea) const
   {

      rectanglea.clear();

      const BoxRec *boxPtr = REGION_RECTS(&m_reg);
      long numRects = REGION_NUM_RECTS(&m_reg);
      rectanglea.set_size(numRects);
      for (long i = 0; i < numRects; i++)
      {
         rectanglea[i].set(boxPtr[i].x1, boxPtr[i].y1, boxPtr[i].x2, boxPtr[i].y2);
      }

   }

   // FIXME: Optimize, make BoxRec and ::i32_rectangle identical to get rid of conversions.
   ::int_rectangle_array_base Region::getRects() const
   {

      ::int_rectangle_array_base recta;
      //dst->clear();

      const BoxRec *boxPtr = REGION_RECTS(&m_reg);
      long numRects = REGION_NUM_RECTS(&m_reg);
      for (long i = 0; i < numRects; i++) {
         ::i32_rectangle rectangle(boxPtr[i].x1, boxPtr[i].y1, boxPtr[i].x2, boxPtr[i].y2);
         recta.add(rectangle);
      }
      return recta;
   }

   size_t Region::getCount() const
   {
      return REGION_NUM_RECTS(&m_reg);
   }

   ::i32_rectangle Region::getBounds() const
   {
      const BoxRec *boxPtr = REGION_EXTENTS(&m_reg);
      return ::i32_rectangle(boxPtr->x1, boxPtr->y1, boxPtr->x2, boxPtr->y2);
   }
} // namespace remoting_rfb