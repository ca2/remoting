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
#include "FileTransferRequestHandler.h"

#include "io_lib/DataOutputStream.h"

#include "remoting/ftp_common/FileTransferException.h"
#include "io_lib/ByteArrayOutputStream.h"
#include "file_lib/::file::item.h"
#include "file_lib/EOFException.h"
#include "remoting/ftp_common/FolderListener.h"
#include "remoting/ftp_common/FTMessage.h"
#include "remoting/ftp_common/WinFilePath.h"
#include "remoting/ftp_common/FileInfo.h"
#include "util/md5.h"
#include "network/RfbOutputGate.h"
#include "network/RfbInputGate.h"
#include "thread/AutoLock.h"
#include "win_system/Impersonator.h"
//#include "win_system/Environment.h"
#include "server_config_lib/Configurator.h"
#include "win_system/SystemException.h"
#include "rfb/VendorDefs.h"

FileTransferRequestHandler::FileTransferRequestHandler(RfbCodeRegistrator *registrator,
                                                       RfbOutputGate *output,
                                                       Desktop *desktop,
                                                       LogWriter *log,
                                                       bool enabled)
: m_downloadFile(NULL), m_fileInputStream(NULL),
  m_uploadFile(NULL), m_fileOutputStream(NULL),
  m_output(output), m_enabled(enabled),
  m_log(log)
{
  m_security = new FileTransferSecurity(desktop, m_log);

  if (!FileTransferRequestHandler::isFileTransferEnabled()) {
    return ;
  }

  registrator->addSrvToClCap(FTMessage::COMPRESSION_SUPPORT_REPLY, VendorDefs::TIGHTVNC, FTMessage::COMPRESSION_SUPPORT_REPLY_SIG);
  registrator->addSrvToClCap(FTMessage::FILE_LIST_REPLY, VendorDefs::TIGHTVNC, FTMessage::FILE_LIST_REPLY_SIG);
  registrator->addSrvToClCap(FTMessage::MD5_REPLY, VendorDefs::TIGHTVNC, FTMessage::MD5_REPLY_SIG);
  registrator->addSrvToClCap(FTMessage::UPLOAD_START_REPLY, VendorDefs::TIGHTVNC, FTMessage::UPLOAD_START_REPLY_SIG);
  registrator->addSrvToClCap(FTMessage::UPLOAD_DATA_REPLY, VendorDefs::TIGHTVNC, FTMessage::UPLOAD_DATA_REPLY_SIG);
  registrator->addSrvToClCap(FTMessage::UPLOAD_END_REPLY, VendorDefs::TIGHTVNC, FTMessage::UPLOAD_END_REPLY_SIG);
  registrator->addSrvToClCap(FTMessage::DOWNLOAD_START_REPLY, VendorDefs::TIGHTVNC, FTMessage::DOWNLOAD_START_REPLY_SIG);
  registrator->addSrvToClCap(FTMessage::DOWNLOAD_DATA_REPLY, VendorDefs::TIGHTVNC, FTMessage::DOWNLOAD_DATA_REPLY_SIG);
  registrator->addSrvToClCap(FTMessage::DOWNLOAD_END_REPLY, VendorDefs::TIGHTVNC, FTMessage::DOWNLOAD_END_REPLY_SIG);
  registrator->addSrvToClCap(FTMessage::MKDIR_REPLY, VendorDefs::TIGHTVNC, FTMessage::MKDIR_REPLY_SIG);
  registrator->addSrvToClCap(FTMessage::REMOVE_REPLY, VendorDefs::TIGHTVNC, FTMessage::REMOVE_REPLY_SIG);
  registrator->addSrvToClCap(FTMessage::RENAME_REPLY, VendorDefs::TIGHTVNC, FTMessage::RENAME_REPLY_SIG);
  registrator->addSrvToClCap(FTMessage::DIRSIZE_REPLY, VendorDefs::TIGHTVNC, FTMessage::DIRSIZE_REPLY_SIG);
  registrator->addSrvToClCap(FTMessage::LAST_REQUEST_FAILED_REPLY, VendorDefs::TIGHTVNC, FTMessage::LAST_REQUEST_FAILED_REPLY_SIG);

  registrator->addClToSrvCap(FTMessage::COMPRESSION_SUPPORT_REQUEST, VendorDefs::TIGHTVNC, FTMessage::COMPRESSION_SUPPORT_REQUEST_SIG);
  registrator->addClToSrvCap(FTMessage::FILE_LIST_REQUEST, VendorDefs::TIGHTVNC, FTMessage::FILE_LIST_REQUEST_SIG);
  registrator->addClToSrvCap(FTMessage::MD5_REQUEST, VendorDefs::TIGHTVNC, FTMessage::MD5_REQUEST_SIG);
  registrator->addClToSrvCap(FTMessage::UPLOAD_START_REQUEST, VendorDefs::TIGHTVNC, FTMessage::UPLOAD_START_REQUEST_SIG);
  registrator->addClToSrvCap(FTMessage::UPLOAD_DATA_REQUEST, VendorDefs::TIGHTVNC, FTMessage::UPLOAD_DATA_REQUEST_SIG);
  registrator->addClToSrvCap(FTMessage::UPLOAD_END_REQUEST, VendorDefs::TIGHTVNC, FTMessage::UPLOAD_END_REQUEST_SIG);
  registrator->addClToSrvCap(FTMessage::DOWNLOAD_START_REQUEST, VendorDefs::TIGHTVNC, FTMessage::DOWNLOAD_START_REQUEST_SIG);
  registrator->addClToSrvCap(FTMessage::DOWNLOAD_DATA_REQUEST, VendorDefs::TIGHTVNC, FTMessage::DOWNLOAD_DATA_REQUEST_SIG);
  registrator->addClToSrvCap(FTMessage::MKDIR_REQUEST, VendorDefs::TIGHTVNC, FTMessage::MKDIR_REQUEST_SIG);
  registrator->addClToSrvCap(FTMessage::REMOVE_REQUEST, VendorDefs::TIGHTVNC, FTMessage::REMOVE_REQUEST_SIG);
  registrator->addClToSrvCap(FTMessage::RENAME_REQUEST, VendorDefs::TIGHTVNC, FTMessage::RENAME_REQUEST_SIG);
  registrator->addClToSrvCap(FTMessage::DIRSIZE_REQUEST, VendorDefs::TIGHTVNC, FTMessage::DIRSIZE_REQUEST_SIG);

  unsigned int rfbMessagesToProcess[] = {
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

  for (size_t i = 0; i < sizeof(rfbMessagesToProcess) / sizeof(unsigned int); i++) {
    registrator->regCode(rfbMessagesToProcess[i], this);
  }

  m_log->debug("::file::item transfer request handler created");
}

FileTransferRequestHandler::~FileTransferRequestHandler()
{
  delete m_security;

  if (m_fileInputStream != NULL) {
    delete m_fileInputStream;
  }
  if (m_downloadFile != NULL) {
    delete m_downloadFile;
  }
  if (m_fileOutputStream != NULL) {
    delete m_fileOutputStream;
  }
  if (m_uploadFile != NULL) {
    delete m_uploadFile;
  }

  m_log->debug("::file::item transfer request handler deleted");
}

void FileTransferRequestHandler::onRequest(unsigned int reqCode, RfbInputGate *backGate)
{
  m_security->beginMessageProcessing();

  m_input = backGate;

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
  } catch (::remoting::Exception &someEx) {
    lastRequestFailed(someEx.get_message());
  } // try / catch.

  m_input = NULL;

  m_security->endMessageProcessing();
}

bool FileTransferRequestHandler::isFileTransferEnabled()
{
  return m_enabled && Configurator::getInstance()->getServerConfig()->isFileTransfersEnabled();
}

void FileTransferRequestHandler::compressionSupportRequested()
{
  m_log->debug("{}"), _T("compression support requested");

  //
  // Can be 0 - compression not supported by server
  //     or 1 - compression is supported by server
  //

  unsigned char compressionSupport = 1;

  m_log->debug("sending compression support reply: {}"), (compressionSupport == 1) ? _T("supported") : _T("not supported");

  {
    AutoLock l(m_output);

    m_output->writeUInt32(FTMessage::COMPRESSION_SUPPORT_REPLY);
    m_output->writeUInt8(compressionSupport);

    m_output->flush();
  }
}

void FileTransferRequestHandler::fileListRequested()
{
  unsigned char requestedCompressionLevel;
  WinFilePath fullPathName;

  //
  // Read input data
  //

  {
    requestedCompressionLevel = m_input->readUInt8();

    m_input->readUTF8(&fullPathName);
  }

  m_log->debug("::file::item ::list_base of folder '{}' requested",
               fullPathName);

  checkAccess();

  unsigned char compressionLevel = requestedCompressionLevel;
  unsigned int compressedSize = 0;
  unsigned int uncompressedSize = 0;
  unsigned int filesCount = 0;
  unsigned int filesInfoDataSize = 0;
  const FileInfo *files = NULL;

  //
  // Get file ::list_base from specified folder
  //

  FolderListener folderListener(fullPathName);

  if (!folderListener.::list_base()) {
    throw SystemException();
  }

  files = folderListener.getFilesInfo();
  filesCount = folderListener.getFilesCount();

  //
  // Create buffer with "CompressedData" block inside
  //

  ByteArrayOutputStream memStream;
  DataOutputStream outMemStream(&memStream);

  outMemStream.writeUInt32(filesCount);
  for (unsigned int i = 0; i < filesCount; i++) {
    outMemStream.writeUInt64(files[i].getSize());
    outMemStream.writeUInt64(files[i].lastModified());
    outMemStream.writeUInt16(files[i].getFlags());
    outMemStream.writeUTF8(files[i].getFileName());
  } // for

  _ASSERT((unsigned int)memStream.size() == memStream.size());
  uncompressedSize = (unsigned int)memStream.size();

  //
  // Buffer for data in "CompressedData" block
  //

  compressedSize = uncompressedSize;

  if (compressionLevel != 0) {
    m_deflater.setInput(memStream.toByteArray(), memStream.size());
    m_deflater.deflate();
    _ASSERT((unsigned int)m_deflater.getOutputSize() == m_deflater.getOutputSize());
    compressedSize = (unsigned int)m_deflater.getOutputSize();
  }

  //
  // Write data to socket
  //

  {
    AutoLock l(m_output);

    m_output->writeUInt32(FTMessage::FILE_LIST_REPLY);

    m_output->writeUInt8(compressionLevel);
    m_output->writeUInt32(compressedSize);
    m_output->writeUInt32(uncompressedSize);

    if (compressionLevel != 0) {
      m_output->writeFully(m_deflater.getOutput(), compressedSize);
    } else {
      m_output->writeFully(memStream.toByteArray(), uncompressedSize);
    }

    m_output->flush();
  } // synchronized(m_output)
} // void

void FileTransferRequestHandler::mkDirRequested()
{
  WinFilePath folderPath;

  {
    m_input->readUTF8(&folderPath);
  } // end of reading block.

  m_log->debug("mkdir \"{}\" command requested", folderPath);

  checkAccess();

  if (folderPath.parentPathIsRoot()) {
    throw FileTransferException("Cannot create folder in root folder");
  }

  ::file::item folder(folderPath);

  if (folder.exists()) {
    throw FileTransferException("Directory already exists");
  }

  if (!folder.mkdir()) {
    throw SystemException();
  }

  {
    AutoLock l(m_output);

    m_output->writeUInt32(FTMessage::MKDIR_REPLY);

    m_output->flush();
  }
}

void FileTransferRequestHandler::rmFileRequested()
{
  WinFilePath fullPathName;

  {
    m_input->readUTF8(&fullPathName);
  } // end of reading block.

  m_log->debug("rm \"{}\" command requested", fullPathName);

  checkAccess();

  ::file::item file(fullPathName);

  if (!file.exists()) {
    throw SystemException();
  } // file file does not exists

  if (!file.remove()) {
    throw SystemException();
  } // if cannot delete file

  {
    AutoLock l(m_output);

    m_output->writeUInt32(FTMessage::REMOVE_REPLY);

    m_output->flush();
  }
} // void

void FileTransferRequestHandler::mvFileRequested()
{
  WinFilePath oldFileName;
  WinFilePath newFileName;

  {
    m_input->readUTF8(&oldFileName);
    m_input->readUTF8(&newFileName);
  } // end of reading block.

  m_log->debug("move \"{}\" \"{}\" command requested", oldFileName, newFileName);

  checkAccess();

  ::file::item srcFile(oldFileName);
  ::file::item dstFile(newFileName);

  if (!srcFile.renameTo(&dstFile)) {
    throw SystemException();
  }

  {
    AutoLock l(m_output);

    m_output->writeUInt32(FTMessage::RENAME_REPLY);

    m_output->flush();
  }
}

void FileTransferRequestHandler::dirSizeRequested()
{
  WinFilePath fullPathName;

  {
    m_input->readUTF8(&fullPathName);
  } // end of reading block.

  m_log->debug("Size of folder '{}\' requested",
               fullPathName);

  checkAccess();

  unsigned long long directorySize = 0;

  if (!getDirectorySize(fullPathName, &directorySize)) {
    throw SystemException();
  }

  {
    AutoLock l(m_output);

    m_output->writeUInt32(FTMessage::DIRSIZE_REPLY);
    m_output->writeUInt64(directorySize);

    m_output->flush();
  }
} // void

void FileTransferRequestHandler::md5Requested()
{
  WinFilePath fullPathName;

  unsigned long long offset;
  unsigned long long dataLen;

  {
    m_input->readUTF8(&fullPathName);

    offset = m_input->readUInt64();
    dataLen = m_input->readUInt64();
  } // end of reading block.

  m_log->debug("md5 \"{}\" {} {} command requested", fullPathName, offset, dataLen);

  checkAccess();

  //
  // Action
  //

  MD5 md5calculator;

  ::file::item file(fullPathName);

  ::string path;
  file.getPath(&path);
  WinFileChannel fileInputStream(path, F_READ, FM_OPEN);
  fileInputStream.seek(offset);

  //
  // Begin reading needed file data
  //

  DWORD bytesToRead = 1024 * 1024;
  unsigned long long bytesToReadTotal = dataLen;
  size_t bytesRead = 0;
  unsigned long long bytesReadTotal = 0;

  if (dataLen < (unsigned long long)bytesToRead) {
    bytesToRead = (DWORD)dataLen;
  }

  ::array_base<unsigned char> buffer(bytesToRead);

  while (bytesToReadTotal > 0) {
    bytesRead = fileInputStream.read(&buffer.front(), bytesToRead);
    bytesReadTotal += bytesRead;
    bytesToReadTotal -= bytesRead;

    if (bytesToReadTotal < (unsigned long long)bytesToRead) {
      bytesToRead = (DWORD)bytesToReadTotal;
    }

    md5calculator.update(&buffer.front(), (unsigned int)bytesRead);
  } // while

  md5calculator.finalize();

  {
    AutoLock l(m_output);

    m_output->writeUInt32(FTMessage::MD5_REPLY);
    m_output->writeFully((const char *)md5calculator.getHash(), 16);

    m_output->flush();
  }
}

void FileTransferRequestHandler::uploadStartRequested()
{
  //
  // Request input variables.
  //

  WinFilePath fullPathName;
  unsigned char uploadFlags;
  unsigned long long initialOffset;

  {
    m_input->readUTF8(&fullPathName);
    uploadFlags = m_input->readUInt8();
    initialOffset = m_input->readUInt64();
  }

  m_log->debug("upload \"{}\" {} {} command requested", fullPathName, uploadFlags, initialOffset);

  checkAccess();

  //
  // Closing previous upload if it was broken
  //

  if (m_fileOutputStream != NULL) {
    delete m_fileOutputStream;
    m_fileOutputStream = 0;
  }
  if (m_uploadFile != NULL) {
    delete m_uploadFile;
    m_uploadFile = 0;
  }

  if (fullPathName.parentPathIsRoot()) {
    throw FileTransferException("Cannot upload file to root folder");
  }

  m_uploadFile = new ::file::item(fullPathName);

  //
  // Trying to create file or overwrite existing
  //

  if ((uploadFlags & 0x1) && (!m_uploadFile->truncate())) {
    throw SystemException();
  }

  //
  // Trying to open file and seek to initial file position
  //
  m_fileOutputStream = new WinFileChannel(fullPathName,
                                          F_WRITE,
                                          FM_OPEN);
  m_fileOutputStream->seek(initialOffset);

  //
  // Send reply
  //

  {
    AutoLock l(m_output);

    m_output->writeUInt32(FTMessage::UPLOAD_START_REPLY);

    m_output->flush();
  }
} // void

void FileTransferRequestHandler::uploadDataRequested()
{
  //
  // Request input variables.
  //

  unsigned char compressionLevel;
  unsigned int compressedSize;
  unsigned int uncompressedSize;

  compressionLevel = m_input->readUInt8();
  compressedSize = m_input->readUInt32();
  uncompressedSize = m_input->readUInt32();
  ::array_base<char> buffer(compressedSize);
  if (compressedSize != 0) {
    m_input->readFully(&buffer.front(), compressedSize);
  }

  m_log->information("upload data (cs = {}, us = {}) requested", compressedSize, uncompressedSize);

  checkAccess();

  if (m_uploadFile == NULL) {
    throw FileTransferException("No active upload at the moment");
  }

  if (compressedSize != 0) {
    if (compressionLevel == 0) {
      DataOutputStream dataOutStream(m_fileOutputStream);
      dataOutStream.writeFully(&buffer.front(), uncompressedSize);
    } else {
      m_inflater.setInput(&buffer.front(), compressedSize);
      m_inflater.setUnpackedSize(uncompressedSize);
      m_inflater.inflate();

      DataOutputStream dataOutStream(m_fileOutputStream);

      dataOutStream.writeFully(m_inflater.getOutput(), m_inflater.getOutputSize());
    } // if using compression
  }

  {
    AutoLock l(m_output);

    m_output->writeUInt32(FTMessage::UPLOAD_DATA_REPLY);

    m_output->flush();
  }
}

void FileTransferRequestHandler::uploadEndRequested()
{
  unsigned short fileFlags;
  unsigned long long modificationTime;

  {
    fileFlags = m_input->readUInt16();
    modificationTime = m_input->readUInt64();
  } // end of reading block.

  m_log->debug("{}"), _T("end of upload requested\n");

  checkAccess();

  //
  // No active uploads at the moment.
  // Client is "bad" if send to us this scopedstrMessage
  //

  if (m_uploadFile == NULL) {
    throw FileTransferException("No active upload at the moment");
  }

  //
  // Close file output stream
  //

  try {
    m_fileOutputStream->close();
  } catch (...) { }

  //
  // Trying to set modification time
  //

  if (!m_uploadFile->setLastModified(modificationTime)) {
    throw FileTransferException("Cannot change last write file time");
  } // if cannot set modification time

  //
  // Send reply
  //

  {
    AutoLock l(m_output);

    m_output->writeUInt32(FTMessage::UPLOAD_END_REPLY);

    m_output->flush();
  }

  //
  // Cleanup
  //

  if (m_fileOutputStream != NULL) {
    delete m_fileOutputStream;
    m_fileOutputStream = NULL;
  }

  if (m_uploadFile != NULL) {
    delete m_uploadFile;
    m_uploadFile = NULL;
  }

} // void

void FileTransferRequestHandler::downloadStartRequested()
{
  WinFilePath fullPathName;
  unsigned long long initialOffset;

  //
  // Reading command arguments
  //

  {
    m_input->readUTF8(&fullPathName);
    initialOffset = m_input->readUInt64();
  } // end of reading block.

  m_log->debug("download of \"{}\" file (offset = {}) requested", fullPathName, initialOffset);

  checkAccess();

  //
  // Ending previous download if it was broken
  //

  if (m_fileInputStream != 0) {
    delete m_fileInputStream;
    m_fileInputStream = 0;
  }
  if (m_downloadFile != 0) {
    delete m_downloadFile;
    m_downloadFile = 0;
  }

  m_downloadFile = new ::file::item(fullPathName);


  //
  // Try to open file for reading and seek to initial
  // file position.
  //

  m_fileInputStream = new WinFileChannel(fullPathName, F_READ,
                                         FM_OPEN);
  m_fileInputStream->seek(initialOffset);

  {
    AutoLock l(m_output);

    m_output->writeUInt32(FTMessage::DOWNLOAD_START_REPLY);

    m_output->flush();
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

  unsigned char requestedCompressionLevel;
  unsigned int dataSize;

  {
    requestedCompressionLevel = m_input->readUInt8();
    dataSize = m_input->readUInt32();
  } // end of reading block.

  m_log->information("download {} bytes (comp flag = {}) requested", dataSize, requestedCompressionLevel);

  checkAccess();

  //
  // Data download reply variables.
  //

  unsigned char compressionLevel = requestedCompressionLevel;
  unsigned int compressedSize;
  unsigned int uncompressedSize;

  //
  // Client cannot send this request cause there is no
  // active download at the moment
  //

  if (m_downloadFile == NULL) {
    throw FileTransferException("No active download at the moment");
  }

  ::array_base<char> buffer(dataSize);

  DWORD read = 0;

  try {
    if (dataSize != 0) {
      size_t portion = m_fileInputStream->read(&buffer.front(), dataSize);
      read = (DWORD)portion;
      _ASSERT(read == portion);
    }
  } catch (EOFException) {

    //
    // End of file detected
    //

    try { m_fileInputStream->close(); } catch (...) { }

    unsigned char fileFlags = 0;

    {
      AutoLock l(m_output);

      m_output->writeUInt32(FTMessage::DOWNLOAD_END_REPLY);
      m_output->writeUInt8(fileFlags);
      m_output->writeUInt64(m_downloadFile->lastModified());

      m_output->flush();
    } // rfb io handle block

    m_log->debug("{}"), _T("downloading has finished\n");

    delete m_fileInputStream;
    delete m_downloadFile;

    m_fileInputStream = NULL;
    m_downloadFile = NULL;

    return ;

  } catch (::io_exception &ioEx) {
    throw FileTransferException(&ioEx);
  } // try / catch

  compressedSize = read;
  uncompressedSize = read;

  if (compressionLevel != 0) {
    if (dataSize != 0) {
      m_deflater.setInput(&buffer.front(), uncompressedSize);
      m_deflater.deflate();
      _ASSERT((unsigned int)m_deflater.getOutputSize() == m_deflater.getOutputSize());
      compressedSize = (unsigned int)m_deflater.getOutputSize();
    }
  }

  //
  // Send download data reply
  //

  AutoLock l(m_output);

  m_output->writeUInt32(FTMessage::DOWNLOAD_DATA_REPLY);
  m_output->writeUInt8(compressionLevel);
  m_output->writeUInt32(compressedSize);
  m_output->writeUInt32(uncompressedSize);

  if (compressionLevel == 0) {
    if (dataSize != 0) {
      m_output->writeFully(&buffer.front(), uncompressedSize);
    }
  } else {
    m_output->writeFully((const char *)m_deflater.getOutput(), compressedSize);
  }

  m_output->flush();
}

void FileTransferRequestHandler::lastRequestFailed(::string & storage)
{
  lastRequestFailed(storage->getString());
}

void FileTransferRequestHandler::lastRequestFailed(const ::scoped_string & scopedstrDescription)
{
  m_log->error("last request failed: \"{}\"", description);

  {
    AutoLock l(m_output);

    m_output->writeUInt32(FTMessage::LAST_REQUEST_FAILED_REPLY);
    m_output->writeUTF8(description);

    m_output->flush();
  }
}

bool FileTransferRequestHandler::getDirectorySize(const ::scoped_string & scopedstrPathname, unsigned long long *dirSize)
{
  unsigned long long currentDirSize = 0;
  unsigned int filesCount = 0;
  unsigned int dataSize = 0;

  ::file::item folder(pathname);

  //
  // Get files count
  //

  if (!folder.::list_base(NULL, &filesCount)) {
    return false;
  }

  if (filesCount != 0) {
    ::string_array fileNames(filesCount);

    //
    // Get file names
    //

    folder.::list_base(&fileNames.front(), NULL);

    for (unsigned int i = 0; i < filesCount; i++) {
      ::file::item subfile(pathname, fileNames[i]);
      if (subfile.isDirectory()) {

        unsigned long long subDirSize = 0;
        ::string subDirPath;

        subfile.getPath(&subDirPath);

        if (getDirectorySize(subDirPath, &subDirSize)) {
          currentDirSize += subDirSize;
        }  // if it got sub directory size
      } else {
        currentDirSize += subfile.length();
      } // if subfile is normal file
    } // for every subfile in file ::list_base
  }
  *dirSize = currentDirSize;

  return true;
}

void FileTransferRequestHandler::checkAccess()
{
  try {
    if (!isFileTransferEnabled()) {
      throw ::remoting::Exception("::file::item transfers is disabled");
    }
    m_security->throwIfAccessDenied();
  } catch (::remoting::Exception &someEx) {
    throw SystemException(someEx.get_message());
  } // try / catch.
}
