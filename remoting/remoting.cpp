// Created by camilo on 2026-04-16 23:52 <3ThomasBorregaardSørensen!!
#include "framework.h"
#include "remoting.h"
#include "remoting/remoting_common/remoting.h"
#include "acme/filesystem/filesystem/file_context.h"
#include "innate_subsystem/resource_loader.h"
#include "innate_subsystem/subsystem.h"
#include "remoting_impact.h"
#include "resource.h"
#include "subsystem/socket/SocketIPv4.h"
#include "ConnectingDialog.h"
#include "acme/constant/id.h"


namespace remoting_remoting
{


   remoting::remoting() 
   {
   }

   remoting::~remoting()
   {
   }


   void remoting::on_start() 
   {
   
   construct_newø(m_pconnectingdialog);


   
   
   }


   void remoting::open_file(const ::file::path &path)
   {
         
      
      
   // new ::platform::system();

      // if (__argv[1])
      // {
      //
      //    if (::file_exists(__argv[1]))
      //    {
      //
      //       ::file::path path = __argv[1];
      //
      //    }
      //
      // }


      if (!m_pconfig)
      {

      ::remoting::ViewerSettingsManager::initInstance(RegistryPaths::VIEWER_PATH);
         ::remoting::SettingsManager *sm = ::remoting::ViewerSettingsManager::getInstance();
         m_pconfig = allocateø ::remoting::ViewerConfig(RegistryPaths::VIEWER_PATH);

         m_pconfig->loadFromStorage(sm);

      }

      if (!m_pconConf)
      {
         construct_newø(m_pconConf);

      }
      if (!m_pcondata)
      {
         construct_newø(m_pcondata);
      }
      // auto hinstance = remoting_impact_hinstance();
      bool isListening = false;
      //::string strHost;
      auto ini = file()->get_ini(path);

      ::string strHost = ini["host"];
      int iNumerator = ini["numerator"].as_int();
      int iDenominator = ini["denominator"].as_int();
      m_pconConf->enableFullscreen(true);
      m_pconConf->requestShapeUpdates(true);
      m_pconConf->ignoreShapeUpdates(false);

      if (iDenominator <= 0)
      {

         iDenominator = 1;
      }

      if (iNumerator <= 0)
      {

         iNumerator = 1;
      }

      m_pconConf->setScale(iNumerator, iDenominator);

      if (strHost.has_character())
      {
         m_pcondata->setHost(wstring(strHost).c_str());
         m_pconnectingdialog->initialize(this);
         m_pconnectingdialog->show();
         m_pconnectingdialog->set_host(strHost);
         m_pconnectingdialog->set_status("Initiating connection...");
         m_pconnectingdialog->set_phase1();
      }
      // ViewerCmdLine cmd(&condata, &conConf, &config, &isListening);
      //  resource-loader initialization
      // ResourceLoader resourceLoader(hInstance);
      // auto presource
      //  try {
      //     config.initLog(LogNames::LOG_DIR_NAME, LogNames::VIEWER_LOG_FILE_STUB_NAME);
      //  }  catch (...) {
      //  }

      // try {
      //   cmd.parse();
      // }
      // catch (const CommandLineFormatException &exception) {
      //   ::string strError(exception.get_message());
      //   main_subsystem()->message_box({},
      //     strError,
      //     ProductNames::VIEWER_PRODUCT_NAME,
      //     ::user::e_message_box_ok | MB_ICONERROR);
      //   return 0;
      // }
      // catch (const CommandLineFormatHelp &) {
      //   //cmd.onHelp();
      //   return 0;
      // }

      m_plogwriter = m_pconfig->getLogWriter();


      m_plogwriter->debug("main()");
      m_plogwriter->debug("loading settings from storage completed");
      m_plogwriter->debug("Log initialization completed");

      //int result = 0;
      try
      {


         if (!m_premotingimpact)
         {

            m_premotingimpact  = allocateø remoting_impact(this, this, ApplicationNames::WINDOW_CLASS_NAME, WindowNames::TVN_WINDOW_CLASS_NAME);

         }
         //// remoting_impact tvnViewer(this, hinstance,
         //remoting_impact tvnViewer(this, ApplicationNames::WINDOW_CLASS_NAME, WindowNames::TVN_WINDOW_CLASS_NAME);
         if (isListening)
         {
            // FIXME: set listening connection options.
            m_premotingimpact->startListening(ConnectionListener::DEFAULT_PORT);
         }
         else if (!m_pcondata->is_empty())
         {
            m_premotingimpact->newConnection(m_pcondata, m_pconConf);
         }
         else
         {
            m_premotingimpact->showLoginDialog();
         }
         //tvnViewer.run();
         //result = tvnViewer.getExitCode();
      }
      catch (const ::subsystem::Exception &ex)
      {
         main_subsystem()->message_box({}, main_subsystem()->string_table()->getString(IDS_UNKNOWN_ERROR_IN_VIEWER),
                                       ProductNames::VIEWER_PRODUCT_NAME,
                                       ::user::e_message_box_ok | ::user::e_message_box_icon_error);
         m_plogwriter->debug(ex.get_message());
      }

   }


   lresult remoting::handle_direct_id(const enum_id eid, wparam wparam, lparam lparam)
   {

         if (eid == id_remoting_connecting)
   {
            if (m_pconnectingdialog && m_pconnectingdialog->isWindow())
            {
               m_pconnectingdialog->postMessage(WM_USER + 328, id_remoting_connecting, wparam.m_number);
            }
   }
   else if (eid == id_remoting_connected)
   {

      if (m_pconnectingdialog->m_panimation)
      {

         m_pconnectingdialog->m_panimation->m_bRunning = false;
      }

      m_pconnectingdialog->hide();
   }
   return 0;

            }


} // namespace remoting_remoting
