// Created  from ServerApplication and Service by 
// camilo on 2026-04-23 13:59 <3ThomasBorregaardSørensen!!
#pragma once


//#include "subsystem/_common_header.h"
// #include "subsystem/platform/winhdr.h"
// #include "acme/_operating_system.h"


#include "subsystem/node/Service.h"

//#include "subsystem_node_desktop/Server.h"
#include "remoting/node_desktop/ServerListener.h"
//#include "WsConfigRunner.h"
// #include "log_writer/FileLogWriter.h"
#include "remoting/node_desktop/LogInitListener.h"


namespace remoting_node_desktop
{

   /**
    * Windows Remoting Node application.
    * Used for running Remoting Node as single windows application.
    */
   class CLASS_DECL_REMOTING_RTC_NODE_DESKTOP Task : 
      virtual public ::subsystem::Task,
      virtual public ServerListener,
      virtual public LogInitListener
   {
   public:


       /**
       * Remoting Node.
       */
      ::pointer<Server> m_pserver;

      ::pointer < ::remoting_rtc_node::NewConnectionEvents  > m_pnewconnectionevents;

      ::subsystem::LogWriter * m_plogwriter;

      bool m_bService;

      //::i32 m_iex

      /**
       * Creates Remoting Node application instance.
       * @param hInstance HINSTANCE of application.
       * @param commaneLine command line string.
       */
      Task();
      /**
       * Deletes Remoting Node application instance.
       */
      ~Task() override;


      virtual void initialize_remoting_node_desktop_server_task(::remoting_rtc_node::NewConnectionEvents * pnewconnectionevents);


      void _start() override;
      //virtual void task_start() = 0;
      //virtual void maintain_task_running_wait_stop_task_signal_and_stop() = 0;
      // virtual void _maintain_task_running() = 0;
      ///virtual void signal_task_stop() = 0;
      // virtual void task_stop() = 0;
      void _stop() override;


      /**
       * Runs Remoting Node windows application.
       *
       * Makes several things:
       *   1) Starts TigthVNC server.
       *   2) Starts TvnControl application.
       *   3) Enters main windows scopedstrMessage loop.
       *   4) Stops TigthVNC server.
       *
       * @return application exit code.
       */
      // virtual ::i32 onThreadMain();
      //virtual void on_server_task_start();

      //virtual void on_server_task_execute();

      //virtual void on_server_task_stop();

      /**
       * Inherited from ServerListener abstact class.
       *
       * Shutdowns WindowsApplication.
       */
      virtual void onServerShutdown();

      // private:
      //  This is a callback function that calls when the log can be initialized.
      virtual void onLogInit(const ::scoped_string &scopedstrLogDir, const ::scoped_string &scopedstrFileName,
                             ::u8 logLevel);

      // This is a callback function that calls when log properties have changed.
      virtual void onChangeLogProps(const ::scoped_string &scopedstrNewLogDir, ::u8 newLevel);

      // FileLogWriter m_fileLogWriter;
      //::subsystem::LogWriter *m_fileLogWriter;

      /**
       * Command line string.
       */
      //::string m_commandLine;
      /**
       * TvnControl application watcher.
       */
      //WsConfigRunner *m_tvnControlRunner;

      //NewConnectionEvents *m_newConnectionEvents;
   };



} // namespace remoting_node_desktop



