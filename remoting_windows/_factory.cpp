//
// Created by camilo on 2026-04-04.
//
#include "framework.h"
#include "remoting/remoting_macos/desktop/DesktopWinImpl.h"
#include "remoting/remoting_macos/desktop/Win32ScreenDriverFactory.h"
#include "remoting/remoting_macos/desktop/WindowsInputBlocker.h"
#include "remoting/remoting_macos/desktop/WindowsUserInput.h"
#include "remoting/remoting_macos/desktop/DesktopFactory.h"


IMPLEMENT_FACTORY(remoting_macos)
{

   pfactory->add_factory_item<::remoting_macos::DesktopWinImpl, ::remoting::Desktop>();
   pfactory->add_factory_item<::remoting_macos::WindowsUserInput, ::remoting::UserInput>();
   pfactory->add_factory_item<::remoting_macos::WindowsInputBlocker, ::remoting::InputBlocker>();

   //pfactory->add_factory_item<::remoting::ApplicationDesktopFactory, ::remoting::DesktopFactory>();
   pfactory->add_factory_item<::remoting_macos::DesktopFactory, ::remoting::DesktopFactory>();
   pfactory->add_factory_item<::remoting_macos::Win32ScreenDriverFactory, ::remoting::ScreenDriverFactory>();

}


//} // namespace subsystem_windows

