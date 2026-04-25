// Copyright (C) 2010,2011,2012 GlavSoft LLC.
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
#include "AuthTracker.h"
//#include "subsystem/thread/critical_section.h"


namespace remoting_node_desktop
{


   AuthTracker::AuthTracker(unsigned long long failureTimeInterval, unsigned int failureMaxCount) :
       m_failureCount(0), m_failureTimeInterval(failureTimeInterval), m_failureMaxCount(failureMaxCount)
   {
   }

   AuthTracker::~AuthTracker() {}

   unsigned long long AuthTracker::checkBan()
   {
      refresh();

      unsigned long long banTime = 0;
      {
         critical_section_lock al(&m_countMutex);
         if (m_failureCount >= m_failureMaxCount)
         {
            banTime = maximum(0, m_failureTimeInterval -
                                    //(class ::time::now() - m_firstFailureTime).getTime());
                                    (class ::time::now() - m_firstFailureTime).m_iSecond);
         }
      }
      return banTime;
   }

   void AuthTracker::notifyAbAuthFailed()
   {
      critical_section_lock al(&m_countMutex);
      if (m_failureCount == 0)
      {
         m_firstFailureTime = class ::time::now();
      }
      m_failureCount++;
   }

   void AuthTracker::refresh()
   {
      critical_section_lock al(&m_countMutex);
      // if ((class ::time::now() - m_firstFailureTime).getTime() >= m_failureTimeInterval) {
      if ((class ::time::now() - m_firstFailureTime).m_iSecond >= m_failureTimeInterval)
      {
         m_failureCount = 0;
      }
   }


} // namespace remoting_node_desktop
