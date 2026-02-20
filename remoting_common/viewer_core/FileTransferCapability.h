// Copyright (C) 2012 GlavSoft LLC.
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


#include "remoting/remoting_common/ftp_client/FileTransferRequestSender.h"
#include "remoting/remoting_common/ftp_client/FileTransferReplyBuffer.h"
#include "remoting/remoting_common/ftp_client/FileTransferMessageProcessor.h"
#include "remoting/remoting_common/ftp_client/FileTransferCore.h"

//#include "log_writer/LogWriter.h"

#include "ServerMessageListener.h"
#include "CapabilitiesManager.h"


namespace  remoting
{
   namespace  ftp
   {
      class CLASS_DECL_REMOTING_COMMON FileTransferCapability : public ServerMessageListener
      {
      public:

         LogWriter * m_logWriter;

         FileTransferRequestSender m_ftRequestSender;
         FileTransferReplyBuffer m_ftReplyBuffer;
         FileTransferMessageProcessor m_ftMessageProcessor;

         FileTransferCore m_ftCore;

         FileTransferCapability(LogWriter *LogWriter = nullptr);
         virtual ~FileTransferCapability();

         //
         // This method return true, if server support ::file::item Transfer.
         //
         virtual bool isEnabled();

         //
         // This method must called from event onConnected() in CoreEventsAdapter.
         //
         void setOutput(RfbOutputGate *output);

         //
         // Overrides MessageListener::onRequest().
         //
         virtual void onServerMessage(unsigned int code, DataInputStream * pinput);

         virtual FileTransferCore *getCore();
         virtual void setInterface(FileTransferInterface *ftInterface);

         //
         // This method must be called before call RemoteViewerCore::start(),
         // otherwise FT will no worked.
         //
         virtual void addCapabilities(CapabilitiesManager *capabilitiesManager);

      };
   }
}