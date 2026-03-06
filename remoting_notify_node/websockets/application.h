#pragma once


#include "apex/interprocess/target.h"
#include "core/platform/application.h"
//#include "apex/networking/sockets/httpd/incoming_socket_thread.h"

//namespace httpd
//{
//
//   class incoming_socket_thread;
//
//}
namespace install
{


   //class socket_thread;
   class incoming_socket_thread;


   class CLASS_DECL_APP_CORE_LIBRARY application :
      virtual public ::core::application
      //, virtual public ::interprocess::target
   {
   public:


      //::library::library *                m_plibrary;
      // 
      ::pointer<::library::library>          m_plibraryFromInternet;

      ::pointer<::library::library>          m_plibraryFromLocalSystem;

      //window *                          m_pwindow;
      bool                                m_bRunMainLoop;
      bool                                m_bAdmin;

      double                              m_dProgress;
      double                              m_dProgress1;
      double                              m_dProgress2;

      string                              m_strInstallId;
      string                              m_strInstallConfiguration;

      //::task_pointer                      m_pthreadSsl;
      ::pointer < incoming_socket_thread > m_pincomingsocketthread;

      string                              m_strConfiguration;
      bool                                m_bFinished;

      string_map_base < bootstrap * >          m_bootstrap;

      //string_map_base < status >             m_mapUpdated;
      //string_map_base < status >             m_mapInstalled;


      ::e_status                          m_estatusMessage;
      ::pointer < ::mutex >                             m_pmutexInstallerInstall;
      //::inteprocess::handler    m_rxchannel;

      bool                                m_bInstallerInstalling;
      ::pointer < ::mutex >                 m_spmutexAppInstall;

      //::pointer<install>               m_spinstall;


      application();
      ~application() override;
      
      __DECLARE_APPLICATION_RELEASE_TIME();



      virtual void     start_app(const ::scoped_string & scopedstr);
      //virtual int start_installation(const ::scoped_string & scopedstrCommand);

      void get_temp_file_name_template(string & strRet, const ::scoped_string & scopedstrName, const ::scoped_string & scopedstrExtension, const ::scoped_string & scopedstrTemplate) override;
      void get_temp_file_name(string & strRet, const ::scoped_string & scopedstrName, const ::scoped_string & scopedstrExtension) override;

      void install_trace(const ::scoped_string & scopedstr) override;
      void install_trace(double dRate) override;

      void init_instance() override;

      void term_instance() override;

      void on_request(::request * prequest) override;

      //virtual bool is_application_installed(const ::scoped_string & scopedstrAppId, const ::scoped_string & scopedstrPlatform, const ::scoped_string & scopedstrConfiguration, unsigned int & dwGoodToCheckAgain);

      //bool is_application_installed(const ::file::path & pathExe, const ::scoped_string & scopedstrAppId, string & strBuild, const ::scoped_string & scopedstrPlatform, const ::scoped_string & scopedstrConfiguration, const ::scoped_string & scopedstrLocale, const ::scoped_string & scopedstrSchema, unsigned int & dwGoodToCheckAgain);

      //bool is_application_installed(const ::scoped_string & scopedstrAppId, string & strBuild, const ::scoped_string & scopedstrPlatform, const ::scoped_string & scopedstrConfiguration, const ::scoped_string & scopedstrLocale, const ::scoped_string & scopedstrSchema, unsigned int & dwGoodToCheckAgain);

      //bool is_application_updated(const ::file::path & pathExe, const ::scoped_string & scopedstrAppId, const ::scoped_string & scopedstrPlatform, const ::scoped_string & scopedstrConfiguration, const ::scoped_string & scopedstrLocale, const ::scoped_string & scopedstrSchema, unsigned int & dwGoodToCheckAgain);

      //bool is_application_updated(const ::scoped_string & scopedstrAppId, const ::scoped_string & scopedstrPlatform, const ::scoped_string & scopedstrConfiguration, const ::scoped_string & scopedstrLocale, const ::scoped_string & scopedstrSchema, unsigned int & dwGoodToCheckAgain);

      //::install::installer & installer()
      //{
      //   return *m_pinstaller;
      //}

      virtual string install_pick_command_line();


      virtual string install_get_title(const ::scoped_string & scopedstrTitle);

      virtual string install_get_build();

      virtual int check_soon_launch(const ::scoped_string & scopedstrAppId, bool bLaunch, unsigned int & dwGoodToCheckAgain);
      virtual int check_soon_file_launch(const ::scoped_string & scopedstrAppId, bool bLaunch, unsigned int & dwGoodToCheckAgain);
      virtual int check_soon_app_id(const ::scoped_string & scopedstrAppId, bool bLaunch, unsigned int & dwGoodToCheckAgain);
      virtual int check_soon_app_id1(const ::scoped_string & scopedstrAppId, bool bLaunch, unsigned int & dwGoodToCheckAgain);
      virtual int check_soon_app_id2(const ::scoped_string & scopedstrAppId, bool bLaunch, unsigned int & dwGoodToCheckAgain);


      virtual string get_install_configuration();


      virtual bool install_get_admin();
      virtual string install_get_id();


      void add_command_line(const ::scoped_string & scopedstr);
      string pick_command_line();

      class install & install();

      void install_set_id(const ::scoped_string & scopedstr);


      virtual void app_app_main();
      virtual bool start_app_app(const ::scoped_string & scopedstrPlatform);

      virtual void install_set_admin(bool bSet);

      //virtual bool show_window();
      //virtual bool hide_window();

      virtual void end_app_app();


      virtual bool is_user_service_running();

      //virtual string get_configuration(const ::scoped_string & scopedstrConfiguration);



      virtual void start_installer();

      virtual void get_system_locale_schema(string & strLocale, string & strSchema);

      virtual void defer_show_debug_box();




      virtual void app_app_admin();


      //virtual void on_interprocess_receive(::inteprocess::handler * prx, ::string && pszMessage) override;

      bool _handle_uri(const ::scoped_string & scopedstrUri) override;

      virtual void initialize_app_install();

      ::e_status on_html_response(::networking::application_socket * psocket, ::string& strHtml, const ::scoped_string & scopedstrUrl, const ::property_set& setPost) override;


   };


} // namespace install



