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
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//

#pragma once


#include "ft_client_lib/FileTransferRequestSender.h"
#include "ft_client_lib/FileTransferReplyBuffer.h"
#include "ft_client_lib/FileTransferMessageProcessor.h"
#include "ft_client_lib/FileTransferCore.h"

#include "log_writer/LogWriter.h"

#include "ServerMessageListener.h"
#include "CapabilitiesManager.h"

class FileTransferCapability : public ServerMessageListener
{
public:
  FileTransferCapability(Logger *logger = 0);
  virtual ~FileTransferCapability();

  //
  // This method return true, if server support File Transfer.
  //
  virtual bool isEnabled();

  //
  // This method must called from event onConnected() in CoreEventsAdapter.
  //
  void setOutput(RfbOutputGate *output);

  //
  // Overrides MessageListener::onRequest().
  //
  virtual void onServerMessage(unsigned int code, DataInputStream *input);

  virtual FileTransferCore *getCore();
  virtual void setInterface(FileTransferInterface *ftInterface);

  //
  // This method must be called before call RemoteViewerCore::start(),
  // otherwise FT will no worked.
  //
  virtual void addCapabilities(CapabilitiesManager *capabilitiesManager);

protected:
  LogWriter m_logWriter;

  FileTransferRequestSender m_ftRequestSender;
  FileTransferReplyBuffer m_ftReplyBuffer;
  FileTransferMessageProcessor m_ftMessageProcessor;

  FileTransferCore m_ftCore;
};


