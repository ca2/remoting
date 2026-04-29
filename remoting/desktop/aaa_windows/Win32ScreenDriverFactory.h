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

#include "remoting/remoting/desktop/windows/_common_header.h"
#include "../ScreenDriverFactory.h"
#include "remoting/remoting/node_config/ServerConfig.h"


namespace remoting
{

   class CLASS_DECL_REMOTING Win32ScreenDriverFactory :
   virtual public ScreenDriverFactory
   {
   public:
      // The factory stores pointer to a ServerConfig object and will use it on
      // createScreenDriver() function calls.
      Win32ScreenDriverFactory();
      virtual ~Win32ScreenDriverFactory();


      void initialize_screen_driver_factory(ServerConfig *pserverconfig) override;

      virtual ScreenDriver *createScreenDriver(UpdateKeeper * pupdatekeeper, UpdateListener * pupdatelistener,
                                               ::innate_subsystem::Framebuffer *pframebuffer,
                                               lockable_critical_section *pcriticalsectionFramebuffer, ::subsystem::LogWriter * plogwriter);
   private:
      ScreenDriver *createStandardScreenDriver(UpdateKeeper * pupdatekeeper, UpdateListener * pupdatelistener,
                                               ::innate_subsystem::Framebuffer *pframebuffer,
                                               lockable_critical_section *pcriticalsectionFramebuffer, ::subsystem::LogWriter * plogwriter);
      ScreenDriver *createMirrorScreenDriver(UpdateKeeper * pupdatekeeper, UpdateListener * pupdatelistener,
                                             lockable_critical_section *pcriticalsectionFramebuffer, ::subsystem::LogWriter * plogwriter);

      bool isMirrorDriverAllowed();
      bool isD3DAllowed();

      ::pointer < ServerConfig > m_pserverconfig;
   };

   //// __WIN32SCREENDRIVERFACTORY_H__

} // namespace remoting



