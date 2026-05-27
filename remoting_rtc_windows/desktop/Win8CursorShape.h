// Copyright (C) 2012 GlavSoft LLC.
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


#include "remoting/remoting_rtc/rfb/CursorShape.h"
#include "remoting/remoting_rtc_windows/_common_header.h"


namespace remoting_rtc_windows
{

   // This class  is wrapper for CursorShape class CLASS_DECL_REMOTING_RTC_WINDOWS to extend it behaviour for the Win8 desktop
   // duplication model.
   class CLASS_DECL_REMOTING_RTC_WINDOWS Win8CursorShape
   {
   public:
      Win8CursorShape();
      virtual ~Win8CursorShape();

      const ::remoting_rtc::CursorShape *getCursorShape() const;

      ::remoting_rtc::CursorShape *getCursorShapeForWriting();

      // threadOwner is a number of latest thread which call this function.
      void setVisibility(bool value, ::i32 threadOwner);

      bool getIsVisible() const;

   private:
      ::i32 m_threadOwner;

      bool m_isVisible;
      ::remoting_rtc::CursorShape m_cursorshapeEmptyDimension;
      ::remoting_rtc::CursorShape m_cursorshape;
   };


} //  namespace remoting_rtc_windows








