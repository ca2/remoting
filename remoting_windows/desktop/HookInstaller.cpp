// Copyright (C) 2010,2011,2012 GlavSoft LLC.
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
#include "HookInstaller.h"
#include "acme/_operating_system.h"

namespace remoting_windows
{

#ifndef _WIN64
   ::string_literal HookInstaller::LIBRARY_NAME = "screenhooks32.dll";
#else
   ::string_literal HookInstaller::LIBRARY_NAME = "screenhooks64.dll";
#endif

   ::string_literal HookInstaller::SET_HOOK_FUNCTION_NAME = "setHook";
   ::string_literal HookInstaller::UNSET_HOOK_FUNCTION_NAME = "unsetHook";

   typedef bool (*SetHookFunction)(HWND targedWinHwnd);
   typedef bool (*UnsetHookFunction)();

   HookInstaller::HookInstaller() :m_pSetHook(0), m_pUnSetHook(0)
   {
      m_library.initialize_dynamic_library(LIBRARY_NAME);

   }

   HookInstaller::~HookInstaller() { uninstall(); }

   void HookInstaller::install(const ::operating_system::window & operatingsystemwindow)
   {
      //HINSTANCE hinst = GetModuleHandle(0);

      m_pSetHook = m_library.getProcAddress(SET_HOOK_FUNCTION_NAME);
      m_pUnSetHook = m_library.getProcAddress(UNSET_HOOK_FUNCTION_NAME);
      if (!m_pSetHook || !m_pUnSetHook)
      {
         throw ::subsystem::Exception("Cannot find the setHook() and unsetHook() functions");
      }
      auto hwnd = ::as_HWND(operatingsystemwindow);
      // Hooks initializing
      SetHookFunction setHookFunction = (SetHookFunction)m_pSetHook;
      if (!setHookFunction(hwnd))
      {
         throw ::subsystem::Exception("setHook() function failed");
      }
   }

   void HookInstaller::uninstall()
   {
      if (m_pUnSetHook)
      {
         UnsetHookFunction unsetHookFunction = (UnsetHookFunction)m_pUnSetHook;
         if (!unsetHookFunction())
         {
            throw ::subsystem::Exception("unsetHook() function failed");
         }
         m_pUnSetHook = 0;
         m_pSetHook = 0;
      }
   }


} // namespace remoting_windows
