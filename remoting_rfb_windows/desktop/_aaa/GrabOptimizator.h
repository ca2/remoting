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

#pragma once


#include "::remoting_rfb::ScreenDriver.h"
#include "remoting/remoting_rfb/region/Region.h"
#include "subsystem/platform/DemandTimer.h"
//#include "log_writer/LogWriter.h"
//#include aaa_<vector>
//#include aaa_<list>

namespace remoting_rfb_windows
{

   // This class  provides the screen grabbing by an optimal way.
   // The class CLASS_DECL_REMOTING_RFB determines an optimal way by oneself dynamically.
   class CLASS_DECL_REMOTING_RFB GrabOptimizator :
      virtual public ::particle
   {
   public:

      ::i32 m_wholeS;
      ::list_base<::f64> m_wholeTElements;
      ::f64 m_wholeTSum;

      ::list_base<::f64> m_gElements;
      ::f64 m_gSum;

      ::subsystem::DemandTimer m_happeningTimer;

      ::pointer < ::subsystem::LogWriter > m_plogwriter;


      //GrabOptimizator(::subsystem::LogWriter * plogwriter);
      GrabOptimizator();
      ~GrabOptimizator();

      virtual void initialize_grab_optimizator(::subsystem::LogWriter * plogwriter);

      bool grab(const ::remoting_rfb::Region & regionGrab, ::remoting_rfb::ScreenDriver *grabber);

   //private:
      // Returns true when the m_wholeTElements is comletly calculated.
      bool getWholeTCompleted();

      // Returns true when the m_gElements is comletly calculated.
      bool getGCompleted();

      // Returns true if all needed coefficients is calculated to use
      // the grab optimization.
      bool getOptimizationAvailable(::remoting_rfb::ScreenDriver *grabber);
      bool isAvailableWholeStats();
      bool isAvailableFragmentStats();

      // Removes a part of the statistic to made it available to a refresh.
      void refreshStatistic(::remoting_rfb::ScreenDriver *grabber);

      // Returns absolute sum area of rectangle ::array_base.
      ::i32 getArea(const ::i32_rectangle_array_base & rectanglea);

      // If the grab region is alike to whole desktop the function return true.
      bool isAlikeToWhole(const ::i32_rectangle_array_base & rectanglea);
      bool isEnoughForWholeStats(const ::i32_rectangle & rectangle);
      // If the grab region is alike to separate fragments the function
      // return true.
      bool isAlikeToFragments(const ::i32_rectangle_array_base & rectanglea);

      // This functions store to the log all statistic data.
      void logStatistic();

      static const size_t MIN_ELEMENTS_SIZE = 3;
      static const size_t MAX_ELEMENTS_SIZE = 10;

      __int64 grabWhole(::remoting_rfb::ScreenDriver *grabber);
      __int64 grabOneRect(const ::i32_rectangle & rectangle, ::remoting_rfb::ScreenDriver *grabber);
      __int64 grabFragments(const ::i32_rectangle_array_base & rectanglea, ::remoting_rfb::ScreenDriver *grabber);

      void addWholeTElement(::f64 wholeT);
      void removeObsoleteWholeTElements();
      void removeFirstWholeTElement();

      void addFragmentStats(::f64 g);
      void removeObsoleteFragmentStats();
      void removeFirstElementsFromFragmentStats();

   };

 

} // namespace remoting_rfb_windows
 


