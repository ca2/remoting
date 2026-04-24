// Into tightvnc sources by Camilo on 2026-02-10 <3ThomasBorregaardSorensen!!
#pragma once



#include "remoting/remoting/_.h"
#include "acme/_operating_system.h"
#include "subsystem_bsd_sockets/_.h"
#if defined(_remoting_node_desktop_project)
#define CLASS_DECL_REMOTING_NODE_DESKTOP CLASS_DECL_EXPORT
#else
#define CLASS_DECL_REMOTING_NODE_DESKTOP CLASS_DECL_IMPORT
#endif


namespace remoting_node_desktop
{


   class Server;


   class ServiceControlCommandLine;


   class ServerApplication;


   class Service;


   class WsConfigRunner;


   class NewConnectionEvents;


   class WinServiceEvents;


} // namespace remoting_node_desktop