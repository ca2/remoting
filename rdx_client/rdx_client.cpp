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
#include "main_window.h"
#include "acme/_operating_system.h"
#include "acme/operating_system/windows_common/com/comptr.h"
#include "acme/platform/system.h"
#include "acme/windowing/windowing.h"
#include "application.h"
#include "client_site.h"
#include "com_window_thread.h"
#include "com_window.h"
#include "__implement/resource.h"
#include "event_sink.h"
#include "in_place_site.h"
//#include "main_window.h"
#include "in_place_frame.h"


//#import "mstscax.dll" rename_namespace("RDP")


//#pragma comment(lib, "ole32.lib")
//#pragma comment(lib, "oleaut32.lib")

namespace remoting_rdx_client
{

   //HWND hwnd = nullptr;
   //IOleObject *m_poleobject = nullptr;
   //IMsRdpClient9 *m_prdxclient->m_prdpclient = nullptr;


   //IConnectionPoint *m_pconnectionpoint = nullptr;
   //DWORD m_dwCookie = 0;
   //RdpEventSink *m_peventsink = nullptr;
   //
   // struct rdx_client_t
   // {
   //
   //    ::comptr < RDP::IMsRdpClient9  > m_prdpclient;
   //
   //
   // };



   rdx_client::rdx_client()
   {


      //m_prdxclient = new rdx_client_t;

   }

   rdx_client::~rdx_client()
   {

      // if (m_prdxclient)
      // {
      //
      //
      //    delete m_prdxclient;
      //
      // }



   }
   //
   // i64 rdx_client::get_style_for_creating_window()
   // {
   //
   //    return WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
   //
   // }
   //

   //
   // LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
   // {
   //    switch (msg)
   //    {
   //       case WM_SIZE:
   //       {
   //          if (m_poleobject)
   //          {
   //             IOleInPlaceObject *pinplaceobject = nullptr;
   //
   //             if (SUCCEEDED(m_poleobject->QueryInterface(IID_IOleInPlaceObject, (void **)&pinplaceobject)))
   //             {
   //                RECT rc;
   //                GetClientRect(hwnd, &rc);
   //
   //                pinplaceobject->SetObjectRects(&rc, &rc);
   //                pinplaceobject->Release();
   //             }
   //          }
   //
   //          return 0;
   //       }
   //       case WM_ERASEBKGND:
   //          return 1;
   //       case WM_DESTROY:
   //       {
   //          PostQuitMessage(0);
   //          return 0;
   //       }
   //    }
   //
   //    return DefWindowProc(hwnd, msg, wParam, lParam);
   // }
   //
   //
   // void rdx_client::on_size()
   // {
   //
   //    if (m_poleobject)
   //    {
   //
   //       ::comptr < IOleInPlaceObject > pinplaceobject;
   //
   //       m_poleobject.as(pinplaceobject);
   //
   //       auto hwnd = ::as_HWND(this->operating_system_window());
   //
   //       RECT rc;
   //
   //       GetClientRect(hwnd, &rc);
   //
   //       pinplaceobject->SetObjectRects(&rc, &rc);
   //
   //    }
   //
   // }
   //
   //
   // string rdx_client::get_title()
   // {
   //
   //    return "Remoting RDX Client";
   //
   // }



   void rdx_client::start_main_window()
   {
      //construct_newø(m_pcomwindowthread);


   }


   void rdx_client::main_window_main()
   {

      // CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
      //OleInitialize(nullptr);

      //HRESULT hr = CoInitializeEx(
    //nullptr,
    // COINIT_APARTMENTTHREADED);
    //
    //   if (FAILED(hr))
    //      throw ::hresult_exception(hr);
    //

      APTTYPE aptType;
      APTTYPEQUALIFIER qual;

      CoGetApartmentType(&aptType, &qual);

      informationf(
          "Apartment: %d qualifier: %d",
          aptType,
          qual);




      construct_newø(m_pmainwindow);

      HICON hiconBig = (HICON)LoadImage(
      GetModuleHandle(nullptr),
      MAKEINTRESOURCE(IDI_APP_ICON),
      IMAGE_ICON,
      32,
      32,
      LR_DEFAULTCOLOR);

      HICON hiconSmall = (HICON)LoadImage(
      GetModuleHandle(nullptr),
      MAKEINTRESOURCE(IDI_APP_ICON),
      IMAGE_ICON,
      32,
      32,
      LR_DEFAULTCOLOR);

      m_pmainwindow->m_pHICON_Big = (void*) hiconBig;

      m_pmainwindow->m_pHICON_Small = (void*) hiconSmall;

      //construct_newø(m_pmainwindow);

      auto sizeMainScreen = m_pmainwindow->get_main_screen_size();

      m_pmainwindow->m_rectangle.left = 0;
      m_pmainwindow->m_rectangle.top = 0;
      m_pmainwindow->m_rectangle.set_size( sizeMainScreen);

      // m_pmainwindow->m_rectangle.left = 140*2;
      // m_pmainwindow->m_rectangle.top = 90 *2;
      // m_pmainwindow->m_rectangle.set_width(1400);
      // m_pmainwindow->m_rectangle.set_height(900);

      m_pmainwindow->create_window();


      //m_pmainwindow->show_window(SW_SHOW);
      //m_pmainwindow->update_window();

      auto hwnd = ::as_HWND(m_pmainwindow->operating_system_window());

      LONG style = GetWindowLong(hwnd, GWL_STYLE);
      LONG exstyle = GetWindowLong(hwnd, GWL_EXSTYLE);

      informationf(
          "STYLE=%08X EXSTYLE=%08X",
          style,
          exstyle);

      ::get_task()->m_bMessageThread2 = true;

      ::get_task()->add_msg_translator(m_pmainwindow->in_place_frame()->get_translator_handler());

      ::get_task()->run_loop();

      //MSG msg;

      //while (GetMessage(&msg, nullptr, 0, 0))
      //{

      //   BOOL handled = FALSE;

      //   // if (m_pmainwindow->m_pclientsite->m_pinplacesite->m_pinplaceframe->m_pinplaceactiveobject)
      //   // {
      //   //
      //   //    HRESULT hr = m_pmainwindow->m_pclientsite->m_pinplacesite->m_pinplaceframe->m_pinplaceactiveobject->TranslateAccelerator(&msg);
      //   //
      //   //    handled = (hr == S_OK);
      //   //
      //   // }

      //   if (!handled)
      //   {

      //      TranslateMessage(&msg);
      //      DispatchMessage(&msg);

      //   }

      //}

      //OleUninitialize();

      //CoUninitialize();



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
//CComPtr<IMsRdpClient9> m_prdxclient->m_prdpclient;
//
//HWND hwnd = nullptr;
//
//LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
//{
//   switch (msg)
//   {
//      case WM_SIZE:
//      {
//         if (m_prdxclient->m_prdpclient)
//         {
//            CComQIPtr<IOleInPlaceObject> pinplaceobject(m_prdxclient->m_prdpclient);
//            if (pinplaceobject)
//            {
//               RECT rc;
//               GetClientRect(hwnd, &rc);
//               pinplaceobject->SetObjectRects(&rc, &rc);
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
////::i32 WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, ::i32)
//::i32 rdx_client_main()
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
//   hwnd = CreateWindowEx(0, wc.lpszClassName, L"Embedded RDP", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
//                           1280, 720, nullptr, nullptr, hInst, nullptr);
//
//   ShowWindow(hwnd, SW_SHOW);
//
//   HRESULT hr = m_prdxclient->m_prdpclient.CoCreateInstance(__uuidof(MsRdpClient9));
//
//   if (FAILED(hr))
//   {
//      MessageBox(nullptr, L"Failed to create RDP client", L"Error", MB_ICONERROR);
//      return 1;
//   }
//
//   // Create ActiveX host
//   CComQIPtr<IOleObject> oleObj(m_prdxclient->m_prdpclient);
//
//   oleObj->SetClientSite(nullptr);
//
//   RECT rc;
//   GetClientRect(hwnd, &rc);
//
//   oleObj->DoVerb(OLEIVERB_INPLACEACTIVATE, nullptr, nullptr, 0, hwnd, &rc);
//
//   // Basic settings
//   //m_prdxclient->m_prdpclient->put_Server(_bstr_t(L"canada2.camilothomas.com"));
//   m_prdxclient->m_prdpclient->put_Server(_bstr_t(L"192.168.18.51"));
//   //m_prdxclient->m_prdpclient->put_UserName(_bstr_t(L"user"));
//
//   // Fixed resolution
//   m_prdxclient->m_prdpclient->put_DesktopWidth(2560);
//   m_prdxclient->m_prdpclient->put_DesktopHeight(1440);
//
//   // Fullscreen
//   m_prdxclient->m_prdpclient->put_FullScreen(VARIANT_TRUE);
//
//   // Smart sizing
//   CComQIPtr<IMsRdpClientAdvancedSettings7> adv(m_prdxclient->m_prdpclient);
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
//   CComQIPtr<IMsRdpExtendedSettings> ext(m_prdxclient->m_prdpclient);
//
//   if (ext)
//   {
//      CComVariant v(false);
//
//      ext->put_Property(_bstr_t(L"EnableFrameBufferRedirection"), &v);
//   }
//
//   hr = m_prdxclient->m_prdpclient->Connect();
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
//   if (m_prdxclient->m_prdpclient)
//   {
//      m_prdxclient->m_prdpclient->Disconnect();
//   }
//
//   CoUninitialize();
//
//   return 0;
//}
