//
// Created by camilo on 2026-04-04.
//
#include "framework.h"
#include "desktop/windows/DesktopWinImpl.h"
#include "desktop/windows/Win32ScreenDriverFactory.h"
#include "desktop/windows/WindowsInputBlocker.h"
#include "desktop/windows/WindowsUserInput.h"
#include "remoting/desktop/ApplicationDesktopFactory.h"


IMPLEMENT_FACTORY(remoting_windows)
{

   pfactory->add_factory_item<::remoting::DesktopWinImpl, ::remoting::Desktop>();
   pfactory->add_factory_item<::remoting::WindowsUserInput, ::remoting::UserInput>();
   pfactory->add_factory_item<::remoting::WindowsInputBlocker, ::remoting::InputBlocker>();

   pfactory->add_factory_item<::remoting::ApplicationDesktopFactory, ::remoting::DesktopFactory>();
   pfactory->add_factory_item<::remoting::Win32ScreenDriverFactory, ::remoting::ScreenDriverFactory>();

}


//} // namespace subsystem_windows

