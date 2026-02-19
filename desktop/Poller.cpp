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
#include "remoting/util/CommonHeader.h"
#include "Poller.h"
#include "region/Region.h"
#include "server_config_lib/Configurator.h"

Poller::Poller(UpdateKeeper *updateKeeper,
               UpdateListener *updateListener,
               ScreenGrabber *screenGrabber,
               FrameBuffer *backupFrameBuffer,
               LocalMutex *frameBufferCriticalSection,
               LogWriter *log)
: UpdateDetector(updateKeeper, updateListener),
  m_screenGrabber(screenGrabber),
  m_backupFrameBuffer(backupFrameBuffer),
  m_fbMutex(frameBufferCriticalSection),
  m_log(log)
{
  m_pollingRect.setRect(0, 0, 16, 16);
}

Poller::~Poller()
{
  terminate();
  wait();
}

void Poller::onTerminate()
{
  m_intervalWaiter.notify();
}

void Poller::execute()
{
  m_log->information("poller thread id = {}", getThreadId());

  FrameBuffer *screenFrameBuffer;

  {
    AutoLock al(m_fbMutex);
    screenFrameBuffer = m_screenGrabber->getScreenBuffer();
    ::int_rectangle fullScreenRect(screenFrameBuffer->getDimension());
    m_updateKeeper->addChangedRect(&fullScreenRect);
  }

  while (!isTerminating()) {
    Region region;

    {
      AutoLock al(m_fbMutex);

      screenFrameBuffer = m_screenGrabber->getScreenBuffer();
      if (!screenFrameBuffer->isEqualTo(m_backupFrameBuffer)) {
        m_updateKeeper->setScreenSizeChanged();
      } else {
        m_log->information("grabbing screen for polling");
        m_screenGrabber->grab();
        m_log->information("end of grabbing screen for polling");

        // Polling
        int pollingWidth = m_pollingRect.width();
        int pollingHeight = m_pollingRect.height();
        int screenWidth = screenFrameBuffer->getDimension().cx;
        int screenHeight = screenFrameBuffer->getDimension().cy;

        ::int_rectangle scanRect;
        for (int iRow = 0; iRow < screenHeight; iRow += pollingHeight) {
          for (int iCol = 0; iCol < screenWidth; iCol += pollingWidth) {
            scanRect.setRect(iCol, iRow, min(iCol + pollingWidth, screenWidth),
                             min(iRow + pollingHeight, screenHeight));
            if (!screenFrameBuffer->cmpFrom(&scanRect, m_backupFrameBuffer,
                                            scanRect.left, scanRect.top)) {
              region.addRect(&scanRect);
            }
          }
        }

        m_updateKeeper->addChangedRegion(&region);
      }
    } // AutoLock

    // Send event
    if (!region.is_empty()) {
      doUpdate();
    }

    unsigned int pollInterval = Configurator::getInstance()->
                                getServerConfig()->getPollingInterval();
    m_intervalWaiter.waitForEvent(pollInterval);
  }
}
