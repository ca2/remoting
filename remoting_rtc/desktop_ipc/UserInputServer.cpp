// Copyright (C) 2009,2010,2011,2012 GlavSoft LLC.
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
#include "UserInputServer.h"
//#include "subsystem/thread/lockable_critical_section.h"
#include "subsystem/platform/BrokenHandleException.h"

namespace remoting
{


   UserInputServer::UserInputServer(::remoting_rtc_node::Configurator * pconfigurator, BlockingGate *pblockinggate, DesktopSrvDispatcher * pdispatcher,
                                    //AnEventListener *extTerminationListener,
                                    const ::procedure &procedureTermination,
                                    ::subsystem::LogWriter * plogwriter) :
       m_procedureTermination(procedureTermination), m_plogwriter(plogwriter)
   {
      initialize_desktop_server_proto(pconfigurator, pblockinggate);
      bool ctrlAltDelEnabled = true;

      auto puserinputNew =  createø< UserInput>();
      puserinputNew->initialize_user_input(this, ctrlAltDelEnabled, m_plogwriter);
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
      critical_section_lock al(m_pcontrolgate);
      try
      {
         // Send clipboard data
         if (newClipboard.has_character())
         {
            m_pcontrolgate->writeUInt8(CLIPBOARD_CHANGED);
            sendNewClipboard(newClipboard, m_pcontrolgate);
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

   void UserInputServer::onRequest(::u8 reqCode, BlockingGate *pblockinggate)
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
            errMess.format("Unknown {} protocol code received"
                            " from a UserInputClient",
                            reqCode);
            throw ::subsystem::Exception(errMess);
            break;
      }
   }

   void UserInputServer::serverInit(BlockingGate *pblockinggate)
   {
      ::u8 keyFlags = pblockinggate->readUInt8();
      m_puserinput->initKeyFlag(keyFlags);
   }

   void UserInputServer::applyNewPointerPos(BlockingGate *pblockinggate)
   {
      ::i32_point newPointerPos;
      ::u8 keyFlags;
      readNewPointerPos(&newPointerPos, &keyFlags, pblockinggate);
      m_puserinput->setMouseEvent(newPointerPos, keyFlags);
   }

   void UserInputServer::applyNewClipboard(BlockingGate *pblockinggate)
   {
      ::string newClipboard;
      readNewClipboard(newClipboard, pblockinggate);
      m_puserinput->setNewClipboard(newClipboard);
   }

   void UserInputServer::applyKeyEvent(BlockingGate *pblockinggate)
   {
      ::u32 keySym;
      bool down;
      readKeyEvent(&keySym, &down, pblockinggate);
      m_puserinput->setKeyboardEvent(keySym, down);
   }

   void UserInputServer::ansUserInfo(BlockingGate *pblockinggate)
   {
      ::string desktopName, userName;

      m_puserinput->getCurrentUserInfo(desktopName, userName);
      sendUserInfo(desktopName, userName, pblockinggate);
   }

   void UserInputServer::ansDesktopCoords(BlockingGate *pblockinggate)
   {
      ::i32_rectangle rectangle;
      m_puserinput->getPrimaryDisplayCoords(rectangle);
      sendRect(rectangle, pblockinggate);
   }

   void UserInputServer::ansWindowCoords(BlockingGate *pblockinggate)
   {
      ::i32_rectangle rectangle;
      ::operating_system::window operatingsystemwindow = ::as_operating_system_window((::uptr)pblockinggate->readUInt64());
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
      pblockinggate->writeUInt64(::as_u64(operatingsystemwindow));
   }

   void UserInputServer::ansDisplayNumberCoords(BlockingGate *pblockinggate)
   {
      ::u8 dispNumber = pblockinggate->readUInt8();
      ::i32_rectangle rectangle;
      m_puserinput->getDisplayNumberCoords(rectangle, dispNumber);
      sendRect(rectangle, pblockinggate);
   }

   void UserInputServer::ansDisplaysCoords(BlockingGate *pblockinggate)
   {
      ::i32_rectangle_array_base rectanglea = m_puserinput->getDisplaysCoords();
      size_t number = rectanglea.size();
      if (number > 255)
      {
         number = 255;
      }
      pblockinggate->writeUInt8((::u8)number);
      for (size_t i = 0; i < number; i++)
      {
         ::i32_rectangle rectangle = rectanglea[i];
         sendRect(rectangle, pblockinggate);
      }
   }

   void UserInputServer::ansNormalizeRect(BlockingGate *pblockinggate)
   {
      ::i32_rectangle rectangle = readRect(pblockinggate);
      m_puserinput->getNormalizedRect(rectangle);
      sendRect(rectangle, pblockinggate);
   }

   void UserInputServer::ansApplicationRegion(BlockingGate *pblockinggate)
   {
      ::u32 procId = pblockinggate->readUInt32();
      Region region;
      m_puserinput->getApplicationRegion(procId, region);
      sendRegion(region, pblockinggate);
   }

   void UserInputServer::ansApplicationInFocus(BlockingGate *pblockinggate)
   {
      ::u32 procId = pblockinggate->readUInt32();
      bool result = m_puserinput->isApplicationInFocus((::u32)procId);
      pblockinggate->writeUInt8(result ? 1 : 0);
   }


} // namespace remoting
