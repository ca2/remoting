// Copyright (C) 2011,2012 GlavSoft LLC.
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
#include "DesktopBaseImpl.h"
#include "subsystem/platform/BrokenHandleException.h"


namespace remoting_windows

{

   // DesktopBaseImpl::DesktopBaseImpl(ClipboardListener *pclipboardlistenerExternal, UpdateSendingListener *pupdatesendinglistenerExternal,
   //                                  AbnormDeskTermListener *pdesktermlistenerExternal, ::subsystem::LogWriter * plogwriter) :
   //     m_pupdatesendinglistenerExternal(pupdatesendinglistenerExternal), m_pdesktermlistenerExternal(pdesktermlistenerExternal),
   //     m_pclipboardlistenerExternal(pclipboardlistenerExternal), m_puserinput(0), m_pupdatehandler(0), m_plogwriter = plogwriter;
   // {
   // }
   DesktopBaseImpl::DesktopBaseImpl() :
       m_pupdatesendinglistenerExternal(nullptr), m_pdesktermlistenerExternal(nullptr),
       m_pclipboardlistenerExternal(nullptr), m_puserinput(0), m_pupdatehandler(0), m_plogwriter(nullptr)
   {
   }

   DesktopBaseImpl::~DesktopBaseImpl() {}

   void DesktopBaseImpl::initialize_desktop_base_impl(::subsystem::ClipboardListener *pclipboardlistenerExternal, UpdateSendingListener *pupdatesendinglistenerExternal,
                AbnormDeskTermListener *pdesktermlistenerExternal, ::subsystem::LogWriter * plogwriter)
   {
      m_pupdatesendinglistenerExternal = pupdatesendinglistenerExternal;
      m_pdesktermlistenerExternal = pdesktermlistenerExternal;
      m_pclipboardlistenerExternal = pclipboardlistenerExternal;
      m_plogwriter = plogwriter;

   }


   void DesktopBaseImpl::getCurrentUserInfo(::string &desktopName, ::string &userName)
   {
      ASSERT(m_puserinput != 0);
      ASSERT(m_pdesktermlistenerExternal != 0);
      m_plogwriter->information("get current user information");
      try
      {
         m_puserinput->getCurrentUserInfo(desktopName, userName);
      }
      catch (::exception &e)
      {
         m_plogwriter->error("::subsystem::Exception in DesktopBaseImpl::getCurrentUserInfo: {}", e.get_message());
         m_pdesktermlistenerExternal->onAbnormalDesktopTerminate();
      }
   }

   void DesktopBaseImpl::getFramebufferProperties(::i32_size & size, ::innate_subsystem::PixelFormat & pixelformat)
   {
      ASSERT(m_pupdatehandler != 0);
      ASSERT(m_pdesktermlistenerExternal != 0);
      m_plogwriter->debug("get frame buffer properties");
      try
      {
         m_pupdatehandler->getFramebufferProp(size, pixelformat);
      }
      catch (::exception &e)
      {
         m_plogwriter->error("::subsystem::Exception in DesktopBaseImpl::getFramebufferProperties: {}",
                             e.get_message());
         m_pdesktermlistenerExternal->onAbnormalDesktopTerminate();
      }
   }

   void DesktopBaseImpl::getPrimaryDesktopCoords(::i32_rectangle rectangle)
   {
      ASSERT(m_puserinput != 0);
      ASSERT(m_pdesktermlistenerExternal != 0);
      m_plogwriter->information("get primary desktop coordinates");
      try
      {
         m_puserinput->getPrimaryDisplayCoords(rectangle);
      }
      catch (::exception &e)
      {
         m_plogwriter->error("::subsystem::Exception in DesktopBaseImpl::getPrimaryDesktopCoords: {}", e.get_message());
         m_pdesktermlistenerExternal->onAbnormalDesktopTerminate();
      }
   }

   void DesktopBaseImpl::getDisplayNumberCoords(::i32_rectangle rectangle, unsigned char dispNumber)
   {
      ASSERT(m_puserinput != 0);
      ASSERT(m_pdesktermlistenerExternal != 0);
      m_plogwriter->information("get the %u display coordinates", (unsigned int)dispNumber);
      try
      {
         m_puserinput->getDisplayNumberCoords(rectangle, dispNumber);
      }
      catch (::exception &e)
      {
         m_plogwriter->error("::subsystem::Exception in DesktopBaseImpl::getDisplayNumberCoords: {}", e.get_message());
         m_pdesktermlistenerExternal->onAbnormalDesktopTerminate();
      }
   }

   ::int_rectangle_array_base DesktopBaseImpl::getDisplaysCoords()
   {
      ASSERT(m_puserinput != 0);
      ASSERT(m_pdesktermlistenerExternal != 0);
      m_plogwriter->information("get the displays coordinates");
      try
      {
         return m_puserinput->getDisplaysCoords();
      }
      catch (::exception &e)
      {
         m_plogwriter->error("::subsystem::Exception in DesktopBaseImpl::getDisplayCoords: {}", e.get_message());
         m_pdesktermlistenerExternal->onAbnormalDesktopTerminate();
      }
      return ::int_rectangle_array_base();
   }


   void DesktopBaseImpl::getNormalizedRect(::i32_rectangle rectangle)
   {
      ASSERT(m_puserinput != 0);
      ASSERT(m_pdesktermlistenerExternal != 0);
      m_plogwriter->information("normilize a rectangle to frame buffer coordinates");
      try
      {
         m_puserinput->getNormalizedRect(rectangle);
      }
      catch (::exception &e)
      {
         m_plogwriter->error("::subsystem::Exception in DesktopBaseImpl::getNormalizedRect: {}", e.get_message());
         m_pdesktermlistenerExternal->onAbnormalDesktopTerminate();
      }
   }

   void DesktopBaseImpl::getWindowCoords(const ::operating_system::window & operatingsystemwindow, ::i32_rectangle rectangle)
   {
      ASSERT(m_puserinput != 0);
      ASSERT(m_pdesktermlistenerExternal != 0);
      m_plogwriter->information("get window coordinates");
      try
      {
         m_puserinput->getWindowCoords(operatingsystemwindow, rectangle);
      }
      catch (::subsystem::BrokenHandleException &)
      {
         throw;
      }
      catch (::exception &e)
      {
         m_plogwriter->error("::subsystem::Exception in DesktopBaseImpl::getWindowCoords: {}", e.get_message());
         m_pdesktermlistenerExternal->onAbnormalDesktopTerminate();
      }
   }

   ::operating_system::window DesktopBaseImpl::getWindowHandleByName(const ::scoped_string &windowName)
   {
      ASSERT(m_puserinput != 0);
      ASSERT(m_pdesktermlistenerExternal != 0);
      m_plogwriter->information("get a window handle by a window name");
      try
      {
         return m_puserinput->getWindowHandleByName(windowName);
      }
      catch (::exception &e)
      {
         m_plogwriter->error("::subsystem::Exception in DesktopBaseImpl::getWindowHandleByName: {}", e.get_message());
         m_pdesktermlistenerExternal->onAbnormalDesktopTerminate();
      }
      return 0;
   }

   void DesktopBaseImpl::getApplicationRegion(unsigned int procId, Region & region)
   {
      ASSERT(m_puserinput != 0);
      ASSERT(m_pdesktermlistenerExternal != 0);
      m_plogwriter->information("get application region");
      try
      {
         m_puserinput->getApplicationRegion(procId, region);
      }
      catch (::exception &e)
      {
         m_plogwriter->error("::subsystem::Exception in DesktopBaseImpl::getApplicationRegion: {}", e.get_message());
         m_pdesktermlistenerExternal->onAbnormalDesktopTerminate();
      }
   }

   bool DesktopBaseImpl::isApplicationInFocus(unsigned int procId)
   {
      ASSERT(m_puserinput != 0);
      ASSERT(m_pdesktermlistenerExternal != 0);
      m_plogwriter->information("checking if application is in focus");
      try
      {
         return m_puserinput->isApplicationInFocus(procId);
      }
      catch (::exception &e)
      {
         m_plogwriter->error("::subsystem::Exception in DesktopBaseImpl::isApplicationInFocus {}", e.get_message());
         m_pdesktermlistenerExternal->onAbnormalDesktopTerminate();
      }
      return false;
   }

   bool DesktopBaseImpl::isRemoteInputAllowed()
   {
      m_plogwriter->debug("checking remote input allowing");

      bool enabled = !m_pconfigurator->getServerConfig()->isBlockingRemoteInput();
      enabled = enabled && !isRemoteInputTempBlocked();
      return enabled;
   }

   void DesktopBaseImpl::setKeyboardEvent(unsigned int keySym, bool down)
   {
      ASSERT(m_puserinput != 0);
      ASSERT(m_pdesktermlistenerExternal != 0);

      m_plogwriter->information("set keyboard event (keySym = %u, down = {})", keySym, (int)down);
      try
      {
         if (isRemoteInputAllowed())
         {
            m_puserinput->setKeyboardEvent(keySym, down);
         }
      }
      catch (::exception &e)
      {
         m_plogwriter->error("setKeyboardEvent() crashed: {}", e.get_message());
         m_pdesktermlistenerExternal->onAbnormalDesktopTerminate();
      }
   }

   void DesktopBaseImpl::setMouseEvent(unsigned short x, unsigned short y, unsigned char buttonMask)
   {
      ASSERT(m_puserinput != 0);
      ASSERT(m_pdesktermlistenerExternal != 0);

      m_plogwriter->information("set mouse event (x = %u, y = %u, mask = %u)", (unsigned int)x, (unsigned int)y,
                                (unsigned int)buttonMask);
      ::i32_point point(x, y);
      try
      {
         if (isRemoteInputAllowed())
         {
            m_puserinput->setMouseEvent(point, buttonMask);
         }
      }
      catch (::exception &e)
      {
         m_plogwriter->error("::subsystem::Exception in DesktopBaseImpl::setMouseEvent {}", e.get_message());
         m_pdesktermlistenerExternal->onAbnormalDesktopTerminate();
      }
   }

   void DesktopBaseImpl::setNewClipText(const ::scoped_string &newClipboard)
   {
      ASSERT(m_puserinput != 0);
      ASSERT(m_pdesktermlistenerExternal != 0);

      m_plogwriter->debug("set new clipboard text, length: {}", newClipboard->length());

      {
         critical_section_lock al(&m_storedClipCritSec);
         m_strReceivedClipboard = *newClipboard;
      }
      try
      {
         m_puserinput->setNewClipboard(newClipboard);
      }
      catch (::exception &e)
      {
         m_plogwriter->error("::subsystem::Exception in DesktopBaseImpl::setNewClipText {}", e.get_message());
         m_pdesktermlistenerExternal->onAbnormalDesktopTerminate();
      }
   }

   void DesktopBaseImpl::sendUpdate()
   {
      ASSERT(m_pupdatehandler != 0);
      ASSERT(m_pdesktermlistenerExternal != 0);
      ASSERT(m_pupdatesendinglistenerExternal != 0);

      if (!m_pupdatesendinglistenerExternal->isReadyToSend())
      {
         m_plogwriter->debug("nobody is ready for updates");
         return;
      }
      UpdateContainer updatecontainer;
      try
      {
         if (!m_regionFullReq.is_empty())
         {
            m_plogwriter->debug("set full update request to UpdateHandler");
            m_pupdatehandler->setFullUpdateRequested(&m_regionFullReq);
         }

         m_plogwriter->debug("extracting updates from UpdateHandler");
         m_pupdatehandler->extract(&updatecontainer);
      }
      catch (::exception &e)
      {
         m_plogwriter->information("WinDesktop::sendUpdate() failed with error:{}", e.get_message());
         m_pdesktermlistenerExternal->onAbnormalDesktopTerminate();
      }

      if (!updatecontainer.is_empty() || !m_regionFullReq.is_empty())
      {
         m_plogwriter->debug("UpdateContainer is not empty."
                             " Updates will be given to all.");
         m_pupdatesendinglistenerExternal->onSendUpdate(&updatecontainer, m_pupdatehandler->getCursorShape());
         critical_section_lock al(&m_criticalsectionReqReg);
         m_regionFullReq.clear();
      }
      else
      {
         m_plogwriter->information("UpdateContainer is empty");
      }
   }

   void DesktopBaseImpl::onUpdate()
   {
      m_plogwriter->debug("update detected");
      m_happeningNewUpdate.set_happening();
   }

   void DesktopBaseImpl::onUpdateRequest(const ::i32_rectangle &rectRequested, bool incremental)
   {
      m_plogwriter->debug("DesktopBaseImpl::onUpdateRequest: update requested");

      critical_section_lock al(&m_criticalsectionReqReg);
      if (!incremental)
      {
         m_regionFullReq.addRect(rectRequested);
      }
      m_happeningNewUpdate.set_happening();
   }

   void DesktopBaseImpl::onClipboardUpdate(const ::scoped_string &newClipboard)
   {
      ASSERT(m_pclipboardlistenerExternal != 0);

      m_plogwriter->debug("clipboard update detected, length: {}", newClipboard->length());
      bool isEqual;
      {
         critical_section_lock al(&m_storedClipCritSec);
         isEqual = m_strReceivedClipboard.isEqualTo(newClipboard);
      }
      if (!isEqual)
      {
         {
            critical_section_lock al(&m_storedClipCritSec);
            m_strReceivedClipboard = "";
         }
         // Send new clipboard text, even if it is empty.
         m_plogwriter->debug("Send new clipboard content");
         m_pclipboardlistenerExternal->onClipboardUpdate(newClipboard);
      }
      else
      {
         m_plogwriter->debug("do not send new clipboard content");
      }
   }

   void DesktopBaseImpl::onConfigReload(::remoting_node::ServerConfig *serverConfig) { applyNewConfiguration(); }

   bool DesktopBaseImpl::updateExternalFramebuffer(::innate_subsystem::Framebuffer *pframebuffer, const Region & region,
                                                   const ::i32_rectangle &rectangleViewport)
   {
      return m_pupdatehandler->updateExternalFramebuffer(pframebuffer, region, rectangleViewport);
   }


} // namespace remoting_windows
 