//
// Created by camilo on 2026-05-24 08:49 <ThomasBorregaardSørensen!! Mummi!! Bilbo!!
//
#include "framework.h"
#include "com_window.h"
#include "toolbar.h"
#include "acme/operating_system/windows/window.h"
#pragma comment(lib, "msimg32.lib")


namespace remoting_rdx_client {


   bool Hit(const RectF& rc, float x, float y)
   {
      return x >= rc.X &&
             y >= rc.Y &&
             x <= rc.X + rc.Width &&
             y <= rc.Y + rc.Height;
   }
   bool Hit(const RectF& rc, const ::i32_point & point)
   {
      return Hit(rc, (float)point.x, (float) point.y);
   }
constexpr wchar_t toolbar::CLASS_NAME[];
   HHOOK toolbar::s_mouseHook = nullptr;
   toolbar* toolbar::s_instance = nullptr;

constexpr wchar_t
toolbar::CLASS_NAME[];

toolbar::toolbar()
{

   m_bHasOwnWindow = true;
   //m_iExStyle = WS_EX_LAYERED |
   //WS_EX_TOOLWINDOW;

    InitGdiplus();
}

toolbar::~toolbar()
{
    Destroy();
    ShutdownGdiplus();
}


   void toolbar::InstallMouseHook()
{
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
bool toolbar::InitGdiplus()
{
    GdiplusStartupInput input{};

    return GdiplusStartup(
        &m_gdiplusToken,
        &input,
        nullptr) == Ok;
}

void toolbar::ShutdownGdiplus()
{
    if (m_gdiplusToken)
    {
        GdiplusShutdown(m_gdiplusToken);
        m_gdiplusToken = 0;
    }
}

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

   return WS_EX_LAYERED | WS_EX_TOOLWINDOW;

}

//
// bool toolbar::Create(
//     ::windows::com_window * pcomwindowParent,
//     int x,
//     int y,
//     int width,
//     int height)
// {
//     if (!RegisterClass())
//         return false;
//
//     m_pacmeuserinteractionMain = pcomwindowParent;
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
    int nCode,
    WPARAM wParam,
    LPARAM lParam)
{
   if (nCode == HC_ACTION && s_instance)
   {
      MSLLHOOKSTRUCT* info =
          reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);

      if (info)
      {
         s_instance->on_global_mouse(
             {info->pt.x, info->pt.y
      },
             wParam);
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

//    float toolbar::get_window_scale()
// {
//    auto hwnd = m_hwnd;
//    UINT dpi = GetDpiForWindow(hwnd);
//    return (float)dpi / 96.0f;
// }


   ::i32_rectangle toolbar::get_rectangle()
{

   return m_rectangle;

}

   void toolbar::on_global_mouse( const ::i32_point & point, ::wparam wparam)
{
   if (!m_bMouseEnable)
   {

      return;

   }
   auto rectangleWindow = get_window_rectangle();
   if (!rectangleWindow.contains(point))
   {

      return;

   }
   auto pointClient = screen_to_window_client(point);
   //ScreenToClient(m_hwnd, &pointClient);

   bool oldMin = m_hoverMin;
   bool oldRestore = m_hoverRestore;
   bool oldClose = m_hoverClose;

   m_hoverMin = Hit(m_rcMin, pointClient);
   m_hoverRestore = Hit(m_rcRestore, pointClient);
   m_hoverClose = Hit(m_rcClose, pointClient);

   if (wparam.m_wparam == WM_LBUTTONUP)
   {
      if (m_hoverClose)
      {
         m_bMouseEnable = false;
         PostToHost(3); // close
      }
      else if (m_hoverRestore)
      {
         m_bMouseEnable = true;
         PostToHost(2); // restore
      }
      else if (m_hoverMin)
      {
         m_bMouseEnable = false;
         PostToHost(1); // minimize
      }
   }

   if (m_hoverMin != oldMin ||
       m_hoverRestore != oldRestore ||
       m_hoverClose != oldClose)
   {
      Redraw();
   }

}


   void toolbar::PostToHost(int cmd)
{
   if (!m_pacmeuserinteractionMain)
      return;

   HWND hwndHost =
       ::as_HWND(m_pacmeuserinteractionMain->operating_system_window());

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

void toolbar::Move(
    int x,
    int y,
    int width,
    int height)
{
    m_rectangle.left = x;
   m_rectangle.top = y;
   m_rectangle.set_size({width, height});

    set_window_position(
        ::as_operating_system_window({HWND_TOPMOST}),
        m_rectangle.origin(),
        m_rectangle.size(),
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
          float x = lparam.x();
          float y = lparam.y();

          bool newHoverMin = Hit(m_rcMin, x, y);
          bool newHoverRestore = Hit(m_rcRestore, x, y);
          bool newHoverClose = Hit(m_rcClose, x, y);

          if (newHoverMin != m_hoverMin ||
              newHoverRestore != m_hoverRestore ||
              newHoverClose != m_hoverClose)
          {
             m_hoverMin = newHoverMin;
             m_hoverRestore = newHoverRestore;
             m_hoverClose = newHoverClose;

             Redraw();
          }

       lresult = 0;
          return true;
       }
       case WM_LBUTTONDOWN:
       {
          m_mouseDown = true;
          set_mouse_capture();
       lresult = 0;
       return true;
       }
       case WM_LBUTTONUP:
       {
          m_mouseDown = false;
          ReleaseCapture();

          float x = lparam.x();
          float y = lparam.y();

          if (Hit(m_rcMin, x, y))
          {
             m_pacmeuserinteractionMain->post_message((::user::enum_message) WM_TOOLBAR_CMD, 1); // minimize
          }
          else if (Hit(m_rcRestore, x, y))
          {
             m_pacmeuserinteractionMain->post_message((::user::enum_message) WM_TOOLBAR_CMD, 2); // restore
          }
          else if (Hit(m_rcClose, x, y))
          {
             m_pacmeuserinteractionMain->post_message((::user::enum_message) WM_TOOLBAR_CMD, 3);  // close
          }

       lresult = 0;
       return true;
       }
    }

   return false;
}

void toolbar::RenderLayered()
{
    HDC screenDC = GetDC(nullptr);
    HDC memDC = CreateCompatibleDC(screenDC);

    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = m_rectangle.width();
    bmi.bmiHeader.biHeight = -m_rectangle.height();
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;

    HBITMAP bitmap = CreateDIBSection(
        screenDC,
        &bmi,
        DIB_RGB_COLORS,
        &bits,
        nullptr,
        0);

    HBITMAP oldBitmap =
        (HBITMAP)SelectObject(memDC, bitmap);

   {

       Graphics g(memDC);

       
       _on_draw(&g);

   }

    POINT ptSrc{0,0};
    SIZE sizeWnd{
        m_rectangle.width(),
        m_rectangle.height()
    };

    POINT ptDst{
        m_rectangle.left,
        m_rectangle.top
    };

    BLENDFUNCTION blend{};
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = 255;
    blend.AlphaFormat = AC_SRC_ALPHA;

   auto hwnd = ::as_HWND(this->operating_system_window());

    UpdateLayeredWindow(
        hwnd,
        screenDC,
        &ptDst,
        &sizeWnd,
        memDC,
        &ptSrc,
        0,
        &blend,
        ULW_ALPHA);

    SelectObject(memDC, oldBitmap);

    DeleteObject(bitmap);
    DeleteDC(memDC);
    ReleaseDC(nullptr, screenDC);
}
   
   
   void toolbar::_on_draw(::Gdiplus::Graphics * pgraphics)
{


   float wForDraw = ((REAL)m_rectangle.width() - 2.0f);
   float hForDraw = ((REAL)m_rectangle.height() - 2.0f);


   {

         pgraphics->SetSmoothingMode(
             SmoothingModeHighQuality);

         pgraphics->SetTextRenderingHint(
             TextRenderingHintClearTypeGridFit);

         pgraphics->Clear(Color(0,0,0,0));


         // Toolbar background
         RectF bgRect(
             0.f,
             0.f,
             (REAL)wForDraw,
             (REAL)hForDraw);

         GraphicsPath path;

         const float radius = (hForDraw / 2.f);

         path.AddArc(0.f, 0.f, radius, radius, 180.f, 90.f);
         path.AddArc(
             (float) (wForDraw - radius),
             0.f,
             radius,
             radius,
             270.f,
             90.f);

         path.AddArc(
             (float) (wForDraw - radius),
             (float) (hForDraw - radius),
             radius,
             radius,
             0.f,
             90.f);

         path.AddArc(
             0.f,
             (float) (hForDraw - radius),
             radius,
             radius,
             90.f,
             90.f);

         path.CloseFigure();

         SolidBrush bgBrush(
             Color(120, 30, 130, 230));

         pgraphics->FillPath(&bgBrush, &path);

         Pen border(
             Color(180, 255, 255, 255),
             1.5f);

         pgraphics->DrawPath(&border, &path);

         // Buttons
         SolidBrush buttonBrush(
             Color(220, 0, 120, 215));

         SolidBrush textBrush(
             Color(255,255,255,255));

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
         // pgraphics->FillRectangle(
         //     &buttonBrush,
         //     btn1);
         //
         // pgraphics->FillRectangle(
         //     &buttonBrush,
         //     btn2);
         //
         // pgraphics->FillRectangle(
         //     &buttonBrush,
         //     btn3);
         //
         // pgraphics->DrawString(
         //     L"CTRL",
         //     -1,
         //     &font,
         //     btn1,
         //     &sf,
         //     &textBrush);
         //
         // pgraphics->DrawString(
         //     L"ALT",
         //     -1,
         //     &font,
         //     btn2,
         //     &sf,
         //     &textBrush);
         //
         // pgraphics->DrawString(
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
         // RectF bgRect(
         //     0.f,
         //     0.f,
         //     (REAL)m_width,
         //     (REAL)m_height);
         //
         // GraphicsPath path;
         //
         // const float radius = 14.f;
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
         // pgraphics->FillPath(&bgBrush, &path);
         //
         // Pen border(
         //     Color(120, 255, 255, 255),
         //     1.f);
         //
         // pgraphics->DrawPath(&border, &path);

         //
         // Left-side placeholder text
         //

         FontFamily ff(L"Segoe UI");

         Gdiplus::Font textFont(
             &ff,
             12.f * get_window_scale(),
             FontStyleRegular,
             UnitPixel);

         SolidBrush textBrush(
             Color(230,255,255,255));

         StringFormat leftAlign;
         leftAlign.SetAlignment(
             StringAlignmentNear);

         leftAlign.SetLineAlignment(
             StringAlignmentCenter);

         RectF titleRect(
             14.f,
             0.f,
             300.f * get_window_scale(),
             (REAL)m_rectangle.height());

         ::wstring wstrTitle(m_strTitle);

         pgraphics->DrawString(
             wstrTitle,
             -1,
             &textFont,
             titleRect,
             &leftAlign,
             &textBrush);

         //
         // Window buttons (right side)
         //

         const float btnSize = m_btnSize * get_window_scale();
         const float btnSpce = m_btnSpacing* get_window_scale();

         float closeX =
             (float)m_rectangle.width() - btnSize - 10.f;

         float restoreX =
             closeX - btnSize - btnSpce;

         float minimizeX =
             restoreX - btnSize - btnSpce;

         float topY =
             ((float)hForDraw - btnSize) * 0.5f;

         RectF rcMin(
             minimizeX,
             topY,
             btnSize,
             btnSize);

         m_rcMin = rcMin;

         RectF rcRestore(
             restoreX,
             topY,
             btnSize,
             btnSize);

         m_rcRestore = rcRestore;

         RectF rcClose(
             closeX,
             topY,
             btnSize,
             btnSize);

         m_rcClose = rcClose;

         //
         // Hover-style fills
         //



         Color minColor =
    m_hoverMin
    ? Color(120, 255, 255, 255)
    : Color(60, 255, 255, 255);

         SolidBrush minBrush(minColor);

         pgraphics->FillEllipse(
             &minBrush,
             rcMin);

         Color resColor =
    m_hoverRestore
    ? Color(120, 255, 255, 255)
    : Color(60, 255, 255, 255);

         SolidBrush resBrush(resColor);

         pgraphics->FillEllipse(
             &resBrush,
             rcRestore);


         Color closeColor =
    m_hoverClose
    ? Color(200, 255, 80, 80)
    : Color(120, 220, 60, 60);

         SolidBrush closeBrush(closeColor);

         pgraphics->FillEllipse(
             &closeBrush,
             rcClose);

         //
         // Glyph pen
         //

         Pen glyphPen(
             Color(240,255,255,255),
             1.8f);

         glyphPen.SetStartCap(LineCapRound);
         glyphPen.SetEndCap(LineCapRound);

         float fSmaller = 6.f * get_window_scale();

         //
         // Minimize glyph
         //

         float midY =
             rcMin.Y + rcMin.Height * 0.62f;

         pgraphics->DrawLine(
             &glyphPen,
             rcMin.X + fSmaller,
             midY,
             rcMin.X + rcMin.Width - fSmaller,
             midY);

         //
         // Restore glyph
         //

         RectF restoreInner(
             rcRestore.X + fSmaller,
             rcRestore.Y + fSmaller,
             rcRestore.Width - fSmaller*2.f,
             rcRestore.Height - fSmaller*2.f);

         pgraphics->DrawRectangle(
             &glyphPen,
             restoreInner);

         //
         // Close glyph
         //

         pgraphics->DrawLine(
             &glyphPen,
             rcClose.X + fSmaller,
             rcClose.Y + fSmaller,
             rcClose.X + rcClose.Width - fSmaller,
             rcClose.Y + rcClose.Height - fSmaller);

         pgraphics->DrawLine(
             &glyphPen,
             rcClose.X + rcClose.Width - fSmaller,
             rcClose.Y + fSmaller,
             rcClose.X + fSmaller,
             rcClose.Y + rcClose.Height - fSmaller);

       }

   
}
   
} // namespace remoting_rdx_client
