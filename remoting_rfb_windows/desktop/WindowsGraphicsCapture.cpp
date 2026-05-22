// Created by camilo on 2026-05-21 15:35 <3ThomasBorregaardSørensen!!
#include "framework.h"
#include "WindowsGraphicsCapture.h"
#include "acme/operating_system/windows_common/com/hresult_exception.h"
#include "D3D11Device.h"
//#include <windows.h>
#include <windows.graphics.directx.direct3d11.interop.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "windowsapp.lib")

using namespace winrt;
using namespace winrt::Windows::Graphics;
using namespace winrt::Windows::Graphics::Capture;
using namespace winrt::Windows::Graphics::DirectX;
using namespace winrt::Windows::Graphics::DirectX::Direct3D11;



#include <d3d11.h>
#include <vector>
#include <stdint.h>

#pragma comment(lib, "d3d11.lib")



//
// Convert DXGI device -> WinRT IDirect3DDevice
//
IDirect3DDevice CreateDirect3DDevice(IDXGIDevice *dxgiDevice)
{
   com_ptr<::IInspectable> inspectable;

   check_hresult(CreateDirect3D11DeviceFromDXGIDevice(dxgiDevice, inspectable.put()));

   return inspectable.as<IDirect3DDevice>();
}
//
//int main()
//{
//   
//
//   //
//   // Create capture session
//   //
//   auto session = framePool.CreateCaptureSession(item);
//
//   //
//   // Receive frames
//   //
//   framePool.FrameArrived(
//      [&](auto &pool, auto &)
//      {
//         auto frame = pool.TryGetNextFrame();
//
//         auto surface = frame.Surface();
//
//         //
//         // Convert WinRT surface -> ID3D11Texture2D
//         //
//         auto access = surface.as<IDirect3DDxgiInterfaceAccess>();
//
//         com_ptr<ID3D11Texture2D> texture;
//
//         check_hresult(access->GetInterface(guid_of<ID3D11Texture2D>(), texture.put_void()));
//
//         D3D11_TEXTURE2D_DESC desc{};
//         texture->GetDesc(&desc);
//
//         printf("Frame arrived: %ux%u\n", desc.Width, desc.Height);
//
//         //
//         // Process texture here
//         //
//      });
//
//   //
//   // Start capture
//   //
//   session.StartCapture();
//
//   //printf("Capturing... press Enter to quit\n");
//
//   //getchar();
//
//   /*return 0;
//}*/

namespace remoting_windows
{


   bool CopyTextureToBitmap(ID3D11Device *device, ID3D11DeviceContext *context, ID3D11Texture2D *gpuTexture,
                            BitmapData &outBitmap)
   {
      if (!device || !context || !gpuTexture)
         return false;

      //
      // Get source texture description
      //
      D3D11_TEXTURE2D_DESC desc{};
      gpuTexture->GetDesc(&desc);

      //
      // Create staging texture
      //
      D3D11_TEXTURE2D_DESC stagingDesc = desc;

      stagingDesc.BindFlags = 0;
      stagingDesc.MiscFlags = 0;

      stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

      stagingDesc.Usage = D3D11_USAGE_STAGING;

      stagingDesc.ArraySize = 1;
      stagingDesc.MipLevels = 1;

      com_ptr<ID3D11Texture2D> stagingTexture;

      HRESULT hr = device->CreateTexture2D(&stagingDesc, nullptr, stagingTexture.put());

      if (FAILED(hr))
         return false;

      //
      // Copy GPU texture -> staging texture
      //
      context->CopyResource(stagingTexture.get(), gpuTexture);

      //
      // Map staging texture
      //
      D3D11_MAPPED_SUBRESOURCE mapped{};

      hr = context->Map(stagingTexture.get(), 0, D3D11_MAP_READ, 0, &mapped);

      if (FAILED(hr))
         return false;

      //
      // Copy rows
      //
      outBitmap.m_uWidth = desc.Width;
      outBitmap.m_uHeight = desc.Height;
      outBitmap.m_uStride = desc.Width * 4;

      outBitmap.m_memory.set_size(outBitmap.m_uStride * outBitmap.m_uHeight);

      uint8_t *dst = outBitmap.m_memory.data();
      uint8_t *src = static_cast<uint8_t *>(mapped.pData);

      for (UINT y = 0; y < desc.Height; y++)
      {
         memcpy(dst + y * outBitmap.m_uStride, src + y * mapped.RowPitch, outBitmap.m_uStride);
      }

      context->Unmap(stagingTexture.get(), 0);

      return true;
   }


   WindowsGraphicsCapture::WindowsGraphicsCapture(
      ::innate_subsystem::Framebuffer *targetFb, ::int_rectangle_array_base &targetRect,
      Win8CursorShape *targetCurShape, LONGLONG *cursorTimeStamp, lockable_critical_section *cursorMutex,
      Win8DuplicationListener *duplListener, D3D11Device *pd3d11device,
      const ::array_base<::comptr<IDXGIOutput>> &dxgioutputa, ::subsystem::LogWriter *plogwriter) :
       m_pd3d11device(pd3d11device) 
      
      // m_targetFb(targetFb), m_targetRects(targetRect), m_targetCurShape(targetCurShape),
       //m_cursorTimeStamp(cursorTimeStamp), m_cursorMutex(cursorMutex), m_duplListener(duplListener),
       //m_pdevice(pd3d11device), m_hasCriticalError(false), m_hasRecoverableError(false), m_plogwriter(plogwriter)
   {
       winrt::init_apartment();


       
   //
       // Create D3D11 device
       //
       //com_ptr<ID3D11Device> d3dDevice;
       //com_ptr<ID3D11DeviceContext> d3dContext;

       //D3D_FEATURE_LEVEL featureLevel;

       //check_hresult(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT,
         //                              nullptr, 0, D3D11_SDK_VERSION, d3dDevice.put(), &featureLevel,
           //                            d3dContext.put()));

       //
       // Convert to WinRT device
       //
       ::comptr<IDXGIDevice> pdxgidevice;
       //d3dDevice.as(dxgiDevice);

       m_pd3d11device->m_pd3d11device.as(pdxgidevice);

       m_d3device = CreateDirect3DDevice(pdxgidevice);

       //
       // Get primary monitor
       //
       HMONITOR monitor = MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY);

       //
       // Create GraphicsCaptureItem from monitor
       //
       auto interopFactory = get_activation_factory<GraphicsCaptureItem, IGraphicsCaptureItemInterop>();

       GraphicsCaptureItem item{nullptr};

       check_hresult(interopFactory->CreateForMonitor(
          monitor, guid_of<ABI::Windows::Graphics::Capture::IGraphicsCaptureItem>(), put_abi(item)));

       //
       // Create frame pool
       //
       auto framePool =
          Direct3D11CaptureFramePool::Create(m_d3device, DirectXPixelFormat::B8G8R8A8UIntNormalized, 2, item.Size());


       
   //
       // Create capture session
       //
       auto session = framePool.CreateCaptureSession(item);

       //
       // Receive frames
       //
       framePool.FrameArrived(
          [&](auto &pool, auto &)
          {
             auto frame = pool.TryGetNextFrame();

             auto surface = frame.Surface();

             //
             // Convert WinRT surface -> ID3D11Texture2D
             //
             auto access = surface.as<::Windows::Graphics::DirectX::Direct3D11::IDirect3DDxgiInterfaceAccess>();

             ::comptr<ID3D11Texture2D> ptexture;

             HRESULT hr;

             if(FAILED(hr = access->GetInterface(__interface_of(ptexture))))
             {
                throw ::hresult_exception(hr);
             } 

             

if (CopyTextureToBitmap(
        m_pd3d11device->m_pd3d11device,
        m_pd3d11device->m_pd3d11devicecontext,
        ptexture,
        m_bitmapdata))
{
            printf("Got bitmap: %ux%u\n", m_bitmapdata.m_uWidth, m_bitmapdata.m_uHeight);

                        on_frame(m_bitmapdata.m_memory.data(), m_bitmapdata.m_uWidth, m_bitmapdata.m_uHeight, m_bitmapdata.m_uStride);
}
 

             //D3D11_TEXTURE2D_DESC desc{};
             //texture->GetDesc(&desc);

             //printf("Frame arrived: %ux%u\n", desc.Width, desc.Height);

             //
             // Process texture here
             //
          });

       //
       // Start capture
       //
       session.StartCapture();
   
   }   


   WindowsGraphicsCapture::~WindowsGraphicsCapture() {}


   void WindowsGraphicsCapture::on_frame(::remoting_rfb::bitmap * pbitmap)
   {
      // Process the frame data here
      // 'pbitmap' is a pointer to the bitmap structure
      // Example: Just print the frame information
      printf("Received frame: %dx%d, stride: %d\n", pbitmap->m_uWidth, pbitmap->m_uHeight, pbitmap->m_uStride);
      // You can also copy the data to your framebuffer or process it as needed
   }



} // namespace remoting_windows
