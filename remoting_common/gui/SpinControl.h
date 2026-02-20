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


#include "remoting/remoting_common/gui/Window.h"
#include <commctrl.h>

class CLASS_DECL_REMOTING_COMMON SpinControl : public ::remoting::Window
{
public:
  SpinControl();
  ~SpinControl();

  void setBuddy(::remoting::Window *buddyControl);
  void setRange(short lower, short upper);
  void setRange32(int lower, int upper);
  void setAccel(UINT nSec, UINT nInc);

  //
  // Auto acceleration methods
  //

  //
  // Handler, call it on UDN_DELTAPOS notification
  //

  void autoAccelerationHandler(LPNMUPDOWN scopedstrMessage);
  void enableAutoAcceleration(bool enabled);
  void setAutoAccelerationParams(const ::array_base<int> *limitters,
                                 const ::array_base<int> *deltas,
                                 int maxDelta);
protected:
  ::remoting::Window *m_buddy;

  //
  // Members needed for auto acceleration
  //

  bool m_isAutoAccelerationEnabled;
  ::array_base<int> m_limitters;
  ::array_base<int> m_deltas;
  int m_maxDelta;
};


