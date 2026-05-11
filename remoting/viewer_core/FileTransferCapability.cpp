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
#include "framework.h"

#include "FileTransferCapability.h"

#include "remoting/remoting/file_transfer_client/FileTransferRequestSender.h"
#include "remoting/remoting/file_transfer_client/FileTransferReplyBuffer.h"
#include "remoting/remoting/file_transfer_client/FileTransferMessageProcessor.h"
#include "remoting/remoting/file_transfer_client/FileTransferCore.h"

#include "remoting/remoting/file_transfer_common/FTMessage.h"
#include "remoting/remoting/rfb/VendorDefs.h"


namespace remoting_client
{

   namespace file_transfer
   {

      FileTransferCapability::FileTransferCapability(::subsystem::LogWriter * plogwriter)
      : m_plogwriter(plogwriter),
        m_ftReplyBuffer(m_plogwriter),
        m_ftRequestSender(m_plogwriter),
        m_ftCore(m_plogwriter,
                 &m_ftRequestSender,
                 &m_ftReplyBuffer,
                 &m_ftMessageProcessor)
      {
         m_ftMessageProcessor.addListener(&m_ftReplyBuffer);
      }

      FileTransferCapability::~FileTransferCapability()
      {
      }

      bool FileTransferCapability::isEnabled()
      {
         return m_ftCore.getSupportedOps().isFileListSupported();
      }

      void FileTransferCapability::setOutput(::remoting::RfbOutputGate *output)
      {
         m_ftRequestSender.setOutput(output);
      }

      ::remoting::file_transfer::FileTransferCore *FileTransferCapability::getCore()
      {
         return &m_ftCore;
      }

      void FileTransferCapability::setInterface(::remoting::file_transfer::FileTransferInterface *ftInterface)
      {
         m_ftCore.setInterface(ftInterface);
      }

      void FileTransferCapability::onServerMessage(::u32 code, ::DataInputStream * pinput)
      {
         m_ftMessageProcessor.processRfbMessage(pinput, code);
      }

      void FileTransferCapability::addCapabilities(CapabilitiesManager *pcapabilitiesmanager)
      {
         // Client-to-Server messages:
         pcapabilitiesmanager->addClientMsgCapability(FTMessage::COMPRESSION_SUPPORT_REQUEST,
                                         ::remoting::VendorDefs::TIGHTVNC,
                                         FTMessage::COMPRESSION_SUPPORT_REQUEST_SIG,
                                         "Compression support");

         pcapabilitiesmanager->addClientMsgCapability(FTMessage::FILE_LIST_REQUEST,
                                         ::remoting::VendorDefs::TIGHTVNC,
                                         FTMessage::FILE_LIST_REQUEST_SIG,
                                         "::file::item ::list_base request");

         pcapabilitiesmanager->addClientMsgCapability(FTMessage::MD5_REQUEST,
                                         ::remoting::VendorDefs::TIGHTVNC,
                                         FTMessage::MD5_REQUEST_SIG,
                                         "::file::item md5 sum request");

         pcapabilitiesmanager->addClientMsgCapability(FTMessage::DIRSIZE_REQUEST,
                                         ::remoting::VendorDefs::TIGHTVNC,
                                         FTMessage::DIRSIZE_REQUEST_SIG,
                                         "Directory size request");

         pcapabilitiesmanager->addClientMsgCapability(FTMessage::RENAME_REQUEST,
                                         ::remoting::VendorDefs::TIGHTVNC,
                                         FTMessage::RENAME_REQUEST_SIG,
                                         "::file::item move request");

         pcapabilitiesmanager->addClientMsgCapability(FTMessage::MKDIR_REQUEST,
                                         ::remoting::VendorDefs::TIGHTVNC,
                                         FTMessage::MKDIR_REQUEST_SIG,
                                         "Directory create request");

         pcapabilitiesmanager->addClientMsgCapability(FTMessage::REMOVE_REQUEST,
                                         ::remoting::VendorDefs::TIGHTVNC,
                                         FTMessage::REMOVE_REQUEST_SIG,
                                         "::file::item remove request");

         pcapabilitiesmanager->addClientMsgCapability(FTMessage::DOWNLOAD_START_REQUEST,
                                         ::remoting::VendorDefs::TIGHTVNC,
                                         FTMessage::DOWNLOAD_START_REQUEST_SIG,
                                         "::file::item download start request");

         pcapabilitiesmanager->addClientMsgCapability(FTMessage::DOWNLOAD_DATA_REQUEST,
                                         ::remoting::VendorDefs::TIGHTVNC,
                                         FTMessage::DOWNLOAD_DATA_REQUEST_SIG,
                                         "::file::item download data request");

         pcapabilitiesmanager->addClientMsgCapability(FTMessage::UPLOAD_START_REQUEST,
                                         ::remoting::VendorDefs::TIGHTVNC,
                                         FTMessage::UPLOAD_START_REQUEST_SIG,
                                         "::file::item upload start request");

         pcapabilitiesmanager->addClientMsgCapability(FTMessage::UPLOAD_DATA_REQUEST,
                                         ::remoting::VendorDefs::TIGHTVNC,
                                         FTMessage::UPLOAD_DATA_REQUEST_SIG,
                                         "::file::item upload data request");

         pcapabilitiesmanager->addClientMsgCapability(FTMessage::UPLOAD_END_REQUEST,
                                         ::remoting::VendorDefs::TIGHTVNC,
                                         FTMessage::UPLOAD_END_REQUEST_SIG,
                                         "::file::item upload end request");

         // Server-to-Client messages:
         pcapabilitiesmanager->addServerMsgCapability(this,
                                         FTMessage::COMPRESSION_SUPPORT_REPLY,
                                         ::remoting::VendorDefs::TIGHTVNC,
                                         FTMessage::COMPRESSION_SUPPORT_REPLY_SIG,
                                         "Compression support");

         pcapabilitiesmanager->addServerMsgCapability(this,
                                         FTMessage::FILE_LIST_REPLY,
                                         ::remoting::VendorDefs::TIGHTVNC,
                                         FTMessage::FILE_LIST_REPLY_SIG,
                                         "::file::item ::list_base reply");

         pcapabilitiesmanager->addServerMsgCapability(this,
                                         FTMessage::LAST_REQUEST_FAILED_REPLY,
                                         ::remoting::VendorDefs::TIGHTVNC,
                                         FTMessage::LAST_REQUEST_FAILED_REPLY_SIG,
                                         "Last request failed");

         pcapabilitiesmanager->addServerMsgCapability(this,
                                         FTMessage::MD5_REPLY,
                                         ::remoting::VendorDefs::TIGHTVNC,
                                         FTMessage::MD5_REPLY_SIG,
                                         "::file::item md5 sum reply");

         pcapabilitiesmanager->addServerMsgCapability(this,
                                         FTMessage::DIRSIZE_REPLY,
                                         ::remoting::VendorDefs::TIGHTVNC,
                                         FTMessage::DIRSIZE_REPLY_SIG,
                                         "Directory size reply");

         pcapabilitiesmanager->addServerMsgCapability(this,
                                         FTMessage::RENAME_REPLY,
                                         ::remoting::VendorDefs::TIGHTVNC,
                                         FTMessage::RENAME_REPLY_SIG,
                                         "::file::item move reply");

         pcapabilitiesmanager->addServerMsgCapability(this,
                                         FTMessage::MKDIR_REPLY,
                                         ::remoting::VendorDefs::TIGHTVNC,
                                         FTMessage::MKDIR_REPLY_SIG,
                                         "Directory create reply");

         pcapabilitiesmanager->addServerMsgCapability(this,
                                         FTMessage::REMOVE_REPLY,
                                         ::remoting::VendorDefs::TIGHTVNC,
                                         FTMessage::REMOVE_REPLY_SIG,
                                        "::file::item remove reply");

         pcapabilitiesmanager->addServerMsgCapability(this,
                                         FTMessage::DOWNLOAD_START_REPLY,
                                        ::remoting::VendorDefs::TIGHTVNC,
                                        FTMessage::DOWNLOAD_START_REPLY_SIG,
                                        "::file::item download start reply");

         pcapabilitiesmanager->addServerMsgCapability(this,
                                         FTMessage::DOWNLOAD_DATA_REPLY,
                                         ::remoting::VendorDefs::TIGHTVNC,
                                         FTMessage::DOWNLOAD_DATA_REPLY_SIG,
                                         "::file::item download data reply");

         pcapabilitiesmanager->addServerMsgCapability(this,
                                         FTMessage::DOWNLOAD_END_REPLY,
                                         ::remoting::VendorDefs::TIGHTVNC,
                                         FTMessage::DOWNLOAD_END_REPLY_SIG,
                                         "::file::item download end reply");

         pcapabilitiesmanager->addServerMsgCapability(this,
                                         FTMessage::UPLOAD_START_REPLY,
                                         ::remoting::VendorDefs::TIGHTVNC,
                                         FTMessage::UPLOAD_START_REPLY_SIG,
                                         "::file::item upload start reply");

         pcapabilitiesmanager->addServerMsgCapability(this,
                                         FTMessage::UPLOAD_DATA_REPLY,
                                         ::remoting::VendorDefs::TIGHTVNC,
                                         FTMessage::UPLOAD_DATA_REPLY_SIG,
                                         "::file::item upload data reply");

         pcapabilitiesmanager->addServerMsgCapability(this,
                                         FTMessage::UPLOAD_END_REPLY,
                                         ::remoting::VendorDefs::TIGHTVNC,
                                         FTMessage::UPLOAD_END_REPLY_SIG,
                                         "::file::item upload end reply");
      }
   }
}