// Copyright (C) 2008,2009,2010,2011,2012 GlavSoft LLC.
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
#include "BaseDialog.h"

#include "remoting/remoting_common/util/CommonHeader.h"

#include <commctrl.h>
#include <crtdbg.h>


HINSTANCE remoting_impact_hinstance();

BaseDialog::BaseDialog()
: m_pwindowParent(NULL), m_resourceName(0), m_resourceId(0), m_hicon(0)
{
}

BaseDialog::BaseDialog(DWORD resourceId)
: m_pwindowParent(NULL), m_resourceName(0), m_resourceId(resourceId), m_hicon(0)
{
}

BaseDialog::BaseDialog(const ::scoped_string & scopedstrResourceName)
: m_pwindowParent(NULL), m_resourceName(0), m_resourceId(0), m_hicon(0)
{
  setResourceName(scopedstrResourceName);
}

BaseDialog::~BaseDialog()
{
  if (m_hicon) {
    DeleteObject(m_hicon);
  }
  if (m_resourceName != 0) {
    free(m_resourceName);
  }
}

void BaseDialog::setResourceName(const ::scoped_string & scopedstrResourceName)
{
  if (m_resourceName != 0) {
    free(m_resourceName);
  }

  m_resourceName = _tcsdup(::wstring(scopedstrResourceName).c_str());
}

void BaseDialog::setResourceId(DWORD id)
{
  m_resourceId = id;
}

void BaseDialog::setDefaultPushButton(UINT buttonId)
{
  SendMessage(m_hwnd, DM_SETDEFID, buttonId, 0);
}

void BaseDialog::set_parent(::remoting::Window *pwindowParent)
{
  m_pwindowParent = pwindowParent;
}

int BaseDialog::show()
{
  if (m_hwnd == NULL) {
    create();
  } else {
    set_foreground_window();
  }
  return 0;
}

void BaseDialog::hide()
{
  set_visible(false);
}

void BaseDialog::close_dialog(int code)
{
  // Destroy dialog
  if (!m_isModal) {
    DestroyWindow(m_hwnd);
  } else {
    EndDialog(m_hwnd, code);
  }
  // We have no valid hwnd, so forse set hwnd to NULL
  setWindow(NULL);
}

void BaseDialog::create()
{
  HWND window, parentWindow = NULL;

  if (m_pwindowParent != NULL) {
    parentWindow = m_pwindowParent->get_hwnd();
  }

   auto pszResourceName = getResouceName();

   HINSTANCE hinstance = remoting_impact_hinstance();

  //window = CreateDialogParam(GetModuleHandle(NULL), pszResourceName,
   window = CreateDialogParam(hinstance, pszResourceName,
                             parentWindow, dialogProc, (LPARAM)this);

  m_isModal = false;

  _ASSERT(window != NULL);
}



int BaseDialog::showModal()
{
  int result = 0;
  if (m_hwnd == NULL) {
    m_isModal = true;
     HINSTANCE hinstance = remoting_impact_hinstance();
    HWND parentWindow = (m_pwindowParent != NULL) ? m_pwindowParent->get_hwnd() : NULL;
    result = (int)DialogBoxParam(hinstance,
                                 getResouceName(),
                                 parentWindow, dialogProc, (LPARAM)this);
  } else {
    set_visible(true);
    set_foreground_window();
  }

  //
  // TODO: Place error notification here
  //

  if (result == -1) {
  }

  return result;
}

bool BaseDialog::isCreated()
{
  bool isInit = m_hwnd != 0;

  if (!isInit) {
    return false;
  }

  return !!IsWindow(m_hwnd);
}

BOOL BaseDialog::onDrawItem(WPARAM controlID, LPDRAWITEMSTRUCT dis)
{
  return TRUE;
}

void BaseDialog::onMessageReceived(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
}

INT_PTR CALLBACK BaseDialog::dialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

   BaseDialog * pbasedialog = nullptr;
  BOOL bResult;

  bResult = FALSE;
  if (uMsg == WM_INITDIALOG) {
    pbasedialog = (BaseDialog *)lParam;
    SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pbasedialog);
    pbasedialog->setWindow(hwnd);
    pbasedialog->updateIcon();
  } else {
    pbasedialog = (BaseDialog *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    if (pbasedialog == 0) {
      return FALSE;
    }
  }
//   BaseDialog *_this;

    INT_PTR iptr = 0;

    pbasedialog->dialog_procedure(iptr, uMsg, wParam, lParam);

   return iptr;

}

bool BaseDialog::dialog_procedure(INT_PTR & iptrResult, UINT message, ::wparam wparam, ::lparam lparam)
{

   BOOL bResult = FALSE;

   onMessageReceived(message, wparam, lparam);

   switch (message) {
      case WM_INITDIALOG:
         bResult = onInitDialog();
         break;
      case WM_NOTIFY:
         bResult = onNotify(LOWORD(wparam.m_number), lparam.m_lparam);
         break;
      case WM_COMMAND:
         bResult =onCommand(LOWORD(wparam.m_number), HIWORD(wparam.m_number));
         break;
      case WM_CLOSE:
         bResult = onClose();
         break;
      case WM_DESTROY:
         bResult = onDestroy();
         break;
      case WM_DRAWITEM:
         bResult = onDrawItem(wparam.m_number, (LPDRAWITEMSTRUCT)lparam.m_lparam);
         break;
   }


    iptrResult = bResult;

   return true;

}

TCHAR *BaseDialog::getResouceName()
{
  if (m_resourceId != 0) {
    return MAKEINTRESOURCE(m_resourceId);
  }
  return m_resourceName;
}

void BaseDialog::setControlById(::remoting::Window &control, DWORD id) 
{
  control = GetDlgItem(m_hwnd, id);
}

void BaseDialog::subclassControlById(::remoting::Window * pcontrol, DWORD id)
{
   pcontrol->m_hwnd = GetDlgItem(m_hwnd, id);
   pcontrol->subclass_window();
}

void BaseDialog::updateIcon()
{
  if (m_hicon) {
    SetClassLongPtr(m_hwnd, GCLP_HICON, (LONG_PTR)m_hicon);
  }
}

void BaseDialog::loadIcon(DWORD id)
{
  if (m_hicon) {
    DeleteObject(m_hicon);
  }
  m_hicon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(id));
}

// bool BaseDialog::set_foreground_window()
// {
//   return set_foreground_window();
// }

BOOL BaseDialog::onInitDialog()
{
  return FALSE;
}

BOOL BaseDialog::onNotify(UINT controlID, LPARAM data)
{
  return FALSE;
}

BOOL BaseDialog::onCommand(UINT controlID, UINT notificationID)
{
  return FALSE;
}

BOOL BaseDialog::onDestroy()
{
  return FALSE;
}

BOOL BaseDialog::onClose()
{
  return FALSE;
}
