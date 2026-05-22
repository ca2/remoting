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
#include "remoting/remoting_windows/desktop/WinDxCriticalException.h"
#include "remoting/remoting_windows/desktop/WinDxRecoverableException.h"
#include "subsystem/framebuffer/StandardPixelFormatFactory.h"
#include "remoting/remoting_windows/desktop/WinCursorShapeUtils.h"

// The header including of this cpp file must be at last place to avoid build conflicts.
#include "remoting/remoting_windows/desktop/DXGIOutputDuplication.h"
#include "remoting/remoting_windows/desktop/D3D11Device.h"


// dxgi_diagnostic.cpp
//
// Compile with:
// cl /EHsc dxgi_diagnostic.cpp /link dxgi.lib d3d11.lib wtsapi32.lib
//
// Prints diagnostic information useful before calling
// IDXGIOutput1::DuplicateOutput()

#include <windows.h>
#include <wtsapi32.h>

#include <d3d11.h>
#include <dxgi.h>
#include <dxgi1_2.h>

#include <iomanip>
#include <iostream>
#include <string>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "wtsapi32.lib")

namespace str
{

   template < prototype_integral INTEGRAL >
   ::string lohex(INTEGRAL i)
   {

      return ::hex::lower_case_from(i);

   }

   template<prototype_integral INTEGRAL>
   ::string dec(INTEGRAL i)
   {

      return ::as_string(i);
   }
} // namespace str

const char *FeatureLevelToString(D3D_FEATURE_LEVEL fl)
{
   switch (fl)
   {
      case D3D_FEATURE_LEVEL_11_1:
         return "11.1";
      case D3D_FEATURE_LEVEL_11_0:
         return "11.0";
      case D3D_FEATURE_LEVEL_10_1:
         return "10.1";
      case D3D_FEATURE_LEVEL_10_0:
         return "10.0";
      case D3D_FEATURE_LEVEL_9_3:
         return "9.3";
      case D3D_FEATURE_LEVEL_9_2:
         return "9.2";
      case D3D_FEATURE_LEVEL_9_1:
         return "9.1";
      default:
         return "Unknown";
   }
}

static ::wstring HResultToString(HRESULT hr)
{
   wchar_t *buffer = nullptr;

   FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr,
                  hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&buffer, 0, nullptr);

   std::wstring result = buffer ? buffer : L"(unknown)";
   LocalFree(buffer);

   return result;
}

::string dxgi_d3d11_diagnostic_for_duplicate_output1()
{
   string wstr;

   wstr << L"===== DXGI / D3D11 Diagnostic =====\n\n";

   //
   // Session info
   //
   DWORD activeSession = WTSGetActiveConsoleSessionId();
   DWORD currentSession = 0;

   ProcessIdToSessionId(GetCurrentProcessId(), &currentSession);

   wstr << L"Process ID:          " << GetCurrentProcessId() << L"\n";
   wstr << L"Current Session ID:  " << currentSession << L"\n";
   wstr << L"Active Console ID:   " << activeSession << L"\n";

   if (currentSession == activeSession)
      wstr << L"Interactive Session: YES\n";
   else
      wstr << L"Interactive Session: NO\n";

   //
   // Elevation info
   //
   HANDLE token = nullptr;
   if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token))
   {
      TOKEN_ELEVATION elevation{};
      DWORD size = 0;

      if (GetTokenInformation(token, TokenElevation, &elevation, sizeof(elevation), &size))
      {
         wstr << L"Elevated:            " << (elevation.TokenIsElevated ? L"YES" : L"NO") << L"\n";
      }

      CloseHandle(token);
   }

   wstr << L"\n";

   return wstr;

}

::string dxgi_d3d11_diagnostic_for_duplicate_output2(IDXGIAdapter1 *adapter, UINT adapterIndex)
{

   ::string wstr;

   ////
   //// Create DXGI factory
   ////
   // IDXGIFactory1 *factory = nullptr;

   // HRESULT hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void **)&factory);

   // if (FAILED(hr))
   //{
   //    wstr << L"CreateDXGIFactory1 failed: 0x" << ::str::lohex(hr) << L"\n";

   //   return 1;
   //}

   //
   // Enumerate adapters
   //
   // UINT adapterIndex = 0;
   // IDXGIAdapter1 *adapter = nullptr;

   // while (factory->EnumAdapters1(adapterIndex, &adapter) != DXGI_ERROR_NOT_FOUND)
   //{
   DXGI_ADAPTER_DESC1 desc{};
   adapter->GetDesc1(&desc);

   wstr << L"=====================================\n";
   wstr << L"Adapter #" << adapterIndex << L"\n";
   wstr << L"=====================================\n";

   wstr << L"Description:         " << desc.Description << L"\n";
   wstr << L"Vendor ID:           0x" << ::str::lohex(desc.VendorId) << L"\n";
   wstr << L"Device ID:           0x" << ::str::lohex(desc.DeviceId) << L"\n";
   wstr << L"Dedicated VRAM:      " << ::str::dec(desc.DedicatedVideoMemory / (1024 * 1024)) << L" MB\n";

   bool isSoftware = (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) != 0;

   wstr << L"Software Adapter:    " << (isSoftware ? L"YES" : L"NO") << L"\n";

   return wstr;
}


::string dxgi_d3d11_diagnostic_for_duplicate_output2(UINT outputIndex, IDXGIOutput *output)
{

   ::string wstr;
   ////
   //// Create D3D11 device ON THIS ADAPTER
   ////
   // ID3D11Device *device = nullptr;
   // ID3D11DeviceContext *context = nullptr;

   // D3D_FEATURE_LEVEL obtainedLevel{};

   // UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

   // D3D_FEATURE_LEVEL levels[] = {D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1,
   //                               D3D_FEATURE_LEVEL_10_0};

   // hr = D3D11CreateDevice(adapter, D3D_DRIVER_TYPE_UNKNOWN, nullptr, flags, levels, ARRAYSIZE(levels),
   //                        D3D11_SDK_VERSION, &device, &obtainedLevel, &context);

   // if (FAILED(hr))
   //{
   //    wstr << L"D3D11CreateDevice FAILED: 0x" << ::str::lohex(hr) << L" (" << HResultToString(hr) << L")\n\n";

   //   adapter->Release();
   //   adapterIndex++;
   //   continue;
   //}

   // wstr << L"D3D11 Device Created: YES\n";
   // wstr << L"Feature Level:       " << FeatureLevelToString(obtainedLevel) << L"\n";

   ////
   //// Enumerate outputs
   ////
   // UINT outputIndex = 0;
   // IDXGIOutput *output = nullptr;

   // while (adapter->EnumOutputs(outputIndex, &output) != DXGI_ERROR_NOT_FOUND)
   //{
   DXGI_OUTPUT_DESC outDesc{};
   output->GetDesc(&outDesc);

   wstr << L"\n";
   wstr << L"  Output #" << outputIndex << L"\n";
   wstr << L"  -----------------------------\n";

   wstr << L"  Device Name:        " << outDesc.DeviceName << L"\n";

   wstr << L"  AttachedToDesktop:  " << (outDesc.AttachedToDesktop ? L"YES" : L"NO") << L"\n";

   wstr << L"  Desktop Coordinates:" << L" (" << outDesc.DesktopCoordinates.left << L", "
        << outDesc.DesktopCoordinates.top << L") - (" << outDesc.DesktopCoordinates.right << L", "
        << outDesc.DesktopCoordinates.bottom << L")\n";

   return wstr;
}


::string dxgi_d3d11_diagnostic_for_duplicate_output4(HRESULT hr)
{

   ::string wstr;
         ////
         //// Try DuplicateOutput
         ////
         //IDXGIOutput1 *output1 = nullptr;

         //hr = output->QueryInterface(__uuidof(IDXGIOutput1), (void **)&output1);

         //if (SUCCEEDED(hr))
         //{
         //   IDXGIOutputDuplication *duplication = nullptr;

         //   hr = output1->DuplicateOutput(device, &duplication);

            if (SUCCEEDED(hr))
            {
               wstr << L"  DuplicateOutput:    SUCCESS\n";

               //duplication->Release();
            }
            else
            {
               wstr << L"  DuplicateOutput:    FAILED\n";
               wstr << L"  HRESULT:            0x" << ::str::lohex(hr) << L"\n";

               wstr << L"  Message:            " << HResultToString(hr) << L"\n";
            }

            //output1->Release();
         //}
         //else
         //{
         //   wstr << L"  IDXGIOutput1 unsupported\n";
         //}

         //output->Release();
         //outputIndex++;
      //}

      return wstr;

   //   context->Release();
   //   device->Release();

   //   adapter->Release();
   //   adapterIndex++;

   //   wstr << L"\n";
   //}

   //factory->Release();

   //wstr << L"\nDone.\n";

   //return wstr;
}

namespace remoting_windows
{


   DXGIOutputDuplication::DXGIOutputDuplication(IDXGIOutput1 * pdxgioutput1, D3D11Device *pd3d11device) ///:
       //m_outDupl(0)
   {
      information() << dxgi_d3d11_diagnostic_for_duplicate_output1();


      //retry:
      int iTry = 0;
      //auto pdxgiOutput = dxgiOutput;

      //auto pdxgiOutput1 = dxgiOutput->getDxgiOutput1();

      //auto pd3D11Device = d3D11Device;

      auto pdevice = pd3d11device->getDevice();
      HRESULT hr = pdxgioutput1->DuplicateOutput(pdevice, &m_poutputduplication);
      information() << dxgi_d3d11_diagnostic_for_duplicate_output4(hr);
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
         else if (hr == E_UNEXPECTED)
         {
            //if (iTry < 3)
            //{
            //   iTry++;
            //   preempt(2_s * iTry);
            //   goto retry;
            //}
            throw WinDxCriticalException("Can't DuplicateOutput()", hr);
         }
         else
         {
            throw WinDxCriticalException("Can't DuplicateOutput()", hr);
         }
      }
   }

   //WinDxgiOutputDuplication::WinDxgiOutputDuplication(const WinDxgiOutputDuplication &src) { copy(src); }

   DXGIOutputDuplication::~DXGIOutputDuplication()
   {
      //if (m_poutputduplication != 0)
      {
        // m_poutputduplication->Release();
         //m_poutputduplication = 0;
      }
   }


   // WinDxgiOutputDuplication &WinDxgiOutputDuplication::operator=(WinDxgiOutputDuplication const &src)
   // {
   //    copy(src);
   //    return *this;
   // }

   // void WinDxgiOutputDuplication::copy(const WinDxgiOutputDuplication &src)
   // {
   //    if (this != &src)
   //    {
   //       m_poutputduplication = src.m_poutputduplication;
   //       m_poutputduplication->AddRef();
   //    }
   // }

   IDXGIOutputDuplication *DXGIOutputDuplication::getDxgiOutputDuplication() { return m_poutputduplication; }

   size_t DXGIOutputDuplication::getFrameMoveRects(::array_base<DXGI_OUTDUPL_MOVE_RECT> *moveRects)
   {
      // Get move rectangle buffer size.
      char stub;
      ::u32 reqBufSize = 0;
      HRESULT hr;
      hr = m_poutputduplication->GetFrameMoveRects(reqBufSize, reinterpret_cast<DXGI_OUTDUPL_MOVE_RECT *>(&stub), &reqBufSize);
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
      hr = m_poutputduplication->GetFrameMoveRects(bufSize, moveRects->data(), &bufSize);
      if (FAILED(hr))
      {
         throw WinDxException("Can't get move rectanglea", hr);
      }
      return bufSize / elementSize;
   }

   size_t DXGIOutputDuplication::getFrameDirtyRects(::array_base<RECT> *dirtyRects)
   {
      // Get dirty rectangle buffer size.
      char stub;
      ::u32 reqBufSize = 0;
      HRESULT hr;
      hr = m_poutputduplication->GetFrameDirtyRects(reqBufSize, reinterpret_cast<RECT *>(&stub), &reqBufSize);
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
      hr = m_poutputduplication->GetFrameDirtyRects(bufSize, dirtyRects->data(), &bufSize);
      if (FAILED(hr))
      {
         throw WinDxException("Can't get dirty rectanglea", hr);
      }
      return bufSize / elementSize;
   }

   void DXGIOutputDuplication::getFrameCursorShape(::remoting_rfb::CursorShape *cursorShape, ::u32 pointerShapeBufferSize,
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
      hr = m_poutputduplication->GetFramePointerShape((::u32)buffer.size(), buffer.data(), &reqSize, &shapeInfo);
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

      ::remoting_rfb::CursorShape newCursorShape;
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
 


using namespace winrt;

initializeEncoder(size.Width, size.Height);

m_framePool = Direct3D11CaptureFramePool::Create(m_winrtDevice, DirectXPixelFormat::B8G8R8A8UIntNormalized, 2, size);

m_session = m_framePool.CreateCaptureSession(item);

m_framePool.FrameArrived([this](auto &pool, auto &) { onFrame(pool); });

m_session.StartCapture();
}

void onFrame(Direct3D11CaptureFramePool const &pool)
{
   auto frame = pool.TryGetNextFrame();

   auto surface = frame.Surface();

   auto access = surface.as<IDirect3DDxgiInterfaceAccess>();

   com_ptr<ID3D11Texture2D> texture;

   check_hresult(access->GetInterface(guid_of<ID3D11Texture2D>(), texture.put_void()));

   D3D11_TEXTURE2D_DESC desc{};
   texture->GetDesc(&desc);

   // Create FFmpeg frame
   AVFrame *avFrame = av_frame_alloc();

   avFrame->format = AV_PIX_FMT_D3D11;

   avFrame->width = desc.Width;
   avFrame->height = desc.Height;

   // Attach D3D11 texture directly
   avFrame->data[0] = reinterpret_cast<uint8_t *>(texture.get());

   avFrame->data[1] = reinterpret_cast<uint8_t *>(0);

   avFrame->pts = m_pts++;

   int err = avcodec_send_frame(m_codecCtx, avFrame);

   av_frame_free(&avFrame);

   if (err < 0)
      return;

   while (true)
   {
      err = avcodec_receive_packet(m_codecCtx, m_packet);

      if (err == AVERROR(EAGAIN) || err == AVERROR_EOF)
      {
         break;
      }

      if (err < 0)
         break;

      // Encoded H264 packet
      printf("Encoded packet: %d bytes\n", m_packet->size);

      // Send packet->data over network here

      av_packet_unref(m_packet);
   }
}

void shutdown()
{
   if (m_packet)
      av_packet_free(&m_packet);

   if (m_codecCtx)
      avcodec_free_context(&m_codecCtx);
}
}
;

int main() { winrt::init_apartment(); }
