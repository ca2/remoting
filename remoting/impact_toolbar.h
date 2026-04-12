//
// Created by camilo on 2026-02-11 23:14 <3ThomasBorregaardSørensen!!
//

#pragma once

#include "innate_subsystem/drawing/Graphics.h"
#include "innate_subsystem/drawing/Pen.h"
#include "innate_subsystem/drawing/SolidBrush.h"
#include "acme/prototype/geometry2d/rectangle.h"
class DesktopWindow;


namespace remoting_remoting
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
      ::subsystem_apex::SolidBrush m_brushBackground;
      ::subsystem_apex::SolidBrush m_brushBackgroundHover;
      ::subsystem_apex::SolidBrush m_brushButtonBackground;
      ::subsystem_apex::SolidBrush m_brushButtonBackgroundHover;
      ::subsystem_apex::SolidBrush m_brushButtonPaint;
      ::subsystem_apex::Pen m_penPaint;

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
      ::pointer < ::subsystem_apex::Pen > m_ppen001;
      ::int_rectangle m_rectangle;
      bool m_bHover = false;
      bool m_bLButtonDown = false;
      //bool m_bPressed = false;
      bool m_bDrag = false;
      int m_xCursorDragStart = -1;
      int m_xWindowDragStart = -1;

      enum_control m_econtrol;
      enum_id m_eid;
      //Gdiplus::Bitmap * m_pbitmapBuffer = nullptr;
      //Gdiplus::Graphics * m_pgraphicsBuffer = nullptr;
      ::pointer < ::subsystem_apex::BitmapInterface > m_pbitmapBuffer;
      ::pointer < ::subsystem_apex::GraphicsInterface > m_pgraphicsBuffer;

      bool m_bNewRepaintRectangle;
      ::int_rectangle m_rectangleRepaint;

      ::pointer_array < control > m_controlaChildren;

      control();
      ~control();

      control * get_paint_window();
      void add_repaint(const ::int_rectangle & rectangle);
      void _add_repaint(const ::int_rectangle & rectangle);
void defer_repaint();
void set_hover_false();
        virtual bool _000OnMouseEx(unsigned int uMessage, int iButtonMask, const ::int_point& pointRoot, const ::int_point& pointClient);
        virtual bool _001OnMouseEx(unsigned int uMessage, int iButtonMask, const ::int_point& pointRoot, const ::int_point& pointClient);
      //virtual bool _000OnMouse(bool bPress, const ::int_point& pointRoot, const ::int_point& pointClient);
      //virtual bool _001OnMouse(bool bPress, const ::int_point& pointRoot, const ::int_point& pointClient);
      virtual bool on_left_down(const ::int_point& position);
      virtual bool on_left_up(const ::int_point& position);
      virtual void __000OnTopDraw(::subsystem_apex::GraphicsInterface * pgraphics, const ::int_rectangle & rectangle);
      virtual void __000OnDraw(::subsystem_apex::GraphicsInterface * pgraphics, const ::int_rectangle & rectangle);
      virtual void __001OnDraw(::subsystem_apex::GraphicsInterface * pgraphics, const ::int_rectangle & rectangle);
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

      //bool on_mouse(bool bPress, const ::int_point& position);
      //void on_draw(DeviceContext *dc);

       bool m_bPressed;
       class ::time m_timePressed;
       

      toolbar_button();
      ~toolbar_button() override;

     

      bool on_left_down(const ::int_point& position) override;
      bool on_left_up(const ::int_point& position) override;

   };


   class CLASS_DECL_REMOTING_REMOTING toolbar :
   virtual public control
   {
   public:
       
      //SolidBrush m_brushBackgroundMinimizeDash;
      //::pointer<impact_toolbar_button> m_pbuttonMinimize;
      //::pointer<impact_toolbar_button> m_pbuttonRestore;
      //::pointer<impact_toolbar_button> m_pbuttonClose;
      //int m_iDesktopWidth = 1920;
      //float m_fScale;
      ::pointer < ::subsystem_apex::FontInterface > m_pfont001;
      ::pointer<toolbar_button> m_pbuttonMinimize;
      ::pointer<toolbar_button> m_pbuttonRestore;
      ::pointer<toolbar_button> m_pbuttonClose;

      toolbar();
      ~toolbar() override;

      virtual void create_impact_toolbar(DesktopWindow * pdesktopwindow,  style * pstyle);

      virtual void on_size();

      //bool on_mouse(bool bPress, const ::int_point& position);
      void __001OnDraw(::subsystem_apex::GraphicsInterface *pgraphics, const ::int_rectangle & rectangle) override;
      bool on_button_click(enum_id eid) override;
      bool _001OnMouseEx(unsigned int uMessage,  int iButtonMask, const ::int_point & pointRoot, const ::int_point& pointClient) override;
   };


} // namespace remoting_remoting
