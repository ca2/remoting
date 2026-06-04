// Copyright (C) 2009,2010,2011,2012 GlavSoft LLC.
// All rights reserved.
//
//-------------------------------------------------------------------------
// This file is part of the T i g h t V N C software.  Please visit our Web site:
//
//                       http://www.t i g h t v n c.com/
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
#include "remoting/rfb_node_desktop/resource.h"
#include "VideoRegionsConfigDialog.h"
#include "ConfigDialog.h"
#include "UIDataAccess.h"
#include "CommonInputValidation.h"
#include "remoting/remoting_rfb/region/RectSerializer.h"
#include "subsystem/platform/StringParser.h"
#include "acme/constant/user_notification.h"

namespace remoting_rfb_node
{
   VideoRegionsConfigDialog::VideoRegionsConfigDialog(Configurator * pconfigurator)
   :  m_pconfigurator(pconfigurator), m_pdialogParent(NULL)
   {
      initialize_dialog(IDD_CONFIG_VIDEO_PAGE);
   }

   VideoRegionsConfigDialog::~VideoRegionsConfigDialog()
   {
   }

   void VideoRegionsConfigDialog::setParentDialog(::innate_subsystem::DialogInterface * pdialog)
   {
      m_pdialogParent = pdialog;
   }

   bool VideoRegionsConfigDialog::onInitDialog()
   {
      m_pserverconfig = m_pconfigurator->getServerConfig();
      initControls();
      updateUI();
      return true;
   }


   bool VideoRegionsConfigDialog::_002OnUpDown(::i32 iControl, ::i32 iPos, ::i32 iDelta)
   {

      if (iControl == IDC_VIDEO_RECOGNITION_INTERVAL_SPIN)
         {

         onRecognitionIntervalSpinChangePos(iControl, iPos, iDelta);
         // LPNMUPDOWN scopedstrMessage = (LPNMUPDOWN)data;
         // if (scopedstrMessage->hdr.code == UDN_DELTAPOS) {
         //    onRecognitionIntervalSpinChangePos(scopedstrMessage);
         // }
         return true;
      }

      return false;

   }
   // bool VideoRegionsConfigDialog::onNotify(::u32 controlID, ::lparam data)
   // {
   //    if (controlID == IDC_VIDEO_RECOGNITION_INTERVAL_SPIN) {
   //       LPNMUPDOWN scopedstrMessage = (LPNMUPDOWN)data;
   //       if (scopedstrMessage->hdr.code == UDN_DELTAPOS) {
   //          onRecognitionIntervalSpinChangePos(scopedstrMessage);
   //       }
   //    }
   //    return true;
   // }

   bool VideoRegionsConfigDialog::onCommand(::u32 controlID, ::u32 notificationID)
   {
      if (notificationID == ::user::e_notification_edit_update) {
         if (controlID == IDC_VIDEO_CLASS_NAMES || controlID == IDC_VIDEO_RECTS) {
            onVideoRegionsUpdate();
         } else if (controlID == IDC_VIDEO_RECOGNITION_INTERVAL) {
            onRecognitionIntervalUpdate();
         }
      }
      return true;
   }

   bool VideoRegionsConfigDialog::validateInput()
   {
      if (!CommonInputValidation::validateUINT(
        &m_videoRecognitionInterval,
        MainSubsystem().StringTable().getString(IDS_INVALID_VIDEO_RECOGNITION_INTERVAL))) {
         return false;
        }
      return true;
   }

   void VideoRegionsConfigDialog::updateUI()
   {
      ::string_array *videoClasses = m_pserverconfig->getVideoClassNames();
      ::i32_rectangle_array_base *rectangleaVideo = m_pserverconfig->getVideoRects();
      ::string textAreaData;
      ::i8 endLine[3] = {13, 10, 0};
      {
         AutoLock al(m_pserverconfig);
         textAreaData= "";
         for (size_t i = 0; i < videoClasses->size(); i++) {
            textAreaData+= videoClasses->at(i);
            textAreaData+=endLine;
         }
         ::i8 buffer[32];
         snprintf(buffer, sizeof(buffer), "%d", m_pserverconfig->getVideoRecognitionInterval());
         m_videoRecognitionInterval.setText(buffer);
      }
      m_videoClasses.setText(textAreaData);

      {
         AutoLock al(m_pserverconfig);
         textAreaData= "";
         for (size_t i = 0; i < rectangleaVideo->size(); i++) {
            ::i32_rectangle r = rectangleaVideo->at(i);
            ::string s;
            ::remoting_rfb::RectSerializer::toString(r, s);
            textAreaData+= s;
            textAreaData+=endLine;
         }
      }
      m_videoRects.setText(textAreaData);
   }

   void VideoRegionsConfigDialog::apply()
   {
      // FIXME: Bad code


      AutoLock al(m_pserverconfig);

      //
      // Clear old video classes names container
      //

      ::string_array *videoClasses = m_pserverconfig->getVideoClassNames();
      videoClasses->clear();
      ::i32_rectangle_array_base *rectangleaVideo = m_pserverconfig->getVideoRects();
      rectangleaVideo->clear();

      //
      // Split text from text area to string array
      //

      ::string classNames;
      classNames = m_videoClasses.getText();
      size_t count = 0;

      ::string_array delimiters;

      delimiters.add("\n");
      delimiters.add("\r");
      delimiters.add("\t");
      delimiters.add(",");
      delimiters.add(";");
      //::i8 delimiters[] = " \n\r\t,;";

      {
         ::string_array_base chunks;

         chunks.explode(delimiters, classNames, false);

         for (auto &chunk : chunks)
         {

            videoClasses->add(chunk);

         }
      }


      ::string videoRectsStringStorage;
      videoRectsStringStorage = m_videoRects.getText();
      count = 0;


      {

         ::string_array_base chunks;

         chunks.explode(delimiters, videoRectsStringStorage, false);


         for (auto &chunk : chunks)
         {
            try
            {

               auto rectangle = ::remoting_rfb::RectSerializer::toRect(chunk);
                  rectangleaVideo->add(rectangle);
            }
            catch (...)
            {
                  // Ignore wrong formatted strings
            }

         }

      }

      //
      // TODO: Create parseUInt method
      //

      ::string vriss;

      vriss = m_videoRecognitionInterval.getText();

      ::i32 interval;
      MainSubsystem().StringParser().parseInt(vriss, &interval);
      m_pserverconfig->setVideoRecognitionInterval((::u32)interval);

   }

   void VideoRegionsConfigDialog::initControls()
   {
      //HWND hwnd = operating_system_window();
      dialog_item(m_videoClasses, IDC_VIDEO_CLASS_NAMES);
      dialog_item(m_videoRects, IDC_VIDEO_RECTS);
      dialog_item(m_videoRecognitionInterval, IDC_VIDEO_RECOGNITION_INTERVAL);
      dialog_item(m_videoRecognitionIntervalSpin, IDC_VIDEO_RECOGNITION_INTERVAL_SPIN);

      //::i32 limitersTmp[] = {50, 200};
      //::i32 deltasTmp[] = {5, 10};

      ::array_base<::i32> limitters{50, 200};
      ::array_base<::i32> deltas{5, 10};

      m_videoRecognitionIntervalSpin.setBuddy(&m_videoRecognitionInterval);
      m_videoRecognitionIntervalSpin.setAccel(0, 1);
      m_videoRecognitionIntervalSpin.setRange32(0, INT_MAX);
      m_videoRecognitionIntervalSpin.setAutoAccelerationParams(limitters, deltas, 50);
      m_videoRecognitionIntervalSpin.enableAutoAcceleration(true);
   }

   void VideoRegionsConfigDialog::onRecognitionIntervalSpinChangePos(::i32 iControl, ::i32 iPos, ::i32 iDelta)
   {
      if (iControl == IDC_VIDEO_RECOGNITION_INTERVAL_SPIN)
      {
         m_videoRecognitionIntervalSpin.autoAccelerationHandler(iPos, iDelta);
      }
   }

   void VideoRegionsConfigDialog::onRecognitionIntervalUpdate()
   {

   auto pconfigdialog = m_pdialogParent->get_callback < ConfigDialog >();

      pconfigdialog->updateApplyButtonState();
   }

   void VideoRegionsConfigDialog::onVideoRegionsUpdate()
   {
   auto pconfigdialog = m_pdialogParent->get_callback < ConfigDialog >();
      pconfigdialog->updateApplyButtonState();
   }
} // namespace remoting_rfb_node



