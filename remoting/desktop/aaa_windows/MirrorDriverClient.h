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

#pragma once

#include "remoting/remoting/desktop/windows/_common_header.h"
#include "UpdateKeeper.h"
#include "UpdateListener.h"
#include "innate_subsystem/framebuffer/Framebuffer.h"
#include "subsystem/platform/RegistryKey.h"
#include "DisplayEsc.h"
#include "subsystem/node/Screen.h"
#include "subsystem/platform/WindowMessageHandler.h"
#include "innate_subsystem/gui/MessageWindow.h"
#include "subsystem/thread/GuiThread.h"
#include "acme/parallelization/happening.h"
//#include "log_writer/LogWriter.h"

namespace remoting
{

   class CLASS_DECL_REMOTING MirrorDriverClient :
      virtual public ::subsystem::GuiThread,
      virtual public ::subsystem::WindowMessageHandler
   {
   public:


      MirrorDriverClient(::subsystem::LogWriter * plogwriter);
         virtual ~MirrorDriverClient();

      ::innate_subsystem::PixelFormat getPixelFormat() const;
      ::int_size getDimension() const;

      void *getBuffer();
      //CHANGES_BUF *getChangesBuf() const;
      void *getChangesBuf() const;

      bool getPropertiesChanged();
      bool getScreenSizeChanged();

      bool applyNewProperties();

      void open();
      void close();

      void load();
      void unload();

      void connect();
      void disconnect();

   private:
      static ::string_literal MINIPORT_REGISTRY_PATH[];

      static const int EXT_DEVMODE_SIZE_MAX = 3072;
      struct DFEXT_DEVMODE : DEVMODE
      {
         char extension[EXT_DEVMODE_SIZE_MAX];
      };

   private:
      virtual bool processMessage(unsigned int scopedstrMessage, ::wparam wParam, ::lparam lParam);

      virtual void execute();
      virtual void onTerminate();

      void dispose();

      void extractDeviceInfo(const char *driverName);
      void openDeviceRegKey(const char *miniportName);

      void initScreenPropertiesByCurrent();
      // value - true to attach, false to detach.
      void setAttachToDesktop(bool value);
      void commitDisplayChanges(DEVMODE *pdm);

      // Driver states.
      bool m_isDriverOpened;
      bool m_isDriverLoaded;
      bool m_isDriverAttached;
      bool m_isDriverConnected;

      DWORD m_deviceNumber;
      DISPLAY_DEVICE m_deviceInfo;
      RegistryKey m_regkeyDevice;
      DFEXT_DEVMODE m_deviceMode;
      HDC m_driverDC;

      CHANGES_BUF *m_changesBuffer;
      void *m_screenBuffer;

      ::happening m_initListener;
      bool m_isDisplayChanged;
      MessageWindow m_messagewindowPropertyChangeListener;

      ::innate_subsystem::PixelFormat m_pixelformat;
      ::int_size m_dimension;
      ::int_point m_leftTopCorner;
      Screen m_screen;

      ::pointer < ::subsystem::LogWriter > m_plogwriter;
   };


} // namespace remoting








