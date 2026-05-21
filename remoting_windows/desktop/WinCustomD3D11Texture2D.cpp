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

// The header including of this cpp file must be at last place to avoid build conflicts.
#include "framework.h"
#include "remoting/remoting_windows/desktop/WinCustomD3D11Texture2D.h"
#include "WinDxRecoverableException.h"
//
// namespace remoting_windows
// {
//
//
//    // WinCustomD3D11Texture2D::Texture2DDescInitializer::Texture2DDescInitializer(::u32 width, ::u32 height,
//    //                                                                             DXGI_MODE_ROTATION rotation)
//    // {
//    //
//    // }
//    //
//    // const D3D11_TEXTURE2D_DESC *WinCustomD3D11Texture2D::Texture2DDescInitializer::getDesc() const { return &m_desc; }
//
//    WinCustomD3D11Texture2D::WinCustomD3D11Texture2D(ID3D11Device *pd3d11device, ::u32 width, ::u32 height,
//                                                     DXGI_MODE_ROTATION rotation) //:
//        //m_textDescInitializer(width, height, rotation)
//    {
//
//       D3D11_TEXTURE2D_DESC desc{};
//       if (rotation == DXGI_MODE_ROTATION_ROTATE90 || rotation == DXGI_MODE_ROTATION_ROTATE270)
//       {
//          desc.Width = height;
//          desc.Height = width;
//       }
//       else
//       {
//          desc.Width = width;
//          desc.Height = height;
//       }
//       desc.MipLevels = 1;
//       desc.ArraySize = 1;
//       desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
//       desc.SampleDesc.Count = 1;
//       desc.SampleDesc.Quality = 0;
//       desc.Usage = D3D11_USAGE_STAGING;
//       desc.BindFlags = 0;
//       desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
//       desc.MiscFlags = 0;
//       HRESULT hr = pd3d11device->CreateTexture2D(&desc, 0, &m_pd3d11texture2d);
//
//       if (FAILED(hr) || !m_pd3d11texture2d)
//       {
//
//          throw WinDxRecoverableException("Can't CreateTexture2D()", hr);
//
//       }
//
//       //allocateø
//       //emplace_newø(m_pd3d11texture2d, device, m_textDescInitializer.getDesc());
//
//    }
//
//    // WinCustomD3D11Texture2D::WinCustomD3D11Texture2D(const WinCustomD3D11Texture2D &other) :
//    //     m_textDescInitializer(other.m_textDescInitializer), m_textureWrapper(other.m_textureWrapper)
//    // {
//    // }
//    //
//    // void WinCustomD3D11Texture2D::operator=(const WinCustomD3D11Texture2D &other)
//    // {
//    //    m_textDescInitializer = other.m_textDescInitializer;
//    //    m_textureWrapper = other.m_textureWrapper;
//    // }
//
//    WinCustomD3D11Texture2D::~WinCustomD3D11Texture2D() {}
//
//    ID3D11Texture2D *WinCustomD3D11Texture2D::getTexture() { return m_pd3d11texture2d; }
//
//    //const D3D11_TEXTURE2D_DESC *WinCustomD3D11Texture2D::getDesc() const { return m_textDescInitializer.getDesc(); }
//
//
// } // namespace remoting_windows
