// Copyright (C) 2009,2010,2011,2012 GlavSoft LLC.
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


#include "subsystem/thread/Thread.h"
#include "subsystem/node/Process.h"
//#include "log_writer/LogWriter.h"
#include "remoting/remoting/desktop_ipc/ReconnectionListener.h"


namespace remoting
{

   /**
    * Thread that used to execute desktop server application.
    *
    * @behavour:
    *   Thread in infinity loop executes (and waits until it dies) desktop
    *   server application.
    *   It will break only if thread will be terminated.
    */
   class CLASS_DECL_REMOTING DesktopServerWatcher :
   virtual public ::subsystem::Thread
   {
   public:
      //DesktopServerWatcher(ReconnectionListener *recListener, ::subsystem::LogWriter * plogwriter);
      DesktopServerWatcher();
      ~DesktopServerWatcher() override;

      virtual void initialize_desktop_server_watcher(ReconnectionListener *recListener, ::subsystem::LogWriter * plogwriter);

   //protected:
      virtual void execute();
      virtual void onTerminate();

      // Runs desktop server with XP trick if it's needed.
      void start();

      // Changes active physical session to session0 and locks workstation.
      // @throws SystemException on fail.
      void doXPTrick();

      ::pointer < ::subsystem::Process  > m_pprocess;
      ::pointer < ReconnectionListener > m_preconnectionlistener;

      ::pointer < ::subsystem::LogWriter > m_plogwriter;
   };

 

} // namespace remoting
 


