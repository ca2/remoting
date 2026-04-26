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
#include "remoting/remoting_windows/desktop/WindowsScreenGrabber.h"


namespace remoting
{


   WindowsScreenGrabber::WindowsScreenGrabber(void) : m_destDC(NULL), m_screenDC(NULL), m_hbmDIB(NULL), m_hbmOld(NULL)
   {
      m_pserverconfig = m_pconfigurator->getServerConfig();
      setWorkRectDefault();
      resume();
      m_hasStartedSignal.wait();
   }

   WindowsScreenGrabber::~WindowsScreenGrabber(void)
   {
      closeDIBSection();
      terminate();
      wait();
   }

   bool WindowsScreenGrabber::applyNewProperties()
   {
      if (!applyNewPixelFormat() || !applyNewFullScreenRect() || !openDIBSection())
      {
         return false;
      }

      return true;
   }

   bool WindowsScreenGrabber::openDIBSection()
   {
      closeDIBSection();

      m_pframebufferWork->setBuffer(0);

      m_screenDC = GetDC(0);
      if (m_screenDC == NULL)
      {
         return false;
      }

      if (getPropertiesChanged())
      {
         return false;
      }

      Screen::BMI bmi;
      try
      {
         m_screen.getBMI(&bmi, m_screenDC);
      }
      catch (...)
      {
         return false;
      }

      ::innate_subsystem::PixelFormat pixelFormat = m_pframebufferWork->getPixelFormat();
      ::int_size workDim = m_pframebufferWork->getDimension();

      bmi.bmiHeader.biBitCount = pixelFormat.bitsPerPixel;
      bmi.bmiHeader.biWidth = workDim.cx;
      bmi.bmiHeader.biHeight = -workDim.cy;
      bmi.bmiHeader.biCompression = BI_BITFIELDS;
      bmi.red = pixelFormat.redMax << pixelFormat.redShift;
      bmi.green = pixelFormat.greenMax << pixelFormat.greenShift;
      bmi.blue = pixelFormat.blueMax << pixelFormat.blueShift;

      m_destDC = CreateCompatibleDC(NULL);
      if (m_destDC == NULL)
      {
         DeleteDC(m_screenDC);
         return false;
      }

      void *buffer;
      m_hbmDIB = CreateDIBSection(m_destDC, (BITMAPINFO *)&bmi, DIB_RGB_COLORS, &buffer, NULL, NULL);
      if (m_hbmDIB == 0)
      {
         DeleteDC(m_destDC);
         DeleteDC(m_screenDC);
         return false;
      }
      m_pframebufferWork->setBuffer(buffer);
      m_dibSectionDim = workDim;

      m_hbmOld = (HBITMAP)SelectObject(m_destDC, m_hbmDIB);

      return true;
   }

   bool WindowsScreenGrabber::closeDIBSection()
   {
      // Free resources
      SelectObject(m_destDC, m_hbmOld);

      DeleteObject(m_hbmDIB);
      m_hbmDIB = NULL;

      DeleteDC(m_destDC);
      m_destDC = NULL;

      DeleteDC(m_screenDC);
      m_screenDC = NULL;

      m_pframebufferWork->setBuffer(NULL);
      ;
      return true;
   }

   bool WindowsScreenGrabber::getPropertiesChanged()
   {
      // Check for changing
      if (getScreenSizeChanged() || getPixelFormatChanged())
      {
         return true;
      }

      return false;
   }

   bool WindowsScreenGrabber::getPixelFormatChanged()
   {
      m_screen.update();

      ::innate_subsystem::PixelFormat currentPF = m_screen.getPixelFormat();
      ::innate_subsystem::PixelFormat framebufferPF = m_pframebufferWork->getPixelFormat();

      return !framebufferPF.isEqualTo(&currentPF);
   }

   bool WindowsScreenGrabber::getScreenSizeChanged()
   {
      m_screen.update();

      ::int_rectangle screenRect = m_screen.getDesktopRect();
      int width = screenRect.width();
      int height = screenRect.height();

      if (width != m_rectangleFullScreen.width() || height != m_rectangleFullScreen.height())
      {
         return true;
      }

      int left = screenRect.left;
      int top = screenRect.top;

      if (left != m_rectangleFullScreen.left || top != m_rectangleFullScreen.top)
      {
         // In this case apply new properties automatically
         // and don't inform anybody.
         applyNewProperties();
      }

      return false;
   }

   bool WindowsScreenGrabber::applyNewPixelFormat()
   {
      m_screen.update();
      m_pframebufferWork->setEmptyPixelFmt(&m_screen.getPixelFormat());

      return true;
   }

   bool WindowsScreenGrabber::applyNewFullScreenRect()
   {
      m_screen.update();
      m_rectangleFullScreen = m_screen.getDesktopRect();
      setWorkRect(&m_rectangleFullScreen);

      return true;
   }

   bool WindowsScreenGrabber::grab(const ::int_rectangle & rectangle)
   {
      if (rectangle != NULL)
      {
         return grabByDIBSection(rectangle);
      }

      ::int_rectangle grabRect;
      ::int_size workDim = m_pframebufferWork->getDimension();
      // Set relative co-ordinates
      grabRect.left = 0;
      grabRect.top = 0;
      grabRect.set_width(workDim.cx);
      grabRect.set_height(workDim.cy);

      return grabByDIBSection(&grabRect);
   }

   bool WindowsScreenGrabber::grabByDIBSection(const ::int_rectangle & rectangle)
   {
      ::int_size workDim = m_pframebufferWork->getDimension();
      if (workDim.cx != m_dibSectionDim.cx || workDim.cy != m_dibSectionDim.cy)
      {
         if (!openDIBSection())
         {
            return false;
         }
      }

      DWORD bitBltFlag;
      if (m_pserverconfig->getGrabTransparentWindowsFlag())
      {
         bitBltFlag = SRCCOPY | CAPTUREBLT;
      }
      else
      {
         bitBltFlag = SRCCOPY;
      }

      if (BitBlt(m_destDC, rectangle.left, rectangle.top, rectangle.width(), rectangle.height(), m_screenDC,
                 rectangle.left + m_pointFramebufferOffset.x, rectangle.top + m_pointFramebufferOffset.y, bitBltFlag) == 0)
      {
         return false;
      }

      return !getPropertiesChanged();
   }

   void WindowsScreenGrabber::execute()
   {
      applyNewProperties();
      m_hasStartedSignal.set_happening();
      m_threadStopper.wait();
   }

   void WindowsScreenGrabber::onTerminate() { m_threadStopper.set_happening(); }


} // namespace remoting
 