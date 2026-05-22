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
#include "framework.h"
#include "UpdateHandlerImpl.h"
#include "acme/parallelization/happening.h"
#include "subsystem/platform/Exception.h"


namespace remoting_rfb
{


   UpdateHandlerImpl::UpdateHandlerImpl(::remoting_rfb_node::Configurator * pconfigurator, UpdateListener *pupdatelistenerExternal, ScreenDriverFactory *pscreendriverfactory,
                                        ::subsystem::LogWriter * plogwriter) :
       m_pupdatelistenerExternal(pupdatelistenerExternal), m_fullUpdateRequested(false), m_plogwriter(plogwriter)
   {
      initialize(pupdatelistenerExternal);
      construct_newø(m_pupdatekeeperProperty);
      m_pscreendriver =
         pscreendriverfactory->createScreenDriver(pconfigurator, m_pupdatekeeperProperty, this, m_pframebufferBackup, &m_criticalsectionFramebuffer, plogwriter);
      // At this point the screen driver must contain valid screen properties.
      construct_newø(m_pframebufferBackup);
      m_pframebufferBackup->assignProperties(m_pscreendriver->getScreenBuffer());
      m_pupdatekeeperProperty->setBorderRect(m_pscreendriver->getScreenDimension());
      construct_newø(m_pupdatefilter);
      m_pupdatefilter->initialize_update_filter(m_pscreendriver, m_pframebufferBackup, &m_criticalsectionFramebuffer, plogwriter);

      // At this point all common resources will be covered the mutex for changes.
      m_pscreendriver->executeDetection();

      // Force first update with full screen grab
      m_rectangleAbsolute = m_pframebufferBackup->getDimension();
      m_pupdatekeeperProperty->addChangedRect(m_rectangleAbsolute);
      doUpdate();
   }

   UpdateHandlerImpl::~UpdateHandlerImpl()
   {
      m_pscreendriver->terminateDetection();
      //delete m_pupdatefilter;
      //delete m_pscreendriver;
   }

   UpdateContainer UpdateHandlerImpl::extract()
   {
      ::i32_rectangle rectangleCopy;
      ::i32_point m_pointCopySource;
      m_plogwriter->debug("UpdateHandlerImpl: getCopiedRegion");
      m_pscreendriver->getCopiedRegion(rectangleCopy, m_pointCopySource);
      //{
         AutoLock al1(m_pupdatekeeperProperty); // The following operations should be atomic
         m_pupdatekeeperProperty->addCopyRect(rectangleCopy, m_pointCopySource);
         m_plogwriter->debug("UpdateHandlerImpl: extract Copy ::i32_rectangle");
         auto updatecontainer = m_pupdatekeeperProperty->extract();
      al1.unlock();
      //}

      // Note: The getVideoRegion() function is not a thread safe function, but it invokes
      // only from this one place and so that is why it does not cover by the mutex.
      m_plogwriter->debug("UpdateHandlerImpl: getVideoRegion");
      updatecontainer.m_regionVideo = m_pscreendriver->getVideoRegion();
      // Constrain the video region to the current frame buffer border.
      m_plogwriter->debug("UpdateHandlerImpl: getRect");
      Region regionFramebuffer(getFramebufferDimension());
      m_plogwriter->debug("UpdateHandlerImpl: intersect");
      updatecontainer.m_regionVideo.intersect(regionFramebuffer);

      m_plogwriter->debug("UpdateHandlerImpl::extract : filter updates");
      m_pupdatefilter->filter(updatecontainer);

      if (!m_rectangleAbsolute.is_empty())
      {
         updatecontainer.m_regionChanged.addRect(m_pscreendriver->getScreenBuffer()->getDimension());
         m_rectangleAbsolute.clear();
      }

      // Checking for screen properties changing or frame buffers differ
      m_plogwriter->debug(
         "UpdateHandlerImpl::extract : Checking for screen properties changing or frame buffers differ");
      if (m_pscreendriver->getScreenPropertiesChanged() ||
          !m_pframebufferBackup->isEqualTo(m_pscreendriver->getScreenBuffer()))
      {
         ::i32_size sizeCurrent = m_pframebufferBackup->getDimension();
         ::i32_size sizeNew = m_pscreendriver->getScreenDimension();
         if (m_pscreendriver->getScreenSizeChanged() || sizeCurrent != sizeNew)
         {
            updatecontainer.m_bScreenSizeChanged = true;
         }
         m_plogwriter->debug("UpdateHandlerImpl::extract: old dims: ({},{}), new dims: ({},{})", sizeCurrent.cx,
                             sizeCurrent.cy, sizeNew.cx, sizeNew.cy);
         m_plogwriter->debug("UpdateHandlerImpl::extract : applyNewScreenProperties()");
         applyNewScreenProperties();
         {
            // Only this place the class CLASS_DECL_REMOTING_RFB provides frame buffer changings, and then why it
            // must be under the mutex. Getters for the backup frame buffer in here (at this function)
            // can work without the mutex, but other getters for the frame buffer in other places
            // may be invoked from other threads and then it shall cover by the mutex.
            critical_section_lock al(&m_criticalsectionFramebuffer);
            m_pframebufferBackup->clone(m_pscreendriver->getScreenBuffer());
         }
         updatecontainer.m_regionChanged.clear();
         updatecontainer.m_regionCopied.clear();
         m_rectangleAbsolute = m_pframebufferBackup->getDimension();
         m_pupdatekeeperProperty->setBorderRect(m_rectangleAbsolute);
         return ::transfer(updatecontainer);
      }

      // Cursor pointPosition must always be present.
      updatecontainer.m_pointCursorPos = m_pscreendriver->getCursorPosition();
      {
         int x = updatecontainer.m_pointCursorPos.x;
         int y = updatecontainer.m_pointCursorPos.y;
         m_plogwriter->debug("UpdateHandlerImpl::extract : update cursor pointPosition ({},{})", x, y);
      }
      // Checking for mouse shape changing
      if (updatecontainer.m_bCursorShapeChanged || m_fullUpdateRequested)
      {
         // Update cursor shape
         m_pscreendriver->grabCursorShape(m_pframebufferBackup->getPixelFormat());
         // Store cursor shape
         m_cursorshape.clone(m_pscreendriver->getCursorShape());

         m_fullUpdateRequested = false;
      }
      m_plogwriter->debug("UpdateHandlerImpl::extract finished");
      return updatecontainer;
   }

   void UpdateHandlerImpl::applyNewScreenProperties()
   {
      int applyTryCount = 3;
      bool applyResult = true;
      do
      {
         if (!applyResult)
         {
            ::happening waitEvent; // No way to made it abortable.
            waitEvent.wait(1000 * 1_ms);
         }
         m_plogwriter->information("Screen properties changed, applying new screen properties, total tries = {}",
                                   applyTryCount);
         applyResult = m_pscreendriver->applyNewScreenProperties();
         applyTryCount--;
      }
      while (applyTryCount > 0 && !applyResult);
      if (!applyResult)
      {
         throw ::subsystem::Exception("Unable set new screen properties at all tries");
      }
   }

   void UpdateHandlerImpl::setFullUpdateRequested(const Region & region)
   {
      m_pupdatekeeperProperty->addChangedRegion(region);
      m_fullUpdateRequested = true;
   }

   void UpdateHandlerImpl::executeDetectors()
   {
      // FIXME: Why following string is here?
      m_pframebufferBackup->assignProperties(m_pscreendriver->getScreenBuffer());

      m_pscreendriver->executeDetection();
   }

   void UpdateHandlerImpl::terminateDetectors() { m_pscreendriver->terminateDetection(); }

   void UpdateHandlerImpl::onUpdate()
   {
      //UpdateContainer updatecontainer;
      auto updatecontainer = m_pupdatekeeperProperty->getUpdateContainer();
      if (!updatecontainer.is_empty())
      {
         doUpdate();
      }
   }

   bool UpdateHandlerImpl::checkForUpdates(Region & region) { return m_pupdatekeeperProperty->checkForUpdates(region); }

   void UpdateHandlerImpl::setExcludedRegion(const Region & regionExcluded)
   {
      m_pupdatekeeperProperty->setExcludedRegion(regionExcluded);
   }


} // namespace remoting_rfb
