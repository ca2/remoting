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


namespace remoting_remoting
{
    OptionsDialog::OptionsDialog()
    : Dialog(IDD_OPTIONS),
      m_connected(false)
    {
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
        if (controlID == ::innate_subsystem::IDOK) {
            if (onOkPressed()) {
                closeDialog(1);
            }
            return true;
        }
        if (controlID == ::innate_subsystem::IDCANCEL) {
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
            if (notificationID == CBN_SELCHANGE) {
                onPreferredEncodingSelectionChange();
                return true;
            }
        }
        if (controlID == IDC_CSCALE) {
            if (notificationID == CBN_KILLFOCUS) {
                onScaleKillFocus();
                return true;
            }
        }
        return false;
    }

    bool OptionsDialog::onInitDialog()
    {
        subclassControlById(m_useEnc, IDC_CUSEENC);
        subclassControlById(m_eightBit, IDC_CEIGHTBIT);
        subclassControlById(m_compLvl, IDC_CCOMPRLVL);
        subclassControlById(m_tcompLvl, IDC_SCOMP);
        subclassControlById(m_quality, IDC_SQUALITY);
        subclassControlById(m_jpeg, IDC_CJPEG);
        subclassControlById(m_tjpeg, IDC_SJPEG);
        subclassControlById(m_quality2, IDC_SQUALITY2);
        subclassControlById(m_copyrect, IDC_CCOPYRECT);
        subclassControlById(m_viewonly, IDC_CVIEWONLY);
        subclassControlById(m_disclip, IDC_CDISCLIP);
        subclassControlById(m_sharedses, IDC_CSHAREDSES);
        subclassControlById(m_scale, IDC_CSCALE);
        subclassControlById(m_fullscr, IDC_CFULLSCR);
        subclassControlById(m_deiconfy, IDC_CDEICONFY);
        subclassControlById(m_swapmouse, IDC_CSWAPMOUSE);
        subclassControlById(m_track, IDC_RTRACK);
        subclassControlById(m_cursor, IDC_RCURSOR);
        subclassControlById(m_ncursor, IDC_RNCURSOR);
        subclassControlById(m_dot, IDC_RDOT);
        subclassControlById(m_smalldot, IDC_RSMALLDOT);
        subclassControlById(m_arrow, IDC_RARROW);
        subclassControlById(m_nlocal, IDC_RNLOCAL);

        m_useEnc.addItem("Raw", reinterpret_cast<void *>(EncodingDefs::RAW));
        m_useEnc.addItem("Hextile", reinterpret_cast<void *>(EncodingDefs::HEXTILE));
        m_useEnc.addItem("Tight", reinterpret_cast<void *>(EncodingDefs::TIGHT));
        m_useEnc.addItem("RRE", reinterpret_cast<void *>(EncodingDefs::RRE));
        m_useEnc.addItem("ZRLE", reinterpret_cast<void *>(EncodingDefs::ZRLE));


        // FIXME: replaced literals to named constants
        ::string_array scaleComboText {"25","50", "75", "90",
                                       "100", "125","150", "Auto"};
        for (auto & str :scaleComboText)
        {
            m_scale.addItem(str);
        }

        m_tjpeg.setRange(0, 9);
        m_tcompLvl.setRange(0, 9);
        updateControlValues();
        return false;
    }

    void OptionsDialog::updateControlValues()
    {
        // Preferred encoding
        for (int i = 0; i < m_useEnc.getItemsCount(); i++) {
            int enc = reinterpret_cast<int>(m_useEnc.getItemData(i));
            if (enc == m_pconnectionconfig->getPreferredEncoding()) {
                m_useEnc.setSelectedItem(i);
                break;
            } // if found

            // set default value, if preferred encoding not in ::list_base
            if (enc == EncodingDefs::HEXTILE)
                m_useEnc.setSelectedItem(i);
        } // for i

        m_eightBit.check(m_pconnectionconfig->isUsing8BitColor());

        m_compLvl.check(m_pconnectionconfig->isCustomCompressionEnabled());
        m_jpeg.check(m_pconnectionconfig->isJpegCompressionEnabled());

        m_copyrect.check(m_pconnectionconfig->isCopyRectAllowed());
        m_viewonly.check(m_pconnectionconfig->isViewOnly());
        m_disclip.check(!m_pconnectionconfig->isClipboardEnabled());
        m_fullscr.check(m_pconnectionconfig->isFullscreenEnabled());
        m_deiconfy.check(m_pconnectionconfig->isDeiconifyOnRemoteBellEnabled());
        m_swapmouse.check(m_pconnectionconfig->isMouseSwapEnabled());

        m_sharedses.check(m_pconnectionconfig->getSharedFlag());
        m_sharedses.enable_window(!m_connected);

        if (m_pconnectionconfig->isFitWindowEnabled()) {
            // FIXME: replace literal to named constant
            m_scale.setSelectedItem(7);
        } else {
            int n = m_pconnectionconfig->getScaleNumerator();
            int d = m_pconnectionconfig->getScaleDenominator();

            int percent = (n * 100) / d;

            ::string text;
            text.format("{}", percent);

            m_scale.setText(text);
        }

        {
            bool enableCursorUpdate = m_pconnectionconfig->isRequestingShapeUpdates();
            bool ignoreCursorUpdate = m_pconnectionconfig->isIgnoringShapeUpdates();
            m_track.check(enableCursorUpdate && !ignoreCursorUpdate);
            m_cursor.check(!enableCursorUpdate && ignoreCursorUpdate);
            m_ncursor.check(enableCursorUpdate && ignoreCursorUpdate);
        }

        ::string labelText;
        {
            const int DEFAULT_COMPRESSION_LEVEL = 6;
            int level = DEFAULT_COMPRESSION_LEVEL;
            if (m_pconnectionconfig->isCustomCompressionEnabled())
                level = m_pconnectionconfig->getCustomCompressionLevel();
            m_tcompLvl.setPos(level);
            labelText.format("{}", level);
            m_quality.setText(labelText);
        }

        {
            const int DEFAULT_JPEG_COMPRESSION_LEVEL = 6;
            int level = DEFAULT_JPEG_COMPRESSION_LEVEL;
            if (m_pconnectionconfig->isJpegCompressionEnabled())
                level = m_pconnectionconfig->getJpegCompressionLevel();
            m_tjpeg.setPos(level);
            labelText.format("{}", level);
            m_quality2.setText(labelText);
        }

        switch (m_pconnectionconfig->getLocalCursorShape()) {
            case ConnectionConfig::SMALL_CURSOR:
                m_smalldot.check(true);
                break;
            case ConnectionConfig::NORMAL_CURSOR:
                m_arrow.check(true);
                break;
            case ConnectionConfig::NO_CURSOR:
                m_nlocal.check(true);
                break;
            default:
                m_dot.check(true);
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
        if (m_viewonly.isChecked()) {
            m_swapmouse.enable_window(false);
        } else {
            m_swapmouse.enable_window(true);
        }
    }

    void OptionsDialog::on8BitColorClick()
    {
        if (!m_eightBit.isChecked()) {
            if (m_jpeg.isChecked()) {
                enableJpegCompression(true);
            }
            m_jpeg.enable_window(true);
        } else {
            m_jpeg.enable_window(false);
            enableJpegCompression(false);
        }
    }

    void OptionsDialog::enableJpegCompression(bool enable)
    {
        m_tjpeg.enable_window(enable);
        m_quality2.enable_window(enable);
        EnableWindow(GetDlgItem(operating_system_window(), IDC_SPOOR), enable);
        EnableWindow(GetDlgItem(operating_system_window(), IDC_SBEST2), enable);
        EnableWindow(GetDlgItem(operating_system_window(), IDC_STQUALITY2), enable);
    }
    void OptionsDialog::onAllowCustomCompressionClick()
    {
        enableCustomCompression(m_compLvl.isChecked());
    }

    void OptionsDialog::enableCustomCompression(bool enable)
    {
        m_tcompLvl.enable_window(enable);
        m_quality.enable_window(enable);
        EnableWindow(GetDlgItem(operating_system_window(), IDC_SBEST), enable);
        EnableWindow(GetDlgItem(operating_system_window(), IDC_SFAST), enable);
        EnableWindow(GetDlgItem(operating_system_window(), IDC_STQUALITY), enable);
    }

    void OptionsDialog::onAllowJpegCompressionClick()
    {
        enableJpegCompression(m_jpeg.isChecked());
    }

    void OptionsDialog::onPreferredEncodingSelectionChange()
    {
        int index = m_useEnc.getSelectedItemIndex();
        if (index < 0) {
            return ;
        }
        int encoding = reinterpret_cast<int>(m_useEnc.getItemData(index));
        switch (encoding) {
            case EncodingDefs::TIGHT:
                enableCustomCompression(m_compLvl.isChecked());
                m_compLvl.enable_window(true);
                break;
            default:
                enableCustomCompression(false);
                m_compLvl.enable_window(false);
                break;
        } // switch
    } // void

    void OptionsDialog::onCustomCompressionLevelScroll()
    {
        ::string labelText;
        labelText.format("{}", m_tcompLvl.getPos());
        m_quality.setText(labelText);
    }

    void OptionsDialog::onJpegCompressionLevelScroll()
    {
        ::string labelText;
        labelText.format("{}", m_tjpeg.getPos());
        m_quality2.setText(labelText);
    }

    void OptionsDialog::onMessageReceived(unsigned int uMsg, ::wparam wParam, ::lparam lParam)
    {
        switch (uMsg) {
            case WM_HSCROLL:
                if (HWND(lParam) == m_tcompLvl.operating_system_window()) {
                    onCustomCompressionLevelScroll();
                }
                if (HWND(lParam) == m_tjpeg.operating_system_window()) {
                    onJpegCompressionLevelScroll();
                }
                break;
        }
    }

    void OptionsDialog::onScaleKillFocus()
    {
        //::string scaleText;
        auto scaleText = m_scale.getText();

        int scale;

        if (!StringParser::parseInt(scaleText, &scale)) {
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
        m_scale.setText(scaleText);
    }

    bool OptionsDialog::isInputValid()
    {
        int scaleInt;
        //::string scaleText;

        auto scaleText = m_scale.getText();

        if (scaleText == "Auto") {
            return true;
        }

        if (!StringParser::parseInt(scaleText, &scaleInt)) {
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
        int pesii = m_useEnc.getSelectedItemIndex();
        if (pesii >= 0) {
            int preferredEncoding = reinterpret_cast<int>(m_useEnc.getItemData(pesii));
            m_pconnectionconfig->setPreferredEncoding(preferredEncoding);
        } else {
            _ASSERT(pesii >= 0);
            m_pconnectionconfig->setPreferredEncoding(EncodingDefs::TIGHT);
        }

        if (m_compLvl.isChecked()) {
            int level = static_cast<int>(m_tcompLvl.getPos());
            m_pconnectionconfig->setCustomCompressionLevel(level);
        } else {
            m_pconnectionconfig->disableCustomCompression();
        }

        if (m_jpeg.isChecked()) {
            int level = static_cast<int>(m_tjpeg.getPos());
            m_pconnectionconfig->setJpegCompressionLevel(level);
        } else {
            m_pconnectionconfig->disableJpegCompression();
        }

        m_pconnectionconfig->use8BitColor(m_eightBit.isChecked());
        m_pconnectionconfig->allowCopyRect(m_copyrect.isChecked());
        m_pconnectionconfig->setViewOnly(m_viewonly.isChecked());
        m_pconnectionconfig->enableClipboard(!m_disclip.isChecked());
        m_pconnectionconfig->enableFullscreen(m_fullscr.isChecked());
        m_pconnectionconfig->deiconifyOnRemoteBell(m_deiconfy.isChecked());
        m_pconnectionconfig->swapMouse(m_swapmouse.isChecked());
        m_pconnectionconfig->setSharedFlag(m_sharedses.isChecked());

        ::string scaleText;

        scaleText = m_scale.getText();

        int scaleInt = 0;

        if (StringParser::parseInt(scaleText, &scaleInt)) {
            m_pconnectionconfig->setScale(scaleInt, 100);
            m_pconnectionconfig->fitWindow(false);
        } else {
            m_pconnectionconfig->fitWindow(true);
        }

        if (m_track.isChecked()) {
            m_pconnectionconfig->requestShapeUpdates(true);
            m_pconnectionconfig->ignoreShapeUpdates(false);
        }

        if (m_cursor.isChecked()) {
            m_pconnectionconfig->requestShapeUpdates(false);
            m_pconnectionconfig->ignoreShapeUpdates(true);
        }

        if (m_ncursor.isChecked()) {
            m_pconnectionconfig->requestShapeUpdates(true);
            m_pconnectionconfig->ignoreShapeUpdates(true);
        }

        int localCursorShape = ConnectionConfig::DOT_CURSOR;
        if (m_smalldot.isChecked()) {
            localCursorShape = ConnectionConfig::SMALL_CURSOR;
        } else if (m_arrow.isChecked()) {
            localCursorShape = ConnectionConfig::NORMAL_CURSOR;
        } else if (m_nlocal.isChecked()) {
            localCursorShape = ConnectionConfig::NO_CURSOR;
        }

        m_pconnectionconfig->setLocalCursorShape(localCursorShape);
    }
}//namespace remoting_remoting
