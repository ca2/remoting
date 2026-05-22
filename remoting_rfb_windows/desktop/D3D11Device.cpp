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
#include "subsystem/platform/Exception.h"


// The header including of this cpp file must be at last place to avoid build conflicts.
#include "remoting/remoting_rfb_windows/desktop/D3D11Device.h"
   const char *FeatureLevelToString(D3D_FEATURE_LEVEL fl);


namespace remoting_rfb_windows
{


   typedef HRESULT(WINAPI *D3D11CreateDeviceFunType)(
      _In_opt_ IDXGIAdapter *pAdapter, D3D_DRIVER_TYPE DriverType, HMODULE Software, ::u32 Flags,
      _In_reads_opt_(FeatureLevels) CONST D3D_FEATURE_LEVEL *pFeatureLevels, ::u32 FeatureLevels,
      ::u32 SDKVersion, _Out_opt_ ID3D11Device **ppDevice, _Out_opt_ D3D_FEATURE_LEVEL *pFeatureLevel,
      _Out_opt_ ID3D11DeviceContext **ppImmediateContext);

   D3D11Device::D3D11Device(::subsystem::LogWriter * plogwriter) :
       //m_device(0), m_context(0),
       m_plogwriter(plogwriter)
   {
      m_d3d11Lib.initialize_dynamic_library("d3d11.dll");


        //
      // Create DXGI factory
      //
      ::comptr < IDXGIFactory1 > factory ;

      HRESULT hr = CreateDXGIFactory1(__interface_of(factory));

      if (FAILED(hr))
      {
         //information() << "CreateDXGIFactory1 failed: 0x" << str::lohex << hr << "\n";

         return;
      }

      //
      // Enumerate adapters
      //
      UINT adapterIndex = 0;
      ::comptr < IDXGIAdapter1 > adapter1;

      while (factory->EnumAdapters1(adapterIndex, &adapter1) != DXGI_ERROR_NOT_FOUND)
      {
         DXGI_ADAPTER_DESC1 desc{};
         adapter1->GetDesc1(&desc);

         information() << "=====================================\n";
         information() << "Adapter #" << adapterIndex << "\n";
         information() << "=====================================\n";

         information() << "Description:         " << ::string(desc.Description) << "\n";
         //information() << "Vendor ID:           0x" << std::hex << desc.VendorId << "\n";
         //information() << "Device ID:           0x" << std::hex << desc.DeviceId << "\n";
         //information() << "Dedicated VRAM:      " << std::dec << (desc.DedicatedVideoMemory / (1024 * 1024)) << " MB\n";

         bool isSoftware = (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) != 0;

         information() << "Software Adapter:    " << (isSoftware ? "YES" : "NO") << "\n";

         //
         // Create D3D11 device ON THIS ADAPTER
         //
         ::comptr <ID3D11Device > device;
         ::comptr<ID3D11DeviceContext > context;

         D3D_FEATURE_LEVEL obtainedLevel{};

         UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

         D3D_FEATURE_LEVEL levels[] = {D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1,
                                       D3D_FEATURE_LEVEL_10_0};

         hr = D3D11CreateDevice(adapter1, D3D_DRIVER_TYPE_UNKNOWN, nullptr, flags, levels, ARRAYSIZE(levels),
                                D3D11_SDK_VERSION, &device, &obtainedLevel, &context);

           if (FAILED(hr))
         {
            //information() << "D3D11CreateDevice FAILED: 0x" << std::hex << hr << " (" << HResultToString(hr) << ")\n\n";

            adapter1->Release();
            adapterIndex++;
            continue;
         }

         information() << "D3D11 Device Created: YES\n";
         information() << "Feature Level:       " << FeatureLevelToString(obtainedLevel) << "\n";

         //
         // Enumerate outputs
         //
         UINT outputIndex = 0;
         ::comptr < IDXGIOutput > output;

         while (adapter1->EnumOutputs(outputIndex, &output) != DXGI_ERROR_NOT_FOUND)
         {
            DXGI_OUTPUT_DESC outDesc{};
            output->GetDesc(&outDesc);

            information() << "\n";
            information() << "  Output #" << outputIndex << "\n";
            information() << "  -----------------------------\n";

            information() << "  Device Name:        " << ::string(outDesc.DeviceName) << "\n";

            information() << "  AttachedToDesktop:  " << (outDesc.AttachedToDesktop ? "YES" : "NO") << "\n";

            information() << "  Desktop Coordinates:" << " (" << outDesc.DesktopCoordinates.left << ", "
                       << outDesc.DesktopCoordinates.top << ") - (" << outDesc.DesktopCoordinates.right << ", "
                       << outDesc.DesktopCoordinates.bottom << ")\n";


            if (outDesc.AttachedToDesktop)
            {
               m_pd3d11device = device;
               m_pd3d11devicecontext = context;
               m_pdxgiadapter1 = adapter1;
               return;
            }
         }

      }





      D3D11CreateDeviceFunType d3d11CreateDevice;
      d3d11CreateDevice = (D3D11CreateDeviceFunType)m_d3d11Lib.getProcAddress("D3D11CreateDevice");
      if (d3d11CreateDevice == 0)
      {
         throw ::subsystem::Exception("Unable to load the D3D11CreateDevice() function");
      }

//      // Driver types supported
//      D3D_DRIVER_TYPE driverTypes[] = {D3D_DRIVER_TYPE_HARDWARE};
//      ::u32 driverTypeCount = ARRAYSIZE(driverTypes);
//
//      // Feature levels supported
//      D3D_FEATURE_LEVEL featureLevels[] = {D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0,
//                                           D3D_FEATURE_LEVEL_9_1};
//      ::u32 featureLevelCount = ARRAYSIZE(featureLevels);
//
//      D3D_FEATURE_LEVEL featureLevel;
//      UINT creationFlags = 0;
//      #if defined(_DEBUG)
//      // If the project is in a debug build, enable the debug layer.
//      creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
//#endif
//      // Create device
//      HRESULT hr;
//      for (::u32 iDriverType = 0; iDriverType < driverTypeCount; ++iDriverType)
//      {
//         m_plogwriter->debug("Creating of (%u) driverType device", iDriverType);
//         hr = d3d11CreateDevice(0, driverTypes[iDriverType], 0, creationFlags, featureLevels, featureLevelCount,
//                                D3D11_SDK_VERSION,
//                                &m_pd3d11device, &featureLevel, &m_pd3d11devicecontext);
//         if (SUCCEEDED(hr))
//         {
//            m_plogwriter->debugf("Creating of %u driverType device is successfull, supported D3D_FEATURE_LEVEL is %u",
//                                iDriverType, featureLevel);
//            break;
//         }
//      }
//      if (FAILED(hr))
//      {
//         ::string errMess;
//         errMess.format("D3D11CreateDevice function was failed with code error = (%dl)", (long)hr);
//         m_plogwriter->debug("D3D11CreateDevice function was failed with code error = (%dl)", (long)hr);
//         throw ::subsystem::Exception(errMess);
//      }
   }

   //WinD3D11Device::WinD3D11Device(const WinD3D11Device &src) { copy(src); }

   D3D11Device::~D3D11Device()
   {
      m_plogwriter->debug("Release ID3D11Device");
      m_pd3d11device.release();
      // if (m_device != 0)
      // {
      //    m_device->Release();
      //    m_device = 0;
      // }
      m_plogwriter->debug("Release ID3D11DeviceContext");
      m_pd3d11devicecontext.release();
      // if (m_context != 0)
      // {
      //    m_context->Release();
      //    m_context = 0;
      // }
   }

   // WinD3D11Device &WinD3D11Device::operator=(WinD3D11Device const &src)
   // {
   //    copy(src);
   //    return *this;
   // }
   //
   // void WinD3D11Device::copy(const WinD3D11Device &src)
   // {
   //    if (this != &src)
   //    {
   //       m_pd3d11device = src.m_pd3d11device;
   //       //m_device->AddRef();
   //       m_pd3d11devicecontext = src.m_pd3d11devicecontext;
   //       //m_context->AddRef();
   //    }
   // }

   HRESULT D3D11Device::deviceQueryInterface(REFIID riid, void **ppvObject)
   {
      return m_pd3d11device->QueryInterface(riid, ppvObject);
   }

   HRESULT D3D11Device::contextQueryInterface(REFIID riid, void **ppvObject)
   {
      return m_pd3d11devicecontext->QueryInterface(riid, ppvObject);
   }

   ID3D11Device *D3D11Device::getDevice() { return m_pd3d11device; }

   ID3D11DeviceContext *D3D11Device::getContext() { return m_pd3d11devicecontext; }

   void D3D11Device::copySubresourceRegion(ID3D11Texture2D *dstTexture2D, int dstX, int dstY,
                                              ID3D11Texture2D *srcTexture2D, const ::i32_rectangle &rectangleSource,
                                              ::u32 front, ::u32 back)
   {
      D3D11_BOX box;
      box.left = rectangleSource.left;
      box.top = rectangleSource.top;
      box.right = rectangleSource.right;
      box.bottom = rectangleSource.bottom;
      box.front = front;
      box.back = back;
      m_pd3d11devicecontext->CopySubresourceRegion(dstTexture2D, 0, dstX, dstY, 0, srcTexture2D, 0, &box);
   }


} // namespace remoting_rfb_windows
 