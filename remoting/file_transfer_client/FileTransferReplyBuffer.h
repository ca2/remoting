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

#include "acme/input_output/DataInputStream.h"

#include "remoting/remoting/file_transfer_common/FileInfo.h"
#include "subsystem/platform/Inflater.h"
#include "subsystem/platform/ZLibException.h"

#include "remoting/remoting/file_transfer_common/OperationNotSupportedException.h"

//#include "log_writer/LogWriter.h"

namespace  remoting
{
   namespace file_transfer
   {
      class CLASS_DECL_REMOTING FileTransferReplyBuffer : public FileTransferEventHandler
      {
      public:
         FileTransferReplyBuffer(::subsystem::LogWriter * plogwriter);
         virtual ~FileTransferReplyBuffer();

         ::string getLastErrorMessage();

         bool isCompressionSupported();

         //::u32 getFilesInfoCount();
         //FileInfo *getFilesInfo();

         ::u32 getDownloadBufferSize();
         ::array_base<unsigned char> getDownloadBuffer();

         unsigned char getDownloadFileFlags();
         ::u64 getDownloadLastModified();

         ::u64 getDirSize();

         //
         // Inherited from FileTransferEventHandler abstract class
         //

         virtual void onCompressionSupportReply(::DataInputStream * pinput);
         virtual void onFileListReply(::DataInputStream * pinput);
         virtual void onMd5DataReply(::DataInputStream * pinput);

         virtual void onUploadReply(::DataInputStream * pinput);
         virtual void onUploadDataReply(::DataInputStream * pinput);
         virtual void onUploadEndReply(::DataInputStream * pinput);

         virtual void onDownloadReply(::DataInputStream * pinput);
         virtual void onDownloadDataReply(::DataInputStream * pinput);
         virtual void onDownloadEndReply(::DataInputStream * pinput);

         virtual void onMkdirReply(::DataInputStream * pinput);
         virtual void onRmReply(::DataInputStream * pinput);
         virtual void onMvReply(::DataInputStream * pinput);

         virtual void onDirSizeReply(::DataInputStream * pinput);
         virtual void onLastRequestFailedReply(::DataInputStream * pinput);

      //private:

         ::array_base<unsigned char> readCompressedDataBlock(::DataInputStream * pinput,
                                               ::u32 compressedSize,
                                               ::u32 uncompressedSize,
                                               unsigned char compressionLevel)
       ;

//      protected:

         //
         // Interface of log writer for log data
         //
         ::pointer < ::subsystem::LogWriter > m_plogwriter;

         //
         // ZLib stream for decompression of compressed data
         //
         ::subsystem::Inflater m_inflater;

         //
         // Members than have access from public methods
         //

         // Compression support reply
         bool m_isCompressionSupported;

         // ::file::item ::list_base reply
         //::u32 m_filesInfoCount;
         ::pointer_array <FileInfo > m_fileinfoa;

         // Last request scopedstrMessage failed reply
         ::string m_lastErrorMessage;

         // Download data reply
         ::array_base<unsigned char> m_downloadBuffer;
         ::u32 m_downloadBufferSize;

         // Download end reply
         unsigned char m_downloadFileFlags;
         ::u64 m_downloadLastModified;

         // Dirsize reply data
         ::u64 m_dirSize;
      };
   } //    namespace file_transfer
} // namespace remoting

