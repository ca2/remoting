// Copyright (C) 2011,2012 GlavSoft LLC.
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
#include "MirrorScreenDriver.h"
#include "subsystem/platform/Exception.h"

namespace remoting_windows
{

   // MirrorScreenDriver::MirrorScreenDriver(::remoting::UpdateKeeper * pupdatekeeper, ::remoting::UpdateListener * pupdatelistener,
   //                                        lockable_critical_section *pcriticalsectionFramebuffer, ::subsystem::LogWriter * plogwriter) :
   //     UpdateDetector(pupdatekeeper, pupdatelistener), m_pcriticalsectionFramebuffer(pcriticalsectionFramebuffer), m_lastCounter(0), m_plogwriter(plogwriter)
   // {
   //    m_pmirrordriverclient = new MirrorDriverClient(m_plogwriter);
   //    initFramebuffer();
   // }
   MirrorScreenDriver::MirrorScreenDriver() :
        m_pcriticalsectionFramebuffer(nullptr), m_lastCounter(0)
   {
      // m_pmirrordriverclient = new MirrorDriverClient(m_plogwriter);
      // initFramebuffer();
   }

   MirrorScreenDriver::~MirrorScreenDriver()
   {
      terminateDetection();
      // if (m_pmirrordriverclient)
      // {
      //    delete m_pmirrordriverclient;
      // }
   }

   void MirrorScreenDriver::initialize_mirror_screen_driver(::remoting::UpdateKeeper * pupdatekeeper, ::remoting::UpdateListener * pupdatelistener,
                                       lockable_critical_section *pcriticalsectionFramebuffer, ::subsystem::LogWriter * plogwriter)// :
    //UpdateDetector(pupdatekeeper, pupdatelistener), m_pcriticalsectionFramebuffer(pcriticalsectionFramebuffer), m_lastCounter(0), m_plogwriter(plogwriter)
   {
      initialize_update_detector(pupdatekeeper, pupdatelistener);
      m_pcriticalsectionFramebuffer = pcriticalsectionFramebuffer;
      m_pmirrordriverclient = new MirrorDriverClient(m_plogwriter);
      initFramebuffer();
   }


   void MirrorScreenDriver::executeDetection() { resumeThread(); }

   void MirrorScreenDriver::terminateDetection()
   {
      setThreadToFinish();
      waitThreadToFinish();
   }

   void MirrorScreenDriver::initFramebuffer()
   {
      ::i32_size size = m_pmirrordriverclient->getDimension();
      ::innate_subsystem::PixelFormat pixelformat = m_pmirrordriverclient->getPixelFormat();

      m_pframebuffer->setProperties(size, pixelformat);
   }

   ::i32_size MirrorScreenDriver::getScreenDimension() { return m_pframebuffer->getDimension(); }

   ::innate_subsystem::Framebuffer *MirrorScreenDriver::getScreenBuffer() { return m_pframebuffer; }

   bool MirrorScreenDriver::grab(const ::i32_rectangle & rectangle)
   {
      critical_section_lock al(m_pcriticalsectionFramebuffer);

      if (m_pmirrordriverclient == 0)
      {
         throw ::subsystem::Exception("Mirror driver client didn't initilized.");
      }

      ::i32_rectangle rectangleFramebuffer = m_pframebuffer->getDimension();
      ::i32_rectangle croppedRect;
      if (rectangle != 0)
      {
         croppedRect = rectangle.intersection(rectangleFramebuffer);
      }
      else
      {
         croppedRect = rectangleFramebuffer;
      }

      char *dstPtr = (char *)m_pframebuffer->getBufferPtr(croppedRect.left, croppedRect.top);
      size_t offset = dstPtr - (char *)m_pframebuffer->getBuffer();
      char *srcPtr = (char *)m_pmirrordriverclient->getBuffer();
      if (srcPtr == 0)
      {
         return false;
      }
      srcPtr += offset;

      size_t count = croppedRect.width() * m_pframebuffer->getBytesPerPixel();
      size_t stride = m_pframebuffer->getBytesPerRow();

      for (size_t i = croppedRect.top; i < croppedRect.bottom; i++, dstPtr += stride, srcPtr += stride)
      {
         memcpy(dstPtr, srcPtr, count);
      }
      return true;
   }

   bool MirrorScreenDriver::getPropertiesChanged()
   {
      critical_section_lock al(m_pcriticalsectionFramebuffer);
      if (m_pmirrordriverclient != 0)
      {
         return m_pmirrordriverclient->getPropertiesChanged();
      }
      else
      {
         throw ::subsystem::Exception("Mirror driver client didn't initilized.");
      }
   }

   bool MirrorScreenDriver::getScreenSizeChanged()
   {
      critical_section_lock al(m_pcriticalsectionFramebuffer);
      if (m_pmirrordriverclient != 0)
      {
         return m_pmirrordriverclient->getScreenSizeChanged();
      }
      else
      {
         throw ::subsystem::Exception("Mirror driver client didn't initilized.");
      }
   }

   bool MirrorScreenDriver::applyNewProperties()
   {
      critical_section_lock al(m_pcriticalsectionFramebuffer);

      delete m_pmirrordriverclient;
      m_pmirrordriverclient = 0;
      m_pmirrordriverclient = new MirrorDriverClient(m_plogwriter);

      ::i32_size sizeNew = m_pmirrordriverclient->getDimension();
      ::innate_subsystem::PixelFormat pixelformat = m_pmirrordriverclient->getPixelFormat();
      m_pframebuffer->setProperties(sizeNew, pixelformat);
      m_lastCounter = 0;

      return true;
   }

   void MirrorScreenDriver::startUpdateSearching() { resumeThread(); }

   void MirrorScreenDriver::onThreadMain()
   {
      ::remoting::Region m_regionChanged;
      ::i32_rectangle rectangleChanged;
      unsigned long currentCounter = 0;

      while (!isThreadTerminating())
      {
         m_updateTimeout.wait(20 * 1_ms);

         {
            critical_section_lock al(m_pcriticalsectionFramebuffer);
            if (m_pmirrordriverclient != 0)
            {
               auto changesBuf = (CHANGES_BUF *) m_pmirrordriverclient->getChangesBuf();
               if (changesBuf != 0)
               {
                  currentCounter = changesBuf->counter;
                  for (unsigned long i = m_lastCounter; i != currentCounter; i++, i %= MAXCHANGES_BUF)
                  {
                     rectangleChanged = changesBuf->pointrect[i].rect;
                     if (rectangleChanged.is_null_or_positive())
                     {
                        m_regionChanged.addRect(rectangleChanged);
                     }
                  }

                  m_pupdatekeeper->addChangedRegion(m_regionChanged);
                  m_lastCounter = currentCounter;
               }
            }
         }

         if (!m_regionChanged.is_empty())
         {
            doUpdate();
            m_regionChanged.clear();
         }
      }
   }

   void MirrorScreenDriver::onTermThread() { m_updateTimeout.set_happening(); }


} // namespace remoting_windows
 


