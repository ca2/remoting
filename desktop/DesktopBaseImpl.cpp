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
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//
#include "framework.h"
#include "DesktopBaseImpl.h"
#include "util/BrokenHandleException.h"

DesktopBaseImpl::DesktopBaseImpl(ClipboardListener *extClipListener,
                       UpdateSendingListener *extUpdSendingListener,
                       AbnormDeskTermListener *extDeskTermListener,
                       LogWriter *log)
: m_extUpdSendingListener(extUpdSendingListener),
  m_extDeskTermListener(extDeskTermListener),
  m_extClipListener(extClipListener),
  m_userInput(0),
  m_updateHandler(0),
  m_log(log)
{
}

DesktopBaseImpl::~DesktopBaseImpl()
{
}

void DesktopBaseImpl::getCurrentUserInfo(::string & desktopName,
                                    ::string & userName)
{
  _ASSERT(m_userInput != 0);
  _ASSERT(m_extDeskTermListener != 0);
  m_log->information("get current user information");
  try {
    m_userInput->getCurrentUserInfo(desktopName, userName);
  } catch (::exception &e) {
	m_log->error("::remoting::Exception in DesktopBaseImpl::getCurrentUserInfo: {}", e.get_message());
    m_extDeskTermListener->onAbnormalDesktopTerminate();
  }
}

void DesktopBaseImpl::getFrameBufferProperties(::int_size *dim, PixelFormat *pf)
{
  _ASSERT(m_updateHandler != 0);
  _ASSERT(m_extDeskTermListener != 0);
  m_log->debug("get frame buffer properties");
  try {
    m_updateHandler->getFrameBufferProp(dim, pf);
  } catch (::exception &e) {
	m_log->error("::remoting::Exception in DesktopBaseImpl::getFrameBufferProperties: {}", e.get_message());
    m_extDeskTermListener->onAbnormalDesktopTerminate();
  }
}

void DesktopBaseImpl::getPrimaryDesktopCoords(::int_rectangle *rect)
{
  _ASSERT(m_userInput != 0);
  _ASSERT(m_extDeskTermListener != 0);
  m_log->information("get primary desktop coordinates");
  try {
    m_userInput->getPrimaryDisplayCoords(rect);
  } catch (::exception &e) {
	m_log->error("::remoting::Exception in DesktopBaseImpl::getPrimaryDesktopCoords: {}", e.get_message());
    m_extDeskTermListener->onAbnormalDesktopTerminate();
  }
}

void DesktopBaseImpl::getDisplayNumberCoords(::int_rectangle *rect,
                                        unsigned char dispNumber)
{
  _ASSERT(m_userInput != 0);
  _ASSERT(m_extDeskTermListener != 0);
  m_log->information("get the %u display coordinates", (unsigned int) dispNumber);
  try {
    m_userInput->getDisplayNumberCoords(rect, dispNumber);
  } catch (::exception &e) {
  	m_log->error("::remoting::Exception in DesktopBaseImpl::getDisplayNumberCoords: {}", e.get_message());
    m_extDeskTermListener->onAbnormalDesktopTerminate();
  }
}

::array_base<::int_rectangle> DesktopBaseImpl::getDisplaysCoords()
{
  _ASSERT(m_userInput != 0);
  _ASSERT(m_extDeskTermListener != 0);
  m_log->information("get the displays coordinates");
  try {
    return m_userInput->getDisplaysCoords();
  }
  catch (::exception &e) {
    m_log->error("::remoting::Exception in DesktopBaseImpl::getDisplayCoords: {}", e.get_message());
    m_extDeskTermListener->onAbnormalDesktopTerminate();
  }
  return ::array_base<::int_rectangle>();
}


void DesktopBaseImpl::getNormalizedRect(::int_rectangle *rect)
{
  _ASSERT(m_userInput != 0);
  _ASSERT(m_extDeskTermListener != 0);
  m_log->information("normilize a rect to frame buffer coordinates");
  try {
    m_userInput->getNormalizedRect(rect);
  } catch (::exception &e) {
	m_log->error("::remoting::Exception in DesktopBaseImpl::getNormalizedRect: {}", e.get_message());
    m_extDeskTermListener->onAbnormalDesktopTerminate();
  }
}

void DesktopBaseImpl::getWindowCoords(HWND hwnd, ::int_rectangle *rect)
{
  _ASSERT(m_userInput != 0);
  _ASSERT(m_extDeskTermListener != 0);
  m_log->information("get window coordinates");
  try {
    m_userInput->getWindowCoords(hwnd, rect);
  } catch (BrokenHandleException &) {
    throw;
  } catch (::exception &e) {
	m_log->error("::remoting::Exception in DesktopBaseImpl::getWindowCoords: {}", e.get_message());
    m_extDeskTermListener->onAbnormalDesktopTerminate();
  }
}

HWND DesktopBaseImpl::getWindowHandleByName(const ::scoped_string & windowName)
{
  _ASSERT(m_userInput != 0);
  _ASSERT(m_extDeskTermListener != 0);
  m_log->information("get a window handle by a window name");
  try {
    return m_userInput->getWindowHandleByName(windowName);
  } catch (::exception &e) {
	m_log->error("::remoting::Exception in DesktopBaseImpl::getWindowHandleByName: {}", e.get_message());
    m_extDeskTermListener->onAbnormalDesktopTerminate();
  }
  return 0;
}

void DesktopBaseImpl::getApplicationRegion(unsigned int procId, Region *region)
{
  _ASSERT(m_userInput != 0);
  _ASSERT(m_extDeskTermListener != 0);
  m_log->information("get application region");
  try {
    m_userInput->getApplicationRegion(procId, region);
  } catch (::exception &e) {
	m_log->error("::remoting::Exception in DesktopBaseImpl::getApplicationRegion: {}", e.get_message());
    m_extDeskTermListener->onAbnormalDesktopTerminate();
  }
}

bool DesktopBaseImpl::isApplicationInFocus(unsigned int procId)
{
  _ASSERT(m_userInput != 0);
  _ASSERT(m_extDeskTermListener != 0);
  m_log->information("checking if application is in focus");
  try {
    return m_userInput->isApplicationInFocus(procId);
  } catch (::exception &e) {
	m_log->error("::remoting::Exception in DesktopBaseImpl::isApplicationInFocus {}", e.get_message());
    m_extDeskTermListener->onAbnormalDesktopTerminate();
  }
  return false;
}

bool DesktopBaseImpl::isRemoteInputAllowed()
{
  m_log->debug("checking remote input allowing");

  bool enabled = !Configurator::getInstance()->getServerConfig()->isBlockingRemoteInput();
  enabled = enabled && !isRemoteInputTempBlocked();
  return enabled;
}

void DesktopBaseImpl::setKeyboardEvent(unsigned int keySym, bool down)
{
  _ASSERT(m_userInput != 0);
  _ASSERT(m_extDeskTermListener != 0);

  m_log->information("set keyboard event (keySym = %u, down = {})", keySym, (int)down);
  try {
    if (isRemoteInputAllowed()) {
      m_userInput->setKeyboardEvent(keySym, down);
    }
  } catch (::exception &e) {
    m_log->error("setKeyboardEvent() crashed: {}", e.get_message());
    m_extDeskTermListener->onAbnormalDesktopTerminate();
  }
}

void DesktopBaseImpl::setMouseEvent(unsigned short x, unsigned short y, unsigned char buttonMask)
{
  _ASSERT(m_userInput != 0);
  _ASSERT(m_extDeskTermListener != 0);

  m_log->information("set mouse event (x = %u, y = %u, mask = %u)", (unsigned int)x, (unsigned int)y, (unsigned int)buttonMask);
  Point point(x, y);
  try {
    if (isRemoteInputAllowed()) {
      m_userInput->setMouseEvent(point, buttonMask);
    }
  } catch (::exception &e) {
	m_log->error("::remoting::Exception in DesktopBaseImpl::setMouseEvent {}", e.get_message());
    m_extDeskTermListener->onAbnormalDesktopTerminate();
  }
}

void DesktopBaseImpl::setNewClipText(const ::scoped_string & newClipboard)
{
  _ASSERT(m_userInput != 0);
  _ASSERT(m_extDeskTermListener != 0);

  m_log->debug("set new clipboard text, length: {}", newClipboard->length());

  {
    AutoLock al(&m_storedClipCritSec);
    m_receivedClip = *newClipboard;
  }
  try {
    m_userInput->setNewClipboard(newClipboard);
  } catch (::exception &e) {
	  m_log->error("::remoting::Exception in DesktopBaseImpl::setNewClipText {}", e.get_message());
    m_extDeskTermListener->onAbnormalDesktopTerminate();
  }
}

void DesktopBaseImpl::sendUpdate()
{
  _ASSERT(m_updateHandler != 0);
  _ASSERT(m_extDeskTermListener != 0);
  _ASSERT(m_extUpdSendingListener != 0);

  if (!m_extUpdSendingListener->isReadyToSend()) {
    m_log->debug("nobody is ready for updates");
    return;
  }
  UpdateContainer updCont;
  try {
    if (!m_fullReqRegion.is_empty()) {
      m_log->debug("set full update request to UpdateHandler");
      m_updateHandler->setFullUpdateRequested(&m_fullReqRegion);
    }

    m_log->debug("extracting updates from UpdateHandler");
    m_updateHandler->extract(&updCont);
  } catch (::exception &e) {
    m_log->information("WinDesktop::sendUpdate() failed with error:{}",
               e.get_message());
    m_extDeskTermListener->onAbnormalDesktopTerminate();
  }

  if (!updCont.is_empty() || !m_fullReqRegion.is_empty()) {
    m_log->debug("UpdateContainer is not empty."
              " Updates will be given to all.");
    m_extUpdSendingListener->onSendUpdate(&updCont,
                                          m_updateHandler->getCursorShape());
    AutoLock al(&m_reqRegMutex);
    m_fullReqRegion.clear();
  } else {
    m_log->information("UpdateContainer is empty");
  }
}

void DesktopBaseImpl::onUpdate()
{
  m_log->debug("update detected");
  m_newUpdateEvent.notify();
}

void DesktopBaseImpl::onUpdateRequest(const ::int_rectangle &  rectRequested, bool incremental)
{
  m_log->debug("DesktopBaseImpl::onUpdateRequest: update requested");

  AutoLock al(&m_reqRegMutex);
  if (!incremental) {
    m_fullReqRegion.addRect(rectRequested);
  }
  m_newUpdateEvent.notify();
}

void DesktopBaseImpl::onClipboardUpdate(const ::scoped_string & newClipboard)
{
  _ASSERT(m_extClipListener != 0);

  m_log->debug("clipboard update detected, length: {}", newClipboard->length());
  bool isEqual;
  {
    AutoLock al(&m_storedClipCritSec);
    isEqual = m_receivedClip.isEqualTo(newClipboard);
  }
  if (!isEqual) {
    {
      AutoLock al(&m_storedClipCritSec);
      m_receivedClip = "";
    }
    // Send new clipboard text, even if it is empty.
    m_log->debug("Send new clipboard content");
    m_extClipListener->onClipboardUpdate(newClipboard);
  }
  else {
    m_log->debug("do not send new clipboard content");
  }
}

void DesktopBaseImpl::onConfigReload(ServerConfig *serverConfig)
{
  applyNewConfiguration();
}

bool DesktopBaseImpl::updateExternalFrameBuffer(FrameBuffer *fb, const Region *region,
                                           const ::int_rectangle &  viewPort)
{
  return m_updateHandler->updateExternalFrameBuffer(fb, region, viewPort);
}
