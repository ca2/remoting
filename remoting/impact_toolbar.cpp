//
// Created by camilo on 2026-02-11 23:18 <3ThomasBorregaardSørensen!!
//
#include "framework.h"
#include "DesktopWindow.h"
#include "impact_toolbar.h"
#include <dwmapi.h>
// Link with dwmapi.lib
#pragma comment (lib, "dwmapi.lib")


namespace remoting
{
   control::control()
   {
      m_econtrol  = e_control_none;
      m_eid = id_none;
   }
   control::~control()
   {


   }

   toolbar::toolbar()
   {
      m_econtrol = e_control_toolbar;
   }
   toolbar::~toolbar()
   {


   }

   ::int_rectangle control::get_client_rectangle()
   {
      return m_rectangle.size();
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


   bool control::_001OnMouse(bool bPress, POINT position)
   {

      auto rectangleClient = get_client_rectangle();

      auto bHoverNew= rectangleClient.contains(::int_point{position.x, position.y});

      if (is_different(bHoverNew, m_bHover))
      {

         m_bHover = bHoverNew;

         m_pdesktopwindow->repaint(get_window_rectangle());

      }

      if (bPress && bHoverNew)
      {
         if (!m_bPressed)
         {

            m_bPressed = true;

            on_left_down(position);

         }
      }
      else
      {

         if (m_bPressed)
         {

            m_bPressed = false;

            on_left_up(position);

         }

      }

      return false;

   }


   bool control::on_left_down(POINT position)
   {


      return false;

   }


   bool control::on_left_up(POINT position)
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

   bool control::_000OnMouse(bool bPress, POINT position)
   {

      auto pointClient = position;
      pointClient.x -= m_rectangle.left;
      pointClient.y -= m_rectangle.top;

      if (_001OnMouse(bPress, pointClient))
      {

         return true;

      }

      for (auto & pcontrol:m_controlaChildren)
      {

         if (pcontrol->_000OnMouse(bPress, pointClient))
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

   // bool toolbar_button::on_mouse(bool bPress, POINT position)
   //{


   style::style()
   {

      m_pbrushBackground = øallocate SolidBrush(RGB(40, 120, 180));
      m_pbrushBackgroundHover = øallocate SolidBrush(RGB(40, 120, 220));
      m_pbrushButtonBackground = øallocate SolidBrush(RGB(40, 120, 180));
      m_pbrushButtonBackgroundHover = øallocate SolidBrush(RGB(50, 130, 230));
      m_pbrushButtonPaint  =  øallocate SolidBrush(RGB(255, 255, 255));

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
      int iMarginRight = iButtonSize;
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

   // bool toolbar::on_mouse(bool bPress, POINT position)
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

         BOOL enable = TRUE; // Use TRUE to force disable, which is counter-intuitive but how the flag works
         HRESULT hr = DwmSetWindowAttribute(::GetParent(m_pdesktopwindow->getHWnd()), DWMWA_TRANSITIONS_FORCEDISABLED, &enable, sizeof(enable));
         ::ShowWindow(::GetParent(m_pdesktopwindow->getHWnd()), SW_MINIMIZE);

      }

      return false;

   }


   void control::_000OnDraw(Graphics * pgraphics)
   {
      _001OnDraw(pgraphics);

      for (auto & pcontrol:m_controlaChildren)
      {
         pcontrol->_000OnDraw(pgraphics);
      }

   }


   void control::_001OnDraw(Graphics * pgraphics)
   {

      SolidBrush * pbrushBackground = nullptr;
      if (m_econtrol == e_control_toolbar)
      {
         if (m_bHover)
         {

            pbrushBackground = m_pstyle->m_pbrushBackgroundHover;

         }
         else
         {

            pbrushBackground = m_pstyle->m_pbrushBackground;

         }
      }
      else
      {
         if (m_bHover)
         {

            pbrushBackground = m_pstyle->m_pbrushButtonBackgroundHover;

         }
         else
         {

            pbrushBackground = m_pstyle->m_pbrushButtonBackground;

         }

      }

      if (pbrushBackground)
      {
         auto r = get_window_rectangle();
         pgraphics->fillRect(r.left, r.top,r.right, r.bottom, pbrushBackground);
      }

      if (m_eid == id_minimize)
      {


         auto r = get_window_rectangle();

         ::int_rectangle rDash;

         rDash.left = r.left + 2;
         rDash.right = r.right - 2;
         rDash.top = r.top + 4;
         rDash.bottom = rDash.top + 2;

         pgraphics->fillRect(rDash, m_pstyle->m_pbrushButtonPaint);

      }

   }

} // namespace remoting
