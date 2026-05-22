// Into remoting sources by Camilo on 2026-02-10 <3ThomasBorregaardSorensen!!
#pragma once



#include "remoting/remoting_rtc/_.h"
#include "acme/operating_system/windows_common/com/_.h"

#if defined(_remoting_rdx_client_project)
#define CLASS_DECL_REMOTING_RDX_CLIENT  CLASS_DECL_EXPORT
#else
#define CLASS_DECL_REMOTING_RDX_CLIENT  CLASS_DECL_IMPORT
#endif


#define DISPID_CONNECTED 1
#define DISPID_LOGINCOMPLETE 4
#define DISPID_DISCONNECTED 2
#define DISPID_FATALERROR 3

namespace remoting_rdx_client
{

   class rdx_client;

   class event_sink;

   
   class keyboard_layout_change;

   class remoting;

   class ViewerWindow;

   class DesktopWindow;


} // namespace remoting_client



