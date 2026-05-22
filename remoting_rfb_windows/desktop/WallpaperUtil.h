// Copyright (C) 2009,2010,2011,2012 GlavSoft LLC.
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


#include "remoting/remoting_rfb/desktop/WallpaperUtil.h"
//#include "log_writer/LogWriter.h"

namespace remoting_windows
{

   class CLASS_DECL_REMOTING_WINDOWS WallpaperUtil :
      virtual public ::remoting_rfb::WallpaperUtil
   {
   public:

            ::pointer<::subsystem::LogWriter> m_plogwriter;
      ::string m_strWallpaperPath;
      bool m_bWasDisabled;


      WallpaperUtil();
      ~WallpaperUtil() override;


      void initialize_wallpaper_util(::remoting_rfb_node::Configurator * pconfigurator, ::subsystem::LogWriter * plogwriter) override;

      void updateWallpaper() override;

   //protected:
      void onConfigReload(::remoting_rfb_node::ServerConfig *serverConfig) override;

   //private:
      // Hides the desktop wallpaper.
      // @throws SystemException on a fail.
      void disableWallpaper() override;

      // Restores the desktop wallpaper.
      // @throws SystemException on a fail.
      void restoreWallpaper() override;  

   };

   //// __WALLPAPERUTIL_H__


}// namespace remoting_rfb




