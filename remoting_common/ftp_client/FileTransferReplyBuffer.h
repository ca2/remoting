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


#include "FileTransferEventHandler.h"

#include "remoting/remoting_common/io/DataInputStream.h"

#include "remoting/remoting_common/ftp_common/FileInfo.h"
#include "remoting/remoting_common/util/Inflater.h"
#include "remoting/remoting_common/util/ZLibException.h"

#include "remoting/remoting_common/ftp_common/OperationNotSupportedException.h"

//#include "log_writer/LogWriter.h"

namespace  remoting
{
   namespace  ftp
   {
      class CLASS_DECL_REMOTING_COMMON FileTransferReplyBuffer : public FileTransferEventHandler
      {
      public:
         FileTransferReplyBuffer(LogWriter *logWriter);
         virtual ~FileTransferReplyBuffer();

         ::string getLastErrorMessage();

         bool isCompressionSupported();

         //unsigned int getFilesInfoCount();
         //FileInfo *getFilesInfo();

         unsigned int getDownloadBufferSize();
         ::array_base<unsigned char> getDownloadBuffer();

         unsigned char getDownloadFileFlags();
         unsigned long long getDownloadLastModified();

         unsigned long long getDirSize();

         //
         // Inherited from FileTransferEventHandler abstract class
         //

         virtual void onCompressionSupportReply(DataInputStream * pinput);
         virtual void onFileListReply(DataInputStream * pinput);
         virtual void onMd5DataReply(DataInputStream * pinput);

         virtual void onUploadReply(DataInputStream * pinput);
         virtual void onUploadDataReply(DataInputStream * pinput);
         virtual void onUploadEndReply(DataInputStream * pinput);

         virtual void onDownloadReply(DataInputStream * pinput);
         virtual void onDownloadDataReply(DataInputStream * pinput);
         virtual void onDownloadEndReply(DataInputStream * pinput);

         virtual void onMkdirReply(DataInputStream * pinput);
         virtual void onRmReply(DataInputStream * pinput);
         virtual void onMvReply(DataInputStream * pinput);

         virtual void onDirSizeReply(DataInputStream * pinput);
         virtual void onLastRequestFailedReply(DataInputStream * pinput);

      //private:

         ::array_base<unsigned char> readCompressedDataBlock(DataInputStream * pinput,
                                               unsigned int compressedSize,
                                               unsigned int uncompressedSize,
                                               unsigned char compressionLevel)
       ;

//      protected:

         //
         // Interface of log writer for log data
         //
         LogWriter *m_logWriter;

         //
         // ZLib stream for decompression of compressed data
         //
         Inflater m_inflater;

         //
         // Members than have access from public methods
         //

         // Compression support reply
         bool m_isCompressionSupported;

         // ::file::item ::list_base reply
         //unsigned int m_filesInfoCount;
         ::pointer_array <FileInfo > m_fileinfoa;

         // Last request scopedstrMessage failed reply
         ::string m_lastErrorMessage;

         // Download data reply
         ::array_base<unsigned char> m_downloadBuffer;
         unsigned int m_downloadBufferSize;

         // Download end reply
         unsigned char m_downloadFileFlags;
         unsigned long long m_downloadLastModified;

         // Dirsize reply data
         unsigned long long m_dirSize;
      };
   } //    namespace  ftp
} // namespace remoting

