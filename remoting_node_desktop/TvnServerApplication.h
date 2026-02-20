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

#pragma once


#include "remoting/remoting_common/util/CommonHeader.h"
#include "remoting/remoting_common/util/winhdr.h"

#include "remoting/remoting_common/win_system/WindowsApplication.h"

#include "TvnServer.h"
#include "TvnServerListener.h"
#include "WsConfigRunner.h"
//#include "log_writer/FileLogWriter.h"
#include "LogInitListener.h"

/**
 * Windows TightVNC server application.
 * Used for running TightVNC server as single windows application.
 */
class TvnServerApplication : public WindowsApplication,
                             public TvnServerListener,
                             private LogInitListener
{
public:
  /**
   * Creates TightVNC server application instance.
   * @param hInstance HINSTANCE of application.
   * @param commaneLine command line string.
   */
  TvnServerApplication(HINSTANCE hInstance,
                       const ::scoped_string & scopedstrwindowClassName,
                       const ::scoped_string & scopedstrCommandLine,
                       NewConnectionEvents *newConnectionEvents);
  /**
   * Deletes TightVNC server application instance.
   */
  virtual ~TvnServerApplication();

  /**
   * Runs TightVNC server windows application.
   *
   * Makes several things:
   *   1) Starts TigthVNC server.
   *   2) Starts TvnControl application.
   *   3) Enters main windows scopedstrMessage loop.
   *   4) Stops TigthVNC server.
   *
   * @return application exit code.
   */
  virtual int run();

  /**
   * Inherited from TvnServerListener abstact class.
   *
   * Shutdowns WindowsApplication.
   */
  virtual void onTvnServerShutdown();

private:
  // This is a callback function that calls when the log can be initialized.
  virtual void onLogInit(const ::scoped_string & scopedstrLogDir, const ::scoped_string & scopedstrFileName, unsigned char logLevel);

  // This is a callback function that calls when log properties have changed.
  virtual void onChangeLogProps(const ::scoped_string & scopedstrNewLogDir, unsigned char newLevel);

  FileLogWriter m_fileLogWriter;

  /**
   * Command line string.
   */
  ::string m_commandLine;
  /**
   * TightVNC server.
   */
  TvnServer *m_tvnServer;
  /**
   * TvnControl application watcher.
   */
  WsConfigRunner *m_tvnControlRunner;

  NewConnectionEvents *m_newConnectionEvents;
};


