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


#include "subsystem/_common_header.h"
#include "DesktopConfigLocal.h"
#include "remoting/remoting_rfb/desktop/WallpaperUtil.h"
#include "subsystem/thread/GuiThread.h"
#include "DesktopBaseImpl.h"
#include "windows/Win32ScreenDriverFactory.h"
//#include "log_writer/LogWriter.h"

namespace remoting_windows

{

   class CLASS_DECL_REMOTING_RFB DesktopWinImpl :
   virtual public ::subsystem::GuiThread,
   virtual public DesktopBaseImpl
   {
   public:
      //DesktopWinImpl(ClipboardListener *pclipboardlistenerExternal, UpdateSendingListener *pupdatesendinglistenerExternal,
        //             AbnormDeskTermListener *pdesktermlistenerExternal, ::subsystem::LogWriter * plogwriter);
      DesktopWinImpl();
      ~DesktopWinImpl() override;


      virtual void initialize_desktop_win_impl(ClipboardListener *pclipboardlistenerExternal, UpdateSendingListener *pupdatesendinglistenerExternal,
                     AbnormDeskTermListener *pdesktermlistenerExternal, ::subsystem::LogWriter * plogwriter);

   //protected:
      virtual void onThreadMain();
      virtual void onTermThread();

   //private:
      void freeResource();

      // Writes some desktop info to log.
      void logDesktopInfo();

      virtual bool isRemoteInputTempBlocked();
      virtual void applyNewConfiguration();

      Win32ScreenDriverFactory m_pscreendriverfactory;

      WallpaperUtil *m_pwallpaperutil;

      DesktopConfigLocal *m_pdesktopconfigclient;

      ::pointer < ::subsystem::LogWriter > m_plogwriter;
   };


} // namespace remoting_windows






