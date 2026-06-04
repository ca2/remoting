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
#include "framework.h"
#include "DesktopWinImpl.h"
#include "remoting/remoting_rfb_windows/desktop/WindowsUserInput.h"
#include "remoting/remoting_rfb/node_config/Configurator.h"
#include "remoting/remoting_rfb/desktop_ipc/UpdateHandlerClient.h"
#include "remoting/remoting_rfb/desktop/UpdateHandlerImpl.h"
#include "remoting/remoting_rfb/desktop/InputBlocker.h"
#include "remoting/remoting_rfb/desktop_ipc/UserInputClient.h"
#include "remoting/remoting_rfb/desktop/SasUserInput.h"
#include "remoting/remoting_rfb/desktop/UserInput.h"
#include "subsystem/node/OperatingSystem.h"
#include "subsystem/node/Displays.h"

namespace remoting_rfb_windows

{
   //
   // DesktopWinImpl::DesktopWinImpl(ClipboardListener *pclipboardlistenerExternal, UpdateSendingListener *pupdatesendinglistenerExternal,
   //                                AbnormDeskTermListener *pdesktermlistenerExternal, ::subsystem::LogWriter * plogwriter) :
   //     DesktopBaseImpl(pclipboardlistenerExternal, pupdatesendinglistenerExternal, pdesktermlistenerExternal, log), m_pwallpaperutil(0), m_pdesktopconfigclient(0),
   //     m_plogwriter = plogwriter;, m_pscreendriverfactory(m_pconfigurator->getServerConfig())
   // {
   //    m_plogwriter->information("Creating DesktopWinImpl");
   //
   //    logDesktopInfo();
   //
   //    try
   //    {
   //       m_pupdatehandler = new UpdateHandlerImpl(this, &m_pscreendriverfactory, m_plogwriter);
   //       bool ctrlAltDelEnabled = false;
   //       m_puserinput = new WindowsUserInput(this, ctrlAltDelEnabled, m_plogwriter);
   //       m_pdesktopconfigclient = new DesktopConfigLocal(m_plogwriter);
   //       applyNewConfiguration();
   //       m_pwallpaperutil = new WallpaperUtil(m_plogwriter);
   //       m_pwallpaperutil->updateWallpaper();
   //
   //       m_pconfigurator->addListener(this);
   //    }
   //    catch (::subsystem::Exception &ex)
   //    {
   //       m_plogwriter->error("exception during DesktopWinImpl creaion: {}", ex.get_message());
   //       freeResource();
   //       throw;
   //    }
   //    resumeThread();
   // }

   DesktopWinImpl::DesktopWinImpl()
   {


   }


   void DesktopWinImpl::initialize_desktop(
      ::remoting_rfb_node::Configurator * pconfigurator, ClipboardListener *pclipboardlistenerExternal, ::remoting_rfb::UpdateSendingListener *pupdatesendinglistenerExternal,
                                  ::remoting_rfb::AbnormDeskTermListener *pdesktermlistenerExternal, ::subsystem::LogWriter * plogwriter)
       // DesktopBaseImpl(pclipboardlistenerExternal, pupdatesendinglistenerExternal, pdesktermlistenerExternal, log), m_pwallpaperutil(0), m_pdesktopconfigclient(0),
       // m_plogwriter = plogwriter;, m_pscreendriverfactory(m_pconfigurator->getServerConfig())
   {

      initialize_desktop_base_impl(pconfigurator, pclipboardlistenerExternal, pupdatesendinglistenerExternal, pdesktermlistenerExternal, plogwriter);
      m_plogwriter= plogwriter;

      constructø(m_pscreendriverfactory);

      m_pscreendriverfactory->initialize_screen_driver_factory(m_pconfigurator->getServerConfig());

          m_plogwriter->information("Creating DesktopWinImpl");

          logDesktopInfo();

          try
          {
             m_pupdatehandler = new ::remoting_rfb::UpdateHandlerImpl(m_pconfigurator, this, m_pscreendriverfactory, m_plogwriter);
             bool ctrlAltDelEnabled = false;
             constructø(m_puserinput);
             m_puserinput->initialize_user_input(this, ctrlAltDelEnabled, m_plogwriter);

             construct_newø(m_pdesktopconfiglocal);
             m_pdesktopconfiglocal->initialize_desktop_config_local(pconfigurator, m_plogwriter);
             applyNewConfiguration();

             constructø(m_pwallpaperutil);
             m_pwallpaperutil->initialize_wallpaper_util(pconfigurator,m_plogwriter);
             m_pwallpaperutil->updateWallpaper();

             m_pconfigurator->addListener(this);
          }
          catch (::subsystem::Exception &ex)
          {
             m_plogwriter->error("exception during DesktopWinImpl creaion: {}", ex.get_message());
             freeResource();
             throw;
          }
          resumeThread();
       }



   DesktopWinImpl::~DesktopWinImpl()
   {
      m_plogwriter->information("Deleting DesktopWinImpl");
      //setThreadToFinish();
      //waitThreadToFinish();
      freeResource();
      m_plogwriter->information("DesktopWinImpl deleted");
   }


   // void DesktopWinImpl::initialize_desktop(Configurator * pconfigurator, ClipboardListener *pclipboardlistenerExternal, UpdateSendingListener *pupdatesendinglistenerExternal,
   //                             AbnormDeskTermListener *pdesktermlistenerExternal, ::subsystem::LogWriter * plogwriter)
   // // :
   // //  , m_pwallpaperutil(0), m_pdesktopconfigclient(0),
   // //  m_plogwriter = plogwriter;, m_pscreendriverfactory(m_pconfigurator->getServerConfig())
   //  {
   //     DesktopBaseImpl::initialize_desktop(pconfigurator, pclipboardlistenerExternal, pupdatesendinglistenerExternal, pdesktermlistenerExternal, plogwriter);
   //     m_plogwriter = plogwriter;
   //
   //     m_plogwriter->information("Creating DesktopWinImpl");
   //
   //     logDesktopInfo();
   //
   //     try
   //     {
   //        m_pupdatehandler = allocateø UpdateHandlerImpl(this, m_pscreendriverfactory, m_plogwriter);
   //        bool ctrlAltDelEnabled = false;
   //        m_puserinput = allocateø WindowsUserInput(this, ctrlAltDelEnabled, m_plogwriter);
   //        m_pdesktopconfigclient = new DesktopConfigLocal(m_plogwriter);
   //        applyNewConfiguration();
   //        m_pwallpaperutil = new WallpaperUtil(m_plogwriter);
   //        m_pwallpaperutil->updateWallpaper();
   //
   //        m_pconfigurator->addListener(this);
   //     }
   //     catch (::subsystem::Exception &ex)
   //     {
   //        m_plogwriter->error("exception during DesktopWinImpl creaion: {}", ex.get_message());
   //        freeResource();
   //        throw;
   //     }
   //     resumeThread();
   //  }


   void DesktopWinImpl::freeResource()
   {
      m_pconfigurator->removeListener(this);

      m_pwallpaperutil.defer_destroy_and_release();
      m_pupdatehandler.defer_destroy_and_release();
      m_pdesktopconfiglocal.defer_destroy_and_release();
      m_puserinput.defer_destroy_and_release();

      // if (m_pwallpaperutil)
      //    delete m_pwallpaperutil;
      //
      // if (m_pupdatehandler)
      //    delete m_pupdatehandler;
      // if (m_pdesktopconfigclient)
      //    delete m_pdesktopconfiglocal;
      // if (m_puserinput)
      //    delete m_puserinput;
   }

   void DesktopWinImpl::onTermThread() { m_happeningNewUpdate.set_happening(); }

   void DesktopWinImpl::onThreadMain()
   {
      m_plogwriter->information("DesktopWinImpl thread started");

      while (!isThreadTerminating())
      {
         m_happeningNewUpdate.wait();
         if (!isThreadTerminating())
         {
            m_plogwriter->debug("DesktopWinImpl sendUpdate()");
            sendUpdate();
         }
      }

      m_plogwriter->information("DesktopWinImpl thread stopped");
   }

   bool DesktopWinImpl::isRemoteInputTempBlocked() { return !m_pdesktopconfiglocal->isRemoteInputAllowed(); }

   void DesktopWinImpl::applyNewConfiguration()
   {
      m_plogwriter->information("reload DesktopWinImpl configuration");
      m_pdesktopconfiglocal->updateByNewSettings();
   }

   void DesktopWinImpl::logDesktopInfo()
   {
      try
      {
         if (MainSubsystem().OperatingSystem().isAeroOn(m_plogwriter))
         {
            m_plogwriter->debug("The Aero is On");
         }
         else
         {
            m_plogwriter->debug("The Aero is Off");
         }
      }
      catch (::exception &e)
      {
         m_plogwriter->error("Can't get information for the Aero: {}", e.get_message());
      }

      // Log all display coordinates
      ::subsystem::Displays displays;
      ::i32_rectangle_array_base rectangleaDisplays = displays.getDisplays();
      m_plogwriter->debug("The console desktop has {} displays", (::i32)rectangleaDisplays.size());
      ::i32 i = 1;
      for (auto & rectangleDisplay : rectangleaDisplays)
      {
         m_plogwriter->debug("Display {} placed at the {:d} coordinates", i, rectangleDisplay);
         i++;
      }
   }


} // namespace remoting_rfb_windows
 


