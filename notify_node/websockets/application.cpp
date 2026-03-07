#include "framework.h"
#include "application.h"
#include "incoming_socket_thread.h"
#include "incoming_socket.h"
#include "socket.h"
#include "acme/exception/interface_only.h"
#include "acme/filesystem/filesystem/directory_system.h"
#include "acme/filesystem/filesystem/file_system.h"
#include "acme/parallelization/synchronous_lock.h"
//#include "acme/handler/sequence.h"
#include "acme/platform/node.h"
#include "acme/platform/system.h"
#include "acme/prototype/collection/_container.h"
#include "acme/prototype/datetime/system_time.h"
#include "acme/prototype/string/command_line.h"
//#include "acme/prototype/string/hex.h"
//#include "acme/user/micro/nano.h"
#include "acme/filesystem/filesystem/directory_context.h"
#include "acme/filesystem/filesystem/file_context.h"
#include "apex/networking/sockets/httpd/incoming_socket_thread.h"
#include "apex/platform/node.h"
//#include "apex/platform/os_context.h"

#include "aura/user/user/interaction.h"


#include "acme/_operating_system.h"


#define ID_ONE 1


//__IMPLEMENT_APPLICATION_RELEASE_TIME(install);
//IMPLEMENT_APPLICATION_FACTORY(install);


namespace remoting_notify_node
{


   namespace websockets
   {
      
      ::thread* create_new_socket_thread(::object* pparticle);
      
      
      // __IMPLEMENT_APPLICATION_RELEASE_TIME1(install));
      
      
      application::application()
      {
         
         m_strInstallConfiguration = "stage";
         
         m_bRunMainLoop = true;
         
         //m_pwindow = nullptr;
         
         m_bAdmin = false;
         
         m_dProgress = -1.0;
         
         m_dProgress1 = -1.0;
         
         m_dProgress2 = -1.0;
         
         //#ifdef WINDOWS_DESKTOP
         //
         //      m_hinstance = ::GetModuleHandleA(nullptr);
         //
         //#endif
         
         //m_hmutexInstallerInstall = nullptr;
         
         m_estatusMessage = success_none;
         
         m_bInstallerInstalling = false;
         
         //m_spinstall = allocateø class install (this);
         
         
         
      }
      
      
      application::~application()
      {
         
         
      }
      
      
      
      
      //bool application::is_installed()
      //{
      
      //   string strAppId = m_strAppId;
      
      //   string strBuild = m_strBuild;
      
      //   string strPlatform = psystem->get_system_platform();
      
      //   string strConfiguration = psystem->get_system_configuration();
      
      //   string strLocale = m_strLocale;
      
      //   string strSchema = m_strSchema;
      
      //   return psystem->is_application_installed(
      //          strAppId,
      //          strBuild,
      //          strPlatform,
      //          strConfiguration,
      //          strLocale,
      //          strSchema);
      
      //}
      //class install & application::install()
      //{
      
      //   return *m_spinstall;
      
      //}
      
      
      
      //bool application::is_application_installed(const ::file::path & pathExe, const ::scoped_string & scopedstrAppId, string & strBuild, const ::scoped_string & scopedstrPlatform, const ::scoped_string & scopedstrConfiguration, const ::scoped_string & scopedstrLocale, const ::scoped_string & scopedstrSchema, unsigned int & dwGoodToCheckAgain)
      //{
      
      //   class status & status = m_mapInstalled[strAppId];
      
      //   status.m_iCheck++;
      
      //   if (status.m_bOk)
      //   {
      
      //      return true;
      
      //   }
      
      //   string strName = ::process::app_id_to_app_name(strAppId);
      
      //   string strExe = pathExe;
      
      //   string strDll = pathExe.folder() / (string(pathExe.title()) + ".dll");
      
      //   bool bInstalled = ::is_application_installed(
      //                     pathExe,
      //                     strAppId,
      //                     strBuild,
      //                     strPlatform,
      //                     strConfiguration,
      //                     strLocale,
      //                     strSchema);
      
      //   status.m_bOk = file_system()->exists(strDll) && file_system()->exists(strExe) && bInstalled;
      
      //   dwGoodToCheckAgain = status.calc_when_is_good_to_check_again();
      
      //   return status.m_bOk;
      
      //}
      
      
      //bool application::is_application_installed(const ::scoped_string & scopedstrAppId, string & strBuild, const ::scoped_string & scopedstrPlatform, const ::scoped_string & scopedstrConfiguration, const ::scoped_string & scopedstrLocale, const ::scoped_string & scopedstrSchema, unsigned int & dwGoodToCheckAgain)
      //{
      
      //   ::file::path pathExe;
      
      //   pathExe = ::get_application_path(strAppId, strPlatform, strConfiguration);
      
      //   return is_application_installed(pathExe, strAppId, strBuild, strPlatform, strConfiguration, strLocale, strSchema, dwGoodToCheckAgain);
      
      //}
      
      //bool application::is_application_updated(const ::scoped_string & scopedstrAppId, const ::scoped_string & scopedstrPlatform, const ::scoped_string & scopedstrConfiguration, const ::scoped_string & scopedstrLocale, const ::scoped_string & scopedstrSchema, unsigned int & dwGoodToCheckAgain)
      //{
      
      //   ::file::path pathExe = ::get_application_path(strAppId, strPlatform, strConfiguration);
      
      //   return is_application_updated(pathExe, strAppId, strPlatform, strConfiguration, strLocale, strSchema, dwGoodToCheckAgain);
      
      //}
      
      
      //int application::start_installation(const ::scoped_string & scopedstrCommand)
      //{
      
      //   return install().start(scopedstrCommand);
      
      //}
      
      
      void application::defer_show_debug_box()
      {
         
         if (directory_system()->config() / "spa/beg_debug_box.txt")
         {
            
            string str;
            
//            if (install_get_admin())
//            {
//               
//               str = "zzzAPPzzz app_appadmin : ";
//               
//            }
//            else
//            {
//               
//               str = "zzzAPPzzz install : ";
//               
//            }
            
            //auto pcontext = m_papplication;
            
            str += node()->get_command_line();
            
            auto pmessageboxpayload = message_box(str + " - zzzAPPzzz install", m_strAppName, ::user::e_message_box_icon_information);
            
            post(pmessageboxpayload);
            
         }
         
         
      }
      
      
      void application::initialize_app_install()
      {
         
         string strMutex;
         
         strMutex = "Global\\::ca2::account::ca2_installer_install_" + node()->process_platform_name() + "::7807e510-5579-11dd-ae16-0800200c7784";
         
         m_spmutexAppInstall = node()->create_global_named_mutex(this, false, strMutex);
         
         if (m_spmutexAppInstall->has_already_exists_flag())
         {
            
            m_estatus = error_another_installation_already_running;
            
            throw ::exception(error_failed);
            
         }
         
         //m_rxchannel.m_preceiver = this;
         
         //prepare_small_bell();
         
         string strChannel;
         
         strChannel = "::ca2::account::ca2_installer_install_" + node()->process_platform_name() + "::7807e510-5579-11dd-ae16-0800200c7784";
         
         //m_rxchannel.create(strChannel);
         //      {
         //
         //         m_estatus = error_failed;
         //
         //         return false;
         //
         //      }
         
         //sreturn true;
         
      }
      
      
      void application::init_instance()
      {
         
         ::berg::application::init_instance();
         
         factory()->add_factory_item < ::remoting_notify_node::websockets::incoming_socket >();
         factory()->add_factory_item < ::remoting_notify_node::websockets::socket >();
         
         if (!m_pincomingsocketthread)
         {
            
            construct_newø(m_pincomingsocketthread);
            
         }
         
      }
      
      
//      void application::app_app_admin()
//      {
//         
//         return;
//         
//         //      m_strInstallTraceLabel = "app-admin-" + psystem->get_system_configuration() + "-" + as_string(OSBIT);
//         //
//         //      start_web_server();
//         //
//         //      manual_reset_happening ev86(this);
//         //
//         //      ev86.reset_happening();
//         //
//         //      manual_reset_happening ev64(this);
//         //
//         //      ev64.reset_happening();
//         //
//         //      ::fork(this, [&]()
//         //      {
//         //
//         //         try
//         //         {
//         //
//         //#ifdef WINDOWS
//         //            ::CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
//         //
//         //#endif
//         //
//         //            m_bootstrap["admin:x86"] = ___new bootstrap(this);
//         //
//         //            m_bootstrap["admin:x86"]->m_strTraceLabel = "bootstrap-admin-" + psystem->get_system_configuration() + "-x86";
//         //
//         //            m_bootstrap["admin:x86"]->admin_main("x86");
//         //
//         //         }
//         //         catch (...)
//         //         {
//         //
//         //         }
//         //
//         //         ev86.set_happening();
//         //
//         //      });
//         //
//         //      ::fork(this, [&]()
//         //      {
//         //
//         //         try
//         //         {
//         //
//         //#ifdef WINDOWS
//         //            ::CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
//         //
//         //            m_bootstrap["admin:x64"] = ___new bootstrap(this);
//         //
//         //            m_bootstrap["admin:x64"]->m_strTraceLabel = "bootstrap-admin-" + psystem->get_system_configuration() + "-x64";
//         //
//         //            m_bootstrap["admin:x64"]->admin_main("x64");
//         //
//         //#endif
//         //         }
//         //         catch (...)
//         //         {
//         //
//         //         }
//         //
//         //         ev64.set_happening();
//         //
//         //      });
//         //
//         //      ev86.wait();
//         //
//         //      ev64.wait();
//         //
//         //      ::file::path pathOfficialAdmin = directory()->m_pathCa2Module / "app_app_admin.exe";
//         //
//         //      ::file::path pathThis = ::path::module();
//         //
//         //      if (pathThis.case_insensitive_order(pathOfficialAdmin) == 0)
//         //      {
//         //
//         //         if (!initialize_app_install())
//         //         {
//         //
//         //            psystem->set_finish();
//         //
//         //         }
//         //
//         //      }
//         //      else
//         //      {
//         //
//         //         start_program_files_app_app_admin(process_platform_name());
//         //
//         //      }
//         
//         
//      }
//      
      
      void application::on_request(::request * prequest)
      {
         
   #ifdef WINDOWS
         
         ::CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
         
   #endif
         
         auto psystem = system();
         
         psystem->payload("do_not_initialize_user_presence") = true;
         
//         if (!m_pincomingsocketthread)
//         {
//            
//            construct_newø(m_pincomingsocketthread);
//            
//         }
         
         //if (!m_pthreadSsl)
         //{
         
         //   m_pthreadSsl = create_new_socket_thread(this);
         
         //}
         
      }
      
      
//      void application::add_command_line(const ::scoped_string & scopedstr)
//      {
//         
//         {
//            
//            auto psystem = system();
//            
//            auto pdirectorysystem = psystem->m_pdirectorysystem;
//            
//            auto pmutexCommnadFile= node()->create_local_named_mutex(this, false, "Local\\ca2_spa_command:" + node()->process_platform_name());
//            
//            //auto pmutexCommnadFile = allocateø ::pointer < ::mutex > (this, false, "Local\\ca2_spa_command:" + node()->process_platform_name());
//            
//            ::file::path path = pdirectorysystem->userconfig() / node()->process_platform_name() / "spa_command.txt";
//            
//            string_array_base stra;
//            
//            //auto estatus =
//            //
//            this->file()->get_lines(stra, path, false);
//            
//            //if(estatus.ok())
//            {
//               
//               stra.case_insensitive_add_unique(scopedstr);
//               
//               this->file()->put_lines(path, stra);
//               
//            }
//            
//         }
//         
//      }
//      
//      
//      string application::pick_command_line()
//      {
//         
//         auto psystem = system();
//         
//         auto pdirectorysystem = psystem->m_pdirectorysystem;
//         
//         ::string strMutexName = "Local\\ca2_spa_command:" + node()->process_platform_name();
//         
//         //auto pmutexCommandFile = allocateø ::pointer < ::mutex > (this, false, "Local\\ca2_spa_command:" + node()->process_platform_name());
//         
//         auto pmutexCommandFile = node()->create_local_named_mutex(this, false, strMutexName);
//         
//         ::file::path path = pdirectorysystem->userconfig() / node()->process_platform_name() / "spa_command.txt";
//         
//         string_array_base stra;
//         
//         //auto estatus =
//         //
//         this->file()->get_lines(stra, path, false);
//         
//         //      if(!estatus)
//         //      {
//         //
//         //         return "";
//         //
//         //      }
//         
//         if (stra.is_empty())
//         {
//            
//            return "";
//            
//         }
//         
//         string str;
//         
//         str = stra[0];
//         
//         stra.case_insensitive_erase(str);
//         
//         this->file()->put_lines(path, stra);
//         
//         return str;
//         
//      }
      
      
      //   bool application::show_window()
      //   {
      //
      //      if (m_pwindow == nullptr)
      //      {
      //
      //         m_pwindow = ___new window(this);
      //
      //      }
      //
      //      if (!::IsWindow(m_pwindow->get_safe_handle()))
      //      {
      //
      //         if (!m_pwindow->create())
      //         {
      //
      //            return false;
      //
      //         }
      //
      //      }
      //
      //      int cx = 800;
      //
      //      int cy = cx * 9 / 16;
      //
      //      m_pwindow->_001Emphasize(cx, cy);
      //
      //      return true;
      //
      //   }
      //
      //
      //   bool application::hide_window()
      //   {
      //
      //      if (m_pwindow == nullptr)
      //      {
      //
      //         return true;
      //
      //      }
      //
      //#ifdef WINDOWS_DESKTOP
      //
      //      if (!::IsWindow(m_pwindow->get_safe_handle()))
      //      {
      //
      //         return true;
      //
      //      }
      //
      //      if (!m_pwindow->hide())
      //      {
      //
      //         return false;
      //
      //      }
      //
      //#endif
      //
      //      return true;
      //
      //   }
      //
      //
//      void application::app_app_main()
//      {
//         
//         return ;
//         
//         //::installpointer< class ::pointer < ::mutex > > ::pointer < ::mutex >(process_platform_name());
//         
//         //if (::pointer < ::mutex >.already_exists())
//         //{
//         
//         //   add_command_line(pcontext->os().get_command_line());
//         
//         //   m_error.set(-34);
//         
//         //   return;
//         
//         //}
//         
//         //string str(pcontext->os().get_command_line());
//         
//         //if (str.find(" install ") < 0)
//         //{
//         
//         //   if (check_soon_launch(str, true, m_dwInstallGoodToCheckAgain))
//         //   {
//         
//         //      return;
//         
//         //   }
//         
//         //}
//         
//         //add_command_line(str);
//         
//         //if (!show_window())
//         //{
//         
//         //   m_error.set(-1);
//         
//         //   return;
//         
//         //}
//         
//         //start_web_server();
//         
//         //if (!start_app_app(process_platform_name()))
//         //{
//         
//         //   m_error.set(-2);
//         
//         //   return;
//         
//         //}
//         
//         //m_pwindow->main();
//         
//         ////return m_iErrorCode;
//         
//      }
//      
      
      
//      
//      bool application::start_app_app(const ::scoped_string & scopedstrPlatform)
//      {
//         
//         m_bFinished = false;
//         
//         //      m_bootstrap[strPlatform] = ___new bootstrap(this);
//         //
//         //      m_bootstrap[strPlatform]->m_pthreadInstall = fork([=]()
//         //      {
//         //
//         //#ifdef WINDOWS
//         //
//         //         ::CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
//         //
//         //#endif
//         //
//         //         m_bootstrap[strPlatform]->m_strPlatform = strPlatform;
//         //
//         //         m_bootstrap[strPlatform]->m_strTraceLabel = "bootstrap-" + psystem->get_system_configuration() + "-" + strPlatform;
//         //
//         //         try
//         //         {
//         //
//         //
//         //            int iErrorCode = m_bootstrap[strPlatform]->install();
//         //
//         //            if(iErrorCode != 0)
//         //            {
//         //
//         //               m_result.add(error_failed);
//         //
//         //            }
//         //
//         //         }
//         //         catch (...)
//         //         {
//         //
//         //            m_result.add(error_failed);
//         //
//         //         }
//         //
//         //         end_app_app();
//         //
//         //         m_bFinished = true;
//         //
//         //         //return m_iErrorCode;
//         //
//         //      });
//         //
//         return true;
//         
//      }
//      
//      
//      
//      
//      void application::start_installer()
//      {
//         
//         //defer_show_debug_box();
//         
//         //      psystem->m_bMatterFromHttpCache = true;
//         
//         //m_pinstaller = ___new class ::install::installer (this);
//         
//         auto psystem = system();
//         
//         psystem->payload("do_not_initialize_user_presence") = true;
//         
//         //fork([=]()
//         //{
//         
//         //   install_request request;
//         
//         //   while (::task_get_run())
//         //   {
//         
//         //      installer().step_request();
//         
//         //      sleep(300_ms);
//         
//         //   }
//         
//         //});
//         
//      }
//      
//      
//      bool application::install_get_admin()
//      {
//         
//         return m_bAdmin;
//         
//      }
//      
//      
//      void application::install_set_admin(bool bSet)
//      {
//         
//         m_bAdmin = bSet;
//         
//      }
//      
      
      
      bool application::is_user_service_running()
      {
         
         string strMutexName = "Local\\ca2_application_local_mutex:application-aura/user_service";
         
         //auto pmutex = allocateø ::pointer < ::mutex > (this, false, "Local\\ca2_application_local_mutex:application-aura/user_service");
         auto pmutex = node()->create_local_named_mutex(this, false, strMutexName);
         
         return pmutex->has_already_exists_flag();
         
      }
      
      
      //string application::get_version(const ::scoped_string & scopedstrVersion)
      //{
      //   static string  s_strVersion;
      
      //   if (strVersion.has_character())
      //   {
      //      s_strVersion = strVersion;
      //   }
      
      //   if (s_strVersion.is_empty())
      //   {
      //      if (_ca_is_basis())
      //      {
      //         s_strVersion = "basis";
      //      }
      //      else
      //      {
      //         s_strVersion = "stage";
      //      }
      //   }
      
      //   return s_strVersion;
      //}
      
      
      
      
      
      
      
      
      
      
      
//      void application::install_set_id(const ::scoped_string & scopedstr)
//      {
//         
//         m_strInstallBuild = scopedstr;
//         
//      }
//      
      
      
      
      //string application::get_latest_build_number(const ::scoped_string & scopedstrVersion)
      //{
      
      //   if (m_strBuild.length() > 0)
      //   {
      
      //      return m_strBuild;
      
      //   }
      
      //   string strBuildNumber;
      
      //   string strSpaIgnitionBaseUrl;
      
      //   string strVersion(scopedstrVersion);
      
      //   if (file_system()->as_string("C:\\ca2\\config\\system\\ignition_server.txt").length() > 0)
      //   {
      
      //      strSpaIgnitionBaseUrl = "https://" + file_system()->as_string("C:\\ca2\\config\\system\\ignition_server.txt") + "/api/spaignition";
      
      //   }
      //   else if (scopedstrVersion != nullptr && !strcmp(scopedstrVersion, "basis"))
      //   {
      
      //      strSpaIgnitionBaseUrl = "https://ca2.network/api/spaignition";
      
      //   }
      //   else if (scopedstrVersion != nullptr && !strcmp(scopedstrVersion, "stage"))
      //   {
      
      //      strSpaIgnitionBaseUrl = "https://ca2.network/api/spaignition";
      
      //   }
      //   else
      //   {
      
      //      if (m_strVersion == "basis")
      //      {
      
      //         strVersion = "basis";
      
      //         strSpaIgnitionBaseUrl = "https://ca2.network/api/spaignition";
      
      //      }
      //      else
      //      {
      
      //         strVersion = "stage";
      
      //         strSpaIgnitionBaseUrl = "https://ca2.network/api/spaignition";
      
      //      }
      
      //   }
      
      //   int iRetry = 0;
      
      //RetryBuildNumber:
      
      //   if (iRetry > 10)
      //   {
      
      //      return "";
      
      //   }
      
      //   iRetry++;
      
      //   strBuildNumber = http_get(strSpaIgnitionBaseUrl + "/query?node=build&configuration=" + strVersion);
      
      //   ::str::_008Trim(strBuildNumber);
      
      //   if (strBuildNumber.length() != 19)
      //   {
      
      //      sleep(100 * iRetry);
      
      //      goto RetryBuildNumber;
      
      //   }
      
      //   return strBuildNumber;
      
      //}
      
      
      bool application::_handle_uri(const ::scoped_string & scopedstrUri)
      {
         
         string strMessage(scopedstrUri);
         
         int iRet = 0;
         
         if (strMessage.case_insensitive_begins_eat("appinstall:"))
         {
            
            //if (papp->install().is_installing_ca2())
            //{
            
            //   iRet = 1;
            
            //   return;
            
            //}
            
            if (m_bInstallerInstalling)
            {
               
               iRet = 1;
               
               return true;
               
            }
            
            m_bInstallerInstalling = true;
            
            //         m_pinstaller->install(strMessage);
            
            m_bInstallerInstalling = false;
            
         }
         else if (case_insensitive_ansi_compare(strMessage, "ok") == 0)
         {
            
            m_estatusMessage = success;
            
         }
         else if (case_insensitive_ansi_compare(strMessage, "failed") == 0)
         {
            
            m_estatusMessage = error_failed;
            
         }
         else if (case_insensitive_ansi_compare(strMessage, "exit") == 0)
         {
            
            set_finish();
            
         }
         else if (case_insensitive_ansi_compare(strMessage, "quit") == 0)
         {
            
            set_finish();
            
         }
         else
         {
            
            m_estatusMessage = error_unexpected;
            
         }
         
         return true;
         
      }
      
      
      void application::term_instance()
      {
         //if (m_pwindow != nullptr)
         //{
         
         //   ::acme::del(m_pwindow);
         
         
         //}
         
         //::acme::del(m_pinstaller);
         
         ::berg::application::term_instance();
         
         
         
         //      return true;
         
      }
      
      
      void application::get_system_locale_schema(string & strLocale, string & strSchema)
      {
         
   #if defined(WINDOWS_DESKTOP)
         
         LANGID langid = ::GetUserDefaultLangID();
         
   #define SPR_DEUTSCH LANG_GERMAN
         
         if (langid == LANG_SWEDISH)
         {
            strLocale = "se";
            strSchema = "se";
         }
         else if (langid == MAKELANGID(LANG_PORTUGUESE, SUBLANG_PORTUGUESE_BRAZILIAN))
         {
            strLocale = "pt-br";
            strSchema = "pt-br";
         }
         else if (PRIMARYLANGID(langid) == SPR_DEUTSCH)
         {
            strLocale = "de";
            strSchema = "de";
         }
         else if (PRIMARYLANGID(langid) == LANG_ENGLISH)
         {
            strLocale = "en";
            strSchema = "en";
         }
         else if (PRIMARYLANGID(langid) == LANG_JAPANESE)
         {
            strLocale = "jp";
            strSchema = "jp";
         }
         else if (PRIMARYLANGID(langid) == LANG_POLISH)
         {
            strLocale = "pl";
            strSchema = "pl";
         }
         
   #endif
         
         if (strLocale.length() <= 0)
            strLocale = "_std";
         
         if (strSchema.length() <= 0)
            strSchema = "_std";
         
      }
      
      
//      
//      void application::end_app_app()
//      {
//         
//         m_bRunMainLoop = false;
//         
//         auto psystem = system();
//         
//         psystem->set_finish();
//         
//         
//      }
//      
//      
//      string application::install_get_id()
//      {
//         
//         return m_strInstallId;
//         
//      }
//      
//      
//      string application::install_pick_command_line()
//      {
//         
//         throw ::interface_only();
//         
//         return "";
//         
//      }
//      
//      
//      string  application::install_get_build()
//      {
//         
//         return m_strInstallBuild;
//         
//      }
//      
//      int application::check_soon_launch(const ::scoped_string & scopedstrCommandLine, bool bLaunch, unsigned int & dwGoodToCheckAgain)
//      {
//         
//         string strId;
//         
//         string wstr = scopedstrCommandLine;
//         
//         character_count iFind1 = 0;
//         
//         if (wstr[0] == '\"')
//         {
//            
//            iFind1 = wstr.find_index('\"', 1);
//            
//         }
//         
//         auto iFind = wstr.find_index(" : ", iFind1 + 1);
//         
//         if (::not_found(iFind))
//         {
//            
//            string strFile = wstr.substr(iFind1 + 1);
//            
//            strFile.trim();
//            
//            if (check_soon_file_launch(strFile, bLaunch, dwGoodToCheckAgain))
//            {
//               
//               return 1;
//               
//            }
//            
//         }
//         else
//         {
//            
//            string wstrRequest = wstr.substr(iFind + 3);
//            
//            string wstrValue;
//            
//            if (get_command_line_parameter(wstrValue, wstrRequest, "install"))
//            {
//               
//               if (get_command_line_parameter(strId, wstrRequest, "enable_desktop_launch") && strId.length() > 0)
//               {
//                  
//                  return check_soon_app_id(strId, bLaunch, dwGoodToCheckAgain);
//                  
//               }
//               
//               if (get_command_line_parameter(strId, wstrRequest, "app") && strId.length() > 0)
//               {
//                  
//                  return check_soon_app_id(strId, bLaunch, dwGoodToCheckAgain);
//                  
//               }
//               
//            }
//            else
//            {
//               
//               if (get_command_line_parameter(strId, wstrRequest, "app") && strId.length() > 0)
//               {
//                  
//                  return check_soon_app_id(strId, bLaunch, dwGoodToCheckAgain);
//                  
//               }
//            }
//            
//         }
//         
//         if (strId.is_empty())
//            return false;
//         
//         return check_soon_app_id(strId, bLaunch, dwGoodToCheckAgain);
//         
//      }
//      
//      int application::check_soon_file_launch(const ::scoped_string & scopedstr, bool bLaunch, unsigned int & dwGoodToCheckAgain)
//      {
//         
//         return check_soon_app_id(get_app_id(scopedstr), bLaunch, dwGoodToCheckAgain);
//         
//      }
//      
//      
//      int application::check_soon_app_id(const ::scoped_string & scopedstrId, bool bLaunch, unsigned int & dwGoodToCheckAgain)
//      {
//         
//         if (check_soon_app_id1(scopedstrId, bLaunch, dwGoodToCheckAgain))
//         {
//            
//            return true;
//            
//         }
//         
//         if (check_soon_app_id2(scopedstrId, bLaunch, dwGoodToCheckAgain))
//         {
//            
//            return true;
//            
//         }
//         
//         return false;
//         
//      }
//      
//      
//      int application::check_soon_app_id1(const ::scoped_string & scopedstrId, bool bLaunch, unsigned int & dwGoodToCheckAgain)
//      {
//         
//         if (scopedstrId.length() <= 0)
//         {
//            
//            return 0;
//            
//         }
//         
//         auto psystem = system();
//         
//         auto pnode = psystem->node();
//         
//         string strName = pnode->app_id_to_app_name(scopedstrId);
//         
//         //auto psystem = system();
//         
//         //auto pnode = psystem->node();
//         
//         string strApp = psystem->m_pdirectorysystem->stage(scopedstrId, node()->process_platform_name(), node()->process_configuration_name()) / strName + ".exe";
//         
//         //bool bOk;
//         
//         //if (bLaunch)
//         //{
//         
//         //   string strBuild;
//         
//         //   bOk = is_application_installed(strId, strBuild, process_platform_name(), process_configuration_name(),"_std", "_std",dwGoodToCheckAgain);
//         
//         //}
//         //else
//         //{
//         
//         //   bOk = is_application_updated(strId, process_platform_name(), process_configuration_name(), "_std", "_std", dwGoodToCheckAgain);
//         
//         //}
//         
//         //      if (file_system()->exists(strApp) && bOk)
//         //      {
//         //
//         //         if (!bLaunch)
//         //         {
//         //
//         //            // if dll loads consider good state
//         //
//         //            string strDll = dir::stage(strId, process_platform_name(), process_configuration_name()) / strName + ".dll";
//         //
//         //#ifdef WINDOWS_DESKTOP
//         //
//         //            HMODULE hmodule = ::LoadLibraryW(wstring(strDll));
//         //
//         //            bool bOk = hmodule != nullptr;
//         //
//         //            if (bOk)
//         //            {
//         //
//         //               ::FreeLibrary(hmodule);
//         //
//         //            }
//         //
//         //#endif
//         //
//         //            return bOk;
//         //
//         //         }
//         
//         //#ifdef WINDOWS_DESKTOP
//         //
//         //         SHELLEXECUTEINFOW sei = {};
//         //
//         //         wstring wstrFile(strApp.c_str());
//         //
//         //         sei.cbSize = sizeof(SHELLEXECUTEINFOW);
//         //
//         //         sei.lpFile = wstrFile.c_str();
//         //
//         //         if (::ShellExecuteExW(&sei))
//         //         {
//         //
//         //            return true;
//         //
//         //         }
//         //
//         //#endif
//         //
//         //      }
//         //
//         //      return false;
//         return false;
//         
//      }
//      
//      
//      int application::check_soon_app_id2(const ::scoped_string & scopedstrId, bool bLaunch, unsigned int & dwGoodToCheckAgain)
//      {
//         
//         if (scopedstrId.length() <= 0)
//         {
//            
//            return 0;
//            
//         }
//         
//         auto psystem = system();
//         
//         auto pnode = psystem->node();
//         
//         string strName = pnode->app_id_to_app_name(scopedstrId);
//         
//         //{
//         
//         //   string strDll = dir::stage(strId, process_platform_name(), process_configuration_name()) / strName + ".dll";
//         
//         //   string strApp = dir::stage(strId, process_platform_name(), process_configuration_name()) / "app.exe";
//         
//         //   bool bOk;
//         
//         //if (bLaunch)
//         //{
//         
//         //   string strBuild;
//         
//         //   bOk = papp->is_application_installed(strId, strBuild, process_platform_name(), process_configuration_name(), "_std", "_std", dwGoodToCheckAgain);
//         
//         //}
//         //else
//         //{
//         
//         //   bOk = papp->is_application_updated(strId, process_platform_name(), process_configuration_name(), "_std", "_std", dwGoodToCheckAgain);
//         
//         //}
//         
//         //         if (file_system()->exists(strDll) && file_system()->exists(strApp) && bOk)
//         //         {
//         //
//         //            if (!bLaunch)
//         //            {
//         //
//         //
//         //#ifdef WINDOWS_DESKTOP
//         //               // if dll loads consider good state
//         //
//         //               HMODULE hmodule = ::LoadLibraryW(wstring(strDll));
//         //
//         //               bool bOk = hmodule != nullptr;
//         //
//         //               if (bOk)
//         //               {
//         //
//         //                  ::FreeLibrary(hmodule);
//         //
//         //               }
//         //#endif
//         //
//         //               return bOk;
//         //
//         //            }
//         //
//         //#ifdef WINDOWS_DESKTOP
//         //
//         //            wstring wstrParams(": app=" + strId);
//         //
//         //            wstring wstrApp(strApp);
//         //
//         //            SHELLEXECUTEINFOW sei = {};
//         //
//         //            sei.cbSize = sizeof(SHELLEXECUTEINFOW);
//         //
//         //            sei.lpFile = wstrApp.c_str();
//         //
//         //            sei.lpParameters = wstrParams.c_str();
//         //
//         //            if (::ShellExecuteExW(&sei))
//         //            {
//         //
//         //               return true;
//         //
//         //            }
//         //
//         //#endif
//         //
//         //         }
//         //
//         //      }
//         
//         return false;
//         
//      }
//      
//      
//      string application::install_get_title(const ::scoped_string & scopedstrTitle)
//      {
//         
//         auto psystem = system();
//         
//         if (scopedstrTitle.has_character())
//         {
//            
//            psystem->payload("install_title") =scopedstrTitle;
//            
//         }
//         
//         return psystem->payload("install_title");
//         
//      }
//      
//      
//      void application::get_temp_file_name(string & strRet, const ::scoped_string & scopedstrName, const ::scoped_string & scopedstrExtension)
//      {
//         
//         return get_temp_file_name_template(strRet, scopedstrName, scopedstrExtension, nullptr);
//         
//      }
//      
//      
//      void application::install_trace(const ::scoped_string & scopedstr)
//      {
//         
//         synchronous_lock synchronouslock(this->synchronization(), DEFAULT_SYNCHRONOUS_LOCK_SUFFIX);
//         
//         //::install::trace_file(this, m_strInstallTraceLabel).print(str);
//         
//      }
//      
//      
//      void application::install_trace(double dRate)
//      {
//         
//         synchronous_lock synchronouslock(this->synchronization(), DEFAULT_SYNCHRONOUS_LOCK_SUFFIX);
//         
//         //::install::trace_file(this, m_strInstallTraceLabel).print(dRate);
//         
//      }
//      
//      void     application::start_app(const ::scoped_string & scopedstr)
//      {
//         
//         //return error_failed;
//         
//      }
//      
//      
//      void application::get_temp_file_name_template(string & strRet, const ::scoped_string & scopedstrName, const ::scoped_string & scopedstrExtension, const ::scoped_string & scopedstrTemplate)
//      {
//         
//   #ifdef UNIVERSAL_WINDOWS
//         
//         string str(::winrt::Windows::Storage::ApplicationData::Current->TemporaryFolder->Path);
//         
//   #else
//         
//         //      char lpPathBuffer[MAX_PATH * 4];
//         //
//         //      unsigned int dwRetVal = //GetTempPath(sizeof(lpPathBuffer), lpPathBuffer);
//         //
//         //      if (dwRetVal > sizeof(lpPathBuffer) || (dwRetVal == 0))
//         //      {
//         //
//         //         return false;
//         //
//         //      }
//         
//         //      string str(lpPathBuffer);
//         
//         string str = directory()->time();
//         
//   #endif
//         
//         char bufItem[64];
//         
//         string strRelative;
//         
//         system_time systemtime(now_t{});
//         
//         //memory_set(&st, 0, sizeof(st));
//         
//         //get_system_time(&st);
//         
//         ansi_from_long_long(bufItem, systemtime.wYear, 10);
//         ansi_zero_pad(bufItem, 4);
//         strRelative += bufItem;
//         
//         ansi_from_long_long(bufItem, systemtime.wMonth, 10);
//         ansi_zero_pad(bufItem, 2);
//         strRelative += "-";
//         strRelative += bufItem;
//         
//         ansi_from_long_long(bufItem, systemtime.wDay, 10);
//         ansi_zero_pad(bufItem, 2);
//         strRelative += "-";
//         strRelative += bufItem;
//         
//         ansi_from_long_long(bufItem, systemtime.wHour, 10);
//         ansi_zero_pad(bufItem, 2);
//         strRelative += " ";
//         strRelative += bufItem;
//         
//         ansi_from_long_long(bufItem, systemtime.wMinute, 10);
//         ansi_zero_pad(bufItem, 2);
//         strRelative += "-";
//         strRelative += bufItem;
//         
//         ansi_from_long_long(bufItem, systemtime.wSecond, 10);
//         ansi_zero_pad(bufItem, 2);
//         strRelative += "-";
//         strRelative += bufItem;
//         
//         for (int i = 0; i < (1024 * 1024); i++)
//         {
//            
//            strRet = ::file::path(str) / (strRelative + "-" + hex::lower_case_from(i + 1)) / (string(scopedstrName) + string(".") + scopedstrExtension);
//            
//            //if (scopedstrTemplate != nullptr)
//            //{
//            
//            //   //if (install().is_file_ok(strRet, pszTemplate, ""))
//            //   // return true;
//            
//            //}
//            
//            
//            if (file_system()->exists(strRet))
//            {
//               
//               try
//               {
//                  
//                  file()->erase(strRet);
//                  
//               }
//               catch (...)
//               {
//                  
//                  continue;
//                  
//               }
//               
//               //return true;
//               
//               return;
//               
//            }
//            else
//            {
//               
//               //return true;
//               
//               return ;
//               
//            }
//            
//         }
//         
//         //return false;
//         
//         throw ::exception(error_failed);
//         
//      }
//      
//      
//      string application::get_install_configuration()
//      {
//         
//         return m_strInstallConfiguration;
//         
//      }
//      
//      
      //string application::parse_install_request_uri(const ::scoped_string & scopedstrRequestUri, const ::action_context & context)
      //{
      
      //   ::install::install_request request;
      
      //   request.m_strId = purl->get_param(strRequestUri, "app");
      //   request.m_strType = purl->get_param(strRequestUri, "type");
      //   request.m_strLocale = purl->get_param(strRequestUri, "locale");
      //   request.m_strSchema = purl->get_param(strRequestUri, "schema");
      //   request.m_strConfiguration = purl->get_param(strRequestUri, "configuration");
      //   request.m_strPlatform = purl->get_param(strRequestUri, "platform");
      //   request.m_strCommand = strRequestUri;
      
      //   papp->installer().request_install(request, context);
      
      //   return request.m_strId;
      
      //}
      
      
      //bool application::is_application_updated(const ::file::path & pathExe, const ::scoped_string & scopedstrAppId, const ::scoped_string & scopedstrPlatform, const ::scoped_string & scopedstrConfiguration, const ::scoped_string & scopedstrLocale, const ::scoped_string & scopedstrSchema, unsigned int & dwGoodToCheckAgain)
      //{
      
      //   string strBuild;
      
      //   if (!is_application_installed(pathExe, strBuild, strPlatform, strConfiguration, strLocale, strSchema, dwGoodToCheckAgain))
      //   {
      
      //      return false;
      
      //   }
      
      //   class status & status = m_mapUpdated[strAppId];
      
      //   status.m_iCheck++;
      
      //   if (status.m_bOk)
      //   {
      
      //      return true;
      
      //   }
      
      //   string strLatestBuildNumber = psystem->get_latest_build_number(strConfiguration, strAppId);
      
      //   dwGoodToCheckAgain = status.calc_when_is_good_to_check_again();
      
      //   return status.m_bOk;
      
      //}
      
      ::e_status application::on_html_response(::networking::application_socket * psocket, ::string& strHtml, const ::scoped_string & scopedstrUrl, const ::property_set& setPost)
      {
         //string strResponseState = system()->url()->get_param(strUrl, "state");
         
         //string strResponse;
         
         //if (m_strState != strResponseState)
         //{
         
         //   return error_not_found;
         
         //}
         
         //auto psystem = system();
         
         //auto pdatetime = psystem->datetime();
         
         //strHtml += "<html>";
         //strHtml += "<head>";
         //strHtml += "<meta charset=\"UTF-8\">";
         //// <from https://thomasbs.com/ <3tbs!!>
         //strHtml += "<link href=\"https://fonts.googleapis.com/css?family=Fira+Code&display=swap\" rel=\"stylesheet\">";
         //// </from https://thomasbs.com/ <3tbs!!>
         //strHtml += "</head>";
         //strHtml += "<body style=\"font-family:'Fira Code', monospace;\">";
         
         //string strCode = system()->url()->get_param(strUrl, "code");
         
         //auto& setConfig = m_setConfig;
         
         //::property_set set;
         
         
         ////code = 4 / P7q7W91a - oMsCeLvIaQm6bTrgtp7 &
         //   //client_id = your_client_id &
         //   //client_secret = your_client_secret &
         //   //redirect_uri = urn % 3Aietf % 3Awg % 3Aoauth % 3A2.0 % 3Aoob % 3Aauto &
         //   //grant_type = authorization_code
         
         ////set["headers"]["accept"] = "application/ssml+xml";
         //set["headers"]["User-Agent"] = get_app()->m_strAppId;
         //set["post"]["code"] = strCode;
         //set["post"]["client_id"] = setConfig["installed"]["client_id"];
         //set["post"]["client_secret"] = setConfig["installed"]["client_secret"];
         //set["post"]["redirect_uri"] = m_strRedirectUri;
         //set["post"]["grant_type"] = "authorization_code";
         
         
         
         
         //set["raw_http"] = true;
         //set["disable_common_name_cert_check"] = true;
         
         ///*       set["headers"]["Authorization"] = "Bearer " + m_strToken;*/
         
         
         //string strGet = setConfig["installed"]["token_uri"];
         
         //strResponse = http()->get(strGet, set);
         
         //::::property_set setResponse;
         
         //setResponse.parse_network_payload(strResponse);
         
         //if (!setResponse.has_property("access_token"))
         //{
         
         //   strHtml += "<h2>Some error (2)</h2>";
         
         //}
         //else
         //{
         
         //   string strToken = setResponse["access_token"];
         
         //   string strIdToken = setResponse["id_token"];
         
         //   if (strIdToken.has_character())
         //   {
         
         //      //               auto decoded = ::network_token::decode(strIdToken.c_str());
         //      //
         //      //               auto claims = decoded.get_payload_claims();
         //      //
         //      //               for (auto& e : claims)
         //      //               {
         //      //
         //      //                  std::cout << e.first << " = " << e.second << std::endl;
         //      //
         //      //               }
         //      //
         //      //               string strEmail = claims["email"].as_string().c_str();
         
         //   }
         
         //   m_strToken = strToken;
         
         //   ::api::api_login(m_strConfig, m_strProfile);
         
         //   strHtml += "<h1>";
         //   strHtml += "🌌 🌍 [ ca2/C++/Windows ] api_twitch";
         //   strHtml += "</h1>";
         
         //}
         
         
         
         ////}
         
         //m_bWaitingResponseFromUser = false;
         
         //strHtml += "<h4><a href=\"https://ca2.network/camilo\">cs</a><a href=\"https://ca2.io\">&lt;3</a><a href=\"https://thomasbs.com/\">tbs</a>!!</h4>";
         //strHtml += "&lt;3TBS - InfinityUnicodeCharacter - " + pdatetime->date_time_text() + "- +InfinityUnicodeCharacter - CA2(THWOASONE)";
         //strHtml += "</body>";
         //strHtml += "</html>";
         
         ////return strHtml;
         
         return ::success;
         
      }
      
   
   } // namespace websockets
   

} // namespace install



