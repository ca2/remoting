#include "framework.h"
#include "application.h"
#include "main_window.h"
#include "acme/filesystem/filesystem/file_context.h"
#include "acme/handler/request.h"
#include "acme/platform/node.h"
#include "acme/platform/system.h"

#include <windows.h>
#include <wincred.h>
#include <iostream>
#include <string>

#include <freerdp/freerdp.h>
#include <freerdp/gdi/gdi.h>
#include <freerdp/input.h>
#include <freerdp/settings.h>

#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "Advapi32.lib")

freerdp* g_instance = nullptr;
HWND g_hwnd = nullptr;

__IMPLEMENT_APPLICATION_RELEASE_TIME(remoting_remote_connect);
IMPLEMENT_APPLICATION_FACTORY(remoting_remote_connect);


namespace remoting_remote_connect
{


   // __IMPLEMENT_APPLICATION_RELEASE_TIME1(app_app));



   // minimal_rdp_client.cpp



// -------------------- Credential Manager --------------------

bool LoadCredentials(const std::wstring& target, std::string& user, std::string& pass)
{
    PCREDENTIALW cred = nullptr;

    if (!CredReadW(target.c_str(), CRED_TYPE_GENERIC, 0, &cred))
        return false;

    if (cred->UserName)
    {
        ::i32 len = WideCharToMultiByte(CP_UTF8, 0, cred->UserName, -1, NULL, 0, NULL, NULL);
        user.resize(len);
        WideCharToMultiByte(CP_UTF8, 0, cred->UserName, -1, user.data(), len, NULL, NULL);
    }

    pass.assign((char_pointer )cred->CredentialBlob, cred->CredentialBlobSize);

    CredFree(cred);
    return true;
}

// -------------------- FreeRDP hooks --------------------

BOOL pre_connect(freerdp* instance)
{
    return TRUE;
}

BOOL post_connect(freerdp* instance)
{
    return gdi_init(instance, PIXEL_FORMAT_BGRA32);
}

// -------------------- Rendering --------------------

void PaintRdp(HDC hdc)
{
    if (!g_instance || !g_instance->context || !g_instance->context->gdi)
        return;

    gdiContext* gdi = g_instance->context->gdi;

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = gdi->width;
    bmi.bmiHeader.biHeight = -((LONG)gdi->height);
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;

    StretchDIBits(
        hdc,
        0, 0, gdi->width, gdi->height,
        0, 0, gdi->width, gdi->height,
        gdi->primary_buffer,
        &bmi,
        DIB_RGB_COLORS,
        SRCCOPY
    );
}

// -------------------- Window Proc --------------------

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        PaintRdp(hdc);
        EndPaint(hwnd, &ps);
        break;
    }

    case WM_MOUSEMOVE:
    {
        if (g_instance && g_instance->input)
            freerdp_input_send_mouse_event(
                g_instance->input,
                PTR_FLAGS_MOVE,
                LOWORD(lParam), HIWORD(lParam));
        break;
    }

    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    {
        if (g_instance && g_instance->input)
        {
            ::u16 flags = 0;

            switch (msg)
            {
            case WM_LBUTTONDOWN: flags = PTR_FLAGS_DOWN | PTR_FLAGS_BUTTON1; SetCapture(hwnd); break;
            case WM_LBUTTONUP:   flags = PTR_FLAGS_BUTTON1; ReleaseCapture(); break;
            case WM_RBUTTONDOWN: flags = PTR_FLAGS_DOWN | PTR_FLAGS_BUTTON2; break;
            case WM_RBUTTONUP:   flags = PTR_FLAGS_BUTTON2; break;
            }

            freerdp_input_send_mouse_event(
                g_instance->input,
                flags,
                LOWORD(lParam), HIWORD(lParam));
        }
        break;
    }

    case WM_MOUSEWHEEL:
    {
        if (g_instance && g_instance->input)
        {
            ::i16 delta = GET_WHEEL_DELTA_WPARAM(wParam);
            ::u16 flags = PTR_FLAGS_WHEEL | (delta < 0 ? PTR_FLAGS_WHEEL_NEGATIVE : 0);
            flags |= (abs(delta) & 0xFF);

            freerdp_input_send_mouse_event(g_instance->input, flags, 0, 0);
        }
        break;
    }

    case WM_KEYDOWN:
    case WM_KEYUP:
    {
        if (g_instance && g_instance->input)
        {
            UINT scancode = (lParam >> 16) & 0xFF;
            BOOL ext = (lParam >> 24) & 1;

            ::u16 flags = ext ? KBD_FLAGS_EXTENDED : 0;
            if (msg == WM_KEYUP)
                flags |= KBD_FLAGS_RELEASE;

            freerdp_input_send_keyboard_event(
                g_instance->input,
                flags,
                scancode);
        }
        break;
    }

    case WM_SETFOCUS:
        if (g_instance)
            freerdp_input_send_focus_in_event(g_instance->input, 0);
        break;

    case WM_KILLFOCUS:
        if (g_instance)
            freerdp_input_send_focus_in_event(g_instance->input, 1);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// -------------------- Main --------------------

::i32 main_freerdp()
{
    std::string user, pass;

    if (!LoadCredentials(L"canada.camilothomas.com", user, pass))
    {
        MessageBoxA(NULL, "No stored credentials found", "Error", MB_OK);
        return -1;
    }

    // Window
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"RDPClient";

    RegisterClass(&wc);

    g_hwnd = CreateWindow(
        wc.lpszClassName,
        L"Mini RDP Client",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        1280, 800,
        NULL, NULL, wc.hInstance, NULL);

    ShowWindow(g_hwnd, SW_SHOW);

    // FreeRDP
    g_instance = freerdp_new();
    freerdp_context_new(g_instance);

    g_instance->PreConnect = pre_connect;
    g_instance->PostConnect = post_connect;

    auto s = g_instance->settings;

    s->ServerHostname = _strdup("canada.camilothomas.com");
    s->Username = _strdup(user.c_str());
    s->Password = _strdup(pass.c_str());

    s->DesktopWidth = 1280;
    s->DesktopHeight = 800;
    s->ColorDepth = 32;

    s->NlaSecurity = TRUE;
    s->TlsSecurity = TRUE;

    if (!freerdp_connect(g_instance))
    {
        MessageBoxA(NULL, "Connection failed", "Error", MB_OK);
        return -1;
    }

    MSG msg;

    while (true)
    {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                goto cleanup;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (!freerdp_check_event_handles(g_instance))
            break;

        InvalidateRect(g_hwnd, NULL, FALSE);
    }

cleanup:
    freerdp_disconnect(g_instance);
    freerdp_free(g_instance);
    return 0;
}
   application::application()
   {
      
      m_strAppId = "remoting/remote_connect";

      m_strAppName = "remoting/remote_connect";

      m_strBaseSupportId = "remoting_remote_connect";

      m_bNetworking = false;

      m_bLicense = false;

      m_bImaging = false; // showing application icon may use innate_ui icon?

      m_bWriteText = false;

      m_bInterprocessCommunication = false;

   }


   application::~application()
   {

   }


   void application::on_request(::request * prequest)
   {

      if (prequest->m_ecommand == e_command_file_open)
      {

         auto payloadFile = prequest->m_payloadFile;

         ::string strHost;

         auto pathFile = payloadFile.as_file_path();

         strHost = file()->safe_get_string(pathFile);

         node()->command_system("mstsc.exe /v:" + strHost, 10000_days);

      }
      //if (!m_pmainwindow)
      //{

      //   m_pmainwindow = create_newø<::app_app::main_window>();

      //   m_pmainwindow->create_main_window(prequest);

      //   //m_pmainwindow->display(e_display_normal);
      //   
      //   m_pmainwindow->initial_frame_placement();

      //}

//      pmainwindow->m_procedureOnAfterCreate = [pmainwindow]()
//      {
//
//         //pmainwindow->set_need_layout();
//
//         pmainwindow->set_need_redraw();
//
//         pmainwindow->post_redraw();
//
//      };

      //pmainwindow->create_main_window(prequest);

      //pmainwindow->m_pthreadUserInteraction->m_procedurea.add([this]() {});

      //pmainwindow->m_pthreadUserInteraction->run_posted_procedures();

   }


   string application::draw2d_get_default_implementation_name()
   {

      //return system()->implementation_name("draw2d", "opengl");
      //return system()->implementation_name("draw2d", "gdiplus");
      return ::aura::application::draw2d_get_default_implementation_name();

   }

  
} // namespace app_remoting_remote_connectapp



