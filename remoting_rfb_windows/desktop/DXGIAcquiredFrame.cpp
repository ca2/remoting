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
#include "remoting/remoting_rfb_windows/desktop/WinDxRecoverableException.h"
// The header including of this cpp file must be at last place to avoid build conflicts.
#include "remoting/remoting_rfb_windows/desktop/DXGIAcquiredFrame.h"

namespace remoting_rfb_windows
{



   DXGIAcquiredFrame::DXGIAcquiredFrame(DXGIOutputDuplication *outDupl, ::u32 timeOutMilliSec) :
       m_wasTimeOut(false), m_poutputduplication(outDupl)
   {
      ZeroMemory(&m_frameInfo, sizeof(m_frameInfo));
      HRESULT hr =
         m_poutputduplication->getDxgiOutputDuplication()->AcquireNextFrame(timeOutMilliSec, &m_frameInfo, &m_pdxgiresourceDesktop);
      if (hr == DXGI_ERROR_WAIT_TIMEOUT)
      {
         m_wasTimeOut = true;
      }
      else if (FAILED(hr))
      {
         throw WinDxRecoverableException("Can't AcquireNextFrame()", hr);
      }
   }

   DXGIAcquiredFrame::~DXGIAcquiredFrame()
   {
      if (m_wasTimeOut)
      {
         information("it wasTimeout");
         return;

      }

      m_pdxgiresourceDesktop.release();
      auto hresultReleaseFrame = m_poutputduplication->getDxgiOutputDuplication()->ReleaseFrame();
      if (FAILED(hresultReleaseFrame))
      {

         information("failed to release frame");

      }
   }

   bool DXGIAcquiredFrame::wasTimeOut() { return m_wasTimeOut; }

   IDXGIResource *DXGIAcquiredFrame::dxgi_resource() { return m_pdxgiresourceDesktop; }

   DXGI_OUTDUPL_FRAME_INFO *DXGIAcquiredFrame::getFrameInfo() { return &m_frameInfo; }


} // namespace remoting_rfb_windows
//namespace remoting_rfb_windows
//{
