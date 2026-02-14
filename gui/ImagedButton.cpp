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
#include "ImagedButton.h"
#include "util/UnicodeStringStorage.h"
#include <crtdbg.h>

ImagedButton::ImagedButton()
: m_mouseOver(false),
  m_isUsingTheme(false), m_theme(0), m_icon(0),
  m_iconWidth(0), m_iconHeight(0)
{
}

ImagedButton::~ImagedButton()
{
  if (m_theme) {
    ThemeLib::CloseThemeData(m_theme);
    m_theme = NULL;
  }
}

void ImagedButton::drawItem(LPDRAWITEMSTRUCT dis)
{
  HDC dc = dis->hDC;

  // button state
  BOOL isPressed = (dis->itemState & ODS_SELECTED);
  BOOL isFocused = (dis->itemState & ODS_FOCUS);
  BOOL isDisabled = (dis->itemState & ODS_DISABLED);
  BOOL drawFocusRect = !(dis->itemState & ODS_NOFOCUSRECT);

  RECT itemRect = dis->rcItem;
  SetBkMode(dc, TRANSPARENT);

  // Prepare draw... paint button background

  if (m_isUsingTheme) {
    DWORD state = (isPressed) ? PBS_PRESSED : PBS_NORMAL;
    if (state == PBS_NORMAL) {
      if (isFocused) {
        state = PBS_DEFAULTED;
      } // is focused
      if (m_mouseOver) {
        state = PBS_HOT;
      } // is mouse over button
      if (isDisabled) {
        state = PBS_DISABLED;
      }
    } // if state is normal
    ThemeLib::DrawThemeBackground(m_theme, dc, BP_PUSHBUTTON, state, &itemRect, NULL);
  } else {
    if (isFocused) {
      HBRUSH br = CreateSolidBrush(RGB(0,0,0));
      FrameRect(dc, &itemRect, br);
      InflateRect(&itemRect, -1, -1);
      DeleteObject(br);
    } // if

    HBRUSH background = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
    FillRect(dc, &itemRect, background);
    DeleteObject(background);

    // Draw pressed button

    if (isPressed) {
      HBRUSH shadow = CreateSolidBrush(GetSysColor(COLOR_BTNSHADOW));
      FrameRect(dc, &itemRect, shadow);
      DeleteObject(shadow);
    } else {
      UINT uState = DFCS_BUTTONPUSH |
                    ((m_mouseOver) ? DFCS_HOT : 0) |
                    ((isPressed) ? DFCS_PUSHED : 0);

      DrawFrameControl(dc, &itemRect, DFC_BUTTON, uState);
    } // if not pressed
  } // if not themed

  ::string title;
  getText(&title);

  RECT captionRect = dis->rcItem;

  //
  // Draw icon
  //

  TEXTMETRIC metric;
  GetTextMetrics(dc, &metric);

  RECT prectangleImage;

  calcRect(&itemRect, isPressed == TRUE, 0, metric.tmHeight,
           m_iconWidth, m_iconHeight, &captionRect, &prectangleImage);

  if (m_icon != NULL) {
    drawIcon(&dc, &prectangleImage, isPressed == TRUE, isDisabled == TRUE);
  }

  if (!title.is_empty()) {
    // If button is pressed then "press" title also
    if (isPressed && !m_isUsingTheme) {
      OffsetRect(&captionRect, 1, 1);
    }

    if (m_isUsingTheme) {
      UnicodeStringStorage uniTitle(title);

      DWORD state = PBS_NORMAL;

      if (isDisabled) {
        state = PBS_DISABLED;
      }

      ThemeLib::DrawThemeText(m_theme, dc, BP_PUSHBUTTON, state,
                              uniTitle, (int)uniTitle.getLength(),
                              DT_CENTER | DT_VCENTER | DT_SINGLELINE,
                              0, &captionRect);
    } else {
      SetBkMode(dc, TRANSPARENT);

      if (isDisabled) {
        OffsetRect(&captionRect, 1, 1);
        SetTextColor(dc, ::GetSysColor(COLOR_3DHILIGHT));
        DrawText(dc, title, -1, &captionRect, DT_WORDBREAK | DT_CENTER);
        OffsetRect(&captionRect, -1, -1);
        SetTextColor(dc, ::GetSysColor(COLOR_3DSHADOW));
        DrawText(dc, title, -1, &captionRect, DT_WORDBREAK | DT_CENTER);
      } else {
        SetTextColor(dc, ::GetSysColor(COLOR_BTNTEXT));
        SetBkColor(dc, ::GetSysColor(COLOR_BTNFACE));
        DrawText(dc, title, -1, &captionRect, DT_WORDBREAK | DT_CENTER);
      } // if not disabled
    } // if not themed
  } // if has title

  // Draw the focus rect
  if (isFocused && drawFocusRect) {
    RECT focusRect = itemRect;
    InflateRect(&focusRect, -3, -3);
    DrawFocusRect(dc, &focusRect);
  } // if
} // void

void ImagedButton::setWindow(HWND hwnd)
{
  Control::setWindow(hwnd);
  // Replace window event handler
//  replaceWindowProc(ImagedButton::wndProc);
  // Add owner draw style to button
  Control::addStyle(BS_OWNERDRAW);

  if (ThemeLib::isLoaded())  {
    if (m_theme) {
      ThemeLib::CloseThemeData(m_theme);
    }
    m_theme = ThemeLib::OpenThemeData(hwnd, L"Button");
    if (m_theme) {
      m_isUsingTheme = true;
    }
  }
}

void ImagedButton::setIcon(HICON *icon, int width, int height)
{
  m_icon = icon;
  m_iconWidth = width;
  m_iconHeight = height;
}

void ImagedButton::calcRect(RECT* prectangleButton, bool isButtonPressed,
                            DWORD textWidth, DWORD textHeight,
                            DWORD imageWidth, DWORD imageHeight,
                            RECT *prectangleText, RECT* prectangleImage)
{
  CopyRect(prectangleImage, prectangleButton);
  CopyRect(prectangleText, prectangleButton);

  if (m_icon != NULL) {
    long buttonWidth = prectangleButton->right - prectangleButton->left;
    long buttonHeight = -prectangleButton->top + prectangleButton->bottom;

    // Center image horizontally
    prectangleImage->left += ((buttonWidth - (long)imageWidth) / 2);
    // Center image vertically
    prectangleImage->top += (((buttonHeight) - (long)imageHeight) / 2) - textHeight;

    DWORD margin = 10;
    prectangleText->top += (textHeight + margin) * 2;
  }

  // If button is pressed then press image also
  if (isButtonPressed && !m_isUsingTheme) {
    OffsetRect(prectangleImage, 1, 1);
  }
}

void ImagedButton::drawIcon(HDC* dc, RECT* prectangleImage, bool isPressed, bool isDisabled)
{
  DrawState(*dc, NULL, NULL, (LPARAM)*m_icon, 0,
            prectangleImage->left, prectangleImage->top,
            (prectangleImage->right - prectangleImage->left),
            (prectangleImage->bottom - prectangleImage->top), 
            (isDisabled ? DSS_DISABLED : DSS_NORMAL) | DST_ICON);
} // End of drawIcon

bool ImagedButton::window_procedure(LRESULT &lresul, UINT message, ::wparam wparam, ::lparam lparam)
{
  //ImagedButton *_this = (ImagedButton *)GetWindowLongPtr(hWnd, GWLP_USERDATA);

  switch (message) {
  case WM_LBUTTONDBLCLK:
    post_message(WM_LBUTTONDOWN, wparam, lparam);
  break;
  case WM_MOUSEMOVE:
    if (!m_mouseOver) {
      m_mouseOver = true;

      TRACKMOUSEEVENT evt;

      evt.cbSize = sizeof(evt);
      evt.dwFlags = TME_LEAVE;
      evt.dwHoverTime = HOVER_DEFAULT;
      evt.hwndTrack = m_hwnd;

      invalidate();

      TrackMouseEvent(&evt);
    }
    break;
  case WM_MOUSELEAVE:
    m_mouseOver = false;
    invalidate();
    break;
  } // switch
  // Any messages we don't process must be passed onto the original window function
   return false;
  //return CallWindowProc((WNDPROC)_this->m_defWindowProc, hWnd, message, wParam, lParam);
}
