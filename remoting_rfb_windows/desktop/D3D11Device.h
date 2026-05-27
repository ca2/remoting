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


#include "remoting/remoting_rfb_windows/_common_header.h"
#include "subsystem/node/DynamicLibrary.h"
#include <d3d11.h>
#include <DXGI1_2.h>


namespace remoting_rfb_windows
{

   // #include "log_writer/LogWriter.h"


   class CLASS_DECL_REMOTING_RFB_WINDOWS D3D11Device :
      virtual public ::particle
   {
   public:

            ::comptr<IDXGIDevice> m_pdxgidevice;

      ::comptr<IDXGIAdapter1> m_pdxgiadapter1;

            ::subsystem::DynamicLibrary m_d3d11Lib;
            ::comptr<ID3D11Device> m_pd3d11device;
            ::comptr<ID3D11DeviceContext> m_pd3d11devicecontext;

            ::pointer<::subsystem::LogWriter> m_plogwriter;

      // Creates new device and context of first found.
      D3D11Device();
      D3D11Device(::subsystem::LogWriter * plogwriter);
      // Copy references and increase count for winD3D11Device's internal handles. So the
      // source winD3D11Device object can be destroyed while this object will use.
      //WinD3D11Device(const WinD3D11Device &src);
      virtual ~D3D11Device();

      //WinD3D11Device &operator=(WinD3D11Device const &src);

      HRESULT deviceQueryInterface(REFIID riid, void **ppvObject);
      HRESULT contextQueryInterface(REFIID riid, void **ppvObject);

      ID3D11Device *getDevice();
      ID3D11DeviceContext *getContext();

      // A wrap for the ID3D11DeviceContext::CopySubresourceRegion() function.
      void copySubresourceRegion(ID3D11Texture2D *dstTexture2D, ::i32 dstX, ::i32 dstY, ID3D11Texture2D *srcTexture2D,
                                 const ::i32_rectangle &rectangleSource, ::u32 front, ::u32 back);
   //private:
      //void copy(const WinD3D11Device &src);

   };

   //// __WIND3D11DEVICE_H__


} // namespace remoting_rfb_windows





