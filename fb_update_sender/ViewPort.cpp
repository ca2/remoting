// Copyright (C) 2009,2010,2011,2012 GlavSoft LLC.
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
#include "ViewPort.h"
#include "remoting/remoting_common/thread/AutoLock.h"
#include "remoting/remoting_common/util/BrokenHandleException.h"

ViewPort::ViewPort(LogWriter *log)
: m_desktop(0),
  m_log(log)
{
}

ViewPort::ViewPort(const ViewPortState *viewPortState, LogWriter *log)
: m_desktop(0),
  m_state(*viewPortState),
  m_log(log)
{
}

ViewPort::~ViewPort()
{
}

void ViewPort::initDesktopInterface(Desktop *desktop)
{
  m_desktop = desktop;
}

void ViewPort::changeState(const ViewPortState *newState)
{
  AutoLock al(&m_stateMutex);
  m_state = *newState;
}

void ViewPort::update(const ::int_size & fbDimension)
{
  AutoLock al(&m_stateMutex);

  ::int_rectangle rect;
  switch(m_state.m_mode) {
  case ViewPortState::APPLICATION:
    _ASSERT(m_desktop != 0);
    m_desktop->getApplicationRegion(m_state.m_processId, &m_appRegion);
    // Also, the view port rectangle will be FULL_DESKTOP.
  case ViewPortState::FULL_DESKTOP:
    rect = fbDimension;
    break;
  case ViewPortState::PRIMARY_DISPLAY:
    _ASSERT(m_desktop != 0);
    m_desktop->getPrimaryDesktopCoords(&rect);
    break;
  case ViewPortState::DISPLAY_NUMBER:
    _ASSERT(m_desktop != 0);
    m_desktop->getDisplayNumberCoords(&rect, m_state.m_displayNumber);
    break;
  case ViewPortState::WINDOW_RECT:
    _ASSERT(m_desktop != 0);
    if (!m_state.m_windowIsResolved) {
      // Try resolve a window name to a hwnd.
      resolveWindowName();
    }
    if (m_state.m_windowIsResolved) {
      try {
        m_desktop->getWindowCoords(m_state.m_hwnd, &rect);
      } catch (BrokenHandleException &e) {
		  m_log->error("ViewPort::update: {}", e.get_message());
        // Now hwnd is broken. This should be reflected in the viewport state.
        m_state.unresolveHwnd();
      }
    }
    break;
  case ViewPortState::ARBITRARY_RECT:
    rect = m_state.m_arbitraryRect;
    _ASSERT(m_desktop != 0);
    m_desktop->getNormalizedRect(&rect);
    break;
  }
  m_log->debug("View port coordinates: ({}, {} %dx{})",
    rect.left, rect.top, rect.width(), rect.height());
  // Constrain and save
  m_rect = rect.intersection(::int_rectangle(fbDimension));
  if (m_rect.width() < 0 || m_rect.height() < 0) {
    m_rect.Null();
  }
  m_log->debug("Constrained (to the FrameBuffer dimension) view port coordinates: ({}, {} %dx{})",
    rect.left, rect.top, rect.width(), rect.height());
}

::int_rectangle ViewPort::getViewPortRect()
{
  AutoLock al(&m_stateMutex);
  return m_rect;
}

void ViewPort::resolveWindowName()
{
  // Skip the resolving if have been passed little time.
  if ((::earth::time::now() - m_latestHwndResolvingTime).getTime() > RESOLVING_PERIOD) {
    HWND hwnd = m_desktop->getWindowHandleByName(&m_state.m_windowName);
    if (hwnd != 0) {
      m_state.setWindowHandle(hwnd);
    }
    m_latestHwndResolvingTime = ::earth::time::now();
  }
}

bool ViewPort::getOnlyApplication()
{
  return m_state.m_mode == ViewPortState::APPLICATION;
}

unsigned int ViewPort::getApplicationId()
{
  return m_state.m_processId;
}

void ViewPort::getApplicationRegion(Region *region)
{
  *region = m_appRegion;
}
