// Copyright (C) 2009,2010,2011,2012 GlavSoft LLC.
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


#include "subsystem/thread/GuiThread.h"
#include "BlockingGate.h"
#include "ClientListener.h"
//#include "subsystem/platform/AnEventListener.h"
//#include "acme/parallelization/happening.h"
//#include "log_writer/LogWriter.h"
//#include aaa_<map>

namespace remoting_rfb
{

   class CLASS_DECL_REMOTING_RFB  DesktopSrvDispatcher : public ::subsystem::GuiThread
   {
   public:
      //DesktopSrvDispatcher(BlockingGate *pblockinggate,
                           // AnEventListener *m_extTerminationListener,
        //                   const ::procedure &procedureDesktopSrvDispatcher, ::subsystem::LogWriter * plogwriter);
      DesktopSrvDispatcher();
      DesktopSrvDispatcher(BlockingGate *pblockinggate,
       //AnEventListener *m_extTerminationListener,
                         const ::procedure &procedureDesktopSrvDispatcher, ::subsystem::LogWriter * plogwriter);
      ~DesktopSrvDispatcher() override;
      
      
      void destroy() override;
      
      virtual void initialize_desktop_srv_dispatcher(BlockingGate *pblockinggate,
                           // AnEventListener *m_extTerminationListener,
                           const ::procedure &procedureDesktopSrvDispatcher, ::subsystem::LogWriter * plogwriter);

      void registerNewHandle(unsigned char code, ClientListener *listener);

   //protected:
      void onThreadMain() override;
      void onTermThread() override;
      void notifyOnError();

      BlockingGate *m_pcontrolgate;

      ::map<unsigned char, ClientListener *> m_handlers;

      // AnEventListener *m_extErrorListener;

      ::procedure m_procedureDesktopSrvDispatcher;

      ::pointer < ::subsystem::LogWriter > m_plogwriter;
   };


} // namespace remoting_rfb





