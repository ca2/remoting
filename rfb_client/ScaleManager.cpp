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
//#include "acme/_operating_system.h"
#include "ScaleManager.h"

namespace remoting_rfb_client
{
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

    ::i32 ScaleManager::sDiv(::i32 x, ::i32 y) const
    {
        return (x + y - 1) / y;
    }

    void ScaleManager::setScreenResolution(::i32 maxWidth, ::i32 maxHeight)
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

    void ScaleManager::keepAspectRatio(::i32_rectangle &rectangle) const
    {
        ::i32 iHeight = rectangle.height() / m_scrHScale;
        ::i32 iWidth = rectangle.width() / m_scrWScale;
        ::i32 iBar = minimum(iHeight, iWidth);
        rectangle.right = rectangle.left + iBar * m_scrWScale;
        rectangle.bottom = rectangle.top + iBar * m_scrHScale;
    }

    void ScaleManager::setScale(::i32 scale)
    {
        m_scale = scale;
        setStartPoint(m_xStart, m_yStart);
    }

    void ScaleManager::setWindow(const ::i32_rectangle & rectangleWnd)
    {
        m_rcWindow = rectangleWnd;
        setStartPoint(m_xStart, m_yStart);
    }

    ::f32 ScaleManager::getScale() const
    {

        return (::f32) m_scale / (::f32) DEFAULT_SCALE_DENOMERATOR;

    }

    ::i32_rectangle ScaleManager::calcScaled(const ::i32_rectangle &  rcViewed, bool bCent)
    {
        ::i32_rectangle rcScaled;
        // calculate scaled window from viewed
        rcScaled = rcViewed;
        ::i32 scale = m_scale;
        ::i32 denomeratorScale = DEFAULT_SCALE_DENOMERATOR;
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

    bool ScaleManager::getVertPages(::i32 iHeight) const
    {
        if (m_scale == -1) {
            return false;
        }
        ::i32 lenScr = sDiv(m_scrHeight * m_scale, DEFAULT_SCALE_DENOMERATOR);
        ::i32 result = sDiv(lenScr, iHeight);
        if (result > 1) {
            return true;
        }
        return false;
    }

    bool ScaleManager::getHorzPages(::i32 iWidth) const
    {
        if (m_scale == -1) {
            return false;
        }
        ::i32 lenScr = sDiv(m_scrWidth * m_scale, DEFAULT_SCALE_DENOMERATOR);
        ::i32 result = sDiv(lenScr, iWidth);
        if (result > 1) {
            return true;
        }
        return false;
    }

    ::i32 ScaleManager::getVertPoints() const
    {
        if (m_scale == -1) {
            return 0;
        }
        return sDiv(m_scrHeight * m_scale, DEFAULT_SCALE_DENOMERATOR);
    }

    ::i32 ScaleManager::getHorzPoints() const
    {
        if (m_scale == -1) {
            return 0;
        }
        return sDiv(m_scrWidth * m_scale, DEFAULT_SCALE_DENOMERATOR);
    }

    void ScaleManager::setStartPoint(::i32 x, ::i32 y)
    {
        ::i32 wndWidth = m_rcWindow.width();
        ::i32 wndHeight = m_rcWindow.height();

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
        ::i32_rectangle rcScaled = calcScaled(m_rcViewed, true);
    }

    void ScaleManager::getViewedRect(::i32_rectangle & rectangleViewed) const
    {
        ::i32_rectangle rectangle(m_rcViewed);
        rectangle.set_top_left(m_iCentX, m_iCentY);

        rectangleViewed = rectangle;
    }

    ::i32_rectangle ScaleManager::getViewedRect() const
    {

        i32_rectangle r;
        getViewedRect(r);
        return r;

    }

    ::i32_rectangle ScaleManager::getScaledRect()
    {
        return calcScaled(m_rcViewed, true);
    }

    void ScaleManager::getDestinationRect(::i32_rectangle & rectangleDestination)
    {
        ::i32_rectangle rcScaled = calcScaled(m_rcViewed, true);
        rcScaled.set_top_left(m_iCentX, m_iCentY);

        rectangleDestination = rcScaled;
    }

    void ScaleManager::getSourceRect(::i32_rectangle & rectangleSource) const
    {
        rectangleSource = m_rcViewed;
    }

    void ScaleManager::getWndFromScreen(const ::i32_rectangle &  screen, ::i32_rectangle &rectangleWnd)
    {
        ::i32_rectangle scr = screen;
        scr.offset(-m_rcViewed.left, -m_rcViewed.top);
        ::i32_rectangle rcScaled = calcScaled(scr, false);
        rectangleWnd = rcScaled;
        rectangleWnd.offset(m_iCentX, m_iCentY);
    }

    ::i32_point ScaleManager::transformDispToScr(::i32 xPoint, ::i32 yPoint) const
    {
        xPoint -= m_iCentX;
        yPoint -= m_iCentY;

        ::i32 scale = m_scale;
        ::i32 denomeratorScale = DEFAULT_SCALE_DENOMERATOR;
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

        ::i32_point point;
        point.x = static_cast<::i16>(xPoint + m_rcViewed.left);
        point.y = static_cast<::i16>(yPoint + m_rcViewed.top);

        return point;
    }
} // namespace remoting_rfb_client
