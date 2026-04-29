// Copyright (C) 2013 GlavSoft LLC.
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

#include "remoting/remoting_windows/_common_header.h"
//#include "subsystem/platform/class ::time.h"
#include "remoting/remoting/region/Region.h"
//#include "subsystem/platform/::string_array.h"
#include "remoting/remoting/desktop/ScreenDriver.h"
//#include "subsystem/thread/lockable_critical_section.h"
//#include "log_writer/LogWriter.h"
#include "subsystem/thread/Thread.h"
#include "acme/parallelization/happening.h"

namespace remoting_windows
{

   class CLASS_DECL_REMOTING_WINDOWS WinVideoRegionUpdaterImpl :
      virtual public ::remoting::ScreenDriver,
      virtual public ::subsystem::Thread
   {
   public:


      class ::time m_timeLastVideoUpdate;
      ::remoting::Region m_regionVideo;
      lockable_critical_section m_criticalsectionRegion;
      ::pointer < ::subsystem::LogWriter > m_plogwriter;
      ::happening m_happeningSleeper;
      ::pointer < ::remoting_node::Configurator > m_pconfigurator;


      //WinVideoRegionUpdaterImpl(::subsystem::LogWriter * plogwriter);
      WinVideoRegionUpdaterImpl();
      ~WinVideoRegionUpdaterImpl() override;


      void initialize_screen_driver(::remoting_node::Configurator * pconfigurator, ::remoting::UpdateKeeper * pupdatekeeper, ::remoting::UpdateListener * pupdatelistener,
                                        ::innate_subsystem::Framebuffer *pframebuffer,
                          lockable_critical_section *pcriticalsectionFramebuffer, ::subsystem::LogWriter * plogwriter) override;


   //protected:
      virtual void execute();
      virtual void onTerminate();
   //private:
      virtual ::remoting::Region getVideoRegion();
      void updateVideoRegion();
      void getClassNamesAndRectsFromConfig(::string_array &classNames, ::int_rectangle_array_base &rectanglea);
      unsigned int getInterval();
      ::remoting::Region getRectsByClass(::string_array classNames);
      ::remoting::Region getRectsByCoords(::int_rectangle_array_base &rectanglea);

   };


}// namespace remoting_windows



