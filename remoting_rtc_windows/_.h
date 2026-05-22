//
// Created by camilo on 2026-02-14 23:03 <3ThomasBorregaardSørensen!!
//

///TBS__<3tbs
#pragma once


#include "remoting/remoting_rtc/_.h"
#include "subsystem_windows/_.h"
#include "acme/_operating_system.h"
#include "acme/operating_system/windows_common/com/comptr.h"


#if defined(_remoting_windows_project)
#define CLASS_DECL_REMOTING_WINDOWS CLASS_DECL_EXPORT
#else
#define CLASS_DECL_REMOTING_WINDOWS CLASS_DECL_IMPORT
#endif


//#include "remoting/remoting_windows/desktop/_.h"
// #include "remoting/remoting_rtc/framebuffer_update_sender/_.h"
// #include "remoting/remoting_rtc/file_transfer_node/_.h"
// #include "remoting/remoting_rtc/node/_.h"
// #include "remoting/remoting_rtc/node_config/_.h"
// #include "remoting/remoting_rtc/viewer_core/_.h"



