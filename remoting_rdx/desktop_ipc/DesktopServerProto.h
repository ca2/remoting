// Copyright (C) 2009,2010,2011,2012 GlavSoft LLC.
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


//#include "subsystem/platform/inttypes.h"
#include "innate_subsystem/framebuffer/Framebuffer.h"
#include "remoting/remoting_rfb/region/Region.h"

//#include "remoting/remoting_rfb/region/::i32_point.h"
//#include "subsystem/platform/class CLASS_DECL_REMOTING_RDX  ::time.h"

#include "BlockingGate.h"

namespace remoting_rfb
{

   class CLASS_DECL_REMOTING_RDX  DesktopServerProto :
   virtual public ::particle
   {
   public:
      // FIXME: Remove m_pcontrolgate from this class CLASS_DECL_REMOTING_RDX .
      // Forward pblockinggate will send requests
      ::pointer < BlockingGate > m_pcontrolgate;
      ::pointer < ::remoting_rfb_node::Configurator > m_pconfigurator;


      DesktopServerProto();
      ~DesktopServerProto() override;

      virtual void initialize_desktop_server_proto(::remoting_rfb_node::Configurator * pconfigurator, BlockingGate *pblockinggate);

      virtual void readPixelFormat(::innate_subsystem::PixelFormat & pixelformat, BlockingGate *pblockinggate);
      virtual void sendPixelFormat(const ::innate_subsystem::PixelFormat &pixelformat, BlockingGate *pblockinggate);
      virtual ::i32_size readDimension(BlockingGate *pblockinggate);
      virtual void sendDimension(const ::i32_size &size, BlockingGate *pblockinggate);
      virtual ::i32_point readPoint(BlockingGate *pblockinggate);
      virtual void sendPoint(const ::i32_point &point, BlockingGate *pblockinggate);
      virtual ::i32_rectangle readRect(BlockingGate *pblockinggate);
      virtual void sendRect(const ::i32_rectangle & rectangle, BlockingGate *pblockinggate);
      virtual void sendRegion(const ::remoting_rfb::Region & region, BlockingGate *pblockinggate);
      virtual void readRegion(::remoting_rfb::Region & region, BlockingGate *pblockinggate);

      void sendFramebuffer(const ::innate_subsystem::Framebuffer *pframebufferSource, const ::i32_rectangle &rectangleSource,
                           BlockingGate *pblockinggate);
      void readFramebuffer(::innate_subsystem::Framebuffer *pframebufferTarget, const ::i32_rectangle &rectangleTarget, BlockingGate *pblockinggate);

      virtual void sendNewClipboard(const ::scoped_string &newClipboard, BlockingGate *pblockinggate);
      virtual void readNewClipboard(::string &newClipboard, BlockingGate *pblockinggate);
      virtual void sendNewPointerPos(const ::i32_point pointNewPosition, ::u8 keyFlag, BlockingGate *pblockinggate);
      virtual void readNewPointerPos(::i32_point *pointNewPosition, ::u8 *keyFlag, BlockingGate *pblockinggate);
      virtual void sendKeyEvent(::u32 keySym, bool down, BlockingGate *pblockinggate);
      virtual void readKeyEvent(::u32 *keySym, bool *down, BlockingGate *pblockinggate);
      virtual void sendUserInfo(const ::scoped_string &desktopName, const ::scoped_string &userName,
                                BlockingGate *pblockinggate);
      virtual void readUserInfo(::string &desktopName, ::string &userName, BlockingGate *pblockinggate);
      virtual void sendConfigSettings(BlockingGate *pblockinggate);
      virtual void readConfigSettings(BlockingGate *pblockinggate);


      static const ::u8 EXTRACT_REQ = 0;
      static const ::u8 SCREEN_PROP_REQ = 1;
      static const ::u8 FRAME_BUFFER_INIT = 2;
      static const ::u8 SET_FULL_UPD_REQ_REGION = 3;
      static const ::u8 SET_EXCLUDING_REGION = 4;
      static const ::u8 UPDATE_DETECTED = 10;

      static const ::u8 CLIPBOARD_CHANGED = 30;
      static const ::u8 POINTER_POS_CHANGED = 31;
      static const ::u8 KEYBOARD_EVENT = 32;
      static const ::u8 USER_INPUT_INIT = 33;
      static const ::u8 USER_INFO_REQ = 34;
      static const ::u8 DESKTOP_COORDS_REQ = 35;
      static const ::u8 WINDOW_COORDS_REQ = 36;
      static const ::u8 WINDOW_HANDLE_REQ = 37;
      static const ::u8 DISPLAY_NUMBER_COORDS_REQ = 38;
      static const ::u8 APPLICATION_REGION_REQ = 39;
      static const ::u8 NORMALIZE_RECT_REQ = 40;
      static const ::u8 APPLICATION_CHECK_FOCUS = 41;
      static const ::u8 DISPLAYS_COORDS_REQ = 42;

      static const ::u8 CONFIG_RELOAD_REQ = 50;
      static const ::u8 SOFT_INPUT_ENABLING_REQ = 51;

   //private:
      void checkPixelFormat(const ::innate_subsystem::PixelFormat &pixelformat);
      void checkRectangle(const ::i32_rectangle & rectangle);
      void checkDimension(const ::i32_size &size);
   };


} // namespace remoting_rfb







