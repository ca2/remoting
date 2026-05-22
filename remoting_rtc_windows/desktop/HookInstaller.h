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

#pragma once


#include "subsystem/_common_header.h"
#include "subsystem/node/DynamicLibrary.h"
#include "remoting/desktop/HookInstaller.h"


namespace remoting_rtc_windows
{


   class CLASS_DECL_REMOTING_RTC_WINDOWS HookInstaller :
        virtual public ::remoting_rtc::HookInstaller
   {
   public:
      HookInstaller();
      ~HookInstaller();

      // Instals hooks to windowed applications.
      // targedWinHwnd - is hwnd to a target window that will receive
      // hooked messages.
      void install(const ::operating_system::window & operatingsystemwindow) override;
      // Uninstals hooks from windowed applications.
      void uninstall() override;

   //private:
      static ::string_literal LIBRARY_NAME;
      
#ifdef WINDOWS
      static ::string_literal SET_HOOK_FUNCTION_NAME;
      static ::string_literal UNSET_HOOK_FUNCTION_NAME;
#endif
      ::subsystem::DynamicLibrary m_library;
      void * m_pSetHook;
      void * m_pUnSetHook;
   };


} // namespace remoting







