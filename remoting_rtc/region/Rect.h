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

#pragma once


#include "subsystem/_common_header.h"
//
// namespace remoting
// {
//    class ::i32_rectangle
//    {
//       public:
//       ::i32_rectangle(void) : left(0), top(0), right(0), bottom(0) {}
//       //::i32_rectangle(const ::i32_rectangle &  rectangle) { setRect(rectangle); }
//       ::i32_rectangle(const ::i32_rectangle & rectangle) { setRect(rectangle); }
//       ::i32_rectangle(::i32 lt, ::i32 tp, ::i32 rt, ::i32 bm) { setRect(lt, tp, rt, bm); }
//       ::i32_rectangle(const ::i32_size & size) { setRect(0, 0, width, height); }
//       ::i32_rectangle(const RECT *rectangle) {fromWindowsRect(rectangle);}
//
//       virtual ~::i32_rectangle(void) {}
//
//       ::i32 left;
//       ::i32 top;
//       ::i32 right;
//       ::i32 bottom;
//
//       inline void setRect(::i32 lt, ::i32 tp, ::i32 rt, ::i32 bm)
//       {
//          left = lt;
//          top = tp;
//          right = rt;
//          bottom = bm;
//       }
//
//       inline void setRect(const ::i32_rectangle &  rectangle)
//       {
//          left    = rectangle.left;
//          top     = rectangle.top;
//          right   = rectangle.right;
//          bottom  = rectangle.bottom;
//       }
//
//       inline ::i32_rectangle & operator=(const ::i32_rectangle & rectangle)
//       {
//          setRect(rectangle);
//          return *this;
//       }
//
//       inline bool isValid() const
//       {
//          if (right < left || bottom < top) {
//             return false;
//          }
//          return true;
//       }
//
//       inline bool isPointInRect(::i32 pointX, ::i32 pointY) {
//          if (pointX < left || pointX >= right) {
//             return false;
//          }
//          if (pointY < top || pointY >= bottom) {
//             return false;
//          }
//          return true;
//       }
//
//       // Return true this ::i32_rectangle fully contain the inner ::i32_rectangle, otherwise return false.
//       inline bool isFullyContainRect(const ::i32_rectangle &  inner) {
//          if (inner.left < left || inner.top < top ||
//              inner.right > right || inner.bottom > bottom) {
//             return false;
//              } else {
//                 return true;
//              }
//       }
//
//       // Convert to windows RECT
//       inline RECT toWindowsRect() const {
//          RECT rectangle;
//          rectangle.left    = left;
//          rectangle.top     = top;
//          rectangle.right   = right;
//          rectangle.bottom  = bottom;
//          return rectangle; } const
//
// // Load from windows RECT
// inline void fromWindowsRect(const RECT *rectangle)
//       {
//          left    = rectangle.left;
//          top     = rectangle.top;
//          right   = rectangle.right;
//          bottom  = rectangle.bottom;
//       }
//
//       inline void move(::i32 offsetX, ::i32 offsetY)
//       {
//          left    += offsetX;
//          right   += offsetX;
//          top     += offsetY;
//          bottom  += offsetY;
//       }
//
//       inline void set_top_left(::i32 destX, ::i32 destY)
//       {
//          ::i32 offsetX = destX - left;
//          ::i32 offsetY = destY - top;
//          left    = destX;
//          right   += offsetX;
//          top     = destY;
//          bottom  += offsetY;
//       }
//
//       inline bool isEqualTo(const ::i32_rectangle &  rectangle)  const { return  rectangle.left == left &&
//                                                         rectangle.top == top &&
//                                                         rectangle.right == right &&
//                                                         rectangle.bottom == bottom; }
//
//       inline void set_width(::i32 value)   { right = left + value; }
//       inline void set_height(::i32 value)  { bottom = top + value; }
//
//       inline ::i32 getWidth()  const { return right - left; }
//       inline ::i32 getHeight() const { return bottom - top; }
//
//       void rotateOn90InsideDimension(::i32 dimHeight)
//       {
//          ::i32_rectangle localCopy(this);
//          set_width(localCopy.height());
//          set_height(localCopy.width());
//          ::i32 newLeft = dimHeight - localCopy.top - localCopy.height();
//          ::i32 newTop = localCopy.left;
//          set_top_left(newLeft, newTop);
//       }
//
//       void rotateOn180InsideDimension(::i32 dimWidth, ::i32 dimHeight)
//       {
//          ::i32 newLeft = dimWidth - left - getWidth();
//          ::i32 newTop = dimHeight - top - getHeight();
//          set_top_left(newLeft, newTop);
//       }
//
//       void rotateOn270InsideDimension(::i32 dimWidth)
//       {
//          ::i32_rectangle localCopy(this);
//          set_width(localCopy.height());
//          set_height(localCopy.width());
//          ::i32 newLeft = localCopy.top;
//          ::i32 newTop = dimWidth - localCopy.left - localCopy.width();
//          set_top_left(newLeft, newTop);
//       }
//
//       inline bool is_empty() const { return getWidth() <= 0 || getHeight() <= 0; }
//       inline ::i32 area() const { return is_empty() ? 0 : getWidth() * getHeight(); }
//
//       static ::i32 totalArea(::int_rectangle_array_base &v) {
//          ::i32 area = 0;
//          for (::int_rectangle_array_base::iterator i = v.begin(); i < v.end(); i++)
//             area += i.area();
//          return area;
//       }
//
//       inline void clear() { left = top = right = bottom = 0; }
//
//       ::i32_rectangle intersection(const ::i32_rectangle &  other) const {
//          ::i32_rectangle result;
//          result.setRect((left > other.left) ? left : other.left,
//                         (top > other.top) ? top : other.top,
//                         (right < other.right) ? right : other.right,
//                         (bottom < other.bottom) ? bottom : other.bottom);
//          if (!result.isValid()) {
//             result.clear();
//          }
//          return result;
//       }
//    };
//
//    //// __RECT_H__
// }namespace remoting
//
//
//
