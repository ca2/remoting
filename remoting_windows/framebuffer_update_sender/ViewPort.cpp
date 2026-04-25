// Copyright (C) 2009,2010,2011,2012 GlavSoft LLC.
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
#include "Viewport.h"
//#include "subsystem/thread/critical_section.h"
#include "subsystem/platform/BrokenHandleException.h"

namespace remoting
{


   Viewport::Viewport() :
   m_pdesktop(0), m_plogwriter(nullptr)
   {

   }

   // Viewport::Viewport(const ViewPortState *viewPortState, ::subsystem::LogWriter * plogwriter) :
   //     m_pdesktop(0), m_state(*viewPortState), m_plogwriter = plogwriter;
   // {
   // }




   Viewport::~Viewport() {}

      void Viewport::initialize_viewport(::subsystem::LogWriter *plogwriter)
   { m_plogwriter = m_plogwriter;
   }

   void Viewport::initialize_viewport(const ViewPortState *viewPortState, ::subsystem::LogWriter *plogwriter)
       
   {
      m_state = *viewPortState;
      
      m_plogwriter = plogwriter;
   }

   void Viewport::initDesktopInterface(Desktop *desktop) { m_pdesktop = desktop; }

   void Viewport::changeState(const ViewPortState *newState)
   {
      critical_section_lock al(&m_stateMutex);
      m_state = *newState;
   }

   void Viewport::update(const ::int_size &fbDimension)
   {
      critical_section_lock al(&m_stateMutex);

      ::int_rectangle rectangle;
      switch (m_state.m_mode)
      {
         case ViewPortState::APPLICATION:
            _ASSERT(m_pdesktop != 0);
            m_pdesktop->getApplicationRegion(m_state.m_processId, m_regionApp);
            // Also, the view port rectangle will be FULL_DESKTOP.
         case ViewPortState::FULL_DESKTOP:
            rectangle = fbDimension;
            break;
         case ViewPortState::PRIMARY_DISPLAY:
            _ASSERT(m_pdesktop != 0);
            m_pdesktop->getPrimaryDesktopCoords(rectangle);
            break;
         case ViewPortState::DISPLAY_NUMBER:
            _ASSERT(m_pdesktop != 0);
            m_pdesktop->getDisplayNumberCoords(rectangle, m_state.m_displayNumber);
            break;
         case ViewPortState::WINDOW_RECT:
            _ASSERT(m_pdesktop != 0);
            if (!m_state.m_windowIsResolved)
            {
               // Try resolve a window name to a hwnd.
               resolveWindowName();
            }
            if (m_state.m_windowIsResolved)
            {
               try
               {
                  m_pdesktop->getWindowCoords(m_state.m_operatingsystemwindow, rectangle);
               }
               catch (::subsystem::BrokenHandleException &e)
               {
                  m_plogwriter->error("Viewport::update: {}", e.get_message());
                  // Now hwnd is broken. This should be reflected in the viewport state.
                  m_state.unresolveHwnd();
               }
            }
            break;
         case ViewPortState::ARBITRARY_RECT:
            rectangle = m_state.m_arbitraryRect;
            _ASSERT(m_pdesktop != 0);
            m_pdesktop->getNormalizedRect(rectangle);
            break;
      }
      m_plogwriter->debug("View port coordinates: ({}, {} %dx{})", rectangle.left, rectangle.top, rectangle.width(), rectangle.height());
      // Constrain and save
      m_rectangle = rectangle.intersection(::int_rectangle(fbDimension));
      if (m_rectangle.width() < 0 || m_rectangle.height() < 0)
      {
         m_rectangle.Null();
      }
      m_plogwriter->debug(
         "Constrained (to the ::innate_subsystem::Framebuffer dimension) view port coordinates: ({}, {} %dx{})",
         rectangle.left, rectangle.top, rectangle.width(), rectangle.height());
   }

   ::int_rectangle Viewport::getViewport()
   {
      critical_section_lock al(&m_stateMutex);
      return m_rectangle;
   }

   void Viewport::resolveWindowName()
   {
      // Skip the resolving if have been passed little time.
      if (m_timeLatestHwndResolving.elapsed().integral_millisecond() > RESOLVING_PERIOD)
      {
         auto operatingsystemwindow = m_pdesktop->getWindowHandleByName(m_state.m_windowName);
         if (operatingsystemwindow.is_set())
         {
            m_state.setWindowHandle(operatingsystemwindow);
         }
         m_timeLatestHwndResolving.Now();
      }
   }

   bool Viewport::getOnlyApplication() { return m_state.m_mode == ViewPortState::APPLICATION; }

   unsigned int Viewport::getApplicationId() { return m_state.m_processId; }

   void Viewport::getApplicationRegion(Region & region) { region = m_regionApp; }


} // namespace remoting
 