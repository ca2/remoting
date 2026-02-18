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
#include "framework.h"
#include "FileTransferReplyBuffer.h"
#include "io_lib/ByteArrayInputStream.h"
#include "thread/AutoLock.h"
#include <crtdbg.h>

namespace  remoting
{
   namespace  ftp
   {
      FileTransferReplyBuffer::FileTransferReplyBuffer(LogWriter *logWriter)
      : m_logWriter(logWriter),
        m_isCompressionSupported(false),
        //m_filesInfoCount(0), m_filesInfo(NULL),
        m_downloadBufferSize(0),
        m_downloadFileFlags(0), m_downloadLastModified(0),
        m_dirSize(0)
      {
         m_lastErrorMessage= "";
      }

      FileTransferReplyBuffer::~FileTransferReplyBuffer()
      {
         //if (m_filesInfo != NULL) {
           // delete[] m_filesInfo;
         //}
      }

      ::string FileTransferReplyBuffer::getLastErrorMessage()
      {
         return m_lastErrorMessage;
      }

      bool FileTransferReplyBuffer::isCompressionSupported()
      {
         return m_isCompressionSupported;
      }

      // unsigned int FileTransferReplyBuffer::getFilesInfoCount()
      // {
      //    return m_filesInfoCount;
      // }
      //
      // FileInfo *FileTransferReplyBuffer::getFilesInfo()
      // {
      //    return m_filesInfo;
      // }

      unsigned int FileTransferReplyBuffer::getDownloadBufferSize()
      {
         return m_downloadBufferSize;
      }

      unsigned char FileTransferReplyBuffer::getDownloadFileFlags()
      {
         return m_downloadFileFlags;
      }

      unsigned long long FileTransferReplyBuffer::getDownloadLastModified()
      {
         return m_downloadLastModified;
      }

      unsigned long long FileTransferReplyBuffer::getDirSize()
      {
         return m_dirSize;
      }

      ::array_base<unsigned char> FileTransferReplyBuffer::getDownloadBuffer()
      {
         return m_downloadBuffer;
      }

      void FileTransferReplyBuffer::onCompressionSupportReply(DataInputStream * pinput)
      {
         m_isCompressionSupported = (pinput->readUInt8() == 1);

         m_logWriter->information("Received compression support reply: {}\n",
                           m_isCompressionSupported ? "supported" : "not supported");
      }

      void FileTransferReplyBuffer::onFileListReply(DataInputStream * pinput)
      {
         unsigned char compressionLevel = 0;
         unsigned int compressedSize = 0;
         unsigned int uncompressedSize = 0;

         ::array_base<unsigned char> buffer;

         {
            compressionLevel = pinput->readUInt8();
            compressedSize = pinput->readUInt32();
            uncompressedSize = pinput->readUInt32();

            buffer = readCompressedDataBlock(pinput,
                                             compressedSize,
                                             uncompressedSize,
                                             compressionLevel);
         }

         if (!buffer.empty()) {
            // FIXME: type conversion in C-style
            ByteArrayInputStream memoryInputStream(reinterpret_cast<char *>(buffer.data()),
                                                   uncompressedSize);
            DataInputStream filesInfoReader(&memoryInputStream);

            // if (m_filesInfo != 0) {
            //    delete[] m_filesInfo;
            //    m_filesInfo = 0;
            // }

            auto filesInfoCount = filesInfoReader.readUInt32();

            m_fileinfoa.set_size(filesInfoCount);
            //if (m_filesInfoCount != 0) {
//               m_filesInfo = new FileInfo[m_filesInfoCount];
//            }

            for (unsigned int i = 0; i < filesInfoCount; i++)
         {

               auto & pfileinfo = m_fileinfoa[i];

               øconstruct_new(pfileinfo);

               pfileinfo->setSize(filesInfoReader.readUInt64());
               pfileinfo->setLastModified(filesInfoReader.readUInt64());
               pfileinfo->setFlags(filesInfoReader.readUInt16());

               auto strFileName = filesInfoReader.read_utf8_string();

               pfileinfo->setFileName(strFileName);
            } // for all newly created file's info

            m_logWriter->information("Received file ::list_base reply: \n"
                              "\t files count = {}\n"
                              "\t use compression = {}\n",
                              filesInfoCount, compressionLevel);
         } else {
            m_logWriter->information("Received file ::list_base reply is not read: "
                              "compressed buffer is empty");
         }
      }

      void FileTransferReplyBuffer::onMd5DataReply(DataInputStream * pinput)
      {
         throw OperationNotSupportedException();
      }

      void FileTransferReplyBuffer::onUploadReply(DataInputStream * pinput)
      {
         m_logWriter->information("Received upload reply\n");
      }

      void FileTransferReplyBuffer::onUploadDataReply(DataInputStream * pinput)
      {
         m_logWriter->information("Received upload data reply\n");
      }

      void FileTransferReplyBuffer::onUploadEndReply(DataInputStream * pinput)
      {
         m_logWriter->information("Received upload end reply\n");
      }

      void FileTransferReplyBuffer::onDownloadReply(DataInputStream * pinput)
      {
         m_logWriter->information("Received download reply\n");
      }

      void FileTransferReplyBuffer::onDownloadDataReply(DataInputStream * pinput)
      {
         unsigned char coLevel = pinput->readUInt8();
         unsigned int coBufferSize = pinput->readUInt32();
         unsigned int uncoBufferSize = pinput->readUInt32();

         m_downloadBuffer = readCompressedDataBlock(pinput, coBufferSize, uncoBufferSize, coLevel);
         m_downloadBufferSize = uncoBufferSize;

         m_logWriter->information("Received download data reply:\n"
                           "\tcompressed size: {}\n"
                           "\tuncompressed size: {}\n"
                           "\tuse compression: {}\n",
                           coBufferSize, uncoBufferSize, coLevel);
      }

      void FileTransferReplyBuffer::onDownloadEndReply(DataInputStream * pinput)
      {
         m_downloadFileFlags = pinput->readUInt8();
         m_downloadLastModified = pinput->readUInt64();

         m_logWriter->information("Received download end reply:\n"
                           "\tfile flags: {}\n"
                           "\tmodification time: {}\n",
                           m_downloadFileFlags, m_downloadLastModified);
      }

      void FileTransferReplyBuffer::onMkdirReply(DataInputStream * pinput)
      {
         m_logWriter->information("Received mkdir reply\n");
      }

      void FileTransferReplyBuffer::onRmReply(DataInputStream * pinput)
      {
         m_logWriter->information("Received rm reply\n");
      }

      void FileTransferReplyBuffer::onMvReply(DataInputStream * pinput)
      {
         m_logWriter->information("Received rename reply\n");
      }

      void FileTransferReplyBuffer::onDirSizeReply(DataInputStream * pinput)
      {
         m_dirSize = pinput->readUInt64();

         m_logWriter->information("Received dirsize reply\n");
      }


      void FileTransferReplyBuffer::onLastRequestFailedReply(DataInputStream * pinput)
      {

         m_lastErrorMessage = pinput->read_utf8_string();

         m_logWriter->information("Received last request failed reply:\n"
                           "\terror scopedstrMessage: {}\n",
                           m_lastErrorMessage);
      }


      ::array_base<unsigned char> FileTransferReplyBuffer::readCompressedDataBlock(DataInputStream * pinput,
                                                                     unsigned int compressedSize,
                                                                     unsigned int uncompressedSize,
                                                                     unsigned char compressionLevel)
      {
         //
         // Buffers with compressed and uncompressed data.
         // When not using compression uncoBuffer = coBuffer.
         //

         unsigned int coSize = compressedSize;
         unsigned int uncoSize = uncompressedSize;

         ::array_base<unsigned char> coBuffer(coSize);

         //
         // Read compressed data
         //

         if (coSize == 0) {
            return coBuffer;
         }

         pinput->readFully(coBuffer.data(), coSize);

         if (compressionLevel == 0) {
            return coBuffer;
         }

         ::array_base<unsigned char> uncoBuffer(uncoSize);

         m_inflater.setUnpackedSize(uncoSize);
         // FIXME: type conversion in C-style
         m_inflater.setInput((const char*)coBuffer.data(), coSize);

         m_inflater.inflate();

         _ASSERT(m_inflater.getOutputSize() == uncoSize);

         if (uncoSize != 0) {
            memcpy(uncoBuffer.data(), m_inflater.getOutput(), uncoSize);
         }

         return uncoBuffer;
      }
   } // namespace  ftp
} // namespace remoting