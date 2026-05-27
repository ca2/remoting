//
// Created by camilo on 2026-05-24 08:49 <ThomasBorregaardSørensen!! Mummi!! Bilbo!!
//
#include "framework.h"
#include "toolbar.h"
#include "acme/nano/nano.h"
#include "acme/nano/graphics/brush.h"
#include "acme/nano/graphics/context.h"
#include "acme/nano/graphics/font.h"
#include "acme/nano/graphics/graphics.h"
#include "acme/nano/graphics/path.h"
#include "acme/nano/graphics/pen.h"
#include "acme/operating_system/windows/window.h"
#include "acme/operating_system/windows/windows.h"
#include "com_window.h"
#include "main_window.h"
#pragma comment(lib, "msimg32.lib")


namespace remoting_rdx_client 
{


   //bool Hit(const Gdiplus::RectF& rc, ::f32 x, ::f32 y)
   //{
   //   return x >= rc.X &&
   //          y >= rc.Y &&
   //          x <= rc.X + rc.Width &&
   //          y <= rc.Y + rc.Height;
   //}
   //bool Hit(const Gdiplus::RectF &rc, const ::i32_point &point)
   //{
   //   return Hit(rc, (::f32)point.x, (::f32) point.y);
   //}
constexpr wchar_t toolbar::CLASS_NAME[];
   HHOOK toolbar::s_mouseHook = nullptr;
   toolbar* toolbar::s_instance = nullptr;

constexpr wchar_t
toolbar::CLASS_NAME[];

toolbar::toolbar()
{

   m_ehitPress = e_hit_none;
   m_ehitHover = e_hit_none;

   m_bHasOwnWindow = true;
   //m_iExStyle = WS_EX_LAYERED |
   //WS_EX_TOOLWINDOW;

   m_bControlDeactivated = false;

   //InitGdiplus();

}

toolbar::~toolbar()
{
    Destroy();
    //ShutdownGdiplus();
}


   void toolbar::InstallMouseHook()
{
      //return;
   s_instance = this;

   if (!s_mouseHook)
   {
      s_mouseHook = SetWindowsHookEx(
          WH_MOUSE_LL,
          MouseProc,
          GetModuleHandle(nullptr),
          0);
   }
}

   void toolbar::UninstallMouseHook()
{
   if (s_mouseHook)
   {
      UnhookWindowsHookEx(s_mouseHook);
      s_mouseHook = nullptr;
   }

   s_instance = nullptr;
}
//bool toolbar::InitGdiplus()
//{
//    GdiplusStartupInput input{};
//
//    return GdiplusStartup(
//        &m_gdiplusToken,
//        &input,
//        nullptr) == Ok;
//}
//
//void toolbar::ShutdownGdiplus()
//{
//    if (m_gdiplusToken)
//    {
//        GdiplusShutdown(m_gdiplusToken);
//        m_gdiplusToken = 0;
//    }
//}

   ::windows::window_class toolbar::_get_window_class()
{
    static ::windows::window_class s_windowclass;

    if (s_windowclass.m_wstrClassName.has_character())
        return s_windowclass;

      s_windowclass.m_wstrClassName = CLASS_NAME;
   s_windowclass.m_hinstance = GetModuleHandle(nullptr);;

    WNDCLASSEX wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = ::windows::window::s_window_procedure;
    wc.hInstance = (HINSTANCE) s_windowclass.m_hinstance;
    wc.lpszClassName = s_windowclass.m_wstrClassName;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = nullptr;

    if (!RegisterClassEx(&wc))
    {
        if (GetLastError() != ERROR_CLASS_ALREADY_EXISTS)
            return {};
    }


    return s_windowclass;
}

   ::i64 toolbar::get_style_for_creating_window()
{

   return WS_POPUP;

}
   ::i64 toolbar::get_ex_style_for_creating_window()
{

   return WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE;

}

//
// bool toolbar::Create(
//     ::windows::com_window * pcomwindowParent,
//     ::i32 x,
//     ::i32 y,
//     ::i32 width,
//     ::i32 height)
// {
//     if (!RegisterClass())
//         return false;
//
//     m_pmainwindow = pcomwindowParent;
//
//    auto hwndParent = ::as_HWND(pcomwindowParent->operating_system_window());
//     m_x = x;
//     m_y = y;
//     m_width = width;
//     m_height = height;
//
//     m_hwnd = CreateWindowEx(
//         WS_EX_LAYERED |
//         WS_EX_TOOLWINDOW,
//         CLASS_NAME,
//         L"",
//         WS_POPUP,
//         x,
//         y,
//         width,
//         height,
//         hwndParent,
//         nullptr,
//         GetModuleHandle(nullptr),
//         this);
//
//     if (!m_hwnd)
//         return false;
//
//     ShowWindow(m_hwnd, SW_SHOW);
//
//     Redraw();
//    InstallMouseHook();
//     return true;
// }

//   void toolbar::on_c

   LRESULT CALLBACK toolbar::MouseProc(
    ::i32 nCode,
    WPARAM wParam,
    LPARAM lParam)
{

          // If nCode is less than zero, you must pass the message on without processing it
   if (nCode < 0)
   {
      return CallNextHookEx(s_mouseHook, nCode, wParam, lParam);
   }

   if (nCode == HC_ACTION && s_instance)
   {
      MSLLHOOKSTRUCT* info =
          reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);

      if (info)
      {
         s_instance->on_global_mouse(
            { info->pt.x, info->pt.y
            },
            wParam);
         // if (s_instance->on_global_mouse(
         //    { info->pt.x, info->pt.y
         //    },
         //    wParam))
         // {
         //
         //    return 1;
         //
         // }
      }
   }

   return CallNextHookEx(
       s_mouseHook,
       nCode,
       wParam,
       lParam);
}
void toolbar::Destroy()
{
    // if (m_hwnd)
    // {
    //     DestroyWindow(m_hwnd);
    //     m_hwnd = nullptr;
    // }
}

//    ::f32 toolbar::get_window_scale()
// {
//    auto hwnd = m_hwnd;
//    UINT dpi = GetDpiForWindow(hwnd);
//    return (::f32)dpi / 96.0f;
// }


   ::i32_rectangle toolbar::get_rectangle()
{

      return {m_point, m_size};

}

   toolbar::enum_hit toolbar::hitTest(const ::i32_point &point)
   {
      enum_hit ehit = e_hit_none;
      if (m_rectangleMinimize.contains(point))
      {
         ehit = e_hit_min;
      }
      else if (m_rectangleRestore.contains(point))
      {
         ehit = e_hit_restore;
      }
      else if (m_rectangleClose.contains( point))
      {
         ehit = e_hit_close;
      }
      else if (i32_rectangle(m_size).contains(point))
      {

         ehit = e_hit_client;
      }
      return ehit;
   }

   
   bool toolbar::on_global_mouse( const ::i32_point & point, ::wparam wparam)
   {

      if (!m_bMouseEnable)
      {

         return false;

      }

      auto rectangleWindow = get_window_rectangle();
      
      if (m_ehitHover == e_hit_none && m_ehitPress == e_hit_none && !rectangleWindow.contains(point))
      {

         if (m_bControlDeactivated)
         {

            m_bControlDeactivated = false;

            release_mouse_capture();

            m_pmainwindow->do_cancel_mode();

            information("::remoting_rdx_client::toolbar::on_global_mouse Setting Focus to Control...");

            m_pmainwindow->set_focus_to_rdp_host();

            m_pmainwindow->activate_rdp_host_control(true);

            m_pmainwindow->ui_activate_rdp_host_control();

         }

         return false;

      }

      if (!m_bControlDeactivated)
      {

         m_bControlDeactivated = true;

         set_mouse_capture();

         ///m_pmainwindow->activate_rdp_host_control(false);

      }

      bool bRet = false;
   
      auto pointClient = screen_to_window_client(point);
   //ScreenToClient(m_hwnd, &pointClient);

   auto ehitHoverOld = m_ehitHover;
   //bool oldMin = m_hoverMin;
   //bool oldRestore = m_hoverRestore;
   //bool oldClose = m_hoverClose;

   auto ehitNew = hitTest(pointClient);

   if (wparam.m_wparam == WM_LBUTTONDOWN)
   {

      m_ehitPress = ehitNew;

      if (ehitNew == e_hit_client)
      {

         m_pointDragStartCursor = point;

         m_pointDragWindowOrigin = m_point;

         m_iMainScreenWidth = m_pmainwindow->get_main_screen_size().width();

      }

      if (m_ehitPress != e_hit_none)
      {

         bRet = true;

      }
   }
   else if (wparam.m_wparam == WM_MOUSEMOVE)
   {
      
      if (m_ehitPress == e_hit_client)
      {

         ::i32 dx = point.x - m_pointDragStartCursor.x;

         auto x = m_pointDragWindowOrigin.x + dx;

         x = minimum_maximum(x, 0, m_iMainScreenWidth - m_size.cx);

         Move(x, m_pointDragWindowOrigin.y);
         //SetWindowPos(hwnd, nullptr, m_xDragStartWindow + dx, m_yDragStartWindow, 0, 0,
                      //SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
         bRet = true;
      }
   }
   else if (wparam.m_wparam == WM_LBUTTONUP)
   {
      if (ehitNew == m_ehitPress)
      {
         if (ehitNew == e_hit_close)
         {
            m_bMouseEnable = false;
            PostToHost(3); // close
         }
         else if (ehitNew == e_hit_restore)
         {
            m_bMouseEnable = true;
            PostToHost(2); // restore
         }
         else if (ehitNew == e_hit_min)
         {
            m_bMouseEnable = false;
            PostToHost(1); // minimize
         }
      }
      if (m_ehitPress != e_hit_none)
      {

         m_pmainwindow->set_focus_to_rdp_host();

         bRet = true;

      }
      m_ehitPress = e_hit_none;
   }

   if (m_ehitHover != ehitNew)
   {
      m_ehitHover = ehitNew;
      Redraw();
   }
   return bRet;
}


   void toolbar::PostToHost(::i32 cmd)
{
   if (!m_pmainwindow)
      return;

   HWND hwndHost =
       ::as_HWND(m_pmainwindow->operating_system_window());

   PostMessage(hwndHost, WM_APP + 100, cmd, 0);
}
// void toolbar::Show(bool show)
// {
//     //ShowWindow(m_hwnd, show ? SW_SHOW : SW_HIDE);
//    if (show)
//    {
//
//
//
//    }
// }

   void toolbar::Move(::i32 x, ::i32 y)
{
   m_point.x= x;
   m_point.y = y;

   set_window_position(
      ::as_operating_system_window({HWND_TOPMOST}), m_point, {},
                       SWP_NOACTIVATE | SWP_NOSIZE);

   Redraw();
}

void toolbar::Place(
    ::i32 x,
    ::i32 y,
    ::i32 width,
    ::i32 height)
{
   m_point = {x, y};
   m_size = {width, height};

    set_window_position(
        ::as_operating_system_window({HWND_TOPMOST}),
        m_point,
        m_size,
        SWP_NOACTIVATE);

    Redraw();

}


void toolbar::Redraw()
{
    RenderLayered();
}
//
// LRESULT CALLBACK toolbar::WndProc(
//     HWND hwnd,
//     UINT msg,
//     WPARAM wParam,
//     LPARAM lParam)
// {
//     toolbar* self = nullptr;
//
//     if (msg == WM_NCCREATE)
//     {
//         auto cs = reinterpret_cast<CREATESTRUCT*>(lParam);
//
//         self = reinterpret_cast<
//             toolbar*>(
//                 cs->lpCreateParams);
//
//         SetWindowLongPtr(
//             hwnd,
//             GWLP_USERDATA,
//             reinterpret_cast<LONG_PTR>(self));
//
//         self->m_hwnd = hwnd;
//     }
//
//     else
//     {
//         self = reinterpret_cast<
//             toolbar*>(
//                 GetWindowLongPtr(
//                     hwnd,
//                     GWLP_USERDATA));
//     }
//
//     if (self)
//         return self->HandleMessage(
//             msg,
//             wParam,
//             lParam);
//
//     return DefWindowProc(
//         hwnd,
//         msg,
//         wParam,
//         lParam);
// }

bool toolbar::_on_window_procedure(lresult &lresult, u32 message, wparam wparam, lparam lparam)
   {
    switch (message)
    {
    case WM_NCHITTEST:
    {
       lresult = HTTRANSPARENT;
       return true;
    }

    case WM_ERASEBKGND:
    {

       lresult = TRUE;

       return true;

    }
       case WM_MOUSEMOVE:
       {

             //::f32 x = lparam.x();
             //::f32 y = lparam.y();

             //bool newHoverMin = Hit(m_rectangleMinimize, x, y);
             //bool newHoverRestore = Hit(m_rectangleRestore, x, y);
             //bool newHoverClose = Hit(m_rectangleClose, x, y);

             //if (newHoverMin != m_hoverMin || newHoverRestore != m_hoverRestore || newHoverClose != m_hoverClose)
             //{
             //   m_hoverMin = newHoverMin;
             //   m_hoverRestore = newHoverRestore;
             //   m_hoverClose = newHoverClose;

             //   Redraw();
             //}

       lresult = 0;
          return true;
       }
       case WM_LBUTTONDOWN:
       {
          //m_mouseDown = true;
          //set_mouse_capture();
       lresult = 0;
       return true;
       }
       case WM_LBUTTONUP:
       {
          //m_mouseDown = false;
          
          //ReleaseCapture();

          auto point = lparam.point();

          auto ehit = hitTest(point);

          if (ehit == e_hit_min)
          {
             m_pmainwindow->post_message((::user::enum_message) WM_TOOLBAR_CMD, 1); // minimize
          }
          else if (ehit == e_hit_restore)
          {
             m_pmainwindow->post_message((::user::enum_message) WM_TOOLBAR_CMD, 2); // restore
          }
          else if (ehit == e_hit_close)
          {
             m_pmainwindow->post_message((::user::enum_message) WM_TOOLBAR_CMD, 3);  // close
          }

       lresult = 0;
       return true;
       }
    }

   return false;
}

void toolbar::RenderLayered()
{
    HDC hdcScreen = GetDC(nullptr);
    HDC hdcMemory = CreateCompatibleDC(hdcScreen);

    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = m_size.width();
    bmi.bmiHeader.biHeight = -m_size.height();
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;

    HBITMAP bitmap = CreateDIBSection(
        hdcScreen,
        &bmi,
        DIB_RGB_COLORS,
        &bits,
        nullptr,
        0);

    HBITMAP oldBitmap =
        (HBITMAP)SelectObject(hdcMemory, bitmap);

   {

       system()->do_graphics_factory();

       auto pgraphicscontext = createø<::nano::graphics::context>();

       pgraphicscontext->attach(hdcMemory, m_size, 0);

       _on_draw(pgraphicscontext);

   }

    POINT ptSrc{0,0};
    SIZE sizeWnd{
        m_size.width(),
        m_size.height()
    };

    POINT ptDst{
        m_point.x,
        m_point.y
    };

    BLENDFUNCTION blend{};
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = 255;
    blend.AlphaFormat = AC_SRC_ALPHA;

   auto hwnd = ::as_HWND(this->operating_system_window());

    UpdateLayeredWindow(
        hwnd,
        hdcScreen,
        &ptDst,
        &sizeWnd,
        hdcMemory,
        &ptSrc,
        0,
        &blend,
        ULW_ALPHA);

    SelectObject(hdcMemory, oldBitmap);

    DeleteObject(bitmap);
    DeleteDC(hdcMemory);
    ReleaseDC(nullptr, hdcScreen);
}
   
   
   void toolbar::_on_draw(::nano::graphics::context * pgraphicscontext)
   {

      auto fW = (::f64) m_size.width();
      auto fH = (::f64) m_size.height();

      //auto wForDraw = fW - 2.0f;
      //auto hForDraw = fH - 2.0f;

      auto wForDraw = fW - 1.0;
      auto hForDraw = fH - 1.0;

      auto pnanographics = nano()->graphics();

      {

         //pgraphicscontext->SetSmoothingMode(
           //    SmoothingModeHighQuality);

         pgraphicscontext->set_smoothing_mode(::nano::graphics::e_smoothing_mode_high_quality);

         //pgraphicscontext->SetTextRenderingHint(
           //    TextRenderingHintClearTypeGridFit);

         pgraphicscontext->set_text_rendering_hint(::nano::graphics::e_text_rendering_hint_clear_type_grid_fit);

         //pgraphicscontext->Clear(Color(0,0,0,0));
         pgraphicscontext->clear(::color::transparent);

         // Toolbar background
         auto rectangleBackground = ::f64_rectangle_dimension(
             0.,
             0.,
               wForDraw,
               hForDraw);

         auto ppath = createø<::nano::graphics::path>();

//            Gdiplus::GraphicsPath path;

            const ::f64 radius = (hForDraw / 2.);

         /*   path.AddArc(
                (::f32) (wForDraw - radius),
                0.f,
                radius,
                radius,
                270.f,
                90.f);

            path.AddArc(
                (::f32) (wForDraw - radius),
                (::f32) (hForDraw - radius),
                radius,
                radius,
                0.f,
                90.f);

            path.AddArc(
                0.f,
                (::f32) (hForDraw - radius),
                radius,
                radius,
                90.f,
                90.f);*/
            // ppath->AddArc(0.f, 0.f, radius, radius, 180.f, 90.f);
            ppath->add_arc(0., 0., radius, radius, 180_degree, 90_degree);


            ppath->add_arc(wForDraw - radius, 0., radius, radius, 270_degrees, 90_degrees);

            ppath->add_arc(wForDraw - radius, hForDraw - radius, radius, radius, 0_degrees,
                                       90_degrees);

            ppath->add_arc(0., hForDraw - radius, radius, radius, 90_degrees, 90_degrees);


            ppath->close_figure();
            //path.CloseFigure();

            auto pbrushBackground = createø<::nano::graphics::brush>();

            pbrushBackground->create_solid_brush(::argb(120, 30, 130, 230));

            //SolidBrush bgBrush(
              //  Color(120, 30, 130, 230));

            

            //pgraphicscontext->FillPath(&bgBrush, &path);

            auto ppenBorder = createø<::nano::graphics::pen>();

            ppenBorder->create_pen(::argb(180, 255, 255, 255), 1.5);
            //Pen border(
            //    Color(180, 255, 255, 255),
            //    1.5f);

            pgraphicscontext->do_path(ppath, pbrushBackground, ppenBorder);

            //pgraphicscontext->DrawPath(&border, &path);

            //   pgraphicscontext->draw_path(ppath, ppenBorder);

            // Buttons
               auto pbrushButton = pnanographics->create_solid_brush(::argb(220, 0, 120, 215));

            //SolidBrush buttonBrush(
              //  Color(220, 0, 120, 215));

            auto pbrushText = pnanographics->create_solid_brush(::color::white);

            //SolidBrush textBrush(
               // Color(255,255,255,255));

            // FontFamily ff(L"Segoe UI");
            // Gdiplus::Font font(
            //     &ff,
            //     12.f,
            //     FontStyleRegular,
            //     UnitPixel);
            //
            // StringFormat sf;
            // sf.SetAlignment(
            //     StringAlignmentCenter);
            //
            // sf.SetLineAlignment(
            //     StringAlignmentCenter);
            //
            // RectF btn1(12.f, 8.f, 80.f, 32.f);
            // RectF btn2(100.f, 8.f, 80.f, 32.f);
            // RectF btn3(188.f, 8.f, 80.f, 32.f);
            //
            // pgraphicscontext->FillRectangle(
            //     &buttonBrush,
            //     btn1);
            //
            // pgraphicscontext->FillRectangle(
            //     &buttonBrush,
            //     btn2);
            //
            // pgraphicscontext->FillRectangle(
            //     &buttonBrush,
            //     btn3);
            //
            // pgraphicscontext->DrawString(
            //     L"CTRL",
            //     -1,
            //     &font,
            //     btn1,
            //     &sf,
            //     &textBrush);
            //
            // pgraphicscontext->DrawString(
            //     L"ALT",
            //     -1,
            //     &font,
            //     btn2,
            //     &sf,
            //     &textBrush);
            //
            // pgraphicscontext->DrawString(
            //     L"DEL",
            //     -1,
            //     &font,
            //     btn3,
            //     &sf,
            //     &textBrush);
          }

          {


            // Replace the old button drawing section inside RenderLayered()
            // with this version.
            //
            // Layout:
            //
            // -----------------------------------------------------
            // |  Host Placeholder                     _ []  X     |
            // -----------------------------------------------------
            //
            // The right-side buttons are placeholders only.
            // You can later map them to host callbacks/messages.
            //

            // // Toolbar background
            // RectF rectangleBackground(
            //     0.f,
            //     0.f,
            //     (REAL)m_width,
            //     (REAL)m_height);
            //
            // GraphicsPath path;
            //
            // const ::f32 radius = 14.f;
            //
            // path.AddArc(0, 0, radius, radius, 180, 90);
            // path.AddArc(
            //     m_width - radius,
            //     0,
            //     radius,
            //     radius,
            //     270,
            //     90);
            //
            // path.AddArc(
            //     m_width - radius,
            //     m_height - radius,
            //     radius,
            //     radius,
            //     0,
            //     90);
            //
            // path.AddArc(
            //     0,
            //     m_height - radius,
            //     radius,
            //     radius,
            //     90,
            //     90);
            //
            // path.CloseFigure();
            //
            // SolidBrush bgBrush(
            //     Color(210, 25, 25, 25));
            //
            // pgraphicscontext->FillPath(&bgBrush, &path);
            //
            // Pen border(
            //     Color(120, 255, 255, 255),
            //     1.f);
            //
            // pgraphicscontext->DrawPath(&border, &path);

            //
            // Left-side placeholder text
            //

            //FontFamily ff(L"Segoe UI");

            //Gdiplus::Font textFont(
            //    &ff,
            //    12.f * get_window_scale(),
            //    FontStyleRegular,
            //    UnitPixel);

            auto pfontText = pnanographics->create_pixel_font(e_font_sans_ui, 12. * get_window_scale(), false);


            auto pbrushText = pnanographics->create_solid_brush(::argb(230, 255, 255, 255));
            //SolidBrush textBrush(
              //  Color(230,255,255,255));

            //StringFormat leftAlign;
            //leftAlign.SetAlignment(
            //    StringAlignmentNear);

            //leftAlign.SetLineAlignment(
            //    StringAlignmentCenter);

            auto rectangleTitle = ::f64_rectangle_dimension(
                14.,
                0.,
                300. * get_window_scale(),
                (::f64) m_size.height());

            ::wstring wstrTitle(m_strTitle);

            /*pgraphicscontext->DrawString(
                wstrTitle,
                -1,
                &textFont,
                titleRect,
                &leftAlign,
                &textBrush);*/

            pgraphicscontext->draw_text123(m_strTitle, rectangleTitle, e_align_left_center, e_draw_text_none, nullptr,
                                           pbrushText, pfontText);

            //
            // Window buttons (right side)
            //

            const ::f64 btnSize = m_btnSize * get_window_scale();
            const ::f64 btnSpce = m_btnSpacing* get_window_scale();

            ::f64 closeX =
                (::f64)m_size.width() - btnSize - 10.f;

            ::f64 restoreX =
                closeX - btnSize - btnSpce;

            ::f64 minimizeX =
                restoreX - btnSize - btnSpce;

            ::f64 topY =
                ((::f64)hForDraw - btnSize) * 0.5f;

            auto rectangleMinimize = ::f64_rectangle_dimension(
                minimizeX,
                topY,
                btnSize,
                btnSize);

            m_rectangleMinimize = rectangleMinimize;

            auto rectangleRestore = ::f64_rectangle_dimension(
                restoreX,
                topY,
                btnSize,
                btnSize);

            m_rectangleRestore = rectangleRestore;

            auto rectangleClose = ::f64_rectangle_dimension(
                closeX,
                topY,
                btnSize,
                btnSize);

            m_rectangleClose = rectangleClose;

            //
            // Hover-style fills
            //



            ::color::color colorMinimize =
       m_ehitHover == e_hit_min
       ? ::argb(120, 255, 255, 255)
       : ::argb(60, 255, 255, 255);

            auto pbrushMinimize = pnanographics->create_solid_brush(colorMinimize);

            //SolidBrush minBrush(minColor);

            //pgraphicscontext->FillEllipse(
            //    &minBrush,
            //    rcMin);

            pgraphicscontext->ellipse(rectangleMinimize, pbrushMinimize, nullptr);

            ::color::color colorRestore = m_ehitHover == e_hit_restore
       ? ::argb(120, 255, 255, 255)
       : ::argb(60, 255, 255, 255);

            auto pbrushRestore = pnanographics->create_solid_brush(colorRestore);
            //SolidBrush resBrush(resColor);

            //pgraphicscontext->FillEllipse(
            //    &resBrush,
            //    rcRestore);

            pgraphicscontext->ellipse(rectangleRestore, pbrushRestore, nullptr);
            ::color::color colorClose = m_ehitHover == e_hit_close
       ? ::argb(200, 255, 80, 80)
       : ::argb(120, 220, 60, 60);

            auto pbrushClose = pnanographics->create_solid_brush(colorClose);
            //SolidBrush closeBrush(closeColor);

            //pgraphicscontext->FillEllipse(
            //    &closeBrush,
            //    rectangleClose);

            pgraphicscontext->ellipse(rectangleClose, pbrushClose, nullptr);

            //
            // Glyph pen
            //

            auto ppenGlyph = pnanographics->create_pen(::argb(240, 255, 255, 255), 1.8);
            //Pen glyphPen(
            //    Color(240,255,255,255),
            //    1.8f);

            ppenGlyph->set_start_cap(::nano::graphics::e_line_cap_round);
            ppenGlyph->set_end_cap(::nano::graphics::e_line_cap_round);

            //glyphPen.SetStartCap(LineCapRound);
            //glyphPen.SetEndCap(LineCapRound);

            ::f64 fSmaller = 6 * get_window_scale();

            //
            // Minimize glyph
            //

            ::f64 midY = rectangleMinimize.top + rectangleMinimize.height() * 0.62;

            //pgraphicscontext->DrawLine(
            //    &glyphPen,
            //    rcMin.X + fSmaller,
            //    midY,
            //    rcMin.X + rcMin.Width - fSmaller,
            //    midY);
            pgraphicscontext->line({rectangleMinimize.left + fSmaller, midY},
                                   {rectangleMinimize.left + rectangleMinimize.width() - fSmaller, midY}, 
                ppenGlyph);

            //
            // Restore glyph
            //

            auto rectangleRestoreInner = ::f64_rectangle_dimension(
                rectangleRestore.left + fSmaller, rectangleRestore.top + fSmaller,
                rectangleRestore.width() - fSmaller * 2., rectangleRestore.height() - fSmaller * 2.);

            //pgraphicscontext->DrawRectangle(
            //    &glyphPen,
            //    restoreInner);

            pgraphicscontext->rectangle(rectangleRestoreInner, nullptr, ppenGlyph);
            //
            // Close glyph
            //

            pgraphicscontext->line({rectangleClose.left + fSmaller, rectangleClose.top + fSmaller},
                                   {rectangleClose.right - fSmaller, rectangleClose.bottom - fSmaller},
               ppenGlyph);

            pgraphicscontext->line({rectangleClose.right - fSmaller, rectangleClose.top + fSmaller},
                                   {rectangleClose.left + fSmaller, rectangleClose.bottom - fSmaller}
            , ppenGlyph);

          }

   
   }
   
} // namespace remoting_rdx_client
