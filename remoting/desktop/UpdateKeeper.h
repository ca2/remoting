// Copyright (C) 2008,2009,2010,2011,2012 GlavSoft LLC.
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


//#include "subsystem/thread/critical_section.h"
#include "remoting/remoting/region/Region.h"
#include "UpdateContainer.h"
#include "subsystem/thread/Lockable.h"
#include "acme/prototype/geometry2d/rectangle.h"

namespace remoting
{


   class CLASS_DECL_REMOTING UpdateKeeper : public ::subsystem::LockableInterface
   {
   public:
      UpdateKeeper();
      UpdateKeeper(const ::int_rectangle &borderRect);
      ~UpdateKeeper(void);

      virtual ::e_status lock()
      {
         m_updContLocMut.lock();
         return ::success;
      }

      virtual void unlock() { m_updContLocMut.unlock(); }

      void addChangedRegion(const ::remoting::Region *changedRegion);
      void addChangedRect(const ::int_rectangle &changedRect);
      // Adds border rectangle to changed region.
      void dazzleChangedReg()
      {
         critical_section_lock al(&m_updContLocMut);
         addChangedRect(m_borderRect);
      }

      void addCopyRect(const ::int_rectangle &copyRect, const ::int_point *src);

      void setBorderRect(const ::int_rectangle &borderRect);

      void setScreenSizeChanged();
      void setCursorPosChanged();
      void setCursorPos(const ::int_point *curPos);
      void setCursorShapeChanged();

      void setExcludedRegion(const ::remoting::Region *excludedRegion);

      void addUpdateContainer(const UpdateContainer *updateContainer);
      void getUpdateContainer(UpdateContainer *updCont);
      bool checkForUpdates(const ::remoting::Region *region);

      void extract(UpdateContainer *updateContainer);

   private:
      ::int_rectangle m_borderRect;

      ::remoting::Region m_excludedRegion;
      critical_section m_exclRegLocMut;

      UpdateContainer m_updateContainer;
      critical_section m_updContLocMut;
   };


} // namespace remoting






