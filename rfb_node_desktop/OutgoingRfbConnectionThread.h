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


//////#include "subsystem/platform/::string.h"
#include "subsystem/thread/Thread.h"
#include "RfbClientManager.h"
//#include "log_writer/LogWriter.h"

namespace remoting_rfb_node_desktop
{


   /**
   Thread that makes attempt to connect to listening rfb client in separate thread.
   To use it, you must create instance, call start method, and pass it to zombie killer.
   @remark don't forget to add it to zombie killer after thread is started.
   @author enikey.
   */
   class CLASS_DECL_REMOTING_NODE_DESKTOP OutgoingRfbConnectionThread : public ::subsystem::Thread
   {
   public:
      /**
      Creates new instance of outgoing rfb connection thread.
      @param connectHost target host to connect.
      @param connectPort port to connect.
      @param viewOnly flag that determinates mode for rfb connection.
      @param clientManager rfb client manager.
      */
      OutgoingRfbConnectionThread(const ::scoped_string &scopedstrConnectHost, ::u32 connectPort, bool viewOnly,
                                  RfbClientManager *clientManager, ::subsystem::LogWriter * plogwriter);
      virtual ~OutgoingRfbConnectionThread();

   protected:
      virtual void onThreadMain();

   private:
      ::string m_connectHost;
      ::u32 m_connectPort;
      bool m_viewOnly;
      RfbClientManager *m_clientManager;
      ::pointer < ::subsystem::LogWriter > m_plogwriter;
   };


} // namespace remoting_rfb_node_desktop
 


