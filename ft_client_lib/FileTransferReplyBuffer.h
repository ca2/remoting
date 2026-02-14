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
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//

#pragma once


#include "FileTransferEventHandler.h"

#include "io_lib/DataInputStream.h"

#include "ft_common/FileInfo.h"
#include "util/Inflater.h"
#include "util/ZLibException.h"

#include "ft_common/OperationNotSupportedException.h"

#include "log_writer/LogWriter.h"

class FileTransferReplyBuffer : public FileTransferEventHandler
{
public:
  FileTransferReplyBuffer(LogWriter *logWriter);
  virtual ~FileTransferReplyBuffer();

  void getLastErrorMessage(::string & storage);

  bool isCompressionSupported();

  unsigned int getFilesInfoCount();
  FileInfo *getFilesInfo();

  unsigned int getDownloadBufferSize();
  ::array_base<unsigned char> getDownloadBuffer();

  unsigned char getDownloadFileFlags();
  unsigned long long getDownloadLastModified();

  unsigned long long getDirSize();

  //
  // Inherited from FileTransferEventHandler abstract class
  //

  virtual void onCompressionSupportReply(DataInputStream *input);
  virtual void onFileListReply(DataInputStream *input);
  virtual void onMd5DataReply(DataInputStream *input);

  virtual void onUploadReply(DataInputStream *input);
  virtual void onUploadDataReply(DataInputStream *input);
  virtual void onUploadEndReply(DataInputStream *input);

  virtual void onDownloadReply(DataInputStream *input);
  virtual void onDownloadDataReply(DataInputStream *input);
  virtual void onDownloadEndReply(DataInputStream *input);

  virtual void onMkdirReply(DataInputStream *input);
  virtual void onRmReply(DataInputStream *input);
  virtual void onMvReply(DataInputStream *input);

  virtual void onDirSizeReply(DataInputStream *input);
  virtual void onLastRequestFailedReply(DataInputStream *input);

private:

  ::array_base<unsigned char> readCompressedDataBlock(DataInputStream *input,
                                        unsigned int compressedSize,
                                        unsigned int uncompressedSize,
                                        unsigned char compressionLevel)
;

protected:

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

  // File ::list reply
  unsigned int m_filesInfoCount;
  FileInfo *m_filesInfo;

  // Last request message failed reply
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


