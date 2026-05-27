//
// Created by camilo on 2026-05-25 11:38 <3ThomasBorregaardSørensen!!
//
#include "framework.h"
#include "main_window.h"
#include "rdp_host.h"
#include "client_site.h"
#include "in_place_frame.h"
#include "in_place_site.h"
#include "acme/_operating_system.h"
#include "acme/operating_system/windows_common/com/comptr.h"
#include "acme/platform/system.h"
#include "application.h"
#include "acme/windowing/windowing.h"
#include "acme/nano/graphics/brush.h"
#include "acme/nano/graphics/context.h"
// #include "main_window.h"
#include "rdp_host.h"
#include "toolbar.h"
#include "__implement/resource.h"


namespace remoting_rdx_client
{


   main_window::main_window()
   {

      m_bIsClosing = false;
      m_bFullscreen = false;
      m_bFullscreenStored = true;

      //m_pinternal = new main_window_internal;

      //m_pinternal->m_pmainwindow = this;

      //m_dwCookie = 0;

      //m_dBreathPeriod = 60.0;

      //m_timeStart.Now();

      //m_dPhaseShift = 0.0;

      //m_bCustomPaint = true;

   }


   main_window::~main_window()
   {

      // if (m_pinternal)
      // {
      //
      //    delete m_pinternal;
      //
      //    m_pinternal = nullptr;
      //
      // }

   }


   i64 main_window::get_style_for_creating_window()
   {

      //return WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
      //return WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
      return WS_POPUP | WS_HSCROLL | WS_VSCROLL | WS_SYSMENU  | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_VISIBLE;

   }

   i64 main_window::get_ex_style_for_creating_window()
   {

      return WS_EX_APPWINDOW;

   }

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


   // void main_window::do_control_layout()
   // {
   //
   //    if (!m_poleobject)
   //    {
   //
   //       return;
   //
   //    }
   //
   //    ::comptr<IOleInPlaceObject> pinplaceobject;
   //
   //    m_poleobject.as(pinplaceobject);
   //
   //    if (!pinplaceobject)
   //    {
   //
   //       information("No IOleInPlaceObject");
   //
   //       return;
   //
   //    }
   //
   //    auto hwnd = ::as_HWND(this->operating_system_window());
   //
   //    RECT rc;
   //
   //    GetClientRect(hwnd, &rc);
   //
   //    informationf(
   //        "SetObjectRects: %d x %d",
   //        rc.right - rc.left,
   //        rc.bottom - rc.top);
   //
   //    HRESULT hr =
   //        pinplaceobject->SetObjectRects(&rc, &rc);
   //
   //    if (FAILED(hr))
   //    {
   //
   //       information(hresult_to_string(hr));
   //
   //    }
   //
   // }
   //
   //
   // void main_window::control_activate(bool bActivate)
   // {
   //
   //    if (m_pclientsite->m_pinplacesite->m_pinplaceframe->m_pinplaceactiveobject)
   //    {
   //
   //       m_pclientsite->m_pinplacesite->m_pinplaceframe->m_pinplaceactiveobject->OnFrameWindowActivate(bActivate ? TRUE : FALSE);
   //
   //       m_pclientsite->m_pinplacesite->m_pinplaceframe->m_pinplaceactiveobject->OnDocWindowActivate(bActivate ? TRUE : FALSE);
   //
   //    }
   //
   // }
   //
   //
   // void main_window::do_control_show_verb()
   // {
   //
   //    auto hwnd = ::as_HWND(this->operating_system_window());
   //
   //    auto rectangleClient = window_get_client_rect();
   //
   //    auto rect = ::as_RECT(rectangleClient);
   //
   //    auto hr = m_poleobject->DoVerb(OLEIVERB_SHOW, nullptr, m_pclientsite, 0, hwnd, &rect);
   //
   //    if (FAILED(hr))
   //    {
   //
   //       information("do_control_ui_activate DoVer(OLEIVERB_SHOW ... ) Failed");
   //
   //    }
   //
   // }
   //
   //
   // void main_window::do_control_ui_activate()
   // {
   //
   //    auto hwnd = ::as_HWND(this->operating_system_window());
   //
   //    auto rectangleClient = window_get_client_rect();
   //
   //    auto rect = ::as_RECT(rectangleClient);
   //
   //    auto hr = m_poleobject->DoVerb(OLEIVERB_UIACTIVATE, nullptr, m_pclientsite, 0, hwnd, &rect);
   //
   //    if (FAILED(hr))
   //    {
   //
   //       information("do_control_ui_activate DoVer(OLEIVERB_UIACTIVATE ... ) Failed");
   //
   //    }
   //
   // }
   //
   //
   void main_window::on_window_size()
   {

   }


   /// Rdp Host size is set to the host operating system main display size
   void main_window::defer_update_rdp_host_size()
   {

      if (m_prdphost)
      {

         ::i32_rectangle rectangleMainScreen(get_main_screen_size());

         if (rectangleMainScreen != m_rectangleRdpHost)
         {
            m_rectangleRdpHost = rectangleMainScreen;
            auto rectangleClient = get_client_rectangle();
            m_pointScroll.x = minimum(m_pointScroll.x, (m_rectangleRdpHost.width() - rectangleClient.width()));
            m_pointScroll.y = minimum(m_pointScroll.y, (m_rectangleRdpHost.height() - rectangleClient.height()));
            m_prdphost->set_window_position(::as_operating_system_window({HWND_TOP}), -m_pointScroll,
                                            m_rectangleRdpHost.size(), SWP_SHOWWINDOW);
         }

      }

   }
   //
   //
   // void main_window::on_window_set_focus()
   // {
   //
   //    control_activate(true);
   //
   // }
   //
   //
   // bool main_window::on_window_activate(::i32 iActivate, bool bMinimized,
   //                                      const operating_system::window &operatingsystemwindow)
   // {
   //
   //    auto active = iActivate != WA_INACTIVE;
   //
   //    control_activate(active);
   //
   //    return false;
   //
   // }
   //
   //
   // bool main_window::on_window_mouse_activate(::i32 &iResult, const operating_system::window &operatingsystemwindowTop, ::i32 iHitTest, ::i32 iMessage)
   // {
   //
   //    iResult = MA_ACTIVATE;
   //
   //    return true;
   //
   // }


   void main_window::update_style()
   {
      auto iStyle = get_window_style();

      if (m_bFullscreen)
      {

         iStyle &= ~WS_OVERLAPPEDWINDOW;
         iStyle |= WS_POPUP | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;

      }
      else
      {

         iStyle &= ~WS_POPUP;
         iStyle |= WS_OVERLAPPEDWINDOW;

      }

      set_window_style(iStyle);

      set_window_position({}, {}, {},
      SWP_FRAMECHANGED |
                        SWP_NOMOVE |
                        SWP_NOSIZE |
                     SWP_NOZORDER);

      defer_update_system_menu();


   }


   void main_window::layout_rdp_host()
   {

      m_prdphost->set_window_position(
         ::as_operating_system_window({HWND_TOP}),
         -m_pointScroll,
         {},
         SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

      //m_prdphost->do_control_layout();

   }


   void main_window::set_focus_to_rdp_host()
   {

      information("::remoting_rdx_client::main_window::set_focus_to_rdp_host");

      set_foreground_window(::system()->acme_windowing()->get_user_activation_token());

      set_active_window();

      m_prdphost->window_set_focus();

   }


   void main_window::activate_rdp_host_control(bool bActive)
   {

      m_prdphost->control_activate(bActive);

   }

   
   void main_window::ui_activate_rdp_host_control()
   {

      m_prdphost->do_control_ui_activate();

   }


   void main_window::do_cancel_mode()
   {

      m_ptoolbar->send_message((::user::enum_message) WM_CANCELMODE);
      send_message((::user::enum_message) WM_CANCELMODE);
      m_prdphost->send_message((::user::enum_message) WM_CANCELMODE);

   }



   bool main_window::_on_window_procedure(lresult &lresult, u32 message, wparam wparam, lparam lparam)
   {

      switch (message)
      {
         case WM_HSCROLL:
         {
            SCROLLINFO si{};
            si.cbSize = sizeof(si);
            si.fMask = SIF_ALL;

            auto hwnd = ::as_HWND(this->operating_system_window());

            GetScrollInfo(hwnd, SB_HORZ, &si);

            ::i32 xOld = si.nPos;

            switch(wparam.loword())
            {
               case SB_LINELEFT:
                  si.nPos -= 20;
                  break;

               case SB_LINERIGHT:
                  si.nPos += 20;
                  break;

               case SB_PAGELEFT:
                  si.nPos -= si.nPage;
                  break;

               case SB_PAGERIGHT:
                  si.nPos += si.nPage;
                  break;

               case SB_THUMBTRACK:
                  si.nPos = si.nTrackPos;
                  break;
            }

            si.fMask = SIF_POS;

            SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
            GetScrollInfo(hwnd, SB_HORZ, &si);

            m_pointScroll.x = si.nPos;

            ScrollWindowEx(
               hwnd,
               xOld - m_pointScroll.x,
               0,
               nullptr,
               nullptr,
               nullptr,
               nullptr,
               SW_INVALIDATE);

            layout_rdp_host();

            return true;
         }
         case WM_VSCROLL:
         {
            SCROLLINFO si{};
            si.cbSize = sizeof(si);
            si.fMask = SIF_ALL;

            auto hwnd = ::as_HWND(this->operating_system_window());

            GetScrollInfo(hwnd, SB_VERT, &si);

            ::i32 yOld = si.nPos;

            switch(wparam.loword())
            {
               case SB_LINELEFT:
                  si.nPos -= 20;
                  break;

               case SB_LINERIGHT:
                  si.nPos += 20;
                  break;

               case SB_PAGELEFT:
                  si.nPos -= si.nPage;
                  break;

               case SB_PAGERIGHT:
                  si.nPos += si.nPage;
                  break;

               case SB_THUMBTRACK:
                  si.nPos = si.nTrackPos;
                  break;
            }

            si.fMask = SIF_POS;

            SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
            GetScrollInfo(hwnd, SB_VERT, &si);

            m_pointScroll.y = si.nPos;

            ScrollWindowEx(
               hwnd,
               0,
               yOld - m_pointScroll.y,
               nullptr,
               nullptr,
               nullptr,
               nullptr,
               SW_INVALIDATE);

            layout_rdp_host();

            return true;
         }
         case WM_SIZE:
         {

            bool bMinimized = wparam == 1;

            if (bMinimized)
            {

               information("WM_SIZE Minimized!");

            }
            else
            {

               information("WM_SIZE NOT Minimized!");

               update_scrollbars();

            }

            RECT rcClient;
            RECT rcWindow;

            auto hwnd = ::as_HWND(this->operating_system_window());

            GetClientRect(hwnd, &rcClient);

            ::i32_rectangle rectangleClient(rcClient);

            information("main_window WM_SIZE Client {}", rectangleClient);

            GetWindowRect(hwnd, &rcWindow);

            ::i32_rectangle rectangleWindow(rcWindow);

            information("main_window WM_SIZE Window {} {}x{}", rectangleWindow, rectangleWindow.width(), rectangleWindow.height());

            MapWindowPoints(nullptr, hwnd, (POINT*)&rcWindow, 2);

            ::i32_rectangle rectangleWindow2 = rcWindow;

            information("main_window WM_SIZE MapWindowPoints {}", rectangleWindow2);

            ::string strMessage;

            auto iStyle = get_window_style();

            strMessage.format("main_window WM_SIZE;");

            if (iStyle & WS_POPUP)
            {

               strMessage += " with popup;";

            }
            else
            {

               strMessage += " without popup;";

            }

            if (iStyle & WS_BORDER)
            {

               strMessage += " with border;";

            }
            else
            {

               strMessage += " without border;";

            }

            if (iStyle & WS_DLGFRAME)
            {

               strMessage += " with dialog frame;";

            }
            else
            {

               strMessage += " without dialog frame;";

            }

            if (iStyle & WS_SYSMENU)
            {

               strMessage += " with system menu;";

            }
            else
            {

               strMessage += " without system menu;";

            }

            information(strMessage);

         }
         break;
         case WM_STYLECHANGED:
         {

            auto pstylestruct = lparam.raw_cast<LPSTYLESTRUCT>();

            if (wparam == GWL_STYLE)
            {

               ::i32 iStyleOld =pstylestruct->styleNew;
               ::i32 iStyleNew =pstylestruct->styleNew;

               information("old style {} new style {}", iStyleOld, iStyleNew);

            }
            else if (wparam == GWL_EXSTYLE)
            {

               ::i32 iExStyleOld =pstylestruct->styleNew;
               ::i32 iExStyleNew =pstylestruct->styleNew;

               information("old ex style {} new ex style {}", iExStyleOld, iExStyleNew);

            }

         }
         break;
         case WM_TIMER:
         {

            if (wparam == 1453)
            {

               show_window(SW_SHOW);

               auto hwnd = ::as_HWND(this->operating_system_window());
               KillTimer(hwnd, 1453);

            }

         }
            break;
         case WM_SYSCOMMAND:
         {
            if ((wparam.loword() & 0xFFF0) == SC_MAXIMIZE)
            {
               //m_pinternal->m_prdpclient->put_FullScreen(VARIANT_TRUE);
               doFullscreen();
               lresult= 0; // 👈 block normal maximize
               return true;
            }
            else if ((wparam.loword() & 0xFFF0) == SC_RESTORE)
            {
               if (m_bFullscreenStored)
               {
                  //m_pinternal->m_prdpclient->put_FullScreen(VARIANT_TRUE);
                  doFullscreen();
               }
               else
               {
                  doRestore();

               }
               lresult= 0; // 👈 block normal maximize
               return true;
            }

            break;
         }
         case WM_CLOSE:
         {
            auto hwnd = ::as_HWND(this->operating_system_window());
            KillTimer(hwnd, 1453);

            shutdown_rdp();

         }
            break;
         case WM_APP + 100:
         {

            ::i32 iCmd = wparam.raw_cast<::i32>();

            switch (iCmd)
            {
            case 1: MinimizeRdp(); break;
            case 2: RestoreRdp(); break;
            case 3: CloseRdp(); break;
            }

         }


            break;

         case WM_MOUSEMOVE:
         case WM_LBUTTONDOWN:
         case WM_LBUTTONUP:
         {

            ::string strMessage;

            auto iStyle = get_window_style();

            strMessage.format("mouse message;");

            if (iStyle & WS_POPUP)
            {

               strMessage += " with popup;";

            }
            else
            {

               strMessage += " without popup;";

            }

            if (iStyle & WS_BORDER)
            {

               strMessage += " with border;";

            }
            else
            {

               strMessage += " without border;";

            }

            if (iStyle & WS_DLGFRAME)
            {

               strMessage += " with dialog frame;";

            }
            else
            {

               strMessage += " without dialog frame;";

            }

            information(strMessage);

         }
            break;

         case WM_NCMOUSEMOVE:
         case WM_NCLBUTTONDOWN:
         case WM_NCLBUTTONUP:
         {

            ::string strMessage;

            auto iStyle = get_window_style();

            strMessage.format("nc mouse message;");

            if (iStyle & WS_POPUP)
            {

               strMessage += " with popup;";

            }
            else
            {

               strMessage += " without popup;";

            }

            if (iStyle & WS_BORDER)
            {

               strMessage += " with border;";

            }
            else
            {

               strMessage += " without border;";

            }

            if (iStyle & WS_DLGFRAME)
            {

               strMessage += " with dialog frame;";

            }
            else
            {

               strMessage += " without dialog frame;";

            }

            information(strMessage);

         }
            break;
         case WM_SHOWWINDOW:
         {

            auto bShow = wparam.m_wparam != FALSE;

            if (!bShow)
            {

               information("main window is being hidden");

            }

         }
            break;


      }
      return false;
      // if (message == WM_CREATE)
      // {
      //
      //    on_create_window();
      //
      //    lresult = 0;
      //
      //    return true;
      //
      // }
      // else if (message == WM_SIZE)
      // {
      //
      //    on_window_size();
      //
      //    lresult = 0;
      //
      //    return true;
      //
      // }
      //
      // auto hwnd = ::as_HWND(this->operating_system_window());
      //
      // lresult = DefWindowProc(hwnd, message, wparam, lparam);
      //
      // return true;


   }


   void main_window::shutdown_rdp()
   {

      if (m_bIsClosing)
      {

         return;

      }

      m_bIsClosing = true;

      auto hwnd = ::as_HWND(this->operating_system_window());

      KillTimer(hwnd, 1453);

      show_window(SW_HIDE);

      if (m_prdphost)
      {

         m_prdphost->shutdown_rdp();

      }

      PostQuitMessage(0);

   }


   string main_window::get_title()
   {

      return "Remoting RDX Client";

   }


   void main_window::on_create_window()
   {

      m_papplication->m_pacmeuserinteractionMain = this;

      //m_rclsid = __uuidof(RDP::MsRdpClient9);

      //m_rclsid = CLSID_MsRdpClient9NotSafeForScripting;

      //com_window::on_create_window();

      // HRESULT hr =
      //    CoCreateInstance(__uuidof(RDP::MsRdpClient9), nullptr, CLSCTX_INPROC_SERVER, IID_IOleObject, (void **)&m_poleobject);
      //
      // if (FAILED(hr))
      // {
      //
      //    post_message_box(L"Failed to create RDP ActiveX", L"Error", ::user::e_message_box_icon_error);
      //
      //    return;
      //
      // }
      //
      // m_pclientsite->initialize_client_site(this);
      //
      // m_poleobject->SetClientSite(m_pclientsite);
      //
      // OleSetContainedObject(m_poleobject, TRUE);
      //
      // OleRun(m_poleobject);
      //
      // do_control_show_verb();
      //
      // do_control_ui_activate();
      //
      // ::comptr < IOleInPlaceObject > pinplaceobject;
      //
      // m_poleobject.as(pinplaceobject);

//      auto hr = m_poleobject.as(m_pinternal->m_prdpclient);

      auto hwnd = ::as_HWND(this->operating_system_window());

      // if (FAILED(hr))
      // {
      //
      //    MessageBox(hwnd, L"QueryInterface failed", L"Error", MB_ICONERROR);
      //
      //    return;
      //
      // }
      //
      //  ::comptr < IConnectionPointContainer > pconnectionpointcontainer;
      //
      //  m_pinternal->m_prdpclient.as(pconnectionpointcontainer);
      //
      //  m_pinternal->event_sink<IMsTscAxEvents>::initialize_event_sink(pconnectionpointcontainer);

       ::cast<::remoting_rdx_client::application> papp = m_papplication;

      ::wstring wstrHost = papp->m_strHost;

      // m_pinternal->m_prdpclient->put_Server(_bstr_t(wstrHost));
      //
      // ::comptr<IMsRdpClientAdvancedSettings7> padvancedsettings;
      //
      // m_pinternal->m_prdpclient->get_AdvancedSettings8(&padvancedsettings);
      //
      // if (padvancedsettings)
      // {
      //
      //    padvancedsettings->put_EnableCredSspSupport(VARIANT_TRUE);
      //
      //    //padvancedsettings->put_ContainerHandledFullScreen(VARIANT_TRUE);
      //
      //    padvancedsettings->put_SmartSizing(VARIANT_FALSE);
      //
      //    padvancedsettings->put_EnableAutoReconnect(VARIANT_FALSE);
      //
      //    padvancedsettings->put_SmartSizing(VARIANT_FALSE);
      //
      //    padvancedsettings->put_BitmapCacheSize(0);
      //
      //    padvancedsettings->put_BitmapPersistence(VARIANT_FALSE);
      //
      //    padvancedsettings->put_PerformanceFlags(0);
      //
      //    //padvancedsettings->put_ConnectionBarShowMinimizeButton(VARIANT_FALSE);
      //
      // }
      // IMsRdpExtendedSettings *pext = nullptr;
      //
      // ::comptr<IMsRdpExtendedSettings> ext;
      // m_pinternal->m_prdpclient.as(ext);
      //
      // if (ext)
      // {
      //    _variant_t vFalse(VARIANT_FALSE);
      //
      //    ext->put_Property(
      //        _bstr_t(L"EnableHardwareMode"),
      //        &vFalse);
      //
      //    ext->put_Property(
      //        _bstr_t(L"UseURCP"),
      //        &vFalse);
      //
      //    ext->put_Property(
      //        _bstr_t(L"DisableUDPTransport"),
      //        &vFalse);
      //
      //    _variant_t varScaleFactor((LONG)100);
      //
      //    ext->put_Property(_bstr_t(L"DesktopScaleFactor"), &varScaleFactor);
      //
      //    ext->put_Property(_bstr_t(L"DeviceScaleFactor"), &varScaleFactor);
      //
      // }
      //
      // m_pinternal->m_prdpclient->put_ColorDepth(32);

      construct_newø(m_prdphost);

      m_prdphost->m_pmainwindow = this;

      m_prdphost->m_pacmeuserinteractionParent = this;

      m_prdphost->m_rectangle = this->m_rectangle;

      m_prdphost->create_window();

      defer_update_rdp_host_size();

  //     ::i32 screenWidth = GetSystemMetrics(SM_CXSCREEN);
  //
  //     ::i32 screenHeight = GetSystemMetrics(SM_CYSCREEN);
  //
  //     m_pinternal->m_prdpclient->put_DesktopWidth(screenWidth);
  //
  //     m_pinternal->m_prdpclient->put_DesktopHeight(screenHeight);
  //
  // //    m_pinternal->m_prdpclient->put_FullScreen(VARIANT_TRUE);
  //

      auto sizeMainScreen = get_main_screen_size();

      construct_newø(m_ptoolbar);

     m_ptoolbar->m_strTitle = papp->m_strHost;

     ::i32_size sizeToolbar(400 * get_window_scale(), 24 * get_window_scale());

     set_window_text(papp->m_strHost);

     // m_ptoolbar->m_rectangle.left = ;
     // m_ptoolbar->m_rectangle.top = 0;
     // m_ptoolbar->m_rectangle.set_size(sizeToolbar);

     m_ptoolbar->m_pacmeuserinteractionParent = this;
     //m_ptoolbar->Create(this, screenWidth /2 - iToolbarWidth / 2, 0, iToolbarWidth, iToolbarHeight);

     m_ptoolbar->m_pmainwindow = this;

     m_ptoolbar->create_window();

     m_ptoolbar->Place(sizeMainScreen.width() /2 - sizeToolbar.cx / 2, 0, sizeToolbar.cx, sizeToolbar.cy);


     m_ptoolbar->show_window(SW_SHOW);
     m_ptoolbar->Redraw();
     m_ptoolbar->InstallMouseHook();
     // ShowWindow(m_hwnd, SW_SHOW);
     //     //
     //     // Redraw();
     //     // InstallMouseHook();
     //     //

   //
      // auto hrConnect = m_pinternal->m_prdpclient->Connect();

   }


   void main_window::on_window_paint(::nano::graphics::context *pgraphicscontext)
   {


      // auto rectangle = ::i32_rectangle(100, 100, 500, 200);
      //
      // auto pnanobrush = createø<::nano::graphics::brush>();
      //
      // pnanobrush->m_color = argb(160, 100, 180, 240);
      //
      // pgraphicscontext->rectangle(rectangle, pnanobrush, nullptr);


   }


// #define DISPID_RDPCONTROL_ONCONNECTING 1
// #define DISPID_RDPCONTROL_ONCONNECTED 2
// #define DISPID_RDPCONTROL_ONLOGINCOMPLETE 3
// #define DISPID_RDPCONTROL_ONDISCONNECTED 4
// #define DISPID_RDPCONTROL_ONENTERFULLSCREENMODE 5
// #define DISPID_RDPCONTROL_ONLEAVEFULLSCREENMODE 6
// #define DISPID_RDPCONTROL_ONREQUESTENTERFULLSCREENMODE 8
// #define DISPID_RDPCONTROL_ONREQUESTLEAVEFULLSCREENMODE 9
// #define DISPID_RDPCONTROL_ONFATALERROR 10
//
//
//    HRESULT STDMETHODCALLTYPE main_window_internal::Invoke(DISPID dispIdMember, REFIID, LCID, WORD, DISPPARAMS *pDispParams,
//                                                 VARIANT *, EXCEPINFO *, UINT *)
//    {
//       switch (dispIdMember)
//       {
//          case DISPID_RDPCONTROL_ONCONNECTING:
//             //MessageBox(m_hwnd, L"Connected", L"RDP Event", MB_OK);
//             information("RDP Event :: Connecting");
//             break;
//          case DISPID_RDPCONTROL_ONCONNECTED:
//             //MessageBox(m_hwnd, L"Connected", L"RDP Event", MB_OK);
//             information("RDP Event :: Connected");
//             on_rdp_connected();
//             break;
//
//          case DISPID_RDPCONTROL_ONLOGINCOMPLETE:
//             //information("RDP Event :: Connected");MessageBox(m_hwnd, L"Login complete", L"RDP Event", MB_OK);
//
//             information("RDP Event :: Login Complete");
//             on_rdp_login_complete();
//             break;
//             // case DISPID_DISCONNECTED:
//             //  MessageBox(g_hwnd, L"Disconnected", L"RDP Event", MB_OK);
//             // break;
//          case DISPID_RDPCONTROL_ONDISCONNECTED:
//          {
//
//             LONG reason = 0;
//
//             if (pDispParams && pDispParams->cArgs >= 1)
//             {
//
//                VARIANTARG &arg = pDispParams->rgvarg[0];
//
//                if (arg.vt == VT_I4)
//                {
//
//                   reason = arg.lVal;
//                }
//             }
//
//             auto hwnd = ::as_HWND(this->operating_system_window());
//
//             wchar_t buf[256];
//
//             swprintf_s(buf, L"Disconnected. Reason code = %ld", reason);
//
//             // MessageBox(hwnd, buf, L"RDP Disconnect", MB_OK);
//             ::PostMessage(hwnd, WM_CLOSE, 0, 0);
//          }
//          break;
//          case DISPID_RDPCONTROL_ONFATALERROR:
//          {
//
//             auto hwnd = ::as_HWND(this->operating_system_window());
//
//             //MessageBox(hwnd, L"Fatal error", L"RDP Event", MB_OK);
//             wchar_t buf[256];
//
//             swprintf_s(buf, L"Fatal error RDP Event");
//
//             ::PostMessage(hwnd, WM_CLOSE, 0, 0);
//
//          }
//             break;
//
//
//          case DISPID_RDPCONTROL_ONENTERFULLSCREENMODE:
//             OnEnterFullScreen();
//             break;
//
//          case DISPID_RDPCONTROL_ONLEAVEFULLSCREENMODE:
//             OnLeaveFullScreen();
//             break;
//
//          case DISPID_RDPCONTROL_ONREQUESTENTERFULLSCREENMODE:
//             OnRequestGoFullScreen();
//             break;
//
//          case DISPID_RDPCONTROL_ONREQUESTLEAVEFULLSCREENMODE:
//             OnRequestLeaveFullScreen();
//             break;
//       }
//             return S_OK;
//       }

      // void main_window::OnEnterFullScreen()
      // {
      //    auto hwnd = ::as_HWND(this->operating_system_window());
      //
      //    if (::IsWindowVisible(hwnd))
      //    {
      //       ShowWindow(hwnd, SW_HIDE);
      //    }
      // }
      //
      // void main_window::OnLeaveFullScreen()
      // {
      // auto hwnd = ::as_HWND(this->operating_system_window());
      //    ShowWindow(hwnd, SW_SHOW);
      // }
      //
      // void main_window::OnRequestGoFullScreen()
      // {
      // auto hwnd = ::as_HWND(this->operating_system_window());
      //    ShowWindow(hwnd, SW_HIDE);
      // }
      //
      // void main_window::OnRequestLeaveFullScreen()
      // {
      // auto hwnd = ::as_HWND(this->operating_system_window());
      //    // 👈 THIS is your “restore clicked”
      //    ShowWindow(hwnd, SW_SHOW);
      // }

//
// } // namespace remoting_rdx_client
//
//
//
// //#define _CRT_SECURE_NO_WARNINGS
// //
// //#include <atlbase.h>
// //#include <atlwin.h>
// //#include <windows.h>
// //
// //#import "mstscax.dll" rename_namespace("RDP")
// //
// //using namespace RDP;
// //
// //CComPtr<IMsRdpClient9> m_pinternal->m_prdpclient;
// //
// //HWND hwnd = nullptr;
// //
// //LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
// //{
// //   switch (msg)
// //   {
// //      case WM_SIZE:
// //      {
// //         if (m_pinternal->m_prdpclient)
// //         {
// //            CComQIPtr<IOleInPlaceObject> pinplaceobject(m_pinternal->m_prdpclient);
// //            if (pinplaceobject)
// //            {
// //               RECT rc;
// //               GetClientRect(hwnd, &rc);
// //               pinplaceobject->SetObjectRects(&rc, &rc);
// //            }
// //         }
// //         return 0;
// //      }
// //
// //      case WM_DESTROY:
// //         PostQuitMessage(0);
// //         return 0;
// //   }
// //
// //   return DefWindowProc(hwnd, msg, wParam, lParam);
// //}
// //
// ////::i32 WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, ::i32)
// //::i32 rdx_client_main()
// //{
// //   CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
// //   HINSTANCE hInst = (HINSTANCE)::system()->m_hinstanceThis;
// //   WNDCLASS wc = {};
// //   wc.lpfnWndProc = WndProc;
// //   wc.hInstance = hInst;
// //   wc.lpszClassName = L"MinimalRDP";
// //
// //   RegisterClass(&wc);
// //
// //   hwnd = CreateWindowEx(0, wc.lpszClassName, L"Embedded RDP", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
// //                           1280, 720, nullptr, nullptr, hInst, nullptr);
// //
// //   ShowWindow(hwnd, SW_SHOW);
// //
// //   HRESULT hr = m_pinternal->m_prdpclient.CoCreateInstance(__uuidof(MsRdpClient9));
// //
// //   if (FAILED(hr))
// //   {
// //      MessageBox(nullptr, L"Failed to create RDP client", L"Error", MB_ICONERROR);
// //      return 1;
// //   }
// //
// //   // Create ActiveX host
// //   CComQIPtr<IOleObject> oleObj(m_pinternal->m_prdpclient);
// //
// //   oleObj->SetClientSite(nullptr);
// //
// //   RECT rc;
// //   GetClientRect(hwnd, &rc);
// //
// //   oleObj->DoVerb(OLEIVERB_INPLACEACTIVATE, nullptr, nullptr, 0, hwnd, &rc);
// //
// //   // Basic settings
// //   //m_pinternal->m_prdpclient->put_Server(_bstr_t(L"canada2.camilothomas.com"));
// //   m_pinternal->m_prdpclient->put_Server(_bstr_t(L"192.168.18.51"));
// //   //m_pinternal->m_prdpclient->put_UserName(_bstr_t(L"user"));
// //
// //   // Fixed resolution
// //   m_pinternal->m_prdpclient->put_DesktopWidth(2560);
// //   m_pinternal->m_prdpclient->put_DesktopHeight(1440);
// //
// //   // Fullscreen
// //   m_pinternal->m_prdpclient->put_FullScreen(VARIANT_TRUE);
// //
// //   // Smart sizing
// //   CComQIPtr<IMsRdpClientAdvancedSettings7> adv(m_pinternal->m_prdpclient);
// //
// //   if (adv)
// //   {
// //      adv->put_SmartSizing(VARIANT_TRUE);
// //   }
// //
// //   if (adv)
// //   {
// //      adv->put_EnableCredSspSupport(VARIANT_TRUE);
// //   }
// //
// //   // Disable dynamic resolution updates
// //   CComQIPtr<IMsRdpExtendedSettings> ext(m_pinternal->m_prdpclient);
// //
// //   if (ext)
// //   {
// //      CComVariant v(false);
// //
// //      ext->put_Property(_bstr_t(L"EnableFrameBufferRedirection"), &v);
// //   }
// //
// //   hr = m_pinternal->m_prdpclient->Connect();
// //
// //   if (FAILED(hr))
// //   {
// //      MessageBox(nullptr, L"Connect failed", L"Error", MB_ICONERROR);
// //   }
// //
// //   MSG msg;
// //
// //   while (GetMessage(&msg, nullptr, 0, 0))
// //   {
// //      TranslateMessage(&msg);
// //      DispatchMessage(&msg);
// //   }
// //
// //   if (m_pinternal->m_prdpclient)
// //   {
// //      m_pinternal->m_prdpclient->Disconnect();
// //   }
// //
// //   CoUninitialize();
// //
// //   return 0;
// //}


   void main_window::OnRdpConnecting() {


   }


   void main_window::OnRdpConnected() {





   }


   void main_window::OnRdpLoginComplete() {


      show_window(SW_SHOW);

   }


   void main_window::OnRdpDisconnected()
   {

      shutdown_rdp();

   }


   void main_window::OnRdpEnterFullScreen() {

      auto hwnd = ::as_HWND(this->operating_system_window());

      if (::IsIconic(hwnd))
      {
         ShowWindow(hwnd, SW_MINIMIZE);
      }

   }


   void main_window::OnRdpLeaveFullScreen() {

      auto hwnd = ::as_HWND(this->operating_system_window());

      //if (::IsWindowVisible(hwnd))
      {

         SetTimer(hwnd, 1453, 150, nullptr);
         //ShowWindow(hwnd, SW_SHOW);
      }

   }


   void main_window::OnRdpRequestGoFullScreen() {

      auto hwnd = ::as_HWND(this->operating_system_window());

      if (::IsWindowVisible(hwnd))
      {
         ShowWindow(hwnd, SW_HIDE);
      }

   }


   void main_window::OnRdpRequestLeaveFullScreen() {

      auto hwnd = ::as_HWND(this->operating_system_window());

      if (::IsIconic(hwnd))
      {
         ShowWindow(hwnd, SW_MINIMIZE);
      }

   }



//
//
//    void main_window::on_rdp_connected()
//    {
//
//  //     do_control_layout();
//
//    }
//
//
//    void main_window::on_rdp_login_complete()
//    {
//
//       //do_control_layout();
//       //window_invalidate_rect(nullptr, true);
//       //show_window(SW_SHOW);
//       ///update_window();
//       //redraw_window(nullptr, nullptr, RDW_INVALIDATE | RDW_ALLCHILDREN | RDW_UPDATENOW);
//       //dump_operating_system_child_window_hierarchy();
//       //control_activate(true);
//       //window_set_focus();
//       //do_control_show_verb();
//       //do_control_ui_activate();
// //
// //       auto hwnd = ::as_HWND(this->operating_system_window());
// //
// //
// //       EnumChildWindows(hwnd, [](HWND child, LPARAM)
// // {
// //     wchar_t cls[256];
// //     GetClassNameW(child, cls, 256);
// //
// //     if (
// //         wcscmp(cls, L"OPWindowClass") == 0 ||
// //         wcscmp(cls, L"OPContainerClass") == 0)
// //     {
// //         LONG style = GetWindowLong(child, GWL_STYLE);
// //
// //         style |= WS_VISIBLE;
// //
// //         SetWindowLong(child, GWL_STYLE, style);
// //
// //         ShowWindow(child, SW_SHOW);
// //
// //         UpdateWindow(child);
// //     }
// //
// //     return TRUE;
// // }, 0);
// //
//
//    }
//



   void main_window::MinimizeRdp()
   {

      doMinimize();

   }


   void main_window::RestoreRdp()
   {

      doRestore();

   }


   void main_window::CloseRdp()
   {

      shutdown_rdp();

   }


   void main_window::doMinimize()
   {

      m_bFullscreen = false;


      m_prdphost->onExitFullscreen();

      m_ptoolbar->show_window(SW_HIDE);
      m_ptoolbar ->m_bMouseEnable = false;
      m_ptoolbar->m_ehitHover = toolbar::e_hit_none;
      m_ptoolbar->m_ehitPress = toolbar::e_hit_none;
      m_ptoolbar->Redraw();
      show_window(SW_MINIMIZE);

   }


   void main_window::doRestore()
   {

      m_bFullscreen = false;

      m_bFullscreenStored = false;

      m_prdphost->onExitFullscreen();

      m_ptoolbar->show_window(SW_HIDE);
      m_ptoolbar ->m_bMouseEnable = false;
      m_ptoolbar->m_ehitHover = toolbar::e_hit_none;
      m_ptoolbar->m_ehitPress = toolbar::e_hit_none;
      m_ptoolbar->Redraw();

      //set_window_style(WS_OVERLAPPEDWINDOW);

      show_window(SW_SHOWNORMAL);

      auto r = get_client_rectangle();

      auto sMainScreen = get_main_screen_size();

      sMainScreen *= 9;
      sMainScreen /= 10;

      if (r.width() > sMainScreen.width() || r.height() > sMainScreen.height()
         || r.width() < 300 || r.height() < 300)
      {

         sMainScreen *= 3;
         sMainScreen /= 5;

         r.set_size(sMainScreen);
         r.left = sMainScreen.width()/4;
         r.top = sMainScreen.height()/4;


         set_window_position(
            ::as_operating_system_window((HWND)HWND_TOP),
            r.origin(),
            r.size(),
            SWP_SHOWWINDOW 
               | SWP_FRAMECHANGED);

      }

      update_style();

      m_ptoolbar->m_bMouseEnable = true;

   }


   ::windows::in_place_frame *main_window::in_place_frame()
   {

      return m_prdphost->m_pclientsite->m_pinplacesite->m_pinplaceframe;

   }



   void main_window::update_scrollbars()
   {

      auto rectangleClient = get_client_rectangle();

      ::i32_rectangle rectangleMainScreen = get_main_screen_size();

      SCROLLINFO si{};
      si.cbSize = sizeof(si);
      si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;

      auto hwnd = ::as_HWND(this->operating_system_window());

      // Horizontal
      si.nMin = 0;
      si.nMax = rectangleMainScreen.width() - 1;
      si.nPage = rectangleClient.right;
      si.nPos = m_pointScroll.x;

      SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);

      // Vertical
      si.nMin = 0;
      si.nMax = rectangleMainScreen.height() - 1;
      si.nPage = rectangleClient.bottom;
      si.nPos = m_pointScroll.y;

      SetScrollInfo(hwnd, SB_VERT, &si, TRUE);

   }


   void main_window::doFullscreen()
   {

      m_bFullscreen = true;

      m_bFullscreenStored = true;

      //set_window_style(WS_POPUP);

      update_style();

      auto sMainScreen = get_main_screen_size();

      ::i32_rectangle rFullscreen(sMainScreen);

      if (is_window_iconic())
      {

         show_window(SW_SHOWNORMAL);

      }

      m_pointScroll.clear();

      layout_rdp_host();

      set_window_position(
         ::as_operating_system_window((HWND) HWND_TOP), 
         rFullscreen.origin(),
         rFullscreen.size(), 
         SWP_SHOWWINDOW 
            | SWP_FRAMECHANGED);

      m_ptoolbar->m_bMouseEnable = true;

      m_ptoolbar->show_window(SW_SHOW);

      m_prdphost->onEnterFullscreen();

   }


   i32_size main_window::get_main_screen_size()
   {

      ::i32 screenWidth = GetSystemMetrics(SM_CXSCREEN);
      ::i32 screenHeight = GetSystemMetrics(SM_CYSCREEN);

      return {screenWidth, screenHeight};

   }


} // namespace remoting_rdx_clinet



