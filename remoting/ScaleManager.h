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
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//

#pragma once



#include "acme/prototype/geometry2d/rectangle.h"


class ScaleManager 
{
public:
  ScaleManager();

  // set resolution of the screen
  void setScreenResolution(int maxWidth, int maxHeight);
  // set the scale of image, for example, 10 - it means 10% from original
  void setScale(int scale);
  // set the rectangle of window
  void setWindow(const ::int_rectangle & rectangleWnd);

  // need to know for scrolling 
  // false -> single page
  // true -> multiple pages
  bool getVertPages(int iHeight) const;
  bool getHorzPages(int iWidth) const;

  // how much to scroll
  int getVertPoints() const;
  int getHorzPoints() const;

  // set the current starting point
  // point is scaled
  void setStartPoint(int x, int y);

  // get viewed rectangle
  void getViewedRect(::int_rectangle * prectangleViewed) const;

  // get scaled rectangle
  ::int_rectangle getScaledRect();

  // get source rectangle
  void getSourceRect(::int_rectangle * prectangleSource) const;

  // get destination rectangle
  void getDestinationRect(::int_rectangle * prectangleDestination);
 
  // get window rectangle from screen
  void getWndFromScreen(const ::int_rectangle &  screen, ::int_rectangle *wnd);

  // transform display coordinate to screen
  POINTS transformDispToScr(int xPoint, int yPoint) const;

  static const int DEFAULT_SCALE_DENOMERATOR = 100;
protected:
  // This method return round to up of (x/y).
  int sDiv(int x, int y) const;

  // size of window with frame buffer
  ::int_rectangle m_rcWindow;
  // size of visible part of the frame buffer considering scale
  ::int_rectangle m_rcViewed;

  // position of m_rcViewer
  int m_iCentX;
  int m_iCentY;

  int m_xStart;
  int m_yStart;

  // size of frame buffer
  int m_scrWidth;
  int m_scrHeight;

  int m_scrWScale;
  int m_scrHScale;

  int m_scale;

private:
  ::int_rectangle calcScaled(const ::int_rectangle &  rcViewed, bool bCent);
  void keepAspectRatio(::int_rectangle *prectangle) const;

};


