// Copyright (C) 2009,2010,2011,2012 GlavSoft LLC.
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
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//
#include "framework.h"
#include "Graphics.h"

#include "acme/prototype/geometry2d/rectangle.h"
#include <gdiplus.h>

Graphics::Graphics(HDC hdc)
: m_hdc(hdc)
{
   //m_ppen = nullptr;
}

Graphics::~Graphics()
{
   // if (m_ppen)
   // {
   //
   //    delete m_ppen;
   //
   // }
}

// void Graphics::setExPen(FLOAT fWidth, const color::color &color)
// {
//
//    if (m_ppen)
//    {
//
//       delete m_ppen;
//
//    }
//
//    m_ppen = new Gdiplus::Pen(Gdiplus::Color(color.byte_opacity(),
//       color.byte_red(),
//       color.byte_green(),
//       color.byte_blue()), fWidth);
//
// }

void Graphics::setBkMode(bool transparent)
{
  SetBkMode(m_hdc, transparent ? TRANSPARENT : OPAQUE);
}

void Graphics::setTextColor(COLORREF color)
{
  SetTextColor(m_hdc, color);
}

void Graphics::setBkColor(COLORREF color)
{
  SetBkColor(m_hdc, color);
}

void Graphics::setBrush(const Brush *brush)
{
  HGDIOBJ object = (brush != 0) ? brush->m_brush : 0;
   ::SelectObject(m_hdc, object);
}

void Graphics::setPen(const Pen *pen)
{
  HGDIOBJ object = (pen != 0) ? pen->m_pen : 0;
  ::SelectObject(m_hdc, object);
}

void Graphics::moveTo(int x, int y)
{
  MoveToEx(m_hdc, x, y, NULL);
   //m_xLast = x;
   //m_yLast = y;
}

void Graphics::lineTo(int x, int y)
{
  LineTo(m_hdc, x, y);
   //m_xLast = x;
   //m_yLast = y;
}
//
// void Graphics::ExLineTo(int x, int y)
// {
//    Gdiplus::Graphics g(m_hdc);
//    g.DrawLine(m_ppen, m_xLast, m_yLast, x, y);
//    m_xLast = x;
//    m_yLast = y;
// }
//
// void Graphics::fill_solid_rectangle(const ::int_rectangle & rectangle, const ::color::color & color)
// {
//
//    ::Gdiplus::Graphics g(m_hdc);
//    g.SetCompositingMode(Gdiplus::CompositingModeSourceOver);
//    g.SetCompositingQuality(Gdiplus::CompositingQualityHighQuality);
//
//    //r.deflate(2, 2);
//    Gdiplus::Rect gdiplusr;
//    ::copy(gdiplusr, rectangle);
//    Gdiplus::SolidBrush brush(Gdiplus::Color(color.byte_opacity(), color.byte_red(), color.byte_green(), color.byte_blue()));
//    g.FillRectangle(&brush, gdiplusr);
//    // // Set background color
//    // COLORREF oldColor = SetBkColor(m_hdc, RGB(color.byte_red(), color.byte_green(), color.byte_blue()));
//    //
//    // RECT r;
//    // ::copy(r, rectangle);
//    // // Fill rectangle using ETO_OPAQUE
//    // ExtTextOut(
//    // m_hdc,
//    //     0,              // x (ignored when no text)
//    //     0,              // y (ignored when no text)
//    //     ETO_OPAQUE,
//    //     &r,            // rectangle to fill
//    //     nullptr,        // no text
//    //     0,              // text length
//    //     nullptr         // dx array
//    // );
//    //
//    // // Restore previous background color
//    // SetBkColor(m_hdc, oldColor);
// }

void Graphics::fillRect(const ::int_rectangle & r, const Brush *pbrush)
{

   fillRect(r.left, r.top, r.right, r.bottom, pbrush);

}
void Graphics::fillRect(int l, int t, int r, int b, const Brush *brush)
{
  RECT rect;

  rect.top = t;
  rect.left = l;
  rect.bottom = b;
  rect.right = r;

  FillRect(m_hdc, &rect, brush->m_brush);
}

void Graphics::ellipse(int l, int t, int r, int b)
{
  Ellipse(m_hdc, l, t, r, b);
}

void Graphics::rectangle(int l, int t, int r, int b)
{
  Rectangle(m_hdc, l, t, r, b);
}

void Graphics::drawBitmap(const Bitmap *bitmap, int x, int y, int w, int h)
{
  HDC hdcMem = ::CreateCompatibleDC(m_hdc);

  HGDIOBJ oldBitmap = ::SelectObject(hdcMem, bitmap->m_bitmap);

  BitBlt(m_hdc, x, y, w, h, hdcMem, 0, 0, SRCCOPY);

  ::SelectObject(hdcMem, oldBitmap);

   ::DeleteDC(hdcMem);
}

void Graphics::drawText(const TCHAR *text, int cchText, RECT *rect, UINT format)
{
  DrawText(m_hdc, text, cchText, rect, format);
}


GraphicsPlus::GraphicsPlus(Gdiplus::Graphics * pgraphics)
: m_pgraphics(pgraphics)
{
   m_ppen = nullptr;

   m_pgraphics->SetCompositingQuality(Gdiplus::CompositingQualityHighQuality);
}

void GraphicsPlus::set_copy_source_mode()
{
   m_pgraphics->SetCompositingMode(Gdiplus::CompositingModeSourceCopy);
}
void GraphicsPlus::set_blend_mode()
{

   m_pgraphics->SetCompositingMode(Gdiplus::CompositingModeSourceOver);

}

void GraphicsPlus::set_antialias_off()
{

   m_pgraphics->SetSmoothingMode(Gdiplus::SmoothingModeNone);

}


void GraphicsPlus::set_antialias_on()
{

   m_pgraphics->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

}


GraphicsPlus::~GraphicsPlus()
{
   if (m_ppen)
   {

      delete m_ppen;

   }
}

void GraphicsPlus::setPen(FLOAT fWidth, const color::color &color)
{

   if (m_ppen)
   {

      delete m_ppen;

   }

   m_ppen = new Gdiplus::Pen(Gdiplus::Color(color.byte_opacity(),
      color.byte_red(),
      color.byte_green(),
      color.byte_blue()), fWidth);

}

// void Graphics::setBkMode(bool transparent)
// {
//   SetBkMode(m_hdc, transparent ? TRANSPARENT : OPAQUE);
// }
//
// void Graphics::setTextColor(COLORREF color)
// {
//   SetTextColor(m_hdc, color);
// }
//
// void Graphics::setBkColor(COLORREF color)
// {
//   SetBkColor(m_hdc, color);
// }
//
// void Graphics::setBrush(const Brush *brush)
// {
//   HGDIOBJ object = (brush != 0) ? brush->m_brush : 0;
//   m_dc->selectObject(object);
// }
//
// void Graphics::setPen(const Pen *pen)
// {
//   HGDIOBJ object = (pen != 0) ? pen->m_pen : 0;
//   m_dc->selectObject(object);
// }
//
 void GraphicsPlus::moveTo(int x, int y)
 {
//   MoveToEx(m_hdc, x, y, NULL);
    m_xLast = x;
    m_yLast = y;
 }
//
// void Graphics::lineTo(int x, int y)
// {
//   LineTo(m_hdc, x, y);
//    m_xLast = x;
//    m_yLast = y;
// }

void GraphicsPlus::lineTo(int x, int y)
{
   //Gdiplus::Graphics g(m_hdc);
   m_pgraphics->DrawLine(m_ppen, m_xLast, m_yLast, x, y);
   m_xLast = x;
   m_yLast = y;
}

void GraphicsPlus::fill_solid_rectangle(const ::int_rectangle & rectangle, const ::color::color & color)
{

   //::Gdiplus::Graphics g(m_hdc);

   //r.deflate(2, 2);
   Gdiplus::Rect gdiplusr;
   ::copy(gdiplusr, rectangle);
   Gdiplus::SolidBrush brush(Gdiplus::Color(color.byte_opacity(), color.byte_red(), color.byte_green(), color.byte_blue()));
   m_pgraphics->FillRectangle(&brush, gdiplusr);
   // // Set background color
   // COLORREF oldColor = SetBkColor(m_hdc, RGB(color.byte_red(), color.byte_green(), color.byte_blue()));
   //
   // RECT r;
   // ::copy(r, rectangle);
   // // Fill rectangle using ETO_OPAQUE
   // ExtTextOut(
   // m_hdc,
   //     0,              // x (ignored when no text)
   //     0,              // y (ignored when no text)
   //     ETO_OPAQUE,
   //     &r,            // rectangle to fill
   //     nullptr,        // no text
   //     0,              // text length
   //     nullptr         // dx array
   // );
   //
   // // Restore previous background color
   // SetBkColor(m_hdc, oldColor);
}

// void Graphics::fillRect(const ::int_rectangle & r, const Brush *pbrush)
// {
//
//    fillRect(r.left, r.top, r.right, r.bottom, pbrush);
//
// }
// void Graphics::fillRect(int l, int t, int r, int b, const Brush *brush)
// {
//   RECT rect;
//
//   rect.top = t;
//   rect.left = l;
//   rect.bottom = b;
//   rect.right = r;
//
//   FillRect(m_hdc, &rect, brush->m_brush);
// }
//
// void Graphics::ellipse(int l, int t, int r, int b)
// {
//   Ellipse(m_hdc, l, t, r, b);
// }
//
// void Graphics::rectangle(int l, int t, int r, int b)
// {
//   Rectangle(m_hdc, l, t, r, b);
// }

// void GraphicsPlus::drawBitmap(const Bitmap *bitmap, int x, int y, int w, int h)
// {
//   DeviceContext memDC(m_dc);
//
//   HGDIOBJ oldBitmap = memDC.selectObject(bitmap->m_bitmap);
//
//   BitBlt(m_hdc, x, y, w, h, memDC.m_dc, 0, 0, SRCCOPY);
//
//   memDC.selectObject(oldBitmap);
// }

// void GraphicsPlus::drawText(const TCHAR *text, int cchText, RECT *rect, UINT format)
// {
//   DrawText(m_hdc, text, cchText, rect, format);
// }
