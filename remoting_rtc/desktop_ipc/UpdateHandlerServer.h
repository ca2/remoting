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


#include "remoting/remoting_rtc/desktop_ipc/DesktopServerProto.h"
#include "remoting/remoting_rtc/desktop_ipc/DesktopSrvDispatcher.h"
#include "remoting/remoting_rtc/desktop/UpdateHandler.h"
//#include "remoting/remoting_rtc/desktop/UpdateHandlerImpl.h"

//#include "log_writer/LogWriter.h"
//#include "remoting/remoting_rtc/desktop/ScreenDriverFactory.h"


namespace remoting
{

   class CLASS_DECL_REMOTING  UpdateHandlerServer :
      virtual public DesktopServerProto,
      virtual public ClientListener,
      virtual public UpdateListener
   {
   public:

      ::pointer < ScreenDriverFactory > m_pscreendriverfactory;

      ::pointer < ::remoting_rtc_node::Configurator > m_pconfigurator;

      ::innate_subsystem::PixelFormat m_pixelformatOld;

      ::pointer < UpdateHandler > m_pupdatehandler;
      // AnEventListener *m_extTerminationListener;
      ::procedure m_procedureTermination;

      ::pointer < ::subsystem::LogWriter > m_plogwriter;


      //UpdateHandlerServer(::remoting_rtc_node::Configurator *pconfigurator, BlockingGate *pblockinggate, DesktopSrvDispatcher * pdispatcher,
        //                  // AnEventListener *extTerminationListener,
          //                const ::procedure &procedureTermination, ::subsystem::LogWriter * plogwriter);
      UpdateHandlerServer();
      ~UpdateHandlerServer() override;

      virtual void initialize_update_handler_server(::remoting_rtc_node::Configurator *pconfigurator, BlockingGate *pblockinggate, DesktopSrvDispatcher * pdispatcher,
                    // AnEventListener *extTerminationListener,
                    const ::procedure &procedureTermination, ::subsystem::LogWriter * plogwriter);

      // Internal dispatcher
      virtual void onRequest(::u8 reqCode, BlockingGate *pblockinggate);

   //protected:
      virtual void onUpdate();

      // At first time server must get init information.
      void serverInit(BlockingGate *pblockinggate);

      void extractReply(BlockingGate *pblockinggate);
      void screenPropReply(BlockingGate *pblockinggate);
      void receiveFullReqReg(BlockingGate *pblockinggate);
      void receiveExcludingReg(BlockingGate *pblockinggate);

   };


} // namespace remoting








