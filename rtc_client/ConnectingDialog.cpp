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
// From ConnectingDialog.cpp by
// camilo on 2026-02-12 21:12 <3ThomasBorregaardSørensen!!
#include "framework.h"
#include "ConnectingDialog.h"
#include "resource.h"
#include "acme/constant/id.h"
#include "innate_subsystem/gui/ProgressBar.h"
#include "innate_subsystem/gui/TextBox.h"
#include "acme/platform/application.h"

namespace remoting_client
{
   ConnectingDialog::ConnectingDialog()
   //: Dialog(IDD_CONNECTING)
   {

      initialize_dialog(IDD_CONNECTING);
   }


   bool ConnectingDialog::onInitDialog()
   {

      //constructø(m_ptextboxHost);
      //constructø(m_ptextboxStatus);
      construct_newø(m_panimation);
      //constructø(m_panimation->m_pbar);

      subclassControlById(m_textboxHost, IDC_HOST);
      m_textboxHost.setText(m_strHost);
      subclassControlById(m_textboxStatus, IDC_STATUS);
      m_textboxStatus.setFocus();
      defer_construct_newø(m_panimation);
      subclassControlById(m_panimation->m_bar, IDC_PROGRESS1);
      m_panimation->m_bar.setRange(0, 8000);
      return false;
   }

   void ConnectingDialog::set_host(const ::scoped_string &hostname) {
      m_strHost = hostname;
      if (m_textboxHost)
      {
         m_textboxHost.setText(m_strHost);
      }
   }

   void ConnectingDialog::set_status(const ::scoped_string &status) {
      m_strStatus = status;
      if (m_textboxStatus)
      {
         m_textboxStatus.setText(m_strStatus);
      }
   }

   bool ConnectingDialog::onCommand(::u32 controlID, ::u32 notificationID)
   {
      if (controlID == ::innate_subsystem::e_control_id_ok) {
         closeDialog(1);
         return true;
      }
      if (controlID == ::innate_subsystem::e_control_id_cancel) {
         closeDialog(0);
         return true;
      }
      return false;
   }
   // class progress_bar_animation :
   // virtual public ::particle
   // {
   // public:

   // ProgressBar*m_pbar;
   // ::f64 m_dStart = 0.0;
   // ::f64 m_dEnd = 0.0;
   // class ::time & m_time;


   progress_bar_animation::progress_bar_animation()
   {

      //m_pbar = nullptr;
   }

   progress_bar_animation::~progress_bar_animation()
   {


   }

   void progress_bar_animation::set_animation_range(::f64 dStart, ::f64 dEnd)
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
         m_f64 = m_time.elapsed().floating_second();
         auto d= fmod(m_f64 * 0.125, m_dEnd - m_dStart) + m_dStart;
         m_bar.postMessage(WM_USER + 327, (::i32) (d * 8'000.0), 0);
         preempt(100_ms);
      }

   }
   //
   // };

   // const ::scoped_string & ConnectingDialog::get_status()
   // {
   //   return m_strPassword;
   // }
   void ConnectingDialog::_start_animating_progress_range(::f64 dStart, ::f64 dEnd)
   {

      defer_construct_newø(m_panimation);

      m_panimation->set_animation_range(dStart, dEnd);
      //m_dAnimationStart = dStart;
      //m_dAnimationEnd = dEnd;
   }
   void ConnectingDialog::set_phase1()
   {


      _start_animating_progress_range(0.0, 0.4);
   }

   void ConnectingDialog::set_connecting(::i32 iPhase)
   {

      if (iPhase == 1)
      {
         set_status("Authenticating...");
      }
      else
      {


      }
      _start_animating_progress_range(0.4, 0.6);
   }


   bool ConnectingDialog::dialog_procedure(iptr & iptrResult, ::u32 message, ::wparam wparam, ::lparam lparam)
   {

      if (message == WM_USER + 328)
      {

         if (wparam == id_remoting_connecting)
         {

            set_connecting((::i32) lparam.m_lparam);

         }

         iptrResult = true;

         return true;


      }


      return Dialog::dialog_procedure(iptrResult, message, wparam, lparam);

   }
} // namespace remoting_client
