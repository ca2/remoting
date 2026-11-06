#include "platform.h"
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
#include "acme/nano/idn/idn.h"
#include "acme/nano/nano.h"
#include "acme/user/interface/message_box.h"
//#include "rdp_host.h"
#include "toolbar.h"
#include <wincred.h>

#import "mstscax.dll" raw_native_types no_namespace named_guids

#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")
#pragma comment(lib, "credui.lib")
#pragma comment(lib, "advapi32.lib")



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
      bool prepare_credentials(HWND hwnd, const ::string & strHost, int iPort, bool bPrompt)
      {

         ::string strTarget;
         strTarget.formatf("remoting_rdx/%s:%d", strHost.c_str(), iPort > 0 ? iPort : 3389);
         ::wstring wstrTarget(strTarget);

         struct credential_buffers
         {
            WCHAR username[CREDUI_MAX_USERNAME_LENGTH + 1] = {};
            WCHAR domain[CREDUI_MAX_USERNAME_LENGTH + 1] = {};
            WCHAR password[CREDUI_MAX_PASSWORD_LENGTH + 1] = {};
            ~credential_buffers() { SecureZeroMemory(this, sizeof(*this)); }
         } credentials;

         struct stored_credential
         {
            PCREDENTIALW value = nullptr;
            ~stored_credential()
            {
               if (value)
               {
                  if (value->CredentialBlob)
                     SecureZeroMemory(value->CredentialBlob, value->CredentialBlobSize);
                  CredFree(value);
               }
            }
         } stored;

         bool bHaveCredentials = false;
         if (!bPrompt)
         {

            if (CredReadW(wstrTarget, CRED_TYPE_GENERIC, 0, &stored.value))
            {

               auto bytes = stored.value->CredentialBlobSize;
               if (!stored.value->UserName ||
                   bytes % sizeof(WCHAR) != 0 || bytes >= sizeof(credentials.password) ||
                   (bytes && !stored.value->CredentialBlob) ||
                   wcscpy_s(credentials.username, stored.value->UserName) != 0)
               {

                  throw ::hresult_exception(HRESULT_FROM_WIN32(ERROR_INVALID_DATA));

               }

               if (bytes)
                  memcpy(credentials.password, stored.value->CredentialBlob, bytes);
               // Match the domain/username split used by the Windows dialog.
               auto separator = wcschr(credentials.username, L'\\');
               if (separator)
               {
                  auto domainLength = separator - credentials.username;
                  wcsncpy_s(credentials.domain, credentials.username, domainLength);
                  memmove(credentials.username, separator + 1,
                     (wcslen(separator + 1) + 1) * sizeof(WCHAR));
               }
               bHaveCredentials = true;

            }
            else
            {

               auto error = GetLastError();
               if (error != ERROR_NOT_FOUND)
                  throw ::hresult_exception(HRESULT_FROM_WIN32(error));

            }

         }

         if (!bHaveCredentials)
         {

            CREDUI_INFOW ui = {};
            ui.cbSize = sizeof(ui);
            ui.hwndParent = hwnd;
            ui.pszCaptionText = L"Remoting RDX credentials";
            ui.pszMessageText = wstrTarget;
            BOOL save = FALSE;
            ULONG authPackage = 0;
            struct authentication_buffer
            {
               void * data = nullptr;
               ULONG size = 0;
               ~authentication_buffer()
               {
                  if (data)
                  {
                     SecureZeroMemory(data, size);
                     CoTaskMemFree(data);
                  }
               }
            } auth;

            auto result = CredUIPromptForWindowsCredentialsW(&ui, 0, &authPackage,
               nullptr, 0, &auth.data, &auth.size, &save,
               CREDUIWIN_GENERIC | CREDUIWIN_CHECKBOX);
            if (result == ERROR_CANCELLED)
               return false;
            if (result != NO_ERROR)
               throw ::hresult_exception(HRESULT_FROM_WIN32(result));

            DWORD usernameLength = _countof(credentials.username);
            DWORD domainLength = _countof(credentials.domain);
            DWORD passwordLength = _countof(credentials.password);
            if (!CredUnPackAuthenticationBufferW(0, auth.data, auth.size,
               credentials.username, &usernameLength, credentials.domain, &domainLength,
               credentials.password, &passwordLength))
            {

               throw ::hresult_exception(HRESULT_FROM_WIN32(GetLastError()));

            }

            // The modern dialog returns the checkbox state; it does not save.
            // Persist on explicit consent, independently of server login events.
            if (save)
            {

               ::wstring wstrUsername;
               if (credentials.domain[0])
               {
                  wstrUsername = credentials.domain;
                  wstrUsername += L"\\";
               }
               wstrUsername += credentials.username;
               CREDENTIALW entry = {};
               entry.Type = CRED_TYPE_GENERIC;
               entry.TargetName = const_cast<LPWSTR>(static_cast<LPCWSTR>(wstrTarget));
               entry.UserName = const_cast<LPWSTR>(static_cast<LPCWSTR>(wstrUsername));
               entry.CredentialBlob = reinterpret_cast<LPBYTE>(credentials.password);
               entry.CredentialBlobSize = static_cast<DWORD>(wcslen(credentials.password) * sizeof(WCHAR));
               entry.Persist = CRED_PERSIST_LOCAL_MACHINE;
               if (!CredWriteW(&entry, 0))
                  throw ::hresult_exception(HRESULT_FROM_WIN32(GetLastError()));

            }

         }

         ::comptr<IMsRdpClientNonScriptable5> pcredentials;
         auto hr = m_prdpclient.as(pcredentials);
         if (SUCCEEDED(hr))
            hr = pcredentials->put_AllowPromptingForCredentials(VARIANT_FALSE);
         if (SUCCEEDED(hr))
            hr = m_prdpclient->put_Domain(_bstr_t(credentials.domain));
         if (SUCCEEDED(hr))
            hr = m_prdpclient->put_UserName(_bstr_t(credentials.username));
         if (SUCCEEDED(hr))
         {

            BSTR password = SysAllocString(credentials.password);
            if (!password)
            {
               hr = E_OUTOFMEMORY;
            }
            else
            {
               hr = pcredentials->put_ClearTextPassword(password);
               SecureZeroMemory(password, SysStringByteLen(password));
               SysFreeString(password);
            }

         }
         if (FAILED(hr))
            throw ::hresult_exception(hr);

         return true;

      }


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
               {
                  ::i32 reason = 0;
                  if (pDispParams && pDispParams->cArgs >= 1 && pDispParams->rgvarg[0].vt == VT_I4)
                     reason = pDispParams->rgvarg[0].lVal;
                  informationf("RDP disconnected: reason=%d", reason);
                  m_pmainwindow->OnRdpDisconnected(reason);
                  break;
               }
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

      m_pinternal = nullptr;

      //m_dwCookie = 0;

      m_dBreathPeriod = 60.0;

      m_timeStart.Now();

      m_dPhaseShift = 0.0;

      //m_bCustomPaint = true;

   }


   rdp_host::~rdp_host()
   {

      //if (m_pinternal)
      //{

      //   delete m_pinternal;

      //   m_pinternal = nullptr;

      //}

   }


   void rdp_host::initialize(::particle* pparticle)
   {

      ::windows::com_window::initialize(pparticle);

      m_pinternal = create_newø<rdp_host_internal>();

      m_pinternal->m_pmainwindow = this;

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


      show_window(::user_interface::e_show_window_hide);

      // Detach callbacks before Disconnect can notify us synchronously.
      if (m_pinternal)
      {

         m_pinternal->close_event_sink();

      }

      if (m_pinternal)
      {

         if (m_pinternal->m_padvancedsettings)
         {
            m_pinternal->m_padvancedsettings.release();
         }
         if (m_pinternal->m_pextendedsettings)
         {
            m_pinternal->m_pextendedsettings.release();
         }
         if (m_pinternal->m_psecuredsettings)
         {
            m_pinternal->m_psecuredsettings.release();
         }
      }

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

      if (m_poleobject)
      {

         m_poleobject->Close(OLECLOSE_NOSAVE);
         m_poleobject->SetClientSite(nullptr);

      }

      if (m_pclientsite && m_pclientsite->m_pinplacesite &&
          m_pclientsite->m_pinplacesite->m_pinplaceframe)
      {

         auto pframe = m_pclientsite->m_pinplacesite->m_pinplaceframe;
         pframe->m_pinplaceactiveobject.release();
         pframe->m_pinplaceobject.release();
         pframe->m_pcomwindow.release();

      }

      m_pclientsite.release();

      m_poleobject.release();

      if (m_pinternal)
      {

         m_pinternal->m_prdpclient.release();
         m_pinternal->m_pmainwindow.release();
         m_pinternal.release();

      }

   }


   string rdp_host::get_title()
   {

      return "Remoting RDX Client";

   }


   bool rdp_host::on_window_create(::user::window_create_t * pwindowcreate)
   {


      m_rclsid = CLSID_MsRdpClient9NotSafeForScripting;

      if (!::windows::com_window::on_window_create(pwindowcreate))
      {

         return false;

      }

      auto hr = m_poleobject.as(m_pinternal->m_prdpclient);

      auto hwnd = ::as_HWND(this->operating_system_window());

      if (FAILED(hr))
      {

         auto pmessagebox = message_box("QueryInterface failed", L"Error");

         pmessagebox->display(::e_display_normal, {});

         pmessagebox->wait_for_dialog_response();

         return false;

      }

      ::comptr < IConnectionPointContainer > pconnectionpointcontainer;

      m_pinternal->m_prdpclient.as(pconnectionpointcontainer);

      m_pinternal->event_sink<IMsTscAxEvents>::initialize_event_sink(pconnectionpointcontainer);
       
      ::cast<::remoting_rdx_client::application> papp = m_papplication;

      if (papp->m_bUseRedirectionServerName)
      {

         ::comptr < IMsRdpPreferredRedirectionInfo > predirectioninfo;
         auto hrRedirection = m_pinternal->m_prdpclient.as(predirectioninfo);

         if (SUCCEEDED(hrRedirection))
         {

            hrRedirection = predirectioninfo->put_UseRedirectionServerName(VARIANT_TRUE);

         }

         if (FAILED(hrRedirection))
         {

            throw ::hresult_exception(hrRedirection);

         }

      }

      auto strParse = papp->m_strHost;

      auto strName = strParse.get_word(":");

      ::string strPort(strParse);

      int iPort = -1;

      if (strPort.has_character())
      {

         iPort = atoi(strPort);

      }

      auto strPunycode = nano()->idn()->idn_to_punycode(strName);
      strPunycode.make_lower();

      ::wstring wstrHost(strPunycode);

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
         if (iPort > 0)
         {
            m_pinternal->m_padvancedsettings->put_RDPPort(iPort);
         }


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

      if (papp->m_bUseWindowsCredentialManager &&
          !m_pinternal->prepare_credentials(hwnd, strPunycode, iPort, papp->m_bPromptForCredentials))
      {

         papp->set_finish();
         return false;

      }

      auto hrConnect = m_pinternal->m_prdpclient->Connect();
      if (FAILED(hrConnect))
      {

         throw ::hresult_exception(hrConnect);

      }

      return true;

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


   void rdp_host::OnRdpDisconnected(::i32 reason)
   {

      if (m_pmainwindow->m_bIsClosing)
         return;

      ExtendedDisconnectReasonCode extended = static_cast<ExtendedDisconnectReasonCode>(0);
      m_pinternal->m_prdpclient->get_ExtendedDisconnectReason(&extended);
      BSTR description = nullptr;
      m_pinternal->m_prdpclient->raw_GetErrorDescription(reason, extended, &description);
      ::string message;
      message.formatf("Remote Desktop disconnected (reason %d, extended %u).", reason,
         static_cast<unsigned int>(extended));
      if (description)
      {
         message += "\n\n";
         message += ::string(description);
         SysFreeString(description);
      }
      information(message);
      if (reason != 1 && reason != 2)
         m_pmainwindow->m_strDisconnectMessage = message;

      // Report and close only after returning from the ActiveX callback.
      auto hwnd = ::as_HWND(m_pmainwindow->operating_system_window());
      ::PostMessageW(hwnd, WM_APP + 101, 0, 0);

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

         auto iStyle = get_window_style();

         strMessage.append_formatf(" style=%016x;", iStyle);

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



