// Into remoting sources by Camilo on 2026-02-10 <3ThomasBorregaardSorensen!!
#pragma once



#include "remoting/remoting_rfb/_.h"
#include "acme/_operating_system.h"
#include "subsystem_bsd_sockets/_.h"
#if defined(_remoting_node_desktop_project)
#define CLASS_DECL_REMOTING_NODE_DESKTOP CLASS_DECL_EXPORT
#else
#define CLASS_DECL_REMOTING_NODE_DESKTOP CLASS_DECL_IMPORT
#endif


namespace remoting_rfb_node_desktop
{


   // class Configurator;
   //
   //
   class Server;


   class ServiceControlCommandLine;


   class ServerApplication;


   class Service;


   class WsConfigRunner;


   //class NewConnectionEvents;


   class ServiceEvents;


} // namespace remoting_rfb_node_desktop