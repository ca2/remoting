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


#include "remoting/remoting_common/util/inttypes.h"
#include "remoting/remoting_common/network/RfbInputGate.h"
#include "remoting/remoting_common/network/RfbOutputGate.h"
#include "remoting/remoting_common/ftp_common/FileInfo.h"
#include "file_lib/WinFileChannel.h"
#include "remoting/remoting_common/util/Inflater.h"
#include "remoting/remoting_common/util/Deflater.h"
#include "desktop/Desktop.h"
#include "rfb_sconn/RfbCodeRegistrator.h"
#include "rfb_sconn/RfbDispatcherListener.h"
#include "FileTransferSecurity.h"
//#include "log_writer/LogWriter.h"

/**
 * Handler of file transfer plugin client to server messages.
 * Processes client requests and sends replies.
 */
class FileTransferRequestHandler : public RfbDispatcherListener
{
public:
  /**
   * Creates new file transfler client messages handler.
   * @param registrator rfb registrator which needs to register FT messages
   *   to rfb dispatcher address whem to this object.
   * @param output gate for writting replies for requests.
   * @param desktop windows desktop to known current user name and desktop name.
   *   ::file::item transfers are disabled if current desktop is winlogon.
   * @pararm enabled indicates if file transfer should be enabled or disabled
   *   (for example, it's disabled in view-only mode).
   */
  FileTransferRequestHandler(RfbCodeRegistrator *registrator,
                             RfbOutputGate *output,
                             Desktop *desktop,
                             LogWriter *log,
                             bool enabled = true);

  /**
   * Deletes file transfer request handler.
   */
  virtual ~FileTransferRequestHandler();

  /**
   * Inherited from RfbDispatcherListener.
   * Processes file transfer client messages.
   */
  virtual void onRequest(unsigned int reqCode, RfbInputGate *backGate);

protected:

  /**
   * Checks if file transfer if enabled.
   * @return true if file transfer is enabled, false otherwise.
   */
  bool isFileTransferEnabled();

  //
  // Common request handlers.
  //

  void compressionSupportRequested();
  void fileListRequested();
  void mkDirRequested();
  void rmFileRequested();
  void mvFileRequested();
  void dirSizeRequested();
  void md5Requested();

  //
  // Upload requests handlers.
  //

  void uploadStartRequested();
  void uploadDataRequested();
  void uploadEndRequested();

  //
  // Download requests handlers.
  //

  void downloadStartRequested();
  void downloadDataRequested();

  //
  // Method sends "Last request failed" scopedstrMessage with error description.
  //

  void lastRequestFailed(::string & storage);
  void lastRequestFailed(const ::scoped_string & scopedstrDescription);

  //
  // Helper methods
  //

  bool getDirectorySize(const ::scoped_string & scopedstrPathname, unsigned long long *dirSize);

protected:
  /**
   * Checks if we can run file transfer now (using FileTransferSecurity).
   * @throws SystemException if file transfer cannot handle request now (
   * for example, winlogon desktop is active) or access denied (for example,
   * when view-only mode is enabled).
   */
  void checkAccess();

protected:
  //
  // Input and output gates.
  //

  RfbInputGate *m_input;
  RfbOutputGate *m_output;

  //
  // Download operation members
  //

  ::file::item *m_downloadFile;
  WinFileChannel *m_fileInputStream;

  //
  // Upload operation members
  //

  ::file::item *m_uploadFile;
  WinFileChannel *m_fileOutputStream;

  //
  // Zlib encoder / decoder
  //

  Deflater m_deflater;
  Inflater m_inflater;

  //
  // Security and impersonation.
  //

  FileTransferSecurity *m_security;

  // Determinates if file transfer is enabled.
  bool m_enabled;

  LogWriter *m_log;
};


