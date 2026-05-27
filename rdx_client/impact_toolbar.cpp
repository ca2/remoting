//
// Created by camilo on 2026-02-11 23:18 <3ThomasBorregaardSørensen!!
//
#include "impact_toolbar.h"
#include "DesktopWindow.h"
#include "___aaa2026Q1/ViewerWindow.h"
#include "framework.h"

#include "innate_subsystem/drawing/Bitmap.h"
#include "innate_subsystem/drawing/Font.h"
// // #include aaa_<dwmapi.h>
// // Link with dwmapi.lib
// #pragma comment (lib, "dwmapi.lib")
// #pragma comment (lib, "gdiplus.lib")
// // #include aaa_<gdiplus.h>

namespace remoting_client
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


   void add_repaint(const ::i32_rectangle & rectangle);

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


   ::i32_rectangle control::get_client_rectangle()
   {

      return m_rectangle.size();
   }

   void control::_add_repaint(const ::i32_rectangle & rectangle)
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
   void control::add_repaint(const i32_rectangle & rectangle)
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

   void control::on_hover()
   {


   }

   void control::on_hover_off()
   {

       m_bHover = false;

       for (auto& pcontrol : m_controlaChildren)
       {

           pcontrol->on_hover_off();

       }

   }


   ::i32_rectangle control::get_window_rectangle()
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



   ::i32_rectangle control::get_paint_rectangle()
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

   bool control::_001OnMouseEx(::u32 uMessage, ::i32 iButtonMask, const ::i32_point &pointRoot,
                               const ::i32_point &pointClient, bool &bDoDefaultProcessing)
   {

      auto rectangleClient = get_client_rectangle();

      if (uMessage == ::user::e_message_left_button_down)
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
      else if(uMessage == ::user::e_message_left_button_up)
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

   //bool control::_001OnMouseEx(::u32 uMessage, const ::i32_point& pointRoot, const ::i32_point& pointClient)
   //{

   //    auto rectangleClient = get_client_rectangle();

   //    auto bHoverNew = rectangleClient.contains(::i32_point{ pointClient.x, pointClient.y });

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


   bool control::on_left_down(const ::i32_point& pointPosition)
   {

       return false;

   }


   bool control::on_left_up(const ::i32_point& pointPosition)
   {

      return false;
   }


   bool control::_000OnMouseEx(::u32 uMessage, ::i32 iButtonMask, const ::i32_point &pointRoot,
                               const ::i32_point &pointClientParam, bool &bDoDefaultProcessing)
   {

      auto pointClient = pointClientParam;
      pointClient.x -= m_rectangle.left;
      pointClient.y -= m_rectangle.top;

      auto rClient = get_client_rectangle();

      bool bHoverNew = rClient.contains(pointClient);

      if (is_different(bHoverNew, m_bHover))
      {

          m_bHover = bHoverNew;

          if (m_bHover)
          {

              on_hover();

          }
          else
          {

              on_hover_off();

          }

          add_repaint(get_window_rectangle());

      }

      if (!m_bLButtonDown && !bHoverNew)
      {

          if (m_bHover)
          {

              if (uMessage == ::user::e_message_mouse_move)
             {
                bDoDefaultProcessing = true;
                return true;
             }

          }

          return false;

      }


      if (uMessage == ::user::e_message_left_button_down)
      {

          m_bLButtonDown = true;

      }
      else if (uMessage == ::user::e_message_right_button_down)
      {

          m_bLButtonDown = false;

      }
      else if (!(iButtonMask & ::innate_subsystem::e_mouse_left))
      {

          m_bLButtonDown = false;

      }

      for (auto& pcontrol : m_controlaChildren)
      {

          if (pcontrol->_000OnMouseEx(uMessage, iButtonMask, pointRoot, pointClient, bDoDefaultProcessing))
          {
              
              return true;

          }

      }

      if (_001OnMouseEx(uMessage, iButtonMask, pointRoot, pointClient, bDoDefaultProcessing))
      {

         return true;

      }

      return true;


   }

   //bool control::_000OnMouseEx(::u32 uMessage, const ::i32_point& pointRoot, const ::i32_point& pointClientParam)
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


   bool toolbar_button::on_left_down(const ::i32_point& pointPosition)
   {

       m_bPressed = true;
       m_timePressed.Now();
       return true;

   }


   bool toolbar_button::on_left_up(const ::i32_point& pointPosition)
   {

       if (m_bPressed && m_timePressed.elapsed() < 5_s)
       {
           on_button_click(m_eid);

           return true;

       }

       return false;

   }


   // bool toolbar_button::on_mouse(bool bPress, const ::i32_point& pointPosition)
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

      void toolbar::on_hover() { m_pdesktopwindow->setCursor(e_cursor_arrow); }

   void toolbar::on_hover_off()
   {

      m_pdesktopwindow->setCursor(e_cursor_none);

      control::on_hover_off();

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
      ::i32 iDesktopWidth = pdesktopwindow->m_scManager.getScaledRect().width();
      if (iDesktopWidth < 100)
      {

         return;

      }
      //::i32 iToolbarWidth = 400 * iDesktopWidth / (1920 * iDivisor);
      //::i32 iButtonSize = 24 * iDesktopWidth / (1920 * iDivisor);
      ::i32 iToolbarWidth = 400;
      ::i32 iButtonSize = 24;
      m_rectangle = { ((iDesktopWidth - iToolbarWidth) / 2), 0, ((iDesktopWidth + iToolbarWidth) / 2), iButtonSize };

      //m_brushBackgroundMinimizeDash(RGB(255, 255, 255))

      ::i32 iButtonCount = 3;
      ::i32 iMarginRight = iButtonSize / 3;
      ::i32 iButton = 0;
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

   // bool toolbar::on_mouse(bool bPress, const ::i32_point& pointPosition)
   // {
   //
   //    auto bHoverNew= m_rectangle.contains(::i32_point{pointPosition.x, pointPosition.y});
   //    if (is_different(bHoverNew, m_bHover))
   //    {
   //
   //       m_bHover = bHoverNew;
   //
   //       m_pdesktopwindow->repaint(m_rectangle);
   //
   //    }
   //    if (m_pbuttonMinimize->on_mouse(bPress, pointPosition))
   //    {
   //
   //       return true;
   //
   //    }
   //    if (m_pbuttonRestore->on_mouse(bPress, pointPosition))
   //    {
   //
   //       return true;
   //
   //    }
   //    if (m_pbuttonClose->on_mouse(bPress, pointPosition))
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

         system()->m_papplication->HideApplication();
         //m_pdesktopwindow->m_pviewerwindow->minimizeWindow();

         return true;
      }
      else if (eid==id_restore)
      {
         //m_pdesktopwindow->m_pviewercore->ge
         //bool enable = true; // Use true to force disable, which is counter-intuitive but how the flag works
         //HRESULT hr = DwmSetWindowAttribute(::GetParent((HWND) m_pdesktopwindow->_HWND()), DWMWA_TRANSITIONS_FORCEDISABLED, &enable, sizeof(enable));

         m_pdesktopwindow->m_pviewerwindow->postMessage(ViewerWindow::WM_USER_SWITCH_FULL_SCREEN_MODE, 0, 0);
         return true;
      }
      else if (eid==id_close)
      {
         //m_pdesktopwindow->m_pviewercore->ge

         m_pdesktopwindow->m_pviewerwindow->postMessage(ViewerWindow::WM_USER_DISCONNECT_NO_CONFIRM, 0, 0);
         //m_pdesktopwindow->m_pviewerwindow->postMessage(::user::e_message_close, 0, 0);
         return true;
      }

      return false;

   }


   void control::__000OnDraw(::innate_subsystem::GraphicsInterface * pgraphics, const ::i32_rectangle & rectangle)
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


   bool toolbar::_001OnMouseEx(::u32 uMessage, ::i32 iButtonMask, const ::i32_point& pointRoot, const ::i32_point& pointClient, bool & bDoDefaultProcessing)
   {

      //auto pointClient = pointClientParameter * m_pdesktopwindow->m_scManager.getScale();

      auto rClient = get_client_rectangle();

      if (m_bHover || m_bDrag || rClient.contains(pointClient))
      {

         control::_001OnMouseEx(uMessage, iButtonMask, pointRoot, pointClient, bDoDefaultProcessing);

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

            if (uMessage == ::user::e_message_left_button_down)
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

            if(uMessage == ::user::e_message_left_button_up)
            {

               m_bDrag = false;

            }


         }

         //m_pdesktopwindow->m_bShowCursor = m_bHover;

      }

      return false;

   }


   void control::__000OnTopDraw(::innate_subsystem::GraphicsInterface * pgraphics, const ::i32_rectangle & rectangle)
   {

      auto rIntersect = rectangle.intersection(m_rectangle);

      if (rIntersect.is_empty())
      {

         return;

      }

      //Gdiplus::Graphics g(hdc);

      auto size = m_rectangle.size();
      //::i32 h = m_rectangle.height();

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
      
      m_pgraphicsBuffer->setBlendModeOn(false);
      m_pgraphicsBuffer->setAntiAliasOn(false);
      
      ::i32_rectangle rectangleClearBuffer(size);
      
      m_pgraphicsBuffer->fillRect(rectangleClearBuffer, ::color::transparent);
      
      m_pgraphicsBuffer->setBlendModeOn(true);
      m_pgraphicsBuffer->setAntiAliasOn(true);

      
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
      
      pgraphics->setBlendModeOn(true);
      pgraphics->drawBitmap(m_pbitmapBuffer,rIntersect.top_left(),
         { (::i32_point) (rIntersect.origin()-m_rectangle.origin()),  rIntersect.size()});
   }


   void control::__001OnDraw(::innate_subsystem::GraphicsInterface * pgraphics, const ::i32_rectangle & rectangle)
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

         ::i32_rectangle rDash;

         rDash.left = r.left + 7;
         rDash.right = r.right - 7;
         rDash.top = r.top + 5;
         rDash.bottom = rDash.top + 2;

         pgraphics->fillRect(rDash, colorPaint);

      }
      else if (m_eid == id_restore)
      {


         auto r = get_paint_rectangle();

         ::i32_rectangle rDeflate = r;

         rDeflate.deflate(7, 5, 7, 9);

         pgraphics->fillRect(::i32_rectangle(rDeflate.left, rDeflate.top, rDeflate.right, rDeflate.top + 2), colorPaint);
         pgraphics->fillRect(::i32_rectangle(rDeflate.right - 2, rDeflate.top, rDeflate.right, rDeflate.bottom), colorPaint);
         pgraphics->fillRect(::i32_rectangle(rDeflate.left, rDeflate.bottom-2, rDeflate.right, rDeflate.bottom), colorPaint);
         pgraphics->fillRect(::i32_rectangle(rDeflate.left, rDeflate.top, rDeflate.left+2, rDeflate.bottom), colorPaint);

      }
      else if (m_eid == id_close)
      {


         auto r = get_paint_rectangle();

         ::i32_rectangle rDeflate = r;

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


   void toolbar::__001OnDraw(innate_subsystem::GraphicsInterface * pgraphics, const ::i32_rectangle & rectangle)
   {

      pgraphics->setBlendModeOn(false);
      pgraphics->setAntiAliasOn(false);


      ::i32 iAlpha = 180;

      if (m_bHover)
      {

         iAlpha = 222;

      }

      ::color::color colorLite = argb(iAlpha, 200, 240, 255);
      ::color::color colorDark = argb(iAlpha, 50, 80, 160);
      auto r = get_paint_rectangle();


      pgraphics->fillRect(r, colorDark);

      ::i32 x = r.left;
      ::i32 y = r.top;
      ::i32 w = r.width();
      ::i32 h = 1;
      ::color::color color;

      ::f32 fOpacity = 1.0f;
      
      for (::i32 i = 0; i < 10; i++)
      {
         
         color=colorDark;
         
         color.blend(colorLite, fOpacity);
         
         pgraphics->fillRect(::i32_rectangle_dimension(x, y, w, h), color);

         y+= h;
         
         fOpacity -= 0.1f;
         
      }
      
      pgraphics->setBlendModeOn(true);
      pgraphics->setAntiAliasOn(true);


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
         r, 0, e_align_center);
      //pgraphics->m_pgraphics->DrawString(wstr, wstr.size(),)
      // color=colorDark;
      // color.blend(colorLite, 0.6);
      // pgraphics->fill_solid_rectangle(::i32_rectangle_dimension(x, y, w, h), color);
      //
      // y+= h;
      // color=colorDark;
      // color.blend(colorLite, 0.4);
      // pgraphics->fill_solid_rectangle(::i32_rectangle_dimension(x, y, w, h), color);
      //
      // y+= h;
      // color=colorDark;
      // color.blend(colorLite, 0.2);
      // pgraphics->fill_solid_rectangle(::i32_rectangle_dimension(x, y, w, h), color);
      //

   }

} // namespace remoting_client
