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


#include "remoting/remoting/desktop/UpdateHandler.h"
#include "DesktopServerProto.h"
#include "DesktopSrvDispatcher.h"
//#include "log_writer/LogWriter.h"

namespace remoting
{

   class CLASS_DECL_REMOTING  UpdateHandlerClient :
   virtual public UpdateHandler,
   virtual public DesktopServerProto,
   virtual public ClientListener
   {
   public:
      // UpdateHandlerClient(BlockingGate *pblockinggate, DesktopSrvDispatcher * pdispatcher,
      //                     UpdateListener *pupdatelistenerExternal, ::subsystem::LogWriter * plogwriter);
      UpdateHandlerClient();
      UpdateHandlerClient(::remoting_node::Configurator * pconfigurator, BlockingGate *pblockinggate, DesktopSrvDispatcher * pdispatcher,
                           UpdateListener *pupdatelistenerExternal, ::subsystem::LogWriter * plogwriter);
      ~UpdateHandlerClient() override;

      virtual void initialize_update_handler_client(::remoting_node::Configurator * pconfigurator, BlockingGate *pblockinggate, DesktopSrvDispatcher * pdispatcher,
                          UpdateListener *pupdatelistenerExternal, ::subsystem::LogWriter * plogwriter);


      virtual UpdateContainer extract();
      virtual void setFullUpdateRequested(const Region & region);
      virtual void setExcludedRegion(const Region & regionExcluded);
      virtual bool checkForUpdates(Region & region);

   //protected:
      virtual void getScreenProperties(::innate_subsystem::PixelFormat & pixelformat, ::int_size & size);
      virtual void sendInit(BlockingGate *pblockinggate);

      // To catch update event
      virtual void onRequest(unsigned char reqCode, BlockingGate *pblockinggate);

      ::pointer < UpdateListener > m_pupdatelistenerExternal;

      ::pointer < ::subsystem::LogWriter > m_plogwriter;
   };


} // namespace remoting







