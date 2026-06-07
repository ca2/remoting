//
// Created by camilo on 2026-02-12 02:03 <3ThomasBorregaardSørensen!!
//
#include "framework.h"
#include "application.h"
#include "Server.h"
#include "ServerApplication.h"
#include "acme/constant/id.h"
#include "acme/handler/request.h"
#include "acme/platform/system.h"
#include "remoting/rfb_client/remoting.h"
#include "remoting/remoting_rfb/platform/remoting.h"
#include "Service.h"
#include "resource.h"
//#include "acme/_operating_system.h"
//#include "main_window.h"


__IMPLEMENT_APPLICATION_RELEASE_TIME(remoting_rfb_node_desktop);
IMPLEMENT_APPLICATION_FACTORY(remoting_rfb_node_desktop);


//::i32 remoting_impact_main(::particle * pparticle, const ::file::path & path);
//CLASS_DECL_ACME HMODULE GetModuleFromFunction(void* pFunc);


namespace remoting_rfb_node_desktop
{

   void function_at_remoting_remoting()
   {
   }

   application::application()
   {

      ::remoting_rfb::defer_initialize_remoting();

      //HINSTANCE hInstance = (HINSTANCE)GetModuleFromFunction(&function_at_remoting_remoting);

      // return hInstance;

      //::system()->m_premoting->m_pHinstance = (void *) hInstance;

      ::system()->m_bFinalizeIfNoSession = false;

      m_bNetworking = true;

      m_strAppId = "remoting/rfb_node_desktop";

      m_bUserApplication = false;

   }


   application::~application()
   {

   }


   void application::init_instance()
   {
   
      ::apex::application::init_instance();
      
      setResourceName(IDD_CONFIG_ADMINISTRATION_PAGE, "IDD_CONFIG_ADMINISTRATION_PAGE");
      setResourceName(IDD_CONFIG_SERVER_PAGE, "IDD_CONFIG_SERVER_PAGE");
      setResourceName(IDD_CONFIG_VIDEO_PAGE, "IDD_CONFIG_VIDEO_PAGE");
      setResourceName(IDD_CONFIG_ACCESS_CONTROL_PAGE, "IDD_CONFIG_ACCESS_CONTROL_PAGE");
      setResourceName(IDD_CONFIG_PORT_MAPPING_PAGE, "IDD_CONFIG_PORT_MAPPING_PAGE");
      setResourceName(IDD_CONFIG, "IDD_CONFIG");
      setResourceName(IDD_EDIT_PORT_MAPPING, "IDD_EDIT_PORT_MAPPING");
      setResourceName(IDD_EDIT_IP_ACESS_CONTROL, "IDD_EDIT_IP_ACESS_CONTROL");
      setResourceName(IDD_CHANGE_PASSWORD, "IDD_CHANGE_PASSWORD");
      setResourceName(IDD_OUTGOING_CONN, "IDD_OUTGOING_CONN");
      setResourceName(IDD_CONTROL_AUTH, "IDD_CONTROL_AUTH");
      setResourceName(IDD_ABOUT_DIALOG, "IDD_ABOUT_DIALOG");
      setResourceName(IDD_QUERY_RFB_CONNECTION, "IDD_QUERY_RFB_CONNECTION");
      setResourceName(IDD_DISPATCHER_CONN, "IDD_DISPATCHER_CONN");
      setResourceName(IDD_SET_PASSWORDS, "IDD_SET_PASSWORDS");
      setResourceName(IDC_VIDEO_RECTS, "IDC_VIDEO_RECTS");
      setResourceName(IDC_ACCEPT_BUTTON, "IDC_ACCEPT_BUTTON");
      setResourceName(IDC_SHOW_TVNCONTROL_ICON_CHECKBOX, "IDC_SHOW_TVNCONTROL_ICON_CHECKBOX");
      setResourceName(IDC_STATIC_VERSION, "IDC_STATIC_VERSION");
      setResourceName(IDC_CONFIG_TAB, "IDC_CONFIG_TAB");
      setResourceName(IDC_DISPATCH_ID, "IDC_DISPATCH_ID");
      setResourceName(IDC_IP_ACCESS_CONTROL_LIST, "IDC_IP_ACCESS_CONTROL_LIST");
      setResourceName(IDC_LOG_LEVEL_SPIN, "IDC_LOG_LEVEL_SPIN");
      setResourceName(IDC_PASSWORD_EDIT, "IDC_PASSWORD_EDIT");
      setResourceName(IDC_RFB_PORT_SPIN, "IDC_RFB_PORT_SPIN");
      setResourceName(IDC_VIDEO_CLASS_NAMES, "IDC_VIDEO_CLASS_NAMES");
      setResourceName(IDC_VIEW_ONLY_CHECKBOX, "IDC_VIEW_ONLY_CHECKBOX");
      setResourceName(IDC_VISIT_WEB_SITE_BUTTON, "IDC_VISIT_WEB_SITE_BUTTON");
      setResourceName(IDC_ALLOW_LOOPBACK_CONNECTIONS, "IDC_ALLOW_LOOPBACK_CONNECTIONS");
      setResourceName(IDC_DISPATCH_NAME, "IDC_DISPATCH_NAME");
      setResourceName(IDC_LOG_FILEPATH_EDIT, "IDC_LOG_FILEPATH_EDIT");
      setResourceName(IDC_PORT_EDIT, "IDC_PORT_EDIT");
      setResourceName(IDC_STATIC_LICENSING, "IDC_STATIC_LICENSING");
      setResourceName(IDC_UNSET_READONLY_PASSWORD_BUTTON, "IDC_UNSET_READONLY_PASSWORD_BUTTON");
      setResourceName(IDC_VIDEO_RECOGNITION_INTERVAL, "IDC_VIDEO_RECOGNITION_INTERVAL");
      setResourceName(IDC_APPLY, "IDC_APPLY");
      setResourceName(IDC_LOG_FOR_ALL_USERS, "IDC_LOG_FOR_ALL_USERS");
      setResourceName(IDC_ORDER_SUPPORT_BUTTON, "IDC_ORDER_SUPPORT_BUTTON");
      setResourceName(IDC_UNSET_PRIMARY_PASSWORD_BUTTON, "IDC_UNSET_PRIMARY_PASSWORD_BUTTON");
      setResourceName(IDC_BLOCK_REMOTE_INPUT, "IDC_BLOCK_REMOTE_INPUT");
      setResourceName(IDC_DISPATCH_KEYWORD, "IDC_DISPATCH_KEYWORD");
      setResourceName(IDC_USE_D3D, "IDC_USE_D3D");
      setResourceName(IDC_ADD_BUTTON, "IDC_ADD_BUTTON");
      setResourceName(IDC_USE_CONTROL_AUTH_CHECKBOX, "IDC_USE_CONTROL_AUTH_CHECKBOX");
      setResourceName(IDC_CONTROL_PASSWORD_BUTTON, "IDC_CONTROL_PASSWORD_BUTTON");
      setResourceName(IDC_EDIT_BUTTON, "IDC_EDIT_BUTTON");
      setResourceName(IDC_REPEAT_CONTROL_AUTH_CHECKBOX, "IDC_REPEAT_CONTROL_AUTH_CHECKBOX");
      setResourceName(IDC_REMOVE_BUTTON, "IDC_REMOVE_BUTTON");
      setResourceName(IDC_UNSET_CONTROL_PASWORD_BUTTON, "IDC_UNSET_CONTROL_PASWORD_BUTTON");
      setResourceName(IDC_GEOMETRY_EDIT, "IDC_GEOMETRY_EDIT");
      setResourceName(IDC_MOVE_UP_BUTTON, "IDC_MOVE_UP_BUTTON");
      setResourceName(IDC_MOVE_DOWN_BUTTON, "IDC_MOVE_DOWN_BUTTON");
      setResourceName(IDC_ADD_PORT, "IDC_ADD_PORT");
      setResourceName(IDC_EDIT_PORT, "IDC_EDIT_PORT");
      setResourceName(IDC_REMOVE_PORT, "IDC_REMOVE_PORT");
      setResourceName(IDC_BLOCK_LOCAL_INPUT, "IDC_BLOCK_LOCAL_INPUT");
      setResourceName(IDC_MAPPINGS, "IDC_MAPPINGS");
      setResourceName(IDC_LOCAL_INPUT_PRIORITY_TIMEOUT, "IDC_LOCAL_INPUT_PRIORITY_TIMEOUT");
      setResourceName(IDC_LOCAL_INPUT_PRIORITY, "IDC_LOCAL_INPUT_PRIORITY");
      setResourceName(IDC_TIMEOUT, "IDC_TIMEOUT");
      setResourceName(IDC_REFUSE, "IDC_REFUSE");
      setResourceName(IDC_ACCEPT, "IDC_ACCEPT");
      setResourceName(IDC_ACCEPT_RFB_CONNECTIONS, "IDC_ACCEPT_RFB_CONNECTIONS");
      setResourceName(IDC_ACCEPT_HTTP_CONNECTIONS, "IDC_ACCEPT_HTTP_CONNECTIONS");
      setResourceName(IDC_HTTP_PORT, "IDC_HTTP_PORT");
      setResourceName(IDC_RFB_PORT, "IDC_RFB_PORT");
      setResourceName(IDC_LOCK_WORKSTATION, "IDC_LOCK_WORKSTATION");
      setResourceName(IDC_LOGOFF_WORKSTATION, "IDC_LOGOFF_WORKSTATION");
      setResourceName(IDC_ENABLE_FILE_TRANSFERS, "IDC_ENABLE_FILE_TRANSFERS");
      setResourceName(IDC_REMOVE_WALLPAPER, "IDC_REMOVE_WALLPAPER");
      setResourceName(IDC_LOG_LEVEL, "IDC_LOG_LEVEL");
      setResourceName(IDC_ALLOW_ONLY_LOOPBACK_CONNECTIONS, "IDC_ALLOW_ONLY_LOOPBACK_CONNECTIONS");
      setResourceName(IDC_APPLET_PARAMS_IN_URL, "IDC_APPLET_PARAMS_IN_URL");
      setResourceName(IDC_USE_AUTHENTICATION, "IDC_USE_AUTHENTICATION");
      setResourceName(IDC_ALLOW, "IDC_ALLOW");
      setResourceName(IDC_DENY, "IDC_DENY");
      setResourceName(IDC_QUERY, "IDC_QUERY");
      setResourceName(IDC_PASSWORD, "IDC_PASSWORD");
      setResourceName(IDC_PASSWORD2, "IDC_PASSWORD2");
      setResourceName(IDC_PRIMARY_PASSWORD, "IDC_PRIMARY_PASSWORD");
      setResourceName(IDC_SHARED_RADIO1, "IDC_SHARED_RADIO1");
      setResourceName(IDC_VIEW_ONLY_PASSWORD, "IDC_VIEW_ONLY_PASSWORD");
      setResourceName(IDC_SHARED_RADIO2, "IDC_SHARED_RADIO2");
      setResourceName(IDC_SHARED_RADIO3, "IDC_SHARED_RADIO3");
      setResourceName(IDC_SHARED_RADIO4, "IDC_SHARED_RADIO4");
      setResourceName(IDC_SHARED_RADIO5, "IDC_SHARED_RADIO5");
      setResourceName(IDC_POLLING_INTERVAL, "IDC_POLLING_INTERVAL");
      setResourceName(IDC_HTTP_PORT_SPIN, "IDC_HTTP_PORT_SPIN");
      setResourceName(IDC_POLLING_INTERVAL_SPIN, "IDC_POLLING_INTERVAL_SPIN");
      setResourceName(IDC_QUERY_TIMEOUT_SPIN, "IDC_QUERY_TIMEOUT_SPIN");
      setResourceName(IDC_INACTIVITY_TIMEOUT_SPIN, "IDC_INACTIVITY_TIMEOUT_SPIN");
      setResourceName(IDC_VIDEO_RECOGNITION_INTERVAL_SPIN, "IDC_VIDEO_RECOGNITION_INTERVAL_SPIN");
      setResourceName(IDC_IP_FOR_CHECK_EDIT, "IDC_IP_FOR_CHECK_EDIT");
      setResourceName(IDC_IP_CHECK_RESULT_LABEL, "IDC_IP_CHECK_RESULT_LABEL");
      setResourceName(IDC_OPEN_LOG_FOLDER_BUTTON, "IDC_OPEN_LOG_FOLDER_BUTTON");
      setResourceName(IDC_DO_NOTHING, "IDC_DO_NOTHING");
      setResourceName(IDC_FIRST_IP, "IDC_FIRST_IP");
      setResourceName(IDC_LAST_IP, "IDC_LAST_IP");
      setResourceName(IDC_USE_MIRROR_DRIVER, "IDC_USE_MIRROR_DRIVER");
      setResourceName(IDC_ENTER_RFB_PASSWORD, "IDC_ENTER_RFB_PASSWORD");
      setResourceName(IDC_ENTER_ADMIN_PASSWORD, "IDC_ENTER_ADMIN_PASSWORD");
      setResourceName(IDC_CONFIRM_RFB_PASSWORD, "IDC_CONFIRM_RFB_PASSWORD");
      setResourceName(IDC_CONFIRM_ADMIN_PASSWORD, "IDC_CONFIRM_ADMIN_PASSWORD");
      setResourceName(IDC_USE_RFB_AUTH_RADIO, "IDC_USE_RFB_AUTH_RADIO");
      setResourceName(IDC_DONT_USE_RFB_AUTH_RADIO, "IDC_DONT_USE_RFB_AUTH_RADIO");
      setResourceName(IDC_DONT_CHANGE_RFB_AUTH_SETTINGS_RADIO, "IDC_DONT_CHANGE_RFB_AUTH_SETTINGS_RADIO");
      setResourceName(IDC_PROTECT_CONTROL_INTERFACE_RADIO, "IDC_PROTECT_CONTROL_INTERFACE_RADIO");
      setResourceName(IDC_DONT_USE_CONTROL_PROTECTION_RADIO, "IDC_DONT_USE_CONTROL_PROTECTION_RADIO");
      setResourceName(IDC_DONT_CHANGE_CONTROL_PROTECTION_SETTINGS_RADIO, "IDC_DONT_CHANGE_CONTROL_PROTECTION_SETTINGS_RADIO");
      setResourceName(IDC_CONNECT_RDP_SESSION, "IDC_CONNECT_RDP_SESSION");
      setResourceName(IDC_HOSTNAME_COMBO, "IDC_HOSTNAME_COMBO");
      setResourceName(IDC_REJECT_BUTTON, "IDC_REJECT_BUTTON");
      setResourceName(IDC_IP_EDIT, "IDC_IP_EDIT");
      setResourceName(IDS_SERVER_TAB_CAPTION, "IDS_SERVER_TAB_CAPTION");
      setResourceName(IDS_ACCESS_CONTROL_TAB_CAPTION, "IDS_ACCESS_CONTROL_TAB_CAPTION");
      setResourceName(IDS_EXTRA_PORTS_TAB_CAPTION, "IDS_EXTRA_PORTS_TAB_CAPTION");
      setResourceName(IDS_VIDEO_WINDOWS_TAB_CAPTION, "IDS_VIDEO_WINDOWS_TAB_CAPTION");
      setResourceName(IDS_ADMINISTRATION_TAB_CAPTION, "IDS_ADMINISTRATION_TAB_CAPTION");

      auto pfactory = system()->factory("remoting_rfb", OPERATING_SYSTEM_NAME);

      pfactory->merge_to_global_factory();
      
   }


   void application::on_request(::request * prequest)
   {

      auto ecommand = prequest->m_ecommand;

//      if (ecommand == e_command_application_start)
//      {
//
//
//         auto pfactory = system()->factory("remoting", "windows");
//
//         pfactory->merge_to_global_factory();
//         //defer_construct_newø(m_pserverapplication);///
//
//         //m_pserverapplication->on_start();
//
//      }
//      else
      
      if (ecommand == e_command_file_open)
      {

         auto path = prequest->m_payloadFile.as_file_path();

         m_bOpenFile = true;

         main_node(path);

         //fork([this, path]()
         //{
           // remoting_impact_main(path);

         //});

      }
      else if (ecommand == e_command_default_start)
      {

         if (!m_bOpenFile)
         {

            main_node({});

         }

      }
      
   }


   bool application::check_pipe_node_client_executable_paths(const file::path& pathNode, const file::path& pathClient)
   {

      auto pathNodeFolder = pathNode.folder();

      auto pathClientFolder = pathClient.folder();

      if (pathNodeFolder.is_empty() || pathClientFolder.is_empty())
      {

         return false;

      }

      if (pathNodeFolder != pathClientFolder)
      {

         return false;

      }

      ::string strNodeName = pathNode.name();

      ::string strClientName = pathClient.name();

      if (!strNodeName.ends_eat(".exe"))
      {

         return false;

      }

      if (!strClientName.ends_eat(".exe"))
      {

         return false;

      }

      bool bEaten = strClientName.ends_eat("_control");

      if (strNodeName != strClientName)
      {

         return false;

      }

      return true;

   }


   lresult application::handle_direct_id(const enum_id eid, wparam wparam, lparam lparam)
   {

      //Server().handle_direct_id(eid, wparam, lparam);

      return ::platform::application::handle_direct_id(eid, wparam, lparam);

   }


   Server & application::Server()
   {

      if (m_pservice)
      {

         return *m_pservice->m_pserver;

      }
      else if (m_pserverapplication)
      {

         return *m_pserverapplication->m_pserver;

      }

      throw ::exception(error_wrong_state);

      // if (!m_pserver)
      // {
      //    construct_newø(m_pserver);
      //    m_pserver->initialize_remoting_node_desktop_server(false, nullptr, nullptr, this);
      // }
      //

      return *m_pserver;

   }


} // namespace remoting_rfb_node_desktop
