//
// Created by camilo on 2026-04-04.
//
#include "framework.h"
#include "remoting/remoting_rfb_macos/desktop/MacosUserInput.h"
#include "remoting/remoting_rfb_macos/desktop/DesktopFactory.h"
#include "remoting/remoting_rfb_macos/desktop/WallpaperUtil.h"


IMPLEMENT_FACTORY(remoting_rfb_macos)
{

   pfactory->add_factory_item<::remoting_macos::DesktopFactory, ::remoting_rfb::DesktopFactory>();
   pfactory->add_factory_item<::remoting_macos::MacosUserInput, ::remoting_rfb::UserInput>();
   pfactory->add_factory_item<::remoting_macos::WallpaperUtil, ::remoting_rfb::WallpaperUtil>();
   
}


//} // namespace subsystem_windows
