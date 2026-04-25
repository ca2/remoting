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
#include "framework.h"
#include "FTMessage.h"

::string_literal FTMessage::COMPRESSION_SUPPORT_REQUEST_SIG = "FTCCSRST";
::string_literal FTMessage::COMPRESSION_SUPPORT_REPLY_SIG = "FTSCSRLY";
::string_literal FTMessage::FILE_LIST_REQUEST_SIG = "FTCFLRST";
::string_literal FTMessage::FILE_LIST_REPLY_SIG = "FTSFLRLY";
::string_literal FTMessage::MD5_REQUEST_SIG = "FTCM5RST";
::string_literal FTMessage::MD5_REPLY_SIG = "FTSM5RLY";
::string_literal FTMessage::UPLOAD_START_REQUEST_SIG = "FTCFURST";
::string_literal FTMessage::UPLOAD_START_REPLY_SIG = "FTSFURLY";
::string_literal FTMessage::UPLOAD_DATA_REQUEST_SIG = "FTCUDRST";
::string_literal FTMessage::UPLOAD_DATA_REPLY_SIG = "FTSUDRLY";
::string_literal FTMessage::UPLOAD_END_REQUEST_SIG = "FTCUERST";
::string_literal FTMessage::UPLOAD_END_REPLY_SIG = "FTSUERLY";
::string_literal FTMessage::DOWNLOAD_START_REQUEST_SIG = "FTCFDRST";
::string_literal FTMessage::DOWNLOAD_START_REPLY_SIG = "FTSFDRLY";
::string_literal FTMessage::DOWNLOAD_DATA_REQUEST_SIG = "FTCDDRST";
::string_literal FTMessage::DOWNLOAD_DATA_REPLY_SIG = "FTSDDRLY";
::string_literal FTMessage::DOWNLOAD_END_REPLY_SIG = "FTSDERLY";
::string_literal FTMessage::MKDIR_REQUEST_SIG = "FTCMDRST";
::string_literal FTMessage::MKDIR_REPLY_SIG = "FTSMDRLY";
::string_literal FTMessage::REMOVE_REQUEST_SIG = "FTCFRRST";
::string_literal FTMessage::REMOVE_REPLY_SIG = "FTSFTRLY";
::string_literal FTMessage::RENAME_REQUEST_SIG = "FTCFMRST";
::string_literal FTMessage::RENAME_REPLY_SIG = "FTSFMRLY";
::string_literal FTMessage::DIRSIZE_REQUEST_SIG = "FTCDSRST";
::string_literal FTMessage::DIRSIZE_REPLY_SIG = "FTSDSRLY";
::string_literal FTMessage::LAST_REQUEST_FAILED_REPLY_SIG = "FTLRFRLY";
