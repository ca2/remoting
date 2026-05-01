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


#include "remoting/remoting/node_config/Configurator.h"

#include "innate_subsystem/gui/Dialog.h"
#include "innate_subsystem/gui/TextBox.h"
#include "innate_subsystem/gui/SpinControl.h"

namespace remoting_node
{
   class VideoRegionsConfigDialog : virtual public ::innate_subsystem::Dialog
   {
   public:
      VideoRegionsConfigDialog(Configurator *pconfigurator);
      virtual ~VideoRegionsConfigDialog();

      void setParentDialog(::innate_subsystem::DialogInterface * pdialog);;

      //
      // BaseDialog overrided methods
      //

      virtual bool onInitDialog();
      //virtual bool onNotify(unsigned int controlID, ::lparam data);
      virtual bool onCommand(unsigned int controlID, unsigned int notificationID) override;
      virtual bool onDestroy() { return true; }

      //
      // Helper methods
      //

      bool _002OnUpDown(int iControl, int iPos, int iDelta) override;

      bool validateInput();
      void updateUI();
      void apply();

   protected:

      void initControls();

      //
      // ::innate_subsystem::Control event handlers
      //

      void onRecognitionIntervalSpinChangePos(int iControlID, int iPos, int iDelta);
      void onRecognitionIntervalUpdate();
      void onVideoRegionsUpdate();

   protected:
      ::pointer < Configurator > m_pconfigurator;
      ::pointer < ::remoting_node::ServerConfig > m_pserverconfig;
      ::innate_subsystem::TextBox m_videoClasses;
      ::innate_subsystem::TextBox m_videoRects;
      ::innate_subsystem::TextBox m_videoRecognitionInterval;
      ::innate_subsystem::SpinControl m_videoRecognitionIntervalSpin;
      ::pointer < ::innate_subsystem::DialogInterface > m_pdialogParent;
   };
} // namespace remoting_node



