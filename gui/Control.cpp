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
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//
#include "framework.h"
#include "Control.h"

namespace windows
{

   Window::Window()
   {

   }

   Window::~Window()
   {


   }

   void Window::setWindow(HWND hwnd)
   {
      // Save handle
      m_hwnd = hwnd;
      // Save pointer to default window proc
      subclass_window();
   }
   // void Control::replaceWindowProc(WNDPROC wndProc)
   // {
   // }
   LRESULT CALLBACK Window::s_window_procedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
   {
      auto p = (Window *)GetWindowLongPtr(hWnd, GWLP_USERDATA);
      LRESULT lresult = 0;
      if ( p->window_procedure(lresult, message, wParam, lParam))
      {

         return lresult;
      }
      return ::CallWindowProc((WNDPROC)p->m_defWindowProc, hWnd, message, wParam, lParam);
   }

   HWND Window::get_hwnd() {

      return m_hwnd;

   }

   void Window::subclass_window()
   {
      if (m_defWindowProc)
         return;
      m_defWindowProc = (WNDPROC)GetWindowLongPtr(m_hwnd, GWLP_WNDPROC);
      SetWindowLongPtr(m_hwnd, GWLP_USERDATA, (LONG_PTR)(Window*)this);
      SetWindowLongPtr(m_hwnd, GWLP_WNDPROC, (LONG_PTR)&Control::s_window_procedure);



   }
   void Window::post_message(UINT message, ::wparam wparam, ::lparam lparam)
   {

      ::PostMessage(m_hwnd, message, wparam.m_number, lparam.m_lparam);
   }
   void Window::unsubclass_window()
   {
      if (!m_defWindowProc)
         return;

      SetWindowLongPtr(m_hwnd, GWLP_USERDATA, (LONG_PTR)0);
      SetWindowLongPtr(m_hwnd, GWLP_WNDPROC, (LONG_PTR)m_defWindowProc);
      m_defWindowProc = nullptr;
   }

   bool Window::window_procedure(LRESULT & lresult, UINT message, ::wparam wparam, ::lparam lparam)
   {

      return false;
   }
}

Control::Control()
//: m_hwnd(NULL)
{
}

// Control::Control(HWND hwnd)
// : m_hwnd(hwnd)
// {
// }

Control::~Control()
{
}

void Control::setEnabled(bool enabled)
{
  if (enabled) {
    SendMessage(m_hwnd, WM_ENABLE, TRUE, NULL);
    removeStyle(WS_DISABLED);
  } else {
    if (isStyleEnabled(WS_DISABLED)) {
      return ;
    } // if already disabled
    SendMessage(m_hwnd, WM_ENABLE, FALSE, NULL);
    addStyle(WS_DISABLED);
  }
  invalidate();
}

void Control::setText(const ::scoped_string & scopedstrtext)
{
  SetWindowText(m_hwnd, text);
}

void Control::setSignedInt(int value)
{
  ::string text;
  text.formatf("{}", value);
  setText(text);
}

void Control::setUnsignedInt(unsigned int value)
{
  ::string text;
  text.formatf("%u", value);
  setText(text);
}

//
// FIXME: Stub
//

void Control::setTextVerticalAlignment(VerticalAlignment align)
{
}

void Control::setFocus()
{
  ::SetFocus(m_hwnd);
}

bool Control::hasFocus()
{
  return (::GetFocus() == m_hwnd) || (GetForegroundWindow() == m_hwnd);
}

bool Control::setForeground()
{
  return SetForegroundWindow(get_hwnd()) != 0;
}

void Control::setVisible(bool visible)
{
  ShowWindow(m_hwnd, visible ? SW_SHOW : SW_HIDE);
}

bool Control::isEnabled()
{
  return (!isStyleEnabled(WS_DISABLED));
}

void Control::invalidate()
{
  InvalidateRect(m_hwnd, NULL, TRUE);
}

void Control::getText(::string & storage)
{
  int length = (int)SendMessage(m_hwnd, WM_GETTEXTLENGTH, 0, 0);
  ::array_base<TCHAR> buf(length + 1);
  GetWindowText(m_hwnd, &buf.front(), length + 1);
  storage-= &buf.front();
}

//
// FIXME: Stub
//

VerticalAlignment Control::getTextVerticalAlignment()
{
  return Left;
}

// HWND Control::getWindow()
// {
//   return m_hwnd;
// }

void Control::setStyle(DWORD style)
{
  ::SetWindowLong(m_hwnd, GWL_STYLE, style);
}

DWORD Control::getStyle()
{
  return ::GetWindowLong(m_hwnd, GWL_STYLE);
}

void Control::addStyle(DWORD styleFlag)
{
  DWORD flags = getStyle();
  flags |= styleFlag;
  setStyle(flags);
}

void Control::removeStyle(DWORD styleFlag)
{
  DWORD flags = getStyle();
  flags &= ~styleFlag;
  setStyle(flags);
}

bool Control::isStyleEnabled(DWORD styleFlag)
{
  DWORD flags = getStyle();
  return (flags & styleFlag) == styleFlag;
}

void Control::setExStyle(DWORD style)
{
  SetWindowLong(m_hwnd, GWL_EXSTYLE, style);
}

DWORD Control::getExStyle()
{
  return (DWORD)GetWindowLong(m_hwnd, GWL_EXSTYLE);
}

void Control::addExStyle(DWORD styleFlag)
{
  DWORD flags = getExStyle();
  flags |= styleFlag;
  setExStyle(flags);
}

void Control::removeExStyle(DWORD styleFlag)
{
  DWORD flags = getExStyle();
  flags &= ~styleFlag;
  setExStyle(flags);
}

bool Control::isExStyleEnabled(DWORD styleFlag)
{
  DWORD flags = getExStyle();
  return (flags & styleFlag) == styleFlag;
}

