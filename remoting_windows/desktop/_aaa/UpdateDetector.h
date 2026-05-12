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


#include "::remoting::UpdateKeeper.h"
#include "subsystem/thread/GuiThread.h"
#include "::remoting::UpdateListener.h"



namespace remoting_macos
{


   class CLASS_DECL_REMOTING UpdateDetector : public ::subsystem::GuiThread
   {
   public:


      UpdateDetector();
      ~UpdateDetector() override;


      virtual void initialize_update_detector(::remoting::UpdateKeeper * pupdatekeeper, ::remoting::UpdateListener * pupdatelistener);


      void setUpdateKeeper(::remoting::UpdateKeeper *pupdatekeeper) { m_pupdatekeeper = pupdatekeeper; }
      ::remoting::UpdateKeeper *getUpdateKeeper() const { return m_pupdatekeeper; }

   //protected:
      void doUpdate()
      {
         if (m_pupdatelistener)
         {
            m_pupdatelistener->onUpdate();
         }
      }

      ::pointer < ::remoting::UpdateKeeper > m_pupdatekeeper;

      ::pointer < ::remoting::UpdateListener > m_pupdatelistener;
   };

   //// __UPDATEDETECTOR_H__

} // namespace remoting_macos





