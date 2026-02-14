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


#include "gui/BaseDialog.h"
#include "gui/Control.h"
#include "gui/TextBox.h"
#include "gui/ComboBox.h"
#include "gui/ImagedButton.h"
#include "gui/ProgressBar.h"

#include "ft_common/FileInfo.h"

#include "io_lib/io_exception.h"

#include "FileInfoListView.h"
#include "FileExistDialog.h"
#include "thread/Thread.h"
#include "ft_client_lib/FileTransferInterface.h"

//#include <vector>



class FileTransferMainDialog : public BaseDialog,
                               public FileTransferInterface
{
public:
  FileTransferMainDialog(FileTransferCore *core);
  virtual ~FileTransferMainDialog();

  //
  // Kill main dialog.
  // Function return false, if closing dialog is canceled, and true else
  //
  bool tryClose();

  //
  // Inherited from FtInterface
  //
  int onFtTargetFileExists(FileInfo *sourceFileInfo,
                           FileInfo *targetFileInfo,
                           const ::scoped_string & scopedstrPathToTargetFile);
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
  // Called if local file ::list is updated
  //
  void onRefreshLocalFileList();

  // Called if remote file ::list is updated
  void onRefreshRemoteFileList();

  //
  // Shows error message and throws exception
  //

  void raise(::remoting::Exception &ex);

protected:

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
  // depending of file ::list views selected items count.
  //

  void checkRemoteListViewSelection();
  void checkLocalListViewSelection();

  //
  // Text notification methods
  //

  void insertMessageIntoComboBox(const ::scoped_string & scopedstrMessage);

private:

  //
  // Enables or disables all controls from m_controlsToBlock ::list
  //

  void enableControls(bool enabled);

  //
  // Links gui control class members with windows in dialog
  //

  void initControls();


  //
  // Refreshes local file ::list
  //

  void refreshLocalFileList();

  //
  // Refreshes remote file ::list
  //

  void refreshRemoteFileList();

  //
  // Displays file ::list of pathToFile folder of local machine
  // to local file ::list view
  //

  void tryListLocalFolder(const ::scoped_string & scopedstrPathToFile);

  //
  // Sends file ::list request to server and shows result
  // in remote file ::list view
  //

  void tryListRemoteFolder(const ::scoped_string & scopedstrPathToFile);

  //
  // Filenames helper methods
  //

  //
  // FIXME: Make classes for getPathTo*** methods
  //

  void getPathToCurrentLocalFolder(::string & out);
  void getPathToParentLocalFolder(::string & out);
  void getPathToSelectedLocalFile(::string & out);

  void getPathToCurrentRemoteFolder(::string & out);
  void getPathToParentRemoteFolder(::string & out);
  void getPathToSelectedRemoteFile(::string & out);

protected:
  //
  // True if window state is closing.
  //

  bool m_isClosing;



  ::string m_lastSentFileListPath;
  ::string m_lastReceivedFileListPath;

  //
  // Buttons
  //

  Control m_renameRemoteButton;
  Control m_mkDirRemoteButton;
  Control m_removeRemoteButton;
  Control m_refreshRemoteButton;

  Control m_renameLocalButton;
  Control m_mkDirLocalButton;
  Control m_removeLocalButton;
  Control m_refreshLocalButton;

  Control m_uploadButton;
  Control m_downloadButton;

  Control m_cancelButton;

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
  // File info of ".." fake folder
  //

  FileInfo *m_fakeMoveUpFolder;

private:

  static const UINT WM_OPERATION_FINISHED = WM_USER + 2;
};


