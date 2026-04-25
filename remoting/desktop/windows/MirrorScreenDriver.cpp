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
#include "MirrorScreenDriver.h"
#include "subsystem/platform/Exception.h"

namespace remoting
{

   MirrorScreenDriver::MirrorScreenDriver(UpdateKeeper * pupdatekeeper, UpdateListener * pupdatelistener,
                                          critical_section *fbcritical_section, ::subsystem::LogWriter * plogwriter) :
       UpdateDetector(pupdatekeeper, pupdatelistener), m_fbMutex(fbcritical_section), m_lastCounter(0), m_plogwriter(plogwriter)
   {
      m_mirrorClient = new MirrorDriverClient(m_plogwriter);
      initFrameBuffer();
   }

   MirrorScreenDriver::~MirrorScreenDriver()
   {
      terminateDetection();
      if (m_mirrorClient)
      {
         delete m_mirrorClient;
      }
   }

   void MirrorScreenDriver::executeDetection() { resume(); }

   void MirrorScreenDriver::terminateDetection()
   {
      terminate();
      wait();
   }

   void MirrorScreenDriver::initFrameBuffer()
   {
      ::int_size size = m_mirrorClient->getDimension();
      ::innate_subsystem::PixelFormat pf = m_mirrorClient->getPixelFormat();

      m_pframebuffer->setProperties(&size, &pf);
   }

   ::int_size MirrorScreenDriver::getScreenDimension() { return m_pframebuffer->getDimension(); }

   ::innate_subsystem::FrameBuffer *MirrorScreenDriver::getScreenBuffer() { return &m_pframebuffer; }

   bool MirrorScreenDriver::grab(const ::int_rectangle &rect)
   {
      critical_section_lock al(m_fbMutex);

      if (m_mirrorClient == 0)
      {
         throw ::subsystem::Exception("Mirror driver client didn't initilized.");
      }

      ::int_rectangle fbRect = m_pframebuffer->getDimension();
      ::int_rectangle croppedRect;
      if (rect != 0)
      {
         croppedRect = rect.intersection(&fbRect);
      }
      else
      {
         croppedRect = fbRect;
      }

      char *dstPtr = (char *)m_pframebuffer->getBufferPtr(croppedRect.left, croppedRect.top);
      size_t offset = dstPtr - (char *)m_pframebuffer->getBuffer();
      char *srcPtr = (char *)m_mirrorClient->getBuffer();
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
      critical_section_lock al(m_fbMutex);
      if (m_mirrorClient != 0)
      {
         return m_mirrorClient->getPropertiesChanged();
      }
      else
      {
         throw ::subsystem::Exception("Mirror driver client didn't initilized.");
      }
   }

   bool MirrorScreenDriver::getScreenSizeChanged()
   {
      critical_section_lock al(m_fbMutex);
      if (m_mirrorClient != 0)
      {
         return m_mirrorClient->getScreenSizeChanged();
      }
      else
      {
         throw ::subsystem::Exception("Mirror driver client didn't initilized.");
      }
   }

   bool MirrorScreenDriver::applyNewProperties()
   {
      critical_section_lock al(m_fbMutex);

      delete m_mirrorClient;
      m_mirrorClient = 0;
      m_mirrorClient = new MirrorDriverClient(m_plogwriter);

      ::int_size newDim = m_mirrorClient->getDimension();
      ::innate_subsystem::PixelFormat pf = m_mirrorClient->getPixelFormat();
      m_pframebuffer->setProperties(&newDim, &pf);
      m_lastCounter = 0;

      return true;
   }

   void MirrorScreenDriver::startUpdateSearching() { resume(); }

   void MirrorScreenDriver::execute()
   {
      Region m_regionChanged;
      ::int_rectangle changedRect;
      unsigned long currentCounter = 0;

      while (!isTerminating())
      {
         m_updateTimeout.wait(20 * 1_ms);

         {
            critical_section_lock al(m_fbMutex);
            if (m_mirrorClient != 0)
            {
               CHANGES_BUF *changesBuf = m_mirrorClient->getChangesBuf();
               if (changesBuf != 0)
               {
                  currentCounter = changesBuf->counter;
                  for (unsigned long i = m_lastCounter; i != currentCounter; i++, i %= MAXCHANGES_BUF)
                  {
                     changedRect.fromWindowsRect(&changesBuf->pointrect[i].rect);
                     if (changedRect.isValid())
                     {
                        m_regionChanged.addRect(&changedRect);
                     }
                  }

                  m_pupdatekeeper->addChangedRegion(&m_regionChanged);
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

   void MirrorScreenDriver::onTerminate() { m_updateTimeout.set_happening(); }


} // namespace remoting
 


