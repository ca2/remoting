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

//#pragma once
//_
//
//#include "::i32_rectangle.h"
//
//class ::i32_size
//{
//public:
//  ::i32_size(void) : width(0), height(0) {}
//  ::i32_size(const ::i32 w, const ::i32 h) { set(w, h); }
//  ::i32_size(const ::i32_rectangle &  r) { width = r.width(); height = r.height(); }
//  ::i32_size(const ::i32_size &other): width(other.cx), height(other.cy) {}
//
//  const ::i32_size &operator=(const ::i32_size &other) { width = other.cx;
//                                                       height = other.cy;
//                                                       return *this; }
//  bool operator==(const ::i32_size &other) const { return isEqualTo(other); }
//  bool operator!=(const ::i32_size &other)const { return !isEqualTo(other); }
//
//  virtual ~::i32_size(void) {}
//
//  ::i32_rectangle getRect() const
//  { 
//    ::i32_rectangle r(width, height);
//    return r;
//  }
//
//  ::i32_size getTransposition() const
//  { 
//    ::i32_size r(height, width);
//    return r;
//  }
//
//  // Exchanges width and height
//  void transpose()
//  {
//    ::i32 temp = width;
//    width = height;
//    height = temp;
//  }
//
//  inline void set(const ::i32 w, const ::i32 h) { width = w; height = h; }
//  inline void set(const ::i32_rectangle &  rectangle) { width = rectangle.width();
//                                         height = rectangle.height(); }
//
//  inline bool cmpDim(const ::i32_size & size) const { return size.cx == width &&
//                                                          size.cy == height; }
//
//  inline bool is_empty() const { return width <= 0 || height <= 0; }
//  inline bool isEqualTo(const ::i32_size & other) const
//  {
//    return (width == other.cx)&& (height == other.cy);
//  }
//  inline ::i32 area() const { return is_empty() ? 0 : width * height; }
//
//  inline void clear() { width = height = 0; }
//
//  inline ::i32_size intersection(const ::i32_size & other) const
//  {
//    return ::i32_size(width < other.cx ? width : other.cx,
//                     height < other.cy ? height : other.cy);
//  }
//
//  ::i32 width;
//  ::i32 height;
//};
//
////// __DIMENSION_H__
