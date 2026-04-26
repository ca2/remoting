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


//#include "subsystem/platform/inttypes.h"
#include "innate_subsystem/framebuffer/Framebuffer.h"
#include "remoting/remoting/region/Region.h"

//#include "remoting/remoting/region/::int_point.h"
//#include "subsystem/platform/class ::time.h"

#include "BlockingGate.h"

namespace remoting
{

   class CLASS_DECL_REMOTING DesktopServerProto :
   virtual public ::particle
   {
   public:
      // FIXME: Remove m_pblockinggate from this class.
      // Forward pblockinggate will send requests
      ::pointer < BlockingGate > m_pblockinggate;
      ::pointer < Configurator > m_pconfigurator;


      DesktopServerProto();
      ~DesktopServerProto() override;

      virtual void initialize_desktop_server_proto(Configurator * pconfigurator, BlockingGate *pblockinggate);

      virtual void readPixelFormat(::innate_subsystem::PixelFormat & pixelformat, BlockingGate *pblockinggate);
      virtual void sendPixelFormat(const ::innate_subsystem::PixelFormat &pixelformat, BlockingGate *pblockinggate);
      virtual ::int_size readDimension(BlockingGate *pblockinggate);
      virtual void sendDimension(const ::int_size &size, BlockingGate *pblockinggate);
      virtual ::int_point readPoint(BlockingGate *pblockinggate);
      virtual void sendPoint(const ::int_point &point, BlockingGate *pblockinggate);
      virtual ::int_rectangle readRect(BlockingGate *pblockinggate);
      virtual void sendRect(const ::int_rectangle & rectangle, BlockingGate *pblockinggate);
      virtual void sendRegion(const ::remoting::Region & region, BlockingGate *pblockinggate);
      virtual void readRegion(::remoting::Region & region, BlockingGate *pblockinggate);

      void sendFramebuffer(const ::innate_subsystem::Framebuffer *pframebufferSource, const ::int_rectangle &rectangleSource,
                           BlockingGate *pblockinggate);
      void readFramebuffer(::innate_subsystem::Framebuffer *pframebufferTarget, const ::int_rectangle &rectangleTarget, BlockingGate *pblockinggate);

      virtual void sendNewClipboard(const ::scoped_string &newClipboard, BlockingGate *pblockinggate);
      virtual void readNewClipboard(::string &newClipboard, BlockingGate *pblockinggate);
      virtual void sendNewPointerPos(const ::int_point newPos, unsigned char keyFlag, BlockingGate *pblockinggate);
      virtual void readNewPointerPos(::int_point *newPos, unsigned char *keyFlag, BlockingGate *pblockinggate);
      virtual void sendKeyEvent(unsigned int keySym, bool down, BlockingGate *pblockinggate);
      virtual void readKeyEvent(unsigned int *keySym, bool *down, BlockingGate *pblockinggate);
      virtual void sendUserInfo(const ::scoped_string &desktopName, const ::scoped_string &userName,
                                BlockingGate *pblockinggate);
      virtual void readUserInfo(::string &desktopName, ::string &userName, BlockingGate *pblockinggate);
      virtual void sendConfigSettings(BlockingGate *pblockinggate);
      virtual void readConfigSettings(BlockingGate *pblockinggate);


      static const unsigned char EXTRACT_REQ = 0;
      static const unsigned char SCREEN_PROP_REQ = 1;
      static const unsigned char FRAME_BUFFER_INIT = 2;
      static const unsigned char SET_FULL_UPD_REQ_REGION = 3;
      static const unsigned char SET_EXCLUDING_REGION = 4;
      static const unsigned char UPDATE_DETECTED = 10;

      static const unsigned char CLIPBOARD_CHANGED = 30;
      static const unsigned char POINTER_POS_CHANGED = 31;
      static const unsigned char KEYBOARD_EVENT = 32;
      static const unsigned char USER_INPUT_INIT = 33;
      static const unsigned char USER_INFO_REQ = 34;
      static const unsigned char DESKTOP_COORDS_REQ = 35;
      static const unsigned char WINDOW_COORDS_REQ = 36;
      static const unsigned char WINDOW_HANDLE_REQ = 37;
      static const unsigned char DISPLAY_NUMBER_COORDS_REQ = 38;
      static const unsigned char APPLICATION_REGION_REQ = 39;
      static const unsigned char NORMALIZE_RECT_REQ = 40;
      static const unsigned char APPLICATION_CHECK_FOCUS = 41;
      static const unsigned char DISPLAYS_COORDS_REQ = 42;

      static const unsigned char CONFIG_RELOAD_REQ = 50;
      static const unsigned char SOFT_INPUT_ENABLING_REQ = 51;

   //private:
      void checkPixelFormat(const ::innate_subsystem::PixelFormat &pixelformat);
      void checkRectangle(const ::int_rectangle & rectangle);
      void checkDimension(const ::int_size &size);
   };


} // namespace remoting







