// Copyright (C) 2008,2009,2010,2011,2012 GlavSoft LLC.
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
#include "DesktopWinImpl.h"
#include "remoting/remoting_common/server_config/Configurator.h"
#include "desktop_ipc/UpdateHandlerClient.h"
#include "UpdateHandlerImpl.h"
#include "WindowsInputBlocker.h"
#include "desktop_ipc/UserInputClient.h"
#include "SasUserInput.h"
#include "WindowsUserInput.h"
//#include "remoting/remoting_common/win_system/Environment.h"
#include "remoting/remoting_common/win_system/WindowsDisplays.h"

DesktopWinImpl::DesktopWinImpl(ClipboardListener *extClipListener,
                       UpdateSendingListener *extUpdSendingListener,
                       AbnormDeskTermListener *extDeskTermListener,
                       ::subsystem::LogWriter *log)
: DesktopBaseImpl(extClipListener, extUpdSendingListener, extDeskTermListener, log),
  m_wallPaper(0),
  m_deskConf(0),
  m_plogwriter(log),
  m_scrDriverFactory(Configurator::getInstance()->getServerConfig())
{
  m_plogwriter->information("Creating DesktopWinImpl");

  logDesktopInfo();

  try {
    m_updateHandler = new UpdateHandlerImpl(this, &m_scrDriverFactory, m_plogwriter);
    bool ctrlAltDelEnabled = false;
    m_userInput = new WindowsUserInput(this, ctrlAltDelEnabled, m_plogwriter);
    m_deskConf = new DesktopConfigLocal(m_plogwriter);
    applyNewConfiguration();
    m_wallPaper = new WallpaperUtil(m_plogwriter);
    m_wallPaper->updateWallpaper();

    Configurator::getInstance()->addListener(this);
  } catch (::remoting::Exception &ex) {
    m_plogwriter->error("exception during DesktopWinImpl creaion: {}", ex.get_message());
    freeResource();
    throw;
  }
  resume();
}

DesktopWinImpl::~DesktopWinImpl()
{
  m_plogwriter->information("Deleting DesktopWinImpl");
  terminate();
  wait();
  freeResource();
  m_plogwriter->information("DesktopWinImpl deleted");
}

void DesktopWinImpl::freeResource()
{
  Configurator::getInstance()->removeListener(this);

  if (m_wallPaper) delete m_wallPaper;

  if (m_updateHandler) delete m_updateHandler;
  if (m_deskConf) delete m_deskConf;
  if (m_userInput) delete m_userInput;
}

void DesktopWinImpl::onTerminate()
{
  m_newUpdateEvent.notify();
}

void DesktopWinImpl::execute()
{
  m_plogwriter->information("DesktopWinImpl thread started");

  while (!isTerminating()) {
    m_newUpdateEvent.waitForEvent();
    if (!isTerminating()) {
      m_plogwriter->debug("DesktopWinImpl sendUpdate()");
      sendUpdate();
    }
  }

  m_plogwriter->information("DesktopWinImpl thread stopped");
}

bool DesktopWinImpl::isRemoteInputTempBlocked()
{
  return !m_deskConf->isRemoteInputAllowed();
}

void DesktopWinImpl::applyNewConfiguration()
{
  m_plogwriter->information("reload DesktopWinImpl configuration");
  m_deskConf->updateByNewSettings();
}

void DesktopWinImpl::logDesktopInfo()
{
  try {
    if (Environment::isAeroOn(m_plogwriter)) {
      m_plogwriter->debug("The Aero is On");
    } else {
      m_plogwriter->debug("The Aero is Off");
    }
  } catch (::exception &e) {
    m_plogwriter->error("Can't get information for the Aero: {}", e.get_message());
  }

  // Log all display coordinates
  WindowsDisplays m_winDisp;
  ::array_base<::int_rectangle> displays = m_winDisp.getDisplays();
  m_plogwriter->debug("The console desktop has {} displays", (int)displays.size());
  for (size_t i = 0; i < displays.size(); i++) {
    m_plogwriter->debug("Display {} placed at the {}, {}, %dx{} coordinates",
               i + 1,
               displays[i].left, displays[i].top,
               displays[i].width(), displays[i].height());
  }
}
