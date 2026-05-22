// Copyright (C) 2008,2009,2010,2011,2012 GlavSoft LLC.
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

#pragma once


#include "UpdateContainer.h"
#include "UpdateKeeper.h"
#include "UpdateFilter.h"
#include "ScreenGrabber.h"
//#include "windows/WindowsCursorShapeGrabber.h"
#include "innate_subsystem/framebuffer/Framebuffer.h"
//#include "subsystem/thread/lockable_critical_section.h"
#include "UpdateListener.h"
#include "UpdateDetector.h"
#include "CopyRectDetector.h"
#include "remoting/remoting_rtc/desktop_ipc/BlockingGate.h"


namespace remoting
{

   class CLASS_DECL_REMOTING UpdateHandler :
   virtual public ::particle
   {
   public:
      UpdateHandler();
      virtual ~UpdateHandler(void);

      // The extract() function fills in a UpdateContainer object.
      // Also, if screen properties (such as resolution, pixel format)
      // has changed the function reconfigures Framebuffers. The
      // reconfiguration posible the only one function.
      //
      // If screen size or pixel format have changed the copied region
      // and the changed region will be cleaned, Framebuffers will
      // be reinitialized. Also, if screen size have changed the
      // m_bScreenSizeChanged flag will be set to true. In the next
      // time call of this function no additional information about
      // these changes will present.

      // Parameters:
      //   updatecontainer - pointer to a UpdateContainer object that will be filled
      virtual UpdateContainer extract() = 0;

      // This function unconventionally set to update pending of the frame buffer
      // in the next time call of the extract() function. All found changes
      // saves to the m_regionChanged and m_regionCopied.
      virtual void setFullUpdateRequested(const ::remoting_rtc::Region & region) = 0;

      // Checking a region for updates.
      // Return:
      //   true - if updates presents,
      //   false - if not.
      virtual bool checkForUpdates(::remoting_rtc::Region & region) = 0;

      // Set a region excluded from the region that updates detects.
      // regionExcluded will never be present in m_regionChanged or m_regionCopied.
      virtual void setExcludedRegion(const ::remoting_rtc::Region & regionExcluded) = 0;

      // The function provides access to ::innate_subsystem::Framebuffer data.
      // The data usage be able until next extract() function call.
      // Return:
      //   constant pointer to the ::innate_subsystem::Framebuffer object.
      const ::innate_subsystem::Framebuffer *getFramebuffer() const { return m_pframebufferBackup; }
      const ::remoting_rtc::CursorShape *getCursorShape() const { return &m_cursorshape; }
      // This function for asynchronous access to frame buffer properties
      // (dimension and pixel format)
      void getFramebufferProp(::i32_size & size, ::innate_subsystem::PixelFormat & pixelformat)
      {
         critical_section_lock al(&m_criticalsectionFramebuffer);
         size = m_pframebufferBackup->getDimension();
         pixelformat = m_pframebufferBackup->getPixelFormat();
      }

      ::i32_size getFramebufferDimension()
      {
         critical_section_lock al(&m_criticalsectionFramebuffer);
         return m_pframebufferBackup->getDimension();
      }

      ::innate_subsystem::PixelFormat getFramebufferPixelFormat(::i32_size & size, ::innate_subsystem::PixelFormat & pixelformat)
      {
         critical_section_lock al(&m_criticalsectionFramebuffer);
         return m_pframebufferBackup->getPixelFormat();
      }

      void initFramebuffer(const ::innate_subsystem::Framebuffer *newFb);

      virtual bool updateExternalFramebuffer(::innate_subsystem::Framebuffer *pframebuffer, const ::remoting_rtc::Region & region,
                                             const ::i32_rectangle &rectangleViewport);

      // FIXME: It's no good idea to place this function to here.
      // Because it uses only for the UpdateHandlerClient class.
      virtual void sendInit(::remoting_rtc::BlockingGate *pblockinggate) {}

   protected:
      virtual bool updateExternalFramebuffer(::innate_subsystem::Framebuffer *pframebufferTarget,
                                             ::innate_subsystem::Framebuffer *pframebufferSource, const ::remoting_rtc::Region & region,
                                             const ::i32_rectangle &rectangleViewport);

      ::pointer < ::innate_subsystem::Framebuffer > m_pframebufferBackup;
      lockable_critical_section m_criticalsectionFramebuffer;

      // m_cursorshape not thread safed
      ::remoting_rtc::CursorShape m_cursorshape;
   };


} // namespace remoting






