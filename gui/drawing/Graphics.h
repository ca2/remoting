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
// with this program; if not, w_rite to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//

#pragma once


#include "remoting/util/CommonHeader.h"

#include "DeviceContext.h"
#include "Bitmap.h"
#include "Brush.h"
#include "Pen.h"
#include <gdiplus.h>


namespace remoting
{
   class Graphics
   {
   public:
      // Creates graphics object with specified device context.
      //Graphics(DeviceContext *dc);
      Graphics(HDC hdc);
      // Graphics class destructor.
      virtual ~Graphics();

      // Sets background colors mix mode.
      void setBkMode(bool transparent);
      // Sets background color.
      void setBkColor(COLORREF color);

      // Sets text color.
      void setTextColor(COLORREF color);
      // Sets current brush.
      void setBrush(const Brush *brush);
      // Sets current pen.
      void setPen(const Pen *pen);
      //void setExPen(FLOAT fWidth, const ::color::color & color);

      // Moves cursor to specified position.
      void moveTo(int x, int y);
      // Draws line from current position to specified line.
      void lineTo(int x, int y);
      //void ExLineTo(int x, int y);

      // Draws filled rect.
      void fillRect(const ::int_rectangle & r, const Brush *brush);
      void fillRect(int l, int t, int r, int b, const Brush *brush);
      //void fill_solid_rectangle( const ::int_rectangle & rectangle, const ::color::color & color);

      // Draws ellipse.
      void ellipse(int l, int t, int r, int b);
      // Draws rectance.
      void rectangle(int l, int t, int r, int b);

      // Draws bitmap.
      void drawBitmap(const Bitmap *bitmap, int x, int y, int w, int h);
      // Draws text.
      void drawText(const ::scoped_string & scopedstrText, int cchText, RECT *rect, UINT format);

      //protected:
      //DeviceContext *m_dc;
      HDC m_hdc;
   };


   class GraphicsPlus
   {
   public:
      // Creates graphics object with specified device context.
      GraphicsPlus(Gdiplus::Graphics *pgraphics);
      // Graphics class destructor.
      virtual ~GraphicsPlus();

      void set_copy_source_mode();
      void set_blend_mode();
      void set_antialias_on();
      void set_antialias_off();
      // // Sets background colors mix mode.
      // void setBkMode(bool transparent);
      // // Sets background color.
      // void setBkColor(COLORREF color);
      //
      // // Sets text color.
      // void setTextColor(COLORREF color);
      // // Sets current brush.
      // void setBrush(const Brush *brush);
      // // Sets current pen.
      // void setPen(const Pen *pen);
      void setPen(FLOAT fWidth, const ::color::color & color);

      // Moves cursor to specified position.
      void moveTo(int x, int y);
      // Draws line from current position to specified line.
      void lineTo(int x, int y);
      //void ExLineTo(int x, int y);

      // Draws filled rect.
      //void fillRect(const ::int_rectangle & r, const Brush *brush);
      //void fillRect(int l, int t, int r, int b, const Brush *brush);
      void fill_solid_rectangle( const ::int_rectangle & rectangle, const ::color::color & color);

      // Draws ellipse.
      //void ellipse(int l, int t, int r, int b);
      // Draws rectance.
      //void rectangle(int l, int t, int r, int b);

      // Draws bitmap.
      //void drawBitmap(const Bitmap *bitmap, int x, int y, int w, int h);
      // Draws text.
      //void drawText(const ::scoped_string & scopedstrText, int cchText, RECT *rect, UINT format);

      //protected:
      Gdiplus::Graphics * m_pgraphics;
      Gdiplus::Pen * m_ppen;
      int m_xLast = -1;
      int m_yLast = -1;
   };
}// namespace remoting



