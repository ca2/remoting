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
#include "remoting/remoting_rfb/rfb/MsgDefs.h"


namespace remoting_rfb
{
   const_char_pointer const Utf8CutTextDefs::CLIENT_CUT_TEXT_UTF8_SIG = "UTF8CUTC";
   const_char_pointer const Utf8CutTextDefs::SERVER_CUT_TEXT_UTF8_SIG = "UTF8CUTS";
   const_char_pointer const Utf8CutTextDefs::ENABLE_CUT_TEXT_UTF8_SIG = "UTF8CUTE";

   const_char_pointer const EchoExtensionDefs::ECHO_REQUEST_SIG = "ECHOCREQ";
   const_char_pointer const EchoExtensionDefs::ECHO_RESPONSE_SIG = "ECHOSRES";
} // namespace remoting_rfb