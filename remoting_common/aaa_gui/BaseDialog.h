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

#pragma once


#include "innate_subsystem/Window.h"
//////#include "remoting/remoting_common/util/::string.h"
//
//class CLASS_DECL_REMOTING_COMMON BaseDialog :
//virtual public ::subsystem_apex::Control
//{
//public:
//
//
//    TCHAR *m_resourceName;        // Name of dialog resource
//    DWORD m_resourceId;            // Id of dialog resouce
//    //::subsystem_apex::Control m_ctrlThis;           // This dialog control
//    ::subsystem_apex::Control *m_pwindowParent;        // Parent dialog or NULL if no parent
//
//    bool m_isModal;
//    bool m_isCreated;
//
//    HICON m_hicon;
//
//
//
//   BaseDialog();
//   BaseDialog(DWORD resourceId);
//   BaseDialog(const ::scoped_string & scopedstrResourceName);
//   ~BaseDialog() override;
////public:
//
//   void setParent(::subsystem_apex::Control *pwindowParam) override;
//  // Method creates non modal window but not shows it
//   void create();
//
//  //
//  // Methods creates windows and show it in nonmodal/modal mode
//  //
//
//  int show();
//  int showModal();
//
//  // Returns true if dialog is already created.
//  bool isCreated();
//
//  // Method hides window
//  void hide();
//  // Method closes dialog
//  virtual void closeDialog(int code);
//  // Method sets parent window
//
//  // Set resource name for dialog
//  void setResourceName(const ::scoped_string & scopedstrResourceName);
//  // Set resource id for dialog.
//  void setResourceId(DWORD id);
//  // Return
//  //::subsystem_apex::Control *getControl() { return &m_ctrlThis; }
//  // Setup control by ID
//  void subclassControlById(::subsystem_apex::Control &control, DWORD id);
//   void subclassControlById(::subsystem_apex::Control * pcontrol, DWORD id);
//  // Icon manipulation 
//  void loadIcon(DWORD id);
//  void updateIcon();
//
//  // Puts this control foreground and activates it
//  //bool set_foreground_window();
//
////protected:
//  /**
//   * Sets default push button for dialog.
//   * @pararm buttonId new default push button id.
//   */
//  void setDefaultPushButton(unsigned int buttonId);
//
////protected:
//
//  //
//  // This methods must be overrided by child classes.
//  //
//
//  virtual bool onInitDialog();
//  virtual bool onNotify(unsigned int controlID, ::lparam data);
//  virtual bool onCommand(unsigned int controlID, unsigned int notificationID);
//  virtual bool onClose();
//  virtual bool onDestroy();
//
//  //
//  // This methods can be overrided by child classes.
//  //
//
//  virtual bool onDrawItem(::wparam controlID, LPDRAWITEMSTRUCT dis);
//  virtual void onMessageReceived(unsigned int uMsg, ::wparam wParam, ::lparam lParam);
//
//  //
//  // Window scopedstrMessage proccessing method
//  //
//
//    static INT_PTR CALLBACK dialogProc(HWND hwnd, unsigned int uMsg, ::wparam wParam, ::lparam lParam);
//    virtual bool dialog_procedure(INT_PTR & iptrResult, unsigned int message, ::wparam wparam, ::lparam lparam);
////private:
//    TCHAR *getResouceName();
//
////protected:
//
//};
//
//
