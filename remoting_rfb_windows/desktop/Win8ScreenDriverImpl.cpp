// Copyright (C) 2012 GlavSoft LLC.
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
#include "subsystem/platform/Exception.h"
#include "subsystem/framebuffer/StandardPixelFormatFactory.h"
#include "remoting/remoting_rfb_windows/desktop/WinDxCriticalException.h"
#include "remoting/remoting_rfb_windows/desktop/WinDxRecoverableException.h"
//#include aaa_<crtdbg.h>
#include "subsystem/node/Screen.h"

#include "remoting/remoting_rfb_windows/desktop/aaa_WinDxgiOutput.h"
#include "remoting/remoting_rfb_windows/desktop/D3D11Device.h"
// The header including of this cpp file must be at last place to avoid build conflicts.
#include "remoting/remoting_rfb_windows/desktop/Win8ScreenDriverImpl.h"
::string dxgi_d3d11_diagnostic_for_duplicate_output1();
::string dxgi_d3d11_diagnostic_for_duplicate_output2(UINT outputIndex, IDXGIOutput *output);

namespace remoting_rfb_windows
{

   //
   // Win8ScreenDriverImpl::Win8ScreenDriverImpl(::subsystem::LogWriter * plogwriter, ::remoting_rfb::UpdateKeeper *pupdatekeeper,
   //                                            lockable_critical_section *pcriticalsectionFramebuffer, ::remoting_rfb::UpdateListener *pupdatelistener,
   //                                            bool detectionEnabled) :
   //     m_pupdatekeeper(pupdatekeeper), m_pupdatelistener = pupdatelistener;, m_plogwriter(plogwriter), m_curTimeStamp(0),
   //     m_hasCriticalError(false), m_hasRecoverableError(false), m_detectionEnabled(detectionEnabled)
   // {
   //    resumeThread();
   //    m_plogwriter->debug("Win8ScreenDriverImpl:: waiting for DXGI init");
   //    m_happeningInit.waitThreadToFinish();
   //
   //    if (m_hasCriticalError)
   //    {
   //       m_plogwriter->debug("Win8ScreenDriverImpl init critical error");
   //       setThreadToFinish();
   //       waitThreadToFinish();
   //       throw ::subsystem::Exception("Win8ScreenDriverImpl can't be successfully initialized");
   //    }
   //
   //
   //    // Checking that builded dimension is equal to virtual desktop dimension.
   //    ::i32_size sizeBuilt = getScreenBuffer()->getDimension();
   //    Screen screen;
   //    ::i32_size virtDimension = screen.getDesktopDimension();
   //    if (!sizeBuilt.isEqualTo(&virtDimension))
   //    {
   //       setThreadToFinish();
   //       waitThreadToFinish();
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
      //setThreadToFinish();
      ::i32 activeResult = (::i32)isThreadActive();
      //::i32 waitResult = (::i32)waitThreadToFinish();
      //waitThreadToFinish();
      m_plogwriter->debug("Win8ScreenDriverImpl::activeResult = {}", activeResult);
      //m_plogwriter->debug("Win8ScreenDriverImpl::waitResult = {}", waitResult);
   }


   void Win8ScreenDriverImpl::initialize_win8_screen_driver_impl(::subsystem::LogWriter * plogwriter, ::remoting_rfb::UpdateKeeper *pupdatekeeper,
                                           lockable_critical_section *pcriticalsectionFramebuffer, ::remoting_rfb::UpdateListener *pupdatelistener,
                                           bool detectionEnabled)
    {
       m_pupdatekeeper = pupdatekeeper;
       m_pupdatelistener = pupdatelistener;
       m_plogwriter = plogwriter;
       m_detectionEnabled = detectionEnabled;
       //m_pupdatekeeper(pupdatekeeper), m_pupdatelistener = pupdatelistener;, m_plogwriter(plogwriter), m_curTimeStamp(0),
       //m_hasCriticalError(false), m_hasRecoverableError(false), m_detectionEnabled(detectionEnabled)

          resumeThread();
       m_plogwriter->debug("Win8ScreenDriverImpl:: waiting for DXGI init");
       m_happeningInit.wait();

       if (m_hasCriticalError)
       {
          m_plogwriter->debug("Win8ScreenDriverImpl init critical error");
          setThreadToFinish();
          waitThreadToFinish();
          throw ::subsystem::Exception("Win8ScreenDriverImpl can't be successfully initialized");
       }


       // Checking that builded dimension is equal to virtual desktop dimension.
       ::i32_size sizeBuilt = getScreenBuffer()->getDimension();
       ::subsystem::Screen screen;
       ::i32_size virtDimension = screen.getDesktopDimension();
       if (sizeBuilt != virtDimension)
       {
          setThreadToFinish();
          waitThreadToFinish();
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
      //m_deskDuplThreadBundle.destroyAllThreads();
      m_detectionEnabled = false;
   }

   ::innate_subsystem::Framebuffer *Win8ScreenDriverImpl::getScreenBuffer() { return m_pframebufferProperty; }

   void Win8ScreenDriverImpl::initDxgi()
   {

      information() << dxgi_d3d11_diagnostic_for_duplicate_output1();

      m_plogwriter->debug("Creating of D3D11Device");
      //WinD3D11Device d3D11Device(m_plogwriter);
      m_pd3d11device = allocateø D3D11Device(m_plogwriter);
      m_plogwriter->debug("Quering Interface for IDXGIDevice");
      //WinDxgiDevice dxgiDevice(&d3D11Device);
      //m_pd3d11device->m_pd3d11device.as(m_pdxgidevice);
      //m_pdxgiDevice = allocateø WinDxgiDevice(m_pd3D11Device);
      m_plogwriter->debug("Getting Parent for IDXGIAdapter");
      //WinDxgiAdapter dxgiAdapter(&dxgiDevice);
      //m_pdxgidevice->GetParent(__interface_of(m_pdxgiadapter));
      //m_pdxgiAdapter = allocateø WinDxgiAdapter(m_pdxgiDevice);
      ::remoting_rfb::Region virtDeskRegion;
      m_plogwriter->debug("Try to enumerate dxgi outputs");
      ::array_base<::comptr<IDXGIOutput > > dxgioutputa;
      ::i32_rectangle_array_base deskCoordArray;
      ::u32 iOutput = 0;
      //try
      //{
         for (iOutput = 0; iOutput < 65535; iOutput++)
         {

            comptr < IDXGIOutput > pdxgioutput;

            //auto hr = m_pdxgiAdapter->getDxgiOutput(iOutput, &pdxgioutput);

            HRESULT hr = m_pd3d11device->m_pdxgiadapter1->EnumOutputs(iOutput, &pdxgioutput);
            //return hr;
            if (hr == DXGI_ERROR_NOT_FOUND)
            {

               break;

            }
            else if (FAILED(hr))
            {

               break;

            }

            dxgi_d3d11_diagnostic_for_duplicate_output2(iOutput, pdxgioutput);

            DXGI_OUTPUT_DESC desc{};

            if (FAILED(pdxgioutput->GetDesc(&desc)))
            {

               throw WinDxCriticalException("Can't get output description", hr);

            }

            //auto pdxgiOutput = allocateø WinDxgiOutput(m_pdxgiAdapter, iOutput);
            if (desc.AttachedToDesktop)
            {
               dxgioutputa.add(pdxgioutput);
               ::i32_rectangle deskCoord = desc.DesktopCoordinates;
               deskCoordArray.add(deskCoord);
               virtDeskRegion.addRect(deskCoord);
            }
         }
      //}
      //catch (WinDxRecoverableException &)
      //{
        // m_plogwriter->debug("Reached the end of dxgi output ::list_base with iOutput = %u", iOutput);
         // End of output ::list_base.
      //}
      m_plogwriter->debug("We have {} dxgi output(s) connected", dxgioutputa.size());

      // Check that all outputs for the virtual screen are found (in case two or more
      // hardware graphic interfaces are used). It's better to avoid using buggy
      // Desktop Duplication API here rather than getting the wrong pframebuffer->
      ::subsystem::Screen screen;
      auto iScreenVisibleMonitorCount = screen.getVisibleMonitorCount();
      auto iDxgiOutputCount = dxgioutputa.size();
      if (iScreenVisibleMonitorCount != iDxgiOutputCount)
      {
         throw ::subsystem::Exception("Unable get all DXGI outputs for virtual screen");
      }

      if (iDxgiOutputCount <= 0)
      {
         throw ::subsystem::Exception("No screens to get DXGI output from");
      }

      construct_newø(m_pframebufferProperty);


      ::innate_subsystem::PixelFormat pixelformat = getDxPixelFormat();
      ::i32_rectangle rectangleVirtDeskBound = virtDeskRegion.getBounds();
      m_pframebufferProperty->setProperties(rectangleVirtDeskBound, pixelformat);
      m_pframebufferProperty->setColor(0, 0, 0);

      for (size_t iDxgiOutput = 0; iDxgiOutput < dxgioutputa.size(); iDxgiOutput++)
      {
         deskCoordArray[iDxgiOutput].offset(-rectangleVirtDeskBound.left, -rectangleVirtDeskBound.top);
      }
      //size_t threadsNum = m_deskDuplThreadBundle.Size();
      //if (threadsNum > 12)
      //   threadsNum = 12;
      //else if (threadsNum <= 0)
      //   threadsNum = 1;
      ////DWORD millis = 1 << threadsNum; // delay up to 4 seconds if there are threads waiting to delete
      //auto timeSlice = 4_s / 12;
      //preempt(timeSlice * threadsNum);
      //preempt(4_s);
      auto pdeskduplication = allocateø Win8DeskDuplication(m_pframebufferProperty, deskCoordArray, &m_win8CursorShape, &m_curTimeStamp,
                                               &m_cursorMutex, this,m_pd3d11device, dxgioutputa, m_plogwriter);
      //DWORD id = pthread->getThreadId();
      //m_plogwriter->debug("Created a new Win8DeskDuplication with ID: ({})", id);
      //m_deskDuplThreadBundle.addThread(pthread);
      m_pdeskduplication = pdeskduplication;
   }

   void Win8ScreenDriverImpl::onThreadMain()
   {
      try
      {
         initDxgi();
      }
      catch (WinDxRecoverableException &e)
      {
         m_plogwriter->error("Win8ScreenDriverImpl:: Catched WinDxRecoverableException: {}, (%x)", e.get_message(),
                             (::i32)e.getErrorCode());
         m_hasRecoverableError = true;
      }
      catch (WinDxCriticalException &e)
      {
         m_plogwriter->error("Win8ScreenDriverImpl:: Catched WinDxCriticalException: {}, (%x)", e.get_message(),
                             (::i32)e.getErrorCode());
         m_hasCriticalError = true;
      }
      catch (::exception &e)
      {
         m_plogwriter->error("Catched ::subsystem::Exception in the Win8ScreenDriverImpl::onThreadMain() function: {}."
                             " The exception will consider as critical",
                             e.get_message());
         m_hasCriticalError = true;
      }
      m_happeningInit.set_happening();

      while (!isThreadTerminating() && isValid())
      {
         m_pdeskduplication->onRunStep();
         preempt(25_ms);
         //m_happeningError.wait();
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

   void Win8ScreenDriverImpl::onTermThread() { m_happeningError.set_happening(); }

   void Win8ScreenDriverImpl::onFramebufferUpdate(const ::remoting_rfb::Region & regionChanged)
   {
      if (m_detectionEnabled)
      {
         m_pupdatekeeper->addChangedRegion(regionChanged);
         m_pupdatelistener->onUpdate();
      }
   }

   void Win8ScreenDriverImpl::onCopyRect(const ::i32_rectangle &rectangleTarget, ::i32 srcX, ::i32 srcY)
   {
      if (m_detectionEnabled)
      {
         ::i32_point pointSource(srcX, srcY);
         m_pupdatekeeper->addCopyRect(rectangleTarget, pointSource);
         m_pupdatelistener->onUpdate();
      }
   }

   void Win8ScreenDriverImpl::onCursorPositionChanged(::i32 x, ::i32 y)
   {
      critical_section_lock al(&m_cursorMutex);
      ::i32_point pointNewPosition(x, y);
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

   bool Win8ScreenDriverImpl::grabFb(const ::i32_rectangle & rectangle) { return isValid(); }

   bool Win8ScreenDriverImpl::isValid() { return !m_hasRecoverableError && !m_hasCriticalError; }

   ::innate_subsystem::PixelFormat Win8ScreenDriverImpl::getDxPixelFormat() const
   {
      return ::innate_subsystem::StandardPixelFormatFactory::create32bppPixelFormat();
   }

   void Win8ScreenDriverImpl::updateCursorShape(::remoting_rfb::CursorShape *pcursorshapeTarget)
   {
      critical_section_lock al(&m_cursorMutex);
      pcursorshapeTarget->clone(m_win8CursorShape.getCursorShape());
   }

   ::i32_point Win8ScreenDriverImpl::getCursorPosition()
   {
      critical_section_lock al(&m_cursorMutex);
      return m_latestCursorPos;
   }


} // namespace remoting_rfb_windows
