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
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//

//#ifndef __DIMENSION_H__
//#define __DIMENSION_H__
//
//#include "::int_rectangle.h"
//
//class ::int_size
//{
//public:
//  ::int_size(void) : width(0), height(0) {}
//  ::int_size(const int w, const int h) { setDim(w, h); }
//  ::int_size(const ::int_rectangle &  r) { width = r.width(); height = r.height(); }
//  ::int_size(const ::int_size &other): width(other.cx), height(other.cy) {}
//
//  const ::int_size &operator=(const ::int_size &other) { width = other.cx;
//                                                       height = other.cy;
//                                                       return *this; }
//  bool operator==(const ::int_size &other) const { return isEqualTo(other); }
//  bool operator!=(const ::int_size &other)const { return !isEqualTo(other); }
//
//  virtual ~::int_size(void) {}
//
//  ::int_rectangle getRect() const
//  { 
//    ::int_rectangle r(width, height);
//    return r;
//  }
//
//  ::int_size getTransposition() const
//  { 
//    ::int_size r(height, width);
//    return r;
//  }
//
//  // Exchanges width and height
//  void transpose()
//  {
//    int temp = width;
//    width = height;
//    height = temp;
//  }
//
//  inline void setDim(const int w, const int h) { width = w; height = h; }
//  inline void setDim(const ::int_rectangle &  rect) { width = rect.width();
//                                         height = rect.height(); }
//
//  inline bool cmpDim(const ::int_size & dim) const { return dim.cx == width &&
//                                                          dim.cy == height; }
//
//  inline bool is_empty() const { return width <= 0 || height <= 0; }
//  inline bool isEqualTo(const ::int_size & other) const
//  {
//    return (width == other.cx)&& (height == other.cy);
//  }
//  inline int area() const { return is_empty() ? 0 : width * height; }
//
//  inline void clear() { width = height = 0; }
//
//  inline ::int_size intersection(const ::int_size & other) const
//  {
//    return ::int_size(width < other.cx ? width : other.cx,
//                     height < other.cy ? height : other.cy);
//  }
//
//  int width;
//  int height;
//};
//
//#endif // __DIMENSION_H__
