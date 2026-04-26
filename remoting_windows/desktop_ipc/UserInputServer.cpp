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


   UserInputServer::UserInputServer(Configurator * pconfigurator, BlockingGate *pblockinggate, DesktopSrvDispatcher * pdispatcher,
                                    //AnEventListener *extTerminationListener,
                                    const ::procedure &procedureTermination,
                                    ::subsystem::LogWriter * plogwriter) :
       m_procedureTermination(procedureTermination), m_plogwriter(plogwriter)
   {
      initialize_desktop_server_proto(pconfigurator, pblockinggate);
      bool ctrlAltDelEnabled = true;

      auto puserinputNew =  create_newø< WindowsUserInput>();
      puserinputNew->initialize_windows_user_input(this, ctrlAltDelEnabled, m_plogwriter);
      m_puserinput = puserinputNew;

      pdispatcher->registerNewHandle(POINTER_POS_CHANGED, this);
      pdispatcher->registerNewHandle(CLIPBOARD_CHANGED, this);
      pdispatcher->registerNewHandle(KEYBOARD_EVENT, this);
      pdispatcher->registerNewHandle(USER_INFO_REQ, this);
      pdispatcher->registerNewHandle(DESKTOP_COORDS_REQ, this);
      pdispatcher->registerNewHandle(WINDOW_COORDS_REQ, this);
      pdispatcher->registerNewHandle(WINDOW_HANDLE_REQ, this);
      pdispatcher->registerNewHandle(DISPLAY_NUMBER_COORDS_REQ, this);
      pdispatcher->registerNewHandle(DISPLAYS_COORDS_REQ, this);
      pdispatcher->registerNewHandle(APPLICATION_REGION_REQ, this);
      pdispatcher->registerNewHandle(APPLICATION_CHECK_FOCUS, this);
      pdispatcher->registerNewHandle(NORMALIZE_RECT_REQ, this);
      pdispatcher->registerNewHandle(USER_INPUT_INIT, this);
   }

   UserInputServer::~UserInputServer()
   {
      m_plogwriter->debug("The UserInputServer destructor has been called");
      //delete m_puserinput;
   }

   void UserInputServer::onClipboardUpdate(const ::scoped_string &newClipboard)
   {
      critical_section_lock al(m_pblockinggate);
      try
      {
         // Send clipboard data
         if (newClipboard.has_character())
         {
            m_pblockinggate->writeUInt8(CLIPBOARD_CHANGED);
            sendNewClipboard(newClipboard, m_pblockinggate);
         }
      }
      catch (::exception &e)
      {
         m_plogwriter->error("An error has been occurred while sending a"
                             " CLIPBOARD_CHANGED scopedstrMessage from UserInputServer: {}",
                             e.get_message());
         m_procedureTermination();
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
      m_puserinput->initKeyFlag(keyFlags);
   }

   void UserInputServer::applyNewPointerPos(BlockingGate *pblockinggate)
   {
      ::int_point newPointerPos;
      unsigned char keyFlags;
      readNewPointerPos(&newPointerPos, &keyFlags, pblockinggate);
      m_puserinput->setMouseEvent(newPointerPos, keyFlags);
   }

   void UserInputServer::applyNewClipboard(BlockingGate *pblockinggate)
   {
      ::string newClipboard;
      readNewClipboard(&newClipboard, pblockinggate);
      m_puserinput->setNewClipboard(&newClipboard);
   }

   void UserInputServer::applyKeyEvent(BlockingGate *pblockinggate)
   {
      unsigned int keySym;
      bool down;
      readKeyEvent(&keySym, &down, pblockinggate);
      m_puserinput->setKeyboardEvent(keySym, down);
   }

   void UserInputServer::ansUserInfo(BlockingGate *pblockinggate)
   {
      ::string desktopName, userName;

      m_puserinput->getCurrentUserInfo(&desktopName, &userName);
      sendUserInfo(&desktopName, &userName, pblockinggate);
   }

   void UserInputServer::ansDesktopCoords(BlockingGate *pblockinggate)
   {
      ::int_rectangle rectangle;
      m_puserinput->getPrimaryDisplayCoords(rectangle);
      sendRect(rectangle, pblockinggate);
   }

   void UserInputServer::ansWindowCoords(BlockingGate *pblockinggate)
   {
      ::int_rectangle rectangle;
      ::operating_system::window operatingsystemwindow = ::as_operating_system_window((HWND)pblockinggate->readUInt64());
      try
      {
         m_puserinput->getWindowCoords(operatingsystemwindow, rectangle);
         // handle is correct
         pblockinggate->writeUInt8(0);
         sendRect(rectangle, pblockinggate);
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
      auto operatingsystemwindow = m_puserinput->getWindowHandleByName(windowName);
      pblockinggate->writeUInt64((unsigned long long)(HWND)::as_HWND(operatingsystemwindow));
   }

   void UserInputServer::ansDisplayNumberCoords(BlockingGate *pblockinggate)
   {
      unsigned char dispNumber = pblockinggate->readUInt8();
      ::int_rectangle rectangle;
      m_puserinput->getDisplayNumberCoords(rectangle, dispNumber);
      sendRect(rectangle, pblockinggate);
   }

   void UserInputServer::ansDisplaysCoords(BlockingGate *pblockinggate)
   {
      ::int_rectangle_array_base rectanglea = m_puserinput->getDisplaysCoords();
      size_t number = rectanglea.size();
      if (number > 255)
      {
         number = 255;
      }
      pblockinggate->writeUInt8((unsigned char)number);
      for (size_t i = 0; i < number; i++)
      {
         ::int_rectangle rectangle = rectanglea[i];
         sendRect(rectangle, pblockinggate);
      }
   }

   void UserInputServer::ansNormalizeRect(BlockingGate *pblockinggate)
   {
      ::int_rectangle rectangle = readRect(pblockinggate);
      m_puserinput->getNormalizedRect(rectangle);
      sendRect(rectangle, pblockinggate);
   }

   void UserInputServer::ansApplicationRegion(BlockingGate *pblockinggate)
   {
      unsigned int procId = pblockinggate->readUInt32();
      Region region;
      m_puserinput->getApplicationRegion(procId, &region);
      sendRegion(&region, pblockinggate);
   }

   void UserInputServer::ansApplicationInFocus(BlockingGate *pblockinggate)
   {
      unsigned int procId = pblockinggate->readUInt32();
      bool result = m_puserinput->isApplicationInFocus((unsigned int)procId);
      pblockinggate->writeUInt8(result ? 1 : 0);
   }


} // namespace remoting
