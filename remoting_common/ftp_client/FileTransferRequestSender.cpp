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
#include "framework.h"
#include "FileTransferRequestSender.h"
#include "remoting/remoting_common/ftp_common/FTMessage.h"
#include "remoting/remoting_common/thread/AutoLock.h"


namespace remoting
{
   namespace ftp
   {
      FileTransferRequestSender::FileTransferRequestSender(LogWriter *logWriter)
      : m_logWriter(logWriter),
        m_output(0)
      {
      }

      FileTransferRequestSender::~FileTransferRequestSender()
      {
      }

      void FileTransferRequestSender::setOutput(RfbOutputGate *outputStream)
      {
         m_output = outputStream;
      }

      void FileTransferRequestSender::sendCompressionSupportRequest()
      {
         AutoLock al(m_output);

         m_logWriter->information("{}", "Sending compresion support request");

         m_output->writeUInt32(FTMessage::COMPRESSION_SUPPORT_REQUEST);
         m_output->flush();
      }

      void FileTransferRequestSender::sendFileListRequest(const ::scoped_string & scopedstrFullPath,
                                                          bool useCompression)
      {
         AutoLock al(m_output);

         unsigned int messageId = FTMessage::FILE_LIST_REQUEST;
         unsigned char compressionLevel = useCompression ? (unsigned char)1 : (unsigned char)0;

         m_logWriter->information("Sending file ::list_base request with parameters:\n"
                           "\tpath = {}\n"
                           "\tuse compression = {}\n",
                           scopedstrFullPath,
                           useCompression ? 1 : 0);

         m_output->writeUInt32(messageId);
         m_output->writeUInt8(compressionLevel);
         m_output->writeUTF8(scopedstrFullPath);
         m_output->flush();
      }

      void FileTransferRequestSender::sendDownloadRequest(const ::scoped_string & scopedstrFullPathName,
                                                          unsigned long long offset)
      {
         AutoLock al(m_output);

         m_logWriter->information("Sending download request with parameters:\n"
                           "\tpath = {}\n"
                           "\toffset = {}\n",
                           scopedstrFullPathName, offset);

         m_output->writeUInt32(FTMessage::DOWNLOAD_START_REQUEST);
         m_output->writeUTF8(scopedstrFullPathName);
         m_output->writeUInt64(offset);
         m_output->flush();
      }

      void FileTransferRequestSender::sendDownloadDataRequest(unsigned int size,
                                                              bool useCompression)
      {
         AutoLock al(m_output);

         unsigned char compressionLevel = useCompression ? (unsigned char)1 : (unsigned char)0;

         m_logWriter->information("Sending download data request with parameters:\n"
                           "\tsize = {}\n"
                           "\tuse compression = {}\n",
                           size,
                           compressionLevel);

         m_output->writeUInt32(FTMessage::DOWNLOAD_DATA_REQUEST);
         m_output->writeUInt8(compressionLevel);
         m_output->writeUInt32(size);
         m_output->flush();
      }

      void FileTransferRequestSender::sendRmFileRequest(const ::scoped_string & scopedstrFullPathName)
      {
         AutoLock al(m_output);

         m_logWriter->information("Sending rm file request with parameters:\n\tpath = {}\n",
                           ::string(scopedstrFullPathName).c_str());

         m_output->writeUInt32(FTMessage::REMOVE_REQUEST);
         m_output->writeUTF8(scopedstrFullPathName);
         m_output->flush();
      }

      void FileTransferRequestSender::sendMkDirRequest(const ::scoped_string & scopedstrFullPathName)
      {
         AutoLock al(m_output);

         m_logWriter->information("Sending mkdir request with parameters:\n\tpath = {}\n",
                           ::string(scopedstrFullPathName).c_str());

         m_output->writeUInt32(FTMessage::MKDIR_REQUEST);
         m_output->writeUTF8(scopedstrFullPathName);
         m_output->flush();
      }

      void FileTransferRequestSender::sendMvFileRequest(const ::scoped_string & scopedstroldFileName,
                                                        const ::scoped_string & scopedstrNewFileName)
      {
         AutoLock al(m_output);

         m_logWriter->information("Sending rename file request with parameters:\n"
                           "\t old path = {}\n"
                           "\t new path = {}\n",
                           scopedstroldFileName,
                           scopedstrNewFileName);

         m_output->writeUInt32(FTMessage::RENAME_REQUEST);
         m_output->writeUTF8(scopedstroldFileName);
         m_output->writeUTF8(scopedstrNewFileName);
         m_output->flush();
      }

      void FileTransferRequestSender::sendUploadRequest(const ::scoped_string & scopedstrFullPathName,
                                                        bool overwrite,
                                                        unsigned long long offset)
      {
         AutoLock al(m_output);

         unsigned char flags = 0;
         if (overwrite) {
            flags = 0x1;
         }

         m_logWriter->information("Sending upload request with parameters:\n"
                           "\tpath = {}\n"
                           "\toverwrite flag = {}\n"
                           "\toffset = {}\n",
                           scopedstrFullPathName,
                           overwrite ? 1 : 0,
                           offset);

         m_output->writeUInt32(FTMessage::UPLOAD_START_REQUEST);
         m_output->writeUTF8(scopedstrFullPathName);
         m_output->writeUInt8(flags);
         m_output->writeUInt64(offset);
         m_output->flush();
      }

      void FileTransferRequestSender::sendUploadDataRequest(const char *buffer,
                                                            unsigned int size,
                                                            bool useCompression)
      {
         AutoLock al(m_output);

         m_output->writeUInt32(FTMessage::UPLOAD_DATA_REQUEST);

         //
         // FIXME: Compression is not supported for now
         // TODO: Compression support on upload
         //

         if (useCompression) {
            throw ::io_exception(error_io, "Compression is not supported yet.");
         }

         unsigned char compressionLevel = useCompression ? (short)1 : (short)0;

         m_logWriter->information("Sending upload data request with parameters:\n"
                           "\tsize = {}\n"
                           "\tuse compression = {}\n",
                           size,
                           compressionLevel);

         m_output->writeUInt8(compressionLevel);
         m_output->writeUInt32(size);
         m_output->writeUInt32(size);
         m_output->write(buffer, size);
         m_output->flush();
      }

      void FileTransferRequestSender::sendUploadEndRequest(unsigned char fileFlags,
                                                           unsigned long long modificationTime)
      {
         AutoLock al(m_output);

         m_logWriter->information("Sending upload end request with parameters:\n"
                           "\tflags = {}\n"
                           "\tmodification time = {}\n",
                           fileFlags,
                           modificationTime);

         m_output->writeUInt32(FTMessage::UPLOAD_END_REQUEST);
         m_output->writeUInt16(fileFlags);
         m_output->writeUInt64(modificationTime);
         m_output->flush();
      }

      void FileTransferRequestSender::sendFolderSizeRequest(const ::scoped_string & scopedstrFullPath)
      {
         AutoLock al(m_output);

         m_logWriter->information("Sending get folder size request with parameters:\n\tpath = {}\n",
                           scopedstrFullPath);

         m_output->writeUInt32(FTMessage::DIRSIZE_REQUEST);
         m_output->writeUTF8(scopedstrFullPath);
         m_output->flush();
      }
   }
}