// Copyright (C) 2009,2010,2011,2012 GlavSoft LLC.
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
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//

#pragma once


#include "server_config_lib/ConfigReloadListener.h"
//#include "log_writer/LogWriter.h"

class WallpaperUtil : protected ConfigReloadListener
{
public:
  WallpaperUtil(LogWriter *log);
  virtual ~WallpaperUtil();

  virtual void updateWallpaper();

protected:
  virtual void onConfigReload(ServerConfig *serverConfig);

private:
  // Hides the desktop wallpaper.
  // @throws SystemException on a fail.
  void disableWallpaper();

  // Restores the desktop wallpaper.
  // @throws SystemException on a fail.
  void restoreWallpaper();

  LogWriter *m_log;
  ::string m_wallparerPath;
  bool m_wasDisabled;
};

//// __WALLPAPERUTIL_H__
