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
#include "framework.h"
#include "GrabOptimizator.h"
#include "subsystem/platform/Exception.h"
#include "acme/_operating_system.h"


namespace remoting
{

   GrabOptimizator::GrabOptimizator() :
       m_gSum(0), m_wholeTSum(0), m_wholeS(0), m_happeningTimer(30_minutes)
   {
   }

   GrabOptimizator::~GrabOptimizator() {}

   void GrabOptimizator::initialize_grab_optimizator(::subsystem::LogWriter * plogwriter)
   {
      m_plogwriter = plogwriter;
   }

   bool GrabOptimizator::grab(const Region & regionGrab, ScreenDriver *grabber)
   {
      // The optimization based on the assumption: Tgrab ~= Sgrab/Vgrab + g*N,
      // where Tgrab - is grab time,
      // Sgrab - is grab area,
      // Vgrab - is a grab velocity,
      // N - is number of rectangles to grab,
      // g - overhead time costs adding on each grabbed rectangle.
      //::int_rectangle_array_base rectanglea;
      auto rectanglea = regionGrab.getRects();
      ::int_rectangle rectangleBounds = regionGrab.getBounds();
      int boundsRectS = rectangleBounds.area();
      size_t n = rectanglea.size();

      if (n == 0 || boundsRectS == 0)
      {
         return true;
      }

      refreshStatistic(grabber);

      if (getOptimizationAvailable(grabber))
      {
         _ASSERT(m_wholeTElements.size() != 0);
         _ASSERT(m_gElements.size() != 0);

         // Get estimated time for grab by each rectangle.
         int fragS = getArea(rectanglea);
         double avgWholeT = (double)m_wholeTSum / m_wholeTElements.size();
         double boundsRectT = avgWholeT * boundsRectS / m_wholeS; // Estimated time to
                                                                  // grab by one rectangle
         double grabF = (double)avgWholeT / m_wholeS; // Grab frequency
         double avgG = m_gSum / m_gElements.size();
         double estimatedFragTime = fragS * grabF + avgG * n;
         boundsRectT += avgG;

         if (boundsRectT <= estimatedFragTime)
         {
            __int64 realBoundsRectTime = grabOneRect(rectangleBounds, grabber);
            m_plogwriter->debug("Bounds rectangle grab has been preferred:"
                                " bounds rectangle time = {},"
                                " estimated fragment time = {},"
                                " estimated bounds rectangle time = {}",
                                (int)realBoundsRectTime, (int)estimatedFragTime, (int)boundsRectT);
            // TEST:
            // m_plogwriter->debug("Test: fragment grab time = {}",
            //           (int)grabFragments(&rectanglea, grabber));
         }
         else
         {
            __int64 realFragTime = grabFragments(rectanglea, grabber);
            m_plogwriter->debug("Fragment grab has been preferred:"
                                " fragment time = {}, estimated fragment time = {},"
                                " estimated bounds rectangle time = {}",
                                (int)realFragTime, (int)estimatedFragTime, (int)boundsRectT);
            // TEST:
            // m_plogwriter->debug("Test: bounds rectangle time = {}",
            //           (int)grabOneRect(&rectangleBounds, grabber));
         }
      }
      else
      {
         bool itIsEnoughForWholeStats = isEnoughForWholeStats(rectangleBounds);
         bool itIsEnoughForFragmentsStats = isAlikeToFragments(rectanglea);

         if (itIsEnoughForWholeStats && !isAvailableWholeStats())
         {
            grabOneRect(rectangleBounds, grabber);
         }
         else if (itIsEnoughForFragmentsStats && !isAvailableFragmentStats())
         {
            grabFragments(rectanglea, grabber);
         }
         else if (isAlikeToWhole(rectanglea))
         {
            grabOneRect(rectangleBounds, grabber);
         }
         else
         {
            // Grab as is
            grabFragments(rectanglea, grabber);
         }
      }
      return true;
   }

   bool GrabOptimizator::getOptimizationAvailable(ScreenDriver *grabber)
   {
      int sCurrent = grabber->getScreenBuffer()->getDimension().area();
      if (m_wholeS != sCurrent)
      {
         // Reset all coefficients, and calculate it again.
         m_wholeS = sCurrent;
         m_wholeTSum = 0;
         m_wholeTElements.clear();

         m_gSum = 0;
         m_gElements.clear();
      }

      return isAvailableWholeStats() && isAvailableFragmentStats();
   }

   bool GrabOptimizator::isAvailableWholeStats() { return m_wholeTElements.size() >= MIN_ELEMENTS_SIZE; }

   bool GrabOptimizator::isAvailableFragmentStats() { return m_gElements.size() >= MIN_ELEMENTS_SIZE; }

   bool GrabOptimizator::getWholeTCompleted() { return m_wholeTElements.size() >= MAX_ELEMENTS_SIZE; }

   bool GrabOptimizator::getGCompleted() { return m_gElements.size() >= MAX_ELEMENTS_SIZE; }

   void GrabOptimizator::refreshStatistic(ScreenDriver *grabber)
   {
      if (m_happeningTimer.isElapsed() && getOptimizationAvailable(grabber))
      {
         m_plogwriter->debug("The log statistics before refreshing:");
         logStatistic();

         removeFirstWholeTElement();
         removeFirstElementsFromFragmentStats();

         m_happeningTimer.reset();
         m_plogwriter->debug("The log statistics after refreshing:");
         logStatistic();
      }
   }

   int GrabOptimizator::getArea(const ::int_rectangle_array_base & rectanglea)
   {
      int result = 0;
      for (size_t i = 0; i < rectanglea.size(); i++)
      {
         result += rectanglea[i].area();
      }
      return result;
   }

   bool GrabOptimizator::isAlikeToWhole(const ::int_rectangle_array_base & rectanglea)
   {
      int area = getArea(rectanglea);
      if (area < 1)
      {
         return false;
      }
      return m_wholeS / area <= 9; // area >= 10%
   }

   bool GrabOptimizator::isEnoughForWholeStats(const ::int_rectangle & rectangle)
   {
      int area = rectangle.area();
      if (area < 1)
      {
         return false;
      }
      return m_wholeS / rectangle.area() <= 9; // area >= 10%
   }

   bool GrabOptimizator::isAlikeToFragments(const ::int_rectangle_array_base & rectanglea) { return rectanglea.size() >= 10; }

   __int64 GrabOptimizator::grabWhole(ScreenDriver *grabber)
   {
      // FIXME: WARNING!!! The microsoft API usage!!!
      LARGE_INTEGER timeBegin, timeEnd;
      bool timerResult1 = QueryPerformanceCounter(&timeBegin) != 0;

      if (!grabber->grabFb())
      {
         throw ::subsystem::Exception("Grabber failed. Is it not ready?");
      }

      bool timerResult2 = QueryPerformanceCounter(&timeEnd) != 0;

      if (timerResult1 && timerResult2)
      {
         return timeEnd.QuadPart - timeBegin.QuadPart;
      }
      else
      {
         return -1;
      }
   }

   __int64 GrabOptimizator::grabOneRect(const ::int_rectangle & rectangle, ScreenDriver *grabber)
   {
      int rectS = rectangle.area();
      _ASSERT(rectS != 0);
      // FIXME: WARNING!!! The microsoft API usage!!!
      LARGE_INTEGER timeBegin, timeEnd;
      bool timerResult1 = QueryPerformanceCounter(&timeBegin) != 0;

      if (!grabber->grabFb(rectangle))
      {
         throw ::subsystem::Exception("Grabber failed. Is it not ready?");
      }

      bool timerResult2 = QueryPerformanceCounter(&timeEnd) != 0;

      if (timerResult1 && timerResult2)
      {
         __int64 realOneRectTime = timeEnd.QuadPart - timeBegin.QuadPart;
         if (isEnoughForWholeStats(rectangle))
         {
            // Scale the time as the whole grabbing.
            double wholeT = (double)realOneRectTime * m_wholeS / rectS;
            addWholeTElement(wholeT);
            logStatistic();
         }
         return realOneRectTime;
      }
      else
      {
         return -1;
      }
   }

   void GrabOptimizator::addWholeTElement(double wholeT)
   {
      m_wholeTElements.add(wholeT);
      m_wholeTSum += wholeT;
      m_happeningTimer.reset();

      removeObsoleteWholeTElements();
   }

   void GrabOptimizator::removeObsoleteWholeTElements()
   {
      while (m_wholeTElements.size() > MAX_ELEMENTS_SIZE)
      {
         removeFirstWholeTElement();
         m_happeningTimer.reset();
      }
   }

   void GrabOptimizator::removeFirstWholeTElement()
   {
      ::list_base<double>::iterator iter = m_wholeTElements.begin();
      double wholeT = *iter;
      m_wholeTSum -= wholeT;
      m_wholeTElements.erase(iter);
   }

   __int64 GrabOptimizator::grabFragments(const ::int_rectangle_array_base & rectanglea, ScreenDriver *grabber)
   {
      // FIXME: WARNING!!! The microsoft API usage!!!
      LARGE_INTEGER timeBegin, timeEnd;
      bool timerResult1 = QueryPerformanceCounter(&timeBegin) != 0;

      ::int_rectangle_array_base::const_iterator iRect;
      for (iRect = rectanglea.begin(); iRect < rectanglea.end(); iRect++)
      {
         if (!grabber->grabFb((*iRect)))
         {
            throw ::subsystem::Exception("Grabber failed. Is it not ready?");
         }
      }

      bool timerResult2 = QueryPerformanceCounter(&timeEnd) != 0;

      if (timerResult1 && timerResult2)
      {
         __int64 fragT = timeEnd.QuadPart - timeBegin.QuadPart;

         // To update the statistic wholeT must be calculated at least once.
         if (isAlikeToFragments(rectanglea) && m_wholeTElements.size() > 0)
         {
            int s = getArea(rectanglea);
            size_t n = rectanglea.size();
            double g = (fragT - (double)m_wholeTSum / m_wholeTElements.size() * s / m_wholeS) / n;
            addFragmentStats(g);

            logStatistic();
         }

         return fragT;
      }
      else
      {
         return -1;
      }
   }

   void GrabOptimizator::addFragmentStats(double g)
   {
      m_gElements.add(g);
      m_gSum += g;
      m_happeningTimer.reset();

      removeObsoleteFragmentStats();
   }

   void GrabOptimizator::removeObsoleteFragmentStats()
   {
      while (m_gElements.size() > MAX_ELEMENTS_SIZE)
      {
         removeFirstElementsFromFragmentStats();
         m_happeningTimer.reset();
      }
   }

   void GrabOptimizator::removeFirstElementsFromFragmentStats()
   {
      ::list_base<double>::iterator iterG = m_gElements.begin();
      double g = *iterG;
      m_gSum -= g;
      m_gElements.erase(iterG);
   }

   void GrabOptimizator::logStatistic()
   {
      m_plogwriter->debug("GrabOptimizator::m_wholeS : {}", m_wholeS);

      ::string value;
      ::string statString;
      for (::list_base<double>::iterator iter = m_wholeTElements.begin(); iter != m_wholeTElements.end(); iter++)
      {
         value.format(" %.2f;", *iter);
         statString+=value;
      }
      double avgWholeT = m_wholeTElements.size() != 0 ? m_wholeTSum / m_wholeTElements.size() : 0;
      m_plogwriter->debug("GrabOptimizator::m_wholeT average: %.2f;"
                          " GrabOptimizator::m_wholeTSum: %.2f;"
                          " GrabOptimizator::m_wholeTElements: {}",
                          avgWholeT, m_wholeTSum, statString);

      statString = "";
      for (::list_base<double>::iterator iter = m_gElements.begin(); iter != m_gElements.end(); iter++)
      {
         value.format(" %.2f;", *iter);
         statString+=value;
      }
      double avgG = m_gElements.size() != 0 ? m_gSum / m_gElements.size() : 0;
      m_plogwriter->debug("GrabOptimizator::m_g average: %.2f;"
                          " GrabOptimizator::m_gSum: %.2f;"
                          " GrabOptimizator::m_gElements: {};",
                          avgG, m_gSum, statString);
   }


} // namespace remoting
 
