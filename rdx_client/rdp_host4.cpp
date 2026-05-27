//
// Created by camilo on 5/24/2026.
//
#include "framework.h"
#include "main_rdp_host.h"



// ============================================================================
// Pure Win32 ActiveX Container for Microsoft RDP Client
// No ATL / No MFC / No WTL
//
// File: MinimalRdpContainer.cpp
//
// Build:
//   cl /EHsc /DUNICODE /D_UNICODE MinimalRdpContainer.cpp \
//      ole32.lib oleaut32.lib user32.lib gdi32.lib
//
// ============================================================================

// #define UNICODE
// #define _UNICODE
//
// #include <windows.h>
#include <oleidl.h>
#include <ocidl.h>
#include <olectl.h>
#include <comdef.h>

#import "mstscax.dll" \
    raw_interfaces_only \
    raw_native_types \
    no_namespace \
    named_guids

#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")
#include "acme/operating_system/windows/window.h"
#include "com_base.h"
// #include "in_place_frame.h"
// #include "in_place_site.h"
// #include "client_site.h"
CLASS_DECL_ACME void set_main_wndproc(void *p);
CLASS_DECL_ACME LRESULT CALLBACK RDP_HOST2_WndProc(
    HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam);

namespace rdp_host4
{

// ============================================================================
// Globals
// ============================================================================

HWND g_hwndMain = nullptr;

IOleObject*           g_oleObject      = nullptr;
IOleInPlaceObject*    g_inPlaceObject  = nullptr;
IMsRdpClient9*        g_rdpClient      = nullptr;


   class in_place_frame :
   public IOleInPlaceFrame,
   virtual public ::windows::com_base
   {
   public:


      HWND m_hwnd;
      ::comptr < IOleInPlaceActiveObject > m_pinplaceactiveobject;
      ::comptr < IOleInPlaceObject > m_pinplaceobject;


      øDECL_INTERFACE_MAP();

      HRESULT STDMETHODCALLTYPE GetWindow(HWND* phwnd) override;
      HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(BOOL) override;
      HRESULT STDMETHODCALLTYPE GetBorder(LPRECT) override;
      HRESULT STDMETHODCALLTYPE RequestBorderSpace(LPCBORDERWIDTHS) override;
      HRESULT STDMETHODCALLTYPE SetBorderSpace(LPCBORDERWIDTHS) override;
      HRESULT STDMETHODCALLTYPE SetActiveObject(IOleInPlaceActiveObject*, LPCOLESTR) override;
      HRESULT STDMETHODCALLTYPE InsertMenus(HMENU, LPOLEMENUGROUPWIDTHS) override;
      HRESULT STDMETHODCALLTYPE SetMenu(HMENU, HOLEMENU, HWND) override;
      HRESULT STDMETHODCALLTYPE RemoveMenus(HMENU) override;
      HRESULT STDMETHODCALLTYPE SetStatusText(LPCOLESTR) override;
      HRESULT STDMETHODCALLTYPE EnableModeless(BOOL) override;
      HRESULT STDMETHODCALLTYPE TranslateAccelerator(LPMSG, WORD) override;

   };



   class in_place_site :
       public IOleInPlaceSite,
       virtual public ::windows::com_base
   {
   public:

      ::pointer < in_place_frame > m_pinplaceframe;


      øDECL_INTERFACE_MAP();

      in_place_site();
      ~in_place_site();

      HRESULT STDMETHODCALLTYPE GetWindow(HWND* phwnd) override;
      HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(BOOL) override;
      HRESULT STDMETHODCALLTYPE CanInPlaceActivate() override;
      HRESULT STDMETHODCALLTYPE OnInPlaceActivate() override;
      HRESULT STDMETHODCALLTYPE OnUIActivate() override;
      HRESULT STDMETHODCALLTYPE GetWindowContext(
          IOleInPlaceFrame** ppFrame,
          IOleInPlaceUIWindow** ppDoc,
          LPRECT lprcPosRect,
          LPRECT lprcClipRect,
          LPOLEINPLACEFRAMEINFO lpFrameInfo);
      HRESULT STDMETHODCALLTYPE Scroll(SIZE) override;
      HRESULT STDMETHODCALLTYPE OnUIDeactivate(BOOL) override;
      HRESULT STDMETHODCALLTYPE OnInPlaceDeactivate() override;
      HRESULT STDMETHODCALLTYPE DiscardUndoState() override;
      HRESULT STDMETHODCALLTYPE DeactivateAndUndo() override;
      HRESULT STDMETHODCALLTYPE OnPosRectChange(
          LPCRECT lprcPosRect) override;
   };



   class CLASS_DECL_REMOTING_RDX_CLIENT client_site :
      public IOleClientSite,
      virtual public ::windows::com_base
   {
      //ULONG refCount = 1;

   public:


      ::pointer < in_place_site > m_pinplacesite;
//      ::pointer < com_window > m_pcomwindow;

      //øDECL_INTERFACE_MAP();


      client_site();
      ~client_site() override;


      virtual void  initialize_client_site(HWND hwnd);



      HRESULT STDMETHODCALLTYPE QueryInterface(
 REFIID riid,
 void** ppvObject) override;
      ULONG STDMETHODCALLTYPE AddRef() { return InternalAddRef(); }
      ULONG STDMETHODCALLTYPE Release() { return InternalRelease(); }


      // IOleClientSite
      HRESULT STDMETHODCALLTYPE SaveObject();
      HRESULT STDMETHODCALLTYPE GetMoniker(DWORD, DWORD, IMoniker **);
      HRESULT STDMETHODCALLTYPE GetContainer(IOleContainer **ppContainer);
      HRESULT STDMETHODCALLTYPE ShowObject();
      HRESULT STDMETHODCALLTYPE OnShowWindow(BOOL);
      HRESULT STDMETHODCALLTYPE RequestNewObjectLayout();




   };




   øBEGIN_INTERFACE_MAP(in_place_frame, IOleInPlaceFrame)
   øINTERFACE_MAP_ITEM(IOleInPlaceFrame)
   øINTERFACE_MAP_ITEM(IOleInPlaceUIWindow)
   øINTERFACE_MAP_ITEM(IOleWindow)
   øEND_INTERFACE_MAP()


   HRESULT STDMETHODCALLTYPE in_place_frame::GetWindow(HWND* phwnd)
   {

      auto hwnd = m_hwnd;
      *phwnd = hwnd;
      return S_OK;
   }

   HRESULT STDMETHODCALLTYPE in_place_frame::ContextSensitiveHelp(BOOL)
   {
      return E_NOTIMPL;
   }

   HRESULT STDMETHODCALLTYPE in_place_frame::GetBorder(LPRECT)
   {
      return INPLACE_E_NOTOOLSPACE;
   }

   HRESULT STDMETHODCALLTYPE in_place_frame::RequestBorderSpace(LPCBORDERWIDTHS)
   {
      return INPLACE_E_NOTOOLSPACE;
   }

   HRESULT STDMETHODCALLTYPE in_place_frame::SetBorderSpace(LPCBORDERWIDTHS)
   {
      return S_OK;
   }

   HRESULT STDMETHODCALLTYPE in_place_frame::SetActiveObject(
       IOleInPlaceActiveObject*pinplaceactiveobject,
       LPCOLESTR)
   {
      m_pinplaceactiveobject = pinplaceactiveobject;
      return S_OK;
   }

   HRESULT STDMETHODCALLTYPE in_place_frame::InsertMenus(
       HMENU,
       LPOLEMENUGROUPWIDTHS)
   {
      return E_NOTIMPL;
   }

   HRESULT STDMETHODCALLTYPE in_place_frame::SetMenu(
       HMENU,
       HOLEMENU,
       HWND)
   {
      return S_OK;
   }

   HRESULT STDMETHODCALLTYPE in_place_frame::RemoveMenus(HMENU)
   {
      return S_OK;
   }

   HRESULT STDMETHODCALLTYPE in_place_frame::SetStatusText(LPCOLESTR)
   {
      return S_OK;
   }

   HRESULT STDMETHODCALLTYPE in_place_frame::EnableModeless(BOOL)
   {
      return S_OK;
   }

   HRESULT STDMETHODCALLTYPE in_place_frame::TranslateAccelerator(
       LPMSG,
       WORD)
   {
      return E_NOTIMPL;
   }



   øBEGIN_INTERFACE_MAP(in_place_site, IOleInPlaceSite)
   øINTERFACE_MAP_ITEM(IOleInPlaceSite)
   øINTERFACE_MAP_ITEM(IOleWindow)
   øEND_INTERFACE_MAP()


   in_place_site::in_place_site()
   {

         construct_newø(m_pinplaceframe);

   }


   in_place_site::~in_place_site()
   {

   }


   HRESULT STDMETHODCALLTYPE in_place_site::GetWindow(HWND* phwnd)
   {
      auto hwnd = m_pinplaceframe->m_hwnd;
      *phwnd = hwnd;
      return S_OK;
   }

   HRESULT STDMETHODCALLTYPE in_place_site::ContextSensitiveHelp(BOOL)
   {
      return E_NOTIMPL;
   }

   HRESULT STDMETHODCALLTYPE in_place_site::CanInPlaceActivate()
   {
      return S_OK;
   }

   HRESULT STDMETHODCALLTYPE in_place_site::OnInPlaceActivate()
   {
      return S_OK;
   }

   HRESULT STDMETHODCALLTYPE in_place_site::OnUIActivate()
   {
      return S_OK;
   }

   HRESULT STDMETHODCALLTYPE in_place_site::GetWindowContext(
       IOleInPlaceFrame** ppFrame,
       IOleInPlaceUIWindow** ppDoc,
       LPRECT lprcPosRect,
       LPRECT lprcClipRect,
       LPOLEINPLACEFRAMEINFO lpFrameInfo)
   {

      auto hwndMain = m_pinplaceframe->m_hwnd;
      RECT rc;
      GetClientRect(hwndMain, &rc);

      *ppFrame = m_pinplaceframe;
      (*ppFrame)->AddRef();

      *ppDoc = nullptr;

      *lprcPosRect = rc;
      *lprcClipRect = rc;

      lpFrameInfo->fMDIApp = FALSE;
      lpFrameInfo->hwndFrame = hwndMain;
      lpFrameInfo->haccel = nullptr;
      lpFrameInfo->cAccelEntries = 0;

      return S_OK;
   }

   HRESULT STDMETHODCALLTYPE in_place_site::Scroll(SIZE)
   {
      return E_NOTIMPL;
   }

   HRESULT STDMETHODCALLTYPE in_place_site::OnUIDeactivate(BOOL)
   {
      return S_OK;
   }

   HRESULT STDMETHODCALLTYPE in_place_site::OnInPlaceDeactivate()
   {
      return S_OK;
   }

   HRESULT STDMETHODCALLTYPE in_place_site::DiscardUndoState()
   {
      return E_NOTIMPL;
   }

   HRESULT STDMETHODCALLTYPE in_place_site::DeactivateAndUndo()
   {
      return E_NOTIMPL;
   }

   HRESULT STDMETHODCALLTYPE in_place_site::OnPosRectChange(
       LPCRECT lprcPosRect)
   {
      if (m_pinplaceframe->m_pinplaceobject)
      {
         m_pinplaceframe->m_pinplaceobject->SetObjectRects(
             lprcPosRect,
             lprcPosRect);
      }

      return S_OK;
   }


      //
   // øBEGIN_INTERFACE_MAP(client_site, IOleClientSite)
   // øINTERFACE_MAP_ITEM(IOleClientSite)
   // øINTERFACE_MAP_ITEM(IOleInPlaceSite)
   // øINTERFACE_MAP_ITEM(IOleInPlaceFrame)
   // øINTERFACE_MAP_ITEM(IOleInPlaceSiteEx)
   // øINTERFACE_MAP_ITEM(IOleInPlaceSiteWindowless)
   // øEND_INTERFACE_MAP()


   client_site::client_site() { }

   client_site::~client_site() {}


   void client_site::initialize_client_site(HWND hwnd)
   {

      construct_newø(m_pinplacesite);
      m_pinplacesite->m_pinplaceframe->m_hwnd = hwnd;

   }



   HRESULT STDMETHODCALLTYPE client_site::QueryInterface(
    REFIID riid,
    void** ppvObject)
   {
      if (!ppvObject)
         return E_POINTER;

      *ppvObject = nullptr;

      if (riid == IID_IUnknown ||
          riid == IID_IOleClientSite)
      {
         *ppvObject =
             static_cast<IOleClientSite*>(this);

         AddRef();
         return S_OK;
      }

      if (riid == IID_IOleInPlaceSite)
      {
         return m_pinplacesite->QueryInterface(
             riid,
             ppvObject);
      }

      return E_NOINTERFACE;
   }


   // IOleClientSite::SaveObject
   HRESULT STDMETHODCALLTYPE client_site::SaveObject() { return E_NOTIMPL; }
   // IOleClientSite::GetMoniker
   HRESULT STDMETHODCALLTYPE client_site::GetMoniker(DWORD, DWORD, IMoniker **) { return E_NOTIMPL; }
   // IOleClientSite::GetContainer
   HRESULT STDMETHODCALLTYPE client_site::GetContainer(IOleContainer **ppContainer)
   {
      *ppContainer = nullptr;
      return E_NOINTERFACE;
   }
   // IOleClientSite::ShowObject
   HRESULT STDMETHODCALLTYPE client_site::ShowObject() { return S_OK; }
   // IOleClientSite::OnShowWindow
   HRESULT STDMETHODCALLTYPE client_site::OnShowWindow(BOOL) { return S_OK; }
   // IOleClientSite::RequestNewObjectLayout
   HRESULT STDMETHODCALLTYPE client_site::RequestNewObjectLayout() { return E_NOTIMPL; }

   //
   //
   // // IOleWindow::GetWindow
   // HRESULT STDMETHODCALLTYPE client_site::GetWindow(HWND *phwnd)
   // {
   //
   //    auto hwnd = ::as_HWND(m_pcomwindow->operating_system_window());
   //    *phwnd = hwnd;
   //    return S_OK;
   // }
   //
   // // IOleWindow::ContextSensitiveHelp
   // HRESULT STDMETHODCALLTYPE client_site::ContextSensitiveHelp(BOOL) { return E_NOTIMPL; }
   //
   //
   // // IOleInPlaceSite::CanInPlaceActivate
   // HRESULT STDMETHODCALLTYPE client_site:: CanInPlaceActivate() { return S_OK; }
   // // IOleInPlaceSite::OnInPlaceActivate
   // HRESULT STDMETHODCALLTYPE client_site:: OnInPlaceActivate() { return S_OK; }
   // // IOleInPlaceSite::OnUIActivate
   // HRESULT STDMETHODCALLTYPE client_site::OnUIActivate() { return S_OK; }
   //
   //
   // // IOleInPlaceSite::GetWindowContext
   // HRESULT STDMETHODCALLTYPE client_site::GetWindowContext(IOleInPlaceFrame **ppFrame, IOleInPlaceUIWindow **ppDoc,
   //                                         LPRECT lprcPosRect, LPRECT lprcClipRect, OLEINPLACEFRAMEINFO *lpFrameInfo)
   // {
   //    *ppFrame = m_pinplacesite->m_pinplaceframe;
   //    AddRef();
   //
   //    *ppDoc = nullptr;
   //
   //    auto hwnd = ::as_HWND(m_pcomwindow->operating_system_window());
   //
   //    GetClientRect(hwnd, lprcPosRect);
   //    GetClientRect(hwnd, lprcClipRect);
   //
   //    lpFrameInfo->fMDIApp = FALSE;
   //    lpFrameInfo->hwndFrame = hwnd;
   //    lpFrameInfo->haccel = nullptr;
   //    lpFrameInfo->cAccelEntries = 0;
   //
   //    return S_OK;
   // }
   //
   // // IOleInPlaceSite::Scroll
   // HRESULT STDMETHODCALLTYPE client_site::Scroll(SIZE) { return E_NOTIMPL; }
   // // IOleInPlaceSite::OnUIDeactivate
   // HRESULT STDMETHODCALLTYPE client_site::OnUIDeactivate(BOOL) { return S_OK; }
   // // IOleInPlaceSite::OnInPlaceDeactivate
   // HRESULT STDMETHODCALLTYPE client_site::OnInPlaceDeactivate() { return S_OK; }
   // // IOleInPlaceSite::DiscardUndoState
   // HRESULT STDMETHODCALLTYPE client_site::DiscardUndoState() { return E_NOTIMPL; }
   // // IOleInPlaceSite::DeactivateAndUndo
   // HRESULT STDMETHODCALLTYPE client_site::DeactivateAndUndo() { return E_NOTIMPL; }
   //
   //
   // // IOleInPlaceSite::OnPosRectChange
   // HRESULT STDMETHODCALLTYPE client_site::OnPosRectChange(LPCRECT prcPosRect)
   // {
   //
   //    ::comptr <IOleInPlaceObject > pinplaceobject;
   //
   //    if (SUCCEEDED(m_pcomwindow->m_poleobject.as(pinplaceobject)))
   //    {
   //
   //       pinplaceobject->SetObjectRects(prcPosRect, prcPosRect);
   //
   //    }
   //
   //    return S_OK;
   //
   // }


   //
   // // IOleInPlaceUIWindow / Frame::GetBorder
   // HRESULT STDMETHODCALLTYPE client_site::GetBorder(LPRECT) { return E_NOTIMPL; }
   // // IOleInPlaceUIWindow / Frame::RequestBorderSpace
   // HRESULT STDMETHODCALLTYPE client_site::RequestBorderSpace(LPCBORDERWIDTHS) { return E_NOTIMPL; }
   // // IOleInPlaceUIWindow / Frame::SetBorderSpace
   // HRESULT STDMETHODCALLTYPE client_site::SetBorderSpace(LPCBORDERWIDTHS) { return S_OK; }
   // // IOleInPlaceUIWindow / Frame::SetActiveObject
   // HRESULT STDMETHODCALLTYPE client_site::SetActiveObject(IOleInPlaceActiveObject *pinplaceactiveobject, LPCOLESTR)
   // {
   //    m_pinplacesite->m_pinplaceframe->m_pinplaceactiveobject = pinplaceactiveobject;
   //    return S_OK;
   // }
   // // IOleInPlaceUIWindow / Frame::InsertMenus
   // HRESULT STDMETHODCALLTYPE client_site::InsertMenus(HMENU, LPOLEMENUGROUPWIDTHS) { return E_NOTIMPL; }
   // // IOleInPlaceUIWindow / Frame::SetMenu
   // HRESULT STDMETHODCALLTYPE client_site::SetMenu(HMENU, HOLEMENU, HWND) { return S_OK; }
   // // IOleInPlaceUIWindow / Frame::RemoveMenus
   // HRESULT STDMETHODCALLTYPE client_site::RemoveMenus(HMENU) { return S_OK; }
   // // IOleInPlaceUIWindow / Frame::SetStatusText
   // HRESULT STDMETHODCALLTYPE client_site::SetStatusText(LPCOLESTR) { return S_OK; }
   // // IOleInPlaceUIWindow / Frame::EnableModeless
   // HRESULT STDMETHODCALLTYPE client_site::EnableModeless(BOOL) { return S_OK; }
   // // IOleInPlaceUIWindow / Frame::TranslateAccelerator
   // HRESULT STDMETHODCALLTYPE client_site::TranslateAccelerator(LPMSG, WORD) { return E_NOTIMPL; }
   //
   //
   // // IOleInPlaceSiteEx::OnInPlaceActivateEx
   // HRESULT STDMETHODCALLTYPE client_site::OnInPlaceActivateEx(
   //  BOOL* pfNoRedraw,
   //  DWORD)
   // {
   //    if (pfNoRedraw)
   //       *pfNoRedraw = FALSE;
   //
   //    return S_OK;
   // }
   //
   // // IOleInPlaceSiteEx::OnInPlaceDeactivateEx
   // HRESULT STDMETHODCALLTYPE client_site::OnInPlaceDeactivateEx(BOOL)
   // {
   //    return S_OK;
   // }
   //
   // // IOleInPlaceSiteEx::RequestUIActivate
   // HRESULT STDMETHODCALLTYPE client_site::RequestUIActivate()
   // {
   //    m_pcomwindow->window_set_focus();
   //    return S_OK;
   // }
   //
   //
   // // IOleInPlaceSiteWindowless::CanWindowlessActivate
   // HRESULT STDMETHODCALLTYPE client_site::CanWindowlessActivate()
   // {
   //    return S_FALSE;
   // }
   //
   //
   // HRESULT STDMETHODCALLTYPE client_site::GetCapture() { return S_FALSE; }
   //
   // HRESULT STDMETHODCALLTYPE client_site::SetCapture(BOOL)
   // {
   //    return S_OK;
   // }
   //
   // HRESULT STDMETHODCALLTYPE client_site::GetFocus()
   // {
   //    return S_FALSE;
   // }
   //
   // HRESULT STDMETHODCALLTYPE client_site::SetFocus(BOOL)
   // {
   //    return S_OK;
   // }
   //
   // HRESULT STDMETHODCALLTYPE client_site::GetDC(
   //     LPCRECT,
   //     DWORD,
   //     HDC*)
   // {
   //    return E_NOTIMPL;
   // }
   //
   // HRESULT STDMETHODCALLTYPE client_site::ReleaseDC(HDC)
   // {
   //    return E_NOTIMPL;
   // }
   //
   // HRESULT STDMETHODCALLTYPE client_site::InvalidateRect(
   //     LPCRECT,
   //     BOOL)
   // {
   //    m_pcomwindow->window_invalidate_rect(nullptr, true);
   //    return S_OK;
   // }
   //
   // HRESULT STDMETHODCALLTYPE client_site::InvalidateRgn(
   //     HRGN,
   //     BOOL)
   // {
   //    m_pcomwindow->window_invalidate_rect(nullptr, true);
   //    return S_OK;
   // }
   //
   // HRESULT STDMETHODCALLTYPE client_site::ScrollRect(
   //     INT,
   //     INT,
   //     LPCRECT,
   //     LPCRECT)
   // {
   //    return E_NOTIMPL;
   // }
   //
   // HRESULT STDMETHODCALLTYPE client_site::AdjustRect(LPRECT)
   // {
   //    return S_OK;
   // }
   //
   // HRESULT STDMETHODCALLTYPE client_site::OnDefWindowMessage(
   //     UINT,
   //     WPARAM,
   //     LPARAM,
   //     LRESULT*)
   // {
   //    return S_FALSE;
   // }

//
// // ============================================================================
// // Basic COM RefCount Helper
// // ============================================================================
//
// class ComBase :
// virtual public ::particle
// {
// protected:
//     //ULONG m_ref;
//
// public:
//     ComBase()
//         //: m_ref(1)
//     {
//     }
//
//     virtual ~ComBase()
//     {
//     }
//
//     ULONG InternalAddRef()
//     {
//         return this->increment_reference_count();
//     }
//
//     ULONG InternalRelease()
//     {
//         return this->release();
//
//     }
// };
//
// // ============================================================================
// // IOleInPlaceFrame
// // ============================================================================
//
// class OleInPlaceFrame :
//     public IOleInPlaceFrame,
//     public ComBase
// {
// public:
//
//
//    øDECL_INTERFACE_MAP()
//
//     // HRESULT STDMETHODCALLTYPE QueryInterface(
//     //     REFIID riid,
//     //     void** ppvObject) override
//     // {
//     //     if (!ppvObject)
//     //         return E_POINTER;
//     //
//     //     *ppvObject = nullptr;
//     //
//     //     if (riid == IID_IUnknown ||
//     //         riid == IID_IOleWindow ||
//     //         riid == IID_IOleInPlaceUIWindow ||
//     //         riid == IID_IOleInPlaceFrame)
//     //     {
//     //         *ppvObject =
//     //             static_cast<IOleInPlaceFrame*>(this);
//     //
//     //         AddRef();
//     //         return S_OK;
//     //     }
//     //
//     //     return E_NOINTERFACE;
//     // }
//     //
//     // ULONG STDMETHODCALLTYPE AddRef() override
//     // {
//     //     return InternalAddRef();
//     // }
//     //
//     // ULONG STDMETHODCALLTYPE Release() override
//     // {
//     //     return InternalRelease();
//     // }
//
//     HRESULT STDMETHODCALLTYPE GetWindow(HWND* phwnd) override
//     {
//         *phwnd = g_hwndMain;
//         return S_OK;
//     }
//
//     HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(BOOL) override
//     {
//         return E_NOTIMPL;
//     }
//
//     HRESULT STDMETHODCALLTYPE GetBorder(LPRECT) override
//     {
//         return INPLACE_E_NOTOOLSPACE;
//     }
//
//     HRESULT STDMETHODCALLTYPE RequestBorderSpace(LPCBORDERWIDTHS) override
//     {
//         return INPLACE_E_NOTOOLSPACE;
//     }
//
//     HRESULT STDMETHODCALLTYPE SetBorderSpace(LPCBORDERWIDTHS) override
//     {
//         return S_OK;
//     }
//
//     HRESULT STDMETHODCALLTYPE SetActiveObject(
//         IOleInPlaceActiveObject*,
//         LPCOLESTR) override
//     {
//         return S_OK;
//     }
//
//     HRESULT STDMETHODCALLTYPE InsertMenus(
//         HMENU,
//         LPOLEMENUGROUPWIDTHS) override
//     {
//         return E_NOTIMPL;
//     }
//
//     HRESULT STDMETHODCALLTYPE SetMenu(
//         HMENU,
//         HOLEMENU,
//         HWND) override
//     {
//         return S_OK;
//     }
//
//     HRESULT STDMETHODCALLTYPE RemoveMenus(HMENU) override
//     {
//         return S_OK;
//     }
//
//     HRESULT STDMETHODCALLTYPE SetStatusText(LPCOLESTR) override
//     {
//         return S_OK;
//     }
//
//     HRESULT STDMETHODCALLTYPE EnableModeless(BOOL) override
//     {
//         return S_OK;
//     }
//
//     HRESULT STDMETHODCALLTYPE TranslateAccelerator(
//         LPMSG,
//         WORD) override
//     {
//         return E_NOTIMPL;
//     }
// };
//
//    øBEGIN_INTERFACE_MAP(OleInPlaceFrame, IOleInPlaceFrame)
// øINTERFACE_MAP_ITEM(IOleInPlaceFrame)
// øINTERFACE_MAP_ITEM(IOleInPlaceUIWindow)
// øINTERFACE_MAP_ITEM(IOleWindow)
// øEND_INTERFACE_MAP()
//
//
//
// // ============================================================================
// // IOleInPlaceSite
// // ============================================================================
//
// class OleInPlaceSite :
//    public IOleInPlaceSite,
//    public ComBase
// {
// public:
//
//
//    ::pointer < OleInPlaceFrame >  m_poleinplaceframe;
//
//       OleInPlaceSite()
//       {
//
//          construct_newø(m_poleinplaceframe);
//
//       }
//
//
//       ~OleInPlaceSite() override
//       {
//
//
//       }
//
//
//    HRESULT STDMETHODCALLTYPE QueryInterface(
//       REFIID riid,
//       void** ppvObject) override
//    {
//       if (!ppvObject)
//          return E_POINTER;
//
//       *ppvObject = nullptr;
//
//       if (riid == IID_IUnknown ||
//           riid == IID_IOleWindow ||
//           riid == IID_IOleInPlaceSite)
//       {
//          *ppvObject =
//             static_cast<IOleInPlaceSite *>(this);
//
//          AddRef();
//          return S_OK;
//       }
//
//       return E_NOINTERFACE;
//    }
//
//
//    ULONG STDMETHODCALLTYPE AddRef() override
//    {
//       return InternalAddRef();
//    }
//
//
//    ULONG STDMETHODCALLTYPE Release() override
//    {
//       return InternalRelease();
//    }
//
//
//    HRESULT STDMETHODCALLTYPE GetWindow(HWND* phwnd) override
//    {
//       *phwnd = g_hwndMain;
//       return S_OK;
//    }
//
//
//    HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(BOOL) override
//    {
//       return E_NOTIMPL;
//    }
//
//
//    HRESULT STDMETHODCALLTYPE CanInPlaceActivate() override
//    {
//       return S_OK;
//    }
//
//
//    HRESULT STDMETHODCALLTYPE OnInPlaceActivate() override
//    {
//       return S_OK;
//    }
//
//
//    HRESULT STDMETHODCALLTYPE OnUIActivate() override
//    {
//       return S_OK;
//    }
//
//
//    HRESULT STDMETHODCALLTYPE GetWindowContext(
//       IOleInPlaceFrame** ppFrame,
//       IOleInPlaceUIWindow** ppDoc,
//       LPRECT lprcPosRect,
//       LPRECT lprcClipRect,
//       LPOLEINPLACEFRAMEINFO lpFrameInfo) override
//    {
//       RECT rc;
//       GetClientRect(g_hwndMain, &rc);
//
//       *ppFrame = m_poleinplaceframe;
//       (*ppFrame)->AddRef();
//
//       *ppDoc = nullptr;
//
//       *lprcPosRect = rc;
//       *lprcClipRect = rc;
//
//       lpFrameInfo->fMDIApp = FALSE;
//       lpFrameInfo->hwndFrame = g_hwndMain;
//       lpFrameInfo->haccel = nullptr;
//       lpFrameInfo->cAccelEntries = 0;
//
//       return S_OK;
//    }
//
//
//    HRESULT STDMETHODCALLTYPE Scroll(SIZE) override
//    {
//       return E_NOTIMPL;
//    }
//
//
//    HRESULT STDMETHODCALLTYPE OnUIDeactivate(BOOL) override
//    {
//       return S_OK;
//    }
//
//
//    HRESULT STDMETHODCALLTYPE OnInPlaceDeactivate() override
//    {
//       return S_OK;
//    }
//
//
//    HRESULT STDMETHODCALLTYPE DiscardUndoState() override
//    {
//       return E_NOTIMPL;
//    }
//
//
//    HRESULT STDMETHODCALLTYPE DeactivateAndUndo() override
//    {
//       return E_NOTIMPL;
//    }
//
//
//    HRESULT STDMETHODCALLTYPE OnPosRectChange(
//       LPCRECT lprcPosRect) override
//    {
//       if (g_inPlaceObject)
//       {
//          g_inPlaceObject->SetObjectRects(
//             lprcPosRect,
//             lprcPosRect);
//       }
//
//       return S_OK;
//    }
// };
//
// // ============================================================================
// // IOleClientSite
// // ============================================================================
//
// class OleClientSite :
//     public IOleClientSite,
//     public ComBase
// {
// public:
//
//     OleInPlaceSite m_inPlaceSite;
//
//     HRESULT STDMETHODCALLTYPE QueryInterface(
//         REFIID riid,
//         void** ppvObject) override
//     {
//         if (!ppvObject)
//             return E_POINTER;
//
//         *ppvObject = nullptr;
//
//         if (riid == IID_IUnknown ||
//             riid == IID_IOleClientSite)
//         {
//             *ppvObject =
//                 static_cast<IOleClientSite*>(this);
//
//             AddRef();
//             return S_OK;
//         }
//
//         if (riid == IID_IOleInPlaceSite)
//         {
//             return m_inPlaceSite.QueryInterface(
//                 riid,
//                 ppvObject);
//         }
//
//         return E_NOINTERFACE;
//     }
//
//     ULONG STDMETHODCALLTYPE AddRef() override
//     {
//         return InternalAddRef();
//     }
//
//     ULONG STDMETHODCALLTYPE Release() override
//     {
//         return InternalRelease();
//     }
//
//     HRESULT STDMETHODCALLTYPE SaveObject() override
//     {
//         return E_NOTIMPL;
//     }
//
//     HRESULT STDMETHODCALLTYPE GetMoniker(
//         DWORD,
//         DWORD,
//         IMoniker**) override
//     {
//         return E_NOTIMPL;
//     }
//
//     HRESULT STDMETHODCALLTYPE GetContainer(
//         IOleContainer** ppContainer) override
//     {
//         *ppContainer = nullptr;
//         return E_NOINTERFACE;
//     }
//
//     HRESULT STDMETHODCALLTYPE ShowObject() override
//     {
//         return S_OK;
//     }
//
//     HRESULT STDMETHODCALLTYPE OnShowWindow(BOOL) override
//     {
//         return S_OK;
//     }
//
//     HRESULT STDMETHODCALLTYPE RequestNewObjectLayout() override
//     {
//         return E_NOTIMPL;
//     }
// };
//
// OleClientSite g_clientSite;

   ::pointer < client_site > g_pclientsite;

// ============================================================================
// Resize Helper
// ============================================================================

void ResizeActiveX()
{
    if (!g_inPlaceObject)
        return;

    RECT rc;
    GetClientRect(g_hwndMain, &rc);

    g_inPlaceObject->SetObjectRects(&rc, &rc);
}

// ============================================================================
// Create RDP ActiveX Control
// ============================================================================

HRESULT CreateRdpControl(HWND hwnd)
{
    HRESULT hr;

    hr = CoCreateInstance(
        CLSID_MsRdpClient9NotSafeForScripting,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_IOleObject,
        (void**)&g_oleObject);

    if (FAILED(hr))
        return hr;

   ::system()->construct_newø(g_pclientsite);

   g_pclientsite->initialize_client_site(hwnd);

    hr = g_oleObject->SetClientSite(g_pclientsite);

    if (FAILED(hr))
        return hr;

    OleSetContainedObject(g_oleObject, TRUE);

    RECT rc;
    GetClientRect(hwnd, &rc);

    hr = g_oleObject->DoVerb(
        OLEIVERB_SHOW,
        nullptr,
        g_pclientsite,
        0,
        hwnd,
        &rc);

    if (FAILED(hr))
        return hr;

    hr = g_oleObject->QueryInterface(
        IID_IOleInPlaceObject,
        (void**)&g_inPlaceObject);

    if (FAILED(hr))
        return hr;

    hr = g_oleObject->QueryInterface(
        __uuidof(IMsRdpClient9),
        (void**)&g_rdpClient);

    if (FAILED(hr))
        return hr;

    return S_OK;
}

// ============================================================================
// Connect
// ============================================================================

void ConnectRdp()
{
    if (!g_rdpClient)
        return;

    g_rdpClient->put_Server(_bstr_t(L"canada5.camilothomas.com"));
    //g_rdpClient->put_UserName(_bstr_t(L"Administrator"));

    IMsRdpClientAdvancedSettings7* settings = nullptr;

    if (SUCCEEDED(
        g_rdpClient->get_AdvancedSettings8(&settings)))
    {
      //  settings->put_ClearTextPassword(
            //_bstr_t(L"password"));

        settings->put_SmartSizing(VARIANT_TRUE);

        settings->put_EnableCredSspSupport(
            VARIANT_TRUE);

        settings->Release();
    }

    g_rdpClient->put_ColorDepth(32);

    g_rdpClient->Connect();
}

// ============================================================================
// Cleanup
// ============================================================================

void CleanupRdp()
{
    if (g_rdpClient)
    {
        g_rdpClient->Disconnect();
        g_rdpClient->Release();
        g_rdpClient = nullptr;
    }

    if (g_inPlaceObject)
    {
        g_inPlaceObject->Release();
        g_inPlaceObject = nullptr;
    }

    if (g_oleObject)
    {
        g_oleObject->Close(OLECLOSE_NOSAVE);
        g_oleObject->Release();
        g_oleObject = nullptr;
    }
}

// ============================================================================
// Window Procedure
// ============================================================================

LRESULT CALLBACK WndProc(
    HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
    {
        g_hwndMain = hwnd;

        HRESULT hr = CreateRdpControl(hwnd);

        if (FAILED(hr))
        {
            MessageBoxW(
                hwnd,
                L"Failed to create RDP ActiveX control",
                L"Error",
                MB_ICONERROR);

            return -1;
        }

        ConnectRdp();

        return 0;
    }

    case WM_SIZE:
    {
        ResizeActiveX();
        return 0;
    }

    case WM_DESTROY:
    {
        CleanupRdp();

        PostQuitMessage(0);
        return 0;
    }
    }

    return DefWindowProc(
        hwnd,
        msg,
        wParam,
        lParam);
}

// ============================================================================
// WinMain
// ============================================================================

::i32 main()
{




   set_main_wndproc(WndProc);

   auto hInstance = (HINSTANCE) ::windows::hinstance_from_function(RDP_HOST2_WndProc);

   HRESULT hr = CoInitializeEx(
       nullptr,
       COINIT_APARTMENTTHREADED);

   if (FAILED(hr))
      return -1;

   WNDCLASSEXW wndclassex = {};

   // wndclassex.lpfnWndProc   = RDP_HOST2_WndProc;
   // wndclassex.hInstance     = hInstance;
   // wndclassex.lpszClassName = L"PureWin32RdpContainer";
   // wndclassex.hCursor       = LoadCursor(nullptr, IDC_ARROW);

   wndclassex.cbSize = sizeof(WNDCLASSEXW);
   wndclassex.style = CS_DBLCLKS;
   wndclassex.lpfnWndProc = RDP_HOST2_WndProc;
   wndclassex.cbClsExtra = 0;
   wndclassex.cbWndExtra = 0;
   wndclassex.hInstance = (HINSTANCE) hInstance;
   wndclassex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
   wndclassex.hCursor = LoadCursor(NULL, IDC_ARROW);
   //wndclassex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
   wndclassex.lpszMenuName = NULL;
   wndclassex.lpszClassName = L"PureWin32RdpContainer";
   wndclassex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

   RegisterClassExW(&wndclassex);

   HWND hwnd = CreateWindowExW(
       0,
       wndclassex.lpszClassName,
       L"Pure Win32 RDP ActiveX Container",
       WS_OVERLAPPEDWINDOW,
       CW_USEDEFAULT,
       CW_USEDEFAULT,
       1280,
       800,
       nullptr,
       nullptr,
       hInstance,
       nullptr);

   if (!hwnd)
   {
      CoUninitialize();
      return -1;
   }

   ShowWindow(hwnd, SW_SHOW);
   UpdateWindow(hwnd);

   LONG style = GetWindowLong(hwnd, GWL_STYLE);
   LONG exstyle = GetWindowLong(hwnd, GWL_EXSTYLE);
   WCHAR wsz[1024];
   wsprintf(
      wsz,
       L"STYLE=%08X EXSTYLE=%08X\n",
       style,
       exstyle);

   OutputDebugStringW(wsz);


   MSG msg;

   while (GetMessage(&msg, nullptr, 0, 0))
   {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
   }

   CoUninitialize();

   return 0;
}


} // namespace rdp_host4

