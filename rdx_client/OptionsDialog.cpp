// Copyright (C) 2012 GlavSoft LLC.
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
#include "OptionsDialog.h"
#include "acme/constant/user_notification.h"
//#include "remoting/remoting_rtc/remoting.h"
#include "remoting/remoting_rtc/client_config/ConnectionConfig.h"


namespace remoting_client
{
    OptionsDialog::OptionsDialog()
    : //Dialog(IDD_OPTIONS),
      m_connected(false)
    {
       initialize_dialog(IDD_OPTIONS);
    }

    void OptionsDialog::setConnectionConfig(::remoting_rtc::ConnectionConfig *conConfig)
    {
        m_pconnectionconfig = conConfig;
    }

    void OptionsDialog::setConnected()
    {
        m_connected = true;
    }

    bool OptionsDialog::onCommand(::u32 controlID, ::u32 notificationID)
    {
        if (controlID == ::innate_subsystem::e_control_id_ok) {
            if (onOkPressed()) {
                closeDialog(1);
            }
            return true;
        }
        if (controlID == ::innate_subsystem::e_control_id_cancel) {
            closeDialog(0);
            return true;
        }
        if (controlID == IDC_CVIEWONLY) {
            if (notificationID == ::user::e_notification_button_clicked) {
                onViewOnlyClick();
                return true;
            }
        }
        if (controlID == IDC_CEIGHTBIT) {
            if (notificationID == ::user::e_notification_button_clicked) {
                on8BitColorClick();
                return true;
            }
        }
        if (controlID == IDC_CJPEG) {
            if (notificationID == ::user::e_notification_button_clicked) {
                onAllowJpegCompressionClick();
                return true;
            }
        }
        if (controlID == IDC_CCOMPRLVL) {
            if (notificationID == ::user::e_notification_button_clicked) {
                onAllowCustomCompressionClick();
                return true;
            }
        }
        if (controlID == IDC_CUSEENC) {
            if (notificationID == ::user::e_notification_combo_box_selection_change) {
                onPreferredEncodingSelectionChange();
                return true;
            }
        }
        if (controlID == IDC_CSCALE) {
            if (notificationID == ::user::e_notification_combo_box_lost_focus) {
                onScaleKillFocus();
                return true;
            }
        }
        return false;
    }

    bool OptionsDialog::onInitDialog()
    {
        dialog_item(m_comboboxUseEnc, IDC_CUSEENC);
        dialog_item(m_checkboxEightBit, IDC_CEIGHTBIT);
        dialog_item(m_checkboxCompressionLevel, IDC_CCOMPRLVL);
        dialog_item(m_trackbarCompressionLevel, IDC_SCOMP);
        dialog_item(m_controlQuality, IDC_SQUALITY);
        dialog_item(m_checkboxJpeg, IDC_CJPEG);
        dialog_item(m_trackbarJpeg, IDC_SJPEG);
        dialog_item(m_controlQuality2, IDC_SQUALITY2);
        dialog_item(m_checkboxCopyrect, IDC_CCOPYRECT);
        dialog_item(m_checkboxViewonly, IDC_CVIEWONLY);
        dialog_item(m_checkboxClipboard, IDC_CDISCLIP);
        dialog_item(m_checkboxSharedses, IDC_CSHAREDSES);
        dialog_item(m_checkboxScale, IDC_CSCALE);
        dialog_item(m_checkboxFullscr, IDC_CFULLSCR);
        dialog_item(m_checkboxDeiconfy, IDC_CDEICONFY);
        dialog_item(m_checkboxSwapmouse, IDC_CSWAPMOUSE);
        dialog_item(m_checkboxTrack, IDC_RTRACK);
        dialog_item(m_checkboxCursor, IDC_RCURSOR);
        dialog_item(m_checkboxNcursor, IDC_RNCURSOR);
        dialog_item(m_checkboxDot, IDC_RDOT);
        dialog_item(m_checkboxSmalldot, IDC_RSMALLDOT);
        dialog_item(m_checkboxArrow, IDC_RARROW);
        dialog_item(m_checkboxNlocal, IDC_RNLOCAL);

        m_comboboxUseEnc.addItem("Raw", reinterpret_cast<void *>(::remoting_rtc::EncodingDefs::RAW));
        m_comboboxUseEnc.addItem("Hextile", reinterpret_cast<void *>(::remoting_rtc::EncodingDefs::HEXTILE));
        m_comboboxUseEnc.addItem("Tight", reinterpret_cast<void *>(::remoting_rtc::EncodingDefs::TIGHT));
        m_comboboxUseEnc.addItem("RRE", reinterpret_cast<void *>(::remoting_rtc::EncodingDefs::RRE));
        m_comboboxUseEnc.addItem("ZRLE", reinterpret_cast<void *>(::remoting_rtc::EncodingDefs::ZRLE));


        // FIXME: replaced literals to named constants
        ::string_array scaleComboText {"25","50", "75", "90",
                                       "100", "125","150", "Auto"};
        for (auto & str :scaleComboText)
        {
            m_checkboxScale.addItem(str);
        }

        m_trackbarJpeg.setRange(0, 9);
        m_trackbarCompressionLevel.setRange(0, 9);
        updateControlValues();
        return false;
    }

    void OptionsDialog::updateControlValues()
    {
        // Preferred encoding
        for (int i = 0; i < m_comboboxUseEnc.getItemsCount(); i++) {
            int enc = (int)(::iptr)m_comboboxUseEnc.getItemData(i);
            if (enc == m_pconnectionconfig->getPreferredEncoding()) {
                m_comboboxUseEnc.setSelectedItem(i);
                break;
            } // if found

            // set default value, if preferred encoding not in ::list_base
            if (enc == ::remoting_rtc::EncodingDefs::HEXTILE)
                m_comboboxUseEnc.setSelectedItem(i);
        } // for i

        m_checkboxEightBit.setChecked(m_pconnectionconfig->isUsing8BitColor());

        m_checkboxCompressionLevel.setChecked(m_pconnectionconfig->isCustomCompressionEnabled());
        m_checkboxJpeg.setChecked(m_pconnectionconfig->isJpegCompressionEnabled());

        m_checkboxCopyrect.setChecked(m_pconnectionconfig->isCopyRectAllowed());
        m_checkboxViewonly.setChecked(m_pconnectionconfig->isViewOnly());
        m_checkboxClipboard.setChecked(!m_pconnectionconfig->isClipboardEnabled());
        m_checkboxFullscr.setChecked(m_pconnectionconfig->isFullscreenEnabled());
        m_checkboxDeiconfy.setChecked(m_pconnectionconfig->isDeiconifyOnRemoteBellEnabled());
        m_checkboxSwapmouse.setChecked(m_pconnectionconfig->isMouseSwapEnabled());

        m_checkboxSharedses.setChecked(m_pconnectionconfig->getSharedFlag());
        m_checkboxSharedses.enableWindow(!m_connected);

        if (m_pconnectionconfig->isFitWindowEnabled()) {
            // FIXME: replace literal to named constant
            m_checkboxScale.setSelectedItem(7);
        } else {
            int n = m_pconnectionconfig->getScaleNumerator();
            int d = m_pconnectionconfig->getScaleDenominator();

            int percent = (n * 100) / d;

            ::string text;
            text.format("{}", percent);

            m_checkboxScale.setText(text);
        }

        {
            bool enableCursorUpdate = m_pconnectionconfig->isRequestingShapeUpdates();
            bool ignoreCursorUpdate = m_pconnectionconfig->isIgnoringShapeUpdates();
            m_checkboxTrack.setChecked(enableCursorUpdate && !ignoreCursorUpdate);
            m_checkboxCursor.setChecked(!enableCursorUpdate && ignoreCursorUpdate);
            m_checkboxNcursor.setChecked(enableCursorUpdate && ignoreCursorUpdate);
        }

        ::string labelText;
        {
            const int DEFAULT_COMPRESSION_LEVEL = 6;
            int level = DEFAULT_COMPRESSION_LEVEL;
            if (m_pconnectionconfig->isCustomCompressionEnabled())
                level = m_pconnectionconfig->getCustomCompressionLevel();
            m_trackbarCompressionLevel.setPos(level);
            labelText.format("{}", level);
            m_controlQuality.setText(labelText);
        }

        {
            const int DEFAULT_JPEG_COMPRESSION_LEVEL = 6;
            int level = DEFAULT_JPEG_COMPRESSION_LEVEL;
            if (m_pconnectionconfig->isJpegCompressionEnabled())
                level = m_pconnectionconfig->getJpegCompressionLevel();
            m_trackbarJpeg.setPos(level);
            labelText.format("{}", level);
            m_controlQuality2.setText(labelText);
        }

        switch (m_pconnectionconfig->getLocalCursorShape()) {
           case ::remoting_rtc::ConnectionConfig::SMALL_CURSOR:
                m_checkboxSmalldot.setChecked(true);
                break;
            case ::remoting_rtc::ConnectionConfig::NORMAL_CURSOR:
                m_checkboxArrow.setChecked(true);
                break;
            case ::remoting_rtc::ConnectionConfig::NO_CURSOR:
                m_checkboxNlocal.setChecked(true);
                break;
            default:
                m_checkboxDot.setChecked(true);
                break;
        }

        onViewOnlyClick();

        onAllowCustomCompressionClick();
        onAllowJpegCompressionClick();

        onCustomCompressionLevelScroll();
        onJpegCompressionLevelScroll();

        onPreferredEncodingSelectionChange();
        on8BitColorClick();
    }

    void OptionsDialog::onViewOnlyClick()
    {
        if (m_checkboxViewonly.isChecked()) {
            m_checkboxSwapmouse.enableWindow(false);
        } else {
            m_checkboxSwapmouse.enableWindow(true);
        }
    }

    void OptionsDialog::on8BitColorClick()
    {
        if (!m_checkboxEightBit.isChecked()) {
            if (m_checkboxJpeg.isChecked()) {
                enableJpegCompression(true);
            }
            m_checkboxJpeg.enableWindow(true);
        } else {
            m_checkboxJpeg.enableWindow(false);
            enableJpegCompression(false);
        }
    }

    void OptionsDialog::enableJpegCompression(bool enable)
    {
        m_trackbarJpeg.enableWindow(enable);
        m_controlQuality2.enableWindow(enable);
       dialog_item (IDC_SPOOR)->enableWindow(enable);
        dialog_item(IDC_SBEST2)->enableWindow(enable);
        dialog_item(IDC_STQUALITY2)->enableWindow(enable);
    }
    void OptionsDialog::onAllowCustomCompressionClick()
    {
        enableCustomCompression(m_checkboxCompressionLevel.isChecked());
    }

    void OptionsDialog::enableCustomCompression(bool enable)
    {
        m_trackbarCompressionLevel.enableWindow(enable);
        m_controlQuality.enableWindow(enable);
        dialog_item(IDC_SBEST)->enableWindow(enable);
        dialog_item(IDC_SFAST)->enableWindow(enable);
        dialog_item(IDC_STQUALITY)->enableWindow(enable);
    }

    void OptionsDialog::onAllowJpegCompressionClick()
    {
        enableJpegCompression(m_checkboxJpeg.isChecked());
    }

    void OptionsDialog::onPreferredEncodingSelectionChange()
    {
        int index = m_comboboxUseEnc.getSelectedItemIndex();
        if (index < 0) {
            return ;
        }
        int encoding = (int)(::iptr)m_comboboxUseEnc.getItemData(index);
        switch (encoding) {
            case ::remoting_rtc::EncodingDefs::TIGHT:
                enableCustomCompression(m_checkboxCompressionLevel.isChecked());
                m_checkboxCompressionLevel.enableWindow(true);
                break;
            default:
                enableCustomCompression(false);
                m_checkboxCompressionLevel.enableWindow(false);
                break;
        } // switch
    } // void

    void OptionsDialog::onCustomCompressionLevelScroll()
    {
        ::string labelText;
        labelText.format("{}", m_trackbarCompressionLevel.getPos());
        m_controlQuality.setText(labelText);
    }

    void OptionsDialog::onJpegCompressionLevelScroll()
    {
        ::string labelText;
        labelText.format("{}", m_trackbarJpeg.getPos());
        m_controlQuality2.setText(labelText);
    }

    void OptionsDialog::onMessageReceived(::u32 uMsg, ::wparam wParam, ::lparam lParam)
    {
        switch (uMsg) {
           case ::user::e_message_scroll_x:
                if (m_trackbarCompressionLevel.operating_system_window() == lParam) {
                    onCustomCompressionLevelScroll();
                }
                if (m_trackbarJpeg.operating_system_window() == lParam) {
                    onJpegCompressionLevelScroll();
                }
                break;
        }
    }

    void OptionsDialog::onScaleKillFocus()
    {
        //::string scaleText;
        auto scaleText = m_checkboxScale.getText();

        int scale;

        if (!MainSubsystem().StringParser().parseInt(scaleText, &scale)) {
            if (scaleText == "Auto") {
                return ;
            }
            scale = 100;
        }

        if (scale < 1) {
            scale = 1;
        } else if (scale > 400) {
            scale = 400;
        }

        scaleText.format("{}", scale);
        m_checkboxScale.setText(scaleText);
    }

    bool OptionsDialog::isInputValid()
    {
        int scaleInt;
        //::string scaleText;

        auto scaleText = m_checkboxScale.getText();

        if (scaleText == "Auto") {
            return true;
        }

        if (!MainSubsystem().StringParser().parseInt(scaleText, &scaleInt)) {
            ::string error;
            error.runtime_format(MainSubsystem().StringTable().getString(IDS_ERROR_VALUE_FIELD_ONLY_NUMERIC).c_str(),
                         MainSubsystem().StringTable().getString(IDS_OPTIONS_SCALE).c_str());
            MainSubsystem().message_box(operating_system_window(),
                       ::wstring(error),
                       ::wstring(MainSubsystem().StringTable().getString(IDS_OPTIONS_CAPTION)),
                       ::user::e_message_box_ok | ::user::e_message_box_icon_warning);
            return false;
        }

        if (scaleInt < 0) {
            ::string error;
            error.runtime_format(MainSubsystem().StringTable().getString(IDS_ERROR_VALUE_FIELD_ONLY_POSITIVE_NUMERIC).c_str(),
                         MainSubsystem().StringTable().getString(IDS_OPTIONS_SCALE).c_str());
            MainSubsystem().message_box(operating_system_window(),
                       error,
                       MainSubsystem().StringTable().getString(IDS_OPTIONS_CAPTION),
                       ::user::e_message_box_ok | ::user::e_message_box_icon_warning);
            return false;
        }

        return true;
    }

    bool OptionsDialog::onOkPressed()
    {
        if (!isInputValid())
            return false;
        apply();
        return true;
    }

    void OptionsDialog::apply()
    {
        // Preferred encoding
        int pesii = m_comboboxUseEnc.getSelectedItemIndex();
        if (pesii >= 0) {
            int preferredEncoding = (int)(::iptr)m_comboboxUseEnc.getItemData(pesii);
            m_pconnectionconfig->setPreferredEncoding(preferredEncoding);
        } else {
            ASSERT(pesii >= 0);
            m_pconnectionconfig->setPreferredEncoding(::remoting_rtc::EncodingDefs::TIGHT);
        }

        if (m_checkboxCompressionLevel.isChecked()) {
            int level = static_cast<int>(m_trackbarCompressionLevel.getPos());
            m_pconnectionconfig->setCustomCompressionLevel(level);
        } else {
            m_pconnectionconfig->disableCustomCompression();
        }

        if (m_checkboxJpeg.isChecked()) {
            int level = static_cast<int>(m_trackbarJpeg.getPos());
            m_pconnectionconfig->setJpegCompressionLevel(level);
        } else {
            m_pconnectionconfig->disableJpegCompression();
        }

        m_pconnectionconfig->use8BitColor(m_checkboxEightBit.isChecked());
        m_pconnectionconfig->allowCopyRect(m_checkboxCopyrect.isChecked());
        m_pconnectionconfig->setViewOnly(m_checkboxViewonly.isChecked());
        m_pconnectionconfig->enableClipboard(m_checkboxClipboard.isChecked());
        m_pconnectionconfig->enableFullscreen(m_checkboxFullscr.isChecked());
        m_pconnectionconfig->deiconifyOnRemoteBell(m_checkboxDeiconfy.isChecked());
        m_pconnectionconfig->swapMouse(m_checkboxSwapmouse.isChecked());
        m_pconnectionconfig->setSharedFlag(m_checkboxSharedses.isChecked());

        ::string scaleText;

        scaleText = m_checkboxScale.getText();

        int scaleInt = 0;

        if (MainSubsystem().StringParser().parseInt(scaleText, &scaleInt)) {
            m_pconnectionconfig->setScale(scaleInt, 100);
            m_pconnectionconfig->fitWindow(false);
        } else {
            m_pconnectionconfig->fitWindow(true);
        }

        if (m_checkboxTrack.isChecked()) {
            m_pconnectionconfig->requestShapeUpdates(true);
            m_pconnectionconfig->ignoreShapeUpdates(false);
        }

        if (m_checkboxCursor.isChecked()) {
            m_pconnectionconfig->requestShapeUpdates(false);
            m_pconnectionconfig->ignoreShapeUpdates(true);
        }

        if (m_checkboxNcursor.isChecked()) {
            m_pconnectionconfig->requestShapeUpdates(true);
            m_pconnectionconfig->ignoreShapeUpdates(true);
        }

        int localCursorShape = ::remoting_rtc::ConnectionConfig::DOT_CURSOR;
        if (m_checkboxSmalldot.isChecked()) {
            localCursorShape = ::remoting_rtc::ConnectionConfig::SMALL_CURSOR;
        } else if (m_checkboxArrow.isChecked()) {
            localCursorShape = ::remoting_rtc::ConnectionConfig::NORMAL_CURSOR;
        } else if (m_checkboxNlocal.isChecked()) {
            localCursorShape = ::remoting_rtc::ConnectionConfig::NO_CURSOR;
        }

        m_pconnectionconfig->setLocalCursorShape(localCursorShape);
    }
}//namespace remoting_client
