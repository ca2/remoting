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

#include "subsystem/_common_header.h"

// #include aaa_<commctrl.h>
//#include aaa_<crtdbg.h>


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

void BaseDialog::setDefaultPushButton(unsigned int buttonId)
{
  SendMessage(m_hwnd, DM_SETDEFID, buttonId, 0);
}

void BaseDialog::setParent(::innate_subsystem::Control *pwindowParent)
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

void BaseDialog::closeDialog(int code)
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
    parentWindow = m_pwindowParent->operating_system_window();
  }

   auto pszResourceName = getResouceName();

   HINSTANCE hinstance = remoting_impact_hinstance();

  //window = CreateDialogParam(GetModuleHandle(NULL), pszResourceName,
   window = CreateDialogParam(hinstance, pszResourceName,
                             parentWindow, dialogProc, (::lparam)this);

  m_isModal = false;

  _ASSERT(window != NULL);
}



int BaseDialog::showModal()
{
  int result = 0;
  if (m_hwnd == NULL) {
    m_isModal = true;
     HINSTANCE hinstance = remoting_impact_hinstance();
    HWND parentWindow = (m_pwindowParent != NULL) ? m_pwindowParent->operating_system_window() : NULL;
    result = (int)DialogBoxParam(hinstance,
                                 getResouceName(),
                                 parentWindow, dialogProc, (::lparam)this);
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

bool BaseDialog::onDrawItem(::wparam controlID, LPDRAWITEMSTRUCT dis)
{
  return true;
}

void BaseDialog::onMessageReceived(unsigned int uMsg, ::wparam wParam, ::lparam lParam)
{
}

INT_PTR CALLBACK BaseDialog::dialogProc(HWND hwnd, unsigned int uMsg, ::wparam wParam, ::lparam lParam)
{

   BaseDialog * pbasedialog = nullptr;
  bool bResult;

  bResult = false;
  if (uMsg == WM_INITDIALOG) {
    pbasedialog = (BaseDialog *)lParam;
    SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pbasedialog);
    pbasedialog->setWindow(hwnd);
    pbasedialog->updateIcon();
  } else {
    pbasedialog = (BaseDialog *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    if (pbasedialog == 0) {
      return false;
    }
  }
//   BaseDialog *_this;

    INT_PTR iptr = 0;

    pbasedialog->dialog_procedure(iptr, uMsg, wParam, lParam);

   return iptr;

}

bool BaseDialog::dialog_procedure(INT_PTR & iptrResult, unsigned int message, ::wparam wparam, ::lparam lparam)
{

   bool bResult = false;

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

void BaseDialog::subclassControlById(::innate_subsystem::Control &control, DWORD id) 
{
  control = GetDlgItem(m_hwnd, id);
}

void BaseDialog::subclassControlById(::innate_subsystem::Control * pcontrol, DWORD id)
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

bool BaseDialog::onInitDialog()
{
  return false;
}

bool BaseDialog::onNotify(unsigned int controlID, ::lparam data)
{
  return false;
}

bool BaseDialog::onCommand(unsigned int controlID, unsigned int notificationID)
{
  return false;
}

bool BaseDialog::onDestroy()
{
  return false;
}

bool BaseDialog::onClose()
{
  return false;
}
