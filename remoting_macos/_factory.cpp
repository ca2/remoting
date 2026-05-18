//
// Created by camilo on 2026-04-04.
//
#include "framework.h"
#include "remoting/remoting_macos/desktop/MacosUserInput.h"
#include "remoting/remoting_macos/desktop/DesktopFactory.h"
#include "remoting/remoting_macos/desktop/WallpaperUtil.h"


IMPLEMENT_FACTORY(remoting_macos)
{

   pfactory->add_factory_item<::remoting_macos::DesktopFactory, ::remoting::DesktopFactory>();
   pfactory->add_factory_item<::remoting_macos::MacosUserInput, ::remoting::UserInput>();
   pfactory->add_factory_item<::remoting_macos::WallpaperUtil, ::remoting::WallpaperUtil>();
   
}


//} // namespace subsystem_windows
