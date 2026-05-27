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
#include "remoting/remoting_rfb_windows/desktop/WinDxRecoverableException.h"
#include "remoting/remoting_rfb_windows/desktop/WinDxCriticalException.h"
//#include "subsystem/thread/lockable_critical_section.h"
#include "remoting/remoting_rfb_windows/desktop/DXGIAcquiredFrame.h"
#include "remoting/remoting_rfb_windows/desktop/WinD3D11Texture2D.h"
#include "remoting/remoting_rfb_windows/desktop/WinAutoMapDxgiSurface.h"
#include "remoting/remoting_rfb_windows/desktop/Win8DeskDuplicationThread.h"
//#include "remoting/remoting_rfb_windows/desktop/WinDxgiOutput1.h"
#include "remoting/remoting_rfb_windows/desktop/D3D11Device.h"


namespace remoting_rfb_windows
{


   Win8DeskDuplication::Win8DeskDuplication(::innate_subsystem::Framebuffer *targetFb,
                                            ::int_rectangle_array_base &targetRect, Win8CursorShape *targetCurShape,
                                            LONGLONG *cursorTimeStamp, lockable_critical_section *cursorMutex,
                                            Win8DuplicationListener *duplListener,
                                            D3D11Device * pd3d11device,
                                            const ::array_base<::comptr < IDXGIOutput > > & dxgioutputa, ::subsystem::LogWriter * plogwriter) :
       m_targetFb(targetFb), m_targetRects(targetRect), m_targetCurShape(targetCurShape),
       m_cursorTimeStamp(cursorTimeStamp), m_cursorMutex(cursorMutex), m_duplListener(duplListener), m_pdevice(pd3d11device),
       m_hasCriticalError(false), m_hasRecoverableError(false), m_plogwriter(plogwriter)
   {
      m_plogwriter->debug("Creating Win8DeskDuplication for {} outputs", dxgioutputa.size());
      for (size_t i = 0; i < dxgioutputa.size(); i++)
      {
         m_dxgioutput1a.add(dxgioutputa[i].as<IDXGIOutput1>());
         m_outputduplicationa.add(allocateø DXGIOutputDuplication(m_dxgioutput1a[i], m_pdevice));
         DXGI_OUTPUT_DESC desc;
         HRESULT hr = m_dxgioutput1a[i]->GetDesc(&desc);
         if (FAILED(hr))
         {
            throw WinDxCriticalException("Can't get output description", hr);
         }
         m_rotations.add(desc.Rotation);

         D3D11_TEXTURE2D_DESC desc2{};
         if (desc.Rotation == DXGI_MODE_ROTATION_ROTATE90 || desc.Rotation == DXGI_MODE_ROTATION_ROTATE270)
         {
            desc2.Width = targetRect[i].height();
            desc2.Height = targetRect[i].width();
         }
         else
         {
            desc2.Width = targetRect[i].width();
            desc2.Height = targetRect[i].height();
         }
         desc2.MipLevels = 1;
         desc2.ArraySize = 1;
         desc2.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
         desc2.SampleDesc.Count = 1;
         desc2.SampleDesc.Quality = 0;
         desc2.Usage = D3D11_USAGE_STAGING;
         desc2.BindFlags = 0;
         desc2.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
         desc2.MiscFlags = 0;
         ::comptr < ID3D11Texture2D > pd3d11texture2d;
         HRESULT hrCreateTexture = pd3d11device->m_pd3d11device->CreateTexture2D(&desc2, 0, &pd3d11texture2d);

         if (FAILED(hrCreateTexture) || !pd3d11texture2d)
         {

            throw WinDxRecoverableException("Can't CreateTexture2D()", hr);

         }
         //m_textureaStage.add(allocateø WinCustomD3D11Texture2D(m_pdevice->getDevice(), (::u32)targetRect[i].width(),
           //                                            (::u32)targetRect[i].height(), m_rotations[i]));
         m_textureaStage.add(pd3d11texture2d);
      }
      m_plogwriter->debug("Win8DeskDuplication created");

               timeouts.resize(m_outputduplicationa.size());
         begins.resize(m_outputduplicationa.size());

      //resumeThread();
   }

   Win8DeskDuplication::~Win8DeskDuplication()
   {
      m_plogwriter->debug("deleting Win8DeskDuplication");
      //setThreadToFinish();
      //waitThreadToFinish();
   }

   bool Win8DeskDuplication::isValid() { return !m_hasRecoverableError && !m_hasCriticalError; }


   bool Win8DeskDuplication::onRunStep()
   {
      if (!::task_get_run() || !isValid())
      {
         return false;
      }
       const ::i32 ACQUIRE_TIMEOUT = 50;
         for (size_t i = 0; i < m_outputduplicationa.size(); i++)
         {
            auto &poutputduplication = m_outputduplicationa[i];
            {
               begins[i].Now();
               auto pwindxgiacquiredframe = allocateø DXGIAcquiredFrame(poutputduplication, ACQUIRE_TIMEOUT);
               if (pwindxgiacquiredframe->wasTimeOut())
               {
                  timeouts[i]++;
                  m_plogwriter->debug("Timeout on acquire frame for output: {}", i);
                  //Thread::threadYield();
                  continue;
               }
               else
               {
                  DXGI_OUTDUPL_FRAME_INFO *info = pwindxgiacquiredframe->getFrameInfo();
                  ::i32 accum_frames = info->AccumulatedFrames;
                  ::f64 dt = begins[i].elapsed().floating_millisecond(); // in milliseconds
                  m_plogwriter->debug("Acquire frame for output: {} for %f ms, accumulated {} frames", i,
                                      dt + ACQUIRE_TIMEOUT * timeouts[i], accum_frames);
                  timeouts[i] = 0;

                  ::comptr<ID3D11Texture2D> ptextureAcquiredDesktopImage;

                  HRESULT hr = pwindxgiacquiredframe->m_pdxgiresourceDesktop.as(ptextureAcquiredDesktopImage);

                  if (FAILED(hr) || !ptextureAcquiredDesktopImage)
                  {

                     hrErrorRecoverable = hr;

                     strErrorRecoverable = "Can't QueryInterface() to create ID3D11Texture2D";

                     break;
                  }

                  // Get metadata
                  if (info->TotalMetadataBufferSize)
                  {
                     size_t moveCount = poutputduplication->getFrameMoveRects(&m_moveRects);
                     size_t dirtyCount = poutputduplication->getFrameDirtyRects(&m_dirtyRects);

                     processMoveRects(moveCount, i);
                     processDirtyRects(dirtyCount, ptextureAcquiredDesktopImage, i);
                  }

                  // Check cursor pointer for updates.
                  try
                  {
                     processCursor(info, i);
                  }
                  catch (WinDxException &e)
                  {
                     m_plogwriter->debug("Error on cursor processing: {}, (%x)", e.get_message(),
                                         (::i32)e.getErrorCode());
                  } // Cursor
               }
            }
            //Thread::threadYield();
         }
      //}
   return true;

   }

   void Win8DeskDuplication::onThreadMain()
   {
      //const ::i32 ACQUIRE_TIMEOUT = 20;
      ////try
      ////{
      //   ::array_base<::i32> timeouts;
      //   ::array_base<class ::time> begins;
      //   timeouts.resize(m_outputduplicationa.size());
      //   begins.resize(m_outputduplicationa.size());

      //   HRESULT hrErrorRecoverable = S_OK;
      //   ::string strErrorRecoverable;
      //   HRESULT hrErrorCritical = S_OK;
      //   ::string strErrorCritical;
      //   ::string strException;

      //   while (onRunStep())
      //   {
      //      for (size_t i = 0; i < m_outputduplicationa.size(); i++)
      //      {
      //         auto & poutputduplication = m_outputduplicationa[i];
      //         {
      //            begins[i].Now();
      //            auto pwindxgiacquiredframe = allocateø DXGIAcquiredFrame(poutputduplication, ACQUIRE_TIMEOUT);
      //            if (pwindxgiacquiredframe->wasTimeOut())
      //            {
      //               timeouts[i]++;
      //               m_plogwriter->debug("Timeout on acquire frame for output: {}", i);
      //               Thread::threadYield();
      //               continue;
      //            }
      //            else
      //            {
      //               DXGI_OUTDUPL_FRAME_INFO *info = pwindxgiacquiredframe->getFrameInfo();
      //               ::i32 accum_frames = info->AccumulatedFrames;
      //               ::f64 dt = begins[i].elapsed().floating_millisecond(); // in milliseconds
      //               m_plogwriter->debug("Acquire frame for output: {} for %f ms, accumulated {} frames", i,
      //                                   dt + ACQUIRE_TIMEOUT * timeouts[i], accum_frames);
      //               timeouts[i] = 0;

      //               ::comptr < ID3D11Texture2D > ptextureAcquiredDesktopImage;

      //               HRESULT hr = pwindxgiacquiredframe->m_pdxgiresourceDesktop.as(ptextureAcquiredDesktopImage);

      //               if (FAILED(hr) || !ptextureAcquiredDesktopImage)
      //               {

      //                  hrErrorRecoverable = hr;

      //                  strErrorRecoverable = "Can't QueryInterface() to create ID3D11Texture2D";

      //                  break;

      //               }

      //               // Get metadata
      //               if (info->TotalMetadataBufferSize)
      //               {
      //                  size_t moveCount = poutputduplication->getFrameMoveRects(&m_moveRects);
      //                  size_t dirtyCount = poutputduplication->getFrameDirtyRects(&m_dirtyRects);

      //                  processMoveRects(moveCount, i);
      //                  processDirtyRects(dirtyCount, ptextureAcquiredDesktopImage, i);
      //               }

      //               // Check cursor pointer for updates.
      //               try
      //               {
      //                  processCursor(info, i);
      //               }
      //               catch (WinDxException &e)
      //               {
      //                  m_plogwriter->debug("Error on cursor processing: {}, (%x)", e.get_message(),
      //                                      (::i32)e.getErrorCode());
      //               } // Cursor
      //            }
      //         }
      //         Thread::threadYield();
      //      }
      //   }
      //   // FIXME: remove it all, catch exceptions in Win8ScreenDriverImpl
      ////}

      //if (strErrorRecoverable.has_character())
      //{
      //   ::string errMess;
      //   errMess.format("Win8DeskDuplication:: Catched WinDxRecoverableException: {}, (%x)",
      //                        strErrorRecoverable,
      //                   (::i32)hrErrorRecoverable);
      //   setRecoverableError(errMess);
      //}
      //else if (strErrorCritical.has_character())
      //{
      //   ::string errMess;
      //   errMess.format("Win8DeskDuplication:: Catched WinDxCriticalException: {}, (%x)", strErrorCritical,
      //                   (::i32)hrErrorCritical);
      //   setRecoverableError(errMess); //?????????
      //   setCriticalError(errMess);
      //}
      //else if (strException.has_character())
      //{
      //   ::string errMess;
      //   errMess.format("Win8DeskDuplication:: Catched ::subsystem::Exception: {}", strException);
      //   setRecoverableError(errMess); //?????????
      //   setCriticalError(errMess);
      //}
   }

   void Win8DeskDuplication::onTermThread() {}

   void Win8DeskDuplication::setCriticalError(const ::scoped_string &scopedstrReason)
   {
      m_hasCriticalError = true;
      m_duplListener->onCriticalError(scopedstrReason);
   }

   void Win8DeskDuplication::setRecoverableError(const ::scoped_string &scopedstrReason)
   {
      m_hasRecoverableError = true;
      m_duplListener->onRecoverableError(scopedstrReason);
   }

   ::i32_size Win8DeskDuplication::getStageDimension(size_t out) const
   {

      D3D11_TEXTURE2D_DESC desc{};

      m_textureaStage[out]->GetDesc(&desc);

      return ::i32_size(desc.Width, desc.Height);
   }

   void Win8DeskDuplication::processMoveRects(size_t moveCount, size_t out)
   {
      ASSERT(moveCount <= m_moveRects.size());
      ::i32_rectangle destinationRect;
      ::i32_rectangle sourceRect;
      ::i32_rectangle targetRect = m_targetRects[out];
      DXGI_MODE_ROTATION rotation = m_rotations[out];

      for (size_t iRect = 0; iRect < moveCount; iRect++)
      {
         destinationRect = m_moveRects[iRect].DestinationRect;
         sourceRect = destinationRect;
         POINT pointSource = m_moveRects[iRect].SourcePoint;
         sourceRect.set_top_left(pointSource.x, pointSource.y);
         rotateRectInsideStage(destinationRect, getStageDimension(out), rotation);
         rotateRectInsideStage(sourceRect, getStageDimension(out), rotation);
         // Translate the rectangle and point to the frame buffer coordinates.
         destinationRect.offset(targetRect.left, targetRect.top);
         sourceRect.offset(targetRect.left, targetRect.top);
         ::i32 x = sourceRect.left;
         ::i32 y = sourceRect.top;
         m_targetFb->move(destinationRect, x, y);

         m_duplListener->onCopyRect(destinationRect, x, y);
      }

   }


   void Win8DeskDuplication::processDirtyRects(size_t dirtyCount, ID3D11Texture2D * ptextureAcquiredDesktopImage, size_t out)
   {

      ASSERT(dirtyCount <= m_dirtyRects.size());

      ::remoting_rfb::Region m_regionChanged;

      ::i32_rectangle dirtyRect;
      ::i32_size sizeStage = getStageDimension(out);
      ::i32_rectangle stageRect = sizeStage;

      DXGI_MODE_ROTATION rotation = m_rotations[out];

      for (size_t iRect = 0; iRect < dirtyCount; iRect++)
      {
         dirtyRect = m_dirtyRects[iRect];

         if (!stageRect.contains(dirtyRect))
         {
            dirtyRect = dirtyRect.intersection(stageRect);
            /* Disabled the followed throwing because it realy may happen and better is to see any picture
            // instead of a black screen.
            ::string errMess;
            errMess.format("During processDirtyRects has been got a rectangle ({}, {}, %dx{}) which outside"
                           " from the stage rectangle ({}, {}, %dx{})",
                           rectangle.left, rectangle.top, rectangle.width(), rectangle.height(),
                           stageRect.left, stageRect.top, stageRect.width(), stageRect.height());
            throw ::subsystem::Exception(errMess);
            */
         }
         ::comptr < ID3D11Texture2D  > pd3d11texture2d = m_textureaStage[out];
         m_pdevice->copySubresourceRegion(pd3d11texture2d, dirtyRect.left, dirtyRect.top,
            ptextureAcquiredDesktopImage,
                                        dirtyRect, 0, 1);

         ::comptr < IDXGISurface > pdxgisurface;
         pd3d11texture2d.as(pdxgisurface);
         auto pautomapsurface = allocateø WinAutoMapDxgiSurface(pdxgisurface, DXGI_MAP_READ);

         ::i32_rectangle rectangleTarget(dirtyRect);
         rotateRectInsideStage(rectangleTarget, sizeStage, rotation);
         // Translate the rectangle to the frame buffer coordinates.
         rectangleTarget.offset(m_targetRects[out].left, m_targetRects[out].top);
         m_plogwriter->debug("Destination dirty rectangle = {}, {}, %dx{}", rectangleTarget.left, rectangleTarget.top, rectangleTarget.width(),
                             rectangleTarget.height());

         sizeStage.cx = static_cast<::i32>(pautomapsurface->getStride() / 4);
         construct_newø(m_pframebufferAuxiliaryProperty);

         m_pframebufferAuxiliaryProperty->setPropertiesWithoutResize(sizeStage, m_targetFb->getPixelFormat());
         m_pframebufferAuxiliaryProperty->setBuffer(pautomapsurface->getBuffer());
         switch (rotation)
         {
            case DXGI_MODE_ROTATION_UNSPECIFIED:
            case DXGI_MODE_ROTATION_IDENTITY:
            {
               m_targetFb->copyFrom(rectangleTarget, m_pframebufferAuxiliaryProperty, dirtyRect.left, dirtyRect.top);
               break;
            }
            case DXGI_MODE_ROTATION_ROTATE90:
            {
               m_targetFb->copyFromRotated90(rectangleTarget, m_pframebufferAuxiliaryProperty, dirtyRect.left, dirtyRect.top);
               break;
            }
            case DXGI_MODE_ROTATION_ROTATE180:
            {
               m_targetFb->copyFromRotated180(rectangleTarget, m_pframebufferAuxiliaryProperty, dirtyRect.left, dirtyRect.top);
               break;
            }
            case DXGI_MODE_ROTATION_ROTATE270:
            {
               m_targetFb->copyFromRotated270(rectangleTarget, m_pframebufferAuxiliaryProperty, dirtyRect.left, dirtyRect.top);
               break;
            }
         }
         m_pframebufferAuxiliaryProperty->setBuffer(0);

         m_regionChanged.addRect(rectangleTarget);
      }

      m_duplListener->onFramebufferUpdate(m_regionChanged);
   }


   void Win8DeskDuplication::rotateRectInsideStage(::i32_rectangle &rectangleToTranspose, const ::i32_size &sizeStage,
                                                   DXGI_MODE_ROTATION rotation)
   {
      ::i32 left = rectangleToTranspose.left;
      ::i32 top = rectangleToTranspose.top;
      ::i32 width = rectangleToTranspose.width();
      ::i32 height = rectangleToTranspose.height();
      switch (rotation)
      {
         case DXGI_MODE_ROTATION_UNSPECIFIED:
         case DXGI_MODE_ROTATION_IDENTITY:
         {
            return;
         }
         case DXGI_MODE_ROTATION_ROTATE90:
         {
            rectangleToTranspose.rotateOn90InsideDimension(sizeStage.cy);
            break;
         }
         case DXGI_MODE_ROTATION_ROTATE180:
         {
            rectangleToTranspose.rotateOn180InsideDimension(sizeStage.cx, sizeStage.cy);
            break;
         }
         case DXGI_MODE_ROTATION_ROTATE270:
         {
            rectangleToTranspose.rotateOn270InsideDimension(sizeStage.cx);
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
     m_plogwriter->debug(newVisibility ? "Cursor became visible" : "Cursor became not visible");
     m_targetCurShape->setVisibility(newVisibility, out);
     m_duplListener->onCursorShapeChanged();
         }

         //
         bool shapeChanged = info->PointerShapeBufferSize != 0;
         if (shapeChanged)
         {
            m_plogwriter->debug("Cursor shape chagned");
            m_outputduplicationa[out]->getFrameCursorShape(m_targetCurShape->getCursorShapeForWriting(),
                                               info->PointerShapeBufferSize, m_plogwriter);
            m_duplListener->onCursorShapeChanged();
         }

         if (pointerPos.Visible)
         {
            m_plogwriter->debug("Cursor pointPosition chagned");
            ::i32_point hotPoint = m_targetCurShape->getCursorShape()->getHotSpot();
            ::i32_rectangle targetRect = m_targetRects[out];
            m_duplListener->onCursorPositionChanged(pointerPos.Position.x + targetRect.left + hotPoint.x,
                                                    pointerPos.Position.y + targetRect.top + hotPoint.y);
         }
      }
   }


} // namespace remoting_rfb_windows
 
