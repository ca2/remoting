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




//#include "remoting/remoting/region/::i32_point.h"
#include "subsystem/framebuffer/PixelFormat.h"
#include "innate_subsystem/framebuffer/Framebuffer.h"

//
// An abstract interface for screen grabbing.
//

/*
  //
  // Usage example:
  //

  ScreenGrabber *pframebuffer;

  // Initialisation
  pframebuffer = new WindowsScreenGrabber;

  ::i32_rectangle rectangleGrab, workRect;
  workRect.setRect(100, 100, 500, 500);
  rectangleGrab.setRect(20, 20, 120, 120); // Relative to the workRect
  pframebuffer->setWorkRect(&workRect);

  // One-time grabbing
  while (!pframebuffer->grab(&rectangleGrab)) {
    if (pframebuffer->getPropertiesChanged()) { // Check desktop properties
      if (!pframebuffer->applyNewProperties()) {
        MessageBox(NULL, "Cannot apply new screen properties"), _T("Error", MB_ICONHAND);
        return 1;
      }
    } else {
      MessageBox(NULL, "Cannot grab screen"), _T("Error", MB_ICONHAND);
      return 1;
    }
  }
*/


namespace remoting
{


   class CLASS_DECL_REMOTING ScreenGrabber :
      virtual public ::particle
   {
   public:
      // Derived constructors will ensure that the getScreenBuffer() returns
      // a frame buffer with valid screen properties.
      ScreenGrabber(void);
      ~ScreenGrabber(void) override;

      /* Provides grabbing.
      Parameters:     *rectangle - Pointer to a ::i32_rectangle object with relative workRect coordinates.
      Return value:   true if success.
      */
      virtual bool grab(const ::i32_rectangle & rectangle = {}) = 0;

      virtual ::innate_subsystem::Framebuffer *getScreenBuffer() { return m_pframebufferWork; }
      virtual void setWorkRect(::i32_rectangle &workRect);
      virtual ::i32_rectangle getWorkRect() const
      {
         return ::i32_rectangle(m_pointFramebufferOffset.x, m_pointFramebufferOffset.y,
                                m_pframebufferWork->getDimension().cx + m_pointFramebufferOffset.x,
                                m_pframebufferWork->getDimension().cy + m_pointFramebufferOffset.y);
      }
      /* Provides read access to rectangular coordinates of the screen (desktop).*/
      virtual ::i32_rectangle getScreenRect() { return m_rectangleFullScreen; }

      // Checks screen(desktop) properties on changes
      inline virtual bool getPropertiesChanged() = 0;
      inline virtual bool getPixelFormatChanged() = 0;
      inline virtual bool getScreenSizeChanged() = 0;

      // Set new values of the WorkRect to default (to full screen rectangle coordinates)
      // and m_rectangleFullScreen if desktop properties has been changed.
      // Also m_pixelformat set to actual value.
      virtual bool applyNewProperties();

   //protected:
      virtual bool applyNewFullScreenRect() = 0;
      virtual bool applyNewPixelFormat() = 0;

      virtual bool setWorkRectDefault();

      ::i32_rectangle m_rectangleFullScreen;
      ::i32_point m_pointFramebufferOffset;

      ::pointer < ::innate_subsystem::Framebuffer > m_pframebufferWork;
   };

   ////// __SCREENGRABBER_H__

} // namespace remoting






