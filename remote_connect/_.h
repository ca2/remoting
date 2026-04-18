#pragma once


#include "aura/_.h"


#if defined(_remoting_remote_connect_project)
#define CLASS_DECL_REMOTING_REMOTE_CONNECT  CLASS_DECL_EXPORT
#else
#define CLASS_DECL_REMOTING_REMOTE_CONNECT CLASS_DECL_IMPORT
#endif


namespace remoting_remote_connect
{


   class application;
   class main_window;


} // namespace remoting_remote_connect



#undef APPLICATION_CLASS
#define APPLICATION_CLASS ::app_app::application
