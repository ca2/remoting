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
#include "remoting/remoting_windows/desktop/Win32ScreenDriverBaseImpl.h"
#include "subsystem/platform/Exception.h"


namespace remoting_windows
{

   // Win32ScreenDriverBaseImpl::Win32ScreenDriverBaseImpl(::remoting::UpdateKeeper * pupdatekeeper, ::remoting::UpdateListener * pupdatelistener,
   //                                                      lockable_critical_section *pcriticalsectionFramebuffer,
   //                                                      ::subsystem::LogWriter * plogwriter) :
   //     WinVideoRegionUpdaterImpl(plogwriter), m_pcriticalsectionFramebuffer(pcriticalsectionFramebuffer),
   //     m_pcursorpositiondetector(pupdatekeeper, pupdatelistener, log),
   //     m_pcursorshapedetector(pupdatekeeper, pupdatelistener, &m_pcursorshapegrabber, pcriticalsectionFramebuffer, log)
   // {
   // }

   Win32ScreenDriverBaseImpl::Win32ScreenDriverBaseImpl()
   {


   }

   Win32ScreenDriverBaseImpl::~Win32ScreenDriverBaseImpl() { terminateDetection(); }


   void Win32ScreenDriverBaseImpl::initialize_screen_driver(::remoting_node::Configurator * pconfigurator, ::remoting::UpdateKeeper * pupdatekeeper,
      ::remoting::UpdateListener * pupdatelistener,
                                        ::innate_subsystem::Framebuffer *pframebuffer,
                                                     lockable_critical_section *pcriticalsectionFramebuffer,
                                                     ::subsystem::LogWriter * plogwriter)
   {

      //WinVideoRegionUpdaterImpl::initialize_screen_driver(pupdatekeeper, pupdatelistener, pcriticalsectionFramebuffer, plogwriter);
    // WinVideoRegionUpdaterImpl(plogwriter), m_pcriticalsectionFramebuffer(pcriticalsectionFramebuffer),
    // m_pcursorpositiondetector(pupdatekeeper, pupdatelistener, log),
    // m_pcursorshapedetector(pupdatekeeper, pupdatelistener, &m_pcursorshapegrabber, pcriticalsectionFramebuffer, log)
      //{
      WinVideoRegionUpdaterImpl::initialize_screen_driver(pconfigurator, pupdatekeeper, pupdatelistener, pframebuffer, pcriticalsectionFramebuffer, plogwriter);
         raw_construct_newø(m_pcursorpositiondetector);
      m_pcursorpositiondetector->initialize_cursor_position_detector(pupdatekeeper, pupdatelistener, plogwriter);
      m_pcursorshapedetector = allocateø WindowsCursorShapeGrabber();
      m_pcursorshapedetector->initialize_cursor_shape_detector(pupdatekeeper, pupdatelistener, m_pcursorshapegrabber, pcriticalsectionFramebuffer, plogwriter);

      //}
   }

   void Win32ScreenDriverBaseImpl::executeDetection()
   {
      m_pcursorpositiondetector->resume();
      m_pcursorshapedetector->resume();
   }

   void Win32ScreenDriverBaseImpl::terminateDetection()
   {
      m_pcursorpositiondetector->terminate();
      m_pcursorshapedetector->terminate();

      m_pcursorpositiondetector->wait();
      m_pcursorshapedetector->wait();
   }

   lockable_critical_section *Win32ScreenDriverBaseImpl::framebuffer_critical_section() { return m_pcriticalsectionFramebuffer; }

   bool Win32ScreenDriverBaseImpl::grabCursorShape(const ::innate_subsystem::PixelFormat & pixelformat)
   {
      // Grabbing under the mutex avoid us from grab void cursor shape in time when the
      // shape hides until grabs screen.
      critical_section_lock al(m_pcriticalsectionFramebuffer);
      return m_pcursorshapegrabber->grab(pixelformat);
   }

   const ::remoting::CursorShape *Win32ScreenDriverBaseImpl::getCursorShape() { return m_pcursorshapegrabber->getCursorShape(); }

   ::i32_point Win32ScreenDriverBaseImpl::getCursorPosition()
   {
      critical_section_lock al(m_pcriticalsectionFramebuffer);
      return m_pcursorpositiondetector->getCursorPos();
   }

   void Win32ScreenDriverBaseImpl::getCopiedRegion(::i32_rectangle &rectangleCopy, ::i32_point & pointSource)
   {
      critical_section_lock al(m_pcriticalsectionFramebuffer);
      m_pcopyrectdetector->detectWindowMovements(rectangleCopy, pointSource);
   }


} // namespace remoting_windows



