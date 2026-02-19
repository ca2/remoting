// Copyright (C) 2011,2012 GlavSoft LLC.
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
#include "config_lib/IniFileSettingsManager.h"
#include "util/Exception.h"
#include "util/ResourceLoader.h"
#include "rfb/StandardPixelFormatFactory.h"

#include "FsWarningDialog.h"
#include "NamingDefs.h"
#include "remoting_impact.h"
#include "ViewerWindow.h"
#include "acme/constant/id.h"
#include "acme/filesystem/file/item.h"
#include "acme/platform/application.h"
#include "remoting/common/remoting.h"

#include <commdlg.h>

ViewerWindow::ViewerWindow(WindowsApplication *application,
                           ConnectionData *conData,
                           ConnectionConfig *conConf,
                           LogWriter *LogWriter)
: m_ccsm(RegistryPaths::VIEWER_PATH,
         conData->getHost()),
  m_application(application),
  m_logWriter(LogWriter),
  m_conConf(conConf),
  m_scale(100),
  m_isFullScr(false),
  m_ftDialog(0),
  m_viewerCore(0),
  m_fileTransfer(0),
  m_conData(conData),
  m_dsktWnd(m_logWriter, conConf),
  m_isConnected(false),
  m_sizeIsChanged(false),
  m_hooksEnabledFirstTime(true),
  m_requiresReconnect(false),
  m_stopped(false)
{
   initialize(application);
  m_standardScale.add(10);
  m_standardScale.add(15);
  m_standardScale.add(25);
  m_standardScale.add(50);
  m_standardScale.add(75);
  m_standardScale.add(90);
  m_standardScale.add(100);
  m_standardScale.add(150);
  m_standardScale.add(200);
  m_standardScale.add(400);

  ::string windowClass = WindowNames::TVN_WINDOW_CLASS_NAME;
  ::string titleName = WindowNames::TVN_WINDOW_TITLE_NAME;
  ::string subTitleName = WindowNames::TVN_SUB_WINDOW_TITLE_NAME;

  setClass(windowClass);
  createWindow(titleName, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);

  m_dsktWnd.setClass(windowClass);
  m_dsktWnd.createWindow(subTitleName,
                         WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_CHILD,
                         getHWnd());

  SetTimer(m_hwnd, TIMER_DESKTOP_STATE, TIMER_DESKTOP_STATE_DELAY, (TIMERPROC)NULL);
}

ViewerWindow::~ViewerWindow()
{
  // Unregistration of keyboard hook.
  m_winHooks.unregisterKeyboardHook(this);

  if (m_ftDialog != 0) {
    try {
      delete m_ftDialog;
    } catch (...) {
    }
    m_ftDialog = 0;
  }
}

void ViewerWindow::setFileTransfer(::remoting::ftp::FileTransferCapability *ft)
{
  m_fileTransfer = ft;
}

void ViewerWindow::setRemoteViewerCore(RemoteViewerCore *pCore)
{
  m_viewerCore = pCore;
  m_dsktWnd.setViewerCore(pCore);
  applySettings();
}

bool ViewerWindow::onCreate(LPCREATESTRUCT lps)
{
  m_control.setWindow(m_hwnd);

  setClassCursor(LoadCursor(NULL, IDC_ARROW));
  loadIcon(IDI_APPICON);
  m_toolbar.loadToolBarfromRes(IDB_TOOLBAR);
  m_toolbar.setButtonsRange(IDS_TB_NEWCONNECTION);
  m_toolbar.setViewAutoButtons(4, ::remoting::ToolBar::TB_Style_sep);
  m_toolbar.setViewAutoButtons(6, ::remoting::ToolBar::TB_Style_sep);
  m_toolbar.setViewAutoButtons(10, ::remoting::ToolBar::TB_Style_sep);
  m_toolbar.setViewAutoButtons(11, ::remoting::ToolBar::TB_Style_sep);
  m_toolbar.setViewAutoButtons(15, ::remoting::ToolBar::TB_Style_sep);
  m_toolbar.attachToolBar(getHWnd());
  m_menu.getSystemMenu(getHWnd());
  m_menu.loadMenu();
  applySettings();
   m_timeStart.Now();
 ::remoting:: ViewerConfig *config = ::remoting::ViewerConfig::getInstance();
  bool bShowToolbar = config->isToolbarShown();
  if (!bShowToolbar) {
    m_toolbar.hide();
    m_bToolBar = false;
  }
  m_menu.checkedMenuItem(IDS_TB_TOOLBAR, bShowToolbar);
  return true;
}

void ViewerWindow::enableUserElements()
{
  bool isEnable = !m_conConf->isViewOnly();

  m_toolbar.enableButton(IDS_TB_ALT, isEnable);
  m_toolbar.enableButton(IDS_TB_CTRL, isEnable);
  m_toolbar.enableButton(IDS_TB_CTRLESC, isEnable);
  m_toolbar.enableButton(IDS_TB_CTRLALTDEL, isEnable);

  UINT enableMenu = static_cast<UINT>(!isEnable);
  m_menu.enableMenuItem(IDS_TB_CTRLALTDEL, enableMenu);
  m_menu.enableMenuItem(IDS_TB_CTRLESC, enableMenu);
  m_menu.enableMenuItem(IDS_TB_CTRL, enableMenu);
  m_menu.enableMenuItem(IDS_TB_ALT, enableMenu);

  if (!isEnable) {
    m_menu.checkedMenuItem(IDS_TB_ALT, false);
    m_menu.checkedMenuItem(IDS_TB_CTRL, false);
  }

  int scale = m_conConf->getScaleNumerator() * 100 / m_conConf->getScaleDenominator();
  m_toolbar.enableButton(IDS_TB_SCALEOUT, scale > m_standardScale[0]);
  m_toolbar.enableButton(IDS_TB_SCALEIN, scale < m_standardScale[m_standardScale.size() - 1]);
  if (m_conConf->isFitWindowEnabled()) {
    m_toolbar.checkButton(IDS_TB_SCALEAUTO, true);
    m_toolbar.enableButton(IDS_TB_SCALE100, true);
  } else {
    m_toolbar.enableButton(IDS_TB_SCALE100, scale != 100);
  }
}

bool ViewerWindow::viewerCoreSettings()
{
  if (!m_viewerCore) {
    return false;
  }

  bool bFileTransfer = m_fileTransfer && m_fileTransfer->isEnabled();

  m_toolbar.enableButton(IDS_TB_TRANSFER, bFileTransfer && !m_conConf->isViewOnly());
  UINT enableMenu = static_cast<UINT>(!(bFileTransfer && !m_conConf->isViewOnly()));
  m_menu.enableMenuItem(IDS_TB_TRANSFER, enableMenu);

  m_viewerCore->allowCopyRect(m_conConf->isCopyRectAllowed());
  m_viewerCore->setPreferredEncoding(m_conConf->getPreferredEncoding());

  m_viewerCore->ignoreCursorShapeUpdates(m_conConf->isIgnoringShapeUpdates());
  m_viewerCore->enableCursorShapeUpdates(m_conConf->isRequestingShapeUpdates());

  // set -1, if compression is disabled
  m_viewerCore->setCompressionLevel(m_conConf->getCustomCompressionLevel());

  // set -1, if jpeg-compression is disabled
  m_viewerCore->setJpegQualityLevel(m_conConf->getJpegCompressionLevel());

  if (m_conConf->isUsing8BitColor()) {
    m_viewerCore->setPixelFormat(StandardPixelFormatFactory::create8bppPixelFormat());
  } else {
    m_viewerCore->setPixelFormat(StandardPixelFormatFactory::create32bppPixelFormat());
  }
  return true;
}

void ViewerWindow::applySettings()
{
  int scale;

  if (m_conConf->isFitWindowEnabled()) {
    scale = -1;
  } else {
    int iNum = m_conConf->getScaleNumerator();
    int iDenom = m_conConf->getScaleDenominator();
    scale = (iNum * 100) / iDenom;
  }

  if (scale != m_scale) {
    m_scale = scale;
    m_dsktWnd.setScale(m_scale);
    doSize();
  }
  if (m_isConnected) {
    if (m_conConf->isFullscreenEnabled()) {
      doFullScr();
    } else {
      doUnFullScr();
    }
  }
  changeCursor(m_conConf->getLocalCursorShape());
  enableUserElements();
  viewerCoreSettings();
}

void ViewerWindow::changeCursor(int type)
{
  HCURSOR hcur = 0;

  ResourceLoader *rLoader = ResourceLoader::getInstance();
  switch (type) {
    case ConnectionConfig::DOT_CURSOR:
      hcur = rLoader->loadCursor(IDI_CDOT);
      break;
    case ConnectionConfig::SMALL_CURSOR:
      hcur = rLoader->loadCursor(IDI_CSMALLDOT);
      break;
    case ConnectionConfig::NO_CURSOR:
      hcur = rLoader->loadCursor(IDI_CNOCURSOR);
      break;
    case ConnectionConfig::NORMAL_CURSOR:
      hcur = rLoader->loadStandardCursor(IDC_ARROW);
      break;
  }
  setClassCursor(hcur);
}

bool ViewerWindow::onSysCommand(WPARAM wParam, LPARAM lParam)
{
  return onCommand(wParam, lParam);
}

bool ViewerWindow::onMessage(UINT scopedstrMessage, WPARAM wParam, LPARAM lParam)
{
  switch (scopedstrMessage) {
  case WM_SIZING:
    m_sizeIsChanged = true;
    return false;
  case WM_NCDESTROY:
    m_stopped = true;
    return true;
  case WM_USER_STOP:
    SendMessage(m_hwnd, WM_DESTROY, 0, 0);
    return true;
  case WM_USER_FS_WARNING:
    return onFsWarning();
     case WM_USER_SWITCH_FULL_SCREEN_MODE:
        switchFullScreenMode();
        return true;
     case WM_CLOSE:
    return onClose();
  case WM_DESTROY:
    return onDestroy();
  case WM_CREATE:
    return onCreate((LPCREATESTRUCT) lParam);
  case WM_SIZE:
    return onSize(wParam, lParam);
  case WM_USER_AUTH_ERROR:
    return onAuthError(wParam);
  case WM_USER_ERROR:
    return onError();
  case WM_USER_DISCONNECT:
    return onDisconnect();
  case WM_ACTIVATE:
    if (!m_isFullScr) {
      return true;
    }
    if (LOWORD(wParam) == WA_ACTIVE || LOWORD(wParam) == WA_CLICKACTIVE) {
      // full screen viewer can be minimized from other screen
      if (IsIconic(this->getHWnd())) {
        return true;
      }
      try {
        // Registration of keyboard hook.
        m_winHooks.registerKeyboardHook(this);
        // Switching off ignoring win key.
        m_dsktWnd.setWinKeyIgnore(false);
      } catch (::exception &e) {
        m_logWriter->error("{}", e.get_message());
      }
    } else if (LOWORD(wParam) == WA_INACTIVE) {
      // Unregistration of keyboard hook.
      m_winHooks.unregisterKeyboardHook(this);
      // Switching on ignoring win key.
      m_dsktWnd.setWinKeyIgnore(true);
    }
    return true;
  case WM_SETFOCUS:
    return onFocus(wParam);
  case WM_ERASEBKGND:
    return onEraseBackground((HDC)wParam);
  case WM_KILLFOCUS:
    return onKillFocus(wParam);
  case WM_TIMER:
    return onTimer(wParam);
  case WM_DISPLAYCHANGE:
    adjustWindowSize();

  }
  return false;
}

bool ViewerWindow::onEraseBackground(HDC hdc) 
{
  return true;
}

bool ViewerWindow::onKillFocus(WPARAM wParam)
{
  if (!m_conConf->isViewOnly()) {
    m_toolbar.checkButton(IDS_TB_ALT, false);
    m_toolbar.checkButton(IDS_TB_CTRL, false);
  }
  return true;
}

bool ViewerWindow::onTimer(WPARAM idTimer)
{
  switch (idTimer) {
  case TIMER_DESKTOP_STATE:
    desktopStateUpdate();
    return true;
  default:
    _ASSERT(false);
    return false;
  }
}

void ViewerWindow::dialogConnectionOptions() 
{
  OptionsDialog dialog;

  dialog.setConnected();
  dialog.setConnectionConfig(m_conConf);
  // FIXME: Removed ::remoting::Window from this code and another
   //auto pcontrol = øcreate_new<::remoting::Window>();
   //pcontrol ->_setWindow(getHWnd());
  dialog.set_parent(this);

  if (dialog.showModal() == 1) {
    m_conConf->saveToStorage(&m_ccsm);
    applySettings();
  }
}

void ViewerWindow::dialogConnectionInfo() 
{
  ::string host = m_conData->getHost();
  ::array_base<TCHAR> kbdName;
  kbdName.resize(KL_NAMELENGTH);
  memset(&kbdName[0], 0, sizeof(TCHAR) * KL_NAMELENGTH);
  if (!GetKeyboardLayoutName( &kbdName[0] )) {
    kbdName[0] = _T('?'); 
    kbdName[1] = _T('?'); 
    kbdName[2] = _T('?'); 
  }

  ::int_rectangle geometry;
  int pixelSize = 0;
  m_dsktWnd.getServerGeometry(&geometry, &pixelSize);
  ::string str;
  str.formatf(StringTable::getString(IDS_CONNECTION_INFO_FORMAT).c_str(),
             host.c_str(),
             m_viewerCore->getRemoteDesktopName().c_str(),
             m_viewerCore->getProtocolString().c_str(),
             geometry.width(),
             geometry.height(),
             pixelSize,
             &kbdName[0]);
  ::remoting::message_box(getHWnd(),
             str,
             StringTable::getString(IDS_CONNECTION_INFO_CAPTION),
             MB_OK | MB_ICONINFORMATION);
}

void ViewerWindow::switchFullScreenMode()
{
  if (m_isFullScr) {
    m_logWriter->debug("Switch to windowed mode");
    doUnFullScr();
  } else {
    m_logWriter->debug("Switch to full screen mode");
    doFullScr();
  }
}

void ViewerWindow::dialogConfiguration() 
{
  m_application->postMessage(remoting_impact::WM_USER_CONFIGURATION);
}

void ViewerWindow::onGoodCursor()
{

   m_dsktWnd.m_timeStartDesktopWindow.m_iSecond -= 120;

}
void ViewerWindow::desktopStateUpdate()
{
  // Adjust window of viewer to size of remote desktop.
  adjustWindowSize();

  // Update state of toolbar-buttons (Ctrl, Alt) to hardware-button state.
  updateKeyState();
}

void ViewerWindow::commandCtrlAltDel() 
{
  LRESULT iState = m_toolbar.getState(IDS_TB_CTRLALTDEL);
  if (iState) {
    m_dsktWnd.sendCtrlAltDel();
  }
}

void ViewerWindow::commandCtrlEsc()
{
  LRESULT iState = m_toolbar.getState(IDS_TB_CTRLESC);
  if (iState) {
    m_dsktWnd.sendKey(VK_LCONTROL, true);
    m_dsktWnd.sendKey(VK_ESCAPE,   true);
    m_dsktWnd.sendKey(VK_ESCAPE,   false);
    m_dsktWnd.sendKey(VK_LCONTROL, false);
  }
}

void ViewerWindow::commandCtrl()
{
  LRESULT iState = m_toolbar.getState(IDS_TB_CTRL);
  if (iState) {
    if (iState == TBSTATE_ENABLED) {
      m_menu.checkedMenuItem(IDS_TB_CTRL, true);
      m_toolbar.checkButton(IDS_TB_CTRL,  true);
      m_dsktWnd.setCtrlState(true);
      m_dsktWnd.sendKey(VK_LCONTROL,      true);
    } else {
      m_menu.checkedMenuItem(IDS_TB_CTRL, false);
      m_toolbar.checkButton(IDS_TB_CTRL,  false);
      m_dsktWnd.setCtrlState(false);
      m_dsktWnd.sendKey(VK_LCONTROL,      false);
    }
  }
}

void ViewerWindow::commandAlt()
{
  LRESULT iState = m_toolbar.getState(IDS_TB_ALT);
  if (iState) {
    if (iState == TBSTATE_ENABLED) {
      m_menu.checkedMenuItem(IDS_TB_ALT, true);
      m_toolbar.checkButton(IDS_TB_ALT,  true);
      m_dsktWnd.setAltState(true);
      m_dsktWnd.sendKey(VK_LMENU,        true);
    } else {
      m_menu.checkedMenuItem(IDS_TB_ALT, false);
      m_toolbar.checkButton(IDS_TB_ALT,  false);
      m_dsktWnd.setAltState(false);
      m_dsktWnd.sendKey(VK_LMENU,        false);
    }
  }
}

void ViewerWindow::commandPause()
{
  LRESULT iState = m_toolbar.getState(IDS_TB_PAUSE);
  if (iState) {
    if (iState == TBSTATE_ENABLED) {
      m_toolbar.checkButton(IDS_TB_PAUSE, true);
      m_viewerCore->stopUpdating(true);
    } else {
      m_toolbar.checkButton(IDS_TB_PAUSE, false);
      m_viewerCore->stopUpdating(false);
    }
  }
}

void ViewerWindow::commandToolBar()
{
  if (m_toolbar.isVisible()) {
    m_logWriter->debug("Hide toolbar");
    m_menu.checkedMenuItem(IDS_TB_TOOLBAR, false);
    m_toolbar.hide();
    doSize();
  } else {
    if (!m_isFullScr) {
      m_logWriter->debug("Show toolbar");
      m_menu.checkedMenuItem(IDS_TB_TOOLBAR, true);
      m_toolbar.show();
      doSize();
    }
  }
}

void ViewerWindow::commandNewConnection()
{
  m_application->postMessage(remoting_impact::WM_USER_SHOW_LOGIN_DIALOG);
}

void ViewerWindow::commandSaveSession()
{
   WCHAR fileName[MAX_PATH] = L"";

   ::wstring filterVncFiles(StringTable::getString(IDS_SAVE_SESSION_FILTER_VNC_FILES));
   ::wstring filterAllFiles(StringTable::getString(IDS_SAVE_SESSION_FILTER_ALL_FILES));
   ::wstring vncMask( L"*.vnc");
   ::wstring allMask( L"*.*");
   ::wstring semicolon( L";");
   ::memory wnull(L"\0", sizeof(L"\0"));

   ::memory filter;
   filter.append(filterVncFiles.block_with_null_terminator());
   filter.append(vncMask.block());
   filter.append(semicolon.block());
   filter.append(wnull);

   filter.append( filterAllFiles.block_with_null_terminator());
   filter.append(allMask.block());
   filter.append(wnull);
   filter.append(wnull);


  OPENFILENAME ofn;
  ZeroMemory(&ofn, sizeof(ofn));
  ofn.lStructSize = sizeof(ofn);
  ofn.hwndOwner = m_hwnd;
  ofn.lpstrFilter = (LPCWSTR) filter.data();
  ofn.lpstrDefExt = (LPCWSTR) "vnc";
  ofn.lpstrFile= fileName;
  ofn.nMaxFile = MAX_PATH;
  ofn.Flags = OFN_OVERWRITEPROMPT;
  try {
    if (GetSaveFileName(&ofn)) {
      auto oldSettings = file_item(fileName);
      if (oldSettings->exists()) {
        oldSettings->erase();
      }
      IniFileSettingsManager sm(fileName);
      sm.setApplicationName("connection");

      auto host = m_conData->getReducedHost();
      sm.setString("host", host);
      sm.setUINT("port", m_conData->getPort());

      if (m_conData->isSetPassword()) {
        int whetherToSavePass = ::remoting::message_box(m_hwnd,
          StringTable::getString(IDS_QUESTION_SAVE_PASSWORD),
          StringTable::getString(IDS_SECURITY_WARNING_CAPTION),
          MB_YESNO);
        if (whetherToSavePass == IDYES) {
          ::string password = m_conData->getCryptedPassword();
          sm.setString("password", password);
        }
      }

      sm.setApplicationName("options");
      m_conConf->saveToStorage(&sm);
    }
  } catch (...) {
    m_logWriter->error("Error in save connection");
  }
}

void ViewerWindow::commandScaleIn()
{
  if (m_conConf->isFitWindowEnabled()) {
    commandScaleAuto();
  }

  if (m_standardScale.empty()) {
    _ASSERT(false);
    return;
  }
    
  int scale = m_conConf->getScaleNumerator() * 100 / m_conConf->getScaleDenominator();
  size_t indexNewScale = 0;
  while (indexNewScale < m_standardScale.size() && m_standardScale[indexNewScale] <= scale + 5)
    indexNewScale++;

  if (indexNewScale >= m_standardScale.size())
    indexNewScale = m_standardScale.size() - 1;

  m_conConf->setScale(m_standardScale[indexNewScale], 100);
  m_conConf->fitWindow(false);
  m_conConf->saveToStorage(&m_ccsm);
  applySettings();
}

void ViewerWindow::commandScaleOut()
{
  if (m_conConf->isFitWindowEnabled()) {
    commandScaleAuto();
  }

  if (m_standardScale.empty()) {
    _ASSERT(false);
    return;
  }

  int scale = m_conConf->getScaleNumerator() * 100 / m_conConf->getScaleDenominator();
  size_t indexNewScale = m_standardScale.size();
  do {
    indexNewScale--;
  } while (indexNewScale < m_standardScale.size() && m_standardScale[indexNewScale] >= scale - 5);
  
  if (indexNewScale > m_standardScale.size())
    indexNewScale = 0;

  m_conConf->setScale(m_standardScale[indexNewScale], 100);
  m_conConf->fitWindow(false);
  m_conConf->saveToStorage(&m_ccsm);
  applySettings();
}

void ViewerWindow::commandScale100()
{
  m_toolbar.checkButton(IDS_TB_SCALEAUTO, false);
  m_conConf->setScale(1, 1);
  m_conConf->fitWindow(false);
  m_conConf->saveToStorage(&m_ccsm);
  applySettings();
}

void ViewerWindow::commandScaleAuto()
{
  LRESULT iState = m_toolbar.getState(IDS_TB_SCALEAUTO);
  if (iState) {
    if (iState == TBSTATE_ENABLED) {
      m_toolbar.checkButton(IDS_TB_SCALEAUTO, true);
      m_conConf->fitWindow(true);
    } else {
      m_toolbar.checkButton(IDS_TB_SCALEAUTO, false);

      RECT rcWindow;
      m_dsktWnd.getClientRect(&rcWindow);
      int wndWidth = rcWindow.right - rcWindow.left - 1;
      int wndHeight = rcWindow.bottom - rcWindow.top;

      ::int_rectangle screen = m_dsktWnd.getFrameBufferGeometry();

      if (wndWidth * screen.height() <= wndHeight * screen.width()) {
        m_conConf->setScale(wndWidth, screen.width());
      } else {
        m_conConf->setScale(wndHeight, screen.height());
      }
      m_conConf->fitWindow(false);
    }
    m_conConf->saveToStorage(&m_ccsm);
    applySettings();
  }
}

int ViewerWindow::translateAccelToTB(int val) 
{
  static const ::pair<int, int> accelerators[] = {
    {ID_CONN_OPTIONS,    IDS_TB_CONNOPTIONS},
    {ID_CONN_INFO,       IDS_TB_CONNINFO},
    {ID_SHOW_TOOLBAR,    IDS_TB_TOOLBAR},
    {ID_FULL_SCR,        IDS_TB_FULLSCREEN},
    {ID_REQ_SCR_REFRESH, IDS_TB_REFRESH},
    {ID_CTRL_ALT_DEL,    IDS_TB_CTRLALTDEL},
    {ID_TRANSF_FILES,    IDS_TB_TRANSFER}
  };

  for (int i = 0; i < sizeof(accelerators) / sizeof(::pair<int, int>); i++) {
    if (accelerators[i].m_element1 == val) {
      m_logWriter->debug("accelerator pressed: {}", val);
      return accelerators[i].m_element2;
    }
  }
  return -1;
}

void ViewerWindow::onAbout()
{
  m_application->postMessage(remoting_impact::WM_USER_ABOUT);
}

bool ViewerWindow::onCommand(WPARAM wParam, LPARAM lParam)
{
  if (HIWORD(wParam) == 1) {
    int transl = translateAccelToTB(LOWORD(wParam));

    if (transl != -1) {
      wParam = transl;
    }
  }
  switch(wParam) {
    case IDS_ABOUT_VIEWER:
      onAbout();
      return true;
    case IDS_TB_CONNOPTIONS:
      dialogConnectionOptions();
      return true;
    case IDS_TB_CONNINFO:
      dialogConnectionInfo();
      return true;
    case IDS_TB_PAUSE:
      commandPause();
      return true;
    case IDS_TB_REFRESH:
      m_viewerCore->refreshFrameBuffer();
      return true;
    case IDS_TB_CTRLALTDEL:
      commandCtrlAltDel();
      return true;
    case IDS_TB_CTRLESC:
      commandCtrlEsc();
      return true;
    case IDS_TB_CTRL:
      commandCtrl();
      return true;
    case IDS_TB_ALT:
      commandAlt();
      return true;
    case IDS_TB_TOOLBAR:
      commandToolBar();
      return true;
    case IDS_TB_TRANSFER:
      showFileTransferDialog();
      return true;
    case IDS_TB_NEWCONNECTION:
      commandNewConnection();
      return true;
    case IDS_TB_SAVESESSION:
      commandSaveSession();
      return true;
    case IDS_TB_SCALEIN:
      commandScaleIn();
      return true;
    case IDS_TB_SCALEOUT:
      commandScaleOut();
      return true;
    case IDS_TB_SCALE100:
      commandScale100();
      return true;
    case IDS_TB_SCALEAUTO:
      commandScaleAuto();
      return true;
    case IDS_TB_FULLSCREEN: 
      switchFullScreenMode();
      return true;
    case IDS_TB_CONFIGURATION:
      dialogConfiguration();
      return true;
  }
  return false;
}

void ViewerWindow::showFileTransferDialog()
{
  LRESULT iState = m_toolbar.getState(IDS_TB_TRANSFER);
  if (iState) {
    // FIXME: FT check it
    if (m_ftDialog != 0) {
      if (!m_ftDialog->isCreated()) {
        _ASSERT(m_fileTransfer != 0);
        m_fileTransfer->setInterface(0);
        delete m_ftDialog;
        m_ftDialog = 0;
      }
    }
    if (m_ftDialog == 0) {
      _ASSERT(m_fileTransfer != 0);
      m_ftDialog = new FileTransferMainDialog(m_fileTransfer->getCore());
      m_fileTransfer->setInterface(m_ftDialog);
    }
    m_ftDialog->show();
    HWND dialogWnd = m_ftDialog->get_hwnd();
    m_application->addModelessDialog(dialogWnd);
  }
}

void ViewerWindow::applyScreenChanges(bool isFullScreen)
{
  m_isFullScr = isFullScreen;
  doSize();
  redraw();
}

void ViewerWindow::setSizeFullScreenWindow()
{
  // Save position of window.
  GetWindowPlacement(m_hwnd, &m_workArea);

  // Get size of desktop.
  HMONITOR hmon = MonitorFromWindow(m_hwnd, MONITOR_DEFAULTTONEAREST);
  MONITORINFO mi;
  mi.cbSize = sizeof(mi);

  RECT fullScreenWindowsRect;
  if (!!GetMonitorInfo(hmon, &mi)) {
    fullScreenWindowsRect = mi.rcMonitor;
  } else {
    m_logWriter->warning("Get monitor info is failed. Use second method (no multi-screen).");
    GetWindowRect(GetDesktopWindow(), &fullScreenWindowsRect);
  }
  ::int_rectangle fullScreenRect;
  fullScreenRect = fullScreenWindowsRect;
  m_logWriter->debug("full screen window rect: {}, {}; {}, {}",
                     fullScreenRect.left, fullScreenRect.top,
                     fullScreenRect.width(), fullScreenRect.height());

  set_style((get_style() | WS_MAXIMIZE) & ~(WS_CAPTION | WS_BORDER | WS_THICKFRAME  | WS_MAXIMIZEBOX));
  set_ex_style(get_ex_style() | WS_EX_TOPMOST);

  SetWindowPos(m_hwnd, 0,
               fullScreenRect.left, fullScreenRect.top,
               fullScreenRect.width(), fullScreenRect.height(),
               SWP_SHOWWINDOW);
}

void ViewerWindow::doFullScr()
{
  if (m_isFullScr) {
    return;
  }

  m_conConf->enableFullscreen(true);
  m_conConf->saveToStorage(&m_ccsm);

  auto config = ::remoting::ViewerConfig::getInstance();
  m_bToolBar = m_toolbar.isVisible();
  m_toolbar.hide();

  m_menu.checkedMenuItem(IDS_TB_FULLSCREEN, true);
  m_menu.checkedMenuItem(IDS_TB_TOOLBAR,    false);
  m_menu.enableMenuItem(IDS_TB_TOOLBAR,     1);

  setSizeFullScreenWindow();

  SetFocus(m_dsktWnd.getHWnd());
  applyScreenChanges(true);

  if (config->isPromptOnFullscreenEnabled()) {
    postMessage(WM_USER_FS_WARNING);
  }
  
  try {
    // Registration of keyboard hook.
    m_winHooks.registerKeyboardHook(this);
    // Switching off ignoring win key.
    m_dsktWnd.setWinKeyIgnore(false);
  } catch (::exception &e) {
    m_logWriter->error("{}", e.get_message());
  }
}

void ViewerWindow::doUnFullScr()
{
  if (!m_isFullScr) {
    return;
  }

  m_conConf->enableFullscreen(false);
  m_conConf->saveToStorage(&m_ccsm);

  m_menu.checkedMenuItem(IDS_TB_FULLSCREEN, false);
  m_menu.checkedMenuItem(IDS_TB_TOOLBAR, m_bToolBar);

  if (m_bToolBar) {
    m_toolbar.show();
  } else {
    m_toolbar.hide();
  }

  UINT isEnable = static_cast<UINT>(m_conConf->isViewOnly());
  m_menu.enableMenuItem(IDS_TB_TOOLBAR, isEnable);

  // Restore position, style and exstyle of windowed window.
  set_style(get_style() | WS_CAPTION | WS_BORDER | WS_THICKFRAME | WS_MAXIMIZEBOX);
  set_ex_style(get_ex_style() & ~WS_EX_TOPMOST);
  ::int_rectangle workArea;
  workArea = m_workArea.rcNormalPosition;
  if (m_rcNormal.height() == workArea.height() ||
      m_rcNormal.width() == workArea.width()) {
    SetWindowPlacement(m_hwnd, &m_workArea);
  } else {
    set_style(get_style() & ~WS_MAXIMIZE);
    setPosition(m_rcNormal.left, m_rcNormal.top);
    setSize(m_rcNormal.width(), m_rcNormal.height());
  }

  m_dsktWnd.setScale(m_scale);
  applyScreenChanges(false);
  
  // Unregistration of keyboard hook.
  m_winHooks.unregisterKeyboardHook(this);
  // Switching on ignoring win key.
  m_dsktWnd.setWinKeyIgnore(true);
}

bool ViewerWindow::onNotify(int idCtrl, LPNMHDR pnmh)
{
  ResourceLoader *rLoader = ResourceLoader::getInstance();
  LPTOOLTIPTEXT toolTipText = reinterpret_cast<LPTOOLTIPTEXT>(pnmh);
  if (toolTipText->hdr.code != TTN_NEEDTEXT) {
    return false;
  }
  int resId = static_cast<int>(toolTipText->hdr.idFrom);
   ::string strToolTip;
  rLoader->loadString(resId, strToolTip);
   m_wstrToolTip = strToolTip;
  toolTipText->lpszText = const_cast<TCHAR *>(m_wstrToolTip.c_str());
  return true;
}

bool ViewerWindow::onClose()
{
  if (m_ftDialog != 0 && m_ftDialog->isCreated()) {
    if (!m_ftDialog->tryClose()) {
      return true;
    }
  }
  m_dsktWnd.destroyWindow();
  destroyWindow();
   ::system()->m_papplicationMain->set_finish();
  return true;
}

bool ViewerWindow::onDestroy()
{
  KillTimer(m_hwnd, TIMER_DESKTOP_STATE);
  return true;
}

void ViewerWindow::doSize()
{
  postMessage(WM_SIZE);
}

bool ViewerWindow::onSize(WPARAM wParam, LPARAM lParam) 
{
  RECT rc;
  int x, y;

  getClientRect(&rc);
  m_logWriter->debug("client rect: {}, {}; {}, {}",
                    rc.left, rc.top, rc.right, rc.bottom);
  x = y = 0;
  if (m_toolbar.isVisible()) {
    m_toolbar.autoSize();
    y = m_toolbar.height() - 1;
    rc.bottom -= y;
  }
  if (m_dsktWnd.getHWnd()) {

    int h = rc.bottom - rc.top;
    int w = rc.right - rc.left;

    m_logWriter->debug("Desktop-window. (x, y): ({}, {}); (w, h): ({}, {})",
                      x, y, w, h);
    if (h > 0 && w > 0) {
      m_dsktWnd.setPosition(x, y);
      m_dsktWnd.setSize(w, h);
    }
  } 
  return true;
}

void ViewerWindow::showWindow()
{
  show();

  ::string windowName = formatWindowName();
  setWindowText(windowName);
  m_dsktWnd.setWindowText(windowName);

}

bool ViewerWindow::onDisconnect()
{
  ::remoting::message_box(getHWnd(),
             m_disconnectMessage,
             formatWindowName(),
             MB_OK);

  m_dsktWnd.destroyWindow();
  destroyWindow();
  return true;
}

bool ViewerWindow::onAuthError(WPARAM wParam)
{
  // If authentication is canceled, then do quiet exit, else show error-scopedstrMessage.
  if (wParam != AuthException::AUTH_CANCELED) {
    ::string error = m_error.get_message();
    int result = ::remoting::message_box(0,
                            error,
                            formatWindowName(),
                            MB_RETRYCANCEL | MB_ICONERROR);
    if (result == IDRETRY) {
      if (!m_conData->isIncoming()) {
        // Retry connect to remote host.
        m_requiresReconnect = true;
        ConnectionData *connectionData = new ConnectionData(*m_conData);
        connectionData->resetPassword();
        ConnectionConfig *connectionConfig = new ConnectionConfig(*m_conConf);
        m_application->postMessage(remoting_impact::WM_USER_RECONNECT,
                                   (WPARAM)connectionData,
                                   (LPARAM)connectionConfig);
      }
    }
  }
  m_dsktWnd.destroyWindow();
  destroyWindow();
  return true;
}

bool ViewerWindow::onError()
{
  ::string error;
  error.format("Error in {}: {}", ::string(ProductNames::VIEWER_PRODUCT_NAME), m_error.get_message());
  ::remoting::message_box(getHWnd(),
             error,
             formatWindowName(),
             MB_OK | MB_ICONERROR);

  m_dsktWnd.destroyWindow();
  destroyWindow();
  return true;
}

bool ViewerWindow::onFsWarning()
{
  FsWarningDialog fsWarning;
  fsWarning.set_parent(&m_control);
  fsWarning.showModal();
  return true;
}

bool ViewerWindow::onFocus(WPARAM wParam)
{
  SetFocus(m_dsktWnd.getHWnd());
  return true;
}

void ViewerWindow::onBell()
{
  if (m_conConf->isDeiconifyOnRemoteBellEnabled()) {
    ShowWindow(getHWnd(), SW_RESTORE);
    setForegroundWindow();
  }
  MessageBeep(MB_ICONASTERISK);
}

::int_rectangle ViewerWindow::calculateDefaultSize()
{
  // Get work area.
  ::int_rectangle defaultRect;

  HMONITOR hmon = MonitorFromWindow(m_hwnd, MONITOR_DEFAULTTONEAREST);
  MONITORINFO mi;
  mi.cbSize = sizeof(mi);

  if (!!GetMonitorInfo(hmon, &mi)) {
    defaultRect = mi.rcWork;
  } else {
    m_logWriter->debug("Get monitor info is failed. Use second method (no multi-screen).");
    RECT desktopRc;
    if (!m_sysinf.getDesktopArea(&desktopRc)) {
       m_sysinf.getDesktopAllArea(&desktopRc);
    }
    defaultRect  = desktopRc;
  }

  int widthDesktop  = defaultRect.width();
  int heightDesktop = defaultRect.height();

  ::int_rectangle viewerRect = m_dsktWnd.getViewerGeometry();
  int serverWidth = viewerRect.width();
  int serverHeight = viewerRect.height();

  if (serverWidth < widthDesktop && serverHeight < heightDesktop) {
    int borderWidth, borderHeight;
    getBorderSize(&borderWidth, &borderHeight);
    int totalWidth     = serverWidth  + borderWidth;
    int totalHeight    = serverHeight + borderHeight + 1;
    if (m_toolbar.isVisible()) {
      totalHeight += m_toolbar.height();
    }
    defaultRect.set_height(totalHeight);
    defaultRect.set_width(totalWidth);
    defaultRect.offset((widthDesktop - totalWidth) / 2,
                     (heightDesktop - totalHeight) / 2);
  }
  return defaultRect;
}


void ViewerWindow::onConnecting(int iPhase)
{

   m_papplication->handle_direct_id(id_remoting_connecting, iPhase, 0);

}


void ViewerWindow::onConnected(RfbOutputGate *output)
{
  // Set flags.
  m_isConnected = true;
  m_sizeIsChanged = false;
  m_dsktWnd.setConnected();

   m_papplication->handle_direct_id(id_remoting_connected, 0, 0);

  // Set output for client-to-server messages in file transfer.
  m_fileTransfer->setOutput(output);

  // Update ::list_base of supported operation for file transfer.
  ::array_base<unsigned int> clientMsgCodes;
  m_viewerCore->getEnabledClientMsgCapabilities(&clientMsgCodes);

  ::array_base<unsigned int> serverMsgCodes;
  m_viewerCore->getEnabledServerMsgCapabilities(&serverMsgCodes);

  m_fileTransfer->getCore()->updateSupportedOperations(&clientMsgCodes, &serverMsgCodes);

  // Start viewer window and applying settings.
  showWindow();
  setForegroundWindow();
  applySettings();
}

void ViewerWindow::onDisconnect(const ::scoped_string & scopedstrMessage)
{
  m_logWriter->information("onDisconnect: {}", scopedstrMessage);
  m_disconnectMessage = scopedstrMessage;
  if (!m_stopped) {
    postMessage(WM_USER_DISCONNECT);
  }
}

void ViewerWindow::onAuthError(const AuthException *exception)
{
  m_logWriter->information("onAuthError ({}): {}",
                   exception->getAuthCode(), exception->get_message());
  int authCode = exception->getAuthCode();
  m_error = *exception;
  postMessage(WM_USER_AUTH_ERROR, authCode);
}

void ViewerWindow::onError(const ::remoting::Exception *exception)
{
  m_error = *exception;
  postMessage(WM_USER_ERROR);
}

void ViewerWindow::onFrameBufferUpdate(const FrameBuffer *fb, const ::int_rectangle &  rect)
{
  m_dsktWnd.updateFramebuffer(fb, rect);
}

void ViewerWindow::onFrameBufferPropChange(const FrameBuffer *fb)
{
  m_dsktWnd.setNewFramebuffer(fb);
}

void ViewerWindow::onCutText(const ::scoped_string & cutText)
{
  m_dsktWnd.setClipboardData(cutText);
}

void ViewerWindow::doCommand(int iCommand)
{
  postMessage(WM_COMMAND, iCommand);
}

bool ViewerWindow::requiresReconnect() const
{
  return m_requiresReconnect;
}

bool ViewerWindow::isStopped() const
{
  return m_stopped;
}

void ViewerWindow::adjustWindowSize()
{
  // If size isn't changed by user, then adjust size.
  if (!m_sizeIsChanged) {
    ::int_rectangle defaultSize = calculateDefaultSize();
    bool defaultSizeIsChanged = defaultSize.width() != m_rcNormal.width() ||
                                defaultSize.height() != m_rcNormal.height();
    // If size is changed, isn't full screen, if window isn't maximized,
    // then set new position and size.
    if (!m_isFullScr && defaultSizeIsChanged) {
      m_rcNormal = defaultSize;
      setPosition(m_rcNormal.left, m_rcNormal.top);
      setSize(m_rcNormal.width(), m_rcNormal.height());
    }

    // This is done for keyboard hooks to work.
    // If m_conConf->isFullscreenEnabled() is true,
    // hooks don't work at the first start of the viewer.
    if (m_hooksEnabledFirstTime && m_isFullScr) {
      try {
        // Registration of keyboard hook.
        m_winHooks.registerKeyboardHook(this);
        // Switching off ignoring win key.
        m_dsktWnd.setWinKeyIgnore(false);
        m_hooksEnabledFirstTime = false;
      } catch (::exception &e) {
        m_logWriter->error("{}", e.get_message());
      }
    }
  }
}

void ViewerWindow::updateKeyState()
{
  LRESULT ctrlState = m_toolbar.getState(IDS_TB_CTRL);
  if (ctrlState != 0) {
    m_toolbar.checkButton(IDS_TB_CTRL, m_dsktWnd.getCtrlState());
  }

  LRESULT altState = m_toolbar.getState(IDS_TB_ALT);
  if (altState != 0) {
    m_toolbar.checkButton(IDS_TB_ALT, m_dsktWnd.getAltState());
  }
}

::string ViewerWindow::formatWindowName() const
{
  ::string desktopName = m_viewerCore->getRemoteDesktopName();
  if (desktopName.is_empty() && !m_conData->getHost().is_empty()) {
    desktopName = m_conData->getHost();
  }
  ::string windowName;
  if (!desktopName.is_empty()) {
    windowName.format("{} - {}", desktopName, ::string(ProductNames::VIEWER_PRODUCT_NAME));
  } else {
    windowName.format("{}", ::string(ProductNames::VIEWER_PRODUCT_NAME));
  }
  return windowName;
}

LRESULT ViewerWindow::onHookProc(int code, WPARAM wParam, LPARAM lParam)
{
  KBDLLHOOKSTRUCT *str = (KBDLLHOOKSTRUCT*) lParam;
  // Ignoring of CapsLock, NumLock, ScrollLock, ::remoting::Window (Ctrl key), Menu (Alt key), Shift (shift key).
  if (str->vkCode != VK_CAPITAL && str->vkCode != VK_NUMLOCK && str->vkCode != VK_SCROLL &&
      str->vkCode != VK_LCONTROL && str->vkCode != VK_RCONTROL &&
      str->vkCode != VK_LMENU && str->vkCode != VK_RMENU &&
      str->vkCode != VK_LSHIFT && str->vkCode != VK_RSHIFT) {
    // Set the repeat count for the current scopedstrMessage bits.
    LPARAM newLParam = 1;
    // Set the scan code bits. 
    newLParam |= (str->scanCode & 0xf) << 16;
    // Set the extended key bit.
    newLParam |= (str->flags & LLKHF_EXTENDED) << 24;
    // Set the context code bit.
    newLParam |= ((str->flags & LLKHF_ALTDOWN) > 0) << 29;
    // Set the transition state bit.
    newLParam |= ((str->flags & LLKHF_UP) > 0) << 31;
    if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
      PostMessage(m_dsktWnd.getHWnd(), wParam, str->vkCode, newLParam);
    } else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
      PostMessage(m_dsktWnd.getHWnd(), wParam, str->vkCode, newLParam);
    }
    return true;
  } else {
    return false;
  }
}
