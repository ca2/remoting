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
#include "Win8ScreenDriver.h"
#include "acme/subsystem/Exception.h"

Win8ScreenDriver::Win8ScreenDriver(UpdateKeeper *updateKeeper,
                                   UpdateListener *updateListener,
                                   critical_section *fbcritical_section,
                                   ::subsystem::LogWriter *log)
: WinVideoRegionUpdaterImpl(log),
  m_plogwriter(log),
  m_fbcritical_section(fbcritical_section),
  m_updateKeeper(updateKeeper),
  m_updateListener(updateListener),
  m_detectionEnabled(false)
{
  m_plogwriter->debug("Win8ScreenDriver creating new Win8ScreenDriverImpl");
  critical_section_lock al(&m_drvImplMutex);
  m_drvImpl = new Win8ScreenDriverImpl(m_plogwriter, m_updateKeeper, m_fbcritical_section, m_updateListener);
}

Win8ScreenDriver::~Win8ScreenDriver()
{
  terminateDetection();
  critical_section_lock al(&m_drvImplMutex);
  if (m_drvImpl != 0) {
    delete m_drvImpl;
    m_drvImpl = 0;
  }
}

void Win8ScreenDriver::executeDetection()
{
  critical_section_lock al(&m_drvImplMutex);
  m_detectionEnabled = true;
  m_drvImpl->executeDetection();
}

void Win8ScreenDriver::terminateDetection()
{
  critical_section_lock al(&m_drvImplMutex);
  m_detectionEnabled = false;
  if (m_drvImpl != 0) {
    m_drvImpl->terminateDetection();
  }
}

::int_size Win8ScreenDriver::getScreenDimension()
{
  critical_section_lock al(&m_drvImplMutex);
  return m_drvImpl->getScreenBuffer()->getDimension();
}

::innate_subsystem::FrameBuffer *Win8ScreenDriver::getScreenBuffer()
{
  critical_section_lock al(&m_drvImplMutex);
  return m_drvImpl->getScreenBuffer();
}

bool Win8ScreenDriver::grabFb(const ::int_rectangle &  rect)
{
  critical_section_lock al(&m_drvImplMutex);
  return m_drvImpl->grabFb(rect);
}

bool Win8ScreenDriver::getScreenPropertiesChanged()
{
  critical_section_lock al(&m_drvImplMutex);
  return !m_drvImpl->isValid();
}

bool Win8ScreenDriver::getScreenSizeChanged()
{
  critical_section_lock al(&m_drvImplMutex);
  return !m_drvImpl->isValid();
}

bool Win8ScreenDriver::applyNewScreenProperties()
{
  try {
    m_plogwriter->debug("Applying new screen properties, deleting old Win8ScreenDriverImpl");
    critical_section_lock al(&m_drvImplMutex);
    if (m_drvImpl != 0) {
      delete m_drvImpl;
      m_drvImpl = 0;
    }
    m_plogwriter->debug("Applying new screen properties, creating new Win8ScreenDriverImpl");
    Win8ScreenDriverImpl *drvImpl =
      new Win8ScreenDriverImpl(m_plogwriter, m_updateKeeper, m_fbcritical_section, m_updateListener, m_detectionEnabled);
    m_drvImpl = drvImpl;
  } catch (::exception &e) {
    m_plogwriter->error("Can't apply new screen properties: {}", e.get_message());
    return false;
  }
  return true;
}

bool Win8ScreenDriver::grabCursorShape(const ::innate_subsystem::PixelFormat & pf)
{
  critical_section_lock al(&m_drvImplMutex);
  m_drvImpl->updateCursorShape(&m_cursorShape);
  return !m_drvImpl->isValid();
}

const CursorShape *Win8ScreenDriver::getCursorShape()
{
  return &m_cursorShape;
}

Point Win8ScreenDriver::getCursorPosition()
{
  critical_section_lock al(&m_drvImplMutex);
  return m_drvImpl->getCursorPosition();
}

void Win8ScreenDriver::getCopiedRegion(::int_rectangle *copyRect, Point *source)
{
  critical_section_lock al(m_fbcritical_section);
  m_copyRectDetector.detectWindowMovements(copyRect, source);
}
