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


#include "subsystem/_common_header.h"
#include "subsystem/node/ClipboardListener.h"
#include "DesktopServerProto.h"
#include "ClientListener.h"
#include "remoting/remoting_rtc/desktop/UserInput.h"
//#include "acme/parallelization/happening.h"
#include "acme/parallelization/happening.h"
#include "DesktopSrvDispatcher.h"

//#include "log_writer/LogWriter.h"

namespace remoting
{


   class CLASS_DECL_REMOTING  UserInputServer : public DesktopServerProto, public ClientListener, public ::subsystem::ClipboardListener
   {
   public:
      UserInputServer(::remoting_rtc_node::Configurator * pconfigurator, BlockingGate *pblockinggate, DesktopSrvDispatcher * pdispatcher,
                      // AnEventListener *extTerminationListener,
                      const ::procedure &procedureTermination, ::subsystem::LogWriter * plogwriter);
      virtual ~UserInputServer();

      // Internal dispatcher
      virtual void onRequest(::u8 reqCode, BlockingGate *pblockinggate);

      virtual void onClipboardUpdate(const ::scoped_string &newClipboard);

   protected:
      virtual void applyNewPointerPos(BlockingGate *pblockinggate);
      virtual void applyNewClipboard(BlockingGate *pblockinggate);
      virtual void applyKeyEvent(BlockingGate *pblockinggate);
      virtual void ansDesktopCoords(BlockingGate *pblockinggate);
      virtual void ansWindowCoords(BlockingGate *pblockinggate);
      virtual void ansUserInfo(BlockingGate *pblockinggate);
      virtual void ansWindowHandle(BlockingGate *pblockinggate);
      virtual void ansDisplayNumberCoords(BlockingGate *pblockinggate);
      virtual void ansDisplaysCoords(BlockingGate *pblockinggate);
      virtual void ansApplicationRegion(BlockingGate *pblockinggate);
      virtual void ansApplicationInFocus(BlockingGate *pblockinggate);
      virtual void ansNormalizeRect(BlockingGate *pblockinggate);

      // At first time server must get init information.
      void serverInit(BlockingGate *pblockinggate);

      ::pointer < UserInput > m_puserinput;
      // AnEventListener *m_extTerminationListener;
      ::procedure m_procedureTermination;

      ::pointer < ::subsystem::LogWriter > m_plogwriter;
   };


} // namespace remoting







