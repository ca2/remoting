//
// Created by camilo on 2026-02-11 23:18 <3ThomasBorregaardSørensen!!
//
#include "framework.h"
#include "DesktopWindow.h"
#include "ViewerWindow.h"
#include "impact_toolbar.h"

#include "apex/innate_subsystem/drawing/Bitmap.h"
#include "apex/innate_subsystem/drawing/Font.h"
// // #include aaa_<dwmapi.h>
// // Link with dwmapi.lib
// #pragma comment (lib, "dwmapi.lib")
// #pragma comment (lib, "gdiplus.lib")
// // #include aaa_<gdiplus.h>

namespace remoting_remoting
{
   control::control()
   {
      m_econtrol  = e_control_none;
      m_eid = id_none;
      //m_pbitmapBuffer = nullptr;
      //m_pgraphicsBuffer = nullptr;
      m_bNewRepaintRectangle = true;
      m_bDrag = false;
      m_bHover = false;

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

         auto r = rectangle;

         auto fScale = m_pdesktopwindow->m_scManager.getScale();

         if (fScale <= 0.f)
         {

            return;

         }

         r /= fScale;

         pwindowPaint->_add_repaint(r );

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


   void control::set_hover_false()
   {

       m_bHover = false;

       for (auto& pcontrol : m_controlaChildren)
       {

           pcontrol->set_hover_false();

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

   bool control::_001OnMouseEx(unsigned int uMessage, int iButtonMask, const ::int_point& pointRoot, const ::int_point& pointClient)
   {

      auto rectangleClient = get_client_rectangle();

      if (uMessage == WM_LBUTTONDOWN)
      {
         //if (!m_bPressed)
         {

            //m_bPressed = true;

            if (on_left_down(pointClient))
            {

                return true;

            }

         }
      }
      else if(uMessage == WM_LBUTTONUP)
      {

         //if (m_bPressed)
         {

            if (on_left_up(pointClient))
            {
            //    m_bPressed = false;


                return true;

            }
            //m_bPressed = false;


         }

      }

      return false;

   }

   //bool control::_001OnMouseEx(unsigned int uMessage, const ::int_point& pointRoot, const ::int_point& pointClient)
   //{

   //    auto rectangleClient = get_client_rectangle();

   //    auto bHoverNew = rectangleClient.contains(::int_point{ pointClient.x, pointClient.y });

   //    if (is_different(bHoverNew, m_bHover))
   //    {

   //        m_bHover = bHoverNew;

   //        add_repaint(get_window_rectangle());

   //    }

   //    if (uMessage == WM_LBUTTONDOWN)
   //    {
   //     //m_bPressed = true;

   //     if (on_left_down(pointClient))
   //     {

   //         return true;

   //     }
   //    }
   //    else if (uMessage == WM_LBUTTONUP)
   //    {

   //         if (on_left_up(pointClient))
   //         {
   //             //m_bPressed = false;


   //             return true;

   //         }
   //            //m_bPressed = false;
   //    }

   //    return false;

   //}


   bool control::on_left_down(const ::int_point& position)
   {

       return false;

   }


   bool control::on_left_up(const ::int_point& position)
   {

      return false;
   }


   bool control::_000OnMouseEx(unsigned int uMessage, int iButtonMask, const ::int_point& pointRoot, const ::int_point& pointClientParam)
   {

      auto pointClient = pointClientParam;
      pointClient.x -= m_rectangle.left;
      pointClient.y -= m_rectangle.top;

      auto rClient = get_client_rectangle();

      bool bHoverNew = rClient.contains(pointClient);

      if (is_different(bHoverNew, m_bHover))
      {

          m_bHover = bHoverNew;

          if (!m_bHover)
          {

              set_hover_false();

          }

          add_repaint(get_window_rectangle());

      }

      if (!m_bLButtonDown && !bHoverNew)
      {

          return false;

      }


      if (uMessage == WM_LBUTTONDOWN)
      {

          m_bLButtonDown = true;

      }
      else if (uMessage == WM_RBUTTONDOWN)
      {

          m_bLButtonDown = false;

      }
      else if (!(iButtonMask & ::innate_subsystem::e_mouse_left))
      {

          m_bLButtonDown = false;

      }

      for (auto& pcontrol : m_controlaChildren)
      {

          if (pcontrol->_000OnMouseEx(uMessage, iButtonMask, pointRoot, pointClient))
          {
              
              return true;

          }

      }

      if (_001OnMouseEx(uMessage, iButtonMask, pointRoot, pointClient))
      {

         return true;

      }

      return true;


   }

   //bool control::_000OnMouseEx(unsigned int uMessage, const ::int_point& pointRoot, const ::int_point& pointClientParam)
   //{

   //    auto pointClient = pointClientParam;
   //    pointClient.x -= m_rectangle.left;
   //    pointClient.y -= m_rectangle.top;

   //    for (auto& pcontrol : m_controlaChildren)
   //    {

   //        if (pcontrol->_000OnMouseEx(uMessage, pointRoot, pointClient))
   //        {
   //            return true;
   //        }

   //    }
   //    if (_001OnMouseEx(uMessage, pointRoot, pointClient))
   //    {

   //        return true;

   //    }


   //    return false;

   //}

   toolbar_button::toolbar_button()
   {
      m_econtrol = e_control_button;
   }

   toolbar_button::~toolbar_button()
   {

   }


   bool toolbar_button::on_left_down(const ::int_point& position)
   {

       m_bPressed = true;
       m_timePressed.Now();
       return true;

   }
   bool toolbar_button::on_left_up(const ::int_point& position)
   {

       if (m_bPressed && m_timePressed.elapsed() < 5_s)
       {
           on_button_click(m_eid);

           return true;

       }

       return false;

   }


   // bool toolbar_button::on_mouse(bool bPress, const ::int_point& position)
   //{


   style::style()
   {

      m_brushBackground.initialize_solid_brush(argb(255, 40, 120, 180));
      m_brushBackgroundHover.initialize_solid_brush(argb(255, 40, 120, 220));
      m_brushButtonBackground.initialize_solid_brush(argb(255, 40, 120, 180));
      m_brushButtonBackgroundHover.initialize_solid_brush(argb(255, 50, 130, 230));
      m_brushButtonPaint.initialize_solid_brush(argb(255, 255, 255, 255));
      m_penPaint.initialize_pen(::innate_subsystem::e_pen_solid, 2, argb(255, 255, 255, 255));

   }


   style::~style()
   {

   }


   void toolbar::on_size()
   {

      //m_iDesktopWidth = iDesktopWidthParameter * fScaleParameter;
      //m_fScale = fScaleParameter;
      auto pdesktopwindow = m_pdesktopwindow;

      if (::is_null(pdesktopwindow))
      {

         throw ::exception(error_failed);

      }
      int iDesktopWidth = pdesktopwindow->m_scManager.getScaledRect().width();
      if (iDesktopWidth < 100)
      {

         return;

      }
      //int iToolbarWidth = 400 * iDesktopWidth / (1920 * iDivisor);
      //int iButtonSize = 24 * iDesktopWidth / (1920 * iDivisor);
      int iToolbarWidth = 400;
      int iButtonSize = 24;
      m_rectangle = { ((iDesktopWidth - iToolbarWidth) / 2), 0, ((iDesktopWidth + iToolbarWidth) / 2), iButtonSize };

      //m_brushBackgroundMinimizeDash(RGB(255, 255, 255))

      int iButtonCount = 3;
      int iMarginRight = iButtonSize / 3;
      int iButton = 0;
      {
         if (!m_pbuttonMinimize)
         {

            m_pbuttonMinimize = allocateø toolbar_button();

            m_pbuttonMinimize->m_pstyle = m_pstyle;
            m_pbuttonMinimize->m_pdesktopwindow = pdesktopwindow;
            m_pbuttonMinimize->m_pcontrolParent = this;
            m_pbuttonMinimize->m_eid = id_minimize;
            //m_pbuttonMinimize->m_pbrushBackground = m_pbrushButtonBackground;
            m_controlaChildren.add(m_pbuttonMinimize);
         }
         m_pbuttonMinimize->m_rectangle.set_top_left(iToolbarWidth - iMarginRight + iButtonSize * (iButton - iButtonCount), 0);
         m_pbuttonMinimize->m_rectangle.set_size(iButtonSize, iButtonSize);
         
      }
      iButton++;
      {
         if (!m_pbuttonRestore)
         {
            m_pbuttonRestore = allocateø toolbar_button();
            m_pbuttonRestore->m_pstyle = m_pstyle;
            m_pbuttonRestore->m_pdesktopwindow = pdesktopwindow;
            m_pbuttonRestore->m_pcontrolParent = this;
            m_pbuttonRestore->m_eid = id_restore;
            //m_pbuttonRestore->m_pbrushBackground = m_pbrushButtonBackground;
            m_controlaChildren.add(m_pbuttonRestore);

         }
         m_pbuttonRestore->m_rectangle.set_top_left(iToolbarWidth - iMarginRight + iButtonSize * (iButton - iButtonCount), 0);
         m_pbuttonRestore->m_rectangle.set_size(iButtonSize, iButtonSize);
      }
         
      iButton++;
      {
         if (!m_pbuttonClose)
         {
            m_pbuttonClose = allocateø toolbar_button();
            m_pbuttonClose->m_pstyle = m_pstyle;
            m_pbuttonClose->m_pdesktopwindow = pdesktopwindow;
            m_pbuttonClose->m_pcontrolParent = this;
            m_pbuttonClose->m_eid = id_close;
            //m_pbuttonClose->m_pbrushBackground = m_pbrushButtonBackground;
            m_controlaChildren.add(m_pbuttonClose);

         }
         m_pbuttonClose->m_rectangle.set_top_left(iToolbarWidth - iMarginRight + iButtonSize * (iButton - iButtonCount), 0);
         m_pbuttonClose->m_rectangle.set_size(iButtonSize, iButtonSize);
      }


   }

   void toolbar::create_impact_toolbar(DesktopWindow * pdesktopwindow, style * pstyle)
   {

      m_pstyle = pstyle;

      m_pdesktopwindow = pdesktopwindow;

      //set_desktop_width(1920);

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
          m_pdesktopwindow->m_bMinimized = true;
          if (m_pdesktopwindow->m_pviewerwindow->m_isFullScr)
          {
              m_pdesktopwindow->m_pviewerwindow->m_isMinimizedFromFullScreen = true;
              m_pdesktopwindow->m_pviewerwindow->doMinimizeFromFullScreen();

          }
         ::ShowWindow(::GetParent((HWND) m_pdesktopwindow->_HWND()), SW_MINIMIZE);
         
         return true;
      }
      else if (eid==id_restore)
      {
         //m_pdesktopwindow->m_viewerCore->ge
         //bool enable = true; // Use true to force disable, which is counter-intuitive but how the flag works
         //HRESULT hr = DwmSetWindowAttribute(::GetParent((HWND) m_pdesktopwindow->_HWND()), DWMWA_TRANSITIONS_FORCEDISABLED, &enable, sizeof(enable));

         ::PostMessageA(::GetParent((HWND) m_pdesktopwindow->_HWND()), ViewerWindow::WM_USER_SWITCH_FULL_SCREEN_MODE, 0, 0);
         return true;
      }
      else if (eid==id_close)
      {
         //m_pdesktopwindow->m_viewerCore->ge

         ::PostMessageA(::GetParent((HWND) m_pdesktopwindow->_HWND()), ViewerWindow::WM_USER_DISCONNECT, 0, 0);
         ::PostMessageA(::GetParent((HWND) m_pdesktopwindow->_HWND()), WM_CLOSE, 0, 0);
         return true;
      }

      return false;

   }


   void control::__000OnDraw(::innate_subsystem::GraphicsInterface * pgraphics, const ::int_rectangle & rectangle)
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


   bool toolbar::_001OnMouseEx(unsigned int uMessage, int iButtonMask, const ::int_point& pointRoot, const ::int_point& pointClient)
   {

      //auto pointClient = pointClientParameter * m_pdesktopwindow->m_scManager.getScale();

      auto rClient = get_client_rectangle();

      if (m_bHover || m_bDrag || rClient.contains(pointClient))
      {

         control::_001OnMouseEx(uMessage, iButtonMask, pointRoot, pointClient);

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
            else if (xNew > m_pdesktopwindow->m_scManager.getScaledRect().width() - m_rectangle.width())
            {

               xNew = m_pdesktopwindow->m_scManager.getScaledRect().width() - m_rectangle.width();

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

            if (uMessage == WM_LBUTTONDOWN)
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

            if(uMessage == WM_LBUTTONUP)
            {

               m_bDrag = false;

            }


         }

         //m_pdesktopwindow->m_bShowCursor = m_bHover;

      }

      return false;

   }


   void control::__000OnTopDraw(::innate_subsystem::GraphicsInterface * pgraphics, const ::int_rectangle & rectangle)
   {

      auto rIntersect = rectangle.intersection(m_rectangle);

      if (rIntersect.is_empty())
      {

         return;

      }

      //Gdiplus::Graphics g(hdc);

      auto size = m_rectangle.size();
      //int h = m_rectangle.height();

      //auto sizeBitmap = m_pbitmapBuffer->GetBit

      if (::is_null(m_pbitmapBuffer) || m_pbitmapBuffer->getSize() != size)
      {
         // if (::is_set(m_pbitmapBuffer))
         // {
         //
         //    m_pbitmapBuffer;
         //
         // }

         constructø(m_pbitmapBuffer);

         m_pbitmapBuffer->initialize_bitmap(size);

         // if (::is_set(m_pgraphicsBuffer))
         // {
         //
         //    delete m_pgraphicsBuffer;
         //
         // }

         constructø(m_pgraphicsBuffer);

         m_pgraphicsBuffer->initialize_graphics(m_pbitmapBuffer);
      }
      // 2. Create graphics from bitmap
      //Gdiplus::Graphics gBuffer(&buffer);
      // Optional: high quality
      //GraphicsPlus gplus(m_pgraphicsBuffer);
      //__000OnDraw(&gplus, rectangle);
      __000OnDraw(m_pgraphicsBuffer, rectangle);
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

      // Gdiplus::Graphics g;
      //
      // g.DrawImage(m_pbitmapBuffer, rIntersect.left, rIntersect.top,
      //    rIntersect.left-m_rectangle.left,
      //    rIntersect.top - m_rectangle.top,
      //    rIntersect.width(),
      //    rIntersect.height(),
      //    Gdiplus::UnitPixel);
      pgraphics->drawBitmap(m_pbitmapBuffer,rIntersect.top_left(),
         { (::int_point) (rIntersect.origin()-m_rectangle.origin()),  rIntersect.size()});
   }

   void control::__001OnDraw(::innate_subsystem::GraphicsInterface * pgraphics, const ::int_rectangle & rectangle)
   {
      ::color::color colorPaint;

      pgraphics->setBlendModeOn(true);
      pgraphics->setAntiAliasOn(true);

      if (m_bHover || m_bLButtonDown)
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
               pgraphics->fillRect(r, color);
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

         pgraphics->fillRect(rDash, colorPaint);

      }
      else if (m_eid == id_restore)
      {


         auto r = get_paint_rectangle();

         ::int_rectangle rDeflate = r;

         rDeflate.deflate(7, 5, 7, 9);

         pgraphics->fillRect(::int_rectangle(rDeflate.left, rDeflate.top, rDeflate.right, rDeflate.top + 2), colorPaint);
         pgraphics->fillRect(::int_rectangle(rDeflate.right - 2, rDeflate.top, rDeflate.right, rDeflate.bottom), colorPaint);
         pgraphics->fillRect(::int_rectangle(rDeflate.left, rDeflate.bottom-2, rDeflate.right, rDeflate.bottom), colorPaint);
         pgraphics->fillRect(::int_rectangle(rDeflate.left, rDeflate.top, rDeflate.left+2, rDeflate.bottom), colorPaint);

      }
      else if (m_eid == id_close)
      {


         auto r = get_paint_rectangle();

         ::int_rectangle rDeflate = r;

         rDeflate.deflate(7, 5, 7, 9);

         //pgraphics->setPen(m_pstyle->m_ppenPaint);
         if (!m_ppen001)
         {
            constructø(m_ppen001);
            m_ppen001->initialize_pen(innate_subsystem::e_pen_solid, 2, colorPaint);
         }
         //pgraphics->setPen(2.0f, colorPaint);
         pgraphics->setPen(m_ppen001);

         pgraphics->moveTo(rDeflate.top_left());
         pgraphics->lineTo(rDeflate.bottom_right());
         pgraphics->moveTo(rDeflate.top_right());
         pgraphics->lineTo(rDeflate.bottom_left());

      }
   }

   void toolbar::__001OnDraw(innate_subsystem::GraphicsInterface * pgraphics, const ::int_rectangle & rectangle)
   {

      pgraphics->setBlendModeOn(false);
      pgraphics->setAntiAliasOn(false);


      int iAlpha = 180;

      if (m_bHover)
      {

         iAlpha = 255;

      }

      ::color::color colorLite = argb(iAlpha, 200, 240, 255);
      ::color::color colorDark = argb(iAlpha, 50, 80, 160);
      auto r = get_paint_rectangle();


      pgraphics->fillRect(r, colorDark);

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
         pgraphics->fillRect(::int_rectangle_dimension(x, y, w, h), color);

         y+= h;
         fOpacity -= 0.1f;
      }

      if (!m_pfont001)
      {

         constructø(m_pfont001);

         m_pfont001->initialize_font("Segoe UI", 14, 400);

      }

      pgraphics->setTextRenderingHintClearType();
      pgraphics->setBlendModeOn(true);

      // Gdiplus::RectF layoutRect;
      //
      // ::copy(layoutRect, r);
      //
      // //layoutRect.Height = 24;
      //
      // ::wstring wstr(m_pdesktopwindow->m_strHost);
      //
      // auto& graphics = *pgraphics->m_pgraphics;
      // using namespace Gdiplus;
      // // Enable better quality rendering
      // graphics.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);
      // graphics.SetCompositingMode(Gdiplus::CompositingModeSourceOver);
      // FLOAT fontSize = 12.f;
      // // Create Segoe UI font
      // Font font(L"Segoe UI", fontSize, FontStyleRegular, UnitPixel);
      //
      // ::Gdiplus::Color colorPaint;

      ::color::color colorPaint;

      if (m_bHover)
      {
         colorPaint = argb(225, 255, 255, 255);
      }
      else
      {
         //colorPaint = argb(160, 240, 240, 240);
         colorPaint = argb(165, 215, 215, 215);

      }

      pgraphics->setTextColor(colorPaint);

      // // Create white brush (ARGB: 255 = fully opaque)
      // Gdiplus::SolidBrush brush(colorPaint);
      //
      // // Create string format
      // StringFormat stringFormat;
      //
      // // Horizontal alignment (center)
      // stringFormat.SetAlignment(StringAlignmentCenter);
      //
      // // Vertical alignment (center)
      // stringFormat.SetLineAlignment(StringAlignmentCenter);
      //
      // // Optional: prevent wrapping
      // stringFormat.SetFormatFlags(StringFormatFlagsNoWrap);
      //
      // stringFormat.SetFormatFlags(StringFormatFlagsNoClip);
      //
      // // Draw the string
      // graphics.DrawString(
      //    wstr.c_str(),
      //    wstr.size(),                 // auto-length
      //    &font,
      //    layoutRect,
      //    &stringFormat,
      //    &brush
      // );

      pgraphics->setFont(m_pfont001);

      pgraphics->drawText(m_pdesktopwindow->m_strHost,
         m_pdesktopwindow->m_strHost.length(),
         r, 0, e_align_center);
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

} // namespace remoting_remoting
