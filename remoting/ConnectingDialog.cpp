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
// From ConnectingDialog.cpp by
// camilo on 2026-02-12 21:12 <3ThomasBorregaardSørensen!!
#include "framework.h"
#include "ConnectingDialog.h"
#include "resource.h"
#include "remoting/gui/ProgressBar.h"
#include "remoting/gui/TextBox.h"
#include "acme/platform/application.h"


ConnectingDialog::ConnectingDialog()
: BaseDialog(IDD_CONNECTING)
{

   øconstruct_new(m_ptextboxHost);
   øconstruct_new(m_ptextboxStatus);
}


BOOL ConnectingDialog::onInitDialog()
{
  subclassControlById(m_ptextboxHost, IDC_EHOST);
  m_ptextboxHost->setText(m_strHost);
  subclassControlById(m_ptextboxStatus, IDC_EPASSW);
  m_ptextboxStatus->set_focus();
  return FALSE;
}

void ConnectingDialog::set_host(const ::scoped_string &hostname) {
  m_strHost = hostname;
}

void ConnectingDialog::set_status(const ::scoped_string &status) {
   m_strStatus = status;
}

BOOL ConnectingDialog::onCommand(UINT controlID, UINT notificationID)
{
  if (controlID == IDOK) {
    close_dialog(1);
    return TRUE;
  }
  if (controlID == IDCANCEL) {
    close_dialog(0);
    return TRUE;
  }
  return FALSE;
}
// class progress_bar_animation :
// virtual public ::particle
// {
// public:

   // ProgressBar*m_pbar;
   // double m_dStart = 0.0;
   // double m_dEnd = 0.0;
   // class ::time & m_time;


   progress_bar_animation::progress_bar_animation()
   {

      m_pbar = nullptr;
   }

   progress_bar_animation::~progress_bar_animation()
   {


   }

void progress_bar_animation::set_animation_range(double dStart, double dEnd)
{

   m_dStart = dStart;

   m_dEnd = dEnd;

   if (!m_bRunning)
   {
      m_bRunning = true;
      m_papplication->fork([this]()
      {
try
{
   run();
}
         catch (...)
         {

            m_bRunning = false;
         }

      });
   }

}


void progress_bar_animation::run()
{

while (m_bRunning)
{
   m_d = m_time.elapsed().floating_second();
   auto d= fmod(m_d, m_dEnd - m_dStart) + m_dStart;
   ::PostMessage(m_pbar->get_hwnd(), WM_USER + 327, (int) (d * 8'000.0), 0);
   preempt(100_ms);
}

}
//
// };

// const ::scoped_string & ConnectingDialog::get_status()
// {
//   return m_strPassword;
// }
void ConnectingDialog::_start_animating_progress_range(double dStart, double dEnd)
{

   ødefer_construct_new(m_panimation);

   m_panimation->set_animation_range(dStart, dEnd);
   //m_dAnimationStart = dStart;
   //m_dAnimationEnd = dEnd;
}
void ConnectingDialog::set_phase1()
{


   _start_animating_progress_range(0.0, 0.2);
}

