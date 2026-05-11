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

#pragma once


//#include "subsystem/thread/lockable_critical_section.h"
//#include "subsystem/platform/class ::time.h"

namespace remoting_node_desktop
{

   // This class will be insert an time interval between trying of
   // authentications
   class CLASS_DECL_REMOTING_NODE_DESKTOP AuthTracker :
   virtual public ::particle
   {
   public:
      AuthTracker(const class ::time & timeFailureInterval, ::u32 failureMaxCount);
      virtual ~AuthTracker();

   protected:
      // Return ban time if new connection banned and zero if it's not banned.
      ::u64 checkBan();
      // If authentication failed this function must be called to increase an
      // internal failure count.
      void notifyAbAuthFailed();

   private:
      // If ban time is elapsed the refresh() function reset m_uFailureCount.
      void refresh();

      class ::time m_timeFailureInterval;

      class ::time m_timeFirstFailure;
      ::u32 m_uFailureCount;
      ::u32 m_uFailureMaxCount;
      lockable_critical_section m_criticalsectionCount;
   };


} // namespace remoting_node_desktop







