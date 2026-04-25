// Copyright (C) 2010,2011,2012 GlavSoft LLC.
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
#include "SasUserInput.h"
//#include "remoting/remoting/win_system/Environment.h"
#include "subsystem_windows/node/WTS.h"
#include "subsystem_windows/platform/subsystem.h"

#include "subsystem/platform/subsystem.h"
#include "subsystem/node/OperatingSystem.h"
#define XK_MISCELLANY
#include "remoting/remoting/rfb/keysymdef.h"
#include "subsystem_windows/node/WTS.h"
#include "subsystem_windows/platform/subsystem.h"


namespace remoting
{


   SasUserInput::SasUserInput() :
        m_ctrlPressed(false), m_altPressed(false), m_underVista(false)
   {
      m_underVista = MainSubsystem().OperatingSystem().isVistaOrLater();
   }

   SasUserInput::~SasUserInput() {}

   void SasUserInput::initialize_sas_user_input(UserInputClient *pclient, ::subsystem::LogWriter * plogwriter)
   {
       //m_client(client), m_ctrlPressed(false), m_altPressed(false), m_underVista(false), m_plogwriter = plogwriter;
       //{
          m_client = pclient;
       m_plogwriter = plogwriter;
      //m_underVista = MainSubsystem().OperatingSystem().isVistaOrLater();
   }

   void SasUserInput::sendInit(BlockingGate *pblockinggate) { m_client->sendInit(pblockinggate); }

   void SasUserInput::setMouseEvent(const ::int_point newPos, unsigned char keyFlag)
   {
      m_client->setMouseEvent(newPos, keyFlag);
   }

   void SasUserInput::setNewClipboard(const ::scoped_string &newClipboard) { m_client->setNewClipboard(newClipboard); }

   void SasUserInput::setKeyboardEvent(unsigned int keySym, bool down)
   {
      bool delPressed = false;

      if (m_underVista)
      {
         switch (keySym)
         {
            case XK_Alt_L:
            case XK_Alt_R:
               m_altPressed = down;
               break;
            case XK_Control_L:
            case XK_Control_R:
               m_ctrlPressed = down;
               break;
            case XK_Delete:
               delPressed = down;
         }
      }

      if (m_ctrlPressed && m_altPressed && delPressed && m_underVista)
      {
         DWORD sessionId = WindowsSubsystem().WTS().getActiveConsoleSessionId(m_plogwriter);
         bool isRdp = WindowsSubsystem().WTS().SessionIsRdpSession(sessionId, m_plogwriter);
         if (!isRdp)
         {
            MainSubsystem().OperatingSystem().simulateCtrlAltDelUnderVista(m_plogwriter);
            return;
         }
      }
      m_client->setKeyboardEvent(keySym, down);
   }

   void SasUserInput::getCurrentUserInfo(::string &desktopName, ::string &userName)
   {
      m_client->getCurrentUserInfo(desktopName, userName);
   }

   void SasUserInput::getPrimaryDisplayCoords(::int_rectangle *rect) { m_client->getPrimaryDisplayCoords(rect); }

   ::int_rectangle_array_base SasUserInput::getDisplaysCoords() { return m_client->getDisplaysCoords(); }

   void SasUserInput::getDisplayNumberCoords(::int_rectangle *rect, unsigned char dispNumber)
   {
      m_client->getDisplayNumberCoords(rect, dispNumber);
   }

   void SasUserInput::getNormalizedRect(::int_rectangle *rect) { m_client->getNormalizedRect(rect); }

   void SasUserInput::getWindowCoords(const ::operating_system::window & operatingsystemwindow, ::int_rectangle *rect) { m_client->getWindowCoords(operatingsystemwindow, rect); }

   ::operating_system::window SasUserInput::getWindowHandleByName(const ::scoped_string &windowName)
   {
      return m_client->getWindowHandleByName(windowName);
   }

   void SasUserInput::getApplicationRegion(unsigned int procId, Region *region)
   {
      m_client->getApplicationRegion(procId, region);
   }

   bool SasUserInput::isApplicationInFocus(unsigned int procId) { return m_client->isApplicationInFocus(procId); }


} // namespace remoting
