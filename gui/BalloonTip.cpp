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
#include "BalloonTip.h"

BalloonTip::BalloonTip(const ::scoped_string & scopedstrtext, const ::scoped_string & scopedstrcaption)
{
  setText(text);
  setTitle(caption);
}

BalloonTip::BalloonTip()
{
  setText("");
  setTitle("");
}

BalloonTip::~BalloonTip()
{
}

void BalloonTip::showTooltip(Control *control)
{
  MessageBox(control->get_hwnd(), m_text, m_title, MB_OK | MB_ICONWARNING);
}

void BalloonTip::setText(const ::scoped_string & scopedstrtext)
{
  m_text= text;
}

void BalloonTip::setTitle(const ::scoped_string & scopedstrtitle)
{
  m_title= title;
}

void BalloonTip::getText(::string & text) const
{
  text-= m_text;
}

void BalloonTip::getTitle(::string & title) const
{
  title-= m_title;
}

void BalloonTip::setIconType(int iconType)
{
}

int BalloonTip::getIconType() const
{
  return 0;
}
