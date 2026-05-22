// Created by camilo on 2026-05-22 09:58 <3ThomasBorregaardSørensen!! Mummi!! Bilbo!!
// Chrome tabs:
// https : // thomasbs.com/
// https : // jumdum.com/
// https : // www.twitch.tv/jackie_codes?referrer=raid
// https : // www.twitch.tv/malthe_hansen
// https : // www.twitch.tv/t2sde
// https : // www.twitch.tv/soegaard92
// https : // www.twitch.tv/draxill69
// https : // www.twitch.tv/notmes
// https : // www.twitch.tv/jaxstyle
// https : // www.twitch.tv/directory/category/basic-programming
// https : // www.twitch.tv/directory/category/software-and-game-development
// https : // www.duolingo.com/learn
// https : // github.com/paullouisageneau/libjuice
// https : // github.com/cpp-port/libjuice
#include "framework.h"
#include "rdx_client.h"
#include "acme/_operating_system.h"
#include "acme/operating_system/windows_common/com/comptr.h"
#include "acme/platform/system.h"
#include "application.h"
//#include "main_window.h"

#define _CRT_SECURE_NO_WARNINGS

#include <comdef.h>
#include <ocidl.h>
#include <oleidl.h>
#include <windows.h>

#import "mstscax.dll" rename_namespace("RDP")

using namespace RDP;

#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")

namespace remoting_rdx_client
{

   //HWND g_hwnd = nullptr;
   //IOleObject *g_oleObject = nullptr;
   //IMsRdpClient9 *g_rdp = nullptr;


   //IConnectionPoint *g_cp = nullptr;
   //DWORD g_cookie = 0;
   //RdpEventSink *g_sink = nullptr;

   rdx_client::rdx_client
      {}


   class ClientSite : public IOleClientSite, public IOleInPlaceSite, public IOleInPlaceFrame
   {
      ULONG refCount = 1;

   public:

      // IUnknown
      HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject)
      {
         if (!ppvObject)
            return E_POINTER;

         *ppvObject = nullptr;

         if (riid == IID_IUnknown || riid == IID_IOleClientSite)
         {
            *ppvObject = (IOleClientSite *)this;
         }
         else if (riid == IID_IOleInPlaceSite)
         {
            *ppvObject = (IOleInPlaceSite *)this;
         }
         else if (riid == IID_IOleInPlaceFrame)
         {
            *ppvObject = (IOleInPlaceFrame *)this;
         }

         if (*ppvObject)
         {
            AddRef();
            return S_OK;
         }

         return E_NOINTERFACE;
      }

      ULONG STDMETHODCALLTYPE AddRef() { return ++refCount; }

      ULONG STDMETHODCALLTYPE Release()
      {
         ULONG r = --refCount;

         if (!r)
            delete this;

         return r;
      }

      // IOleClientSite
      HRESULT STDMETHODCALLTYPE SaveObject() { return E_NOTIMPL; }
      HRESULT STDMETHODCALLTYPE GetMoniker(DWORD, DWORD, IMoniker **) { return E_NOTIMPL; }
      HRESULT STDMETHODCALLTYPE GetContainer(IOleContainer **ppContainer)
      {
         *ppContainer = nullptr;
         return E_NOINTERFACE;
      }

      HRESULT STDMETHODCALLTYPE ShowObject() { return S_OK; }
      HRESULT STDMETHODCALLTYPE OnShowWindow(BOOL) { return S_OK; }
      HRESULT STDMETHODCALLTYPE RequestNewObjectLayout() { return E_NOTIMPL; }

      // IOleWindow
      HRESULT STDMETHODCALLTYPE GetWindow(HWND *phwnd)
      {
         *phwnd = g_hwnd;
         return S_OK;
      }

      HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(BOOL) { return E_NOTIMPL; }

      // IOleInPlaceSite
      HRESULT STDMETHODCALLTYPE CanInPlaceActivate() { return S_OK; }
      HRESULT STDMETHODCALLTYPE OnInPlaceActivate() { return S_OK; }
      HRESULT STDMETHODCALLTYPE OnUIActivate() { return S_OK; }

      HRESULT STDMETHODCALLTYPE GetWindowContext(IOleInPlaceFrame **ppFrame, IOleInPlaceUIWindow **ppDoc,
                                                 LPRECT lprcPosRect, LPRECT lprcClipRect,
                                                 OLEINPLACEFRAMEINFO *lpFrameInfo)
      {
         *ppFrame = this;
         AddRef();

         *ppDoc = nullptr;

         GetClientRect(g_hwnd, lprcPosRect);
         GetClientRect(g_hwnd, lprcClipRect);

         lpFrameInfo->fMDIApp = FALSE;
         lpFrameInfo->hwndFrame = g_hwnd;
         lpFrameInfo->haccel = nullptr;
         lpFrameInfo->cAccelEntries = 0;

         return S_OK;
      }

      HRESULT STDMETHODCALLTYPE Scroll(SIZE) { return E_NOTIMPL; }
      HRESULT STDMETHODCALLTYPE OnUIDeactivate(BOOL) { return S_OK; }
      HRESULT STDMETHODCALLTYPE OnInPlaceDeactivate() { return S_OK; }
      HRESULT STDMETHODCALLTYPE DiscardUndoState() { return E_NOTIMPL; }
      HRESULT STDMETHODCALLTYPE DeactivateAndUndo() { return E_NOTIMPL; }
      HRESULT STDMETHODCALLTYPE OnPosRectChange(LPCRECT prcPosRect)
      {

         IOleInPlaceObject *inplace = nullptr;

         if (SUCCEEDED(g_oleObject->QueryInterface(IID_IOleInPlaceObject, (void **)&inplace)))
         {
            inplace->SetObjectRects(prcPosRect, prcPosRect);
            inplace->Release();
         }
         return S_OK;
      }

      // IOleInPlaceUIWindow / Frame
      HRESULT STDMETHODCALLTYPE GetBorder(LPRECT) { return E_NOTIMPL; }
      HRESULT STDMETHODCALLTYPE RequestBorderSpace(LPCBORDERWIDTHS) { return E_NOTIMPL; }
      HRESULT STDMETHODCALLTYPE SetBorderSpace(LPCBORDERWIDTHS) { return S_OK; }
      HRESULT STDMETHODCALLTYPE SetActiveObject(IOleInPlaceActiveObject *, LPCOLESTR) { return S_OK; }

      HRESULT STDMETHODCALLTYPE InsertMenus(HMENU, LPOLEMENUGROUPWIDTHS) { return E_NOTIMPL; }

      HRESULT STDMETHODCALLTYPE SetMenu(HMENU, HOLEMENU, HWND) { return S_OK; }

      HRESULT STDMETHODCALLTYPE RemoveMenus(HMENU) { return S_OK; }
      HRESULT STDMETHODCALLTYPE SetStatusText(LPCOLESTR) { return S_OK; }
      HRESULT STDMETHODCALLTYPE EnableModeless(BOOL) { return S_OK; }

      HRESULT STDMETHODCALLTYPE TranslateAccelerator(LPMSG, WORD) { return E_NOTIMPL; }
   };

   ClientSite *g_site = nullptr;

   LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
   {
      switch (msg)
      {
         case WM_SIZE:
         {
            if (g_oleObject)
            {
               IOleInPlaceObject *inplace = nullptr;

               if (SUCCEEDED(g_oleObject->QueryInterface(IID_IOleInPlaceObject, (void **)&inplace)))
               {
                  RECT rc;
                  GetClientRect(hwnd, &rc);

                  inplace->SetObjectRects(&rc, &rc);
                  inplace->Release();
               }
            }

            return 0;
         }
         case WM_ERASEBKGND:
            return 1;
         case WM_DESTROY:
         {
            PostQuitMessage(0);
            return 0;
         }
      }

      return DefWindowProc(hwnd, msg, wParam, lParam);
   }

   int rdx_client_main()
   {
      // CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
      OleInitialize(nullptr);
      auto hInst = (HINSTANCE)::system()->m_hinstanceThis;
      WNDCLASS wc = {};
      wc.lpfnWndProc = WndProc;
      wc.hInstance = hInst;
      wc.lpszClassName = L"RawOLEHost";

      RegisterClass(&wc);

      g_hwnd = CreateWindowEx(0, wc.lpszClassName, L"Raw COM RDP Host",
                              WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CW_USEDEFAULT,
                              CW_USEDEFAULT, 1400, 900, nullptr, nullptr, hInst, nullptr);
      auto hwnd = g_hwnd;
      g_site = new ClientSite();

      HRESULT hr =
         CoCreateInstance(__uuidof(MsRdpClient9), nullptr, CLSCTX_INPROC_SERVER, IID_IOleObject, (void **)&g_oleObject);

      if (FAILED(hr))
      {
         MessageBox(hwnd, L"Failed to create RDP ActiveX", L"Error", MB_ICONERROR);
         return 1;
      }

      g_oleObject->SetClientSite(g_site);

      RECT rc;
      GetClientRect(g_hwnd, &rc);

      OleSetContainedObject(g_oleObject, TRUE);

      OleRun(g_oleObject);

      // SHOW first
      hr = g_oleObject->DoVerb(OLEIVERB_SHOW, nullptr, g_site, 0, g_hwnd, &rc);

      // Then UI activate
      hr = g_oleObject->DoVerb(OLEIVERB_UIACTIVATE, nullptr, g_site, 0, g_hwnd, &rc);

      IOleInPlaceObject *inplace = nullptr;

      hr = g_oleObject->QueryInterface(IID_IOleInPlaceObject, (void **)&inplace);


      hr = g_oleObject->QueryInterface(__uuidof(IMsRdpClient9), (void **)&g_rdp);

      if (FAILED(hr))
      {
         MessageBox(hwnd, L"QueryInterface failed", L"Error", MB_ICONERROR);
         return 1;
      }

      IConnectionPointContainer *cpc = nullptr;

      hr = g_rdp->QueryInterface(IID_IConnectionPointContainer, (void **)&cpc);

      if (SUCCEEDED(hr))
      {
         hr = cpc->FindConnectionPoint(__uuidof(IMsTscAxEvents), &g_cp);

         if (SUCCEEDED(hr))
         {
            g_sink = new RdpEventSink();

            g_cp->Advise((IUnknown *)g_sink, &g_cookie);
         }

         cpc->Release();
      }

      g_rdp->put_Server(_bstr_t(L"192.168.18.51"));
      // g_rdp->put_Server(_bstr_t(L"localhost"));
      g_rdp->put_DesktopWidth(2560);
      g_rdp->put_DesktopHeight(1440);

      g_rdp->Connect();

      if (SUCCEEDED(hr))
      {
         RECT rc;
         GetClientRect(g_hwnd, &rc);

         inplace->SetObjectRects(&rc, &rc);

         inplace->Release();
      }
      MSG msg;

      bool bIsConnected = false;

      while (GetMessage(&msg, nullptr, 0, 0))
      {
         TranslateMessage(&msg);
         DispatchMessage(&msg);
         // short connected = 0;

         // g_rdp->get_Connected(&connected);
         // if (connected == 1)
         //{

         //   if (!bIsConnected)
         //   {
         //      bIsConnected = true;

         //      MessageBox(nullptr, L"Now it should be connected", L"", MB_OK);

         //   }
         //}
      }

      if (g_cp)
      {
         g_cp->Unadvise(g_cookie);
         g_cp->Release();
      }

      if (g_sink)
      {
         g_sink->Release();
      }

      if (g_rdp)
         g_rdp->Disconnect();

      if (g_rdp)
         g_rdp->Release();

      if (g_oleObject)
         g_oleObject->Release();

      if (g_site)
         g_site->Release();

      // CoUninitialize();

      OleUninitialize();

      return 0;
   }


} // namespace remoting_rdx_client 



//#define _CRT_SECURE_NO_WARNINGS
//
//#include <atlbase.h>
//#include <atlwin.h>
//#include <windows.h>
//
//#import "mstscax.dll" rename_namespace("RDP")
//
//using namespace RDP;
//
//CComPtr<IMsRdpClient9> g_rdp;
//
//HWND g_hwnd = nullptr;
//
//LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
//{
//   switch (msg)
//   {
//      case WM_SIZE:
//      {
//         if (g_rdp)
//         {
//            CComQIPtr<IOleInPlaceObject> inplace(g_rdp);
//            if (inplace)
//            {
//               RECT rc;
//               GetClientRect(hwnd, &rc);
//               inplace->SetObjectRects(&rc, &rc);
//            }
//         }
//         return 0;
//      }
//
//      case WM_DESTROY:
//         PostQuitMessage(0);
//         return 0;
//   }
//
//   return DefWindowProc(hwnd, msg, wParam, lParam);
//}
//
////int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int)
//int rdx_client_main()
//{
//   CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
//   HINSTANCE hInst = (HINSTANCE)::system()->m_hinstanceThis;
//   WNDCLASS wc = {};
//   wc.lpfnWndProc = WndProc;
//   wc.hInstance = hInst;
//   wc.lpszClassName = L"MinimalRDP";
//
//   RegisterClass(&wc);
//
//   g_hwnd = CreateWindowEx(0, wc.lpszClassName, L"Embedded RDP", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
//                           1280, 720, nullptr, nullptr, hInst, nullptr);
//
//   ShowWindow(g_hwnd, SW_SHOW);
//
//   HRESULT hr = g_rdp.CoCreateInstance(__uuidof(MsRdpClient9));
//
//   if (FAILED(hr))
//   {
//      MessageBox(nullptr, L"Failed to create RDP client", L"Error", MB_ICONERROR);
//      return 1;
//   }
//
//   // Create ActiveX host
//   CComQIPtr<IOleObject> oleObj(g_rdp);
//
//   oleObj->SetClientSite(nullptr);
//
//   RECT rc;
//   GetClientRect(g_hwnd, &rc);
//
//   oleObj->DoVerb(OLEIVERB_INPLACEACTIVATE, nullptr, nullptr, 0, g_hwnd, &rc);
//
//   // Basic settings
//   //g_rdp->put_Server(_bstr_t(L"canada2.camilothomas.com"));
//   g_rdp->put_Server(_bstr_t(L"192.168.18.51"));
//   //g_rdp->put_UserName(_bstr_t(L"user"));
//
//   // Fixed resolution
//   g_rdp->put_DesktopWidth(2560);
//   g_rdp->put_DesktopHeight(1440);
//
//   // Fullscreen
//   g_rdp->put_FullScreen(VARIANT_TRUE);
//
//   // Smart sizing
//   CComQIPtr<IMsRdpClientAdvancedSettings7> adv(g_rdp);
//
//   if (adv)
//   {
//      adv->put_SmartSizing(VARIANT_TRUE);
//   }
//
//   if (adv)
//   {
//      adv->put_EnableCredSspSupport(VARIANT_TRUE);
//   }
//
//   // Disable dynamic resolution updates
//   CComQIPtr<IMsRdpExtendedSettings> ext(g_rdp);
//
//   if (ext)
//   {
//      CComVariant v(false);
//
//      ext->put_Property(_bstr_t(L"EnableFrameBufferRedirection"), &v);
//   }
//
//   hr = g_rdp->Connect();
//
//   if (FAILED(hr))
//   {
//      MessageBox(nullptr, L"Connect failed", L"Error", MB_ICONERROR);
//   }
//
//   MSG msg;
//
//   while (GetMessage(&msg, nullptr, 0, 0))
//   {
//      TranslateMessage(&msg);
//      DispatchMessage(&msg);
//   }
//
//   if (g_rdp)
//   {
//      g_rdp->Disconnect();
//   }
//
//   CoUninitialize();
//
//   return 0;
//}
