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
#include "acme/parallelization/manual_reset_happening.h"
#include "remoting/remoting/config/IniFileSettingsManager.h"
#include "subsystem/platform/Exception.h"
#include "subsystem/platform/ResourceLoader.h"
#include "subsystem/framebuffer/StandardPixelFormatFactory.h"
#include "remoting/client/keyboard_layout_change.h"
#include "innate_subsystem/platform/ResourceLoader.h"
#include "innate_subsystem/platform/subsystem.h"
#include "innate_subsystem/gui/Toolbar.h"
#include "innate_subsystem/drawing/Cursor.h"
#include "FullscreenWarningDialog.h"
#include "remoting/remoting/client/NamingDefs.h"
#include "remoting/client/remoting.h"
#include "remoting_impact.h"
#include "ViewerWindow.h"
#include "acme/constant/id.h"
#include "acme/constant/user_key.h"
#include "acme/filesystem/file/item.h"
#include "acme/platform/application.h"
#include "apex/networking/http/context.h"
#include "remoting/remoting/platform/remoting.h"
#include "resource.h"
#include "acme/platform/node.h"
//// #include aaa_<commdlg.h>

namespace remoting_client
{


    ViewerWindow::ViewerWindow(subsystem::OperatingSystemApplicationInterface *application,
                              ::remoting_client::remoting *premoting, 
                               ConnectionData * pconnectiondata,
                               ::remoting::ConnectionConfig * pconnectionconfig,
                               ::subsystem::LogWriter * plogwriter) :
       m_ccsm(RegistryPaths::VIEWER_PATH, pconnectiondata->getHost()),
       m_poperatingsystemapplication(application),
       m_premoting(premoting),
      m_plogwriter(plogwriter),
      m_pconnectionconfig(pconnectionconfig),
      m_scale(100),
      //m_isFullScr(false),
      m_ftDialog(0),
      m_pviewercore(0),
      m_fileTransfer(0),
      m_pconnectiondata(pconnectiondata),
      m_isConnected(false),
      m_hooksEnabledFirstTime(true),
      m_requiresReconnect(false),
      m_stopped(false)
    {

       initialize(application);
       
       m_pdesktopwindow = allocateø DesktopWindow(m_plogwriter, pconnectionconfig, this);

          // m_poperatingsystemapplication->initialize_operating_system_application()
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

        //::string windowClass = WindowNames::TVN_WINDOW_CLASS_NAME;
        ::string titleName = WindowNames::TVN_WINDOW_TITLE_NAME;
        ::string subTitleName = WindowNames::TVN_SUB_WINDOW_TITLE_NAME;

        //setClass(windowClass);
        setClass(::innate_subsystem::e_window_class_viewer);
        createWindow(titleName, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);

       m_pdesktopwindow->setClipboardViewerInterest();
        m_pdesktopwindow->setDoubleBuffering(true);
        m_pdesktopwindow->setOnDrawInterest();
        //m_pdesktopwindow->setClass(windowClass);
       m_pdesktopwindow->setClass(::innate_subsystem::e_window_class_viewer);
        m_pdesktopwindow->m_pviewerwindow = this;
        m_pdesktopwindow->createWindow(subTitleName,
                               WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_CHILD,
                               operating_system_window());

        ///SetTimer(m_hwnd, TIMER_DESKTOP_STATE, TIMER_DESKTOP_STATE_DELAY, (TIMERPROC)NULL);
        m_papplication->fork([this]()
        {


           while (task_get_run())
           {

              preempt(50_ms);

              desktopStateUpdate();

           }

        });
    }

    ViewerWindow::~ViewerWindow()
    {
        // Unregistration of keyboard hook.
        m_operatingsystemhook.unregisterKeyboardHook(this);

        if (m_ftDialog != 0) {
            try {
                delete m_ftDialog;
            } catch (...) {
            }
            m_ftDialog = 0;
        }
    }

    void ViewerWindow::setFileTransfer(::remoting_client::file_transfer::FileTransferCapability *ft)
    {
        m_fileTransfer = ft;
    }

    void ViewerWindow::setRemoteViewerCore(::remoting_client::RemoteViewerCore *pCore)
    {
        m_pviewercore = pCore;
        m_pdesktopwindow->setViewerCore(pCore);
        applySettings();
    }
   
::innate_subsystem::ControlInterface * ViewerWindow::getControl()
    {
       
       return this;
       
    }
   

    bool ViewerWindow::onCreate(void * pCreateStruct)
    {
        //getControl()->setWindow((HWND) _HWND());

      auto pcursor = createø<::innate_subsystem::CursorInterface>();
      pcursor->initialize_with_system_cursor(::e_cursor_arrow);
      setClassCursor(pcursor);

       //auto pcursor = createø<::innate_subsystem::Icon>();
      //pcursor->initialize_icon(IDI_APPICON);
      //setClassCursor(pcursor);
      //  //setClassCursor(LoadCursor(NULL, IDC_ARROW));
        loadIcon(IDI_APPICON);
        #ifdef WINDOWS
        m_toolbar.loadToolbarfromRes(IDB_TOOLBAR);
        #else
       m_toolbar.loadToolbarFromMatter("matter://toolbar.png");
       #endif
        m_toolbar.setButtonsRange(IDS_TB_NEWCONNECTION);
        m_toolbar.setViewAutoButtons(4, ::innate_subsystem::TB_Style_sep);
        m_toolbar.setViewAutoButtons(6, ::innate_subsystem::TB_Style_sep);
        m_toolbar.setViewAutoButtons(10, ::innate_subsystem::TB_Style_sep);
        m_toolbar.setViewAutoButtons(11, ::innate_subsystem::TB_Style_sep);
        m_toolbar.setViewAutoButtons(15, ::innate_subsystem::TB_Style_sep);
        m_toolbar.attachToolbar(operating_system_window());
        m_menu.getSystemMenu(this);
        m_menu.loadMenu();
        applySettings();
        m_timeStart.Now();
        auto pviewerconfig = m_premoting->m_pviewerconfig;
        bool bShowToolbar = pviewerconfig->isToolbarShown();
        if (!bShowToolbar) {
            m_toolbar.hide();
            m_bToolBar = false;
        }
        m_menu.checkedMenuItem(IDS_TB_TOOLBAR, bShowToolbar);
        return true;
    }

    void ViewerWindow::enableUserElements()
    {
        bool isEnable = !m_pconnectionconfig->isViewOnly();

        m_toolbar.enableButton(IDS_TB_ALT, isEnable);
        m_toolbar.enableButton(IDS_TB_CTRL, isEnable);
        m_toolbar.enableButton(IDS_TB_CTRLESC, isEnable);
        m_toolbar.enableButton(IDS_TB_CTRLALTDEL, isEnable);

        unsigned int enableMenu = static_cast<unsigned int>(!isEnable);
        m_menu.enableMenuItem(IDS_TB_CTRLALTDEL, enableMenu);
        m_menu.enableMenuItem(IDS_TB_CTRLESC, enableMenu);
        m_menu.enableMenuItem(IDS_TB_CTRL, enableMenu);
        m_menu.enableMenuItem(IDS_TB_ALT, enableMenu);

        if (!isEnable) {
            m_menu.checkedMenuItem(IDS_TB_ALT, false);
            m_menu.checkedMenuItem(IDS_TB_CTRL, false);
        }

        int scale = m_pconnectionconfig->getScaleNumerator() * 100 / m_pconnectionconfig->getScaleDenominator();
        m_toolbar.enableButton(IDS_TB_SCALEOUT, scale > m_standardScale[0]);
        m_toolbar.enableButton(IDS_TB_SCALEIN, scale < m_standardScale[m_standardScale.size() - 1]);
        if (m_pconnectionconfig->isFitWindowEnabled()) {
            m_toolbar.checkButton(IDS_TB_SCALEAUTO, true);
            m_toolbar.enableButton(IDS_TB_SCALE100, true);
        } else {
            m_toolbar.enableButton(IDS_TB_SCALE100, scale != 100);
        }
    }

    bool ViewerWindow::viewerCoreSettings()
    {
        if (!m_pviewercore) {
            return false;
        }

        bool bFileTransfer = m_fileTransfer && m_fileTransfer->isEnabled();

        m_toolbar.enableButton(IDS_TB_TRANSFER, bFileTransfer && !m_pconnectionconfig->isViewOnly());
        unsigned int enableMenu = static_cast<unsigned int>(!(bFileTransfer && !m_pconnectionconfig->isViewOnly()));
        m_menu.enableMenuItem(IDS_TB_TRANSFER, enableMenu);

        m_pviewercore->allowCopyRect(m_pconnectionconfig->isCopyRectAllowed());
        m_pviewercore->setPreferredEncoding(m_pconnectionconfig->getPreferredEncoding());

        m_pviewercore->ignoreCursorShapeUpdates(m_pconnectionconfig->isIgnoringShapeUpdates());
        m_pviewercore->enableCursorShapeUpdates(m_pconnectionconfig->isRequestingShapeUpdates());

        // set -1, if compression is disabled
        m_pviewercore->setCompressionLevel(m_pconnectionconfig->getCustomCompressionLevel());

        // set -1, if jpeg-compression is disabled
        m_pviewercore->setJpegQualityLevel(m_pconnectionconfig->getJpegCompressionLevel());

        if (m_pconnectionconfig->isUsing8BitColor()) {
            m_pviewercore->setPixelFormat(::innate_subsystem::StandardPixelFormatFactory::create8bppPixelFormat());
        } else {
            m_pviewercore->setPixelFormat(::innate_subsystem::StandardPixelFormatFactory::create32bppPixelFormat());
        }
        return true;
    }

    void ViewerWindow::applySettings()
    {
        int scale;

        if (m_pconnectionconfig->isFitWindowEnabled()) {
            scale = -1;
        } else {
            int iNum = m_pconnectionconfig->getScaleNumerator();
            int iDenom = m_pconnectionconfig->getScaleDenominator();
            scale = (iNum * 100) / iDenom;
        }

        if (scale != m_scale) {
            m_scale = scale;
            m_pdesktopwindow->setScale(m_scale);
            doSize();
        }
        if (m_isConnected) {
            if (m_pconnectionconfig->isFullscreenEnabled()) {
                doFullScreen();
            } else {
                doUnFullScreen();
            }
        }
        changeCursor(m_pconnectionconfig->getLocalCursorShape());
        enableUserElements();
        viewerCoreSettings();
    }

    void ViewerWindow::changeCursor(int type)
    {
        ::pointer < ::innate_subsystem::CursorInterface > pcursor;

        auto presourceloader = InnateSubsystem().ResourceLoader();
        switch (type) {
           case ::remoting::ConnectionConfig::DOT_CURSOR:
                pcursor = presourceloader->loadCursor(IDI_CDOT);
                break;
            case ::remoting::ConnectionConfig::SMALL_CURSOR:
                pcursor = presourceloader->loadCursor(IDI_CSMALLDOT);
                break;
            case ::remoting::ConnectionConfig::NO_CURSOR:
                pcursor = presourceloader->loadCursor(IDI_CNOCURSOR);
                break;
            case ::remoting::ConnectionConfig::NORMAL_CURSOR:
                pcursor = presourceloader->loadStandardCursor(e_cursor_arrow);
                break;
        }
        setClassCursor(pcursor);
    }

    bool ViewerWindow::onSysCommand(::wparam wParam, ::lparam lParam)
    {

        if ((wParam & 0xfff0) == SC_RESTORE)
        {

           restoreWindow();

            return true;
        }
        return false;

    }

    bool ViewerWindow::onMessage(unsigned int message, ::wparam wParam, ::lparam lParam)
    {
        switch (message) {
            case WM_NCDESTROY:
                m_stopped = true;
                return true;
            case WM_USER_STOP:
                SendMessage((HWND) _HWND(), WM_DESTROY, 0, 0);
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
            case WM_CREATE:;
               return true;
               /*           ;
               return onCreate((void *)lParam);*/
            //case WM_SIZE:;
            //   ;
            //   onSize(wParam, lParam);

            case WM_USER_AUTH_ERROR:
                return onAuthError(wParam);
            case WM_USER_ERROR:
                return onError();
            case WM_USER_DISCONNECT:
                return onDisconnect();
            case WM_SYSCOMMAND:
            {

                //if ((wParam & 0xfff0) == SC_RESTORE)
                //{

                //    if (m_isMinimizedFromFullScreen)
                //    {

                //        doRestoreToFullScreen();
                //
                //    }

                //}

            }

                return false;
            case WM_ACTIVATE:
                if (!isFullScreen()) {
                    return true;
                }
                if (LOWORD(wParam) == WA_ACTIVE || LOWORD(wParam) == WA_CLICKACTIVE) {
                    // full screen viewer can be minimized from other screen
                    if (isIconic()) {
                        return true;
                    }
                    try {
                        // Registration of keyboard hook.
                        m_operatingsystemhook.registerKeyboardHook(this);
                        // Switching off ignoring win key.
                        m_pdesktopwindow->setWinKeyIgnore(false);
                    } catch (::exception &e) {
                        m_plogwriter->error("{}", e.get_message());
                    }
                } else if (LOWORD(wParam) == WA_INACTIVE) {
                    // Unregistration of keyboard hook.
                    m_operatingsystemhook.unregisterKeyboardHook(this);
                    //// Switching on ignoring win key.
                    m_pdesktopwindow->setWinKeyIgnore(true);
                }
                return true;
            case WM_SETFOCUS:
                return onFocus(wParam);
            case WM_ERASEBKGND:
                //return onEraseBackground((HDC)wParam);
              return true;
            case WM_KILLFOCUS:
                return onKillFocus(wParam);
            // case WM_TIMER:
            //     return onTimer(wParam);
            case WM_DISPLAYCHANGE:
                adjustWindowSize();

        }
        return false;
    }

    // bool ViewerWindow::onEraseBackground(HDC hdc)
    // {
    //     return true;
    // }

    bool ViewerWindow::onKillFocus(::wparam wParam)
    {
        if (!m_pconnectionconfig->isViewOnly()) {
            m_toolbar.checkButton(IDS_TB_ALT, false);
            m_toolbar.checkButton(IDS_TB_CTRL, false);
        }
        return true;
    }

    // bool ViewerWindow::onTimer(::wparam idTimer)
    // {
    //     switch (idTimer) {
    //         case TIMER_DESKTOP_STATE:
    //             desktopStateUpdate();
    //             return true;
    //         default:
    //             ASSERT(false);
    //             return false;
    //     }
    // }

    void ViewerWindow::dialogConnectionOptions()
    {
        OptionsDialog dialog;

        dialog.setConnected();
        dialog.setConnectionConfig(m_pconnectionconfig);
        // FIXME: Removed ::innate_subsystem::Control from this code and another
        //auto pcontrol = øcreate_new<::innate_subsystem::Control>();
        //pcontrol ->_setWindow(getHWnd());
        dialog.setParent(this);

        if (dialog.showModal() == 1) {
            m_pconnectionconfig->saveToStorage(&m_ccsm);
            applySettings();
        }
    }


   void ViewerWindow::dialogConnectionInfo()
   {
        
      ::string host = m_pconnectiondata->getHost();

      wstring kbdName;
      auto pwsz= kbdName.get_buffer(KL_NAMELENGTH);
      //memset(&kbdName[0], 0, sizeof(TCHAR) * KL_NAMELENGTH);
      if (!GetKeyboardLayoutNameW( pwsz ))
      {
         pwsz[0] = '?';
         pwsz[1] = '?';
         pwsz[2] = '?';
      }
       kbdName.release_buffer();

        ::int_rectangle geometry;
        int pixelSize = 0;
        m_pdesktopwindow->getServerGeometry(&geometry, &pixelSize);
        ::string str;
        str.runtime_format(MainSubsystem().StringTable().getString(IDS_CONNECTION_INFO_FORMAT),
                   host.c_str(),
                   m_pviewercore->getRemoteDesktopName().c_str(),
                   m_pviewercore->getProtocolString().c_str(),
                   geometry.width(),
                   geometry.height(),
                   pixelSize,
                   ::string(kbdName).c_str());
        MainSubsystem().message_box(operating_system_window(),
                   str,
                   MainSubsystem().StringTable().getString(IDS_CONNECTION_INFO_CAPTION),
                   ::user::e_message_box_ok | ::user::e_message_box_icon_information);
    }

    void ViewerWindow::switchFullScreenMode()
    {
        if (isFullScreen()) {
            m_plogwriter->debug("Switch to windowed mode");
            doUnFullScreen();
        } else {
            m_plogwriter->debug("Switch to full screen mode");
            doFullScreen();
        }
    }

    void ViewerWindow::dialogConfiguration()
    {
        m_poperatingsystemapplication->postMessage(remoting_impact::_WM_USER_CONFIGURATION);
    }

    void ViewerWindow::onGoodCursor()
    {

        m_pdesktopwindow->m_timeStartDesktopWindow.m_iSecond -= 120;

    }
    void ViewerWindow::desktopStateUpdate()
    {
        if (isWindow() && !isIconic())
        {
            // Adjust window of viewer to size of remote desktop.
            adjustWindowSize();

            // Update state of toolbar-buttons (Ctrl, Alt) to hardware-button state.
            updateKeyState();

        }
    }

    void ViewerWindow::commandCtrlAltDel()
    {
        LRESULT iState = m_toolbar.getState(IDS_TB_CTRLALTDEL);
        if (iState) {
            m_pdesktopwindow->sendCtrlAltDel();
        }
    }

    void ViewerWindow::commandCtrlEsc()
    {
        LRESULT iState = m_toolbar.getState(IDS_TB_CTRLESC);
        if (iState) {
            m_pdesktopwindow->sendKey(VK_LCONTROL, true);
            m_pdesktopwindow->sendKey(VK_ESCAPE,   true);
            m_pdesktopwindow->sendKey(VK_ESCAPE,   false);
            m_pdesktopwindow->sendKey(VK_LCONTROL, false);
        }
    }

    void ViewerWindow::commandCtrl()
    {
        LRESULT iState = m_toolbar.getState(IDS_TB_CTRL);
        if (iState) {
            if (iState & ::innate_subsystem::e_toolbar_item_state_enabled) {
                m_menu.checkedMenuItem(IDS_TB_CTRL, true);
                m_toolbar.checkButton(IDS_TB_CTRL,  true);
                m_pdesktopwindow->setCtrlState(true);
                m_pdesktopwindow->sendKey(VK_LCONTROL,      true);
            } else {
                m_menu.checkedMenuItem(IDS_TB_CTRL, false);
                m_toolbar.checkButton(IDS_TB_CTRL,  false);
                m_pdesktopwindow->setCtrlState(false);
                m_pdesktopwindow->sendKey(VK_LCONTROL,      false);
            }
        }
    }

    void ViewerWindow::commandAlt()
    {
        LRESULT iState = m_toolbar.getState(IDS_TB_ALT);
        if (iState) {
            if (iState & ::innate_subsystem::e_toolbar_item_state_enabled) {
                m_menu.checkedMenuItem(IDS_TB_ALT, true);
                m_toolbar.checkButton(IDS_TB_ALT,  true);
                m_pdesktopwindow->setAltState(true);
                m_pdesktopwindow->sendKey(VK_LMENU,        true);
            } else {
                m_menu.checkedMenuItem(IDS_TB_ALT, false);
                m_toolbar.checkButton(IDS_TB_ALT,  false);
                m_pdesktopwindow->setAltState(false);
                m_pdesktopwindow->sendKey(VK_LMENU,        false);
            }
        }
    }

    void ViewerWindow::commandPause()
    {
        LRESULT iState = m_toolbar.getState(IDS_TB_PAUSE);
        if (iState) {
            if (iState & ::innate_subsystem::e_toolbar_item_state_enabled) {
                m_toolbar.checkButton(IDS_TB_PAUSE, true);
                m_pviewercore->stopUpdating(true);
            } else {
                m_toolbar.checkButton(IDS_TB_PAUSE, false);
                m_pviewercore->stopUpdating(false);
            }
        }
    }

    void ViewerWindow::commandToolBar()
    {
        if (m_toolbar.isVisible()) {
            m_plogwriter->debug("Hide toolbar");
            m_menu.checkedMenuItem(IDS_TB_TOOLBAR, false);
            m_toolbar.hide();
            doSize();
        } else {
            if (!isFullScreen()) {
                m_plogwriter->debug("Show toolbar");
                m_menu.checkedMenuItem(IDS_TB_TOOLBAR, true);
                m_toolbar.show();
                doSize();
            }
        }
    }

    void ViewerWindow::commandNewConnection()
    {
        m_poperatingsystemapplication->postMessage(remoting_impact::_WM_USER_SHOW_LOGIN_DIALOG);
    }

    void ViewerWindow::commandSaveSession()
    {
        WCHAR fileName[MAX_PATH] = L"";

        ::wstring filterVncFiles(MainSubsystem().StringTable().getString(IDS_SAVE_SESSION_FILTER_VNC_FILES));
        ::wstring filterAllFiles(MainSubsystem().StringTable().getString(IDS_SAVE_SESSION_FILTER_ALL_FILES));
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


       ::file::path path;
       node()->browse_for_file(path);
        // OPENFILENAME ofn;
        // ZeroMemory(&ofn, sizeof(ofn));
        // ofn.lStructSize = sizeof(ofn);
        // ofn.hwndOwner = m_hwnd;
        // ofn.lpstrFilter = (LPCWSTR) filter.data();
        // ofn.lpstrDefExt = (LPCWSTR) "vnc";
        // ofn.lpstrFile= fileName;
        // ofn.nMaxFile = MAX_PATH;
        // ofn.Flags = OFN_OVERWRITEPROMPT;
        // try {

       if (path.has_character())
       {
          try
          {
             //    if (GetSaveFileName(&ofn)) {
             auto oldSettings = file_item(fileName);
             if (oldSettings->exists()) {
                oldSettings->erase();
             }
             ::remoting::IniFileSettingsManager sm(fileName);
             sm.setApplicationName("connection");

             auto host = m_pconnectiondata->getReducedHost();
             sm.setString("host", host);
             sm.setUINT("port", m_pconnectiondata->getPort());

             if (m_pconnectiondata->isSetPassword()) {
                int whetherToSavePass = MainSubsystem().message_box(operating_system_window(),
                  MainSubsystem().StringTable().getString(IDS_QUESTION_SAVE_PASSWORD),
                  MainSubsystem().StringTable().getString(IDS_SECURITY_WARNING_CAPTION),
                  ::user::e_message_box_yes_no);
                if (whetherToSavePass == ::innate_subsystem::e_control_id_yes) {
                   ::string password = m_pconnectiondata->getCryptedPassword();
                   sm.setString("password", password);
                }
             }

             sm.setApplicationName("options");
             m_pconnectionconfig->saveToStorage(&sm);
          } catch (...) {
             m_plogwriter->error("Error in save connection");
          }
       }
    }

    void ViewerWindow::commandScaleIn()
    {
        if (m_pconnectionconfig->isFitWindowEnabled()) {
            commandScaleAuto();
        }

        if (m_standardScale.empty()) {
            ASSERT(false);
            return;
        }

        int scale = m_pconnectionconfig->getScaleNumerator() * 100 / m_pconnectionconfig->getScaleDenominator();
        size_t indexNewScale = 0;
        while (indexNewScale < m_standardScale.size() && m_standardScale[indexNewScale] <= scale + 5)
            indexNewScale++;

        if (indexNewScale >= m_standardScale.size())
            indexNewScale = m_standardScale.size() - 1;

        m_pconnectionconfig->setScale(m_standardScale[indexNewScale], 100);
        m_pconnectionconfig->fitWindow(false);
        m_pconnectionconfig->saveToStorage(&m_ccsm);
        applySettings();
    }

    void ViewerWindow::commandScaleOut()
    {
        if (m_pconnectionconfig->isFitWindowEnabled()) {
            commandScaleAuto();
        }

        if (m_standardScale.empty()) {
            ASSERT(false);
            return;
        }

        int scale = m_pconnectionconfig->getScaleNumerator() * 100 / m_pconnectionconfig->getScaleDenominator();
        size_t indexNewScale = m_standardScale.size();
        do {
            indexNewScale--;
        } while (indexNewScale < m_standardScale.size() && m_standardScale[indexNewScale] >= scale - 5);

        if (indexNewScale > m_standardScale.size())
            indexNewScale = 0;

        m_pconnectionconfig->setScale(m_standardScale[indexNewScale], 100);
        m_pconnectionconfig->fitWindow(false);
        m_pconnectionconfig->saveToStorage(&m_ccsm);
        applySettings();
    }

    void ViewerWindow::commandScale100()
    {
        m_toolbar.checkButton(IDS_TB_SCALEAUTO, false);
        m_pconnectionconfig->setScale(1, 1);
        m_pconnectionconfig->fitWindow(false);
        m_pconnectionconfig->saveToStorage(&m_ccsm);
        applySettings();
    }

    void ViewerWindow::commandScaleAuto()
    {
        LRESULT iState = m_toolbar.getState(IDS_TB_SCALEAUTO);
        if (iState) {
            if (iState & ::innate_subsystem::e_toolbar_item_state_enabled) {
                m_toolbar.checkButton(IDS_TB_SCALEAUTO, true);
                m_pconnectionconfig->fitWindow(true);
            } else {
                m_toolbar.checkButton(IDS_TB_SCALEAUTO, false);

                auto rcWindow = m_pdesktopwindow->getClientRect();
                int wndWidth = rcWindow.width() - 1;
                int wndHeight = rcWindow.height();

                ::int_rectangle screen = m_pdesktopwindow->getFramebufferGeometry();

                if (wndWidth * screen.height() <= wndHeight * screen.width()) {
                    m_pconnectionconfig->setScale(wndWidth, screen.width());
                } else {
                    m_pconnectionconfig->setScale(wndHeight, screen.height());
                }
                m_pconnectionconfig->fitWindow(false);
            }
            m_pconnectionconfig->saveToStorage(&m_ccsm);
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
                m_plogwriter->debug("accelerator pressed: {}", val);
                return accelerators[i].m_element2;
            }
        }
        return -1;
    }

    void ViewerWindow::onAbout()
    {
        m_poperatingsystemapplication->postMessage(remoting_impact::_WM_USER_ABOUT);
    }

    bool ViewerWindow::onCommand(unsigned int controlID, unsigned int notificationID)
    {



        if (notificationID == 1) {
            int transl = translateAccelToTB(controlID);

            if (transl != -1) {
                controlID = transl;
            }
        }
        switch(controlID) {
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
                m_pviewercore->refreshFramebuffer();
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
                    ASSERT(m_fileTransfer != 0);
                    m_fileTransfer->setInterface(0);
                    delete m_ftDialog;
                    m_ftDialog = 0;
                }
            }
            if (m_ftDialog == 0) {
                ASSERT(m_fileTransfer != 0);
                m_ftDialog = new FileTransferMainDialog(m_fileTransfer->getCore());
                m_fileTransfer->setInterface(m_ftDialog);
            }
            m_ftDialog->show();
            auto dialogWnd = m_ftDialog->operating_system_window();
            m_poperatingsystemapplication->addModelessDialog(dialogWnd);
        }
    }

    // void ViewerWindow::applyScreenChanges(bool isFullScreen)
    // {
    //     m_isFullScr = isFullScreen;
    //     doSize();
    //     redraw();
    // }

    // ::int_rectangle ViewerWindow::getFullScreenRect()
    // {
    //
    //
    //     // Get size of desktop.
    //     HMONITOR hmon = MonitorFromWindow(m_hwnd, MONITOR_DEFAULTTONEAREST);
    //     MONITORINFO mi;
    //     mi.cbSize = sizeof(mi);
    //
    //     RECT fullScreenWindowsRect;
    //     if (!!GetMonitorInfo(hmon, &mi)) {
    //         fullScreenWindowsRect = mi.rcMonitor;
    //     }
    //     else {
    //         m_plogwriter->warning("Get monitor info is failed. Use second method (no multi-screen).");
    //         GetWindowRect(GetDesktopWindow(), &fullScreenWindowsRect);
    //     }
    //     ::int_rectangle fullScreenRect;
    //     fullScreenRect = fullScreenWindowsRect;
    //
    //
    //     return fullScreenRect;
    // }

    // void ViewerWindow::setSizeFullScreenWindow()
    // {
    //     // Save pointPosition of window.
    //     GetWindowPlacement(m_hwnd, &m_workArea);
    //
    //     auto fullScreenRect = getFullScreenRect();
    //
    //     set_style((get_style() | WS_MAXIMIZE) & ~(WS_CAPTION | WS_BORDER | WS_THICKFRAME  | WS_MAXIMIZEBOX));
    //     set_ex_style(get_ex_style() | WS_EX_TOPMOST);
    //
    //     SetWindowPos(m_hwnd, 0,
    //                  fullScreenRect.left, fullScreenRect.top,
    //                  fullScreenRect.width(), fullScreenRect.height(),
    //                  SWP_SHOWWINDOW);
    // }
    //


   void ViewerWindow::onBeforeFullScreen(bool bRestore)
    {

       m_pconnectionconfig->enableFullscreen(true);
       m_pconnectionconfig->saveToStorage(&m_ccsm);

       if (!bRestore)
       {
          m_bToolBar = m_toolbar.isVisible();
          m_toolbar.hide();

          m_menu.checkedMenuItem(IDS_TB_FULLSCREEN, true);
          m_menu.checkedMenuItem(IDS_TB_TOOLBAR,    false);
          m_menu.enableMenuItem(IDS_TB_TOOLBAR,     1);
       }

    }


   void ViewerWindow::onAfterFullScreen(bool bRestore)
    {

       if (!bRestore)
       {
          auto pviewerconfig = m_premoting->m_pviewerconfig;

          if (pviewerconfig->isPromptOnFullscreenEnabled()) {
             postMessage(WM_USER_FS_WARNING);
          }
       }
       //SetFocus(m_pdesktopwindow->getHWnd());
       m_pdesktopwindow->setFocus();


       try {
          // Registration of keyboard hook.
          m_operatingsystemhook.registerKeyboardHook(this);
          // Switching off ignoring win key.
          m_pdesktopwindow->setWinKeyIgnore(false);
       } catch (::exception &e) {
          m_plogwriter->error("{}", e.get_message());
       }
    }


   void ViewerWindow::onBeforeUnFullScreen(bool bMinimizing)
   {

       m_pconnectionconfig->enableFullscreen(false);
       m_pconnectionconfig->saveToStorage(&m_ccsm);

       if (!bMinimizing)
       {
          m_menu.checkedMenuItem(IDS_TB_FULLSCREEN, false);
          m_menu.checkedMenuItem(IDS_TB_TOOLBAR, m_bToolBar);

          if (m_bToolBar) {
             m_toolbar.show();
          } else {
             m_toolbar.hide();
          }

          unsigned int isEnable = static_cast<unsigned int>(m_pconnectionconfig->isViewOnly());
          m_menu.enableMenuItem(IDS_TB_TOOLBAR, isEnable);
       }


   }


   void ViewerWindow::onAfterUnFullScreen(bool bMinimizing)
   {

       if (!bMinimizing)
       {
          m_pdesktopwindow->setScale(m_scale);
       }

       // Unregistration of keyboard hook.
       m_operatingsystemhook.unregisterKeyboardHook(this);
       // Switching on ignoring win key.
       m_pdesktopwindow->setWinKeyIgnore(true);

   }


   bool ViewerWindow::onGetTooltip(int iControl, ::string & strTooltip)
   {

       auto presourceloader = MainSubsystem().ResourceLoader();
       int resId = static_cast<int>(iControl);
       //::string strToolTip;
       presourceloader->loadString(resId, strTooltip);

       return true;

   }


   // void ViewerWindow::doFullScr()
    // {
    //     if (m_isFullScr) {
    //         return;
    //     }
    //
    //     m_pconnectionconfig->enableFullscreen(true);
    //     m_pconnectionconfig->saveToStorage(&m_ccsm);
    //
    //     auto config = m_premoting->m_pviewerconfig;
    //     m_bToolBar = m_toolbar.isVisible();
    //     m_toolbar.hide();
    //
    //     m_menu.checkedMenuItem(IDS_TB_FULLSCREEN, true);
    //     m_menu.checkedMenuItem(IDS_TB_TOOLBAR,    false);
    //     m_menu.enableMenuItem(IDS_TB_TOOLBAR,     1);
    //
    //     setSizeFullScreenWindow();
    //
    //     applyScreenChanges(true);
    //
    //    //SetFocus(m_pdesktopwindow->getHWnd());
    //    m_pdesktopwindow->setFocus();
    //
    //     if (config->isPromptOnFullscreenEnabled()) {
    //         postMessage(WM_USER_FS_WARNING);
    //     }
    //
    //     try {
    //         // Registration of keyboard hook.
    //         m_operatingsystemhook.registerKeyboardHook(this);
    //         // Switching off ignoring win key.
    //         m_pdesktopwindow->setWinKeyIgnore(false);
    //     } catch (::exception &e) {
    //         m_plogwriter->error("{}", e.get_message());
    //     }
    // }

    // void ViewerWindow::doRestoreToFullScreen()
    // {
    //     if (m_isFullScr) {
    //         return;
    //     }
    //
    //     m_pconnectionconfig->enableFullscreen(true);
    //     m_pconnectionconfig->saveToStorage(&m_ccsm);
    //
    //     //auto config = m_premoting->m_pviewerconfig;
    //     //m_bToolBar = m_toolbar.isVisible();
    //     //m_toolbar.hide();
    //
    //     //m_menu.checkedMenuItem(IDS_TB_FULLSCREEN, true);
    //     //m_menu.checkedMenuItem(IDS_TB_TOOLBAR, false);
    //     //m_menu.enableMenuItem(IDS_TB_TOOLBAR, 1);
    //
    //     setSizeFullScreenWindow();
    //
    //     //SetFocus(m_pdesktopwindow->getHWnd());
    //    m_pdesktopwindow->setFocus();
    //     applyScreenChanges(true);
    //
    //
    //     try {
    //         // Registration of keyboard hook.
    //         m_operatingsystemhook.registerKeyboardHook(this);
    //         // Switching off ignoring win key.
    //         m_pdesktopwindow->setWinKeyIgnore(false);
    //     }
    //     catch (::exception& e) {
    //         m_plogwriter->error("{}", e.get_message());
    //     }
    // }

    // void ViewerWindow::doUnFullScr()
    // {
    //     if (!m_isFullScr) {
    //         return;
    //     }
    //
    //     m_pconnectionconfig->enableFullscreen(false);
    //     m_pconnectionconfig->saveToStorage(&m_ccsm);
    //
    //     m_menu.checkedMenuItem(IDS_TB_FULLSCREEN, false);
    //     m_menu.checkedMenuItem(IDS_TB_TOOLBAR, m_bToolBar);
    //
    //     if (m_bToolBar) {
    //         m_toolbar.show();
    //     } else {
    //         m_toolbar.hide();
    //     }
    //
    //     unsigned int isEnable = static_cast<unsigned int>(m_pconnectionconfig->isViewOnly());
    //     m_menu.enableMenuItem(IDS_TB_TOOLBAR, isEnable);
    //
    //    doRestoreFromFullScreen();
    //
    //     m_pdesktopwindow->setScale(m_scale);
    //     applyScreenChanges(false);
    //
    //     // Unregistration of keyboard hook.
    //     m_winHooks.unregisterKeyboardHook(this);
    //     // Switching on ignoring win key.
    //     m_pdesktopwindow->setWinKeyIgnore(true);
    // }
    //
    // void ViewerWindow::doMinimizeFromFullScreen()
    // {
    //     if (!m_isFullScr) {
    //         return;
    //     }
    //
    //     m_pconnectionconfig->enableFullscreen(false);
    //     m_pconnectionconfig->saveToStorage(&m_ccsm);
    //
    //     //m_menu.checkedMenuItem(IDS_TB_FULLSCREEN, false);
    //     //m_menu.checkedMenuItem(IDS_TB_TOOLBAR, m_bToolBar);
    //
    //     //if (m_bToolBar) {
    //     //    m_toolbar.show();
    //     //}
    //     //else {
    //     //    m_toolbar.hide();
    //     //}
    //
    //     //unsigned int isEnable = static_cast<unsigned int>(m_pconnectionconfig->isViewOnly());
    //     //m_menu.enableMenuItem(IDS_TB_TOOLBAR, isEnable);
    //
    //     //// Restore pointPosition, style and exstyle of windowed window.
    //     //set_style(get_style() | WS_CAPTION | WS_BORDER | WS_THICKFRAME | WS_MAXIMIZEBOX);
    //     set_ex_style(get_ex_style() & ~WS_EX_TOPMOST);
    //     //::int_rectangle workArea;
    //     //workArea = m_workArea.rcNormalPosition;
    //     //if (m_rcNormal.height() == workArea.height() ||
    //     //    m_rcNormal.width() == workArea.width()) {
    //     //    SetWindowPlacement(m_hwnd, &m_workArea);
    //     //}
    //     //else {
    //     //    set_style(get_style() & ~WS_MAXIMIZE);
    //     //    setPosition(m_rcNormal.left, m_rcNormal.top);
    //     //    setSize(m_rcNormal.width(), m_rcNormal.height());
    //     //}
    //
    //     //    m_pdesktopwindow->setScale(m_scale);
    //     applyScreenChanges(false);
    //
    //     // Unregistration of keyboard hook.
    //     m_winHooks.unregisterKeyboardHook(this);
    //     // Switching on ignoring win key.
    //     m_pdesktopwindow->setWinKeyIgnore(true);
    // }

    // bool ViewerWindow::onNotify(int idCtrl, LPNMHDR pnmh)
    // {
    //     return true;
    // }

    bool ViewerWindow::onClose()
    {
        if (m_ftDialog != 0 && m_ftDialog->isCreated()) {
            if (!m_ftDialog->tryClose()) {
                return true;
            }
        }
        m_pdesktopwindow->destroyWindow();
        destroyWindow();
        ::system()->m_papplicationMain->set_finish();
        return true;
    }

    bool ViewerWindow::onDestroy()
    {
        //KillTimer(m_hwnd, TIMER_DESKTOP_STATE);
        return true;
    }

    void ViewerWindow::doSize()
    {
        postMessage(WM_SIZE);
    }

    void ViewerWindow::onSize()
    {
        //RECT rc;
        int x, y;

        auto rc = getClientRect();
        m_plogwriter->debug("client rectangle: {}, {}; {}, {}",
                          rc.left, rc.top, rc.right, rc.bottom);
        x = y = 0;
        if (m_toolbar.isVisible()) {
            m_toolbar.autoSize();
            y = m_toolbar.getHeight() - 1;
            rc.bottom -= y;
        }
        if (m_pdesktopwindow->isWindow()) {

            int h = rc.bottom - rc.top;
            int w = rc.right - rc.left;

            m_plogwriter->debug("Desktop-window. (x, y): ({}, {}); (w, h): ({}, {})",
                              x, y, w, h);
            if (h > 0 && w > 0) {
                m_pdesktopwindow->setPlacement(::int_rectangle_dimension(x, y, w, h));
                //m_pdesktopwindow->setSize(w, h);
            }
        }
        //return true;
    }

    void ViewerWindow::showWindow()
    {
        show();

        ::string windowName = formatWindowName();
        setWindowText(windowName);
        m_pdesktopwindow->m_strHost = m_pconnectiondata->getHost();
        m_pdesktopwindow->setWindowText(windowName);

    }

    bool ViewerWindow::onDisconnect()
    {
        MainSubsystem().message_box(operating_system_window(),
                   m_disconnectMessage,
                   formatWindowName(),
                   ::user::e_message_box_ok);

        m_pdesktopwindow->destroyWindow();
        destroyWindow();
        return true;
    }

    bool ViewerWindow::onAuthError(::wparam wParam)
    {
        // If authentication is canceled, then do quiet exit, else show error-scopedstrMessage.
        if (wParam != ::remoting_client::AuthException::AUTH_CANCELED) {
            ::string error = m_error.get_message();
            int result = MainSubsystem().message_box({},
                                    error,
                                    formatWindowName(),
                                    MB_RETRYCANCEL | ::user::e_message_box_icon_error);
            if (result == IDRETRY) {
                if (!m_pconnectiondata->isIncoming()) {
                    // Retry connect to remote host.
                    m_requiresReconnect = true;
                    auto pconnectionData = allocateø ConnectionData(*m_pconnectiondata);
                    pconnectionData->resetPassword();
                    auto pconnectionconfig = allocateø ::remoting::ConnectionConfig(*m_pconnectionconfig);
                    
                    m_poperatingsystemapplication->postMessage(remoting_impact::WM_USER_RECONNECT,
                                               pconnectionData,
                                               pconnectionconfig);
                }
            }
        }
        m_pdesktopwindow->destroyWindow();
        destroyWindow();
        return true;
    }

    bool ViewerWindow::onError()
    {
        ::string error;
        error.format("Error in {}: {}", ::string(ProductNames::VIEWER_PRODUCT_NAME), m_error.get_message());
        MainSubsystem().message_box(operating_system_window(),
                   error,
                   formatWindowName(),
                   ::user::e_message_box_ok | ::user::e_message_box_icon_error);

        m_pdesktopwindow->destroyWindow();
        destroyWindow();
        return true;
    }

    bool ViewerWindow::onFsWarning()
    {
        FullscreenWarningDialog fsWarning(m_premoting);
        fsWarning.setParent(this);
        fsWarning.showModal();
        return true;
    }

    bool ViewerWindow::onFocus(::wparam wParam)
    {
        m_pdesktopwindow->setFocus();
        return true;
    }

    void ViewerWindow::onBell()
    {
        if (m_pconnectionconfig->isDeiconifyOnRemoteBellEnabled()) {
            //ShowWindow(getHWnd(), SW_RESTORE);
           show();
            setForegroundWindow();
        }
        MessageBeep(MB_ICONASTERISK);
    }


   bool ViewerWindow::onCalculateDefaultSize(::int_rectangle & rectangle)
    {

       auto & defaultRect = rectangle;

       defaultRect = getScreenWorkArea();

        int widthDesktop  = defaultRect.width();
        int heightDesktop = defaultRect.height();

        ::int_rectangle viewerRect = m_pdesktopwindow->getViewerGeometry();
        int serverWidth = viewerRect.width();
        int serverHeight = viewerRect.height();

        if (serverWidth < widthDesktop && serverHeight < heightDesktop) {
            //int borderWidth, borderHeight;
            auto sizeBorder = getBorderSize();
            int totalWidth     = serverWidth  + sizeBorder.cx;
            int totalHeight    = serverHeight + sizeBorder.cy + 1;
            if (m_toolbar.isVisible()) {
                totalHeight += m_toolbar.getHeight();
            }
            defaultRect.set_height(totalHeight);
            defaultRect.set_width(totalWidth);
            defaultRect.offset((widthDesktop - totalWidth) / 2,
                             (heightDesktop - totalHeight) / 2);
        }
        //return defaultRect;

       return true;

    }


    void ViewerWindow::onConnecting(int iPhase)
    {

        m_papplication->handle_direct_id(id_remoting_connecting, iPhase, 0);

    }


    void ViewerWindow::onConnected(::remoting::RfbOutputGate *output)
    {
        // Set flags.
        m_isConnected = true;
        //m_sizeIsChanged = false;
        m_pdesktopwindow->setConnected();

        m_papplication->handle_direct_id(id_remoting_connected, 0, 0);

        // Set output for client-to-server messages in file transfer.
        m_fileTransfer->setOutput(output);

        // Update ::list_base of supported operation for file transfer.
        ::array_base<unsigned int> clientMsgCodes;
        m_pviewercore->getEnabledClientMsgCapabilities(&clientMsgCodes);

        ::array_base<unsigned int> serverMsgCodes;
        m_pviewercore->getEnabledServerMsgCapabilities(&serverMsgCodes);

        m_fileTransfer->getCore()->updateSupportedOperations(&clientMsgCodes, &serverMsgCodes);

        //::string strUrl;

        //strUrl.format("wss://{}:{}/start_remoting_notify_node_websocket", m_pconnectiondata->getHost(), 15900);

        //auto pmanualresethappeningWebsocketStarted = createø<::manual_reset_happening>();

        //m_papplication->forkø() << [this, strUrl, pmanualresethappeningWebsocketStarted]
        //   {

        //      construct_newø(m_pkeyboardlayoutchange);

        //      ::task_set_name("wsRemotingNotify");

        //      ::property_set setHttp;

        //      setHttp["websocket_started_manual_reset_happening"] = pmanualresethappeningWebsocketStarted;

        //      setHttp["socket_http_callback"] = m_pkeyboardlayoutchange;

        //      ::system()->m_papplication->http()->http_get(m_phttpclientsocketNotifyChannel, strUrl, setHttp);

        //      information("websocket started or closed");

        //   };

        //pmanualresethappeningWebsocketStarted->wait(30_minutes);

        // Start viewer window and applying settings.
        post(
           [this]()
           {
              showWindow();
              setForegroundWindow();
              applySettings();
           });
    }

    void ViewerWindow::onDisconnect(const ::scoped_string & scopedstrMessage)
    {
        m_plogwriter->information("onDisconnect: {}", scopedstrMessage);
        m_disconnectMessage = scopedstrMessage;
        if (!m_stopped) {
            postMessage(WM_USER_DISCONNECT);
        }
    }

    void ViewerWindow::onAuthError(const ::remoting_client::AuthException *exception)
    {
        m_plogwriter->information("onAuthError ({}): {}",
                         exception->getAuthCode(), exception->get_message());
        int authCode = exception->getAuthCode();
        m_error = *exception;
        postMessage(WM_USER_AUTH_ERROR, authCode);
    }

    void ViewerWindow::onError(const ::subsystem::Exception *exception)
    {
        m_error = *exception;
        postMessage(WM_USER_ERROR);
    }

    void ViewerWindow::onFramebufferUpdate(const ::innate_subsystem::Framebuffer *pframebuffer, const ::int_rectangle &  rectangle)
    {
        m_pdesktopwindow->updateFramebuffer(pframebuffer, rectangle);
    }

    void ViewerWindow::onFramebufferPropChange(const ::innate_subsystem::Framebuffer *pframebuffer)
    {
        //   m_pdesktopwindow->m_iDivisor = m_pconnectiondata->getDivisor();
        // ((::innate_subsystem::Framebuffer*)pframebuffer)->m_iDivisor = m_pdesktopwindow->m_iDivisor;
        m_pdesktopwindow->setNewFramebuffer(pframebuffer);
    }

    void ViewerWindow::onCutText(const ::scoped_string & cutText)
    {
        m_pdesktopwindow->setClipboardData(cutText);
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

    // void ViewerWindow::adjustWindowSize()
    // {
    //     // If size isn't changed by user, then adjust size.
    //     if (!m_sizeIsChanged) {
    //         ::int_rectangle defaultSize = calculateDefaultSize();
    //         bool defaultSizeIsChanged = defaultSize.width() != m_rcNormal.width() ||
    //                                     defaultSize.height() != m_rcNormal.height();
    //         // If size is changed, isn't full screen, if window isn't maximized,
    //         // then set new pointPosition and size.
    //         if (!m_isFullScr && defaultSizeIsChanged) {
    //             m_rcNormal = defaultSize;
    //             setPosition(m_rcNormal.left, m_rcNormal.top);
    //             setSize(m_rcNormal.width(), m_rcNormal.height());
    //         }
    //
    //         // This is done for keyboard hooks to work.
    //         // If m_pconnectionconfig->isFullscreenEnabled() is true,
    //         // hooks don't work at the first start of the viewer.
    //         if (m_hooksEnabledFirstTime && m_isFullScr) {
    //             try {
    //                 // Registration of keyboard hook.
    //                 m_winHooks.registerKeyboardHook(this);
    //                 // Switching off ignoring win key.
    //                 m_pdesktopwindow->setWinKeyIgnore(false);
    //                 m_hooksEnabledFirstTime = false;
    //             } catch (::exception &e) {
    //                 m_plogwriter->error("{}", e.get_message());
    //             }
    //         }
    //     }
    // }

    void ViewerWindow::updateKeyState()
    {
        LRESULT ctrlState = m_toolbar.getState(IDS_TB_CTRL);
        if (ctrlState != 0) {
            m_toolbar.checkButton(IDS_TB_CTRL, m_pdesktopwindow->getCtrlState());
        }

        LRESULT altState = m_toolbar.getState(IDS_TB_ALT);
        if (altState != 0) {
            m_toolbar.checkButton(IDS_TB_ALT, m_pdesktopwindow->getAltState());
        }
    }

    ::string ViewerWindow::formatWindowName() const
    {
        ::string desktopName = m_pviewercore->getRemoteDesktopName();
        if (desktopName.is_empty() && !m_pconnectiondata->getHost().is_empty()) {
            desktopName = m_pconnectiondata->getHost();
        }
        ::string windowName;
        if (!desktopName.is_empty()) {
            windowName.format("{} - {}", desktopName, ::string(ProductNames::VIEWER_PRODUCT_NAME));
        } else {
            windowName.format("{}", ::string(ProductNames::VIEWER_PRODUCT_NAME));
        }
        return windowName;
    }

    bool ViewerWindow::operating_system_hook_on_keyboard_message(::lresult & lresult, ::user::enum_message emessage, int iVkCode, ::lparam lparam)
    {

       //KBDLLHOOKSTRUCT *str = (KBDLLHOOKSTRUCT*) lParam;
       // Ignoring of CapsLock, NumLock, ScrollLock, ::innate_subsystem::Control (Ctrl key), Menu (Alt key), Shift (shift key).
       if (iVkCode != VK_CAPITAL && iVkCode != VK_NUMLOCK && iVkCode != VK_SCROLL &&
           iVkCode != VK_LCONTROL && iVkCode != VK_RCONTROL &&
           iVkCode != VK_LMENU && iVkCode != VK_RMENU &&
           iVkCode != VK_LSHIFT && iVkCode != VK_RSHIFT)
       {
          // // Set the repeat count for the current scopedstrMessage bits.
          // ::lparam newLParam = 1;
          // // Set the scan code bits.
          // newLParam |= (str->scanCode & 0xf) << 16;
          // // Set the extended key bit.
          // newLParam |= (str->flags & LLKHF_EXTENDED) << 24;
          // // Set the context code bit.
          // newLParam |= ((str->flags & LLKHF_ALTDOWN) > 0) << 29;
          // // Set the transition state bit.
          // newLParam |= ((str->flags & LLKHF_UP) > 0) << 31;
         if (emessage == ::user::e_message_key_down || emessage == ::user::e_message_sys_key_down)
         {
             m_pdesktopwindow->postMessage((unsigned int) emessage, iVkCode, lparam);
         }
         else if (emessage == ::user::e_message_key_up || emessage == ::user::e_message_sys_key_up)
         {
             m_pdesktopwindow->postMessage((unsigned int) emessage, iVkCode, lparam);
         }
         lresult = 1;
          return true;
       } else {
          return false;
       }

       return false;

    }
} // namespace remoting_client