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
//#include "acme/_operating_system.h"
#include "DibSection.h"
#include "subsystem/node/SystemException.h"

DibSection::DibSection(const ::innate_subsystem::PixelFormat & pixelformat, const ::int_size & size, HWND compatibleWin)
: m_isOwnTargetDC(false),
  m_targetDC(0),
  m_memDC(0),
  m_hbmOld(0),
  m_hbmDIB(0),
  m_srcOffsetX(0),
  m_srcOffsetY(0),
  m_buffer(0)
{
  try {
    openDIBSection(pixelformat, size, compatibleWin);
  } catch (...) {
    closeDIBSection();
    throw;
  }
}

DibSection::~DibSection()
{
  try {
    closeDIBSection();
  } catch (...) {
  }
}

void DibSection::setTargetDC(HDC targetDC)
{
  releaseTargetDC();
  m_isOwnTargetDC = false;
  m_targetDC = targetDC;
}

void *DibSection::getBuffer()
{
  return m_buffer;
}

void DibSection::blitToDibSection(const ::int_rectangle &  rectangle)
{
  blitToDibSection(rectangle, SRCCOPY);
}

void DibSection::blitTransparentToDibSection(const ::int_rectangle &  rectangle)
{
  blitToDibSection(rectangle, SRCCOPY | CAPTUREBLT);
}

void DibSection::blitFromDibSection(const ::int_rectangle &  rectangle)
{
  blitFromDibSection(rectangle, SRCCOPY);
}

void DibSection::stretchFromDibSection(const ::int_rectangle &  rectangleSource,const ::int_rectangle & rectangleTarget)
{
  stretchFromDibSection(rectangleSource, rectangleTarget, SRCCOPY);
}

void DibSection::blitToDibSection(const ::int_rectangle &  rectangle, DWORD flags)
{
  if (BitBlt(m_memDC, rectangle.left, rectangle.top, rectangle.width(), rectangle.height(),
             m_targetDC, rectangle.left + m_srcOffsetX,
             rectangle.top + m_srcOffsetY, flags) == 0) {
    throw ::subsystem::Exception("Can't blit to DIB section.");
  }
}

void DibSection::blitFromDibSection(const ::int_rectangle &  rectangle, DWORD flags)
{
  if (BitBlt(m_targetDC, rectangle.left + m_srcOffsetX, rectangle.top + m_srcOffsetY,
             rectangle.width(), rectangle.height(),
             m_memDC, rectangle.left, rectangle.top, flags) == 0) {
    throw ::subsystem::Exception("Can't blit from DIB section.");
  }
}

void DibSection::stretchFromDibSection(const ::int_rectangle &  rectangleSource,const ::int_rectangle & rectangleTarget, DWORD flags)
{
  SetStretchBltMode(m_targetDC, HALFTONE);
  if (StretchBlt(m_targetDC, rectangleSource.left + m_srcOffsetX, rectangleSource.top + m_srcOffsetY,
                 rectangleSource.width(), rectangleSource.height(),
                 m_memDC, rectangleTarget.left, rectangleTarget.top, rectangleTarget.width(), rectangleTarget.height(),
                 flags) == 0) {
    throw ::subsystem::Exception("Can't strech blit from DIB section.");
  }
}

void DibSection::setupBMIStruct(BITMAPINFO *pBmi, const ::innate_subsystem::PixelFormat & pixelformat, const ::int_size & size)
{
  if (pixelformat.bitsPerPixel == 8) {
    Screen::Palette8bitBMI *paletteBMI = reinterpret_cast<Screen::Palette8bitBMI *>(pBmi);
    memset(paletteBMI, 0, sizeof(Screen::Palette8bitBMI));
    pBmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    unsigned char index = 0;
    for (int i = 0; i < 256; i++, index++) {
      unsigned int red = (index >> pixelformat.redShift) & pixelformat.redMax;
      red = red * 0xFF / pixelformat.redMax;
      paletteBMI->rgbQuad[index].rgbRed = (unsigned char)red;
      unsigned int green = (index >> pixelformat.greenShift) & pixelformat.greenMax;
      green = green * 0xFF / pixelformat.greenMax;
      paletteBMI->rgbQuad[index].rgbGreen = (unsigned char)(green);
      unsigned int blue = (index >> pixelformat.blueShift) & pixelformat.blueMax;
      blue = blue * 0xFF / pixelformat.blueMax;
      paletteBMI->rgbQuad[index].rgbBlue  = (unsigned char)blue;
    }
  } else {
    Screen::BMI *bitFieldBmi = reinterpret_cast<Screen::BMI *>(pBmi);
    memset(bitFieldBmi, 0, sizeof(Screen::BMI));
    bitFieldBmi->bmiHeader.biCompression = BI_BITFIELDS;
    bitFieldBmi->red   = pixelformat.redMax   << pixelformat.redShift;
    bitFieldBmi->green = pixelformat.greenMax << pixelformat.greenShift;
    bitFieldBmi->blue  = pixelformat.blueMax  << pixelformat.blueShift;
  }
  pBmi->bmiHeader.biPlanes = 1;
  pBmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  pBmi->bmiHeader.biBitCount = pixelformat.bitsPerPixel;
  pBmi->bmiHeader.biWidth = size.cx;
  pBmi->bmiHeader.biHeight = -size.cy;
}

void DibSection::openDIBSection(const ::innate_subsystem::PixelFormat & pixelformat, const ::int_size & size, HWND compatibleWin)
{
  m_targetDC = GetDC(compatibleWin);
  m_isOwnTargetDC = true;
  if (m_targetDC == 0) {
    throw SystemException("Can't get DC to create a DIB section");
  }
  if (compatibleWin == 0) {
    // In this special case is needed to store offset of the desktop, because coordinates
    // of the top level corner may be non zero.
    m_screen.update();
    ::int_rectangle deskRect = m_screen.getDesktopRect();
    m_srcOffsetX = deskRect.left;
    m_srcOffsetY = deskRect.top;
  }

  Screen::BMI bitFieldBmi;
  Screen::Palette8bitBMI paletteBMI;
  BITMAPINFO *pBmi = 0;

  if (pixelformat.bitsPerPixel == 8) {
    pBmi = reinterpret_cast<BITMAPINFO *>(&paletteBMI);
  } else {
    pBmi = reinterpret_cast<BITMAPINFO *>(&bitFieldBmi);
  }
  setupBMIStruct(pBmi, pixelformat, size);

  m_memDC = CreateCompatibleDC(m_targetDC);
  if (m_memDC == NULL) {
    throw SystemException("Can't create a compatible DC to open a dib section");
  }

  m_hbmDIB = CreateDIBSection(m_memDC, (BITMAPINFO *)pBmi, DIB_RGB_COLORS, &m_buffer, NULL, NULL);
  if (m_hbmDIB == 0) {
    throw SystemException("Can't create a dib section");
  }

  m_hbmOld = (HBITMAP)SelectObject(m_memDC, m_hbmDIB);
}

void DibSection::closeDIBSection()
{
  if (m_hbmOld != 0) {
    SelectObject(m_memDC, m_hbmOld);
    m_hbmOld = 0;
  }

  if (m_hbmDIB != 0) {
    DeleteObject(m_hbmDIB);
    m_hbmDIB = 0;
  }

  if (m_memDC != 0) {
    DeleteDC(m_memDC);
    m_memDC = 0;
  }

  releaseTargetDC();
}

void DibSection::releaseTargetDC()
{
  if (m_targetDC != 0 && m_isOwnTargetDC) {
    ReleaseDC(0, m_targetDC);
    m_targetDC = 0;
  }
}
