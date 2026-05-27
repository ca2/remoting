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
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//
#include "framework.h"
#include "FileTransferRequestHandler.h"
#include "FileTransferSecurity.h"
#include "remoting/remoting_rtc/rfb_sconn/RfbCodeRegistrator.h"
#include "remoting/remoting_rtc/file_transfer_common/FileInfo.h"
#include "remoting/remoting_rtc/file_transfer_common/FolderListener.h"
#include "remoting/remoting_rtc/file_transfer_common/FTMessage.h"
#include "remoting/remoting_rtc/rfb/VendorDefs.h"
#include "remoting/remoting_rtc/node_config/Configurator.h"
#include "remoting/remoting_rtc/network/RfbOutputGate.h"
#include "acme/input_output/ByteArrayOutputStream.h"
// #include "io-lib/DataOutputStream.h"
//
// #include "ft-common/FileTransferException.h"
// #include "io-lib/ByteArrayOutputStream.h"
// #include "file-lib/File.h"
// #include "file-lib/EOFException.h"
// #include "ft-common/FolderListener.h"
// #include "ft-common/FTMessage.h"
// #include "ft-common/::string.h"
// #include "ft-common/FileInfo.h"
// #include "util/md5.h"
// #include "network/RfbOutputGate.h"
// #include "network/::remoting_rtc::RfbInputGate.h"
// #include "thread/AutoLock.h"
// #include "win-system/Impersonator.h"
// #include "win-system/Environment.h"
// #include "server-config-lib/Configurator.h"
// #include "win-system/SystemException.h"
// #include "rfb/VendorDefs.h"
#include "subsystem/platform/MD5.h"

namespace remoting
{

   namespace file_transfer
   {


      FileTransferRequestHandler::FileTransferRequestHandler(::remoting_rtc_node::Configurator * pconfigurator,
         RfbCodeRegistrator *registrator,
                                                             RfbOutputGate *prfboutputgate,
                                                             Desktop *pdesktop,
                                                             ::subsystem::LogWriter *plogwriter,
                                                             bool enabled)
      :
      m_pconfigurator(pconfigurator),
      //m_pfileDownload(NULL), m_pfileInputStream(NULL),
        //m_pfileUpload(NULL), m_pfileOutputStream(NULL),
        m_prfboutputgate(prfboutputgate),
        //m_enabled(enabled),
        m_plogwriter(plogwriter)
      {
         m_pfiletransfersecurity = create_newø <FileTransferSecurity>();
         
         m_pfiletransfersecurity->initialize_file_transfer_security(pconfigurator, pdesktop, plogwriter);
         

         if (!FileTransferRequestHandler::isFileTransferEnabled()) {
            return ;
         }

         registrator->addSrvToClCap(FTMessage::COMPRESSION_SUPPORT_REPLY, VendorDefs::REMOTING, FTMessage::COMPRESSION_SUPPORT_REPLY_SIG);
         registrator->addSrvToClCap(FTMessage::FILE_LIST_REPLY, VendorDefs::REMOTING, FTMessage::FILE_LIST_REPLY_SIG);
         registrator->addSrvToClCap(FTMessage::MD5_REPLY, VendorDefs::REMOTING, FTMessage::MD5_REPLY_SIG);
         registrator->addSrvToClCap(FTMessage::UPLOAD_START_REPLY, VendorDefs::REMOTING, FTMessage::UPLOAD_START_REPLY_SIG);
         registrator->addSrvToClCap(FTMessage::UPLOAD_DATA_REPLY, VendorDefs::REMOTING, FTMessage::UPLOAD_DATA_REPLY_SIG);
         registrator->addSrvToClCap(FTMessage::UPLOAD_END_REPLY, VendorDefs::REMOTING, FTMessage::UPLOAD_END_REPLY_SIG);
         registrator->addSrvToClCap(FTMessage::DOWNLOAD_START_REPLY, VendorDefs::REMOTING, FTMessage::DOWNLOAD_START_REPLY_SIG);
         registrator->addSrvToClCap(FTMessage::DOWNLOAD_DATA_REPLY, VendorDefs::REMOTING, FTMessage::DOWNLOAD_DATA_REPLY_SIG);
         registrator->addSrvToClCap(FTMessage::DOWNLOAD_END_REPLY, VendorDefs::REMOTING, FTMessage::DOWNLOAD_END_REPLY_SIG);
         registrator->addSrvToClCap(FTMessage::MKDIR_REPLY, VendorDefs::REMOTING, FTMessage::MKDIR_REPLY_SIG);
         registrator->addSrvToClCap(FTMessage::REMOVE_REPLY, VendorDefs::REMOTING, FTMessage::REMOVE_REPLY_SIG);
         registrator->addSrvToClCap(FTMessage::RENAME_REPLY, VendorDefs::REMOTING, FTMessage::RENAME_REPLY_SIG);
         registrator->addSrvToClCap(FTMessage::DIRSIZE_REPLY, VendorDefs::REMOTING, FTMessage::DIRSIZE_REPLY_SIG);
         registrator->addSrvToClCap(FTMessage::LAST_REQUEST_FAILED_REPLY, VendorDefs::REMOTING, FTMessage::LAST_REQUEST_FAILED_REPLY_SIG);

         registrator->addClToSrvCap(FTMessage::COMPRESSION_SUPPORT_REQUEST, VendorDefs::REMOTING, FTMessage::COMPRESSION_SUPPORT_REQUEST_SIG);
         registrator->addClToSrvCap(FTMessage::FILE_LIST_REQUEST, VendorDefs::REMOTING, FTMessage::FILE_LIST_REQUEST_SIG);
         registrator->addClToSrvCap(FTMessage::MD5_REQUEST, VendorDefs::REMOTING, FTMessage::MD5_REQUEST_SIG);
         registrator->addClToSrvCap(FTMessage::UPLOAD_START_REQUEST, VendorDefs::REMOTING, FTMessage::UPLOAD_START_REQUEST_SIG);
         registrator->addClToSrvCap(FTMessage::UPLOAD_DATA_REQUEST, VendorDefs::REMOTING, FTMessage::UPLOAD_DATA_REQUEST_SIG);
         registrator->addClToSrvCap(FTMessage::UPLOAD_END_REQUEST, VendorDefs::REMOTING, FTMessage::UPLOAD_END_REQUEST_SIG);
         registrator->addClToSrvCap(FTMessage::DOWNLOAD_START_REQUEST, VendorDefs::REMOTING, FTMessage::DOWNLOAD_START_REQUEST_SIG);
         registrator->addClToSrvCap(FTMessage::DOWNLOAD_DATA_REQUEST, VendorDefs::REMOTING, FTMessage::DOWNLOAD_DATA_REQUEST_SIG);
         registrator->addClToSrvCap(FTMessage::MKDIR_REQUEST, VendorDefs::REMOTING, FTMessage::MKDIR_REQUEST_SIG);
         registrator->addClToSrvCap(FTMessage::REMOVE_REQUEST, VendorDefs::REMOTING, FTMessage::REMOVE_REQUEST_SIG);
         registrator->addClToSrvCap(FTMessage::RENAME_REQUEST, VendorDefs::REMOTING, FTMessage::RENAME_REQUEST_SIG);
         registrator->addClToSrvCap(FTMessage::DIRSIZE_REQUEST, VendorDefs::REMOTING, FTMessage::DIRSIZE_REQUEST_SIG);

         ::u32 rfbMessagesToProcess[] = {
            FTMessage::COMPRESSION_SUPPORT_REQUEST,
            FTMessage::FILE_LIST_REQUEST,
            FTMessage::MD5_REQUEST,
            FTMessage::UPLOAD_START_REQUEST,
            FTMessage::UPLOAD_DATA_REQUEST,
            FTMessage::UPLOAD_END_REQUEST,
            FTMessage::DOWNLOAD_START_REQUEST,
            FTMessage::DOWNLOAD_DATA_REQUEST,
            FTMessage::MKDIR_REQUEST,
            FTMessage::REMOVE_REQUEST,
            FTMessage::RENAME_REQUEST,
            FTMessage::DIRSIZE_REQUEST
          };

         for (size_t i = 0; i < sizeof(rfbMessagesToProcess) / sizeof(::u32); i++) {
            registrator->regCode(rfbMessagesToProcess[i], this);
         }

         m_plogwriter->debug("File transfer request handler created");
      }

      FileTransferRequestHandler::~FileTransferRequestHandler()
      {
         // delete m_security;
         //
         // if (m_pfileInputStream != NULL) {
         //    delete m_pfileInputStream;
         // }
         // if (m_pfileDownload != NULL) {
         //    delete m_pfileDownload;
         // }
         // if (m_pfileOutputStream != NULL) {
         //    delete m_pfileOutputStream;
         // }
         // if (m_pfileUpload != NULL) {
         //    delete m_pfileUpload;
         // }

         m_pfiletransfersecurity.defer_destroy_and_release();
         m_pfileDownload.defer_destroy_and_release();
         m_pfileUpload.defer_destroy_and_release();
         m_pfileInputStream.defer_destroy_and_release();
         m_pfileOutputStream.defer_destroy_and_release();

         m_plogwriter->debug("File transfer request handler deleted");
      }

      void FileTransferRequestHandler::onRequest(::u32 reqCode, ::remoting_rtc::RfbInputGate *backGate)
      {
         m_pfiletransfersecurity->beginMessageProcessing();

         m_prfbinputgate = backGate;

         try {
            switch (reqCode) {
               case FTMessage::COMPRESSION_SUPPORT_REQUEST:
                  compressionSupportRequested();
                  break;
               case FTMessage::FILE_LIST_REQUEST:
                  fileListRequested();
                  break;
               case FTMessage::MKDIR_REQUEST:
                  mkDirRequested();
                  break;
               case FTMessage::REMOVE_REQUEST:
                  rmFileRequested();
                  break;
               case FTMessage::RENAME_REQUEST:
                  mvFileRequested();
                  break;
               case FTMessage::DIRSIZE_REQUEST:
                  dirSizeRequested();
                  break;
               case FTMessage::UPLOAD_START_REQUEST:
                  uploadStartRequested();
                  break;
               case FTMessage::UPLOAD_DATA_REQUEST:
                  uploadDataRequested();
                  break;
               case FTMessage::UPLOAD_END_REQUEST:
                  uploadEndRequested();
                  break;
               case FTMessage::DOWNLOAD_START_REQUEST:
                  downloadStartRequested();
                  break;
               case FTMessage::DOWNLOAD_DATA_REQUEST:
                  downloadDataRequested();
                  break;
               case FTMessage::MD5_REQUEST:
                  md5Requested();
                  break;
            } // switch.
         } catch (::subsystem::Exception &someEx) {
            lastRequestFailed(someEx.get_message());
         } // try / catch.

         m_prfbinputgate.defer_destroy_and_release();

         m_pfiletransfersecurity->endMessageProcessing();
      }

      bool FileTransferRequestHandler::isFileTransferEnabled()
      {
         return m_enabled && m_pconfigurator->getServerConfig()->isFileTransfersEnabled();
      }

      void FileTransferRequestHandler::compressionSupportRequested()
      {
         m_plogwriter->debug("%s", "compression support requested");

         //
         // Can be 0 - compression not supported by server
         //     or 1 - compression is supported by server
         //

         ::u8 compressionSupport = 1;

         m_plogwriter->debug("sending compression support reply: {}", (compressionSupport == 1) ? "supported" : "not supported");

         {
            AutoLock l(m_prfboutputgate);

            m_prfboutputgate->writeUInt32(FTMessage::COMPRESSION_SUPPORT_REPLY);
            m_prfboutputgate->writeUInt8(compressionSupport);

            m_prfboutputgate->flush();
         }
      }

      void FileTransferRequestHandler::fileListRequested()
      {
         ::u8 requestedCompressionLevel;
         //::string fullPathName;
         ::string strFullPathName;

         //
         // Read input data
         //

         {
            requestedCompressionLevel = m_prfbinputgate->readUInt8();

            strFullPathName = m_prfbinputgate->readUtf8();
         }

         m_plogwriter->debug("File list of folder '{}' requested",
                      strFullPathName);

         checkAccess();

         ::u8 compressionLevel = requestedCompressionLevel;
         ::u32 compressedSize = 0;
         ::u32 uncompressedSize = 0;
         ::u32 filesCount = 0;
         ::u32 filesInfoDataSize = 0;
         //const ::remoting_rtc::file_transfer::FileInfo *files = NULL;

         //
         // Get file list from specified folder
         //

         ::remoting_rtc::file_transfer::FolderListener folderListener(this, strFullPathName);

         if (!folderListener.list()) {
            throw ::subsystem::SystemException();
         }

         //files = folderListener.getFilesInfo();
         filesCount = folderListener.m_fileinfoa.size();

         //
         // Create buffer with "CompressedData" block inside
         //

         ByteArrayOutputStream memStream;
         DataOutputStream outMemStream(&memStream);

         outMemStream.writeUInt32(filesCount);
         for (auto pfile : folderListener.m_fileinfoa) {
            outMemStream.writeUInt64(pfile->getSize());
            outMemStream.writeUInt64(pfile->lastModified());
            outMemStream.writeUInt16(pfile->getFlags());
            outMemStream.writeUTF8(pfile->getFileName());
         } // for

         ASSERT((::u32)memStream.size() == memStream.size());
         uncompressedSize = (::u32)memStream.size();

         //
         // Buffer for data in "CompressedData" block
         //

         compressedSize = uncompressedSize;

         if (compressionLevel != 0) {
            m_deflater.setInput(memStream.toByteArray(), memStream.size());
            m_deflater.deflate();
            ASSERT((::u32)m_deflater.getOutputSize() == m_deflater.getOutputSize());
            compressedSize = (::u32)m_deflater.getOutputSize();
         }

         //
         // Write data to socket
         //

         {
            AutoLock l(m_prfboutputgate);

            m_prfboutputgate->writeUInt32(FTMessage::FILE_LIST_REPLY);

            m_prfboutputgate->writeUInt8(compressionLevel);
            m_prfboutputgate->writeUInt32(compressedSize);
            m_prfboutputgate->writeUInt32(uncompressedSize);

            if (compressionLevel != 0) {
               m_prfboutputgate->write(m_deflater.getOutput(), compressedSize);
            } else {
               m_prfboutputgate->write(memStream.toByteArray(), uncompressedSize);
            }

            m_prfboutputgate->flush();
         } // synchronized(m_prfboutputgate)
      } // void

      void FileTransferRequestHandler::mkDirRequested()
      {
         ::string strFolderPath;

         {
            strFolderPath = m_prfbinputgate->readUtf8();
         } // end of reading block.

         m_plogwriter->debug("mkdir \"{}\" command requested", strFolderPath);

         checkAccess();

         throw "todo";

         // if (folderPath.parentPathIsRoot()) {
         //    throw FileTransferException("Cannot create folder in root folder");
         // }
         //
         // File folder(folderPath);
         //
         // if (folder.exists()) {
         //    throw FileTransferException("Directory already exists");
         // }
         //
         // if (!folder.mkdir()) {
         //    throw SystemException();
         // }

         {
            AutoLock l(m_prfboutputgate);

            m_prfboutputgate->writeUInt32(FTMessage::MKDIR_REPLY);

            m_prfboutputgate->flush();
         }
      }

      void FileTransferRequestHandler::rmFileRequested()
      {
         //::string fullPathName;

         ::string strFullPathName;

         {
            strFullPathName = m_prfbinputgate->readUtf8();
         } // end of reading block.

         m_plogwriter->debug("rm \"{}\" command requested", strFullPathName);

         checkAccess();

         throw "todo";

         // File file(fullPathName);
         //
         // if (!file.exists()) {
         //    throw SystemException();
         // } // file file does not exists
         //
         // if (!file.remove()) {
         //    throw SystemException();
         // } // if cannot delete file

         {
            AutoLock l(m_prfboutputgate);

            m_prfboutputgate->writeUInt32(FTMessage::REMOVE_REPLY);

            m_prfboutputgate->flush();
         }
      } // void

      void FileTransferRequestHandler::mvFileRequested()
      {
         //::string oldFileName;
         //::string newFileName;
         ::string oldFileName;
         ::string newFileName;

         {
            oldFileName = m_prfbinputgate->readUtf8();
            newFileName = m_prfbinputgate->readUtf8();
         } // end of reading block.

         m_plogwriter->debug("move \"{}\" \"{}\" command requested", oldFileName, newFileName);

         checkAccess();

         throw "todo";

         // File srcFile(oldFileName);
         // File dstFile(newFileName);
         //
         // if (!srcFile.renameTo(&dstFile)) {
         //    throw SystemException();
         // }

         {
            AutoLock l(m_prfboutputgate);

            m_prfboutputgate->writeUInt32(FTMessage::RENAME_REPLY);

            m_prfboutputgate->flush();
         }
      }

      void FileTransferRequestHandler::dirSizeRequested()
      {
         ::string fullPathName;

         {
            fullPathName = m_prfbinputgate->readUtf8();
         } // end of reading block.

         m_plogwriter->debug("Size of folder '{}\' requested",
                      fullPathName);

         checkAccess();

         throw "todo";
         ::u64 directorySize = 0;
         //
         // if (!getDirectorySize(fullPathName, &directorySize)) {
         //    throw SystemException();
         // }

         {
            AutoLock l(m_prfboutputgate);

            m_prfboutputgate->writeUInt32(FTMessage::DIRSIZE_REPLY);
            m_prfboutputgate->writeUInt64(directorySize);

            m_prfboutputgate->flush();
         }
      } // void

      void FileTransferRequestHandler::md5Requested()
      {
         ::string fullPathName;

         ::u64 offset;
         ::u64 dataLen;

         {
            fullPathName = m_prfbinputgate->readUtf8();

            offset = m_prfbinputgate->readUInt64();
            dataLen = m_prfbinputgate->readUInt64();
         } // end of reading block.

         m_plogwriter->debug("md5 \"{}\" {} {} command requested", fullPathName, offset, dataLen);

         checkAccess();

         //
         // Action
         //
         throw "todo";

         ::subsystem::MD5 md5calculator;
         //
         // File file(fullPathName);
         //
         // StringStorage path;
         // file.getPath(&path);
         // WinFileChannel fileInputStream(path, F_READ, FM_OPEN);
         // fileInputStream.seek(offset);
         //
         // //
         // // Begin reading needed file data
         // //
         //
         // DWORD bytesToRead = 1024 * 1024;
         // ::u64 bytesToReadTotal = dataLen;
         // size_t bytesRead = 0;
         // ::u64 bytesReadTotal = 0;
         //
         // if (dataLen < (::u64)bytesToRead) {
         //    bytesToRead = (DWORD)dataLen;
         // }
         //
         // std::vector<::u8> buffer(bytesToRead);
         //
         // while (bytesToReadTotal > 0) {
         //    bytesRead = fileInputStream.read(&buffer.front(), bytesToRead);
         //    bytesReadTotal += bytesRead;
         //    bytesToReadTotal -= bytesRead;
         //
         //    if (bytesToReadTotal < (::u64)bytesToRead) {
         //       bytesToRead = (DWORD)bytesToReadTotal;
         //    }
         //
         //    md5calculator.update(&buffer.front(), (::u32)bytesRead);
         // } // while
         //
         // md5calculator.finalize();

         {
            AutoLock l(m_prfboutputgate);

            m_prfboutputgate->writeUInt32(FTMessage::MD5_REPLY);
            m_prfboutputgate->write((const_char_pointer )md5calculator.getHash(), 16);

            m_prfboutputgate->flush();
         }
      }

      void FileTransferRequestHandler::uploadStartRequested()
      {
         //
         // Request input variables.
         //

         ::string fullPathName;
         ::u8 uploadFlags;
         ::u64 initialOffset;

         {
            fullPathName = m_prfbinputgate->readUtf8();
            uploadFlags = m_prfbinputgate->readUInt8();
            initialOffset = m_prfbinputgate->readUInt64();
         }

         m_plogwriter->debug("upload \"%s\" %d %d command requested", fullPathName, uploadFlags, initialOffset);

         checkAccess();

         //
         // Closing previous upload if it was broken
         //

         if (m_pfileOutputStream != NULL) {
            delete m_pfileOutputStream;
            m_pfileOutputStream = 0;
         }
         if (m_pfileUpload != NULL) {
            delete m_pfileUpload;
            m_pfileUpload = 0;
         }

         throw "todo";

         // if (fullPathName.parentPathIsRoot()) {
         //    throw FileTransferException("Cannot upload file to root folder");
         // }
         //
         // m_pfileUpload = new File(fullPathName);
         //
         // //
         // // Trying to create file or overwrite existing
         // //
         //
         // if ((uploadFlags & 0x1) && (!m_pfileUpload->truncate())) {
         //    throw SystemException();
         // }
         //
         // //
         // // Trying to open file and seek to initial file pointPosition
         // //
         // m_pfileOutputStream = new WinFileChannel(fullPathName,
         //                                         F_WRITE,
         //                                         FM_OPEN);
         // m_pfileOutputStream->seek(initialOffset);

         //
         // Send reply
         //

         {
            AutoLock l(m_prfboutputgate);

            m_prfboutputgate->writeUInt32(FTMessage::UPLOAD_START_REPLY);

            m_prfboutputgate->flush();
         }
      } // void

      void FileTransferRequestHandler::uploadDataRequested()
      {
         //
         // Request input variables.
         //

         ::u8 compressionLevel;
         ::u32 compressedSize;
         ::u32 uncompressedSize;

         compressionLevel = m_prfbinputgate->readUInt8();
         compressedSize = m_prfbinputgate->readUInt32();
         uncompressedSize = m_prfbinputgate->readUInt32();
         //std::vector<::i8> buffer(compressedSize);
         memory buffer;
         buffer.set_size(compressedSize);
         if (compressedSize != 0) {
            m_prfbinputgate->readFully(buffer.data(), compressedSize);
         }

         m_plogwriter->information("upload data (cs = %d, us = %d) requested", compressedSize, uncompressedSize);

         checkAccess();

         throw "todo";

         // if (m_pfileUpload == NULL) {
         //    throw FileTransferException("No active upload at the moment");
         // }
         //
         // if (compressedSize != 0) {
         //    if (compressionLevel == 0) {
         //       DataOutputStream dataOutStream(m_pfileOutputStream);
         //       dataOutStream.writeFully(&buffer.front(), uncompressedSize);
         //    } else {
         //       m_inflater.setInput(&buffer.front(), compressedSize);
         //       m_inflater.setUnpackedSize(uncompressedSize);
         //       m_inflater.inflate();
         //
         //       DataOutputStream dataOutStream(m_pfileOutputStream);
         //
         //       dataOutStream.writeFully(m_inflater.getOutput(), m_inflater.getOutputSize());
         //    } // if using compression
         // }

         {
            AutoLock l(m_prfboutputgate);

            m_prfboutputgate->writeUInt32(FTMessage::UPLOAD_DATA_REPLY);

            m_prfboutputgate->flush();
         }
      }

      void FileTransferRequestHandler::uploadEndRequested()
      {
         ::u16 fileFlags;
         ::u64 modificationTime;

         {
            fileFlags = m_prfbinputgate->readUInt16();
            modificationTime = m_prfbinputgate->readUInt64();
         } // end of reading block.

         m_plogwriter->debug("%s", "end of upload requested\n");

         checkAccess();

         throw todo;
         // //
         // // No active uploads at the moment.
         // // Client is "bad" if send to us this message
         // //
         //
         // if (m_pfileUpload == NULL) {
         //    throw FileTransferException("No active upload at the moment");
         // }
         //
         // //
         // // Close file output stream
         // //
         //
         // try {
         //    m_pfileOutputStream->close();
         // } catch (...) { }
         //
         // //
         // // Trying to set modification time
         // //
         //
         // if (!m_pfileUpload->setLastModified(modificationTime)) {
         //    throw FileTransferException("Cannot change last write file time");
         // } // if cannot set modification time
         //
         // //
         // // Send reply
         // //

         {
            AutoLock l(m_prfboutputgate);

            m_prfboutputgate->writeUInt32(FTMessage::UPLOAD_END_REPLY);

            m_prfboutputgate->flush();
         }

         //
         // Cleanup
         //

         if (m_pfileOutputStream != NULL) {
            delete m_pfileOutputStream;
            m_pfileOutputStream = NULL;
         }

         if (m_pfileUpload != NULL) {
            delete m_pfileUpload;
            m_pfileUpload = NULL;
         }

      } // void

      void FileTransferRequestHandler::downloadStartRequested()
      {
         ::string fullPathName;
         ::u64 initialOffset;

         //
         // Reading command arguments
         //

         {
            fullPathName = m_prfbinputgate->readUtf8();
            initialOffset = m_prfbinputgate->readUInt64();
         } // end of reading block.

         m_plogwriter->debug("download of \"%s\" file (offset = %d) requested", fullPathName, initialOffset);

         checkAccess();

         throw todo;
         // //
         // // Ending previous download if it was broken
         // //
         //
         // if (m_pfileInputStream != 0) {
         //    delete m_pfileInputStream;
         //    m_pfileInputStream = 0;
         // }
         // if (m_pfileDownload != 0) {
         //    delete m_pfileDownload;
         //    m_pfileDownload = 0;
         // }
         //
         // m_pfileDownload = new File(fullPathName);


         //
         // Try to open file for reading and seek to initial
         // file pointPosition.
         //

         // m_pfileInputStream = new WinFileChannel(fullPathName, F_READ,
         //                                        FM_OPEN);
         // m_pfileInputStream->seek(initialOffset);

         {
            AutoLock l(m_prfboutputgate);

            m_prfboutputgate->writeUInt32(FTMessage::DOWNLOAD_START_REPLY);

            m_prfboutputgate->flush();
         }
      }

      //
      // FIXME: file flags is unimplemented
      //

      void FileTransferRequestHandler::downloadDataRequested()
      {
         //
         // Request input variables.
         //

         ::u8 requestedCompressionLevel;
         ::u32 dataSize;

         {
            requestedCompressionLevel = m_prfbinputgate->readUInt8();
            dataSize = m_prfbinputgate->readUInt32();
         } // end of reading block.

         m_plogwriter->information("download %d bytes (comp flag = %d) requested", dataSize, requestedCompressionLevel);

         checkAccess();

         //
         // Data download reply variables.
         //

         ::u8 compressionLevel = requestedCompressionLevel;
         ::u32 compressedSize = 0;
         ::u32 uncompressedSize = 0;

         throw todo;
         //
         // //
         // // Client cannot send this request cause there is no
         // // active download at the moment
         // //
         //
         // if (m_pfileDownload == NULL) {
         //    throw FileTransferException("No active download at the moment");
         // }
         //
         // std::vector<::i8> buffer(dataSize);
         memory buffer;
         buffer.set_size(dataSize);
         //
         // DWORD read = 0;
         //
         // try {
         //    if (dataSize != 0) {
         //       size_t portion = m_pfileInputStream->read(&buffer.front(), dataSize);
         //       read = (DWORD)portion;
         //       ASSERT(read == portion);
         //    }
         // } catch (EOFException) {
         //
         //    //
         //    // End of file detected
         //    //
         //
         //    try { m_pfileInputStream->close(); } catch (...) { }
         //
         //    ::u8 fileFlags = 0;
         //
         //    {
         //       AutoLock l(m_prfboutputgate);
         //
         //       m_prfboutputgate->writeUInt32(FTMessage::DOWNLOAD_END_REPLY);
         //       m_prfboutputgate->writeUInt8(fileFlags);
         //       m_prfboutputgate->writeUInt64(m_pfileDownload->lastModified());
         //
         //       m_prfboutputgate->flush();
         //    } // rfb io handle block
         //
         //    m_plogwriter->debug("%s"), _T("downloading has finished\n");
         //
         //    delete m_pfileInputStream;
         //    delete m_pfileDownload;
         //
         //    m_pfileInputStream = NULL;
         //    m_pfileDownload = NULL;
         //
         //    return ;
         //
         // } catch (IOException &ioEx) {
         //    throw FileTransferException(&ioEx);
         // } // try / catch
         //
         // compressedSize = read;
         // uncompressedSize = read;
         //
         // if (compressionLevel != 0) {
         //    if (dataSize != 0) {
         //       m_deflater.setInput(&buffer.front(), uncompressedSize);
         //       m_deflater.deflate();
         //       ASSERT((::u32)m_deflater.getOutputSize() == m_deflater.getOutputSize());
         //       compressedSize = (::u32)m_deflater.getOutputSize();
         //    }
         // }
         //
         // //
         // // Send download data reply
         // //

         AutoLock l(m_prfboutputgate);

         m_prfboutputgate->writeUInt32(FTMessage::DOWNLOAD_DATA_REPLY);
         m_prfboutputgate->writeUInt8(compressionLevel);
         m_prfboutputgate->writeUInt32(compressedSize);
         m_prfboutputgate->writeUInt32(uncompressedSize);

         if (compressionLevel == 0) {
            if (dataSize != 0) {
               m_prfboutputgate->write(buffer.data(), uncompressedSize);
            }
         } else {
            m_prfboutputgate->write((const_char_pointer )m_deflater.getOutput(), compressedSize);
         }

         m_prfboutputgate->flush();
      }

      void FileTransferRequestHandler::lastRequestFailed(::string &storage)
      {
         lastRequestFailed(storage);
      }

      void FileTransferRequestHandler::lastRequestFailed(const_char_pointer description)
      {
         m_plogwriter->error("last request failed: \"%s\"", description);

         {
            AutoLock l(m_prfboutputgate);

            m_prfboutputgate->writeUInt32(FTMessage::LAST_REQUEST_FAILED_REPLY);
            m_prfboutputgate->writeUTF8(description);

            m_prfboutputgate->flush();
         }
      }

      bool FileTransferRequestHandler::getDirectorySize(const_char_pointer pszPathname, filesize *dirSize)
      {
         throw "todo";

         // ::u64 currentDirSize = 0;
         // ::u32 filesCount = 0;
         // ::u32 dataSize = 0;
         //
         // File folder(pathname);
         //
         // //
         // // Get files count
         // //
         //
         // if (!folder.list(NULL, &filesCount)) {
         //    return false;
         // }
         //
         // if (filesCount != 0) {
         //    std::vector<StringStorage> fileNames(filesCount);
         //
         //    //
         //    // Get file names
         //    //
         //
         //    folder.list(&fileNames.front(), NULL);
         //
         //    for (::u32 i = 0; i < filesCount; i++) {
         //       File subfile(pathname, fileNames[i]);
         //       if (subfile.isDirectory()) {
         //
         //          ::u64 subDirSize = 0;
         //          StringStorage subDirPath;
         //
         //          subfile.getPath(&subDirPath);
         //
         //          if (getDirectorySize(subDirPath, &subDirSize)) {
         //             currentDirSize += subDirSize;
         //          }  // if it got sub directory size
         //       } else {
         //          currentDirSize += subfile.length();
         //       } // if subfile is normal file
         //    } // for every subfile in file list
         // }
         // *dirSize = currentDirSize;

         return true;
      }

      void FileTransferRequestHandler::checkAccess()
      {
         try {
            if (!isFileTransferEnabled()) {
               throw ::subsystem::Exception("File transfers is disabled");
            }
            m_pfiletransfersecurity->throwIfAccessDenied();
         } catch (::subsystem::Exception &someEx) {
            throw ::subsystem::SystemException(someEx.get_message());
         } // try / catch.
      }
   }// namespace file_transfer

} // namespace remoting
