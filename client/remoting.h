// Created by camilo on 2026-04-16 23:52 <3ThomasBorregaardSørensen!!
#pragma once

namespace remoting
{

   class ViewerConfig;
   class ConnectionConfig;
}

namespace remoting_client
{

   class ConnectingDialog;
   class ConnectionData;

   class remoting_impact;

   class remoting :
      public ::particle
   {
   public:
      
      
      ::pointer < ::remoting::ConnectionConfig >         m_pconnectionconfig;
      ::pointer < ConnectionData  >                      m_pconnectiondata;
      ::pointer < ::remoting::ViewerConfig>              m_pviewerconfig;
      ::pointer<ConnectingDialog>                        m_pconnectingdialog;
      ::subsystem::LogWriter *                           m_plogwriter;
      ::pointer<remoting_client::remoting_impact>      m_premotingimpact;


      remoting();
      ~remoting() override;


      virtual void on_start();
      virtual void open_file(const ::file::path & path);


      virtual ::lresult handle_direct_id(const ::enum_id eid, ::wparam wparam, ::lparam lparam);

   };


} // namespace remoting_client
