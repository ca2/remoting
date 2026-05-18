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

namespace remoting_windows

{

   class CLASS_DECL_REMOTING_WINDOWS DesktopWinImpl :
   virtual public ::subsystem::GuiThread,
   virtual public ::remoting::DesktopBaseImpl
   {
   public:

      ::pointer < ::remoting::ScreenDriverFactory > m_pscreendriverfactory;

      ::pointer < ::remoting::WallpaperUtil > m_pwallpaperutil;

      ::pointer < ::remoting::DesktopConfigLocal > m_pdesktopconfiglocal;

      ::pointer < ::subsystem::LogWriter > m_plogwriter;


      //DesktopWinImpl(ClipboardListener *pclipboardlistenerExternal, UpdateSendingListener *pupdatesendinglistenerExternal,
        //             AbnormDeskTermListener *pdesktermlistenerExternal, ::subsystem::LogWriter * plogwriter);
      DesktopWinImpl();
      ~DesktopWinImpl() override;


      void initialize_desktop(::remoting_node::Configurator * pconfigurator, ::subsystem::ClipboardListener *pclipboardlistenerExternal, ::remoting::UpdateSendingListener *pupdatesendinglistenerExternal,
                     ::remoting::AbnormDeskTermListener *pdesktermlistenerExternal, ::subsystem::LogWriter * plogwriter) override;

   //protected:
      virtual void onThreadMain();
      virtual void onTermThread();

   //private:
      void freeResource();

      // Writes some desktop info to log.
      void logDesktopInfo();

      virtual bool isRemoteInputTempBlocked();
      virtual void applyNewConfiguration();

   };


} // namespace remoting_windows



