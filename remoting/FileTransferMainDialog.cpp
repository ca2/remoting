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
#include "FileTransferMainDialog.h"

#include "remoting/remoting_common/util/CommonHeader.h"
#include "remoting/remoting_common/util/winhdr.h"
#include "NewFolderDialog.h"
#include "FileRenameDialog.h"

//#include "file_lib/::file::item.h"

#include "resource.h"
#include <stdio.h>
#include "remoting/remoting_common/remoting.h"


FileTransferMainDialog::FileTransferMainDialog(::remoting::ftp::FileTransferCore *core)
: FileTransferInterface(core)
{
  setResourceId(ftclient_mainDialog);

  m_lastSentFileListPath= "";
  m_lastReceivedFileListPath= "";

  m_fakeMoveUpFolder = new ::remoting::ftp::FileInfo(0, 0, ::remoting::ftp::FileInfo::DIRECTORY, "..");
}

FileTransferMainDialog::~FileTransferMainDialog()
{
  delete m_fakeMoveUpFolder;
}

void FileTransferMainDialog::setProgress(double progress)
{
  WORD pt = 1000;
  WORD pc = static_cast<WORD>(progress * pt);

  m_copyProgressBar.setPos(pc);
}

int FileTransferMainDialog::onFtTargetFileExists(::remoting::ftp::FileInfo *sourceFileInfo,
                                                 ::remoting::ftp::FileInfo *targetFileInfo,
                                                 const ::file::path & pathToTargetFile)
{
  m_fileExistDialog.setFilesInfo(targetFileInfo,
                                 sourceFileInfo,
                                 pathToTargetFile);

  int reasonOfDialog = m_fileExistDialog.showModal();
  switch (reasonOfDialog) {
  case FileExistDialog::SKIP_RESULT:
    return ::remoting::ftp::CopyFileEventListener::TFE_SKIP;
  case FileExistDialog::APPEND_RESULT:
    return ::remoting::ftp::CopyFileEventListener::TFE_APPEND;
  case FileExistDialog::CANCEL_RESULT:
    onCancelOperationButtonClick();
    return ::remoting::ftp::CopyFileEventListener::TFE_CANCEL;
  } // switch

  return ::remoting::ftp::CopyFileEventListener::TFE_OVERWRITE;
}

BOOL FileTransferMainDialog::onInitDialog()
{
  m_isClosing = false;

  initControls();

  tryListRemoteFolder("/");
  tryListLocalFolder("");

  return TRUE;
}

BOOL FileTransferMainDialog::onNotify(UINT controlID, LPARAM data)
{
  LPNMHDR nmhdr = (LPNMHDR)data;
  switch (controlID) {
  case IDC_REMOTE_FILE_LIST:
    switch (nmhdr->code) {
    case NM_DBLCLK:
      onRemoteListViewDoubleClick();
      break;
    case LVN_KEYDOWN:
      {
        LPNMLVKEYDOWN nmlvkd = (LPNMLVKEYDOWN)data;
        onRemoteListViewKeyDown(nmlvkd->wVKey);
      }
      break;
    case LVN_COLUMNCLICK:
      {
        NMLISTVIEW *lpdi = reinterpret_cast<NMLISTVIEW *>(data);
        m_remoteFileListView.sort(lpdi->iSubItem);
      }
      break;
    } // switch notification code

    //
    // FIXME: Not better way to call this method at every notification
    // for ::list_base view control, but windows have no notification for ::list_base view
    // selection changed event. So for now, i didn't found better solution.
    //

    checkRemoteListViewSelection();
    break;
  case IDC_LOCAL_FILE_LIST:
    switch (nmhdr->code) {
    case NM_DBLCLK:
      onLocalListViewDoubleClick();
      break;
    case LVN_KEYDOWN:
      {
        LPNMLVKEYDOWN nmlvkd = (LPNMLVKEYDOWN)data;
        onLocalListViewKeyDown(nmlvkd->wVKey);
      }
      break;
    case LVN_COLUMNCLICK:
      {
        NMLISTVIEW *lpdi = reinterpret_cast<NMLISTVIEW *>(data);
        m_localFileListView.sort(lpdi->iSubItem);
      }
      break;
    } // switch notification code

    //
    // FIXME: Not better way to call this method at every notification
    // for ::list_base view control, but windows have no notification for ::list_base view
    // selection changed event. So for now, i didn't found better solution.
    //

    checkLocalListViewSelection();
    break;
  } // switch controls
  return TRUE;
}

BOOL FileTransferMainDialog::onCommand(UINT controlID, UINT notificationID)
{
  switch (controlID) {
  case IDCANCEL:
    onCancelButtonClick();
    break;
  case IDC_CANCEL_BUTTON:
    onCancelOperationButtonClick();
    break;
  case IDC_RENAME_REMOTE_BUTTON:
    onRenameRemoteButtonClick();
    break;
  case IDC_MKDIR_REMOTE_BUTTON:
    onMkDirRemoteButtonClick();
    break;
  case IDC_REMOVE_REMOTE_BUTTON:
    onRemoveRemoteButtonClick();
    break;
  case IDC_REFRESH_REMOTE_BUTTON:
    onRefreshRemoteButtonClick();
    break;
  case IDC_RENAME_LOCAL_BUTTON:
    onRenameLocalButtonClick();
    break;
  case IDC_MKDIR_LOCAL_BUTTON:
    onMkDirLocalButtonClick();
    break;
  case IDC_REMOVE_LOCAL_BUTTON:
    onRemoveLocalButtonClick();
    break;
  case IDC_REFRESH_LOCAL_BUTTON:
    onRefreshLocalButtonClick();
    break;
  case IDC_UPLOAD_BUTTON:
    onUploadButtonClick();
    break;
  case IDC_DOWNLOAD_BUTTON:
    onDownloadButtonClick();
    break;
  }
  return TRUE;
}

BOOL FileTransferMainDialog::onDestroy()
{
  return TRUE;
}

void FileTransferMainDialog::onMessageReceived(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg) {
  case WM_OPERATION_FINISHED:
    m_ftCore->onOperationFinished();

    if (!m_isClosing) {
      int state = static_cast<int>(wParam);
      int result = static_cast<int>(lParam);
      m_ftCore->onUpdateState(state, result);

      setProgress(0.0);
      enableControls(true);
      break;
    } else { // If window is closing we can it only if operation finished
      close_dialog(0);
      return;
    } 
  } // switch
} // void

bool FileTransferMainDialog::tryClose()
{
  if (m_ftCore->isNothingState()) {
    // No operation is executing - close dialog
    close_dialog(IDCANCEL);
    return true;
  }
  if (::remoting::message_box(m_hwnd,
                 "Do you want to close file transfers and terminate current operation?",
                 "TightVNC ::file::item Transfers",
                 MB_YESNO | MB_ICONQUESTION) == IDYES) {
    // Set flag
    m_isClosing = true;
    // Terminate current operation
    m_ftCore->terminateCurrentOperation();
    return true;
  } // if result is not "yes"
  return false;
}

void FileTransferMainDialog::onCancelButtonClick()
{
  tryClose();
}

void FileTransferMainDialog::onCancelOperationButtonClick()
{
  if (!m_ftCore->isNothingState()) {
  // Logging
    ::string scopedstrMessage("Operation have been canceled by user");
    insertMessageIntoComboBox(scopedstrMessage);

    // Terminate current operation
    m_ftCore->terminateCurrentOperation();
    // Disable "Cancel" button while waiting for a moment
    // when operation will finishe execution.
    m_cancelButton.enable_window(false);
  }
}

void FileTransferMainDialog::onRenameRemoteButtonClick()
{
  ::remoting::ftp::FileInfo *fileInfo = m_remoteFileListView.getSelectedFileInfo();

  if (fileInfo == NULL) {
    ::remoting::message_box(m_hwnd,
               L"No file selected.",
               L"Rename ::file::item", MB_OK | MB_ICONWARNING);
    return ;
  }

  FileRenameDialog renameDialog(this);
  renameDialog.setFileName(fileInfo->getFileName());

  if (renameDialog.showModal() == IDOK) {
    //::string remoteFolder;
    auto remoteFolder  = getPathToCurrentRemoteFolder();

    ::string oldName(fileInfo->getFileName());

    ::string newName;
    newName = renameDialog.getFileName();

    m_ftCore->remoteFileRenameOperation(::remoting::ftp::FileInfo(0, 0, ::remoting::ftp::FileInfo::DIRECTORY, oldName),
                                        ::remoting::ftp::FileInfo(0, 0, ::remoting::ftp::FileInfo::DIRECTORY, newName),
                                        remoteFolder);
  }
}

void FileTransferMainDialog::onMkDirRemoteButtonClick()
{
  NewFolderDialog folderDialog(this);
  if (folderDialog.showModal() == IDOK) {
    ::string remoteFolder;
    remoteFolder = m_remoteCurFolderTextBox.get_text();

    //::string fileName;
    auto fileName = folderDialog.getFileName();


    m_ftCore->remoteFolderCreateOperation(::remoting::ftp::FileInfo(0, 0,
                                                  ::remoting::ftp:: FileInfo::DIRECTORY,
                                                   fileName),
                                          remoteFolder);
  }
}

void FileTransferMainDialog::onRemoveRemoteButtonClick()
{
  unsigned int siCount = m_remoteFileListView.getSelectedItemsCount();

  if (siCount == 0) {
    ::remoting::message_box(m_hwnd,
               L"No files selected.",
               L"Delete Files", MB_OK | MB_ICONWARNING);
    return ;
  }

  //int *indexes = new int[siCount];
  //::remoting::ftp::FileInfo *filesInfo = new ::remoting::ftp::FileInfo[siCount];

   ::pointer_array< ::remoting::ftp::FileInfo> fileinfoa;

  auto indexes = m_remoteFileListView.getSelectedItemsIndexes();
  for (unsigned int i = 0; i < indexes.size(); i++) {
    ::remoting::ftp::FileInfo * pfileinfo = reinterpret_cast<::remoting::ftp::FileInfo *>(m_remoteFileListView.getItemData(indexes[i]));
    fileinfoa.add(pfileinfo);
  }

  if (::remoting::message_box(m_hwnd,
                 L"Do you wish to delete the selected files?",
                 L"Delete Files",
                 MB_YESNO | MB_ICONQUESTION) != IDYES) {
    //delete[] indexes;
    //delete[] filesInfo;
    return ;
  }

  //::string remoteFolder;
  auto remoteFolder = m_remoteCurFolderTextBox.get_text();

  m_ftCore->remoteFilesDeleteOperation(fileinfoa,
                                       remoteFolder);
  //delete[] indexes;
  //delete[] filesInfo;
}

void FileTransferMainDialog::onRefreshRemoteButtonClick()
{
  refreshRemoteFileList();
}

void FileTransferMainDialog::onRenameLocalButtonClick()
{
  ::remoting::ftp::FileInfo *fileInfo = m_localFileListView.getSelectedFileInfo();

  if (fileInfo == NULL) {
    ::remoting::message_box(m_hwnd,
               L"No file selected.",
               L"Rename ::file::item", MB_OK | MB_ICONWARNING);
    return ;
  }

  FileRenameDialog renameDialog(this);
  renameDialog.setFileName(fileInfo->getFileName());

  if (renameDialog.showModal() == IDOK) {
    //::string localFolder;
    auto localFolder = getPathToCurrentLocalFolder();

    //::string oldName;
    //::string newName;

    auto oldName = fileInfo->getFileName();
    auto newName = renameDialog.getFileName();

    ::file::path pathToOldFile(localFolder);
    ::file::path pathToNewFile(localFolder);

    //if (!localFolder.ends('\\')) {
//      pathToOldFile.appendString("\\");
  //    pathToNewFile.appendString("\\");
//    }

    pathToOldFile /= oldName;
    pathToNewFile /= newName;

    //
    // Logging
    //

    ::string scopedstrMessage;

    scopedstrMessage.format("Renaming local file '{}' to '{}'",
                   pathToOldFile, pathToNewFile);

    insertMessageIntoComboBox(scopedstrMessage);


     throw ::todo;

    //  // Rename local file
    // ::file::item oldFile(pathToOldFile);
    //
    // if (!oldFile.renameTo(pathToNewFile)) {
    //   scopedstrMessage.formatf("Error: failed to rename local '{}' file",
    //                  pathToOldFile);
    //
    //   insertMessageIntoComboBox(scopedstrMessage);
    // }

    refreshLocalFileList();
  } // if dialog result is ok
} // void

void FileTransferMainDialog::onMkDirLocalButtonClick()
{
  //::file::path pathToFile;

  auto pathToFile = getPathToCurrentLocalFolder();

  // Not allow user to create folders in our "fake" root folder
  if (pathToFile.is_empty()) {
    ::remoting::message_box(m_hwnd,
               L"It's not allowed to create new folder here.",
               L"New Folder", MB_OK | MB_ICONWARNING);
  }

  NewFolderDialog folderDialog(this);

  if (folderDialog.showModal() == IDOK) {
    //::string fileName;
    auto fileName = folderDialog.getFileName();

    //if (!pathToFile.ends(_T('\\'))) {
      //pathToFile.appendString("\\");
    //}
    pathToFile /= fileName;

    // Logging
    ::string scopedstrMessage;

    scopedstrMessage.format("Creating local folder '{}'", pathToFile);

    insertMessageIntoComboBox(scopedstrMessage);


     throw todo;

    // // ::file::item system object
    // ::file::item file(pathToFile);
    //
    // // Failed to create local folder
    // if (pathToFile.is_empty() || !file.mkdir()) {
    //   scopedstrMessage.formatf("Error: failed to create local folder '{}'",
    //                  pathToFile);

      insertMessageIntoComboBox(scopedstrMessage);
    }

    refreshLocalFileList();
  } // if dialog result is ok
//} // void

void FileTransferMainDialog::onRemoveLocalButtonClick()
{
  unsigned int siCount = m_localFileListView.getSelectedItemsCount();

  if (siCount == 0) {
    ::remoting::message_box(m_hwnd,
               L"No files selected.",
               L"Delete Files", MB_OK | MB_ICONWARNING);
    return ;
  }

  //int *indexes = new int[siCount];
  //::remoting::ftp::FileInfo *filesInfo = new ::remoting::ftp::FileInfo[siCount];
   ::pointer_array<::remoting::ftp::FileInfo> fileinfoa;

  auto indexes = m_localFileListView.getSelectedItemsIndexes();
  for (unsigned int i = 0; i < indexes.size(); i++) {
    ::remoting::ftp::FileInfo *fileInfo = reinterpret_cast<::remoting::ftp::FileInfo *>(m_localFileListView.getItemData(indexes[i]));
    fileinfoa.add(fileInfo);
  }

  if (::remoting::message_box(m_hwnd,
                 L"Do you wish to delete the selected files?",
                 L"Delete Files",
                 MB_YESNO | MB_ICONQUESTION) != IDYES) {
    //delete[] indexes;
    //delete[] filesInfo;
    return ;
  }

  //::file::path localFolder;
  auto localFolder = getPathToCurrentLocalFolder();

  m_ftCore->localFilesDeleteOperation(fileinfoa,
                                      localFolder);

  //delete[] indexes;
  //delete[] filesInfo;
}

void FileTransferMainDialog::onRefreshLocalButtonClick()
{
  refreshLocalFileList();
}

void FileTransferMainDialog::onUploadButtonClick()
{
  //unsigned int siCount = m_localFileListView.getSelectedItemsCount();
   ::pointer_array<::remoting::ftp::FileInfo> fileinfoa;

   auto indexes = m_localFileListView.getSelectedItemsIndexes();

  if (indexes.is_empty()) {
    ::remoting::message_box(m_hwnd,
               L"No files selected.",
               L"Upload Files", MB_OK | MB_ICONWARNING);
    return ;
  }

  ///int *indexes = new int[siCount];
  //FileInfo *filesInfo = new FileInfo[siCount];

  for (unsigned int i = 0; i < indexes.size(); i++) {
    ::remoting::ftp::FileInfo *fileInfo = reinterpret_cast<::remoting::ftp::FileInfo *>(m_localFileListView.getItemData(indexes[i]));
    fileinfoa.add(fileInfo);
  }

  if (::remoting::message_box(m_hwnd,
                 L"Do you wish to upload the selected files?",
                 L"Upload Files",
                 MB_YESNO | MB_ICONQUESTION) != IDYES) {
    //delete[] indexes;
    //delete[] filesInfo;
    return ;
  }

  //::string localFolder;
  auto localFolder = getPathToCurrentLocalFolder();

  //::string remoteFolder;
  auto remoteFolder = getPathToCurrentRemoteFolder();


  m_fileExistDialog.resetDialogResultValue();

  m_ftCore->uploadOperation(fileinfoa,
                            localFolder,
                            remoteFolder);
  //delete[] indexes;
  //delete[] filesInfo;
}

void FileTransferMainDialog::onDownloadButtonClick()
{
//   ::pointer_array<::remoting::ftp::FileInfo> fileinfoa;

   auto indexes = m_remoteFileListView.getSelectedItemsIndexes();

   if (indexes.is_empty()) {
      ::remoting::message_box(m_hwnd,
L"No files selected.",
               L"Download Files", MB_OK | MB_ICONWARNING);
    return ;
  }

  //int *indexes = new int[siCount];
  //FileInfo *filesInfo = new FileInfo[siCount];

   ::pointer_array<::remoting::ftp::FileInfo> fileinfoa;

  //m_remoteFileListView.getSelectedItemsIndexes(indexes);
  for (unsigned int i = 0; i < indexes.size(); i++) {
    auto fileInfo = reinterpret_cast<::remoting::ftp::FileInfo *>(m_remoteFileListView.getItemData(indexes[i]));
    fileinfoa.add(fileInfo);
  }

  if (::remoting::message_box(m_hwnd,
                 L"Do you wish to download the selected files?",
                 L"Download Files",
                 MB_YESNO | MB_ICONQUESTION) != IDYES) {
    //delete[] indexes;
    //delete[] filesInfo;
    return ;
  }

  //::string remoteFolder;
  auto remoteFolder = getPathToCurrentRemoteFolder();

  //::string localFolder;
  auto localFolder = getPathToCurrentLocalFolder();

  m_fileExistDialog.resetDialogResultValue();

  m_ftCore->downloadOperation(fileinfoa,
                              localFolder,
                              remoteFolder);

  //delete[] indexes;
  //delete[] filesInfo;
}

void FileTransferMainDialog::moveUpLocalFolder()
{
  //::file::path pathToFile;
  auto pathToFile = getPathToParentLocalFolder();
  tryListLocalFolder(pathToFile);
}

void FileTransferMainDialog::moveUpRemoteFolder()
{
  //::file::path parent;
  auto parent = getPathToParentRemoteFolder();
  tryListRemoteFolder(parent);
}

void FileTransferMainDialog::onRemoteListViewDoubleClick()
{
  ::remoting::ftp::FileInfo *selFileInfo = m_remoteFileListView.getSelectedFileInfo();
  if (selFileInfo == 0)
    return;

  if (!selFileInfo->isDirectory())
    return;

  int si = m_remoteFileListView.getSelectedIndex();

  // Fake ".." folder - move one folder up
  if ((si == 0) && (selFileInfo != 0) && selFileInfo->getFileName() ==  "..")
     {
    moveUpRemoteFolder();
    return ;
  }
  if (si == -1) {
    return ;
  }
  //::string pathToFile;
  auto pathToFile = getPathToSelectedRemoteFile();
  tryListRemoteFolder(pathToFile);
}

void FileTransferMainDialog::onLocalListViewDoubleClick()
{
  // FIXME: removed duplicate code (see onRemoteListViewDoubleClick)
  ::remoting::ftp::FileInfo *selFileInfo = m_localFileListView.getSelectedFileInfo();

  if (selFileInfo == 0)
    return;

  if (!selFileInfo->isDirectory())
    return;

  int si = m_localFileListView.getSelectedIndex();

  // Fake ".." folder - move one folder up
  if ((si == 0) && (selFileInfo != 0) && selFileInfo->getFileName() == "..") {
    moveUpLocalFolder();
    return ;
  }
  if (si == -1) {
    return ;
  }

  //::string pathToFile;
  auto pathToFile = getPathToSelectedLocalFile();
  tryListLocalFolder(pathToFile);
}

void FileTransferMainDialog::onRemoteListViewKeyDown(UINT key)
{
  switch (key) {
  case VK_RETURN:
    onRemoteListViewDoubleClick();
    break;
  case VK_DELETE:
    onRemoveRemoteButtonClick();
    break;
  case VK_BACK:
    moveUpRemoteFolder();
    break;
  }
}

void FileTransferMainDialog::onLocalListViewKeyDown(UINT key)
{
  // FIXME: removed duplicate code (see onRemoteListViewKeyDown)
  switch (key) {
  case VK_RETURN:
    onLocalListViewDoubleClick();
    break;
  case VK_DELETE:
    onRemoveLocalButtonClick();
    break;
  case VK_BACK:
    moveUpLocalFolder();
    break;
  }
}

void FileTransferMainDialog::checkRemoteListViewSelection()
{
  if (m_ftCore->isNothingState()) {

    bool enabled = m_remoteFileListView.getSelectedItemsCount() > 0;

    m_renameRemoteButton.enable_window(enabled && m_ftCore->getSupportedOps().isRenameSupported());
    m_removeRemoteButton.enable_window(enabled && m_ftCore->getSupportedOps().isRemoveSupported());
  }
}

void FileTransferMainDialog::checkLocalListViewSelection()
{
  if (m_ftCore->isNothingState()) {
    bool enabled = m_localFileListView.getSelectedItemsCount() > 0;

    m_renameLocalButton.enable_window(enabled);
    m_removeLocalButton.enable_window(enabled);
  }
}

void FileTransferMainDialog::insertMessageIntoComboBox(const ::scoped_string & scopedstrMessage)
{
  m_logComboBox.insertItem(0, scopedstrMessage);
  m_logComboBox.setSelectedItem(0);
}

void FileTransferMainDialog::enableControls(bool enabled)
{
  m_mkDirRemoteButton.enable_window(enabled && m_ftCore->getSupportedOps().isMkDirSupported());

  if (m_remoteFileListView.getSelectedItemsCount() > 0 && enabled) {
    m_renameRemoteButton.enable_window(true && m_ftCore->getSupportedOps().isRenameSupported());
    m_removeRemoteButton.enable_window(true && m_ftCore->getSupportedOps().isRemoveSupported());
  } else {
    m_renameRemoteButton.enable_window(enabled && m_ftCore->getSupportedOps().isRenameSupported());
    m_removeRemoteButton.enable_window(enabled && m_ftCore->getSupportedOps().isRemoveSupported());
  }

  m_refreshRemoteButton.enable_window(enabled);

  if (enabled) {
    //::string curLocalPath;

    auto curLocalPath = getPathToCurrentLocalFolder();

    if (!curLocalPath.is_empty()) {
      m_mkDirLocalButton.enable_window(true);
    }
  } else {
    m_mkDirLocalButton.enable_window(enabled);
  }

  if (m_localFileListView.getSelectedItemsCount() > 0 && enabled) {
    m_renameLocalButton.enable_window(true);
    m_removeLocalButton.enable_window(true);
  } else {
    m_renameLocalButton.enable_window(enabled);
    m_removeLocalButton.enable_window(enabled);
  }

  m_refreshLocalButton.enable_window(enabled);

  m_uploadButton.enable_window(enabled && m_ftCore->getSupportedOps().isUploadSupported());
  m_downloadButton.enable_window(enabled && m_ftCore->getSupportedOps().isDownloadSupported());

  m_localFileListView.enable_window(enabled);
  m_remoteFileListView.enable_window(enabled);

  m_cancelButton.enable_window(!enabled);
}

void FileTransferMainDialog::initControls()
{
  HWND hwnd = m_hwnd;

  m_renameRemoteButton.setWindow(GetDlgItem(hwnd, IDC_RENAME_REMOTE_BUTTON));
  m_mkDirRemoteButton.setWindow(GetDlgItem(hwnd, IDC_MKDIR_REMOTE_BUTTON));
  m_removeRemoteButton.setWindow(GetDlgItem(hwnd, IDC_REMOVE_REMOTE_BUTTON));
  m_refreshRemoteButton.setWindow(GetDlgItem(hwnd, IDC_REFRESH_REMOTE_BUTTON));

  m_renameLocalButton.setWindow(GetDlgItem(hwnd, IDC_RENAME_LOCAL_BUTTON));
  m_mkDirLocalButton.setWindow(GetDlgItem(hwnd, IDC_MKDIR_LOCAL_BUTTON));
  m_removeLocalButton.setWindow(GetDlgItem(hwnd, IDC_REMOVE_LOCAL_BUTTON));
  m_refreshLocalButton.setWindow(GetDlgItem(hwnd, IDC_REFRESH_LOCAL_BUTTON));

  m_uploadButton.setWindow(GetDlgItem(hwnd, IDC_UPLOAD_BUTTON));
  m_downloadButton.setWindow(GetDlgItem(hwnd, IDC_DOWNLOAD_BUTTON));

  m_cancelButton.setWindow(GetDlgItem(hwnd, IDC_CANCEL_BUTTON));

  m_copyProgressBar.setWindow(GetDlgItem(hwnd, IDC_TOTAL_PROGRESS));
  m_copyProgressBar.setRange(0, 1000);

  m_logComboBox.setWindow(GetDlgItem(hwnd, IDC_LOG_COMBO));

  m_localCurFolderTextBox.setWindow(GetDlgItem(hwnd, IDC_LOCAL_CURRENT_FOLDER_EDIT));
  m_remoteCurFolderTextBox.setWindow(GetDlgItem(hwnd, IDC_REMOTE_CURRENT_FOLDER_EDIT));

  m_localFileListView.setWindow(GetDlgItem(hwnd, IDC_LOCAL_FILE_LIST));
  m_remoteFileListView.setWindow(GetDlgItem(hwnd, IDC_REMOTE_FILE_LIST));

  m_fileExistDialog.set_parent(this);
}

void FileTransferMainDialog::raise(::exception &ex)
{
  ::remoting::message_box(m_hwnd, ::wstring(ex.get_message()),
             L"Exception", MB_OK | MB_ICONERROR);
  throw ex;
}

void FileTransferMainDialog::refreshLocalFileList()
{
  //::string pathToFile;
  auto pathToFile = getPathToCurrentLocalFolder();
  tryListLocalFolder(pathToFile);
}

void FileTransferMainDialog::tryListLocalFolder(const ::file::path & pathToFile)
{
  try {
    //::array_base <::remoting::ftp::FileInfo> *localFileList = m_ftCore->getListLocalFolder(pathToFile);

     auto localFileList = m_ftCore->getListLocalFolder(pathToFile);


    // Add to ::list_base view
    m_localFileListView.clear();
    if (localFileList.has_element())
    {
      //::remoting::ftp::FileInfo *fileInfo = localFileList->data();
      m_localFileListView.addRange(localFileList);
    }

    bool isRoot = pathToFile.is_empty();

    // Add ".." folder and if not root
    if (!isRoot) {
      m_localFileListView.addItem(0, m_fakeMoveUpFolder);
    }
    // Set label text
    m_localCurFolderTextBox.setText(pathToFile);
    // Enable or disable mkdir button depending on isRoot flag
    m_mkDirLocalButton.enable_window(!isRoot);

  } catch (...) {
    ::string scopedstrMessage;

    scopedstrMessage.format("Error: failed to get file ::list_base in local folder '{}'",
                   pathToFile   );

    insertMessageIntoComboBox(scopedstrMessage);
    return;
  }
}

void FileTransferMainDialog::refreshRemoteFileList()
{
  ::string currentFolder;
  currentFolder = m_remoteCurFolderTextBox.get_text();
  tryListRemoteFolder(currentFolder);
}

void FileTransferMainDialog::tryListRemoteFolder(const ::file::path & pathToFile)
{
  m_lastSentFileListPath= pathToFile;
  m_ftCore->remoteFileListOperation(pathToFile);
}

::file::path FileTransferMainDialog::getPathToCurrentLocalFolder()
{
  return m_localCurFolderTextBox.get_text();
}

::file::path FileTransferMainDialog::getPathToParentLocalFolder()
{
  auto out = getPathToCurrentLocalFolder();
//   size_t ld = out.findLast(_T('\\'));
//   if (ld != (size_t)-1) {
//     out.getSubstring(out, 0, ld);
//   } else {
//     out= "";
//     return out;
//   }
//   //if (out->endsWith('\\') && (out->length() > 2)) {
// //    out->getSubstring(out, 0, out->length() - 2);
//   //}

   out -=1;
   return out;
}

::file::path FileTransferMainDialog::getPathToSelectedLocalFile()
{
  //::string & pathToFile = out;
  auto pathToFile = getPathToCurrentLocalFolder();

//  if (!pathToFile->is_empty() && !pathToFile->endsWith(_T('\\'))) {
  //  pathToFile->appendString("\\");
  //}

  auto strFilename = m_localFileListView.getSelectedFileInfo()->getFileName();
  //pathToFile->appendString(filename);

   pathToFile /= strFilename;

   return pathToFile;

}


::file::path FileTransferMainDialog::getPathToCurrentRemoteFolder()
{

   return m_remoteCurFolderTextBox.get_text();

}


::file::path FileTransferMainDialog::getPathToParentRemoteFolder()
{
  auto out = getPathToCurrentRemoteFolder();
  // size_t ld = out->findLast(_T('/'));
  // if (ld != (size_t)-1) {
  //   out->getSubstring(out, 0, ld);
  // } else {
  //   out-= "/";
  //   return ;
  // }
  // if (out->endsWith('/') && (out->length() > 2)) {
  //   out->getSubstring(out, 0, out->length() - 2);
  // }

   out -=1;

   return out;



}


::file::path FileTransferMainDialog::getPathToSelectedRemoteFile()
{
  //::string & pathToFile = out;
  auto pathToFile = getPathToCurrentRemoteFolder();

  // if (!pathToFile->endsWith(_T('/'))) {
  //   pathToFile->appendString("/");
  // }

  //const ::scoped_string & scopedstrFilename = m_remoteFileListView.getSelectedFileInfo()->getFileName();
   auto strFilename = m_remoteFileListView.getSelectedFileInfo()->getFileName();
  //pathToFile->appendString(filename);

   pathToFile /= strFilename;


   return pathToFile;

}

void FileTransferMainDialog::setNothingState()
{
  m_lastReceivedFileListPath = m_lastSentFileListPath;
  m_remoteCurFolderTextBox.setText(m_lastReceivedFileListPath);

  m_remoteFileListView.clear();
  auto fileRemoteList = m_ftCore->getListRemoteFolder();
  if (fileRemoteList.has_element()) {
    //::remoting::ftp::FileInfo *filesInfo = fileRemoteList->data();
    m_remoteFileListView.addRange(fileRemoteList);
  }

  bool isRoot = m_lastSentFileListPath == "/";

  // Add fake ".." folder if not root
  if (!isRoot) {
    m_remoteFileListView.addItem(0, m_fakeMoveUpFolder);
  }
}

void FileTransferMainDialog::onFtOpError(const ::scoped_string & scopedstrMessage)
{
  insertMessageIntoComboBox(scopedstrMessage);
}

void FileTransferMainDialog::onFtOpInfo(const ::scoped_string & scopedstrMessage)
{
  insertMessageIntoComboBox(scopedstrMessage);
}

void FileTransferMainDialog::onFtOpStarted()
{
  enableControls(false);
}

void FileTransferMainDialog::onFtOpFinished(int state, int result)
{
  PostMessage(m_hwnd, WM_OPERATION_FINISHED, state, result);
}

void FileTransferMainDialog::onRefreshLocalFileList()
{
  refreshLocalFileList();
}
void FileTransferMainDialog::onRefreshRemoteFileList()
{
  refreshRemoteFileList();
}
