// Created by camilo on 2026-05-21 15:35 <3ThomasBorregaardSørensen!!
#pragma once



#include "innate_subsystem/framebuffer/Framebuffer.h"
#include "remoting/remoting_rfb/desktop/ScreenCapture.h"
#include "remoting/remoting_windows/desktop/Win8CursorShape.h"
// #include "subsystem/thread/lockable_critical_section.h"
#include "subsystem_windows/thread/GuiThread.h"

#include "remoting/remoting_windows/_common_header.h"
#include "remoting/remoting_windows/desktop/Win8DuplicationListener.h"
// #include "log_writer/LogWriter.h"

#include "remoting/remoting_windows/desktop/DXGIOutputDuplication.h"
#include "remoting/remoting_windows/desktop/WinCustomD3D11Texture2D.h"

#include <d3d11.h>
#include <dxgi1_2.h>

#include <winrt/base.h>

#include <winrt/Windows.Foundation.h>

#include <winrt/Windows.Graphics.Capture.h>

#include <winrt/Windows.Graphics.DirectX.Direct3D11.h>
#include <winrt/Windows.Graphics.DirectX.h>

#include <windows.graphics.capture.interop.h>
#include <windows.graphics.directx.direct3d11.interop.h>


namespace remoting_windows

{


   struct BitmapData
   {
      UINT m_uWidth = 0;
      UINT m_uHeight = 0;
      UINT m_uStride = 0;

      // BGRA8 pixels
      ::memory m_memory;
   };


   class CLASS_DECL_REMOTING_WINDOWS WindowsGraphicsCapture : virtual public ::remoting_rfb::ScreenCapture
   {
   public:


      BitmapData m_bitmapdata;


      ::pointer<D3D11Device> m_pd3d11device;

      ::winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice m_d3device;


      WindowsGraphicsCapture(::innate_subsystem::Framebuffer *targetFb, ::int_rectangle_array_base &targetRect,
                          Win8CursorShape *targetCurShape, LONGLONG *cursorTimeStamp,
                          lockable_critical_section *cursorMutex, Win8DuplicationListener *duplListener,
                          D3D11Device *pd3d11device, const ::array_base<::comptr<IDXGIOutput>> &dxgioutputa,
                          ::subsystem::LogWriter *plogwriter);
      

      ~WindowsGraphicsCapture() override;

      // brga8
      virtual void on_frame(::remoting_rfb::bitmap * pbitmap);



   };


} // namespace remoting_windows

