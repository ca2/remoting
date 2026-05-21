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
#include "framework.h"
#include "WallpaperUtil.h"
#include "subsystem/node/OperatingSystem.h"
#include "remoting/remoting/node_config/Configurator.h"
#include "subsystem/node/AutoImpersonator.h"
#include "acme/_operating_system.h"


namespace remoting_windows
{


   WallpaperUtil::WallpaperUtil()
   : m_bWasDisabled(false), m_plogwriter(nullptr)
   {



   }

   void WallpaperUtil::initialize_wallpaper_util(::remoting_node::Configurator * pconfigurator, ::subsystem::LogWriter *plogwriter)
   {
      m_pconfigurator = pconfigurator;
      m_plogwriter = plogwriter;
      initialize_config_reload_listener(pconfigurator);

   }

   WallpaperUtil::~WallpaperUtil()
   {
      m_pconfigurator->removeListener(this);
      if (m_bWasDisabled)
      {
         try
         {
            restoreWallpaper();
            m_plogwriter->information("Wallpaper was successfully restored");
         }
         catch (::exception &e)
         {
            m_plogwriter->error(e.get_message());
         }
      }
   }

   void WallpaperUtil::onConfigReload(::remoting_node::ServerConfig *serverConfig) { updateWallpaper(); }

   void WallpaperUtil::updateWallpaper()
   {
      try
      {
         ::remoting_node::ServerConfig *pserverconfig = m_pconfigurator->getServerConfig();
         if (pserverconfig->isRemovingDesktopWallpaperEnabled())
         {
            disableWallpaper();
            m_bWasDisabled = true;
            m_plogwriter->information("Wallpaper was successfully disabled");
         }
         else
         {
            if (m_bWasDisabled)
            {
               restoreWallpaper();
               m_plogwriter->information("Wallpaper was successfully restored");
               m_bWasDisabled = false;
            }
         }
      }
      catch (::exception &e)
      {
         m_plogwriter->error(e.get_message());
      }
   }

   void WallpaperUtil::restoreWallpaper()
   {
      // FIXME: Remove log from here. Log only from caller.
      m_plogwriter->information("Try to restore wallpaper");
      ::subsystem::Impersonator impersonator;
      impersonator.initialize_impersonator(m_plogwriter);
      ::subsystem::AutoImpersonator autoimpersonator(&impersonator, m_plogwriter);
      int result;

      if (m_strWallpaperPath.length() == 0)
      {
         result = SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, 0, 0);
      }
      else
      {
         result = SystemParametersInfo(SPI_SETDESKWALLPAPER, m_strWallpaperPath.length(), (void *)m_strWallpaperPath.c_str(), 0);
      }

      if (result == 0)
      {
         throw ::subsystem::SystemException("Cannot restore desktop wallpaper");
      }
   }

   void WallpaperUtil::disableWallpaper()
   {
      m_plogwriter->information("Try to disable wallpaper");
      ::pointer<::subsystem::Impersonator> impersonator;
      ::auto_pointer<::subsystem::AutoImpersonator> ai;
      if (!m_papplication->m_bUserApplication)
      {
         ///::subsystem::Impersonator impersonator;
         construct_newø(impersonator);
         impersonator->initialize_impersonator(m_plogwriter);
         emplace_newø(ai, impersonator, m_plogwriter);
      }
         
      
      //impersonator.initialize_impersonator(m_plogwriter);
      //::subsystem::AutoImpersonator ai(&impersonator, m_plogwriter);
      WCHAR path[MAX_PATH] = L"";

      if (SystemParametersInfo(SPI_GETDESKWALLPAPER, MAX_PATH, path, 0) == 0)
      {
         path[0] = '\0';
      }

      WCHAR pathEmptyNew[MAX_PATH] = L"";

      if (SystemParametersInfoW(SPI_SETDESKWALLPAPER, 0, pathEmptyNew, 0) == 0)
      {
         throw ::subsystem::SystemException("Cannot disable desktop wallpaper");
      }
      m_strWallpaperPath = ::string(path);
   }


} // namespace remoting_windows




