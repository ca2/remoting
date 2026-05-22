// Copyright (C) 2012 GlavSoft LLC.
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

#include "remoting/remoting_windows/_common_header.h"
#include "remoting/remoting_windows/desktop/aaa_WinDxgiOutput1.h"
#include "remoting/remoting_rtc/rfb/CursorShape.h"

#include <d3d11.h>
#include <DXGI1_2.h>


namespace remoting_windows
{


   class D3D11Device;

   // This class  is a wrapper for the IDXGIOutputDuplication interface.
   class CLASS_DECL_REMOTING_WINDOWS DXGIOutputDuplication :
   virtual public ::particle
   {
   public:
      DXGIOutputDuplication(IDXGIOutput1 * pdxgioutput1, D3D11Device *d3D11Device);
      //WinDxgiOutputDuplication(const WinDxgiOutputDuplication &src);
      virtual ~DXGIOutputDuplication();

      //WinDxgiOutputDuplication &operator=(WinDxgiOutputDuplication const &src);

      // Return pointer to a IDXGIOutputDuplication object. The pointer will be valid until
      // this object destructor has been called.
      IDXGIOutputDuplication *getDxgiOutputDuplication();

      // Throws WinDxException on an error.
      // Returns count of got "move" rectanglea.
      // Also, the function resize the moveRects ::array_base if it's needed.
      size_t getFrameMoveRects(::array_base<DXGI_OUTDUPL_MOVE_RECT> *moveRects);

      // Throws WinDxException on an error.
      // Returns count of got "dirty" rectanglea.
      // Also, the function resize the dirtyRects ::array_base if it's needed.
      size_t getFrameDirtyRects(::array_base<RECT> *dirtyRects);

      // Throws WinDxException on an error.
      // Modifies cursorShape with new data.
      void getFrameCursorShape(::remoting_rtc::CursorShape *cursorShape, ::u32 pointerShapeBufferSize,
                               ::subsystem::LogWriter * plogwriter);

   private:
      //void copy(const WinDxgiOutputDuplication &src);

      ::comptr <IDXGIOutputDuplication > m_poutputduplication;
   };

   //// __WINDXGIOUTPUTDUPLICATION_H__


} // namespace remoting_windows




