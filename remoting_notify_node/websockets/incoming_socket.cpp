#include "framework.h"
#include "application.h"
#include "incoming_socket.h"
#include "socket.h"
#include "acme/filesystem/file/memory_file.h"
#include "acme/parallelization/synchronous_lock.h"
#include "aura/user/user/interaction.h"
#include "acme/prototype/prototype/url.h"
#include "acme/prototype/string/base64.h"
#include "acme/prototype/string/str.h"
#include "acme/crypto/crypto.h"
#include "apex/platform/system.h"
#include "aura/windowing/window.h"
#include "bred/id.h"
#include "app-core/library/application.h"
#include "app-core/library/library.h"


namespace install
{


   incoming_socket::incoming_socket()
   {
      m_typeAttendSocket = typeid(::install::socket);
   }


   incoming_socket::~incoming_socket()
   {

   }


   //void incoming_socket::on_send_response()
   //{

   //   string strHost = inheader("host");

   //   if (!strHost.case_insensitive_begins("localhost.ca2.network:2000"))
   //   {
   //      outattr("http_status_code") = 500;
   //      /*outattr("http_version") = "HTTP/1.1";*/
   //      outattr("http_status") = "Server Error";

   //      return;

   //   }

   //   string strReferer = inheader("referer");

   //   auto psystem = system();

   //   auto purl = psystem->url();

   //   ::string strServer = purl->get_server(strReferer);

   //   if (strServer.case_insensitive_equals("ca2.store"))
   //   {

   //      outheader("Access-Control-Allow-Origin") = "https://ca2.store";

   //   }
   //   else if (strServer.case_insensitive_equals("ca2.network"))
   //   {

   //      outheader("Access-Control-Allow-Origin") = "https://ca2.network";

   //   }
   //   else
   //   {

   //      outattr("http_status_code") = 401;
   //      /*outattr("http_version") = "HTTP/1.1";*/
   //      outattr("http_status") = "Access Denied";

   //      return;

   //   }

   //   //Access - Control - Allow - Origin: https://developer.mozilla.org
   ////Vary: Origin


   //   string strScript = url()->get_script(m_request.m_strRequestUri);

   //   //      if (strPrefix.has_character())
   //   {

   //      //strPrefix.case_insensitive_ends_eat("/");

   //      //outheader("Access-Control-Allow-Origin") = strPrefix;

   //      string strStart;

   //      string strText = m_request.m_strRequestUri;

   //      if (string_begins(m_request.m_strRequestUri, "/passthrough/"))
   //      {
   //      }
   //      else if (strText.case_insensitive_begins_eat("/start?app="))
   //      {

   //         string strAppId = ::str::token(strText, "&");

   //         if (strAppId.is_empty())
   //         {

   //            strAppId = strText;

   //         }

   //         auto papp = get_app();

   //         //auto estatus =
   //         //

   //         try
   //         {


   //            papp->start_app(strAppId);

   //            //            if (::succeeded(estatus))
   //            //            {

   //            outattr("http_status_code") = 200;
   //            /*outattr("http_version") = "HTTP/1.1";*/
   //            outattr("http_status") = "OK";

   //         }
   //         catch (...)
   //         {

   //            outattr("http_status_code") = 404;
   //            /*outattr("http_version") = "HTTP/1.1";*/
   //            outattr("http_status") = "Not Found";

   //         }


   //      }
   //      else if (m_request.m_strRequestUri.case_insensitive_begins("/start?"))
   //      {

   //         auto papp = get_app();

   //         string strId = papp->parse_install_request_uri(m_request.m_strRequestUri, ::e_source_user);

   //         outheader("content-type") = "text/plain; charset=UTF-8";

   //         response().file()->write(strId);

   //      }
   //      else if (m_request.m_strRequestUri == "/installing")
   //      {

   //         outheader("content-type") = "text/html; charset=UTF-8";

   //         //if (papp->installer().m_bInstalling)
   //         //{

   //         //   synchronous_lock synchronouslock(papp->m_plibrary->synchronization(), DEFAULT_SYNCHRONOUS_LOCK_SUFFIX);

   //         //   response().file()->write(papp->m_plibrary->m_itema[papp->installer().m_iRequest]->m_prequest->m_strId);
   //         //   response().file()->write("\r\n");
   //         //   response().file()->write(as_string(papp->installer().get_global_progress()));

   //         //}

   //         response().file()->write("Hello installer user!!");

   //      }
   //      else if (m_request.m_strRequestUri == "/queue")
   //      {

   //         outheader("content-type") = "text/html; charset=UTF-8";

   //         string_array_base stra;

   //         auto papp = get_app();

   //         synchronous_lock synchronouslock(papp->m_plibraryFromInternet->synchronization(), DEFAULT_SYNCHRONOUS_LOCK_SUFFIX);

   //         //for (int i = papp->installer().m_iRequest +
   //         //             (papp->installer().m_bInstalling ? 1: 0);
   //         //      i < papp->m_plibrary->m_itema.count(); i++)
   //         //{

   //         //   stra.add(papp->m_plibrary->m_itema[i]->m_prequest->m_strId);

   //         //}

   //         response().file()->write(stra.implode(","));

   //      }
   //      else if (m_request.m_strRequestUri == "/is_installed")
   //      {

   //         outheader("content-type") = "text/plain; charset=UTF-8";

   //         outattr("http_status_code") = 200;

   //         outattr("http_status") = "OK";

   //         //outheader("Access-Control-Allow-Origin") = "https://ca2.network";

   //         response().file()->write("localhost.ca2.network is installed!!");

   //      }
   //      else if (m_request.m_strRequestUri == "/start_ca2store")
   //      {

   //         outheader("content-type") = "text/plain; charset=UTF-8";

   //         outattr("http_status_code") = 200;

   //         outattr("http_status") = "OK";

   //         auto papp = get_app();

   //         auto puserinteraction = papp->m_pacmeuserinteractionMain;

   //         puserinteraction->display(e_display_normal, ::user::e_activation_set_foreground);

   //         puserinteraction->order(e_zorder_top);

   //         puserinteraction->set_need_redraw();

   //         puserinteraction->post_redraw();

   //         //puserinteraction->ActivateTopParent();


   //         puserinteraction->window()->switch_to_this_window(true);

   //         //outheader("Access-Control-Allow-Origin") = "https://ca2.network";

   //         response().file()->write("localhost.ca2.network starting...");

   //      }
   //      else if (string_begins(m_request.m_strRequestUri, "/matter/"))
   //      {
   //         //         outheader("Cache-control") = "public";
   //         //         outheader("Pragma") = "public";
   //         //         int iPathCount;
   //         //         outheader("Expires") = http()->gmdate(papp->datetime().strtotime(nullptr, "+1 day", 0, iPathCount));
   //         //#ifdef WINDOWS
   //         //         simple_file_server(::file::path("Z:\\") / m_request.m_strRequestUri);
   //         //#else
   //         //         simple_file_server(m_request.m_strRequestUri);
   //         //#endif
   //      }
   //      else if (strScript == "/ca2Store-ping")
   //      {

   //         response().file()->write("ca2Installer-pong");

   //      }
   //      else if (string_begins(m_request.m_strRequestUri, "/is_user_service_installed/"))
   //      {

   //         string strKey = inheader("sec-websocket-key");//
   //         //string strKey = "dGhlIHNhbXBsZSBub25jZQ==";
   //         strKey.trim();
   //         strKey += "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
   //         memory mem;
   //         mem.assign(strKey);
   //         memory memSha1;

   //         auto psystem = system();

   //         auto pcrypto = crypto();

   //         pcrypto->sha1(memSha1, mem);

   //         auto pbase64 = psystem->base64();

   //         strKey = pbase64->encode(memSha1);
   //         outheader("Sec-WebSocket-Accept") = strKey;
   //         outheader("Connection") = "Upgrade";
   //         outheader("Upgrade") = "websocket";
   //         outattr("http_status_code") = 101;
   //         outattr("http_version") = "HTTP/1.1";
   //         outattr("http_status") = "Switching Protocols";
   //         Respond();
   //         memory m;
   //         string strMessage = "yes_account_com";
   //         m.set_size(strMessage.length() + 2);
   //         m.data()[0] = 0x81;
   //         m.data()[1] = (unsigned char)(strMessage.length());
   //         ::memory_copy(&m.data()[2], strMessage.c_str(), strMessage.length());
   //         write(m.data(), m.size());
   //         return;
   //      }
   //      else
   //      {

   //         outheader("content-type") = "text/html; charset=UTF-8";

   //         string strH1;

   //         /*           if (m_pbootstrap->m_strInstallerName.has_character())
   //                    {

   //                       strH1 = "Installing " + m_pbootstrap->m_strInstallerName + " ...";

   //                    }
   //                    else
   //                    {

   //                       strH1 = "Installing ...";

   //                    }*/

   //      }
   //      //         response().file()->write("<html><head><meta http-equiv=\"refresh\" content=\"1\"><title>install.exe</title></head><body><h1>"+strH1+"</h1>install.exe is running.<br><br><h2>Current Install Activity:</h2>"+ m_pbootstrap->m_strHtmlLog+"</html>");

   //      //response().file()->write("<html><head><meta http-equiv=\"refresh\" content=\"1\"><title>install.exe</title></head><body><h1>" + strH1 + "</h1>install.exe is running.</html>");

   //      if (!outheaders().has_property("content-type") && response().file()->size() > 0)
   //      {

   //         outheader("content-type") = "text/html; charset=UTF-8";

   //      }
   //      //if (IsSSLServer())
   //      //{

   //      //   outheader("strict-transport-security") = "maximum-age=31536000; includeSubDomains";

   //      //}

   //      //string strReferer = inheader("referer");

   //      //auto strRefererServer = system()->url()->get_server(strReferer);

   //      //if (strRefererServer.case_insensitive_order("ca2.network") == 0)
   //      //{

   //      //   outheader("Access-Control-Allow-Origin") = "https://ca2.network";

   //      //}
   //      //else if (strRefererServer.case_insensitive_order("ca2.life") == 0)
   //      //{

   //      //   outheader("Access-Control-Allow-Origin") = "https://ca2.life";

   //      //}

   //   }

   //}


} // namespace install


