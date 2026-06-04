// Copyright (C) 2010,2011,2012 GlavSoft LLC.
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

#pragma once


#include "RfbKeySymListener.h"
#include "subsystem/platform/Keymap.h"
#include "innate_subsystem/gui/KeyboardLayout.h"
#include "apex/user/user/key_happening.h"
//#include "log_writer/LogWriter.h"

namespace remoting_rfb
{
   // Translates pressed key to a series of rfb symbols. Gives the series
   // to out by the listener function serial calling.
   class CLASS_DECL_REMOTING_RFB RfbKeySym
   {
   public:
      RfbKeySym(RfbKeySymListener *extKeySymListener, ::subsystem::LogWriter * plogwriter);
      virtual ~RfbKeySym();

      // This function doesn't distinguish between left and right modifiers.
      void sendModifier(const ::user::e_key & ekeyModifier, bool down);

      //void processKeyEvent(::user::enum_key ekey, ::u32 addKeyData);
      void processKeyHappening(::user::key_happening ekeyhappening);
      bool vkCodeToString(const ::user::e_key & ekey, bool isKeyDown, ::wstring *res);
      void processCharEvent(::i32 charCode, ::u32 addKeyData);
      // Checks a new modifiers state after focus restoration and sends difference
      void processFocusRestoration();
      // This function release all modifiers unconditionally.
      void processFocusLoss();

      void sendCtrlAltDel();

      // Set function for m_winKeyIgnore.
      void setWinKeyIgnore(bool winIgnore);

   //private:

      // This function does distinguish between a right or left modifier and
      // if the virtKey is a modifier (e.g VK_CONTROL), the function always return
      // a left- or right-hand virtual key value (e.g. ::user::e_key_left_control or ::user::e_key_right_control)
      // by taking into account the isRightHint flag.
      // If virtKey is not a modifier, the function returns virtKey value without
      // changing.
      ::user::e_key distinguishLeftRightModifier(const ::user::e_key & ekeyModifier,
                                                 bool isRightHint);

      // Checks virtKey state with the server side state and sends difference
      void checkAndSendDiff(const ::user::e_key & ekey, ::u8 state);

      // Send one key event (Alt translated to Meta if Scroll Lock is on).
      virtual void sendKeySymEvent(::u32 rfbKeySym, bool down);

      // Send one key event (Alt not translated to Meta).
      virtual void sendVerbatimKeySymEvent(::u32 rfbKeySym, bool down);

      // helper functions
      //::i32 GettingCharFromCtrlSymbol(::i32 ctrlSymbol);
      // E.g if pressed Ctrl + Alt + A
      // Try found ::i8 without modificators
      // #ifdef WINDOWS
      //   bool TryTranslateNotPrintableToUnicode(::u16 virtKey, HKL currentLayout, WCHAR *unicodeChar);
      //
      // #endif
      void releaseMeta();
      void restoreMeta();
      void releaseModifier(const ::user::e_key & ekeyModifier);
      void restoreModifier(const ::user::e_key & ekeyModifier);
      void releaseModifiers();
      void restoreModifiers();


      RfbKeySymListener *m_extKeySymListener;
      ::subsystem::Keymap m_keyMap;

      ::innate_subsystem::keyboard_state_t m_keyboardstate;

      ::pointer < ::subsystem::LogWriter > m_plogwriter;

      // Flag for ignoring win key.
      bool m_winKeyIgnore;
   };

   //// __RFBKEYSYM_H__
} // namespace remoting_rfb
