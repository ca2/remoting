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


#include "remoting/remoting_common/server_config/Configurator.h"

#include "innate_subsystem/BaseDialog.h"
#include "innate_subsystem/TextBox.h"
#include "innate_subsystem/SpinControl.h"

class VideoRegionsConfigDialog : public BaseDialog
{
public:
  VideoRegionsConfigDialog();
  virtual ~VideoRegionsConfigDialog();

  void setParentDialog(BaseDialog *dialog);

  //
  // BaseDialog overrided methods
  //

  virtual bool onInitDialog();
  virtual bool onNotify(unsigned int controlID, ::lparam data);
  virtual bool onCommand(unsigned int controlID, unsigned int notificationID);
  virtual bool onDestroy() { return true; }

  //
  // Helper methods
  //

  bool validateInput();
  void updateUI();
  void apply();

protected:

  void initControls();

  //
  // ::innate_subsystem::Control event handlers
  //

  void onRecognitionIntervalSpinChangePos(LPNMUPDOWN scopedstrMessage);
  void onRecognitionIntervalUpdate();
  void onVideoRegionsUpdate();

protected:
  ServerConfig *m_config;
  TextBox m_videoClasses;
  TextBox m_videoRects;
  TextBox m_videoRecognitionInterval;
  SpinControl m_videoRecognitionIntervalSpin;
  BaseDialog *m_parentDialog;
};


