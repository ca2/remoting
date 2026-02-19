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
#include "DummyScreenDriver.h"

DummyScreenDriver::DummyScreenDriver(UpdateKeeper *updateKeeper, UpdateListener *updateListener, ::int_size dim, unsigned int interval, LogWriter *log)
  : m_updateListener(updateListener),
  m_updateKeeper(updateKeeper),
  m_interval(interval)
{
  PixelFormat pixelFormat;
  pixelFormat.initBigEndianByNative();
  pixelFormat.bitsPerPixel = 32;
  pixelFormat.redMax = 0xff;
  pixelFormat.greenMax = 0xff;
  pixelFormat.blueMax = 0xff;
  pixelFormat.redShift = 16;
  pixelFormat.greenShift = 8;
  pixelFormat.blueShift = 0;
  m_workFrameBuffer.setProperties(&dim, &pixelFormat);
  m_detectionEnabled = false;
  resume();
}

DummyScreenDriver::~DummyScreenDriver()
{
  terminate();
  wait();
}

void DummyScreenDriver::onTerminate()
{
  m_sleeper.notify();
}

void DummyScreenDriver::execute()
{
  while (!isTerminating()) {
    m_sleeper.waitForEvent(m_interval);
    if (!isTerminating()) {
      try {
        ::int_size dim = m_workFrameBuffer.getDimension();
        int w = dim.cx;
        int h = dim.cy;
        int x = rand() % (w / 50) + (w / 50);
        int y = rand() % (h / 50) + (h / 50);

        ::int_rectangle r(x, y);
        r.move(rand() % w, rand() % h);
        int m = 0xffffff / RAND_MAX;
        unsigned int color = rand() * m + rand() % m;
        ::int_rectangle tmp(w,h);
        Region reg(tmp);
        if (m_detectionEnabled) {
          m_workFrameBuffer.fillRect(&r, color);
          m_updateKeeper->addChangedRegion(&reg);
          m_updateListener->onUpdate();
        }
      }
      catch (...) {
      }
    }
  }
}

void DummyScreenDriver::executeDetection()
{
  m_detectionEnabled = true;
}


void DummyScreenDriver::terminateDetection()
{
  m_detectionEnabled = false;
}

::int_size DummyScreenDriver::getScreenDimension()
{
  return m_workFrameBuffer.getDimension();
}

bool DummyScreenDriver::grabFb(const ::int_rectangle &  rect)
{
  return true;
}

FrameBuffer *DummyScreenDriver::getScreenBuffer()
{
  return &m_workFrameBuffer;
}

bool DummyScreenDriver::getScreenPropertiesChanged()
{
  return false;
}

bool DummyScreenDriver::getScreenSizeChanged()
{
  return false;
}

bool DummyScreenDriver::applyNewScreenProperties()
{
  return true;
}
