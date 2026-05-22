// Copyright (C) 2011,2012 GlavSoft LLC.
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

#include "remoting/remoting_rtc_windows/_common_header.h"
#include "MirrorDriverClient.h"
#include "subsystem/platform/RegistryKey.h"
#include "remoting/remoting_rtc/desktop/DisplayEsc.h"
#include "subsystem/thread/GuiThread.h"
#include "acme/parallelization/happening.h"
#include "remoting/remoting_rtc/desktop/UpdateDetector.h"


namespace remoting_rtc_windows
{

   class CLASS_DECL_REMOTING_RTC_WINDOWS MirrorScreenDriver :
   virtual public ::remoting_rtc::UpdateDetector
   {
   public:


      ::pointer < MirrorDriverClient > m_pmirrordriverclient;
      unsigned long m_lastCounter;
      ::pointer < ::innate_subsystem::Framebuffer > m_pframebuffer;
      // TO THINK: One may use a self mutex here, because do not
      // use external objects here.
      lockable_critical_section *m_pcriticalsectionFramebuffer;

      ::happening m_updateTimeout;

      ::pointer < ::subsystem::LogWriter > m_plogwriter;


      MirrorScreenDriver();
      virtual ~MirrorScreenDriver();


      virtual void initialize_mirror_screen_driver(::remoting_rtc::UpdateKeeper * pupdatekeeper, ::remoting_rtc::UpdateListener * pupdatelistener,
                         lockable_critical_section *pcriticalsectionFramebuffer, ::subsystem::LogWriter * plogwriter);

      // Starts screen update detection if it not started yet.
      virtual void executeDetection();

      // Stops screen update detection.
      virtual void terminateDetection();

      virtual ::i32_size getScreenDimension();
      virtual ::innate_subsystem::Framebuffer *getScreenBuffer();
      virtual bool grab(const ::i32_rectangle & rectangle = {});

      virtual bool getPropertiesChanged();
      virtual bool getScreenSizeChanged();

      virtual bool applyNewProperties();

   private:
      void initFramebuffer();

      void startUpdateSearching();

      virtual void onThreadMain();
      virtual void onTermThread();

   };


} // namespace remoting_rtc_windows






