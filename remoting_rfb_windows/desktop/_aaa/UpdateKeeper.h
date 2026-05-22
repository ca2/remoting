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


//#include "subsystem/thread/lockable_critical_section.h"
#include "remoting/remoting_rfb/region/Region.h"
#include "UpdateContainer.h"
#include "subsystem/thread/Lockable.h"
#include "acme/prototype/geometry2d/rectangle.h"

namespace remoting_windows
{


   class CLASS_DECL_REMOTING_RFB ::remoting_rfb::UpdateKeeper : public ::subsystem::LockableInterface
   {
   public:
      ::remoting_rfb::UpdateKeeper();
      ::remoting_rfb::UpdateKeeper(const ::i32_rectangle &borderRect);
      ~::remoting_rfb::UpdateKeeper(void);

      virtual ::e_status lock()
      {
         m_criticalsectionUpdateContainer.lock();
         return ::success;
      }

      virtual void unlock() { m_criticalsectionUpdateContainer.unlock(); }

      void addChangedRegion(const ::remoting_rfb::Region & regionChanged);
      void addChangedRect(const ::i32_rectangle &rectangleChanged);
      // Adds border rectangle to changed region.
      void dazzleChangedReg()
      {
         critical_section_lock al(&m_criticalsectionUpdateContainer);
         addChangedRect(m_rectangleBorder);
      }

      void addCopyRect(const ::i32_rectangle &rectangleCopy, const ::i32_point &src);

      void setBorderRect(const ::i32_rectangle &borderRect);

      void setScreenSizeChanged();
      void setCursorPosChanged();
      void setCursorPos(const ::i32_point &curPos);
      void setCursorShapeChanged();

      void setExcludedRegion(const ::remoting_rfb::Region & regionExcluded);
      void clearExcludedRegion();

      void addUpdateContainer(const UpdateContainer & updatecontainer);
      void getUpdateContainer(UpdateContainer & updatecontainer);
      bool checkForUpdates(const ::remoting_rfb::Region & region);

      void extract(UpdateContainer & updatecontainer);

   private:
      ::i32_rectangle m_rectangleBorder;

      ::remoting_rfb::Region m_regionExcluded;
      lockable_critical_section m_criticalsectionExclRegLoc;

      UpdateContainer m_updatecontainer;
      lockable_critical_section m_criticalsectionUpdateContainer;
   };


} // namespace remoting_windows






