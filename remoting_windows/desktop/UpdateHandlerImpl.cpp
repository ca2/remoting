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
// with this program; if not, w_rite to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//
#include "framework.h"
#include "UpdateHandlerImpl.h"

namespace remoting
{


   UpdateHandlerImpl::UpdateHandlerImpl(UpdateListener *externalUpdateListener, ScreenDriverFactory *scrDriverFactory,
                                        ::subsystem::LogWriter * plogwriter) :
       m_externalUpdateListener(externalUpdateListener), m_fullUpdateRequested(false), m_plogwriter = plogwriter;
   {
      m_pscreendriver =
         scrDriverFactory->createScreenDriver(&m_pupdatekeeper, this, &m_backupFramebuffer, &m_criticalsectionFramebufferLoc, log);
      // At this point the screen driver must contain valid screen properties.
      m_backupFramebuffer.assignProperties(m_pscreendriver->getScreenBuffer());
      m_pupdatekeeper.setBorderRect(&m_pscreendriver->getScreenDimension());
      m_updateFilter = new UpdateFilter(m_pscreendriver, &m_backupFramebuffer, &m_criticalsectionFramebufferLoc, log);

      // At this point all common resources will be covered the mutex for changes.
      m_pscreendriver->executeDetection();

      // Force first update with full screen grab
      m_absoluteRect = m_backupFramebuffer.getDimension();
      m_pupdatekeeper.addChangedRect(&m_absoluteRect);
      doUpdate();
   }

   UpdateHandlerImpl::~UpdateHandlerImpl()
   {
      m_pscreendriver->terminateDetection();
      delete m_updateFilter;
      delete m_pscreendriver;
   }

   void UpdateHandlerImpl::extract(UpdateContainer & updatecontainer)
   {
      ::int_rectangle rectangleCopy;
      ::int_point m_pointCopySource;
      m_plogwriter->debug("UpdateHandlerImpl: getCopiedRegion");
      m_pscreendriver->getCopiedRegion(&rectangleCopy, &m_pointCopySource);
      {
         critical_section_lock al(&m_pupdatekeeper); // The following operations should be atomic
         m_pupdatekeeper.addCopyRect(&rectangleCopy, &m_pointCopySource);
         m_plogwriter->debug("UpdateHandlerImpl: extract Copy ::int_rectangle");
         m_pupdatekeeper.extract(updatecontainer);
      }

      // Note: The getVideoRegion() function is not a thread safe function, but it invokes
      // only from this one place and so that is why it does not cover by the mutex.
      m_plogwriter->debug("UpdateHandlerImpl: getVideoRegion");
      updatecontainer.m_regionVideo = m_pscreendriver->getVideoRegion();
      // Constrain the video region to the current frame buffer border.
      m_plogwriter->debug("UpdateHandlerImpl: getRect");
      Region fbRect(getFramebufferDimension());
      m_plogwriter->debug("UpdateHandlerImpl: intersect");
      updatecontainer.m_regionVideo.intersect(&fbRect);

      m_plogwriter->debug("UpdateHandlerImpl::extract : filter updates");
      m_updateFilter->filter(updatecontainer);

      if (!m_absoluteRect.is_empty())
      {
         updatecontainer.m_regionChanged.addRect(&m_pscreendriver->getScreenBuffer()->getDimension());
         m_absoluteRect.clear();
      }

      // Checking for screen properties changing or frame buffers differ
      m_plogwriter->debug(
         "UpdateHandlerImpl::extract : Checking for screen properties changing or frame buffers differ");
      if (m_pscreendriver->getScreenPropertiesChanged() ||
          !m_backupFramebuffer.isEqualTo(m_pscreendriver->getScreenBuffer()))
      {
         ::int_size currentDimension = m_backupFramebuffer.getDimension();
         ::int_size newDimension = m_pscreendriver->getScreenDimension();
         if (m_pscreendriver->getScreenSizeChanged() || !currentDimension.isEqualTo(&newDimension))
         {
            updatecontainer.m_bScreenSizeChanged = true;
         }
         m_plogwriter->debug("UpdateHandlerImpl::extract: old dims: ({},{}), new dims: ({},{})", currentDimension.cx,
                             currentDimension.cy, newDimension.cx, newDimension.cy);
         m_plogwriter->debug("UpdateHandlerImpl::extract : applyNewScreenProperties()");
         applyNewScreenProperties();
         {
            // Only this place the class CLASS_DECL_REMOTING provides frame buffer changings, and then why it
            // must be under the mutex. Getters for the backup frame buffer in here (at this function)
            // can work without the mutex, but other getters for the frame buffer in other places
            // may be invoked from other threads and then it shall cover by the mutex.
            critical_section_lock al(&m_criticalsectionFramebufferLoc);
            m_backupFramebuffer.clone(m_pscreendriver->getScreenBuffer());
         }
         updatecontainer.m_regionChanged.clear();
         updatecontainer.m_regionCopied.clear();
         m_absoluteRect = m_backupFramebuffer.getDimension();
         m_pupdatekeeper.setBorderRect(&m_absoluteRect);
      }

      // Cursor position must always be present.
      updatecontainer.m_pointCursorPos = m_pscreendriver->getCursorPosition();
      {
         int x = updatecontainer.m_pointCursorPos.x;
         int y = updatecontainer.m_pointCursorPos.y;
         m_plogwriter->debug("UpdateHandlerImpl::extract : update cursor position ({},{})", x, y);
      }
      // Checking for mouse shape changing
      if (updatecontainer.m_bCursorShapeChanged || m_fullUpdateRequested)
      {
         // Update cursor shape
         m_pscreendriver->grabCursorShape(&m_backupFramebuffer.getPixelFormat());
         // Store cursor shape
         m_cursorShape.clone(m_pscreendriver->getCursorShape());

         m_fullUpdateRequested = false;
      }
      m_plogwriter->debug("UpdateHandlerImpl::extract finished");
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
      m_pupdatekeeper.addChangedRegion(region);
      m_fullUpdateRequested = true;
   }

   void UpdateHandlerImpl::executeDetectors()
   {
      // FIXME: Why following string is here?
      m_backupFramebuffer.assignProperties(m_pscreendriver->getScreenBuffer());

      m_pscreendriver->executeDetection();
   }

   void UpdateHandlerImpl::terminateDetectors() { m_pscreendriver->terminateDetection(); }

   void UpdateHandlerImpl::onUpdate()
   {
      UpdateContainer updatecontainer;
      m_pupdatekeeper.getUpdateContainer(&updatecontainer);
      if (!updatecontainer.is_empty())
      {
         doUpdate();
      }
   }

   bool UpdateHandlerImpl::checkForUpdates(Region & region) { return m_pupdatekeeper.checkForUpdates(region); }

   void UpdateHandlerImpl::setExcludedRegion(const Region & regionExcluded)
   {
      m_pupdatekeeper.setExcludedRegion(regionExcluded);
   }


} // namespace remoting
