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
#include "acme/_operating_system.h"
#include "ScaleManager.h"

ScaleManager::ScaleManager() 
: m_scale(DEFAULT_SCALE_DENOMERATOR),
  m_scrWidth(1),
  m_scrHeight(1),
  m_scrWScale(1),
  m_scrHScale(1),
  m_xStart(0),
  m_yStart(0),
  m_iCentX(0),
  m_iCentY(0)
{
}

int ScaleManager::sDiv(int x, int y) const
{
  return (x + y - 1) / y;
}

void ScaleManager::setScreenResolution(int maxWidth, int maxHeight)
{
  m_scrWidth = maxWidth;
  m_scrHeight = maxHeight;

  // part of Euclid algorithm
  while (!(maxWidth & 1) && !(maxHeight & 1)) {
    maxWidth = maxWidth >> 1;
    maxHeight = maxHeight >> 1;
  }

  maxWidth = maxWidth > 0 ? maxWidth : 1; 
  maxHeight = maxHeight > 0 ? maxHeight : 1;

  m_scrWScale = maxWidth;
  m_scrHScale = maxHeight;

  // renew other parameters
  setWindow(m_rcWindow);
}

void ScaleManager::keepAspectRatio(::int_rectangle *prectangle) const
{
  int iHeight = prectangle->height() / m_scrHScale;
  int iWidth = prectangle->width() / m_scrWScale;
  int iBar = minimum(iHeight, iWidth);
  prectangle->right = prectangle->left + iBar * m_scrWScale;
  prectangle->bottom = prectangle->top + iBar * m_scrHScale;
}

void ScaleManager::setScale(int scale)
{
  m_scale = scale;
  setStartPoint(m_xStart, m_yStart);
}

void ScaleManager::setWindow(const ::int_rectangle & rectangleWnd)
{
  m_rcWindow = rectangleWnd;
  setStartPoint(m_xStart, m_yStart);
}

::int_rectangle ScaleManager::calcScaled(const ::int_rectangle &  rcViewed, bool bCent)
{
  ::int_rectangle rcScaled;
  // calculate scaled window from viewed
  rcScaled = rcViewed;
  int scale = m_scale;
  int denomeratorScale = DEFAULT_SCALE_DENOMERATOR;
  if (m_scale == -1) {
    if (m_rcWindow.width() * m_scrHeight <= m_rcWindow.height() * m_scrWidth) {
      scale = m_rcWindow.width();
      denomeratorScale = m_scrWidth;
    } else {
      scale = m_rcWindow.height();
      denomeratorScale = m_scrHeight;
    }
  }
  rcScaled.left = rcScaled.left * scale / denomeratorScale;
  rcScaled.top = rcScaled.top * scale / denomeratorScale;
  rcScaled.right = sDiv(rcScaled.right * scale, denomeratorScale);
  rcScaled.bottom = sDiv(rcScaled.bottom * scale, denomeratorScale);

  if (bCent) {
    if (m_rcWindow.width() > rcScaled.width()) {
      m_iCentX = (m_rcWindow.width() - rcScaled.width()) / 2;
    } else {
      m_iCentX = 0;
    }
    if (m_rcWindow.height() > rcScaled.height()) {
      m_iCentY = (m_rcWindow.height() - rcScaled.height()) / 2;
    } else {
      m_iCentY = 0;
    }
  }
  return rcScaled;
}

bool ScaleManager::getVertPages(int iHeight) const
{
  if (m_scale == -1) {
    return false;
  }
  int lenScr = sDiv(m_scrHeight * m_scale, DEFAULT_SCALE_DENOMERATOR);
  int result = sDiv(lenScr, iHeight);
  if (result > 1) {
    return true;
  }
  return false;
}

bool ScaleManager::getHorzPages(int iWidth) const
{
  if (m_scale == -1) {
    return false;
  }
  int lenScr = sDiv(m_scrWidth * m_scale, DEFAULT_SCALE_DENOMERATOR);
  int result = sDiv(lenScr, iWidth);
  if (result > 1) {
    return true;
  }
  return false;
}

int ScaleManager::getVertPoints() const
{
  if (m_scale == -1) {
    return 0;
  }
  return sDiv(m_scrHeight * m_scale, DEFAULT_SCALE_DENOMERATOR);
}

int ScaleManager::getHorzPoints() const
{
  if (m_scale == -1) {
    return 0;
  }
  return sDiv(m_scrWidth * m_scale, DEFAULT_SCALE_DENOMERATOR);
}

void ScaleManager::setStartPoint(int x, int y)
{
  int wndWidth = m_rcWindow.width();
  int wndHeight = m_rcWindow.height();

  if (m_scale != -1) {
    x = x * DEFAULT_SCALE_DENOMERATOR / m_scale;
    y = y * DEFAULT_SCALE_DENOMERATOR / m_scale;

    wndWidth = wndWidth * DEFAULT_SCALE_DENOMERATOR / m_scale;
    wndHeight = wndHeight * DEFAULT_SCALE_DENOMERATOR / m_scale;
  } else {
    // scroll is off, is scale is Auto
    x = y = 0;
    wndWidth = m_scrWidth;
    wndHeight = m_scrHeight;
 }

  m_xStart = x;
  m_yStart = y;
  m_rcViewed.left = x;
  m_rcViewed.top = y;
  m_rcViewed.right = x + wndWidth;
  m_rcViewed.bottom = y + wndHeight;

  if (m_rcViewed.right > m_scrWidth) {
    m_rcViewed.right = m_scrWidth;
    m_rcViewed.left = m_scrWidth - wndWidth;
    if (m_rcViewed.left < 0) {
      m_rcViewed.left = 0;
    }
  }
  if (m_rcViewed.bottom > m_scrHeight) {
    m_rcViewed.bottom = m_scrHeight;
    m_rcViewed.top = m_scrHeight - wndHeight;
    if (m_rcViewed.top < 0) {
      m_rcViewed.top = 0;
    }
  }
  ::int_rectangle rcScaled = calcScaled(m_rcViewed, true);
}

void ScaleManager::getViewedRect(::int_rectangle * prectangleViewed) const
{
  ::int_rectangle rect(m_rcViewed);
  rect.set_top_left(m_iCentX, m_iCentY);

  *prectangleViewed = rect;
}

::int_rectangle ScaleManager::getScaledRect()
{
  return calcScaled(m_rcViewed, true);
}

void ScaleManager::getDestinationRect(::int_rectangle * prectangleDestination)
{
  ::int_rectangle rcScaled = calcScaled(m_rcViewed, true);
  rcScaled.set_top_left(m_iCentX, m_iCentY);

  *prectangleDestination = rcScaled;
}

void ScaleManager::getSourceRect(::int_rectangle * prectangleSource) const
{
  *prectangleSource = m_rcViewed;
}

void ScaleManager::getWndFromScreen(const ::int_rectangle &  screen, ::int_rectangle *prectangleWnd)
{
  ::int_rectangle scr = screen;
  scr.offset(-m_rcViewed.left, -m_rcViewed.top);
  ::int_rectangle rcScaled = calcScaled(scr, false);
  *prectangleWnd = rcScaled;
  prectangleWnd->offset(m_iCentX, m_iCentY);
}

POINTS ScaleManager::transformDispToScr(int xPoint, int yPoint) const
{
  xPoint -= m_iCentX;
  yPoint -= m_iCentY;

  int scale = m_scale;
  int denomeratorScale = DEFAULT_SCALE_DENOMERATOR;
  if (m_scale == -1) {
    if (m_rcWindow.width() * m_scrHeight <= m_rcWindow.height() * m_scrWidth) {
      scale = m_rcWindow.width();
      denomeratorScale = m_scrWidth;
    } else {
      scale = m_rcWindow.height();
      denomeratorScale = m_scrHeight;
    }
  }

  xPoint = xPoint * denomeratorScale / scale;
  yPoint = yPoint * denomeratorScale / scale;

  POINTS pt;
  pt.x = static_cast<SHORT>(xPoint + m_rcViewed.left);
  pt.y = static_cast<SHORT>(yPoint + m_rcViewed.top);

  return pt;
}
