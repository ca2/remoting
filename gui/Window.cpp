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
#include "remoting/gui/Window.h"

namespace remoting
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
   // void Window::replaceWindowProc(WNDPROC wndProc)
   // {
   // }
   LRESULT CALLBACK Window::s_window_procedure(HWND hWnd, UINT scopedstrMessage, WPARAM wParam, LPARAM lParam)
   {
      auto p = (Window *)GetWindowLongPtr(hWnd, GWLP_USERDATA);
      LRESULT lresult = 0;
      if ( p->window_procedure(lresult, scopedstrMessage, wParam, lParam))
      {

         return lresult;
      }
      return ::CallWindowProc((WNDPROC)p->m_defWindowProc, hWnd, scopedstrMessage, wParam, lParam);
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
      SetWindowLongPtr(m_hwnd, GWLP_WNDPROC, (LONG_PTR)&Window::s_window_procedure);



   }
   void Window::post_message(UINT scopedstrMessage, ::wparam wparam, ::lparam lparam)
   {

      ::PostMessage(m_hwnd, scopedstrMessage, wparam.m_number, lparam.m_lparam);
   }
   void Window::unsubclass_window()
   {
      if (!m_defWindowProc)
         return;

      SetWindowLongPtr(m_hwnd, GWLP_USERDATA, (LONG_PTR)0);
      SetWindowLongPtr(m_hwnd, GWLP_WNDPROC, (LONG_PTR)m_defWindowProc);
      m_defWindowProc = nullptr;
   }

   bool Window::window_procedure(LRESULT & lresult, UINT scopedstrMessage, ::wparam wparam, ::lparam lparam)
   {

      return false;
   }

   void Window::set_visible(bool visible)
   {
      ShowWindow(m_hwnd, visible ? SW_SHOW : SW_HIDE);
   }

   bool Window::is_enabled()
   {
      return (!has_style(WS_DISABLED));
   }

   void Window::invalidate()
   {
      InvalidateRect(m_hwnd, NULL, TRUE);
   }

   ::string Window::get_text()
   {
      int length = (int)SendMessage(m_hwnd, WM_GETTEXTLENGTH, 0, 0);
      ::array_base<TCHAR> buf(length + 1);
      GetWindowText(m_hwnd, buf.data(), length + 1);
      return buf.data();
   }


   void Window::set_focus()
   {
      ::SetFocus(m_hwnd);
   }

   bool Window::has_focus()
   {
      return (::GetFocus() == m_hwnd) || (GetForegroundWindow() == m_hwnd);
   }

   bool Window::set_foreground_window()
   {
      return SetForegroundWindow(get_hwnd()) != 0;
   }

   void Window::set_style(DWORD style)
   {
      ::SetWindowLong(m_hwnd, GWL_STYLE, style);
   }

   DWORD Window::get_style()
   {
      return ::GetWindowLong(m_hwnd, GWL_STYLE);
   }

   void Window::add_style(DWORD styleFlag)
   {
      DWORD flags = get_style();
      flags |= styleFlag;
      set_style(flags);
   }

   void Window::clear_style(DWORD styleFlag)
   {
      DWORD flags = get_style();
      flags &= ~styleFlag;
      set_style(flags);
   }

   bool Window::has_style(DWORD styleFlag)
   {
      DWORD flags = get_style();
      return (flags & styleFlag) == styleFlag;
   }

   void Window::set_ex_style(DWORD style)
   {
      SetWindowLong(m_hwnd, GWL_EXSTYLE, style);
   }

   DWORD Window::get_ex_style()
   {
      return (DWORD)GetWindowLong(m_hwnd, GWL_EXSTYLE);
   }

   void Window::add_ex_style(DWORD styleFlag)
   {
      DWORD flags = get_ex_style();
      flags |= styleFlag;
      set_ex_style(flags);
   }

   void Window::clear_ex_style(DWORD styleFlag)
   {
      DWORD flags = get_ex_style();
      flags &= ~styleFlag;
      set_ex_style(flags);
   }

   bool Window::has_ex_style(DWORD styleFlag)
   {
      DWORD flags = get_ex_style();
      return (flags & styleFlag) == styleFlag;
   }

//
//
// }
//
// Window::Window()
// //: m_hwnd(NULL)
// {
// }
//
// // Window::Window(HWND hwnd)
// // : m_hwnd(hwnd)
// // {
// // }
//
// Window::~Window()
// {
// }

void Window::enable_window(bool enabled)
{
  if (enabled) {
    SendMessage(m_hwnd, WM_ENABLE, TRUE, NULL);
    clear_style(WS_DISABLED);
  } else {
    if (has_style(WS_DISABLED)) {
      return ;
    } // if already disabled
    SendMessage(m_hwnd, WM_ENABLE, FALSE, NULL);
    add_style(WS_DISABLED);
  }
  invalidate();
}

void Window::setText(const ::scoped_string & scopedstrText)
{
  SetWindowText(m_hwnd, ::wstring(scopedstrText));
}

void Window::setSignedInt(int value)
{
  ::string text;
  text.formatf("{}", value);
  setText(text);
}

void Window::setUnsignedInt(unsigned int value)
{
  ::string text;
  text.formatf("%u", value);
  setText(text);
}

//
// FIXME: Stub
//

void Window::setTextVerticalAlignment(VerticalAlignment align)
{
}



//
// FIXME: Stub
//

VerticalAlignment Window::getTextVerticalAlignment()
{
  return Left;
}

 void Window::set_parent(Window * pwindow)
 {
   ::SetParent(m_hwnd, pwindow->m_hwnd);
 }

} // namespace remoting


