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
#include "framework.h"
#include "UserInputServer.h"
//#include "subsystem/thread/critical_section.h"
#include "subsystem/platform/BrokenHandleException.h"

namespace remoting
{


   UserInputServer::UserInputServer(BlockingGate *pblockinggate, DesktopSrvDispatcher * pdispatcher,
                                    AnEventListener *extTerminationListener, ::subsystem::LogWriter * plogwriter) :
       DesktopServerProto(pblockinggate), m_extTerminationListener(extTerminationListener), m_plogwriter = plogwriter;
   {
      bool ctrlAltDelEnabled = true;
      m_userInput = new WindowsUserInput(this, ctrlAltDelEnabled, m_plogwriter);

      dispatcher->registerNewHandle(POINTER_POS_CHANGED, this);
      dispatcher->registerNewHandle(CLIPBOARD_CHANGED, this);
      dispatcher->registerNewHandle(KEYBOARD_EVENT, this);
      dispatcher->registerNewHandle(USER_INFO_REQ, this);
      dispatcher->registerNewHandle(DESKTOP_COORDS_REQ, this);
      dispatcher->registerNewHandle(WINDOW_COORDS_REQ, this);
      dispatcher->registerNewHandle(WINDOW_HANDLE_REQ, this);
      dispatcher->registerNewHandle(DISPLAY_NUMBER_COORDS_REQ, this);
      dispatcher->registerNewHandle(DISPLAYS_COORDS_REQ, this);
      dispatcher->registerNewHandle(APPLICATION_REGION_REQ, this);
      dispatcher->registerNewHandle(APPLICATION_CHECK_FOCUS, this);
      dispatcher->registerNewHandle(NORMALIZE_RECT_REQ, this);
      dispatcher->registerNewHandle(USER_INPUT_INIT, this);
   }

   UserInputServer::~UserInputServer()
   {
      m_plogwriter->debug("The UserInputServer destructor has been called");
      delete m_userInput;
   }

   void UserInputServer::onClipboardUpdate(const ::scoped_string &newClipboard)
   {
      critical_section_lock al(m_forwGate);
      try
      {
         // Send clipboard data
         if (newClipboard->length() != 0)
         {
            m_forwGate->writeUInt8(CLIPBOARD_CHANGED);
            sendNewClipboard(newClipboard, m_forwGate);
         }
      }
      catch (::exception &e)
      {
         m_plogwriter->error("An error has been occurred while sending a"
                             " CLIPBOARD_CHANGED scopedstrMessage from UserInputServer: {}",
                             e.get_message());
         m_extTerminationListener->onAnObjectEvent();
      }
   }

   void UserInputServer::onRequest(unsigned char reqCode, BlockingGate *pblockinggate)
   {
      switch (reqCode)
      {
         case POINTER_POS_CHANGED:
            applyNewPointerPos(pblockinggate);
            break;
         case CLIPBOARD_CHANGED:
            applyNewClipboard(pblockinggate);
            break;
         case KEYBOARD_EVENT:
            applyKeyEvent(pblockinggate);
            break;
         case USER_INFO_REQ:
            ansUserInfo(pblockinggate);
            break;
         case DESKTOP_COORDS_REQ:
            ansDesktopCoords(pblockinggate);
            break;
         case WINDOW_COORDS_REQ:
            ansWindowCoords(pblockinggate);
            break;
         case WINDOW_HANDLE_REQ:
            ansWindowHandle(pblockinggate);
            break;
         case DISPLAY_NUMBER_COORDS_REQ:
            ansDisplayNumberCoords(pblockinggate);
            break;
         case DISPLAYS_COORDS_REQ:
            ansDisplaysCoords(pblockinggate);
            break;
         case APPLICATION_REGION_REQ:
            ansApplicationRegion(pblockinggate);
            break;
         case APPLICATION_CHECK_FOCUS:
            ansApplicationInFocus(pblockinggate);
            break;
         case NORMALIZE_RECT_REQ:
            ansNormalizeRect(pblockinggate);
            break;
         case USER_INPUT_INIT:
            serverInit(pblockinggate);
            break;
         default:
            ::string errMess;
            errMess.formatf("Unknown {} protocol code received"
                            " from a UserInputClient",
                            reqCode);
            throw ::subsystem::Exception(errMess);
            break;
      }
   }

   void UserInputServer::serverInit(BlockingGate *pblockinggate)
   {
      unsigned char keyFlags = pblockinggate->readUInt8();
      m_userInput->initKeyFlag(keyFlags);
   }

   void UserInputServer::applyNewPointerPos(BlockingGate *pblockinggate)
   {
      ::int_point newPointerPos;
      unsigned char keyFlags;
      readNewPointerPos(&newPointerPos, &keyFlags, pblockinggate);
      m_userInput->setMouseEvent(newPointerPos, keyFlags);
   }

   void UserInputServer::applyNewClipboard(BlockingGate *pblockinggate)
   {
      ::string newClipboard;
      readNewClipboard(&newClipboard, pblockinggate);
      m_userInput->setNewClipboard(&newClipboard);
   }

   void UserInputServer::applyKeyEvent(BlockingGate *pblockinggate)
   {
      unsigned int keySym;
      bool down;
      readKeyEvent(&keySym, &down, pblockinggate);
      m_userInput->setKeyboardEvent(keySym, down);
   }

   void UserInputServer::ansUserInfo(BlockingGate *pblockinggate)
   {
      ::string desktopName, userName;

      m_userInput->getCurrentUserInfo(&desktopName, &userName);
      sendUserInfo(&desktopName, &userName, pblockinggate);
   }

   void UserInputServer::ansDesktopCoords(BlockingGate *pblockinggate)
   {
      ::int_rectangle rect;
      m_userInput->getPrimaryDisplayCoords(&rect);
      sendRect(rect, pblockinggate);
   }

   void UserInputServer::ansWindowCoords(BlockingGate *pblockinggate)
   {
      ::int_rectangle rect;
      ::operating_system::window operatingsystemwindow = ::as_operating_system_window((HWND)pblockinggate->readUInt64());
      try
      {
         m_userInput->getWindowCoords(operatingsystemwindow, &rect);
         // handle is correct
         pblockinggate->writeUInt8(0);
         sendRect(rect, pblockinggate);
      }
      catch (::subsystem::BrokenHandleException &e)
      {
         pblockinggate->writeUInt8(1);
         pblockinggate->writeUTF8(e.get_message());
      }
   }

   void UserInputServer::ansWindowHandle(BlockingGate *pblockinggate)
   {
      ::string windowName;
      windowName = pblockinggate->readUtf8();
      auto operatingsystemwindow = m_userInput->getWindowHandleByName(windowName);
      pblockinggate->writeUInt64((unsigned long long)(HWND)::as_HWND(operatingsystemwindow));
   }

   void UserInputServer::ansDisplayNumberCoords(BlockingGate *pblockinggate)
   {
      unsigned char dispNumber = pblockinggate->readUInt8();
      ::int_rectangle rect;
      m_userInput->getDisplayNumberCoords(&rect, dispNumber);
      sendRect(rect, pblockinggate);
   }

   void UserInputServer::ansDisplaysCoords(BlockingGate *pblockinggate)
   {
      ::int_rectangle_array_base rects = m_userInput->getDisplaysCoords();
      size_t number = rects.size();
      if (number > 255)
      {
         number = 255;
      }
      pblockinggate->writeUInt8((unsigned char)number);
      for (size_t i = 0; i < number; i++)
      {
         ::int_rectangle rect = rects[i];
         sendRect(&rect, pblockinggate);
      }
   }

   void UserInputServer::ansNormalizeRect(BlockingGate *pblockinggate)
   {
      ::int_rectangle rect = readRect(pblockinggate);
      m_userInput->getNormalizedRect(&rect);
      sendRect(&rect, pblockinggate);
   }

   void UserInputServer::ansApplicationRegion(BlockingGate *pblockinggate)
   {
      unsigned int procId = pblockinggate->readUInt32();
      Region region;
      m_userInput->getApplicationRegion(procId, &region);
      sendRegion(&region, pblockinggate);
   }

   void UserInputServer::ansApplicationInFocus(BlockingGate *pblockinggate)
   {
      unsigned int procId = pblockinggate->readUInt32();
      bool result = m_userInput->isApplicationInFocus((unsigned int)procId);
      pblockinggate->writeUInt8(result ? 1 : 0);
   }


} // namespace remoting
