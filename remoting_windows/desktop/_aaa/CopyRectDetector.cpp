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
#include "framework.h"
#include "CopyRectDetector.h"
#include "subsystem/_common_header.h"
#include "acme/_operating_system.h"

namespace remoting_windows
{

   CopyRectDetector::CopyRectDetector() {}

   CopyRectDetector::~CopyRectDetector() {}


   // Callback routine used internally to catch window movement...
   static bool CALLBACK enumWindowsFnCopyRect(HWND hwnd, LPARAM lparam)
   {
      auto _this = (CopyRectDetector *) lparam;
      return _this->checkWindowMovements(::as_operating_system_window(hwnd));
   }


   void CopyRectDetector::detectWindowMovements(::i32_rectangle &rectangleCopy, ::i32_point & pointSource)
   {
      m_rectangleCopy.Null();
      m_pointSource.Null();

      EnumWindows((WNDENUMPROC)enumWindowsFnCopyRect, (::lparam)this);
      m_lastWinProps = m_newWinProps;
      m_newWinProps.clear();
      rectangleCopy = m_rectangleCopy;
      pointSource = m_pointSource;
   }


   bool CopyRectDetector::checkWindowMovements(const ::operating_system::window & operatingsystemwindow)
   {
      ::i32_rectangle rectangleCurrent;
      //auto hwnd = ::as_HWND(operatingsystemwindow);
      if (IsWindowVisible(::as_HWND(operatingsystemwindow)) && getWinRect(operatingsystemwindow, rectangleCurrent))
      {
         // Store window properties in the new ::list_base
         WinProp newWinProp(operatingsystemwindow, rectangleCurrent);
         m_newWinProps.add(newWinProp);

         ::i32_rectangle rectangleOld;
         if (findPrevWinProps(operatingsystemwindow, rectangleOld))
         {
            if (rectangleOld.top_left() != rectangleCurrent.top_left() && rectangleCurrent.area() > m_rectangleCopy.area())
            {

               m_rectangleCopy = rectangleCurrent;

               m_pointSource = rectangleOld.top_left();

               // Adjust
               i32_size sizeDesktop(GetSystemMetrics(SM_XVIRTUALSCREEN),GetSystemMetrics(SM_YVIRTUALSCREEN));

               m_rectangleCopy -= sizeDesktop;

               m_pointSource -= sizeDesktop;

            }
         }
      }
      return true;
   }

   bool CopyRectDetector::getWinRect(const ::operating_system::window & operatingsystemwindow, ::i32_rectangle & rectangle)
   {
      return ::windows::get_window_rect(operatingsystemwindow, rectangle);
   }

   bool CopyRectDetector::findPrevWinProps(const ::operating_system::window & operatingsystemwindow, ::i32_rectangle & rectangle)
   {
      ::list_base<WinProp>::iterator winPropsIter;
      WinProp *winProp;
      for (winPropsIter = m_lastWinProps.begin(); winPropsIter != m_lastWinProps.end(); winPropsIter++)
      {
         winProp = &(*winPropsIter);
         if (winProp->m_operatingsystemwindow == operatingsystemwindow)
         {
            rectangle = winProp->m_rectangleOld;
            return true;
         }
      }

      return false;
   }


} // namespace remoting_windows




