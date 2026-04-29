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
#include "../WinDxRecoverableException.h"
#include "../WinDxCriticalException.h"
//#include "subsystem/thread/lockable_critical_section.h"

#include "../WinDxgiAcquiredFrame.h"
#include "../WinD3D11Texture2D.h"
#include "../WinAutoMapDxgiSurface.h"

#include "../Win8DeskDuplicationThread.h"

namespace remoting
{


   Win8DeskDuplication::Win8DeskDuplication(::innate_subsystem::Framebuffer *targetFb,
                                            ::int_rectangle_array_base &targetRect, Win8CursorShape *targetCurShape,
                                            LONGLONG *cursorTimeStamp, lockable_critical_section *cursorMutex,
                                            Win8DuplicationListener *duplListener,
                                            ::array_base<WinDxgiOutput> &dxgiOutput, ::subsystem::LogWriter * plogwriter) :
       m_targetFb(targetFb), m_targetRects(targetRect), m_targetCurShape(targetCurShape),
       m_cursorTimeStamp(cursorTimeStamp), m_cursorMutex(cursorMutex), m_duplListener(duplListener), m_device(plogwriter),
       m_hasCriticalError(false), m_hasRecoverableError(false), m_plogwriter(plogwriter)
   {
      m_plogwriter->debug("Creating Win8DeskDuplication for {} outputs", dxgiOutput.size());
      for (size_t i = 0; i < dxgiOutput.size(); i++)
      {
         m_dxgiOutput1.add(&dxgiOutput[i]);
         m_outDupl.add(WinDxgiOutputDuplication(&m_dxgiOutput1[i], &m_device));
         m_rotations.add(dxgiOutput[i].getRotation());
         m_stageTextures2D.add(WinCustomD3D11Texture2D(m_device.getDevice(), (unsigned int)targetRect[i].width(),
                                                       (unsigned int)targetRect[i].height(), m_rotations[i]));
      }
      m_plogwriter->debug("Win8DeskDuplication created");
      resume();
   }

   Win8DeskDuplication::~Win8DeskDuplication()
   {
      m_plogwriter->debug("deleting Win8DeskDuplication");
      terminate();
      wait();
   }

   bool Win8DeskDuplication::isValid() { return !m_hasRecoverableError && !m_hasCriticalError; }

   void Win8DeskDuplication::execute()
   {
      const int ACQUIRE_TIMEOUT = 20;
      try
      {
         ::array_base<int> timeouts;
         ::array_base<class ::time> begins;
         timeouts.resize(m_outDupl.size());
         begins.resize(m_outDupl.size());
         while (!isTerminating() && isValid())
         {
            for (size_t i = 0; i < m_outDupl.size(); i++)
            {
               {
                  begins[i] = class ::time::now();
                  WinDxgiAcquiredFrame acquiredFrame(&m_outDupl[i], ACQUIRE_TIMEOUT);
                  if (acquiredFrame.wasTimeOut())
                  {
                     timeouts[i]++;
                     m_plogwriter->debug("Timeout on acquire frame for output: {}", i);
                     Thread::yield();
                     continue;
                  }
                  else
                  {
                     DXGI_OUTDUPL_FRAME_INFO *info = acquiredFrame.getFrameInfo();
                     int accum_frames = info->AccumulatedFrames;
                     double dt = (double)(class ::time::now() - begins[i]).getTime(); // in milliseconds
                     m_plogwriter->debug("Acquire frame for output: {} for %f ms, accumulated {} frames", i,
                                         dt + ACQUIRE_TIMEOUT * timeouts[i], accum_frames);
                     timeouts[i] = 0;
                     WinD3D11Texture2D acquiredDesktopImage(acquiredFrame.getDxgiResource());

                     // Get metadata
                     if (info->TotalMetadataBufferSize)
                     {
                        size_t moveCount = m_outDupl[i].getFrameMoveRects(&m_moveRects);
                        size_t dirtyCount = m_outDupl[i].getFrameDirtyRects(&m_dirtyRects);

                        processMoveRects(moveCount, i);
                        processDirtyRects(dirtyCount, &acquiredDesktopImage, i);
                     }

                     // Check cursor pointer for updates.
                     try
                     {
                        processCursor(info, i);
                     }
                     catch (WinDxException &e)
                     {
                        m_plogwriter->debug("Error on cursor processing: {}, (%x)", e.get_message(),
                                            (int)e.getErrorCode());
                     } // Cursor
                  }
               }
               Thread::yield();
            }
         }
         // FIXME: remove it all, catch exceptions in Win8ScreenDriverImpl
      }
      catch (WinDxRecoverableException &e)
      {
         ::string errMess;
         errMess.formatf("Win8DeskDuplication:: Catched WinDxRecoverableException: {}, (%x)", e.get_message(),
                         (int)e.getErrorCode());
         setRecoverableError(errMess);
      }
      catch (WinDxCriticalException &e)
      {
         ::string errMess;
         errMess.formatf("Win8DeskDuplication:: Catched WinDxCriticalException: {}, (%x)", e.get_message(),
                         (int)e.getErrorCode());
         setRecoverableError(errMess); //?????????
         setCriticalError(errMess);
      }
      catch (::exception &e)
      {
         ::string errMess;
         errMess.formatf("Win8DeskDuplication:: Catched ::subsystem::Exception: {}", e.get_message());
         setRecoverableError(errMess);
      }
   }

   void Win8DeskDuplication::onTerminate() {}

   void Win8DeskDuplication::setCriticalError(const ::scoped_string &scopedstrReason)
   {
      m_hasCriticalError = true;
      m_duplListener->onCriticalError(reason);
   }

   void Win8DeskDuplication::setRecoverableError(const ::scoped_string &scopedstrReason)
   {
      m_hasRecoverableError = true;
      m_duplListener->onRecoverableError(reason);
   }

   ::int_size Win8DeskDuplication::getStageDimension(size_t out) const
   {
      return ::int_size(m_stageTextures2D[out].getDesc()->Width, m_stageTextures2D[out].getDesc()->Height);
   }

   void Win8DeskDuplication::processMoveRects(size_t moveCount, size_t out)
   {
      _ASSERT(moveCount <= m_moveRects.size());
      ::int_rectangle destinationRect;
      ::int_rectangle sourceRect;
      ::int_rectangle targetRect = m_targetRects[out];
      DXGI_MODE_ROTATION rotation = m_rotations[out];

      for (size_t iRect = 0; iRect < moveCount; iRect++)
      {
         destinationRect.fromWindowsRect(&m_moveRects[iRect].DestinationRect);
         sourceRect = destinationRect;
         POINT pointSource = m_moveRects[iRect].SourcePoint;
         sourceRect.set_top_left(pointSource.x, pointSource.y);
         rotateRectInsideStage(&destinationRect, &getStageDimension(out), rotation);
         rotateRectInsideStage(&sourceRect, &getStageDimension(out), rotation);
         // Translate the rectangle and point to the frame buffer coordinates.
         destinationRect.move(targetRect.left, targetRect.top);
         sourceRect.move(targetRect.left, targetRect.top);
         int x = sourceRect.left;
         int y = sourceRect.top;
         m_targetFb->move(&destinationRect, x, y);

         m_duplListener->onCopyRect(&destinationRect, x, y);
      }
   }

   void Win8DeskDuplication::processDirtyRects(size_t dirtyCount, WinD3D11Texture2D *acquiredDesktopImage, size_t out)
   {
      _ASSERT(dirtyCount <= m_dirtyRects.size());

      Region m_regionChanged;

      ::int_rectangle dirtyRect;
      ::int_size sizeStage = getStageDimension(out);
      ::int_rectangle stageRect = sizeStage;

      DXGI_MODE_ROTATION rotation = m_rotations[out];

      for (size_t iRect = 0; iRect < dirtyCount; iRect++)
      {
         dirtyRect.fromWindowsRect(&m_dirtyRects[iRect]);

         if (!stageRect.isFullyContainRect(&dirtyRect))
         {
            dirtyRect = dirtyRect.intersection(&stageRect);
            /* Disabled the followed throwing because it realy may happen and better is to see any picture
            // instead of a black screen.
            ::string errMess;
            errMess.formatf("During processDirtyRects has been got a rectangle ({}, {}, %dx{}) which outside"
                           " from the stage rectangle ({}, {}, %dx{})",
                           rectangle.left, rectangle.top, rectangle.width(), rectangle.height(),
                           stageRect.left, stageRect.top, stageRect.width(), stageRect.height());
            throw ::subsystem::Exception(errMess);
            */
         }
         ID3D11Texture2D *texture = m_stageTextures2D[out].getTexture();
         m_device.copySubresourceRegion(texture, dirtyRect.left, dirtyRect.top, acquiredDesktopImage->getTexture(),
                                        &dirtyRect, 0, 1);

         WinDxgiSurface surface(texture);
         WinAutoMapDxgiSurface autoMapSurface(&surface, DXGI_MAP_READ);

         ::int_rectangle rectangleTarget(dirtyRect);
         rotateRectInsideStage(&rectangleTarget, &sizeStage, rotation);
         // Translate the rectangle to the frame buffer coordinates.
         rectangleTarget.move(m_targetRects[out].left, m_targetRects[out].top);
         m_plogwriter->debug("Destination dirty rectangle = {}, {}, %dx{}", rectangleTarget.left, rectangleTarget.top, rectangleTarget.width(),
                             rectangleTarget.height());

         sizeStage.cx = static_cast<int>(autoMapSurface.getStride() / 4);
         m_auxiliaryFramebuffer.setPropertiesWithoutResize(&sizeStage, &m_targetFb->getPixelFormat());
         m_auxiliaryFramebuffer.setBuffer(autoMapSurface.getBuffer());
         switch (rotation)
         {
            case DXGI_MODE_ROTATION_UNSPECIFIED:
            case DXGI_MODE_ROTATION_IDENTITY:
            {
               m_targetFb->copyFrom(&rectangleTarget, &m_auxiliaryFramebuffer, dirtyRect.left, dirtyRect.top);
               break;
            }
            case DXGI_MODE_ROTATION_ROTATE90:
            {
               m_targetFb->copyFromRotated90(&rectangleTarget, &m_auxiliaryFramebuffer, dirtyRect.left, dirtyRect.top);
               break;
            }
            case DXGI_MODE_ROTATION_ROTATE180:
            {
               m_targetFb->copyFromRotated180(&rectangleTarget, &m_auxiliaryFramebuffer, dirtyRect.left, dirtyRect.top);
               break;
            }
            case DXGI_MODE_ROTATION_ROTATE270:
            {
               m_targetFb->copyFromRotated270(&rectangleTarget, &m_auxiliaryFramebuffer, dirtyRect.left, dirtyRect.top);
               break;
            }
         }
         m_auxiliaryFramebuffer.setBuffer(0);

         m_regionChanged.addRect(&rectangleTarget);
      }

      m_duplListener->onFramebufferUpdate(&m_regionChanged);
   }

   void Win8DeskDuplication::rotateRectInsideStage(::int_rectangle &rectangleToTranspose, const ::int_size &sizeStage,
                                                   DXGI_MODE_ROTATION rotation)
   {
      int left = rectangleToTranspose.left;
      int top = rectangleToTranspose.top;
      int width = rectangleToTranspose.width();
      int height = rectangleToTranspose.height();
      switch (rotation)
      {
         case DXGI_MODE_ROTATION_UNSPECIFIED:
         case DXGI_MODE_ROTATION_IDENTITY:
         {
            return;
         }
         case DXGI_MODE_ROTATION_ROTATE90:
         {
            rectangleToTranspose->rotateOn90InsideDimension(sizeStage->height);
            break;
         }
         case DXGI_MODE_ROTATION_ROTATE180:
         {
            rectangleToTranspose->rotateOn180InsideDimension(sizeStage->width, sizeStage->height);
            break;
         }
         case DXGI_MODE_ROTATION_ROTATE270:
         {
            rectangleToTranspose->rotateOn270InsideDimension(sizeStage->width);
            break;
         }
      }
   }

   void Win8DeskDuplication::processCursor(const DXGI_OUTDUPL_FRAME_INFO *info, size_t out)
   {
      critical_section_lock al(m_cursorMutex);
      LONGLONG lastUpdateTime = info->LastMouseUpdateTime.QuadPart;
      if (lastUpdateTime != 0 && lastUpdateTime > *m_cursorTimeStamp)
      {
         *m_cursorTimeStamp = lastUpdateTime;

         //
         DXGI_OUTDUPL_POINTER_POSITION pointerPos = info->PointerPosition;

         bool newVisibility = pointerPos.Visible != false;
         bool visibleChanged = m_targetCurShape->getIsVisible() != newVisibility;
         if (visibleChanged)
         {
     m_plogwriter->debug(newVisibility ? "Cursor became visible") : "Cursor became not visible";
     m_targetCurShape->setVisibility(newVisibility, out);
     m_duplListener->onCursorShapeChanged();
         }

         //
         bool shapeChanged = info->PointerShapeBufferSize != 0;
         if (shapeChanged)
         {
            m_plogwriter->debug("Cursor shape chagned");
            m_outDupl[out].getFrameCursorShape(m_targetCurShape->getCursorShapeForWriting(),
                                               info->PointerShapeBufferSize, m_plogwriter);
            m_duplListener->onCursorShapeChanged();
         }

         if (pointerPos.Visible)
         {
            m_plogwriter->debug("Cursor pointPosition chagned");
            ::int_point hotPoint = m_targetCurShape->getCursorShape()->getHotSpot();
            ::int_rectangle targetRect = m_targetRects[out];
            m_duplListener->onCursorPositionChanged(pointerPos.Position.x + targetRect.left + hotPoint.x,
                                                    pointerPos.Position.y + targetRect.top + hotPoint.y);
         }
      }
   }


} // namespace remoting
 
