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
#include "remoting/remoting_windows/desktop/WinDxCriticalException.h"
#include "remoting/remoting_windows/desktop/WinDxRecoverableException.h"
#include "subsystem/framebuffer/StandardPixelFormatFactory.h"
#include "remoting/remoting_windows/desktop/WinCursorShapeUtils.h"

// The header including of this cpp file must be at last place to avoid build conflicts.
#include "remoting/remoting_windows/desktop/WinDxgiOutputDuplication.h"

namespace remoting_windows
{


   WinDxgiOutputDuplication::WinDxgiOutputDuplication(WinDxgiOutput1 *dxgiOutput, WinD3D11Device *d3D11Device) :
       m_outDupl(0)
   {

      auto pdxgiOutput = dxgiOutput;

      auto pdxgiOutput1 = dxgiOutput->getDxgiOutput1();

      auto pd3D11Device = d3D11Device;

      auto pdevice = pd3D11Device->getDevice();
      HRESULT hr = pdxgiOutput1->DuplicateOutput(pdevice, &m_outDupl);
      if (FAILED(hr))
      {
         if (hr == DXGI_ERROR_NOT_CURRENTLY_AVAILABLE)
         {
            throw WinDxRecoverableException("Can't DuplicateOutput() because resource doesn't available at the time",
                                            hr);
         }
         else if (hr == E_ACCESSDENIED)
         {
            throw WinDxRecoverableException("Can't DuplicateOutput() because of access denied error", hr);
         }
         else
         {
            throw WinDxCriticalException("Can't DuplicateOutput()", hr);
         }
      }
   }

   WinDxgiOutputDuplication::WinDxgiOutputDuplication(const WinDxgiOutputDuplication &src) { copy(src); }

   WinDxgiOutputDuplication::~WinDxgiOutputDuplication()
   {
      if (m_outDupl != 0)
      {
         m_outDupl->Release();
         m_outDupl = 0;
      }
   }


   WinDxgiOutputDuplication &WinDxgiOutputDuplication::operator=(WinDxgiOutputDuplication const &src)
   {
      copy(src);
      return *this;
   }

   void WinDxgiOutputDuplication::copy(const WinDxgiOutputDuplication &src)
   {
      if (this != &src)
      {
         m_outDupl = src.m_outDupl;
         m_outDupl->AddRef();
      }
   }

   IDXGIOutputDuplication *WinDxgiOutputDuplication::getDxgiOutputDuplication() { return m_outDupl; }

   size_t WinDxgiOutputDuplication::getFrameMoveRects(::array_base<DXGI_OUTDUPL_MOVE_RECT> *moveRects)
   {
      // Get move rectangle buffer size.
      char stub;
      ::u32 reqBufSize = 0;
      HRESULT hr;
      hr = m_outDupl->GetFrameMoveRects(reqBufSize, reinterpret_cast<DXGI_OUTDUPL_MOVE_RECT *>(&stub), &reqBufSize);
      if (!FAILED(hr))
      {
         return 0;
      }
      else if (hr != DXGI_ERROR_MORE_DATA)
      {
         throw WinDxException("Can't get buffer size to get move rectanglea", hr);
      }

      size_t elementSize = sizeof((*moveRects)[0]);
      ::u32 bufSize = (::u32)(moveRects->size() * elementSize);
      if (reqBufSize > bufSize)
      {
         moveRects->resize(reqBufSize / elementSize);
         bufSize = (::u32)(moveRects->size() * elementSize);
      }

      // Get move rectangles.
      hr = m_outDupl->GetFrameMoveRects(bufSize, moveRects->data(), &bufSize);
      if (FAILED(hr))
      {
         throw WinDxException("Can't get move rectanglea", hr);
      }
      return bufSize / elementSize;
   }

   size_t WinDxgiOutputDuplication::getFrameDirtyRects(::array_base<RECT> *dirtyRects)
   {
      // Get dirty rectangle buffer size.
      char stub;
      ::u32 reqBufSize = 0;
      HRESULT hr;
      hr = m_outDupl->GetFrameDirtyRects(reqBufSize, reinterpret_cast<RECT *>(&stub), &reqBufSize);
      if (!FAILED(hr))
      {
         return 0;
      }
      else if (hr != DXGI_ERROR_MORE_DATA)
      {
         throw WinDxException("Can't get buffer size to get dirty rectanglea", hr);
      }

      size_t elementSize = sizeof((*dirtyRects)[0]);
      ::u32 bufSize = (::u32)(dirtyRects->size() * elementSize);
      if (reqBufSize > bufSize)
      {
         dirtyRects->resize(reqBufSize / elementSize);
         bufSize = (::u32)(dirtyRects->size() * elementSize);
      }

      // Get dirty rectangles.
      hr = m_outDupl->GetFrameDirtyRects(bufSize, dirtyRects->data(), &bufSize);
      if (FAILED(hr))
      {
         throw WinDxException("Can't get dirty rectanglea", hr);
      }
      return bufSize / elementSize;
   }

   void WinDxgiOutputDuplication::getFrameCursorShape(::remoting::CursorShape *cursorShape, ::u32 pointerShapeBufferSize,
                                                      ::subsystem::LogWriter * plogwriter)
   {
      // plogwriter->debug("{}", pointerShapeBufferSize);
      //  This function can calculate required buffer size by self but the size is already known.
      if (pointerShapeBufferSize == 0)
      {
         cursorShape->resetToEmpty();
         return;
      }
      HRESULT hr;
      ::u32 reqSize = 0;
      ::array_base<char> buffer(pointerShapeBufferSize);
      DXGI_OUTDUPL_POINTER_SHAPE_INFO shapeInfo;
      hr = m_outDupl->GetFramePointerShape((::u32)buffer.size(), buffer.data(), &reqSize, &shapeInfo);
      plogwriter->debug("CursorShapeInfo: pounter info buffer size: {}, required: {}", pointerShapeBufferSize, reqSize);
      if (FAILED(hr))
      {
         throw WinDxException("Can't get frame cursor shape with GetFramePointerShape() calling", hr);
      }

      plogwriter->debug("CursorShapeInfo: Type: {}", shapeInfo.Type);
      plogwriter->debug("CursorShapeInfo: Width: {}, Height: {}", shapeInfo.Width, shapeInfo.Height);
      plogwriter->debug("CursorShapeInfo: shapeInfo.HotSpot.x: {}, , shapeInfo.HotSpot.y: {}", shapeInfo.HotSpot.x,
                 shapeInfo.HotSpot.y);
      plogwriter->debug("CursorShapeInfo: Pitch: {}", shapeInfo.Pitch);

      buffer.resize(reqSize);

      ::remoting::CursorShape newCursorShape;
      ::innate_subsystem::PixelFormat pixelformat = ::innate_subsystem::StandardPixelFormatFactory::create32bppPixelFormat();
      newCursorShape.setHotSpot(shapeInfo.HotSpot.x, shapeInfo.HotSpot.y);

      ::u32 pitch;
      ::i32_size size;

      if (shapeInfo.Type == DXGI_OUTDUPL_POINTER_SHAPE_TYPE_MONOCHROME)
      {
         pitch = ((shapeInfo.Width + 15) / 16) * 2;
      }
      else
      {
         pitch = shapeInfo.Width * 4;
      }

      if (shapeInfo.Pitch > pitch)
      {
         WinCursorShapeUtils::trimBuffer(&buffer, &shapeInfo);
         pitch = shapeInfo.Pitch;
         plogwriter->debug("Trimmed CursorShapeInfo: Width: {}, Height: {}", shapeInfo.Width, shapeInfo.Height);
         plogwriter->debug("Trimmed CursorShapeInfo: Pitch: {}", shapeInfo.Pitch);
      }

      if (shapeInfo.Type == DXGI_OUTDUPL_POINTER_SHAPE_TYPE_MONOCHROME)
      {
         size.set(shapeInfo.Width, shapeInfo.Height / 2);
      }
      else
      {
         size.set(shapeInfo.Width, shapeInfo.Height);
      }
      newCursorShape.setProperties(size, pixelformat);

      // monochrome cursor
      if (shapeInfo.Type == DXGI_OUTDUPL_POINTER_SHAPE_TYPE_MONOCHROME)
      {
         WinCursorShapeUtils::winMonoShapeToRfb(newCursorShape.getPixels(), buffer.data(), &buffer[pitch * size.cy],
                                                pitch);
         newCursorShape.assignMaskFromWindows(buffer.data());
         cursorShape->clone(&newCursorShape);
         return;
      }

      // 32bpp cursor
      // Check buffer size.
      size_t shapeSize = newCursorShape.getPixelsSize();
      if (shapeSize > buffer.size())
      {
         throw ::subsystem::Exception("Invalid buffer size for color cursor.");
      }
      memcpy(newCursorShape.getPixels()->getBuffer(), buffer.data(), shapeSize);
      int maskPitch = ((size.cx + 15) / 16) * 2;
      ::memory mask;
      mask.set_size(maskPitch * size.cy);
      mask.zero();
      bool maskedColor = shapeInfo.Type == DXGI_OUTDUPL_POINTER_SHAPE_TYPE_MASKED_COLOR;
      WinCursorShapeUtils::winColorShapeToRfb<::u32>(newCursorShape.getPixels(), (char*)mask.data(), maskPitch);
      WinCursorShapeUtils::fixAlphaChannel(newCursorShape.getPixels(), (char*)mask.data(), maskedColor, maskPitch);
      newCursorShape.assignMaskFromWindows((char*)mask.data()); // assumes width is aligned to 2 bytes
      cursorShape->clone(&newCursorShape);
   }


} // namespace remoting_windows
 