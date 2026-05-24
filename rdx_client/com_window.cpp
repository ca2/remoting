//
// Created by camilo on 2026-05-23 12:23 <3ThomasBorregaardSørensen!! Mummi!! Bilbo!!
//
#include "framework.h"
#include "com_window.h"
#include "acme/windowing/windowing.h"
#include "client_site.h"
#include "in_place_frame.h"
#include "in_place_site.h"
//#include "main_window.h"

namespace windows
{

   com_window::com_window()
   {
m_bCustomPaint = false;

   }


   com_window::~com_window()
   {

   }


   ::windows::window_class com_window::_get_window_class()
   {

      return system()->acme_windowing()->_com_host_window_class();

   }


   ::i32_rectangle com_window::get_rectangle()
   {

      return m_rectangle;

   }


   i64 com_window::get_style_for_creating_window()
   {

      return WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

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


   void com_window::do_control_layout()
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

      GetClientRect(hwnd, &rc);

      informationf(
          "SetObjectRects: %d x %d",
          rc.right - rc.left,
          rc.bottom - rc.top);

      HRESULT hr =
          m_pclientsite->m_pinplacesite->m_pinplaceframe->m_pinplaceobject->SetObjectRects(&rc, &rc);

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


   void com_window::control_activate(bool bActivate)
   {

      if (m_pclientsite)
      {
         if (m_pclientsite->m_pinplacesite->m_pinplaceframe->m_pinplaceactiveobject)
         {

            m_pclientsite->m_pinplacesite->m_pinplaceframe->m_pinplaceactiveobject->OnFrameWindowActivate(bActivate ? TRUE : FALSE);

            m_pclientsite->m_pinplacesite->m_pinplaceframe->m_pinplaceactiveobject->OnDocWindowActivate(bActivate ? TRUE : FALSE);

         }
      }

   }





   void com_window::do_control_show_verb()
   {

      auto hwnd = ::as_HWND(this->operating_system_window());

      auto rectangleClient = window_get_client_rect();

      auto rect = ::as_RECT(rectangleClient);

      auto hr = m_poleobject->DoVerb(OLEIVERB_SHOW, nullptr, m_pclientsite, 0, hwnd, &rect);

      if (FAILED(hr))
      {

         information("do_control_ui_activate DoVer(OLEIVERB_SHOW ... ) Failed");

      }

   }


   void com_window::do_control_ui_activate()
   {

      auto hwnd = ::as_HWND(this->operating_system_window());

      auto rectangleClient = window_get_client_rect();

      auto rect = ::as_RECT(rectangleClient);

      auto hr = m_poleobject->DoVerb(OLEIVERB_UIACTIVATE, nullptr, m_pclientsite, 0, hwnd, &rect);

      if (FAILED(hr))
      {

         information("do_control_ui_activate DoVer(OLEIVERB_UIACTIVATE ... ) Failed");

      }

   }


   void com_window::on_window_size()
   {

      do_control_layout();

   }


   void com_window::on_window_set_focus()
   {

      control_activate(true);

   }


   bool com_window::on_window_activate(int iActivate, bool bMinimized,
                                        const operating_system::window &operatingsystemwindow)
   {

      auto active = iActivate != WA_INACTIVE;

      control_activate(active);

      return false;

   }


   bool com_window::on_window_mouse_activate(int &iResult, const operating_system::window &operatingsystemwindowTop, int iHitTest, int iMessage)
   {

      iResult = MA_ACTIVATE;

      return true;

   }


   void com_window::on_create_window()
   {

      auto hwnd = ::as_HWND(this->operating_system_window());

      construct_newø(m_pclientsite);

      HRESULT hr = CoCreateInstance(
            m_rclsid,
            //CLSID_MsRdpClient9NotSafeForScripting,
            nullptr,
            CLSCTX_INPROC_SERVER,
            __interface_of(m_poleobject));

      if (FAILED(hr))
      {

         post_message_box(L"Failed to create RDP ActiveX", L"Error", ::user::e_message_box_icon_error);

         return;

      }

      m_pclientsite->initialize_client_site(this);

      m_poleobject->SetClientSite(m_pclientsite);

      OleSetContainedObject(m_poleobject, TRUE);

      ///OleRun(m_poleobject);

      do_control_show_verb();

      //do_control_ui_activate();

      ///::comptr < IOleInPlaceObject > pinplaceobject;

      m_poleobject.as(m_pclientsite->m_pinplacesite->m_pinplaceframe->m_pinplaceobject);

   }


} // namespace windows
