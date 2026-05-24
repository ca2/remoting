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

// #pragma comment(lib, "ole32.lib")
// #pragma comment(lib, "oleaut32.lib")
//
// // ============================================================================
// // Globals
// // ============================================================================
//
// HWND g_hwndMain2 = nullptr;
//
// IOleObject*           g_oleObject      = nullptr;
// IOleInPlaceObject*    g_inPlaceObject  = nullptr;
// IMsRdpClient9*        g_rdpClient      = nullptr;
//
// // ============================================================================
// // Basic COM RefCount Helper
// // ============================================================================
//
// class ComBase
// {
// protected:
//     ULONG m_ref;
//
// public:
//     ComBase()
//         : m_ref(1)
//     {
//     }
//
//     virtual ~ComBase()
//     {
//     }
//
//     ULONG InternalAddRef()
//     {
//         return ++m_ref;
//     }
//
//     ULONG InternalRelease()
//     {
//         ULONG r = --m_ref;
//
//         if (!r)
//             delete this;
//
//         return r;
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
//             riid == IID_IOleWindow ||
//             riid == IID_IOleInPlaceUIWindow ||
//             riid == IID_IOleInPlaceFrame)
//         {
//             *ppvObject =
//                 static_cast<IOleInPlaceFrame*>(this);
//
//             AddRef();
//             return S_OK;
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
//     HRESULT STDMETHODCALLTYPE GetWindow(HWND* phwnd) override
//     {
//         *phwnd = g_hwndMain2;
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
//    OleInPlaceFrame m_frame;
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
//       *phwnd = g_hwndMain2;
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
//       GetClientRect(g_hwndMain2, &rc);
//
//       *ppFrame = &m_frame;
//       (*ppFrame)->AddRef();
//
//       *ppDoc = nullptr;
//
//       *lprcPosRect = rc;
//       *lprcClipRect = rc;
//
//       lpFrameInfo->fMDIApp = FALSE;
//       lpFrameInfo->hwndFrame = g_hwndMain2;
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
//
// // ============================================================================
// // Resize Helper
// // ============================================================================
//
// void ResizeActiveX()
// {
//     if (!g_inPlaceObject)
//         return;
//
//     RECT rc;
//     GetClientRect(g_hwndMain2, &rc);
//
//     g_inPlaceObject->SetObjectRects(&rc, &rc);
// }
//
// // ============================================================================
// // Create RDP ActiveX Control
// // ============================================================================
//
// HRESULT CreateRdpControl(HWND hwnd)
// {
//     HRESULT hr;
//
//     hr = CoCreateInstance(
//         CLSID_MsRdpClient9NotSafeForScripting,
//         nullptr,
//         CLSCTX_INPROC_SERVER,
//         IID_IOleObject,
//         (void**)&g_oleObject);
//
//     if (FAILED(hr))
//         return hr;
//
//     hr = g_oleObject->SetClientSite(&g_clientSite);
//
//     if (FAILED(hr))
//         return hr;
//
//     OleSetContainedObject(g_oleObject, TRUE);
//
//     RECT rc;
//     GetClientRect(hwnd, &rc);
//
//     hr = g_oleObject->DoVerb(
//         OLEIVERB_SHOW,
//         nullptr,
//         &g_clientSite,
//         0,
//         hwnd,
//         &rc);
//
//     if (FAILED(hr))
//         return hr;
//
//     hr = g_oleObject->QueryInterface(
//         IID_IOleInPlaceObject,
//         (void**)&g_inPlaceObject);
//
//     if (FAILED(hr))
//         return hr;
//
//     hr = g_oleObject->QueryInterface(
//         __uuidof(IMsRdpClient9),
//         (void**)&g_rdpClient);
//
//     if (FAILED(hr))
//         return hr;
//
//     return S_OK;
// }
//
// // ============================================================================
// // Connect
// // ============================================================================
//
// void ConnectRdp()
// {
//     if (!g_rdpClient)
//         return;
//
//     g_rdpClient->put_Server(_bstr_t(L"canada5.camilothomas.com"));
//     //g_rdpClient->put_UserName(_bstr_t(L"Administrator"));
//
//     IMsRdpClientAdvancedSettings7* settings = nullptr;
//
//     if (SUCCEEDED(
//         g_rdpClient->get_AdvancedSettings8(&settings)))
//     {
//       //  settings->put_ClearTextPassword(
//             //_bstr_t(L"password"));
//
//         settings->put_SmartSizing(VARIANT_TRUE);
//
//         settings->put_EnableCredSspSupport(
//             VARIANT_TRUE);
//
//         settings->Release();
//     }
//
//     g_rdpClient->put_ColorDepth(32);
//
//     g_rdpClient->Connect();
// }
//
// // ============================================================================
// // Cleanup
// // ============================================================================
//
// void CleanupRdp()
// {
//     if (g_rdpClient)
//     {
//         g_rdpClient->Disconnect();
//         g_rdpClient->Release();
//         g_rdpClient = nullptr;
//     }
//
//     if (g_inPlaceObject)
//     {
//         g_inPlaceObject->Release();
//         g_inPlaceObject = nullptr;
//     }
//
//     if (g_oleObject)
//     {
//         g_oleObject->Close(OLECLOSE_NOSAVE);
//         g_oleObject->Release();
//         g_oleObject = nullptr;
//     }
// }
//
//
// CLASS_DECL_ACME LRESULT CALLBACK RDP_HOST2_WndProc(
//     HWND hwnd,
//     UINT msg,
//     WPARAM wParam,
//     LPARAM lParam);
// CLASS_DECL_ACME HWND get_hwnd_main();
//
// // ============================================================================
// // Window Procedure
// // ============================================================================
//
// // ============================================================================
// // WinMain
// // ============================================================================
//
// int main_rdp_host::main()
// {
//
//    //auto hInstance = (HINSTANCE) ::system()->m_hinstanceThis;
//
//    auto hinstance = (HINSTANCE) ::windows::hinstance_from_function(RDP_HOST2_WndProc);
//
//
//     HRESULT hr = CoInitializeEx(
//         nullptr,
//         COINIT_APARTMENTTHREADED);
//
//     if (FAILED(hr))
//         return -1;
//
//     WNDCLASSW wc = {};
//
//     wc.lpfnWndProc   = RDP_HOST2_WndProc;
//     wc.hInstance     = hinstance;
//     wc.lpszClassName = L"PureWin32RdpContainer";
//     wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
//
//     RegisterClassW(&wc);
//
//     HWND hwnd = CreateWindowExW(
//         0,
//         wc.lpszClassName,
//         L"Pure Win32 RDP ActiveX Container",
//         WS_OVERLAPPEDWINDOW,
//         CW_USEDEFAULT,
//         CW_USEDEFAULT,
//         1280,
//         800,
//         nullptr,
//         nullptr,
//         hinstance,
//         nullptr);
//
//     if (!hwnd)
//     {
//         CoUninitialize();
//         return -1;
//     }
//
//    g_hwndMain2 = get_hwnd_main();
//
//     ShowWindow(hwnd, SW_SHOW);
//     UpdateWindow(hwnd);
//
//    LONG style = GetWindowLong(hwnd, GWL_STYLE);
//    LONG exstyle = GetWindowLong(hwnd, GWL_EXSTYLE);
// WCHAR wsz[1024];
//    wsprintf(
//       wsz,
//        L"STYLE=%08X EXSTYLE=%08X\n",
//        style,
//        exstyle);
//
//    OutputDebugStringW(wsz);
//
//
//     MSG msg;
//
//     while (GetMessage(&msg, nullptr, 0, 0))
//     {
//         TranslateMessage(&msg);
//         DispatchMessage(&msg);
//     }
//
//     CoUninitialize();
//
//     return 0;
// }
