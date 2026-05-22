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

#pragma once

#include "remoting/remoting_windows/_common_header.h"
#include "subsystem/_common_header.h"
#ifndef CAPTUREBLT
#define CAPTUREBLT          (DWORD)0x40000000 /* Include layered windows */
#endif

#include "remoting/remoting_rtc/desktop/ScreenGrabber.h"



#include "remoting/remoting_rtc/node_config/Configurator.h"
//#include "acme/parallelization/happening.h"
#include "acme/parallelization/happening.h"
#include "subsystem/thread/GuiThread.h"
#include "subsystem_windows/node/Screen.h"
#include "remoting/remoting_windows/_common_header.h"

namespace remoting_windows
{

   //
   // An abstract interface for screen grabbing.
   //

   /*
     //
     // Usage example:
     //

     ScreenGrabber *pframebuffer;

     // Initialisation
     pframebuffer = new WindowsScreenGrabber;

     ::i32_rectangle rectangleGrab, workRect;
     workRect.setRect(100, 100, 500, 500);
     rectangleGrab.setRect(20, 20, 120, 120); // Relative to the workRect
     pframebuffer->setWorkRect(&workRect);

     // One-time grabbing
     while (!pframebuffer->grab(&rectangleGrab)) {
       if (pframebuffer->getPropertiesChanged()) { // Check desktop properties
         if (!pframebuffer->applyNewProperties()) {
           MessageBox(NULL, "Cannot apply new screen properties"), _T("Error", MB_ICONHAND);
           return 1;
         }
       } else {
         MessageBox(NULL, "Cannot grab screen"), _T("Error", MB_ICONHAND);
         return 1;
       }
     }
   */

   class CLASS_DECL_REMOTING_WINDOWS WindowsScreenGrabber :
   virtual public ::remoting_rtc::ScreenGrabber,
   virtual public ::subsystem::GuiThread
   {
   public:
      //WindowsScreenGrabber(::remoting_rtc_node::Configurator * pconfigurator);
      WindowsScreenGrabber();
      ~WindowsScreenGrabber() override;

      virtual void initialize_screen_grabber(::remoting_rtc_node::Configurator * pconfigurator);

      virtual bool grab(const ::i32_rectangle & rectangle);

      inline virtual bool getPropertiesChanged();
      inline virtual bool getPixelFormatChanged();
      inline virtual bool getScreenSizeChanged();

      virtual bool applyNewFullScreenRect();
      virtual bool applyNewPixelFormat();
      virtual bool applyNewProperties();

      // protected:
      virtual void onThreadMain();
      virtual void onTermThread();

      virtual bool openDIBSection();
      virtual bool closeDIBSection();
      virtual bool grabByDIBSection(const ::i32_rectangle & rectangle);

      // Windows specific variebles
      HDC m_destDC, m_screenDC;
      HBITMAP m_hbmOld, m_hbmDIB;

      // ::happening m_hasStartedSignal;
      // ::happening m_threadStopper;
      ::happening m_hasStartedSignal;
      ::happening m_threadStopper;

      // private:
      ::i32_size m_dibSectionDim;
      ::remoting_rtc_node::ServerConfig *m_pserverconfig;

      // Screen m_screen;
      ::subsystem_windows::Screen m_screen;
   };

   //// __WINDOWSSCREENGRABBER_H__

} // namespace remoting_windows





