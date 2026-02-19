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


#include "gui/BaseDialog.h"
//#include "gui/::remoting::Window.h"
#include "gui/TextBox.h"
#include "gui/ComboBox.h"
#include "gui/ImagedButton.h"
#include "gui/ProgressBar.h"

#include "remoting/ftp_common/FileInfo.h"

#include "io_lib/io_exception.h"

#include "FileInfoListView.h"
#include "FileExistDialog.h"
#include "thread/Thread.h"
#include "remoting/ftp_client/FileTransferInterface.h"

//#include <vector>



class FileTransferMainDialog : public BaseDialog,
                               public ::remoting::ftp::FileTransferInterface
{
public:
  FileTransferMainDialog(::remoting::ftp::FileTransferCore *core);
  virtual ~FileTransferMainDialog();

  //
  // Kill main dialog.
  // Function return false, if closing dialog is canceled, and true else
  //
  bool tryClose();

  //
  // Inherited from FtInterface
  //
  int onFtTargetFileExists(::remoting::ftp::FileInfo *sourceFileInfo,
                           ::remoting::ftp::FileInfo *targetFileInfo,
                           const ::file::path & pathToTargetFile);
  void setProgress(double progress);

  void onFtOpError(const ::scoped_string & scopedstrMessage);
  void onFtOpInfo(const ::scoped_string & scopedstrMessage);
  void onFtOpStarted();
  void onFtOpFinished(int state, int result);

  //
  // filetransfer's operation is finished. Need update of control
  //
  void setNothingState();

  //
  // Called if local file ::list_base is updated
  //
  void onRefreshLocalFileList();

  // Called if remote file ::list_base is updated
  void onRefreshRemoteFileList();

  //
  // Shows error scopedstrMessage and throws exception
  //

  void raise(::exception &ex) override;

//protected:

  //
  // Inherited from BaseDialog
  //

  virtual BOOL onInitDialog();
  virtual BOOL onNotify(UINT controlID, LPARAM data);
  virtual BOOL onCommand(UINT controlID, UINT notificationID);
  virtual BOOL onDestroy();

  virtual void onMessageReceived(UINT uMsg, WPARAM wParam, LPARAM lParam);

  //
  // Button event handlers
  //

  void onCancelButtonClick();
  void onCancelOperationButtonClick();

  void onRenameRemoteButtonClick();
  void onMkDirRemoteButtonClick();
  void onRemoveRemoteButtonClick();
  void onRefreshRemoteButtonClick();

  void onRenameLocalButtonClick();
  void onMkDirLocalButtonClick();
  void onRemoveLocalButtonClick();
  void onRefreshLocalButtonClick();

  void onUploadButtonClick();
  void onDownloadButtonClick();

  void moveUpLocalFolder();
  void moveUpRemoteFolder();

  //
  // List view event handlers
  //

  void onRemoteListViewDoubleClick();
  void onLocalListViewDoubleClick();

  void onRemoteListViewKeyDown(UINT key);
  void onLocalListViewKeyDown(UINT key);

  //
  // Enables or disables rename and delete buttons
  // depending of file ::list_base views selected items count.
  //

  void checkRemoteListViewSelection();
  void checkLocalListViewSelection();

  //
  // Text notification methods
  //

  void insertMessageIntoComboBox(const ::scoped_string & scopedstrMessage);

//private:

  //
  // Enables or disables all controls from m_controlsToBlock ::list_base
  //

  void enableControls(bool enabled);

  //
  // Links gui control class members with windows in dialog
  //

  void initControls();


  //
  // Refreshes local file ::list_base
  //

  void refreshLocalFileList();

  //
  // Refreshes remote file ::list_base
  //

  void refreshRemoteFileList();

  //
  // Displays file ::list_base of pathToFile folder of local machine
  // to local file ::list_base view
  //

  void tryListLocalFolder(const ::file::path & pathToFile);

  //
  // Sends file ::list_base request to server and shows result
  // in remote file ::list_base view
  //

  void tryListRemoteFolder(const ::file::path & pathToFile);

  //
  // Filenames helper methods
  //

  //
  // FIXME: Make classes for getPathTo*** methods
  //

  ::file::path getPathToCurrentLocalFolder();
  ::file::path getPathToParentLocalFolder();
  ::file::path getPathToSelectedLocalFile();

  ::file::path getPathToCurrentRemoteFolder();
  ::file::path getPathToParentRemoteFolder();
  ::file::path getPathToSelectedRemoteFile();

//protected:
  //
  // True if window state is closing.
  //

  bool m_isClosing;



  ::string m_lastSentFileListPath;
  ::string m_lastReceivedFileListPath;

  //
  // Buttons
  //

  ::remoting::Window m_renameRemoteButton;
  ::remoting::Window m_mkDirRemoteButton;
  ::remoting::Window m_removeRemoteButton;
  ::remoting::Window m_refreshRemoteButton;

  ::remoting::Window m_renameLocalButton;
  ::remoting::Window m_mkDirLocalButton;
  ::remoting::Window m_removeLocalButton;
  ::remoting::Window m_refreshLocalButton;

  ::remoting::Window m_uploadButton;
  ::remoting::Window m_downloadButton;

  ::remoting::Window m_cancelButton;

  //
  // Progress bar
  //

  ProgressBar m_copyProgressBar;

  //
  // Combo box
  //

  ComboBox m_logComboBox;

  //
  // Text boxes
  //

  TextBox m_localCurFolderTextBox;
  TextBox m_remoteCurFolderTextBox;

  //
  // Tables
  //

  FileInfoListView m_localFileListView;
  FileInfoListView m_remoteFileListView;

  //
  // Helper modal dialogs
  //

  FileExistDialog m_fileExistDialog;

  //
  // ::file::item info of ".." fake folder
  //

 ::remoting::ftp:: FileInfo *m_fakeMoveUpFolder;

//private:

  static const UINT WM_OPERATION_FINISHED = WM_USER + 2;
};


