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
//#include <vector>
//#include <list>

//#include "::int_rectangle.h"

extern "C" {
#include "x11region.h"
}

/**
 * A Region is an area which can be represented by a set of rectangles with
 * integer coordinates. It maintains the ::list_base of rectangles such way that
 * individual rectangles never overlap. When a rectangle is added to a Region,
 * only its non-overlapping part will be actually added. Note that adding a
 * rectangle will not necessarily increment the number of rectangles by one.
 * On such addition, the underlying ::list_base of rectangles may change dramatically
 * and its length may increase, decrease or remain the same.
 */
class Region {
public:
  /**
   * Creates an empty region.
   */
  Region();
  /**
   * Creates a region from the given rectangle.
   * @param rect a pointer to the source rectangle.
   */
  Region(const ::int_rectangle &rect);
  /**
   * Creates a copy of another region.
   * @param rect a reference to the source region.
   */
  Region(const Region &src);
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
  void set(const Region *src);
  /**
   * Overriden assignment operator. Replaces this region with a copy of
   * another region.
   * @param src a reference to the source region.
   */
  Region & operator=(const Region &src);

  /**
   * Adds a rectangle to this region.
   * @param rect rectangle to add.
   */
  void addRect(const ::int_rectangle &  rect);
  /**
   * Adds offset to all rectangles in region.
   * @param dx horizontal offset to add.
   * @param dy vertical offset to add.
   */
  void translate(int dx, int dy);
  /**
   * Replaces this region by its union with another region.
   * @param other region to add.
   */
  void add(const Region *other);
  void add(const Region &other);
  /**
   * Subtracts another region from this region.
   * @param other region to subtract.
   */
  void subtract(const Region *other);
  /**
   * Replace this region by its intersection with another region.
   * @param other region to intersect with.
   */
  void intersect(const Region *other);
  /**
   * Sets this region data to intersection of this region and the specified
   * rectangle.
   * @param rect rectangle to intersect with.
   */
  void crop(const ::int_rectangle &  rect);

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

  /**
   * Checks if this region equals to another region.
   * @param other a pointer to another region to compare this region to.
   * @return true if this region equals to other, false otherwise.
   */
  bool equals(const Region *other) const;

  /**
   * Get the ::array_base of rectangles that constitute this region.
   * @param dst pointer to an ::array_base where the ::list_base of rectangles will be
   *            saved to. The previous contents of the ::array_base will be cleared.
   */
  void getRectVector(::array_base<::int_rectangle> *dst) const;
  /**
   * Get the ::list_base of rectangles that constitute this region.
   * @param dst pointer to an ::list_base where the ::list_base of rectangles will be
   *            saved to. The previous contents of the ::list_base will be cleared.
   */
  ::int_rectangle_array_base getRects() const;

  // Returns count of rectangles in the region.
  size_t getCount() const;

  ::int_rectangle getBounds() const;

private:
  /**
   * The underlying X11 region structure.
   */
  RegionRec m_reg;
};

//// __REGION_REGION_H_INCLUDED__
