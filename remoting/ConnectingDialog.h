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
// From AuthenticationDialog.h by 
// camilo on 2026-02-12 21:12 <3ThomasBorregaardSørensen!!
#pragma once

#include "gui/BaseDialog.h"
//#include "gui/ProgressBar.h"
//#include "gui/TextBox.h"
//#include "resource.h"

class BaseDialog;
class ProgressBar;
class TextBox;

class progress_bar_animation :
virtual public ::particle
{
public:

   ::pointer < ProgressBar>m_pbar;
   double m_dStart = 0.0;
   double m_dEnd = 0.0;
   double m_d;
   class ::time & m_time;


   progress_bar_animation();
   ~progress_bar_animation();

   void set_animation_range(double dStart, double dEnd);

   void run() override;

};


class ConnectingDialog : public BaseDialog
{
public:


   ConnectingDialog();


  // this function returns sets the name of host in dialog
  void set_host(const ::scoped_string & scopedstrHost);
  void set_status(const ::scoped_string &scopedstrStatus);
  void _start_animating_progress_range(double dStart, double dEnd);
   void set_phase1();

//protected:
  BOOL onCommand(UINT controlID, UINT notificationID);
  BOOL onInitDialog();

  ::pointer < ProgressBar > m_pprogressbar;
  ::pointer < TextBox > m_ptextboxHost;
  ::pointer < TextBox > m_ptextboxStatus;
  ::string  m_strHost;
  ::string m_strStatus;
};

