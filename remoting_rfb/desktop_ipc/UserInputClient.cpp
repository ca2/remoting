// Copyright (C) 2009,2010,2011,2012 GlavSoft LLC.
// All rights reserved.
//
//-------------------------------------------------------------------------
// This file is part of the T i g h t V N C software.  Please visit our Web site:
//
//                       http://www.t i g h t v n c.com/
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
#include "UserInputClient.h"
//#include "subsystem/thread/lockable_critical_section.h"
#include "ReconnectException.h"
#if defined(WINDOWS_DESKTOP)
#include "acme/operating_system/windows/_.h"
#endif
#include "subsystem/platform/BrokenHandleException.h"

namespace remoting_rfb
{



// UserInputClient::UserInputClient(BlockingGate *pblockinggate,
//                                  DesktopSrvDispatcher * pdispatcher,
//                                  ClipboardListener *pclipboardlistener)
// : DesktopServerProto(pblockinggate),
//   m_clipboardListener(pclipboardlistener),
//   m_sendMouseFlags(0)
// {
//   dispatcher->registerNewHandle(CLIPBOARD_CHANGED, this);
// }

UserInputClient::UserInputClient() :
   m_sendMouseFlags(0)
{


}


UserInputClient::~UserInputClient()
{
}



void UserInputClient::initialize_user_input_client(::remoting_rfb_node::Configurator * pconfigurator, BlockingGate *pblockinggate, DesktopSrvDispatcher * pdispatcher, ::subsystem::ClipboardListener *pclipboardlistener)
//: DesktopServerProto(pblockinggate),
//  m_clipboardListener(pclipboardlistener),
//  m_sendMouseFlags(0)
{
  initialize_desktop_server_proto(pconfigurator, pblockinggate);
   m_pclipboardlistener = pclipboardlistener;
  pdispatcher->registerNewHandle(CLIPBOARD_CHANGED, this);
}

void UserInputClient::onRequest(::u8 reqCode, BlockingGate *pblockinggate)
{
  switch (reqCode) {
  case CLIPBOARD_CHANGED:
    {
      ::string newClipboard;
      readNewClipboard(newClipboard, pblockinggate);
      m_pclipboardlistener->onClipboardUpdate(newClipboard);
    }
    break;
  default:
    ::string errMess;
    errMess.format("Unknown {} protocol code received from a pipe "
                   "UserInputServer", (::i32)reqCode);
    throw ::subsystem::Exception(errMess);
    break;
  }
}

void UserInputClient::sendInit(BlockingGate *pblockinggate)
{
  critical_section_lock al(pblockinggate);
  pblockinggate->writeUInt8(USER_INPUT_INIT);
  pblockinggate->writeUInt8(m_sendMouseFlags);
}

void UserInputClient::setMouseEvent(const ::i32_point pointNewPosition, ::u8 keyFlag)
{
  critical_section_lock al(m_pcontrolgate);
  try {
    // Send mouse data
    m_pcontrolgate->writeUInt8(POINTER_POS_CHANGED);
    sendNewPointerPos(pointNewPosition, keyFlag, m_pcontrolgate);
    m_sendMouseFlags = keyFlag;
  } catch (ReconnectException &) {
  }
}

void UserInputClient::setNewClipboard(const ::scoped_string & newClipboard)
{
  critical_section_lock al(m_pcontrolgate);
  try {
    // Send clipboard data
    m_pcontrolgate->writeUInt8(CLIPBOARD_CHANGED);
    sendNewClipboard(newClipboard, m_pcontrolgate);
  } catch (ReconnectException &) {
  }
}

void UserInputClient::setKeyboardEvent(::u32 keySym, bool down)
{
  critical_section_lock al(m_pcontrolgate);
  try {
    // Send keyboard data
    m_pcontrolgate->writeUInt8(KEYBOARD_EVENT);
    sendKeyEvent(keySym, down, m_pcontrolgate);
  } catch (ReconnectException &) {
  }
}

void UserInputClient::getCurrentUserInfo(::string & desktopName,
                                         ::string & userName)
{
  critical_section_lock al(m_pcontrolgate);
  try {
    // Send request
    m_pcontrolgate->writeUInt8(USER_INFO_REQ);
    readUserInfo(desktopName, userName, m_pcontrolgate);
  } catch (ReconnectException &) {
  }
}

void UserInputClient::getPrimaryDisplayCoords(::i32_rectangle & rectangle)
{
  critical_section_lock al(m_pcontrolgate);
  bool success = false;
  do {
    try {
      // Send request
      m_pcontrolgate->writeUInt8(DESKTOP_COORDS_REQ);
      rectangle = readRect(m_pcontrolgate);
      success = true;
    } catch (ReconnectException &) {
    }
  } while (!success);
}

void UserInputClient::getDisplayNumberCoords(::i32_rectangle & rectangle,
                                             ::u8 dispNumber)
{
  critical_section_lock al(m_pcontrolgate);
  bool success = false;
  do {
    try {
      // Send request
      m_pcontrolgate->writeUInt8(DISPLAY_NUMBER_COORDS_REQ);
      m_pcontrolgate->writeUInt8(dispNumber);
      rectangle = readRect(m_pcontrolgate);
      success = true;
    } catch (ReconnectException &) {
    }
  } while (!success);
}

::i32_rectangle_array_base UserInputClient::getDisplaysCoords()
{
  ::i32_rectangle_array_base res;
  critical_section_lock al(m_pcontrolgate);
  bool success = false;
  ::u8 number;
  do {
    try {
      res.resize(0);
      // Send request
      m_pcontrolgate->writeUInt8(DISPLAYS_COORDS_REQ);
      number = m_pcontrolgate->readUInt8();
      for (size_t i = 0; i < number; i++) {
        ::i32_rectangle rectangle = readRect(m_pcontrolgate);
        res.add(rectangle);
      }
      success = true;
    }
    catch (ReconnectException &) {
    }
  } while (!success);
  return res;
}

void UserInputClient::getNormalizedRect(::i32_rectangle & rectangle)
{
  critical_section_lock al(m_pcontrolgate);
  bool success = false;
  do {
    try {
      // Send request
      m_pcontrolgate->writeUInt8(NORMALIZE_RECT_REQ);
      sendRect(rectangle, m_pcontrolgate);
      rectangle = readRect(m_pcontrolgate);
      success = true;
    } catch (ReconnectException &) {
    }
  } while (!success);
}

void UserInputClient::getWindowCoords(const ::operating_system::window & operatingsystemwindow, ::i32_rectangle & rectangle)
{
  critical_section_lock al(m_pcontrolgate);
  bool success = false;
  do {
    try {
      // Send request
      m_pcontrolgate->writeUInt8(WINDOW_COORDS_REQ);
      m_pcontrolgate->writeUInt64((::u64)::as_u64(operatingsystemwindow));
      bool isBrokenWindow = m_pcontrolgate->readUInt8() != 0;
      if (!isBrokenWindow)
      {
         rectangle = readRect(m_pcontrolgate);
      }
      else
      {
        // Receive error discription (do not generate it here).
        // This made to avoid code duplication.
        ::string errMess;
        errMess = m_pcontrolgate->readUtf8();
        throw ::subsystem::BrokenHandleException(errMess);
      }
      success = true;
    } catch (ReconnectException &) {
    }
  } while (!success);
}

::operating_system::window UserInputClient::getWindowHandleByName(const ::scoped_string & windowName)
{
  critical_section_lock al(m_pcontrolgate);
  bool success = false;
  ::operating_system::window operatingsystemwindow;
  do {
    try {
      // Send request
      m_pcontrolgate->writeUInt8(WINDOW_HANDLE_REQ);
      m_pcontrolgate->writeUTF8(windowName);
      operatingsystemwindow = ::as_operating_system_window(m_pcontrolgate->readUInt64());
      success = true;
    } catch (ReconnectException &) {
    }
  } while (!success);
  return operatingsystemwindow;
}

void UserInputClient::getApplicationRegion(const ::process_identifier & processidentifier, Region & region)
{
  critical_section_lock al(m_pcontrolgate);
  bool success = false;
  do {
    try {
      // Send request
      m_pcontrolgate->writeUInt8(APPLICATION_REGION_REQ);
      m_pcontrolgate->writeUInt32(processidentifier);
      readRegion(region, m_pcontrolgate);
      success = true;
    } catch (ReconnectException &) {
    }
  } while (!success);
}

bool UserInputClient::isApplicationInFocus(const ::process_identifier & processidentifier)
{
  bool result = false;

  critical_section_lock al(m_pcontrolgate);
  bool success = false;
  do {
    try {
      // Send request
      m_pcontrolgate->writeUInt8(APPLICATION_CHECK_FOCUS);
      m_pcontrolgate->writeUInt32(processidentifier);
      // readRegion(region, m_pcontrolgate);
      result = (m_pcontrolgate->readUInt8() != 0);
      success = true;
    } catch (ReconnectException &) {
    }
  } while (!success);

  return result;
}



} // namespace remoting_rfb



