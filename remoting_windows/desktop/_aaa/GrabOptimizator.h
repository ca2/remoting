// Copyright (C) 2011,2012 GlavSoft LLC.
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


#include "::remoting::ScreenDriver.h"
#include "remoting/remoting/region/Region.h"
#include "subsystem/platform/DemandTimer.h"
//#include "log_writer/LogWriter.h"
//#include aaa_<vector>
//#include aaa_<list>

namespace remoting_macos
{

   // This class  provides the screen grabbing by an optimal way.
   // The class CLASS_DECL_REMOTING determines an optimal way by oneself dynamically.
   class CLASS_DECL_REMOTING GrabOptimizator :
      virtual public ::particle
   {
   public:

      int m_wholeS;
      ::list_base<double> m_wholeTElements;
      double m_wholeTSum;

      ::list_base<double> m_gElements;
      double m_gSum;

      ::subsystem::DemandTimer m_happeningTimer;

      ::pointer < ::subsystem::LogWriter > m_plogwriter;


      //GrabOptimizator(::subsystem::LogWriter * plogwriter);
      GrabOptimizator();
      ~GrabOptimizator();

      virtual void initialize_grab_optimizator(::subsystem::LogWriter * plogwriter);

      bool grab(const ::remoting::Region & regionGrab, ::remoting::ScreenDriver *grabber);

   //private:
      // Returns true when the m_wholeTElements is comletly calculated.
      bool getWholeTCompleted();

      // Returns true when the m_gElements is comletly calculated.
      bool getGCompleted();

      // Returns true if all needed coefficients is calculated to use
      // the grab optimization.
      bool getOptimizationAvailable(::remoting::ScreenDriver *grabber);
      bool isAvailableWholeStats();
      bool isAvailableFragmentStats();

      // Removes a part of the statistic to made it available to a refresh.
      void refreshStatistic(::remoting::ScreenDriver *grabber);

      // Returns absolute sum area of rectangle ::array_base.
      int getArea(const ::int_rectangle_array_base & rectanglea);

      // If the grab region is alike to whole desktop the function return true.
      bool isAlikeToWhole(const ::int_rectangle_array_base & rectanglea);
      bool isEnoughForWholeStats(const ::i32_rectangle & rectangle);
      // If the grab region is alike to separate fragments the function
      // return true.
      bool isAlikeToFragments(const ::int_rectangle_array_base & rectanglea);

      // This functions store to the log all statistic data.
      void logStatistic();

      static const size_t MIN_ELEMENTS_SIZE = 3;
      static const size_t MAX_ELEMENTS_SIZE = 10;

      __int64 grabWhole(::remoting::ScreenDriver *grabber);
      __int64 grabOneRect(const ::i32_rectangle & rectangle, ::remoting::ScreenDriver *grabber);
      __int64 grabFragments(const ::int_rectangle_array_base & rectanglea, ::remoting::ScreenDriver *grabber);

      void addWholeTElement(double wholeT);
      void removeObsoleteWholeTElements();
      void removeFirstWholeTElement();

      void addFragmentStats(double g);
      void removeObsoleteFragmentStats();
      void removeFirstElementsFromFragmentStats();

   };

 

} // namespace remoting_macos
 


