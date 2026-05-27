// Copyright (C) 2010,2011,2012 GlavSoft LLC.
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
#include "ControlAppAuthenticator.h"
//#include "subsystem/thread/lockable_critical_section.h"
#include "subsystem/platform/VncPassCrypt.h"

namespace remoting_rfb_node_desktop
{


   ControlAppAuthenticator::ControlAppAuthenticator(const class ::time & timeFailureInterval,
                                                    ::u32 failureMaxCount) :
       AuthTracker(timeFailureInterval, failureMaxCount), m_isBreaked(false)
   {
   }

   ControlAppAuthenticator::~ControlAppAuthenticator() { breakAndDisableAuthentications(); }

   bool ControlAppAuthenticator::authenticate(const ::u8 cryptPassword[8], const ::u8 challenge[8],
                                              const ::u8 response[8])
   {
      critical_section_lock al(&m_authMutex);

      checkBeforeAuth();
      if (m_isBreaked)
      {
         return false;
      }

      ::subsystem::VncPassCrypt passCrypt;
      passCrypt.updatePlain(cryptPassword);
      bool isAuthSucceed = passCrypt.challengeAndResponseIsValid(challenge, response);

      if (!isAuthSucceed)
      {
         notifyAbAuthFailed();
      }

      return isAuthSucceed;
   }

   void ControlAppAuthenticator::checkBeforeAuth()
   {
      // Wait ban time before authentication
      ::u64 banTime = 1;
      while (banTime != 0 && !m_isBreaked)
      {
         banTime = checkBan();
         if (banTime != 0)
         {
            // m_banDelay.waitForEvent((DWORD)banTime);
            m_banDelay.wait(banTime * 1_s);
         }
      }
   }

   void ControlAppAuthenticator::breakAndDisableAuthentications()
   {
      m_isBreaked = true;
      // m_banDelay.notify();
      m_banDelay.set_happening();
   }


} // namespace remoting_rfb_node_desktop



