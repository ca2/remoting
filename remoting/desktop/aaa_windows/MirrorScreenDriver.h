// Copyright (C) 2011,2012 GlavSoft LLC.
// All rights reserved.
//
//-------------------------------------------------------------------------
// This file is part of the TightVNC software.  Please visit our Web site:
//
//                       http://www.tightvnc.com/
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

#include "remoting/remoting/desktop/windows/_common_header.h"
#include "MirrorDriverClient.h"
#include "subsystem/platform/RegistryKey.h"
#include "DisplayEsc.h"
#include "subsystem/thread/GuiThread.h"
#include "acme/parallelization/happening.h"
#include "UpdateDetector.h"


namespace remoting
{

   class CLASS_DECL_REMOTING MirrorScreenDriver :
   virtual public UpdateDetector
   {
   public:
      MirrorScreenDriver();
      virtual ~MirrorScreenDriver();


      virtual void initialize_mirror_screen_driver(UpdateKeeper * pupdatekeeper, UpdateListener * pupdatelistener,
                         critical_section *pcriticalsectionFramebuffer, ::subsystem::LogWriter * plogwriter);

      // Starts screen update detection if it not started yet.
      virtual void executeDetection();

      // Stops screen update detection.
      virtual void terminateDetection();

      virtual ::int_size getScreenDimension();
      virtual ::innate_subsystem::Framebuffer *getScreenBuffer();
      virtual bool grab(const ::int_rectangle & rectangle = 0);

      virtual bool getPropertiesChanged();
      virtual bool getScreenSizeChanged();

      virtual bool applyNewProperties();

   private:
      void initFramebuffer();

      void startUpdateSearching();

      virtual void execute();
      virtual void onTerminate();

      MirrorDriverClient *m_mirrorClient;
      unsigned long m_lastCounter;
      ::innate_subsystem::Framebuffer m_pframebuffer;
      // TO THINK: One may use a self mutex here, because do not
      // use external objects here.
      critical_section *m_pcriticalsectionFramebuffer;

      ::happening m_updateTimeout;

      ::pointer < ::subsystem::LogWriter > m_plogwriter;
   };


} // namespace remoting






