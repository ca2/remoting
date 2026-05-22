// Copyright (C) 2012 GlavSoft LLC.
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


#include "UpdateHandler.h"
#include "::remoting_rtc::ScreenDriver.h"
#include "ScreenDriverFactory.h"

namespace remoting_rtc_windows
{

   // This class  contain a base architecture implementation of the UpdateHandler class.
   class CLASS_DECL_REMOTING UpdateHandlerImpl :
      virtual public UpdateHandler,
      virtual public ::remoting_rtc::UpdateListener
   {
   public:
      ::pointer < ::remoting_rtc::UpdateKeeper > m_pupdatekeeperProperty;
      ::pointer < ::remoting_rtc::ScreenDriver > m_pscreendriver;
      ::pointer < UpdateFilter > m_pupdatefilter;
      ::pointer < ::remoting_rtc::UpdateListener > m_pupdatelistenerExternal;

      ::i32_rectangle m_rectangleAbsolute;

      ::pointer < ::subsystem::LogWriter > m_plogwriter;

      bool m_fullUpdateRequested;

      UpdateHandlerImpl(::remoting_rtc::UpdateListener *pupdatelistenerExternal, ScreenDriverFactory *pscreendriverfactory,
                        ::subsystem::LogWriter * plogwriter);
      ~UpdateHandlerImpl() override;

      UpdateContainer extract() override;
      virtual void setFullUpdateRequested(const ::remoting_rtc::Region & region);
      bool checkForUpdates(::remoting_rtc::Region & region);

      virtual void setExcludedRegion(const ::remoting_rtc::Region & regionExcluded);

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


} // namespace remoting_rtc_windows






