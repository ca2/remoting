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
#include "remoting/remoting_rtc_windows/desktop/aaa_WinDxgiOutput.h"
//
// namespace remoting_rtc_windows
// {
//
//
//    WinDxgiOutput::WinDxgiOutput(WinDxgiAdapter *dxgiAdapter, ::u32 iOutput)
//    {
//       dxgiAdapter->getDxgiOutput(iOutput, &m_dxgiOutput);
//
//       memset(&m_description, 0, sizeof(m_description));
//       HRESULT hr = m_dxgiOutput->GetDesc(&m_description);
//       if (FAILED(hr))
//       {
//          throw WinDxCriticalException("Can't get output description", hr);
//       }
//    }
//
//    WinDxgiOutput::WinDxgiOutput(const WinDxgiOutput &src) { copy(src); }
//
//    WinDxgiOutput::~WinDxgiOutput()
//    {
//       if (m_dxgiOutput != 0)
//       {
//          m_dxgiOutput->Release();
//          m_dxgiOutput = 0;
//       }
//    }
//
//    WinDxgiOutput &WinDxgiOutput::operator=(WinDxgiOutput const &src)
//    {
//       copy(src);
//       return *this;
//    }
//
//    void WinDxgiOutput::copy(const WinDxgiOutput &src)
//    {
//       if (this != &src)
//       {
//          m_dxgiOutput = src.m_dxgiOutput;
//          m_dxgiOutput->AddRef();
//          m_description = src.m_description;
//       }
//    }
//
//    bool WinDxgiOutput::isAttachedtoDesktop() { return m_description.AttachedToDesktop != false; }
//
//    ::i32_rectangle WinDxgiOutput::getDesktopCoordinates() { return m_description.DesktopCoordinates; }
//
//    void WinDxgiOutput::getDeviceName(::string &out)
//    {
//       //::wstring uniString(m_description.DeviceName);
//       //uniString.toStringStorage(out);
//       out = m_description.DeviceName;
//    }
//
//    DXGI_MODE_ROTATION WinDxgiOutput::getRotation() const { return m_description.Rotation; }
//
//    HRESULT WinDxgiOutput::queryInterface(REFIID riid, void **ppvObject)
//    {
//       return m_dxgiOutput->QueryInterface(riid, ppvObject);
//    }
//
//
// } // namespace remoting_rtc_windows
//