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

#pragma once
//#include aaa_<vector>
//#include aaa_<list>

//#include "::i32_rectangle.h"



extern "C" {
#include "x11region.h"
}

namespace remoting
{
   /**
    * A Region is an area which can be represented by a set of rectangles with
    * integer coordinates. It maintains the ::list_base of rectangles such way that
    * individual rectangles never overlap. When a rectangle is added to a Region,
    * only its non-overlapping part will be actually added. Note that adding a
    * rectangle will not necessarily increment the number of rectangles by one.
    * On such addition, the underlying ::list_base of rectangles may change dramatically
    * and its length may increase, decrease or remain the same.
    */
   class CLASS_DECL_REMOTING Region {
   public:
      /**
       * Creates an empty region.
       */
      Region();
      /**
       * Creates a region from the given rectangle.
       * @param rectangle a pointer to the source rectangle.
       */
      Region(const ::i32_rectangle & rectangle);
      /**
       * Creates a copy of another region.
       * @param src a reference to the source region.
       */
      Region(const Region &src);
      Region(Region &&src);
      /**
       * The destructor.
       */
      virtual ~Region();

      /**
       * Clears the region so that it will consist of zero rectangles.
       */
      void clear();
      /**
       * Replaces this region with a copy of another region.
       * @param src a pointer to the source region.
       */
      void set(const Region &src);
      /**
       * Overriden assignment operator. Replaces this region with a copy of
       * another region.
       * @param src a reference to the source region.
       */
      Region & operator=(const Region &src);
      Region & operator=(Region &&src);

      /**
       * Adds a rectangle to this region.
       * @param rectangle rectangle to add.
       */
      void addRect(const ::i32_rectangle &  rectangle);
      /**
       * Adds offset to all rectangles in region.
       * @param dx horizontal offset to add.
       * @param dy vertical offset to add.
       */
      void translate(int dx, int dy);
      template < prototype_point POINT >
      Region & operator-=(const POINT & point){this->translate(point.x, point.y); return *this;}

      /**
       * Replaces this region by its union with another region.
       * @param other region to add.
       */
      //void add(const Region & other);
      void add(const Region &other);
      /**
       * Subtracts another region from this region.
       * @param other region to subtract.
       */
      void subtract(const Region &other);
      /**
       * Replace this region by its intersection with another region.
       * @param other region to intersect with.
       */
      void intersect(const Region &other);
      /**
       * Sets this region data to intersection of this region and the specified
       * rectangle.
       * @param rectangle rectangle to intersect with.
       */
      void crop(const ::i32_rectangle &  rectangle);

      /**
       * Checks if this region is empty.
       * @return true if this region is empty, false otherwise.
       */
      bool is_empty() const;

      /**
       * Tests point location relative of the region.
       * @return true if this point locates inside the region, false otherwise.
       */
      bool isPointInside(int x, int y) const;
      template < prototype_point POINT >
      bool isPointInside(const POINT & point) const {return this->isPointInside(point.x, point.y); }

      /**
       * Checks if this region equals to another region.
       * @param other a pointer to another region to compare this region to.
       * @return true if this region equals to other, false otherwise.
       */
      bool _equals(const Region &other) const;
      bool operator==(const Region & regionOther) const{return this->_equals(regionOther);}

      /**
       * Get the ::array_base of rectangles that constitute this region.
       * @param rectanglea pointer to an array the list of rectangles will be
       *            saved to. The previous contents of the ::array_base will be cleared.
       */
      void _getRects(::int_rectangle_array_base & rectanglea) const;
      /**
       * Get the ::list_base of rectangles that constitute this region.
       * @return an list of rectangles.
       */
      ::int_rectangle_array_base getRects() const;

      // Returns count of rectangles in the region.
      size_t getCount() const;

      ::i32_rectangle getBounds() const;

   private:
      /**
       * The underlying X11 region structure.
       */
      RegionRec m_reg;
   };

   //// __REGION_REGION_H_INCLUDED__
} // namespace remoting
