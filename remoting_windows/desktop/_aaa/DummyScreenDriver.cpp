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

namespace remoting_macos
{

   //
   // DummyScreenDriver::DummyScreenDriver(::remoting::UpdateKeeper * pupdatekeeper, ::remoting::UpdateListener * pupdatelistener, const ::int_size & size,
   //                                   unsigned int interval, ::subsystem::LogWriter * plogwriter) :
   //  m_pupdatelistener = pupdatelistener;m_pupdatekeeper = pupdatekeeper; m_interval(interval)
   //  {
   //
   //     ::innate_subsystem::PixelFormat pixelFormat;
   //     pixelFormat.initBigEndianByNative();
   //     pixelFormat.bitsPerPixel = 32;
   //     pixelFormat.redMax = 0xff;
   //     pixelFormat.greenMax = 0xff;
   //     pixelFormat.blueMax = 0xff;
   //     pixelFormat.redShift = 16;
   //     pixelFormat.greenShift = 8;
   //     pixelFormat.blueShift = 0;
   //     m_pframebufferWork->setProperties(size, pixelFormat);
   //     m_detectionEnabled = false;
   //     resume();
   //  }


   DummyScreenDriver::DummyScreenDriver()
   {

   }

   DummyScreenDriver::~DummyScreenDriver()
   {
      terminate();
      wait();
   }


   void DummyScreenDriver::initialize_dummy_screen_driver(::remoting::UpdateKeeper * pupdatekeeper, ::remoting::UpdateListener * pupdatelistener, const ::int_size & size,
                                     unsigned int interval, ::subsystem::LogWriter * plogwriter)
   {
      m_pupdatelistener = pupdatelistener;
      m_pupdatekeeper = pupdatekeeper;
      m_interval = interval;
      ::innate_subsystem::PixelFormat pixelFormat;
      pixelFormat.initBigEndianByNative();
      pixelFormat.bitsPerPixel = 32;
      pixelFormat.redMax = 0xff;
      pixelFormat.greenMax = 0xff;
      pixelFormat.blueMax = 0xff;
      pixelFormat.redShift = 16;
      pixelFormat.greenShift = 8;
      pixelFormat.blueShift = 0;
      m_pframebufferWork->setProperties(size, pixelFormat);
      m_detectionEnabled = false;
      resume();
   }

   void DummyScreenDriver::onTerminate() { m_happeningSleeper.set_happening(); }

   void DummyScreenDriver::execute()
   {
      while (!isTerminating())
      {
         m_happeningSleeper.wait(m_interval * 1_ms);
         if (!isTerminating())
         {
            try
            {
               ::int_size size = m_pframebufferWork->getDimension();
               int w = size.cx;
               int h = size.cy;
               int x = rand() % (w / 50) + (w / 50);
               int y = rand() % (h / 50) + (h / 50);

               ::int_rectangle r(0, 0, x, y);
               r.offset(rand() % w, rand() % h);
               int m = 0xffffff / RAND_MAX;
               unsigned int color = rand() * m + rand() % m;
               ::int_rectangle rectangle(0, 0, w, h);
               Region region(rectangle);
               if (m_detectionEnabled)
               {
                  m_pframebufferWork->fillRect(r, color);
                  m_pupdatekeeper->addChangedRegion(region);
                  m_pupdatelistener->onUpdate();
               }
            }
            catch (...)
            {
            }
         }
      }
   }

   void DummyScreenDriver::executeDetection() { m_detectionEnabled = true; }


   void DummyScreenDriver::terminateDetection() { m_detectionEnabled = false; }

   ::int_size DummyScreenDriver::getScreenDimension() { return m_pframebufferWork->getDimension(); }

   bool DummyScreenDriver::grabFb(const ::int_rectangle & rectangle) { return true; }

   ::innate_subsystem::Framebuffer *DummyScreenDriver::getScreenBuffer() { return m_pframebufferWork; }

   bool DummyScreenDriver::getScreenPropertiesChanged() { return false; }

   bool DummyScreenDriver::getScreenSizeChanged() { return false; }

   bool DummyScreenDriver::applyNewScreenProperties() { return true; }


} // namespace remoting_macos
 


