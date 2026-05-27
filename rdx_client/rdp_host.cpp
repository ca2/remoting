#include "framework.h"
#include "rdp_host.h"
#include "main_window.h"
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
//#include "rdp_host.h"
#include "toolbar.h"

#import "mstscax.dll" raw_native_types no_namespace named_guids

#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")



namespace remoting_rdx_client
{


   namespace rdp
   {

      enum enum_dispatch
      {

         DISPID_CONNECTING = 1, // OnConnecting
         DISPID_CONNECTED = 2, // OnConnected
         DISPID_LOGINCOMPLETE = 3, // OnLoginComplete
         DISPID_DISCONNECTED = 4, // OnDisconnected
         DISPID_ENTERFULLSCREENMODE = 5, // OnEnterFullScreenMode
         DISPID_LEAVEFULLSCREENMODE = 6, // OnLeaveFullScreenMode
         DISPID_CHANNELRECEIVEDDATA = 7, // OnChannelReceivedData
         DISPID_REQUESTGOFULLSCREEN = 8, // OnRequestGoFullScreen
         DISPID_REQUESTLEAVEFULLSCREEN = 9, // OnRequestLeaveFullScreen
         DISPID_FATALERROR = 10, // OnFatalError
         DISPID_WARNING = 11, // OnWarning
         DISPID_REMOTEDESKTOPSIZECHANGE = 12, // OnRemoteDesktopSizeChange
         DISPID_IDLETIMEOUTNOTIFICATION = 13, // OnIdleTimeoutNotification
         DISPID_REQUESTCONTAINERMINIMIZE = 14, // OnRequestContainerMinimize
         DISPID_CONFIRMCLOSE = 15, // OnConfirmClose
         DISPID_RECEIVEDTSPUBLICKEY = 16, // OnReceivedTSPublicKey
         DISPID_AUTORECONNECTING = 17, // OnAutoReconnecting
         DISPID_INTERNALDIALOGDISPLAYED = 18, // OnAuthenticationWarningDisplayed
         DISPID_INTERNALDIALOGDISMISSED = 19, // OnAuthenticationWarningDismissed
         DISPID_ONREMOTEPROGRAMRESULT = 20, // OnRemoteProgramResult
         DISPID_ONREMOTEPROGRAMDISPLAYED = 21, // OnRemoteProgramDisplayed
         DISPID_LOGONERROR = 22, // OnLogonError
         DISPID_FOCUSRELEASED = 23, // OnFocusReleased
         DISPID_USERNAMEACQUIRED = 24, // OnUserNameAcquired
         DISPID_MOUSEINPUTMODECHANGED = 26, // OnMouseInputModeChanged
         DISPID_ONSTATUSINFO = 27, // None. Provides a VT_UI4 status code to the application. The application can pass
                                   // the status code contained in `pDispParams->rgvarg[0].ulVal`` to
                                   // IMsRdpClient7::GetStatusText to get the associated status text.
         DISPID_SERVICEMESSAGERECEIVED = 28, // OnServiceMessageReceived
         DISPID_ONREMOTEWINDOWDISPLAYED = 29, // OnRemoteWindowDisplayed
         DISPID_CONNECTIONBARPULLDOWN = 30, // OnConnectionBarPullDown
         DISPID_ONNETWORKSTATUSCHANGED = 32, // OnNetworkStatusChanged
         DISPID_AUTORECONNECTED = 33, // OnAutoReconnected
         DISPID_AUTORECONNECTING2 = 34, // OnAutoReconnecting2
         DISPID_CONNECTIONBARDEVICES = 35, // OnDevicesButtonPressed
         DISPID_HVSINOTIFICATION =
            36, // None. Provides a VT_UI4 event code. This event is emitted in Microsoft Defender Application Guard
                // scenarios. It should not be emitted during normal operations.
         DISPID_ONWOKEUPANDRECONNECTING =
            37, // None. This event is no longer emitted by the Remote Desktop ActiveX control.
         DISPID_ONLOCATIONREDIRECTIONENABLEDRECEIVED =
            39, // None. This event notifies the application that it can start using
                // IMsRdpClientNonScriptable6::SendLocation2D or IMsRdpClientNonScriptable6::SendLocation3D its location
                // to the server so the client's geographic location can be reflected in the remote session.

      };
   } // namespace rdp

   class  rdp_host_internal :
      virtual public ::windows::event_sink<IMsTscAxEvents>
   {
   public:

      ::pointer<rdp_host> m_pmainwindow;
      ::comptr<IMsRdpClient9> m_prdpclient;
      ::comptr<IMsRdpClientAdvancedSettings7> m_padvancedsettings;
      ::comptr < IMsRdpExtendedSettings > m_pextendedsettings;
      ::comptr<IMsRdpClientSecuredSettings> m_psecuredsettings;



         HRESULT STDMETHODCALLTYPE Invoke(DISPID dispIdMember, REFIID, LCID, WORD, DISPPARAMS *pDispParams, VARIANT *,
                                       EXCEPINFO *, UINT *)
      {

            auto edispatch = (rdp::enum_dispatch)dispIdMember;
            switch (edispatch)
            {
               case rdp::DISPID_CONNECTING:
               information("RDP Happening : Connecting");
               m_pmainwindow->OnRdpConnecting();
               break;
               case rdp::DISPID_CONNECTED:
               information("RDP Happening : Connected");
               m_pmainwindow->OnRdpConnected();
               break;
               case rdp::DISPID_LOGINCOMPLETE:
                  information("RDP Happening : Login Complete");
                  m_pmainwindow->OnRdpLoginComplete();
                  break;
               case rdp::DISPID_DISCONNECTED:
                  information("RDP Happening : Disconnected");
                  m_pmainwindow->OnRdpDisconnected();
                  break;
               case rdp::DISPID_ENTERFULLSCREENMODE:
                  information("RDP Happening : Enter full screen");
                  m_pmainwindow->OnRdpEnterFullScreen();
                  break;
               case rdp::DISPID_LEAVEFULLSCREENMODE:
                  information("RDP Happening : Leave full screen");
                  m_pmainwindow->OnRdpLeaveFullScreen();
                  break;
               case rdp::DISPID_REQUESTGOFULLSCREEN:
                  information("RDP Happening : Request go full screen");
                  m_pmainwindow->OnRdpRequestGoFullScreen();
                  break;
               case rdp::DISPID_REQUESTLEAVEFULLSCREEN:
                  information("RDP Happening : Request leave full screen");
                  m_pmainwindow->OnRdpRequestLeaveFullScreen();
                  break;

            }
         return S_OK;
      }



   };


   //::pointer<::windows::com_window> create_rdp_host(::particle * pparticle)
   //{

   //   return pparticle->create_newø<rdp_host>();

   //}

   // HHOOK g_mouseHook;
   //
   // LRESULT CALLBACK MouseProc(::i32 nCode, WPARAM wParam, LPARAM lParam)
   // {
   //    if (nCode == HC_ACTION)
   //    {
   //       MSLLHOOKSTRUCT* info = (MSLLHOOKSTRUCT*)lParam;
   //
   //       POINT pt = info->pt;
   //
   //       // send to toolbar directly
   //       g_toolbar->OnGlobalMouse(pt.x, pt.y, wParam);
   //    }
   //
   //    return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
   // }
   //


   //HWND hwnd = nullptr;
   //IOleObject *m_poleobject = nullptr;
   //IMsRdpClient9 *m_pinternal->m_prdpclient = nullptr;


   //IConnectionPoint *m_pconnectionpoint = nullptr;
   //DWORD m_dwCookie = 0;
   //RdpEventSink *m_peventsink = nullptr;


   rdp_host::rdp_host()
   {

      m_pinternal = new rdp_host_internal;

      m_pinternal->m_pmainwindow = this;

      //m_dwCookie = 0;

      m_dBreathPeriod = 60.0;

      m_timeStart.Now();

      m_dPhaseShift = 0.0;

      //m_bCustomPaint = true;

   }


   rdp_host::~rdp_host()
   {

      if (m_pinternal)
      {

         delete m_pinternal;

         m_pinternal = nullptr;

      }
   }


   i64 rdp_host::get_style_for_creating_window()
   {

      //return WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
      //return WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
      return WS_CHILD |
      WS_VISIBLE |
      WS_CLIPCHILDREN |
      WS_CLIPSIBLINGS;
   }


   bool rdp_host::_on_window_procedure(lresult &lresult, u32 message, wparam wparam, lparam lparam)
   {

      switch (message)
      {
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

         case WM_SIZE:
         {
               RECT rcClient;
               RECT rcWindow;

               auto hwnd = ::as_HWND(this->operating_system_window());

               GetClientRect(hwnd, &rcClient);

               ::i32_rectangle rectangleClient(rcClient);

               information("rdp_host WM_SIZE Client {}", rectangleClient);

               GetWindowRect(hwnd, &rcWindow);

               ::i32_rectangle rectangleWindow(rcWindow);

               information("rdp_host WM_SIZE Window {} {}x{}", rectangleWindow, rectangleWindow.width(), rectangleWindow.height());

               MapWindowPoints(nullptr, hwnd, (POINT*)&rcWindow, 2);

               ::i32_rectangle rectangleWindow2 = rcWindow;

               information("rdp_host WM_SIZE MapWindowPoints {}", rectangleWindow2);

            break;
         }
            break;
         case WM_TIMER:
         {

            // if (wparam == 1453)
            // {
            //
            //    show_window(SW_SHOW);
            //
            //    auto hwnd = ::as_HWND(this->operating_system_window());
            //    KillTimer(hwnd, 1453);
            //
            // }

         }
            break;
         case WM_SYSCOMMAND:
         {
            // if ((wparam.loword() & 0xFFF0) == SC_MAXIMIZE)
            // {
            //    //m_pinternal->m_prdpclient->put_FullScreen(VARIANT_TRUE);
            //    doFullscreen();
            //    lresult= 0; // 👈 block normal maximize
            //    return true;
            // }
            // else if ((wparam.loword() & 0xFFF0) == SC_RESTORE)
            // {
            //    if (m_bFullscreenStored)
            //    {
            //       //m_pinternal->m_prdpclient->put_FullScreen(VARIANT_TRUE);
            //       doFullscreen();
            //    }
            //    else
            //    {
            //       doRestore();
            //
            //    }
            //    lresult= 0; // 👈 block normal maximize
            //    return true;
            // }

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

      }
      return false;


   }


   void rdp_host::shutdown_rdp()
   {


      show_window(SW_HIDE);

      if (m_pinternal &&
          m_pinternal->m_prdpclient)
      {
         ::i16 connected = 0;

         m_pinternal->m_prdpclient->get_Connected(&connected);

         if (connected != 0)
         {
            m_pinternal->m_prdpclient->Disconnect();
         }
      }

      if (m_pinternal)
      {

         m_pinternal->close_event_sink();

      }

      if (m_poleobject)
      {

         m_poleobject->Close(OLECLOSE_NOSAVE);

      }

      m_pclientsite.release();
      m_poleobject.release();

      if (m_pinternal)
      {
         delete m_pinternal;
         m_pinternal = nullptr;
      }

   }


   string rdp_host::get_title()
   {

      return "Remoting RDX Client";

   }


   void rdp_host::on_create_window()
   {

      m_rclsid = CLSID_MsRdpClient9NotSafeForScripting;

      com_window::on_create_window();

      auto hr = m_poleobject.as(m_pinternal->m_prdpclient);

      auto hwnd = ::as_HWND(this->operating_system_window());

      if (FAILED(hr))
      {

         MessageBox(hwnd, L"QueryInterface failed", L"Error", MB_ICONERROR);

         return;

      }

      ::comptr < IConnectionPointContainer > pconnectionpointcontainer;

      m_pinternal->m_prdpclient.as(pconnectionpointcontainer);

      m_pinternal->event_sink<IMsTscAxEvents>::initialize_event_sink(pconnectionpointcontainer);
       
      ::cast<::remoting_rdx_client::application> papp = m_papplication;

      ::wstring wstrHost = papp->m_strHost;

      m_pinternal->m_prdpclient->put_Server(_bstr_t(wstrHost));



      m_pinternal->m_prdpclient->get_AdvancedSettings8(&m_pinternal->m_padvancedsettings);

      if (m_pinternal->m_padvancedsettings)
      {

         m_pinternal->m_padvancedsettings->put_EnableCredSspSupport(VARIANT_TRUE);

         //padvancedsettings->put_ContainerHandledFullScreen(VARIANT_TRUE);

         m_pinternal->m_padvancedsettings->put_SmartSizing(VARIANT_FALSE);

         m_pinternal->m_padvancedsettings->put_EnableAutoReconnect(VARIANT_FALSE);

         m_pinternal->m_padvancedsettings->put_SmartSizing(VARIANT_FALSE);

         m_pinternal->m_padvancedsettings->put_ContainerHandledFullScreen(1);

         m_pinternal->m_padvancedsettings->put_DisplayConnectionBar(VARIANT_FALSE);
         //}
         //m_pinternal->m_padvancedsettings->put_BitmapCacheSize(0);

         //padvancedsettings->put_BitmapPersistence(VARIANT_FALSE);

         ///padvancedsettings->put_PerformanceFlags(0);

         //padvancedsettings->put_ConnectionBarShowMinimizeButton(VARIANT_FALSE);

      }

      //::comptr < IMsRdpExtendedSettings > ext;

      m_pinternal->m_prdpclient.as(m_pinternal->m_pextendedsettings);

      if (m_pinternal->m_pextendedsettings)
      {
         _variant_t vFalse(VARIANT_FALSE);

         m_pinternal->m_pextendedsettings->put_Property(
             _bstr_t(L"EnableHardwareMode"),
             &vFalse);

         m_pinternal->m_pextendedsettings->put_Property(
             _bstr_t(L"UseURCP"),
             &vFalse);

         m_pinternal->m_pextendedsettings->put_Property(
             _bstr_t(L"DisableUDPTransport"),
             &vFalse);

         m_pinternal->m_pextendedsettings->put_Property(
            _bstr_t(L"EnableConnectionBar"),
            &vFalse);

         _variant_t varScaleFactor((LONG)100);

         m_pinternal->m_pextendedsettings->put_Property(_bstr_t(L"DesktopScaleFactor"), &varScaleFactor);

         m_pinternal->m_pextendedsettings->put_Property(_bstr_t(L"DeviceScaleFactor"), &varScaleFactor);

      }

      m_pinternal->m_prdpclient->get_SecuredSettings2(&m_pinternal->m_psecuredsettings);

      m_pinternal->m_prdpclient->put_ColorDepth(32);

      ::i32 screenWidth = GetSystemMetrics(SM_CXSCREEN);

      ::i32 screenHeight = GetSystemMetrics(SM_CYSCREEN);

      m_pinternal->m_prdpclient->put_DesktopWidth(screenWidth);

      m_pinternal->m_prdpclient->put_DesktopHeight(screenHeight);

      auto hrConnect = m_pinternal->m_prdpclient->Connect();

   }


   void rdp_host::on_window_paint(::nano::graphics::context *pgraphicscontext)
   {


   }


   void rdp_host::OnRdpConnecting()
   {


   }


   void rdp_host::OnRdpConnected()
   {


   }


   void rdp_host::OnRdpLoginComplete()
   {

      m_pmainwindow->doFullscreen();

   }


   void rdp_host::OnRdpDisconnected()
   {

      m_pmainwindow->shutdown_rdp();

   }


   void rdp_host::OnRdpEnterFullScreen()
   {

      m_pmainwindow->OnRdpEnterFullScreen();

   }


   void rdp_host::OnRdpLeaveFullScreen()
   {

      m_pmainwindow->OnRdpLeaveFullScreen();

   }


   void rdp_host::OnRdpRequestGoFullScreen()
   {

      m_pmainwindow->OnRdpRequestGoFullScreen();

   }


   void rdp_host::OnRdpRequestLeaveFullScreen()
   {

      m_pmainwindow->OnRdpRequestLeaveFullScreen();

   }


   void rdp_host::MinimizeRdp()
   {

      m_pmainwindow->doMinimize();

   }


   void rdp_host::RestoreRdp()
   {

      m_pmainwindow->doRestore();

   }


   void rdp_host::CloseRdp()
   {

      m_pmainwindow->shutdown_rdp();

   }


      void rdp_host::do_control_layout()
   {

      // if (!m_poleobject)
      // {
      //
      //    return;
      //
      // }

      //::comptr<IOleInPlaceObject> pinplaceobject;

      //m_poleobject.as(pinplaceobject);

      if (!m_pclientsite)
      {

         return;

      }

      if (!m_pclientsite->m_pinplacesite->m_pinplaceframe->m_pinplaceobject)
      {

         information("No IOleInPlaceObject");

         return;

      }

      auto hwnd = ::as_HWND(this->operating_system_window());

      RECT rc;

      auto sizeMainScreen = m_pmainwindow->get_main_screen_size();

      ::i32_rectangle rectangleMainScreen(sizeMainScreen);

      if (rectangleMainScreen != m_rectanglePosition)
      {

         m_rectanglePosition = rectangleMainScreen;

         //GetClientRect(hwnd, &rc);
         copy(rc, rectangleMainScreen);

         HRESULT hr =
             m_pclientsite->m_pinplacesite->m_pinplaceframe->m_pinplaceobject->SetObjectRects(&rc, &rc);

         ::string strMessage;

         strMessage.formatf(
       "SetObjectRects: %d x %d;",
       rc.right - rc.left,
       rc.bottom - rc.top);

         ::i32 iStyle = get_window_style();

         strMessage.append_formatf(" style=%08x;", iStyle);

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

         if (FAILED(hr))
         {

            information(hresult_to_string(hr));

         }

         // RedrawWindow(
         //     hwnd,
         //     nullptr,
         //     nullptr,
         //     RDW_INVALIDATE |
         //     RDW_UPDATENOW |
         //     RDW_ALLCHILDREN);
      }

   }


   // void com_window::control_activate(bool bActivate)
   // {
   //
   //    if (m_pclientsite)
   //    {
   //       if (m_pclientsite->m_pinplacesite->m_pinplaceframe->m_pinplaceactiveobject)
   //       {
   //
   //          m_pclientsite->m_pinplacesite->m_pinplaceframe->m_pinplaceactiveobject->OnFrameWindowActivate(bActivate ? TRUE : FALSE);
   //
   //          m_pclientsite->m_pinplacesite->m_pinplaceframe->m_pinplaceactiveobject->OnDocWindowActivate(bActivate ? TRUE : FALSE);
   //
   //       }
   //    }
   //
   // }


   void rdp_host::onEnterFullscreen()
   {

      if(m_pinternal->m_psecuredsettings)
      {
         auto hresult = m_pinternal->m_psecuredsettings->put_KeyboardHookMode(1);

         if (FAILED(hresult))
         {

            information("Failed to set keyboard hook mode to 1");

         }

      }


//      m_pinternal->m_prdpclient->put_FullScreen(VARIANT_TRUE);

      m_pinternal->m_padvancedsettings->put_DisplayConnectionBar(VARIANT_FALSE);

   }


   void rdp_host::onExitFullscreen()
   {

      if(m_pinternal->m_psecuredsettings)
      {
         auto hresult = m_pinternal->m_psecuredsettings->put_KeyboardHookMode(1);

         if (FAILED(hresult))
         {

            information("Failed to set keyboard hook mode to 1");

         }

      }


      //m_pinternal->m_prdpclient->put_FullScreen(VARIANT_FALSE);

      m_pinternal->m_padvancedsettings->put_DisplayConnectionBar(VARIANT_FALSE);

   }




} // namespace remoting_rdx_clinet



