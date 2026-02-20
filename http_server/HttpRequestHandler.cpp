// Copyright (C) 2009,2010,2011,2012 GlavSoft LLC.
// All rights reserved.
//
//-------------------------------------------------------------------------
// This file is part of the TightVNC software.  Please visit our Web site:
//
//                       http://www.tightvnc.com/
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, w_rite to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//
#include "framework.h"
#include "HttpRequestHandler.h"
#include "HttpRequest.h"
#include "HttpReply.h"
#include "AppletParameter.h"
#include "VncViewerJarBody.h"
//#include "remoting/remoting_common/win_system/Environment.h"
#include "remoting/remoting_common/server_config/Configurator.h"
//#include "remoting/remoting_common/util/::string.h"
#include "remoting_node_desktop/NamingDefs.h"

HttpRequestHandler::HttpRequestHandler(DataInputStream *dataInput,
                                       DataOutputStream *dataOutput,
                                       LogWriter *log,
                                       const ::scoped_string & scopedstrPeerHost)
: m_dataInput(dataInput), m_dataOutput(dataOutput),
  m_peerHost(peerHost),
  m_log(log)
{
}

HttpRequestHandler::~HttpRequestHandler()
{
}

// FIXME: Refactor this code.
void HttpRequestHandler::processRequest()
{
  HttpRequest httpRequest(m_dataInput);

  httpRequest.readHeader();

  ::string ansiRequest(httpRequest.getRequest());
  ::string request;
  ansiRequest.toStringStorage(&request);

  if (!httpRequest.parseHeader()) {
    m_log->warning("invalid http request from {}", m_peerHost);
    return ;
  }

  request.replaceChar(_T('\n'), _T(' '));
  request.replaceChar(_T('\t'), _T(' '));

  m_log->debug("\"{}\" from {}", request, m_peerHost);

  HttpReply reply(m_dataOutput);

  bool pageFound = false;

  //
  // Index page.
  //

  if (strcmp(httpRequest.getFilename(), "/") == 0) {

    //
    // Check arguments.
    //

    ::string paramsString("\n");

    bool isAppletArgsValid = true;

    //
    // Generate applet parameters string.
    //

    bool paramsInUrlIsEnabled = Configurator::getInstance()->getServerConfig()->isAppletParamInUrlEnabled();

    if (httpRequest.hasArguments() && paramsInUrlIsEnabled) {
      ArgList *args = httpRequest.getArguments();

      for (size_t i = 0; i < args->getCount(); i++) {
        const char *key = args->getKey(i);

        AppletParameter parameter(key, args->getValue(key));

        if (!parameter.isValid()) {
          isAppletArgsValid = false;
          break;
        }

        paramsString.appendString(parameter.getFormattedString());
      } // for all arguments.
    } // if has arguments.

    reply.send200();

    if (!isAppletArgsValid) {
      m_dataOutput->writeFully(HttpStrings::HTTP_MSG_BADPARAMS,
                               strlen(HttpStrings::HTTP_MSG_BADPARAMS));
    } else {
      ::string page;

      ::string computerName(DefaultNames::DEFAULT_COMPUTER_NAME);

      Environment::getComputerName(&computerName);

      ::string computerNameANSI(&computerName);

      page.format(HttpStrings::HTTP_INDEX_PAGE_FORMAT,
                  computerNameANSI,
                  Configurator::getInstance()->getServerConfig()->getRfbPort(),
                  paramsString);
      m_dataOutput->writeFully(page, page.length());
    } // if applet arguments is valid.

    pageFound = true;
  } else if ((strcmp(httpRequest.getFilename(), "/tightvnc-jviewer.jar") == 0)) {
    reply.send200();
    m_dataOutput->writeFully(VNC_VIEWER_JAR_BODY, sizeof(VNC_VIEWER_JAR_BODY));

    pageFound = true;
  }

  //
  // 404 Not Found.
  //

  if (!pageFound) {
    reply.send404();
  }
}
