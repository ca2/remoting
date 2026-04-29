// Copyright (C) 2012 GlavSoft LLC.
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
#include "subsystem/platform/Exception.h"
#include "subsystem/framebuffer/StandardPixelFormatFactory.h"
#include "remoting/remoting_windows/desktop/WinDxCriticalException.h"
#include "remoting/remoting_windows/desktop/WinDxRecoverableException.h"
//#include aaa_<crtdbg.h>
#include "subsystem/node/Screen.h"

#include "remoting/remoting_windows/desktop/WinDxgiOutput.h"

// The header including of this cpp file must be at last place to avoid build conflicts.
#include "remoting/remoting_windows/desktop/Win8ScreenDriverImpl.h"

namespace remoting_windows
{

   //
   // Win8ScreenDriverImpl::Win8ScreenDriverImpl(::subsystem::LogWriter * plogwriter, ::remoting::UpdateKeeper *pupdatekeeper,
   //                                            lockable_critical_section *pcriticalsectionFramebuffer, ::remoting::UpdateListener *pupdatelistener,
   //                                            bool detectionEnabled) :
   //     m_pupdatekeeper(pupdatekeeper), m_pupdatelistener = pupdatelistener;, m_plogwriter(plogwriter), m_curTimeStamp(0),
   //     m_hasCriticalError(false), m_hasRecoverableError(false), m_detectionEnabled(detectionEnabled)
   // {
   //    resume();
   //    m_plogwriter->debug("Win8ScreenDriverImpl:: waiting for DXGI init");
   //    m_happeningInit.wait();
   //
   //    if (m_hasCriticalError)
   //    {
   //       m_plogwriter->debug("Win8ScreenDriverImpl init critical error");
   //       terminate();
   //       wait();
   //       throw ::subsystem::Exception("Win8ScreenDriverImpl can't be successfully initialized");
   //    }
   //
   //
   //    // Checking that builded dimension is equal to virtual desktop dimension.
   //    ::int_size sizeBuilt = getScreenBuffer()->getDimension();
   //    Screen screen;
   //    ::int_size virtDimension = screen.getDesktopDimension();
   //    if (!sizeBuilt.isEqualTo(&virtDimension))
   //    {
   //       terminate();
   //       wait();
   //       throw ::subsystem::Exception("The builded screen dimension doesn't match to virtual screen dimension");
   //    }
   //    // At this point the screen driver has valid screen properties.
   // }


   Win8ScreenDriverImpl::Win8ScreenDriverImpl() :
//   m_pupdatekeeper(pupdatekeeper), m_pupdatelistener = pupdatelistener;, m_plogwriter(plogwriter),
   m_curTimeStamp(0),m_hasCriticalError(false), m_hasRecoverableError(false), m_detectionEnabled(false)

   {

   }


   Win8ScreenDriverImpl::~Win8ScreenDriverImpl()
   {

      terminateDetection();
      terminate();
      int activeResult = (int)isActive();
      int waitResult = (int)wait();
      m_plogwriter->debug("Win8ScreenDriverImpl::activeResult = {}", activeResult);
      m_plogwriter->debug("Win8ScreenDriverImpl::waitResult = {}", waitResult);
   }


   void Win8ScreenDriverImpl::initialize_win8_screen_driver_impl(::subsystem::LogWriter * plogwriter, ::remoting::UpdateKeeper *pupdatekeeper,
                                           lockable_critical_section *pcriticalsectionFramebuffer, ::remoting::UpdateListener *pupdatelistener,
                                           bool detectionEnabled)
    {
       m_pupdatekeeper = pupdatekeeper;
       m_pupdatelistener = pupdatelistener;
       m_plogwriter = plogwriter;
       m_detectionEnabled = detectionEnabled;
       //m_pupdatekeeper(pupdatekeeper), m_pupdatelistener = pupdatelistener;, m_plogwriter(plogwriter), m_curTimeStamp(0),
       //m_hasCriticalError(false), m_hasRecoverableError(false), m_detectionEnabled(detectionEnabled)

          resume();
       m_plogwriter->debug("Win8ScreenDriverImpl:: waiting for DXGI init");
       m_happeningInit.wait();

       if (m_hasCriticalError)
       {
          m_plogwriter->debug("Win8ScreenDriverImpl init critical error");
          terminate();
          wait();
          throw ::subsystem::Exception("Win8ScreenDriverImpl can't be successfully initialized");
       }


       // Checking that builded dimension is equal to virtual desktop dimension.
       ::int_size sizeBuilt = getScreenBuffer()->getDimension();
       ::subsystem::Screen screen;
       ::int_size virtDimension = screen.getDesktopDimension();
       if (sizeBuilt != virtDimension)
       {
          terminate();
          wait();
          throw ::subsystem::Exception("The builded screen dimension doesn't match to virtual screen dimension");
       }
       // At this point the screen driver has valid screen properties.
    }

   void Win8ScreenDriverImpl::executeDetection()
   {
      // Detection is already executed in the Win8 model, instead of we should enable it
      m_detectionEnabled = true;
   }

   void Win8ScreenDriverImpl::terminateDetection()
   {
      m_plogwriter->debug("Stop Win8DeskDuplication");
      m_deskDuplThreadBundle.destroyAllThreads();
      m_detectionEnabled = false;
   }

   ::innate_subsystem::Framebuffer *Win8ScreenDriverImpl::getScreenBuffer() { return m_pframebufferProperty; }

   void Win8ScreenDriverImpl::initDxgi()
   {
      m_plogwriter->debug("Creating of D3D11Device");
      WinD3D11Device d3D11Device(m_plogwriter);
      m_plogwriter->debug("Quering Interface for IDXGIDevice");
      WinDxgiDevice dxgiDevice(&d3D11Device);
      m_plogwriter->debug("Getting Parent for IDXGIAdapter");
      WinDxgiAdapter dxgiAdapter(&dxgiDevice);

      ::remoting::Region virtDeskRegion;
      m_plogwriter->debug("Try to enumerate dxgi outputs");
      ::pointer_array<WinDxgiOutput> dxgiOutputArray;
      ::int_rectangle_array_base deskCoordArray;
      unsigned int iOutput = 0;
      try
      {
         for (iOutput = 0; iOutput < 65535; iOutput++)
         {
            auto pdxgiOutput = allocateø WinDxgiOutput(&dxgiAdapter, iOutput);
            if (pdxgiOutput->isAttachedtoDesktop())
            {
               dxgiOutputArray.add(pdxgiOutput);
               ::int_rectangle deskCoord = pdxgiOutput->getDesktopCoordinates();
               deskCoordArray.add(deskCoord);
               virtDeskRegion.addRect(deskCoord);
            }
         }
      }
      catch (WinDxRecoverableException &)
      {
         m_plogwriter->debug("Reached the end of dxgi output ::list_base with iOutput = %u", iOutput);
         // End of output ::list_base.
      }
      m_plogwriter->debug("We have {} dxgi output(s) connected", dxgiOutputArray.size());

      // Check that all outputs for the virtual screen are found (in case two or more
      // hardware graphic interfaces are used). It's better to avoid using buggy
      // Desktop Duplication API here rather than getting the wrong pframebuffer->
      ::subsystem::Screen screen;
      if (screen.getVisibleMonitorCount() != dxgiOutputArray.size())
      {
         throw ::subsystem::Exception("Unable get all DXGI outputs for virtual screen");
      }

      ::innate_subsystem::PixelFormat pixelformat = getDxPixelFormat();
      ::int_rectangle rectangleVirtDeskBound = virtDeskRegion.getBounds();
      m_pframebufferProperty->setProperties(rectangleVirtDeskBound, pixelformat);
      m_pframebufferProperty->setColor(0, 0, 0);

      for (size_t iDxgiOutput = 0; iDxgiOutput < dxgiOutputArray.size(); iDxgiOutput++)
      {
         deskCoordArray[iDxgiOutput].offset(-rectangleVirtDeskBound.left, -rectangleVirtDeskBound.top);
      }
      size_t threadsNum = m_deskDuplThreadBundle.Size();
      if (threadsNum > 12)
         threadsNum = 12;
      DWORD millis = 1 << threadsNum; // delay up to 4 seconds if there are threads waiting to delete
      sleep(millis);
      Thread *thread = new Win8DeskDuplication(m_pframebufferProperty, deskCoordArray, &m_win8CursorShape, &m_curTimeStamp,
                                               &m_cursorMutex, this, dxgiOutputArray, m_plogwriter);
      DWORD id = thread->getThreadId();
      m_plogwriter->debug("Created a new Win8DeskDuplication with ID: ({})", id);
      m_deskDuplThreadBundle.addThread(thread);
   }

   void Win8ScreenDriverImpl::execute()
   {
      try
      {
         initDxgi();
      }
      catch (WinDxRecoverableException &e)
      {
         m_plogwriter->error("Win8ScreenDriverImpl:: Catched WinDxRecoverableException: {}, (%x)", e.get_message(),
                             (int)e.getErrorCode());
         m_hasRecoverableError = true;
      }
      catch (WinDxCriticalException &e)
      {
         m_plogwriter->error("Win8ScreenDriverImpl:: Catched WinDxCriticalException: {}, (%x)", e.get_message(),
                             (int)e.getErrorCode());
         m_hasCriticalError = true;
      }
      catch (::exception &e)
      {
         m_plogwriter->error("Catched ::subsystem::Exception in the Win8ScreenDriverImpl::execute() function: {}."
                             " The exception will consider as critical",
                             e.get_message());
         m_hasCriticalError = true;
      }
      m_happeningInit.set_happening();

      while (!isTerminating() && isValid())
      {
         m_happeningError.wait();
      }

      if (!isValid())
      {
         m_plogwriter->error("Win8ScreenDriverImpl has an invalid state. The invalid state can be"
                             " a part of screen propery changes. An update signal will be generated"
                             " as a screen size changed signal.");
         m_pupdatekeeper->setScreenSizeChanged();
         m_pupdatelistener->onUpdate();
      }
      terminateDetection();
   }

   void Win8ScreenDriverImpl::onTerminate() { m_happeningError.set_happening(); }

   void Win8ScreenDriverImpl::onFramebufferUpdate(const ::remoting::Region & regionChanged)
   {
      if (m_detectionEnabled)
      {
         m_pupdatekeeper->addChangedRegion(regionChanged);
         m_pupdatelistener->onUpdate();
      }
   }

   void Win8ScreenDriverImpl::onCopyRect(const ::int_rectangle &rectangleTarget, int srcX, int srcY)
   {
      if (m_detectionEnabled)
      {
         ::int_point pointSource(srcX, srcY);
         m_pupdatekeeper->addCopyRect(rectangleTarget, pointSource);
         m_pupdatelistener->onUpdate();
      }
   }

   void Win8ScreenDriverImpl::onCursorPositionChanged(int x, int y)
   {
      critical_section_lock al(&m_cursorMutex);
      ::int_point pointNewPosition(x, y);
      if (m_latestCursorPos != pointNewPosition)
      {
         m_latestCursorPos = pointNewPosition;
         m_pupdatekeeper->setCursorPos(pointNewPosition);
         m_pupdatelistener->onUpdate();
      }
   }

   void Win8ScreenDriverImpl::onCursorShapeChanged()
   {
      m_pupdatekeeper->setCursorShapeChanged();
      m_pupdatelistener->onUpdate();
   }

   void Win8ScreenDriverImpl::onRecoverableError(const ::scoped_string &scopedstrReason)
   {
      m_plogwriter->error("Win8ScreenDriverImpl catch an recoverable error with reason: {}", scopedstrReason);
      m_hasRecoverableError = true;
      m_happeningError.set_happening();
   }

   void Win8ScreenDriverImpl::onCriticalError(const ::scoped_string &scopedstrReason)
   {
      m_plogwriter->error("Win8ScreenDriverImpl catch an critical error with reason: {}", scopedstrReason);
      m_hasCriticalError = true;
      m_happeningError.set_happening();
   }

   bool Win8ScreenDriverImpl::grabFb(const ::int_rectangle & rectangle) { return isValid(); }

   bool Win8ScreenDriverImpl::isValid() { return !m_hasRecoverableError && !m_hasCriticalError; }

   ::innate_subsystem::PixelFormat Win8ScreenDriverImpl::getDxPixelFormat() const
   {
      return ::innate_subsystem::StandardPixelFormatFactory::create32bppPixelFormat();
   }

   void Win8ScreenDriverImpl::updateCursorShape(::remoting::CursorShape *pcursorshapeTarget)
   {
      critical_section_lock al(&m_cursorMutex);
      pcursorshapeTarget->clone(m_win8CursorShape.getCursorShape());
   }

   ::int_point Win8ScreenDriverImpl::getCursorPosition()
   {
      critical_section_lock al(&m_cursorMutex);
      return m_latestCursorPos;
   }


} // namespace remoting_windows
