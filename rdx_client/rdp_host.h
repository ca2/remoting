#pragma once


#include "com_window.h"
#include "event_sink.h"


// #import "mstscax.dll" \
// raw_interfaces_only \
// raw_native_types \
// no_namespace \
// named_guids

namespace remoting_rdx_client
{


   class rdp_host_internal;


   class CLASS_DECL_REMOTING_RDX_CLIENT rdp_host :
      virtual public ::windows::com_window
   {
   public:


      // bool              m_dDrawControlBox;
      // bool              m_dDrawOnlyMainRectangles;
      // double            m_dBreathPeriod;
      // class ::time      m_timeStart;
      // double            m_dPhaseShift;
      // int               m_iCloseButtonDraw;
      //
      //
      // ::pointer<event_sink> m_peventsink;
      // ::pointer < client_site > m_pclientsite;
      ::pointer < rdp_host_internal > m_pinternal;


      ::pointer < toolbar > m_ptoolbar;

      //HWND m_hwnd;
      DWORD m_dwCookie;

      //::comptr < IOleObject> m_poleobject;
      //::comptr < IOleInPlaceActiveObject > m_pinplaceactiveobject;
      //::comptr < IConnectionPoint  > m_pconnectionpoint;

      rdp_host();
      ~rdp_host() override;



      ::i64 get_style_for_creating_window() override;


      //virtual void do_control_layout();
      //virtual void control_activate(bool bActivate);
      //virtual void do_control_show_verb();
      //virtual void do_control_ui_activate();

      string get_title() override;

      //void on_window_size() override;
      //void on_window_set_focus() override;
      //bool on_window_activate(int iActivate, bool bMinimized, const ::operating_system::window & operatingsystemwindow) override;
      //bool on_window_mouse_activate(int &iResult, const operating_system::window &operatingsystemwindowTop, int iHitTest, int iMessage) override;


      virtual void on_create_window();


      bool _on_window_procedure(lresult &lresult, u32 message, wparam wparam, lparam lparam) override;
      //void install_message_routing(::channel * pchannel) override;


      //DECLARE_MESSAGE_HANDLER(on_message_create);


      //virtual void on_create_user_interaction() override;

      void on_window_paint(nano::graphics::device *pnanographicsdevice) override;

      //virtual void _001OnDraw(::draw2d::graphics_pointer & pgraphics) override;

      //virtual void _001DrawItem(::draw2d::graphics_pointer& pgraphics, ::user::item & item, const ::user::e_state & estate) override;

      //virtual void on_rdp_connected();
      //virtual void on_rdp_login_complete();


      //HRESULT STDMETHODCALLTYPE Invoke(DISPID dispIdMember, REFIID, LCID, WORD, DISPPARAMS *pDispParams,
        //                                     VARIANT *, EXCEPINFO *, UINT *);


      virtual void shutdown_rdp();


      virtual void OnRdpConnecting();
      virtual void OnRdpConnected();
      virtual void OnRdpLoginComplete();
      virtual void OnRdpDisconnected();
      virtual void OnRdpEnterFullScreen();
      virtual void OnRdpLeaveFullScreen();
      virtual void OnRdpRequestGoFullScreen();
      virtual void OnRdpRequestLeaveFullScreen();

      virtual void MinimizeRdp();
      virtual void RestoreRdp();
      virtual void CloseRdp();


      virtual void doMinimize();
      virtual void doRestore();
      virtual void doFullscreen();


      //virtual ::i32_size get_main_screen_size();

   };



} // namespace remoting_rdx_client



