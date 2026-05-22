//
// Created by camilo on 2026-04-04.
//
#include "framework.h"
#include "remoting/remoting_windows/desktop/DesktopWinImpl.h"
#include "remoting/remoting_windows/desktop/Win32ScreenDriverFactory.h"
#include "remoting/remoting_windows/desktop/WindowsInputBlocker.h"
#include "remoting/remoting_windows/desktop/WindowsUserInput.h"
#include "remoting/remoting_windows/desktop/DesktopFactory.h"
#include "remoting/remoting_windows/desktop/HookInstaller.h"
#include "remoting/remoting_windows/desktop/WallpaperUtil.h"


IMPLEMENT_FACTORY(remoting_windows)
{

   pfactory->add_factory_item<::remoting_windows::HookInstaller, ::remoting_rtc::HookInstaller >();
   pfactory->add_factory_item<::remoting_windows::DesktopWinImpl, ::remoting_rtc::Desktop>();
   pfactory->add_factory_item<::remoting_windows::WindowsUserInput, ::remoting_rtc::UserInput>();
   pfactory->add_factory_item<::remoting_windows::WindowsInputBlocker, ::remoting_rtc::InputBlocker>();
   pfactory->add_factory_item<::remoting_windows::WallpaperUtil, ::remoting_rtc::WallpaperUtil>();

   //pfactory->add_factory_item<::remoting_rtc::ApplicationDesktopFactory, ::remoting_rtc::DesktopFactory>();
   pfactory->add_factory_item<::remoting_windows::DesktopFactory, ::remoting_rtc::DesktopFactory>();
   pfactory->add_factory_item<::remoting_windows::Win32ScreenDriverFactory, ::remoting_rtc::ScreenDriverFactory>();

}


//} // namespace subsystem_windows

