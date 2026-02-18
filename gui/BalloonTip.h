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

#pragma once


#include "remoting/util/CommonHeader.h"
#include "Tooltip.h"
#include <commctrl.h>

/**
 * @deprecated, use ToolTip instead.
 */
class BalloonTip : public Tooltip
{
public:
  BalloonTip(const ::scoped_string & scopedstrText, const ::scoped_string & scopedstrCaption);
  BalloonTip();
  virtual ~BalloonTip();

  void showTooltip(::remoting::Window *control);

  void setText(const ::scoped_string & scopedstrText);
  void setTitle(const ::scoped_string & scopedstrCaption);

  void get_text(::string & text) const;
  void getTitle(::string & title) const;

  void setIconType(int iconType);
  int getIconType() const;

protected:
  ::string m_text;
  ::string m_title;
};


