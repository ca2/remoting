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



//#include "remoting/remoting_rtc/region/::i32_point.h"
#include "subsystem/_common_header.h"
//#include aaa_<list>
#include "acme/prototype/geometry2d/rectangle.h"

namespace remoting_rtc_windows
{

   struct WinProp
   {
      WinProp(const ::operating_system::window & operatingsystemwindow, const ::i32_rectangle &rectangleOld)
      {
         m_operatingsystemwindow = operatingsystemwindow;
         m_rectangleOld = rectangleOld;
      }
      ::operating_system::window m_operatingsystemwindow;
      ::i32_rectangle m_rectangleOld;
   };

   class CLASS_DECL_REMOTING CopyRectDetector :
      virtual public ::particle
   {
   public:
      CopyRectDetector();
      virtual ~CopyRectDetector();

      void detectWindowMovements(::i32_rectangle &rectangleCopy, ::i32_point & pointSource);

   //protected:
      bool checkWindowMovements(const ::operating_system::window & operatingsystemwindow);

      bool getWinRect(const ::operating_system::window & operatingsystemwindow, ::i32_rectangle & rectangle);

      // If window properties successfully was found then function returns
      // true. Else this function returns false.
      bool findPrevWinProps(const ::operating_system::window & operatingsystemwindow, ::i32_rectangle & rectangle);

      ::i32_rectangle m_rectangleCopy;
      ::i32_point m_pointSource;

      ::list_base<WinProp> m_lastWinProps;
      ::list_base<WinProp> m_newWinProps;
   };

   

} // namespace remoting_rtc_windows
 


