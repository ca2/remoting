//
// Created by camilo on 2026-05-23 11:23 <3ThomasBorregaardSørensen!! Mummi!! Bilbo!!
//
#pragma once


#include "acme/user/user/interaction.h"


#include <comdef.h>
#include <ocidl.h>
#include <oleidl.h>


namespace windows
{


   class CLASS_DECL_REMOTING_RDX_CLIENT com_window :
      virtual public ::acme::user::interaction
   {
   public:


      bool              m_dDrawControlBox;
      bool              m_dDrawOnlyMainRectangles;
      double            m_dBreathPeriod;
      class ::time      m_timeStart;
      double            m_dPhaseShift;
      int               m_iCloseButtonDraw;




      //::pointer<event_sink> m_peventsink;
      ::pointer < client_site > m_pclientsite;

      IID m_rclsid;
      //HWND m_hwnd;
      //DWORD m_dwCookie;

      ::comptr < IOleObject> m_poleobject;

      ::i32_rectangle m_rectangle;

      com_window();
      ~com_window() override;


      ::windows::window_class _get_window_class() override;

      ::i32_rectangle get_rectangle() override;

      ::i64 get_style_for_creating_window() override;


      virtual void do_control_layout();
      virtual void control_activate(bool bActivate);
      virtual void do_control_show_verb();
      virtual void do_control_ui_activate();

      //string get_title() override;

      void on_window_size() override;
      void on_window_set_focus() override;
      bool on_window_activate(int iActivate, bool bMinimized, const ::operating_system::window & operatingsystemwindow) override;
      bool on_window_mouse_activate(int &iResult, const operating_system::window &operatingsystemwindowTop, int iHitTest, int iMessage) override;


      virtual void on_create_window();

      //void install_message_routing(::channel * pchannel) override;


      //DECLARE_MESSAGE_HANDLER(on_message_create);


      //virtual void on_create_user_interaction() override;

      //virtual void _001OnDraw(::draw2d::graphics_pointer & pgraphics) override;

      //virtual void _001DrawItem(::draw2d::graphics_pointer& pgraphics, ::user::item & item, const ::user::e_state & estate) override;

      //virtual void on_rdp_connected();
      //virtual void on_rdp_login_complete();


      //virtual float get_window_scale();


   };


} // namespace remoting_rdx_client



