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
#pragma once
//#pragma once
//

#include "util/CommonHeader.h"
//#include "util/::string.h"

enum VerticalAlignment
{
  Left = 0x0,
  Center = 0x1,
  Right = 0x2
};

//
// Base class to control windows control
//

namespace windows
{

   class Window :
   virtual public ::particle
   {
   public:

      HWND m_hwnd = nullptr;
      WNDPROC m_defWindowProc=nullptr;


      Window();
      ~Window() override;

      virtual void setWindow(HWND hwnd);
      virtual void post_message(UINT message, ::wparam wparam=0, ::lparam lparam=0);

      static LRESULT CALLBACK s_window_procedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
      virtual bool window_procedure(LRESULT & lresult, UINT message, ::wparam wparam, ::lparam lparam);

      virtual void subclass_window();
      virtual void unsubclass_window();

      virtual void set_parent(Window *pwindowParent);

      virtual HWND get_hwnd();

      //
      // Sets input focus to this control
      //

      virtual void set_focus();

      //
      // Return true if window has input focus
      //

      virtual bool has_focus();

      //
      // Puts this control foreground and activates it
      //

      virtual bool set_foreground();

      //
      // Changes visible state of this control
      //

      virtual void set_visible(bool bVisible);

      //
      // Checks if this control is active (not disabled)
      //

      virtual bool is_enabled();

      //
      // Invalidates control
      //

      virtual void invalidate();

      //
      // Gets text associated with window
      //

      //virtual void getText(::string & storage);
      virtual ::string get_text();


   };
}

class Control :
virtual public ::windows::Window
{
public:
  Control();
  //Control(HWND hwnd);
  ~Control() override;

  //
  // Sets HWND associated with this control
  //

  //
  // Changes enable state of this control
  //

  virtual void setEnabled(bool enabled);

  //
  // Sets text associated with window
  //

  virtual void setText(const ::scoped_string & scopedstrtext);
  virtual void setSignedInt(int value);
  virtual void setUnsignedInt(unsigned int value);

  //
  // Sets text vertical alignment
  //

  void setTextVerticalAlignment(VerticalAlignment align);


  //
  // Returns text vertical alignment
  //

  VerticalAlignment getTextVerticalAlignment();

  //
  // Gets HWND associated with this control
  //



  void operator= (const HWND& window) 
  {
    setWindow(window);
  };

//protected:

  //
  // Sets windows style for this control
  //

  void setStyle(DWORD styleFlags);

  //
  // Gets windows style for this control
  //

  DWORD getStyle();

  //
  // Adds style to this control
  //

  void addStyle(DWORD styleFlag);

  //
  // Removes style from this control
  //

  void removeStyle(DWORD styleFlag);

  //
  // Returns true if styleFlags parameter is in
  // this control's style
  //

  bool isStyleEnabled(DWORD styleFlags);

  //
  // Private methods for manipulation window extended styles
  //

  void setExStyle(DWORD style);
  void addExStyle(DWORD styleFlag);
  void removeExStyle(DWORD styleFlag);

  DWORD getExStyle();
  bool isExStyleEnabled(DWORD styleFlag);

  //
  // Replaces default window proc with wndProc.
  //
  // This method must be called by child classes if needs to
  // override default window behavour.
  //

  //void replaceWindowProc(WNDPROC wndProc);
//    static LRESULT CALLBACK s_control_window_procedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
//    virtual bool control_window_procedure(LRESULT & lresult, UINT message, ::wparam wparam, ::lparam lparam);
// virtual void subclass_control();
//    virtual void unsubclass_control();
//protected:

  //
  // Handle of window
  //

  //HWND m_hwnd= nullptr;

  //
  // Pointer to default window procedure
  //


};

//
