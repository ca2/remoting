// Copyright (C) 2011,2012 GlavSoft LLC.
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
#include "RectSerializer.h"
// #include aaa_<tchar.h>
// #include aaa_<stdio.h>
#include "subsystem/platform/Exception.h"
#include "acme/prototype/geometry2d/rectangle.h"


namespace remoting
{
   void RectSerializer::toString(const ::i32_rectangle &  rectangle, ::string & strOut)
   {
      strOut.format("{}x{}+{}+{}", rectangle.width(),
                                        rectangle.height(),
                                        rectangle.left,
                                        rectangle.top);
   }

   ::i32_rectangle RectSerializer::toRect(const ::scoped_string & strIn)
   {
      int width, height, x, y;
      char c;
      if (sscanf(::string(strIn).c_str(),
                   (char *)"%dx%d+%d+%d%c", &width, &height, &x, &y, &c) != 4 ||
          width < 0 || height < 0) {
         ::string errMess;
         errMess.format("Invalid string format to convert it to a rectangle"
                        " ({}).", strIn);
         throw ::subsystem::Exception(errMess);
          }
      return ::i32_rectangle(x, y, x + width, y + height);
   }
} // namespace remoting