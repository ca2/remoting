//
// Created by camilo on 2026-02-11 23:14 <3ThomasBorregaardSørensen!!
//

#pragma once

#include "gui/drawing/Graphics.h"

#include "gui/drawing/SolidBrush.h"
#include "acme/prototype/geometry2d/rectangle.h"
class DesktopWindow;


namespace remoting
{
   enum enum_control
   {

      e_control_none,
      e_control_toolbar,
      e_control_button,

   };
   enum enum_id
   {
      id_none,
      id_toolbar,
      id_minimize,
      id_restore,
      id_close

   };

   class style :
      virtual public ::particle
   {
   public:
      ::pointer < SolidBrush > m_pbrushBackground;
      ::pointer < SolidBrush > m_pbrushBackgroundHover;
      ::pointer < SolidBrush > m_pbrushButtonBackground;
      ::pointer < SolidBrush > m_pbrushButtonBackgroundHover;
      ::pointer < SolidBrush > m_pbrushButtonPaint;

      style();
      ~style() override;

   };

   class control :
   virtual public ::particle
   {
   public:
      DesktopWindow * m_pdesktopwindow = nullptr;
      control * m_pcontrolParent = nullptr;
      ::pointer < style > m_pstyle;
      ::int_rectangle m_rectangle;
      bool m_bHover = false;
      bool m_bPressed = false;
      enum_control m_econtrol;
      enum_id m_eid;

      ::pointer_array < control > m_controlaChildren;

      control();
      ~control();




      virtual bool _000OnMouse(bool bPress, POINT position);
      virtual bool _001OnMouse(bool bPress, POINT position);
      virtual bool on_left_down(POINT position);
      virtual bool on_left_up(POINT position);
      virtual void _000OnDraw(Graphics * pgraphics);
      virtual void _001OnDraw(Graphics * pgraphics);
      virtual bool on_button_click(enum_id eid);

      virtual ::int_rectangle get_client_rectangle();
      virtual ::int_rectangle get_window_rectangle();

   };

   class toolbar_button :
   virtual public control
   {
   public:


      // enum enum_button
      // {
      //    e_button_none,
      //    e_button_minimize,
      //    e_button_restore,
      //    e_button_close,
      //
      // };

      //enum_button m_ebutton;
      //::int_rectangle      m_rectangle;

      //::int_rectangle m_rectangleMinimizeDash;

      //bool on_mouse(bool bPress, POINT position);
      //void on_draw(DeviceContext *dc);

      toolbar_button();
      ~toolbar_button() override;
   };
   class toolbar :
   virtual public control
   {
   public:

      //SolidBrush m_brushBackgroundMinimizeDash;
      //::pointer<impact_toolbar_button> m_pbuttonMinimize;
      //::pointer<impact_toolbar_button> m_pbuttonRestore;
      //::pointer<impact_toolbar_button> m_pbuttonClose;

      toolbar();
      ~toolbar() override;

      virtual void create_impact_toolbar(DesktopWindow * pdesktopwindow,  style * pstyle);

      //bool on_mouse(bool bPress, POINT position);
      //void on_draw(DeviceContext *dc);
      bool on_button_click(enum_id eid) override;
   };


} // namespace remoting
