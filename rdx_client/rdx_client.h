// Created by camilo on 2026-05-22 17:10 <3ThomasBorregaardSørensen!! Mummi!! Bilbo!!
#pragma once


#define _CRT_SECURE_NO_WARNINGS

#include <comdef.h>
#include <ocidl.h>
#include <oleidl.h>

class com_window_thread;

//#include <windows.h>



namespace remoting_rdx_client
{



   class CLASS_DECL_REMOTING_RDX_CLIENT rdx_client : 
      virtual public ::particle
   {
   public:


      //::pointer < ::com_window_thread > m_pcomwindowthread;
      ::pointer < main_window > m_pmainwindow;

      // ::pointer<event_sink> m_peventsink;
      // ::pointer < client_site > m_pclientsite;
      // rdx_client_t * m_prdxclient;
      //
      //
      // HWND m_hwnd;
      // DWORD m_dwCookie;
      //
      // ::comptr < IOleObject> m_poleobject;
      // ::comptr < IConnectionPoint  > m_pconnectionpoint;

      rdx_client();
      ~rdx_client() override;


      // ::i64 get_style_for_creating_window() override;
      //
      //
      //
      // string get_title() override;
      //
      // void on_size() override;



      virtual void start_main_window();

      virtual void main_window_main(const ::scoped_string & scopedstrHost);

   };

}// namespace remoting_rdx_client
