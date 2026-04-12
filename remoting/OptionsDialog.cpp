// Copyright (C) 2012 GlavSoft LLC.
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
#include "OptionsDialog.h"
#include "acme/constant/user_notification.h"
#include "remoting/remoting_common/remoting.h"
#include "remoting/remoting_common/client_config/ConnectionConfig.h"


namespace remoting_remoting
{
    OptionsDialog::OptionsDialog()
    : //Dialog(IDD_OPTIONS),
      m_connected(false)
    {
       initialize_dialog(IDD_OPTIONS);
    }

    void OptionsDialog::setConnectionConfig(::remoting::ConnectionConfig *conConfig)
    {
        m_pconnectionconfig = conConfig;
    }

    void OptionsDialog::setConnected()
    {
        m_connected = true;
    }

    bool OptionsDialog::onCommand(unsigned int controlID, unsigned int notificationID)
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
        subclassControlById(m_pcomboboxUseEnc, IDC_CUSEENC);
        subclassControlById(m_pcheckboxEightBit, IDC_CEIGHTBIT);
        subclassControlById(m_pcheckboxCompressionLevel, IDC_CCOMPRLVL);
        subclassControlById(m_ptrackbarCompressionLevel, IDC_SCOMP);
        subclassControlById(m_pcontrolQuality, IDC_SQUALITY);
        subclassControlById(m_pcheckboxJpeg, IDC_CJPEG);
        subclassControlById(m_ptrackbarJpeg, IDC_SJPEG);
        subclassControlById(m_pcontrolQuality2, IDC_SQUALITY2);
        subclassControlById(m_pcheckboxCopyrect, IDC_CCOPYRECT);
        subclassControlById(m_pcheckboxViewonly, IDC_CVIEWONLY);
        subclassControlById(m_pcheckboxClipboard, IDC_CDISCLIP);
        subclassControlById(m_pcheckboxSharedses, IDC_CSHAREDSES);
        subclassControlById(m_pcheckboxScale, IDC_CSCALE);
        subclassControlById(m_pcheckboxFullscr, IDC_CFULLSCR);
        subclassControlById(m_pcheckboxDeiconfy, IDC_CDEICONFY);
        subclassControlById(m_pcheckboxSwapmouse, IDC_CSWAPMOUSE);
        subclassControlById(m_pcheckboxTrack, IDC_RTRACK);
        subclassControlById(m_pcheckboxCursor, IDC_RCURSOR);
        subclassControlById(m_pcheckboxNcursor, IDC_RNCURSOR);
        subclassControlById(m_pcheckboxDot, IDC_RDOT);
        subclassControlById(m_pcheckboxSmalldot, IDC_RSMALLDOT);
        subclassControlById(m_pcheckboxArrow, IDC_RARROW);
        subclassControlById(m_pcheckboxNlocal, IDC_RNLOCAL);

        m_pcomboboxUseEnc->addItem("Raw", reinterpret_cast<void *>(::remoting::EncodingDefs::RAW));
        m_pcomboboxUseEnc->addItem("Hextile", reinterpret_cast<void *>(::remoting::EncodingDefs::HEXTILE));
        m_pcomboboxUseEnc->addItem("Tight", reinterpret_cast<void *>(::remoting::EncodingDefs::TIGHT));
        m_pcomboboxUseEnc->addItem("RRE", reinterpret_cast<void *>(::remoting::EncodingDefs::RRE));
        m_pcomboboxUseEnc->addItem("ZRLE", reinterpret_cast<void *>(::remoting::EncodingDefs::ZRLE));


        // FIXME: replaced literals to named constants
        ::string_array scaleComboText {"25","50", "75", "90",
                                       "100", "125","150", "Auto"};
        for (auto & str :scaleComboText)
        {
            m_pcheckboxScale->addItem(str);
        }

        m_ptrackbarJpeg->setRange(0, 9);
        m_ptrackbarCompressionLevel->setRange(0, 9);
        updateControlValues();
        return false;
    }

    void OptionsDialog::updateControlValues()
    {
        // Preferred encoding
        for (int i = 0; i < m_pcomboboxUseEnc->getItemsCount(); i++) {
            int enc = reinterpret_cast<int>(m_pcomboboxUseEnc->getItemData(i));
            if (enc == m_pconnectionconfig->getPreferredEncoding()) {
                m_pcomboboxUseEnc->setSelectedItem(i);
                break;
            } // if found

            // set default value, if preferred encoding not in ::list_base
            if (enc == ::remoting::EncodingDefs::HEXTILE)
                m_pcomboboxUseEnc->setSelectedItem(i);
        } // for i

        m_pcheckboxEightBit->setChecked(m_pconnectionconfig->isUsing8BitColor());

        m_pcheckboxCompressionLevel->setChecked(m_pconnectionconfig->isCustomCompressionEnabled());
        m_pcheckboxJpeg->setChecked(m_pconnectionconfig->isJpegCompressionEnabled());

        m_pcheckboxCopyrect->setChecked(m_pconnectionconfig->isCopyRectAllowed());
        m_pcheckboxViewonly->setChecked(m_pconnectionconfig->isViewOnly());
        m_pcheckboxClipboard->setChecked(!m_pconnectionconfig->isClipboardEnabled());
        m_pcheckboxFullscr->setChecked(m_pconnectionconfig->isFullscreenEnabled());
        m_pcheckboxDeiconfy->setChecked(m_pconnectionconfig->isDeiconifyOnRemoteBellEnabled());
        m_pcheckboxSwapmouse->setChecked(m_pconnectionconfig->isMouseSwapEnabled());

        m_pcheckboxSharedses->setChecked(m_pconnectionconfig->getSharedFlag());
        m_pcheckboxSharedses->enableWindow(!m_connected);

        if (m_pconnectionconfig->isFitWindowEnabled()) {
            // FIXME: replace literal to named constant
            m_pcheckboxScale->setSelectedItem(7);
        } else {
            int n = m_pconnectionconfig->getScaleNumerator();
            int d = m_pconnectionconfig->getScaleDenominator();

            int percent = (n * 100) / d;

            ::string text;
            text.format("{}", percent);

            m_pcheckboxScale->setText(text);
        }

        {
            bool enableCursorUpdate = m_pconnectionconfig->isRequestingShapeUpdates();
            bool ignoreCursorUpdate = m_pconnectionconfig->isIgnoringShapeUpdates();
            m_pcheckboxTrack->setChecked(enableCursorUpdate && !ignoreCursorUpdate);
            m_pcheckboxCursor->setChecked(!enableCursorUpdate && ignoreCursorUpdate);
            m_pcheckboxNcursor->setChecked(enableCursorUpdate && ignoreCursorUpdate);
        }

        ::string labelText;
        {
            const int DEFAULT_COMPRESSION_LEVEL = 6;
            int level = DEFAULT_COMPRESSION_LEVEL;
            if (m_pconnectionconfig->isCustomCompressionEnabled())
                level = m_pconnectionconfig->getCustomCompressionLevel();
            m_ptrackbarCompressionLevel->setPos(level);
            labelText.format("{}", level);
            m_pcontrolQuality->setText(labelText);
        }

        {
            const int DEFAULT_JPEG_COMPRESSION_LEVEL = 6;
            int level = DEFAULT_JPEG_COMPRESSION_LEVEL;
            if (m_pconnectionconfig->isJpegCompressionEnabled())
                level = m_pconnectionconfig->getJpegCompressionLevel();
            m_ptrackbarJpeg->setPos(level);
            labelText.format("{}", level);
            m_pcontrolQuality2->setText(labelText);
        }

        switch (m_pconnectionconfig->getLocalCursorShape()) {
           case ::remoting::ConnectionConfig::SMALL_CURSOR:
                m_pcheckboxSmalldot->setChecked(true);
                break;
            case ::remoting::ConnectionConfig::NORMAL_CURSOR:
                m_pcheckboxArrow->setChecked(true);
                break;
            case ::remoting::ConnectionConfig::NO_CURSOR:
                m_pcheckboxNlocal->setChecked(true);
                break;
            default:
                m_pcheckboxDot->setChecked(true);
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
        if (m_pcheckboxViewonly->isChecked()) {
            m_pcheckboxSwapmouse->enableWindow(false);
        } else {
            m_pcheckboxSwapmouse->enableWindow(true);
        }
    }

    void OptionsDialog::on8BitColorClick()
    {
        if (!m_pcheckboxEightBit->isChecked()) {
            if (m_pcheckboxJpeg->isChecked()) {
                enableJpegCompression(true);
            }
            m_pcheckboxJpeg->enableWindow(true);
        } else {
            m_pcheckboxJpeg->enableWindow(false);
            enableJpegCompression(false);
        }
    }

    void OptionsDialog::enableJpegCompression(bool enable)
    {
        m_ptrackbarJpeg->enableWindow(enable);
        m_pcontrolQuality2->enableWindow(enable);
       dialog_item (IDC_SPOOR)->enableWindow(enable);
        dialog_item(IDC_SBEST2)->enableWindow(enable);
        dialog_item(IDC_STQUALITY2)->enableWindow(enable);
    }
    void OptionsDialog::onAllowCustomCompressionClick()
    {
        enableCustomCompression(m_pcheckboxCompressionLevel->isChecked());
    }

    void OptionsDialog::enableCustomCompression(bool enable)
    {
        m_ptrackbarCompressionLevel->enableWindow(enable);
        m_pcontrolQuality->enableWindow(enable);
        dialog_item(IDC_SBEST)->enableWindow(enable);
        dialog_item(IDC_SFAST)->enableWindow(enable);
        dialog_item(IDC_STQUALITY)->enableWindow(enable);
    }

    void OptionsDialog::onAllowJpegCompressionClick()
    {
        enableJpegCompression(m_pcheckboxJpeg->isChecked());
    }

    void OptionsDialog::onPreferredEncodingSelectionChange()
    {
        int index = m_pcomboboxUseEnc->getSelectedItemIndex();
        if (index < 0) {
            return ;
        }
        int encoding = reinterpret_cast<int>(m_pcomboboxUseEnc->getItemData(index));
        switch (encoding) {
            case ::remoting::EncodingDefs::TIGHT:
                enableCustomCompression(m_pcheckboxCompressionLevel->isChecked());
                m_pcheckboxCompressionLevel->enableWindow(true);
                break;
            default:
                enableCustomCompression(false);
                m_pcheckboxCompressionLevel->enableWindow(false);
                break;
        } // switch
    } // void

    void OptionsDialog::onCustomCompressionLevelScroll()
    {
        ::string labelText;
        labelText.format("{}", m_ptrackbarCompressionLevel->getPos());
        m_pcontrolQuality->setText(labelText);
    }

    void OptionsDialog::onJpegCompressionLevelScroll()
    {
        ::string labelText;
        labelText.format("{}", m_ptrackbarJpeg->getPos());
        m_pcontrolQuality2->setText(labelText);
    }

    void OptionsDialog::onMessageReceived(unsigned int uMsg, ::wparam wParam, ::lparam lParam)
    {
        switch (uMsg) {
           case ::user::e_message_scroll_x:
                if (m_ptrackbarCompressionLevel->operating_system_window() == lParam) {
                    onCustomCompressionLevelScroll();
                }
                if (m_ptrackbarJpeg->operating_system_window() == lParam) {
                    onJpegCompressionLevelScroll();
                }
                break;
        }
    }

    void OptionsDialog::onScaleKillFocus()
    {
        //::string scaleText;
        auto scaleText = m_pcheckboxScale->getText();

        int scale;

        if (!main_subsystem()->string_parser()->parseInt(scaleText, &scale)) {
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
        m_pcheckboxScale->setText(scaleText);
    }

    bool OptionsDialog::isInputValid()
    {
        int scaleInt;
        //::string scaleText;

        auto scaleText = m_pcheckboxScale->getText();

        if (scaleText == "Auto") {
            return true;
        }

        if (!main_subsystem()->string_parser()->parseInt(scaleText, &scaleInt)) {
            ::string error;
            error.formatf(main_subsystem()->string_table()->getString(IDS_ERROR_VALUE_FIELD_ONLY_NUMERIC).c_str(),
                         main_subsystem()->string_table()->getString(IDS_OPTIONS_SCALE).c_str());
            main_subsystem()->message_box(operating_system_window(),
                       ::wstring(error),
                       ::wstring(main_subsystem()->string_table()->getString(IDS_OPTIONS_CAPTION)),
                       ::user::e_message_box_ok | ::user::e_message_box_icon_warning);
            return false;
        }

        if (scaleInt < 0) {
            ::string error;
            error.formatf(main_subsystem()->string_table()->getString(IDS_ERROR_VALUE_FIELD_ONLY_POSITIVE_NUMERIC).c_str(),
                         main_subsystem()->string_table()->getString(IDS_OPTIONS_SCALE).c_str());
            main_subsystem()->message_box(operating_system_window(),
                       error,
                       main_subsystem()->string_table()->getString(IDS_OPTIONS_CAPTION),
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
        int pesii = m_pcomboboxUseEnc->getSelectedItemIndex();
        if (pesii >= 0) {
            int preferredEncoding = reinterpret_cast<int>(m_pcomboboxUseEnc->getItemData(pesii));
            m_pconnectionconfig->setPreferredEncoding(preferredEncoding);
        } else {
            _ASSERT(pesii >= 0);
            m_pconnectionconfig->setPreferredEncoding(::remoting::EncodingDefs::TIGHT);
        }

        if (m_pcheckboxCompressionLevel->isChecked()) {
            int level = static_cast<int>(m_ptrackbarCompressionLevel->getPos());
            m_pconnectionconfig->setCustomCompressionLevel(level);
        } else {
            m_pconnectionconfig->disableCustomCompression();
        }

        if (m_pcheckboxJpeg->isChecked()) {
            int level = static_cast<int>(m_ptrackbarJpeg->getPos());
            m_pconnectionconfig->setJpegCompressionLevel(level);
        } else {
            m_pconnectionconfig->disableJpegCompression();
        }

        m_pconnectionconfig->use8BitColor(m_pcheckboxEightBit->isChecked());
        m_pconnectionconfig->allowCopyRect(m_pcheckboxCopyrect->isChecked());
        m_pconnectionconfig->setViewOnly(m_pcheckboxViewonly->isChecked());
        m_pconnectionconfig->enableClipboard(m_pcheckboxClipboard->isChecked());
        m_pconnectionconfig->enableFullscreen(m_pcheckboxFullscr->isChecked());
        m_pconnectionconfig->deiconifyOnRemoteBell(m_pcheckboxDeiconfy->isChecked());
        m_pconnectionconfig->swapMouse(m_pcheckboxSwapmouse->isChecked());
        m_pconnectionconfig->setSharedFlag(m_pcheckboxSharedses->isChecked());

        ::string scaleText;

        scaleText = m_pcheckboxScale->getText();

        int scaleInt = 0;

        if (main_subsystem()->string_parser()->parseInt(scaleText, &scaleInt)) {
            m_pconnectionconfig->setScale(scaleInt, 100);
            m_pconnectionconfig->fitWindow(false);
        } else {
            m_pconnectionconfig->fitWindow(true);
        }

        if (m_pcheckboxTrack->isChecked()) {
            m_pconnectionconfig->requestShapeUpdates(true);
            m_pconnectionconfig->ignoreShapeUpdates(false);
        }

        if (m_pcheckboxCursor->isChecked()) {
            m_pconnectionconfig->requestShapeUpdates(false);
            m_pconnectionconfig->ignoreShapeUpdates(true);
        }

        if (m_pcheckboxNcursor->isChecked()) {
            m_pconnectionconfig->requestShapeUpdates(true);
            m_pconnectionconfig->ignoreShapeUpdates(true);
        }

        int localCursorShape = ::remoting::ConnectionConfig::DOT_CURSOR;
        if (m_pcheckboxSmalldot->isChecked()) {
            localCursorShape = ::remoting::ConnectionConfig::SMALL_CURSOR;
        } else if (m_pcheckboxArrow->isChecked()) {
            localCursorShape = ::remoting::ConnectionConfig::NORMAL_CURSOR;
        } else if (m_pcheckboxNlocal->isChecked()) {
            localCursorShape = ::remoting::ConnectionConfig::NO_CURSOR;
        }

        m_pconnectionconfig->setLocalCursorShape(localCursorShape);
    }
}//namespace remoting_remoting
