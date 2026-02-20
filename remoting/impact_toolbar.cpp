//
// Created by camilo on 2026-02-11 23:18 <3ThomasBorregaardSørensen!!
//
#include "framework.h"
#include "DesktopWindow.h"
#include "ViewerWindow.h"
#include "impact_toolbar.h"
#include <dwmapi.h>
// Link with dwmapi.lib
#pragma comment (lib, "dwmapi.lib")
#pragma comment (lib, "gdiplus.lib")
#include <gdiplus.h>

namespace remoting
{
   control::control()
   {
      m_econtrol  = e_control_none;
      m_eid = id_none;
      m_pbitmapBuffer = nullptr;
      m_pgraphicsBuffer = nullptr;
      m_bNewRepaintRectangle = true;

   }
   control::~control()
   {


   }


   void add_repaint(const ::int_rectangle & rectangle);

   toolbar::toolbar()
   {
      m_econtrol = e_control_toolbar;
   }
   toolbar::~toolbar()
   {


   }
   control * control::get_paint_window()
   {

      if (m_pcontrolParent == nullptr)
      {

         return this;

      }

      return m_pcontrolParent->get_paint_window();

   }


   ::int_rectangle control::get_client_rectangle()
   {

      return m_rectangle.size();
   }

   void control::_add_repaint(const ::int_rectangle & rectangle)
   {

      if (get_paint_window() != this)
      {

         throw ::exception(error_wrong_state);

      }

      if (m_bNewRepaintRectangle)
      {

         m_bNewRepaintRectangle = false;



         m_rectangleRepaint = rectangle;

      }
      else
      {

         m_rectangleRepaint.unite(rectangle);

      }

   }

   /// rectangle should in desktop window root coordinates
   void control::add_repaint(const int_rectangle &rectangle)
   {

      auto pwindowPaint = get_paint_window();

      if (::is_set(pwindowPaint))
      {

         pwindowPaint->_add_repaint(rectangle);

      }


   }

   void control::defer_repaint()
   {

      if (!m_bNewRepaintRectangle)
      {
         auto r = m_rectangleRepaint;

         m_pdesktopwindow->repaint(r);

         m_bNewRepaintRectangle = true;
      }

   }


   ::int_rectangle control::get_window_rectangle()
   {

      if (::is_null(m_pcontrolParent))
      {
         return m_rectangle;
      }
      else
      {

         auto r = m_rectangle;

         r.offset(m_pcontrolParent->get_window_rectangle().top_left());

         return r;

      }

   }



   ::int_rectangle control::get_paint_rectangle()
   {

      if (::is_null(m_pcontrolParent))
      {
         return get_client_rectangle();
      }
      else
      {

         auto r = m_rectangle;

         r.offset(m_pcontrolParent->get_paint_rectangle().top_left());

         return r;

      }

   }

   bool control::_001OnMouse(bool bPress, const ::int_point& pointRoot, const ::int_point& pointClient)
   {

      auto rectangleClient = get_client_rectangle();

      auto bHoverNew= rectangleClient.contains(::int_point{pointClient.x, pointClient.y});

      if (is_different(bHoverNew, m_bHover))
      {

         m_bHover = bHoverNew;

         add_repaint(get_window_rectangle());

      }

      if (bPress && bHoverNew)
      {
         if (!m_bPressed)
         {

            m_bPressed = true;

            on_left_down(pointClient);

         }
      }
      else
      {

         if (m_bPressed)
         {

            m_bPressed = false;

            on_left_up(pointClient);

         }

      }

      return false;

   }


   bool control::on_left_down(const ::int_point& position)
   {


      return false;

   }


   bool control::on_left_up(const ::int_point& position)
   {
      if (m_bHover)
      {
         if (on_button_click(m_eid))
         {
            return true;
         }

      }

      return false;
   }

   bool control::_000OnMouse(bool bPress, const ::int_point& pointRoot, const ::int_point& pointClientParam)
   {

      auto pointClient = pointClientParam;
      pointClient.x -= m_rectangle.left;
      pointClient.y -= m_rectangle.top;

      if (_001OnMouse(bPress, pointRoot, pointClient))
      {

         return true;

      }

      for (auto & pcontrol:m_controlaChildren)
      {

         if (pcontrol->_000OnMouse(bPress, pointRoot, pointClient))
         {
            return true;
         }

      }

      return false;

   }

   toolbar_button::toolbar_button()
   {
      m_econtrol = e_control_button;
   }

   toolbar_button::~toolbar_button()
   {

   }

   // bool toolbar_button::on_mouse(bool bPress, const ::int_point& position)
   //{


   style::style()
   {

      m_pbrushBackground = øallocate SolidBrush(RGB(40, 120, 180));
      m_pbrushBackgroundHover = øallocate SolidBrush(RGB(40, 120, 220));
      m_pbrushButtonBackground = øallocate SolidBrush(RGB(40, 120, 180));
      m_pbrushButtonBackgroundHover = øallocate SolidBrush(RGB(50, 130, 230));
      m_pbrushButtonPaint  =  øallocate SolidBrush(RGB(255, 255, 255));
      m_ppenPaint = øallocate Pen(PS_SOLID, 2, RGB(255, 255, 255));

   }


   style::~style()
   {

   }


   void toolbar::create_impact_toolbar(DesktopWindow * pdesktopwindow, style * pstyle)
   {

      m_pstyle = pstyle;
      int iToolbarWidth = 400;
      int iButtonSize = 24;
      int iDesktopWidth = 1920;
      m_rectangle ={((iDesktopWidth - iToolbarWidth)/2), 0, ((iDesktopWidth+iToolbarWidth)/2), iButtonSize};

      m_pdesktopwindow = pdesktopwindow;


      //m_brushBackgroundMinimizeDash(RGB(255, 255, 255))

      int iButtonCount = 3;
      int iMarginRight = iButtonSize /3;
      int iButton =  0;
      {
         auto pbutton =øallocate toolbar_button();
         pbutton->m_rectangle.set_top_left(iToolbarWidth - iMarginRight + iButtonSize*(iButton - iButtonCount), 0);
         pbutton->m_rectangle.set_size(iButtonSize, iButtonSize);
         //pbutton->m_pbrushBackground = m_pbrushButtonBackground;
         pbutton->m_pstyle = m_pstyle;
         pbutton->m_pdesktopwindow = pdesktopwindow;
         pbutton->m_pcontrolParent = this;
         pbutton->m_eid = id_minimize;
         m_controlaChildren.add(pbutton);
      }
      iButton++;
      {
         auto pbutton =øallocate toolbar_button();
         pbutton->m_rectangle.set_top_left(iToolbarWidth - iMarginRight + iButtonSize*(iButton - iButtonCount), 0);
         pbutton->m_rectangle.set_size(iButtonSize, iButtonSize);
         //pbutton->m_pbrushBackground = m_pbrushButtonBackground;
         pbutton->m_pstyle = m_pstyle;
         pbutton->m_pdesktopwindow = pdesktopwindow;
         pbutton->m_pcontrolParent = this;
         pbutton->m_eid = id_restore;
         m_controlaChildren.add(pbutton);
      }
      iButton++;
      {
         auto pbutton =øallocate toolbar_button();
         pbutton->m_rectangle.set_top_left(iToolbarWidth - iMarginRight + iButtonSize*(iButton - iButtonCount), 0);
         pbutton->m_rectangle.set_size(iButtonSize, iButtonSize);
         //pbutton->m_pbrushBackground = m_pbrushButtonBackground;
         pbutton->m_pstyle = m_pstyle;
         pbutton->m_pdesktopwindow = pdesktopwindow;
         pbutton->m_pcontrolParent = this;
         pbutton->m_eid = id_close;
         m_controlaChildren.add(pbutton);
      }

   }

   // bool toolbar::on_mouse(bool bPress, const ::int_point& position)
   // {
   //
   //    auto bHoverNew= m_rectangle.contains(::int_point{position.x, position.y});
   //    if (is_different(bHoverNew, m_bHover))
   //    {
   //
   //       m_bHover = bHoverNew;
   //
   //       m_pdesktopwindow->repaint(m_rectangle);
   //
   //    }
   //    if (m_pbuttonMinimize->on_mouse(bPress, position))
   //    {
   //
   //       return true;
   //
   //    }
   //    if (m_pbuttonRestore->on_mouse(bPress, position))
   //    {
   //
   //       return true;
   //
   //    }
   //    if (m_pbuttonClose->on_mouse(bPress, position))
   //    {
   //
   //       return true;
   //
   //    }
   //    return false;
   // }



   bool toolbar::on_button_click(enum_id eid)
   {
      return false;
   }

   bool control::on_button_click(enum_id eid)
   {

      if (eid==id_minimize)
      {
         //m_pdesktopwindow->m_viewerCore->ge

         ::ShowWindow(::GetParent(m_pdesktopwindow->getHWnd()), SW_MINIMIZE);

      }
      else if (eid==id_restore)
      {
         //m_pdesktopwindow->m_viewerCore->ge
         //BOOL enable = TRUE; // Use TRUE to force disable, which is counter-intuitive but how the flag works
         //HRESULT hr = DwmSetWindowAttribute(::GetParent(m_pdesktopwindow->getHWnd()), DWMWA_TRANSITIONS_FORCEDISABLED, &enable, sizeof(enable));

         ::PostMessageA(::GetParent(m_pdesktopwindow->getHWnd()), ViewerWindow::WM_USER_SWITCH_FULL_SCREEN_MODE, 0, 0);

      }
      else if (eid==id_close)
      {
         //m_pdesktopwindow->m_viewerCore->ge

         ::PostMessageA(::GetParent(m_pdesktopwindow->getHWnd()), ViewerWindow::WM_USER_DISCONNECT, 0, 0);
         ::PostMessageA(::GetParent(m_pdesktopwindow->getHWnd()), WM_CLOSE, 0, 0);

      }

      return false;

   }


   void control::__000OnDraw(GraphicsPlus * pgraphics, const ::int_rectangle & rectangle)
   {
      if (!rectangle.intersects(get_window_rectangle()))
      {
         return;
      }
         __001OnDraw(pgraphics, rectangle);

         for (auto & pcontrol:m_controlaChildren)
         {
            pcontrol->__000OnDraw(pgraphics, rectangle);
         }


   }
   bool toolbar::_001OnMouse(bool bPress, const ::int_point& pointRoot, const ::int_point& pointClient)
   {

      if (m_bHover || get_client_rectangle().contains(pointClient))
      {

         control::_001OnMouse(bPress, pointRoot, pointClient);

         bool bAnyChildHover = false;

         for (auto& pcontrol : m_controlaChildren)
         {

            if (pcontrol->m_bHover)
            {

               bAnyChildHover = true;

            }

         }

         if (m_bDrag)
         {

            auto xNew = pointRoot.x - m_xCursorDragStart + m_xWindowDragStart;

            if (xNew < 0)
            {

               xNew = 0;

            }
            else if (xNew > 1920 - m_rectangle.width())
            {

               xNew = 1920 - m_rectangle.width();

            }

            if (xNew != m_rectangle.left)
            {

               auto r1 = get_window_rectangle();
               m_rectangle.set_top_left(xNew, 0);
               auto r2 = get_window_rectangle();

               auto r = r1.unite(r2);

               //m_pdesktopwindow->repaint(r);

               add_repaint(r);
            }

         }


         if (m_bHover && !bAnyChildHover)
         {

            if (bPress)
            {

               if (!m_bDrag)
               {
                  m_bDrag = true;

                  m_xCursorDragStart = pointRoot.x;
                  m_xWindowDragStart = m_rectangle.left;
               }

            }

         }

         if (m_bDrag)
         {

            if (!bPress)
            {

               m_bDrag = false;

            }


         }

         m_pdesktopwindow->m_bShowCursor = m_bHover;

      }

      return false;

   }


   void control::__000OnTopDraw(HDC hdc, const ::int_rectangle & rectangle)
   {

      auto rIntersect = rectangle.intersection(m_rectangle);

      if (rIntersect.is_empty())
      {

         return;

      }

      Gdiplus::Graphics g(hdc);

      int w = m_rectangle.width();
      int h = m_rectangle.height();

      //auto sizeBitmap = m_pbitmapBuffer->GetBit

      if (::is_null(m_pbitmapBuffer) || m_pbitmapBuffer->GetWidth() != w
         || m_pbitmapBuffer->GetHeight() != h)
      {
         if (::is_set(m_pbitmapBuffer))
         {

            delete m_pbitmapBuffer;

         }

         m_pbitmapBuffer = new  Gdiplus::Bitmap(w, h, PixelFormat32bppARGB);

         if (::is_set(m_pgraphicsBuffer))
         {

            delete m_pgraphicsBuffer;

         }

         m_pgraphicsBuffer = new ::Gdiplus::Graphics(m_pbitmapBuffer);
      }
      // 2. Create graphics from bitmap
      //Gdiplus::Graphics gBuffer(&buffer);
      // Optional: high quality
      GraphicsPlus gplus(m_pgraphicsBuffer);
      __000OnDraw(&gplus, rectangle);
      //gBuffer.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

      // // 3. Draw to the offscreen buffer
      // SolidBrush bgBrush(Color(255, 255, 255, 255));
      // gBuffer.FillRectangle(&bgBrush, 0, 0, width, height);
      //
      // Pen pen(Color(255, 0, 120, 215), 4);
      // gBuffer.DrawEllipse(&pen, 50, 50, 200, 150);
      //
      // SolidBrush rectBrush(Color(180, 0, 128, 255));
      // gBuffer.FillRectangle(&rectBrush, 100, 80, 150, 100);

      // 4. Blit buffer to final graphics
      g.DrawImage(m_pbitmapBuffer, rIntersect.left, rIntersect.top,
         rIntersect.left-m_rectangle.left,
         rIntersect.top - m_rectangle.top,
         rIntersect.width(),
         rIntersect.height(),
         Gdiplus::UnitPixel);
   }

   void control::__001OnDraw(GraphicsPlus * pgraphics, const ::int_rectangle & rectangle)
   {
      ::color::color colorPaint;

      pgraphics->set_blend_mode();
      pgraphics->set_antialias_on();

      if (m_bHover || m_pcontrolParent->m_bHover)
      {
         colorPaint = argb(225, 255, 255, 255);
      }
      else
      {
         //colorPaint = argb(160, 240, 240, 240);
         colorPaint = argb(165, 215, 215, 215);

      }

      if (m_econtrol == e_control_button)
      {
         //SolidBrush * pbrushBackground = nullptr;
         // if (m_econtrol == e_control_toolbar)
         // {
         //    if (m_bHover)
         //    {
         //
         //       pbrushBackground = m_pstyle->m_pbrushBackgroundHover;
         //
         //    }
         //    else
         //    {
         //
         //       pbrushBackground = m_pstyle->m_pbrushBackground;
         //
         //    }
         //
         //    if (pbrushBackground)
         //    {
         //       auto r = get_window_rectangle();
         //       pgraphics->fillRect(r.left, r.top,r.right, r.bottom, pbrushBackground);
         //    }
         // }
         // else
         // {

            if (m_bHover)
            {
               auto r = get_paint_rectangle();
//               ::Gdiplus::Graphics g(pgraphics->m_dc->m_dc);
  //             g.SetCompositingMode(Gdiplus::CompositingModeSourceOver);
    ///;;           g.SetCompositingQuality(Gdiplus::CompositingQualityHighQuality);

               r.bottom -= 3;
               //r.deflate(2, 2);
               //Gdiplus::Rect gdiplusr;
               //::copy(gdiplusr, r);
               auto color = ::argb(20, 255, 255, 255);
               pgraphics->fill_solid_rectangle(r, color);
            }


         //}


      }



      if (m_eid == id_minimize)
      {


         auto r = get_paint_rectangle();

         ::int_rectangle rDash;

         rDash.left = r.left + 7;
         rDash.right = r.right - 7;
         rDash.top = r.top + 13;
         rDash.bottom = rDash.top + 2;

         pgraphics->fill_solid_rectangle(rDash, colorPaint);

      }
      else if (m_eid == id_restore)
      {


         auto r = get_paint_rectangle();

         ::int_rectangle rDeflate = r;

         rDeflate.deflate(7, 5, 7, 9);

         pgraphics->fill_solid_rectangle(::int_rectangle(rDeflate.left, rDeflate.top, rDeflate.right, rDeflate.top + 2), colorPaint);
         pgraphics->fill_solid_rectangle(::int_rectangle(rDeflate.right - 2, rDeflate.top, rDeflate.right, rDeflate.bottom), colorPaint);
         pgraphics->fill_solid_rectangle(::int_rectangle(rDeflate.left, rDeflate.bottom-2, rDeflate.right, rDeflate.bottom), colorPaint);
         pgraphics->fill_solid_rectangle(::int_rectangle(rDeflate.left, rDeflate.top, rDeflate.left+2, rDeflate.bottom), colorPaint);

      }
      else if (m_eid == id_close)
      {


         auto r = get_paint_rectangle();

         ::int_rectangle rDeflate = r;

         rDeflate.deflate(7, 5, 7, 9);

         //pgraphics->setPen(m_pstyle->m_ppenPaint);
         pgraphics->setPen(2.0f, colorPaint);

         pgraphics->moveTo(rDeflate.left, rDeflate.top);
         pgraphics->lineTo(rDeflate.right, rDeflate.bottom);
         pgraphics->moveTo(rDeflate.right, rDeflate.top);
         pgraphics->lineTo(rDeflate.left, rDeflate.bottom);

      }
   }

   void toolbar::__001OnDraw(GraphicsPlus * pgraphics, const ::int_rectangle & rectangle)
   {

      pgraphics->set_copy_source_mode();
      pgraphics->set_antialias_off();


      int iAlpha = 180;

      if (m_bHover)
      {

         iAlpha = 255;

      }

      ::color::color colorLite = argb(iAlpha, 200, 240, 255);
      ::color::color colorDark = argb(iAlpha, 50, 80, 160);
      auto r = get_paint_rectangle();


      pgraphics->fill_solid_rectangle(r, colorDark);

      int x = r.left;
      int y = r.top;
      int w = r.width();
      int h = 1;
      ::color::color color;

      float fOpacity = 1.0f;
      for (int i = 0; i < 10; i++)
      {
         color=colorDark;
         color.blend(colorLite, fOpacity);
         pgraphics->fill_solid_rectangle(::int_rectangle_dimension(x, y, w, h), color);

         y+= h;
         fOpacity -= 0.1f;
      }

      Gdiplus::RectF layoutRect;

      ::copy(layoutRect, r);

      //layoutRect.Height = 24;

      ::wstring wstr(m_pdesktopwindow->m_strHost);

      auto& graphics = *pgraphics->m_pgraphics;
      using namespace Gdiplus;
      // Enable better quality rendering
      graphics.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);
      graphics.SetCompositingMode(Gdiplus::CompositingModeSourceOver);
      FLOAT fontSize = 12.f;
      // Create Segoe UI font
      Font font(L"Segoe UI", fontSize, FontStyleRegular, UnitPixel);

      ::Gdiplus::Color colorPaint;


      if (m_bHover)
      {
         colorPaint = Gdiplus::Color(225, 255, 255, 255);
      }
      else
      {
         //colorPaint = argb(160, 240, 240, 240);
         colorPaint = Gdiplus::Color(165, 215, 215, 215);

      }

      // Create white brush (ARGB: 255 = fully opaque)
      Gdiplus::SolidBrush brush(colorPaint);

      // Create string format
      StringFormat stringFormat;

      // Horizontal alignment (center)
      stringFormat.SetAlignment(StringAlignmentCenter);

      // Vertical alignment (center)
      stringFormat.SetLineAlignment(StringAlignmentCenter);

      // Optional: prevent wrapping
      stringFormat.SetFormatFlags(StringFormatFlagsNoWrap);

      stringFormat.SetFormatFlags(StringFormatFlagsNoClip);

      // Draw the string
      graphics.DrawString(
         wstr.c_str(),
         wstr.size(),                 // auto-length
         &font,
         layoutRect,
         &stringFormat,
         &brush
      );
      //pgraphics->m_pgraphics->DrawString(wstr, wstr.size(),)
      // color=colorDark;
      // color.blend(colorLite, 0.6);
      // pgraphics->fill_solid_rectangle(::int_rectangle_dimension(x, y, w, h), color);
      //
      // y+= h;
      // color=colorDark;
      // color.blend(colorLite, 0.4);
      // pgraphics->fill_solid_rectangle(::int_rectangle_dimension(x, y, w, h), color);
      //
      // y+= h;
      // color=colorDark;
      // color.blend(colorLite, 0.2);
      // pgraphics->fill_solid_rectangle(::int_rectangle_dimension(x, y, w, h), color);
      //

   }

} // namespace remoting
