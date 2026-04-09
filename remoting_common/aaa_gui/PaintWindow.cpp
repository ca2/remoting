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
#include "framework.h"
#include "PaintWindow.h"

::innate_subsystem::PaintWindow::::innate_subsystem::PaintWindow()
 : m_bIsDraw(false)
{
}

::innate_subsystem::PaintWindow::~::innate_subsystem::PaintWindow()
{
}

HDC ::innate_subsystem::PaintWindow::getHDCPaint()
{
  if (!m_bIsDraw) {
    return 0;
  }
  return m_hdc;
}

//void ::innate_subsystem::PaintWindow::onPaint(DeviceContext *dc, PAINTSTRUCT *paintStruct)
void ::innate_subsystem::PaintWindow::onPaint()
{
}

bool ::innate_subsystem::PaintWindow::wndProc(unsigned int scopedstrMessage, ::wparam wParam, ::lparam lParam)
{
  if (scopedstrMessage == WM_PAINT) {
    m_hdc = BeginPaint(m_hwnd, &m_paintStruct);
    m_bIsDraw = true;
    //DeviceContext dc(this);
    //onPaint(&dc, &m_paintStruct);
     onPaint();
    EndPaint(m_hwnd, &m_paintStruct);
    m_bIsDraw = false;
    return true;
  }
  return BaseWindow::wndProc(scopedstrMessage, wParam, lParam);
}

