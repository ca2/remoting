// Copyright (C) 2009,2010,2011,2012,2020 GlavSoft LLC.
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


//#include "subsystem/platform/inttypes.h"
#include "remoting/remoting_rfb/network/RfbInputGate.h"
#include "remoting/remoting_rfb/network/RfbOutputGate.h"
#include "remoting/remoting_rfb/rfb_sconn/RfbCodeRegistrator.h"
#include "remoting/remoting_rfb/rfb_sconn/RfbDispatcherListener.h"
//#include "log_writer/LogWriter.h"

namespace remoting_rfb
{


   namespace file_transfer
   {
      /**
       * Handler of echo extension plugin client to server messages.
       * Processes client requests and sends replies.
       */
      class EchoExtensionRequestHandler : public RfbDispatcherListener
      {
      public:
         /**
          * Creates new echo extension client messages handler.
          * @param registrator rfb registrator which needs to register echo messages
          *   to rfb dispatcher address whem to this object.
          * @param output pblockinggate for writting replies for requests.
          * @pararm enabled indicates if echo response should be enabled or disabled
          */
         EchoExtensionRequestHandler(RfbCodeRegistrator *registrator, ::remoting_rfb::RfbOutputGate *output, ::subsystem::LogWriter * plogwriter,
                                     bool enabled = true);

         /**
          * Deletes echo extension request handler.
          */
         virtual ~EchoExtensionRequestHandler();

         /**
          * Inherited from RfbDispatcherListener.
          * Processes echo extension client messages.
          */
         virtual void onRequest(::u32 reqCode, ::remoting_rfb::RfbInputGate *pblockinggate);

         bool isEchoExtensionEnabled();

      protected:
         //
         // Input and output gates.
         //

         ::remoting_rfb::RfbInputGate *m_input;
         ::remoting_rfb::RfbOutputGate *m_output;

         bool m_enabled;
         ::pointer < ::subsystem::LogWriter > m_plogwriter;
      };


   } // namespace file_transfer


} // namespace remoting_rfb
