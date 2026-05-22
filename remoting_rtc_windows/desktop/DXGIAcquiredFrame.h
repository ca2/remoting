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
#include "remoting/remoting_windows/desktop/DXGIOutputDuplication.h"


namespace remoting_windows
{

   // #include aaa_<d3d11.h>
   // #include aaa_<DXGI1_2.h>

   class CLASS_DECL_REMOTING_WINDOWS DXGIAcquiredFrame :
      virtual public ::particle
   {
   public:

      ::pointer < DXGIOutputDuplication > m_poutputduplication;

      DXGI_OUTDUPL_FRAME_INFO m_frameInfo;
      ::comptr < IDXGIResource > m_pdxgiresourceDesktop;
      bool m_wasTimeOut;


      DXGIAcquiredFrame(DXGIOutputDuplication *outDupl, ::u32 timeOutMilliSec);
      ~DXGIAcquiredFrame() override;


      bool wasTimeOut();

      IDXGIResource *dxgi_resource();
      DXGI_OUTDUPL_FRAME_INFO *getFrameInfo();

   };

   //// __WINDXGIACQUIREDFRAME_H__


} // namespace remoting_windows



