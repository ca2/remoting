//
// Created by camilo on 2026-04-04.
//
#include "framework.h"
#include "remoting/remoting_rfb_windows/desktop/DesktopWinImpl.h"
#include "remoting/remoting_rfb_windows/desktop/Win32ScreenDriverFactory.h"
#include "remoting/remoting_rfb_windows/desktop/WindowsInputBlocker.h"
#include "remoting/remoting_rfb_windows/desktop/WindowsUserInput.h"
#include "remoting/remoting_rfb_windows/desktop/DesktopFactory.h"
#include "remoting/remoting_rfb_windows/desktop/HookInstaller.h"
#include "remoting/remoting_rfb_windows/desktop/WallpaperUtil.h"


IMPLEMENT_FACTORY(remoting_rfb_windows)
{

   pfactory->add_factory_item<::remoting_rfb_windows::HookInstaller, ::remoting_rfb::HookInstaller >();
   pfactory->add_factory_item<::remoting_rfb_windows::DesktopWinImpl, ::remoting_rfb::Desktop>();
   pfactory->add_factory_item<::remoting_rfb_windows::WindowsUserInput, ::remoting_rfb::UserInput>();
   pfactory->add_factory_item<::remoting_rfb_windows::WindowsInputBlocker, ::remoting_rfb::InputBlocker>();
   pfactory->add_factory_item<::remoting_rfb_windows::WallpaperUtil, ::remoting_rfb::WallpaperUtil>();

   //pfactory->add_factory_item<::remoting_rfb::ApplicationDesktopFactory, ::remoting_rfb::DesktopFactory>();
   pfactory->add_factory_item<::remoting_rfb_windows::DesktopFactory, ::remoting_rfb::DesktopFactory>();
   pfactory->add_factory_item<::remoting_rfb_windows::Win32ScreenDriverFactory, ::remoting_rfb::ScreenDriverFactory>();

}


//} // namespace subsystem_windows

