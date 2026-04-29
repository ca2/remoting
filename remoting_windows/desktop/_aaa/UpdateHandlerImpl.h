// Copyright (C) 2012 GlavSoft LLC.
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


#include "UpdateHandler.h"
#include "::remoting::ScreenDriver.h"
#include "ScreenDriverFactory.h"

namespace remoting_windows
{

   // This class  contain a base architecture implementation of the UpdateHandler class.
   class CLASS_DECL_REMOTING UpdateHandlerImpl :
      virtual public UpdateHandler,
      virtual public ::remoting::UpdateListener
   {
   public:
      ::pointer < ::remoting::UpdateKeeper > m_pupdatekeeperProperty;
      ::pointer < ::remoting::ScreenDriver > m_pscreendriver;
      ::pointer < UpdateFilter > m_pupdatefilter;
      ::pointer < ::remoting::UpdateListener > m_pupdatelistenerExternal;

      ::int_rectangle m_rectangleAbsolute;

      ::pointer < ::subsystem::LogWriter > m_plogwriter;

      bool m_fullUpdateRequested;

      UpdateHandlerImpl(::remoting::UpdateListener *pupdatelistenerExternal, ScreenDriverFactory *pscreendriverfactory,
                        ::subsystem::LogWriter * plogwriter);
      ~UpdateHandlerImpl() override;

      UpdateContainer extract() override;
      virtual void setFullUpdateRequested(const ::remoting::Region & region);
      bool checkForUpdates(::remoting::Region & region);

      virtual void setExcludedRegion(const ::remoting::Region & regionExcluded);

   //private:
      virtual void executeDetectors();
      virtual void terminateDetectors();

      void doUpdate()
      {
         if (m_pupdatelistenerExternal)
         {
            m_pupdatelistenerExternal->onUpdate();
         }
      }

      virtual void onUpdate();

      void applyNewScreenProperties();


   };


} // namespace remoting_windows






