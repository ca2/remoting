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


#include "remoting/remoting_rtc_windows/_common_header.h"
#include "remoting/remoting_rtc_windows/_common_header.h"
#include "remoting/remoting_rtc/desktop/CursorShapeGrabber.h"
#include "subsystem/_common_header.h"
#include "subsystem_windows/node/Screen.h"


namespace remoting_rtc_windows
{

   class CLASS_DECL_REMOTING_RTC_WINDOWS WindowsCursorShapeGrabber :
   virtual public ::remoting_rtc::CursorShapeGrabber
   {
   public:
      WindowsCursorShapeGrabber();
      ~WindowsCursorShapeGrabber() override;

      virtual bool grab(const ::innate_subsystem::PixelFormat &pixelFormat);

      virtual bool isCursorShapeChanged();

      // private:
      bool grabPixels(const ::innate_subsystem::PixelFormat &pixelFormat);

      HCURSOR getHCursor();

      HCURSOR m_lastHCursor;
      ::subsystem_windows::Screen m_screen;
   };


} //  namespace remoting_rtc_windows







