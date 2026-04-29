// Copyright (C) 2012 GlavSoft LLC.
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


#include "innate_subsystem/framebuffer/Framebuffer.h"
#include "remoting/remoting_windows/desktop/Win8CursorShape.h"
//#include "subsystem/thread/lockable_critical_section.h"
#include "subsystem/thread/GuiThread.h"

#include "remoting/remoting_windows/_common_header.h"
#include "remoting/remoting_windows/desktop/Win8DuplicationListener.h"
//#include "log_writer/LogWriter.h"

#include "remoting/remoting_windows/desktop/WinCustomD3D11Texture2D.h"
#include "remoting/remoting_windows/desktop/WinDxgiOutputDuplication.h"

namespace remoting_windows
{

   class CLASS_DECL_REMOTING_WINDOWS Win8DeskDuplication :
      virtual public ::subsystem::GuiThread
   {
   public:
      // The WinDxgiOutput *dxgiOutput passed object can be destroyed right after the constructor calling.
      // The WinD3D11Device *device passed object can be destroyed right after the constructor calling.
      Win8DeskDuplication(::innate_subsystem::Framebuffer *targetFb, ::int_rectangle_array_base &targetRect,
                          Win8CursorShape *targetCurShape, LONGLONG *cursorTimeStamp, lockable_critical_section *cursorMutex,
                          Win8DuplicationListener *duplListener, ::pointer_array<WinDxgiOutput> &dxgiOutput,
                          ::subsystem::LogWriter * plogwriter);
      virtual ~Win8DeskDuplication();

      bool isValid();

   private:
      virtual void execute();
      virtual void onTerminate();
      void setCriticalError(const ::scoped_string &scopedstrReason);
      void setRecoverableError(const ::scoped_string &scopedstrReason);

      void processMoveRects(size_t moveCount, size_t out);
      void processDirtyRects(size_t dirtyCount, WinD3D11Texture2D *acquiredDesktopImage, size_t out);
      void processCursor(const DXGI_OUTDUPL_FRAME_INFO *info, size_t out);

      ::int_size getStageDimension(size_t out) const;

      void rotateRectInsideStage(::int_rectangle &rectangleToTranspose, const ::int_size &sizeStage, DXGI_MODE_ROTATION rotation);

      ::innate_subsystem::Framebuffer *m_targetFb;

      ::int_rectangle_array_base m_targetRects;
      Win8CursorShape *m_targetCurShape;
      LONGLONG *m_cursorTimeStamp;
      lockable_critical_section *m_cursorMutex;

      ::array_base<DXGI_MODE_ROTATION> m_rotations;

      Win8DuplicationListener *m_duplListener;

      WinD3D11Device m_device;
      ::pointer_array<WinDxgiOutput1> m_dxgiOutput1;
      ::pointer_array<WinDxgiOutputDuplication> m_outDupl;

      // The duplication interface can't be used
      bool m_hasCriticalError;
      // The interface can be used but it should be reinitialized.
      bool m_hasRecoverableError;


      // Use this variables as class CLASS_DECL_REMOTING_WINDOWS fields to avoid frequency memory allocations.
      ::array_base<RECT> m_dirtyRects;
      ::array_base<DXGI_OUTDUPL_MOVE_RECT> m_moveRects;

      ::pointer_array<WinCustomD3D11Texture2D > m_stageTextures2D;
      ::pointer < ::innate_subsystem::Framebuffer > m_pframebufferAuxiliaryProperty;

      ::pointer < ::subsystem::LogWriter > m_plogwriter;
   };

   //// __WIN8DESKDUPLICATIONTHREAD_H__


}// namespace remoting_windows





