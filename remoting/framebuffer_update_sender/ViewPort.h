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

#pragma once


#include "ViewPortState.h"
#include "innate_subsystem/framebuffer/Framebuffer.h"
#include "remoting/remoting/desktop/Desktop.h"
//#include "subsystem/thread/critical_section.h"
//#include "subsystem/platform/class ::time.h"
//#include "log_writer/LogWriter.h"


namespace remoting
{

   // This class CLASS_DECL_REMOTING  calculates actual view port rectangle.
   // Typical usage:
   // // Initialisation
   // Viewport m_rectangleViewport;
   // ...
   // ...
   // m_rectangleViewport.update(&pframebuffer->getDimension());
   // ::int_rectangle rectangleViewport = m_rectangleViewport.getViewport();
   class CLASS_DECL_REMOTING  Viewport :
   virtual public ::particle
   {
   public:


      static const int RESOLVING_PERIOD = 3000;

      ::pointer < Desktop > m_pdesktop;

      ViewPortState m_viewportstate;
      ::int_rectangle m_rectangle;
      ::remoting::Region m_regionApp;
      critical_section m_stateMutex;

      class ::time m_timeLatestHwndResolving;

      ::pointer < ::subsystem::LogWriter > m_plogwriter;

      //Viewport(::subsystem::LogWriter * plogwriter);
      //Viewport(const ViewPortState *viewPortState, ::subsystem::LogWriter * plogwriter);
      Viewport();
      ~Viewport() override;


      virtual void initialize_viewport(::subsystem::LogWriter * plogwriter);
      virtual void initialize_viewport(const ViewPortState & viewportstate, ::subsystem::LogWriter * plogwriter);

      // Sets desktop interface that can be used in some mode to get
      // desktop info. The desktop interface uses only in the update() function.
      void initDesktopInterface(::remoting::Desktop *desktop);

      // This function updates view port rectangle. The new view port rectangle
      // will be constrained by fbDimension.
      void update(const ::int_size &fbDimension);

      // This function returns the view port rectangle.
      ::int_rectangle getViewport();

      // Returns true if checked share only application.
      bool getOnlyApplication();

      // Returns application PID if we share one application. Check getOnlyApplication() as well,
      // to make sure the returned value is relevant.
      unsigned int getApplicationId();

      // Returns a region that has rectangles of application which was visible
      // window parts at latest calling of the update() function. The region
      // doesn't changes by the update function if checked mode isn't "shareapp".
      void getApplicationRegion(::remoting::Region & region);

      // Assignes self values by an external state.
      void changeState(const ViewPortState *newState);

   //private:
      // Disable the copy operation and constructor.
     // Viewport(const Viewport &);
      //Viewport &operator=(const Viewport &);

      // Resolves a window name of the view port state to window handle.
      // On an error the function do nothing.
      void resolveWindowName();

   };


} // namespace remoting




