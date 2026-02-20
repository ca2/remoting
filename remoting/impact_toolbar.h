//
// Created by camilo on 2026-02-11 23:14 <3ThomasBorregaardSørensen!!
//

#pragma once

#include "remoting/remoting_common/gui/drawing/Graphics.h"

#include "remoting/remoting_common/gui/drawing/SolidBrush.h"
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

   class CLASS_DECL_REMOTING_REMOTING style :
      virtual public ::particle
   {
   public:
      ::pointer < SolidBrush > m_pbrushBackground;
      ::pointer < SolidBrush > m_pbrushBackgroundHover;
      ::pointer < SolidBrush > m_pbrushButtonBackground;
      ::pointer < SolidBrush > m_pbrushButtonBackgroundHover;
      ::pointer < SolidBrush > m_pbrushButtonPaint;
      ::pointer < Pen > m_ppenPaint;

      style();
      ~style() override;

   };

   class CLASS_DECL_REMOTING_REMOTING control :
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
      Gdiplus::Bitmap * m_pbitmapBuffer = nullptr;
      Gdiplus::Graphics * m_pgraphicsBuffer = nullptr;


      bool m_bNewRepaintRectangle;
      ::int_rectangle m_rectangleRepaint;

      ::pointer_array < control > m_controlaChildren;

      control();
      ~control();

      control * get_paint_window();
      void add_repaint(const ::int_rectangle & rectangle);
      void _add_repaint(const ::int_rectangle & rectangle);
void defer_repaint();
bool m_bDrag = false;
      int m_xCursorDragStart = -1;
      int m_xWindowDragStart = -1;

      virtual bool _000OnMouse(bool bPress, POINT pointRoot, POINT pointClient);
      virtual bool _001OnMouse(bool bPress, POINT pointRoot, POINT pointClient);
      virtual bool on_left_down(POINT position);
      virtual bool on_left_up(POINT position);
      virtual void __000OnTopDraw(HDC hdc, const ::int_rectangle & rectangle);
      virtual void __000OnDraw(GraphicsPlus * pgraphics, const ::int_rectangle & rectangle);
      virtual void __001OnDraw(GraphicsPlus * pgraphics, const ::int_rectangle & rectangle);
      virtual bool on_button_click(enum_id eid);

      virtual ::int_rectangle get_client_rectangle();
      virtual ::int_rectangle get_window_rectangle();
      virtual ::int_rectangle get_paint_rectangle();

   };


   class CLASS_DECL_REMOTING_REMOTING toolbar_button :
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


   class CLASS_DECL_REMOTING_REMOTING toolbar :
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
      void __001OnDraw(GraphicsPlus *pgraphics, const ::int_rectangle & rectangle) override;
      bool on_button_click(enum_id eid) override;
      bool _001OnMouse(bool bPress, POINT pointRoot, POINT pointClient) override;
   };


} // namespace remoting
