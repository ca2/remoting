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


#include "subsystem/_common_header.h"
#include "remoting/remoting/desktop/DesktopConfigLocal.h"
#include "remoting/remoting/desktop/WallpaperUtil.h"
#include "subsystem/thread/GuiThread.h"
#include "remoting/remoting/desktop/DesktopBaseImpl.h"
#include "remoting/remoting/desktop/ScreenDriverFactory.h"
//#include "log_writer/LogWriter.h"

namespace remoting

{

   class CLASS_DECL_REMOTING_WINDOWS DesktopWinImpl :
   virtual public ::subsystem::GuiThread,
   virtual public DesktopBaseImpl
   {
   public:

      ::pointer < ScreenDriverFactory > m_pscreendriverfactory;

      ::pointer < WallpaperUtil > m_pwallpaperutil;

      ::pointer < DesktopConfigLocal > m_pdesktopconfigclient;

      ::pointer < ::subsystem::LogWriter > m_plogwriter;


      //DesktopWinImpl(ClipboardListener *pclipboardlistenerExternal, UpdateSendingListener *pupdatesendinglistenerExternal,
        //             AbnormDeskTermListener *pdesktermlistenerExternal, ::subsystem::LogWriter * plogwriter);
      DesktopWinImpl();
      ~DesktopWinImpl() override;


      void initialize_desktop(Configurator * pconfigurator, ClipboardListener *pclipboardlistenerExternal, UpdateSendingListener *pupdatesendinglistenerExternal,
                     AbnormDeskTermListener *pdesktermlistenerExternal, ::subsystem::LogWriter * plogwriter) override;

   //protected:
      virtual void execute();
      virtual void onTerminate();

   //private:
      void freeResource();

      // Writes some desktop info to log.
      void logDesktopInfo();

      virtual bool isRemoteInputTempBlocked();
      virtual void applyNewConfiguration();

   };


} // namespace remoting






