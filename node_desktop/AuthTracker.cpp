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
//#include "subsystem/thread/lockable_critical_section.h"


namespace remoting_node_desktop
{


   AuthTracker::AuthTracker(const class ::time & timeFailureInterval, ::u32 failureMaxCount) :
       m_uFailureCount(0), m_timeFailureInterval(timeFailureInterval), m_uFailureMaxCount(failureMaxCount)
   {
   }

   AuthTracker::~AuthTracker() {}

   ::u64 AuthTracker::checkBan()
   {
      refresh();

      ::u64 banTime = 0;
      {
         critical_section_lock al(&m_criticalsectionCount);
         if (m_uFailureCount >= m_uFailureMaxCount)
         {
            banTime = maximum(0_s, m_timeFailureInterval -
                                    //(class ::time::now() - m_timeFirstFailure).getTime());
                                    m_timeFirstFailure.elapsed()).integral_millisecond();
         }
      }
      return banTime;
   }

   void AuthTracker::notifyAbAuthFailed()
   {
      critical_section_lock al(&m_criticalsectionCount);
      if (m_uFailureCount == 0)
      {
         m_timeFirstFailure.Now();
      }
      m_uFailureCount++;
   }

   void AuthTracker::refresh()
   {
      critical_section_lock al(&m_criticalsectionCount);
      // if ((class ::time::now() - m_timeFirstFailure).getTime() >= m_timeFailureInterval) {
      if (m_timeFirstFailure.elapsed() >= m_timeFailureInterval)
      {
         m_uFailureCount = 0;
      }
   }


} // namespace remoting_node_desktop
