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
#include "framework.h"
#include "remoting/remoting_rtc_windows/desktop/WinDxCriticalException.h"
#include "remoting/remoting_rtc_windows/desktop/WinDxRecoverableException.h"

// The header including of this cpp file must be at last place to avoid build conflicts.
#include "remoting/remoting_rtc_windows/desktop/aaa_WinDxgiAdapter.h"

// namespace remoting_rtc_windows
// {
//
//
//    WinDxgiAdapter::WinDxgiAdapter(WinDxgiDevice *winDxgiDevice) //: m_dxgiAdapter(0)
//    {
//       HRESULT hr = winDxgiDevice->getParent(__interface_of(m_pdxgiadapter));
//       if (FAILED(hr))
//       {
//          throw WinDxCriticalException("Can't GetParent for IDXGIAdapter", hr);
//       }
//    }
//
//    WinDxgiAdapter::~WinDxgiAdapter()
//    {
//       // if (m_dxgiAdapter != 0)
//       // {
//       //    m_dxgiAdapter->Release();
//       //    m_dxgiAdapter = 0;
//       // }
//    }
//
//    HRESULT WinDxgiAdapter::getDxgiOutput(::u32 iOutput, IDXGIOutput **iDxgiOutput)
//    {
//       HRESULT hr = m_pdxgiadapter->EnumOutputs(iOutput, iDxgiOutput);
//       return hr;
//       // if (hr == DXGI_ERROR_NOT_FOUND)
//       // {
//       //    ::string errMess;
//       //    errMess.format("IDXGIOutput not found for iOutput = %u", iOutput);
//       //    throw WinDxRecoverableException(errMess, hr);
//       // }
//       // else if (FAILED(hr))
//       // {
//       //    throw WinDxCriticalException("Can't IDXGIAdapter::EnumOutputs()", hr);
//       // }
//    }
//
//
// } // namespace remoting_rtc_windows
//
