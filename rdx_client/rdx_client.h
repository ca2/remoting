// Created by camilo on 2026-05-22 17:10 <3ThomasBorregaardSørensen!! Mummi!! Bilbo!!
#pragma once


namespace remoting_rdx_client
{

   class CLASS_DECL_REMOTING_RDX_CLIENT rdx_client : 
      virtual public ::particle
   {
   public:


      ::pointer<event_sink> m_peventsink;


      HWND m_hwnd;
      DWORD m_dwCookie;

      ::comptr<IOleObject> m_poleobject;
      ::comptr < IMsRdpClient9  > m_prdpclient;
      ::comptr < IConnectionPoint  > m_pconnectionpoint;

      rdx_client();


      ~rdx_client() override;



   };

}// namespace remoting_rdx_client
