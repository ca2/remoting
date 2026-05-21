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

#pragma once


#include "subsystem/_common_header.h"

namespace remoting_client
{
    class CLASS_DECL_REMOTING ProductNames
    {
    public:
        static ::string_literal PRODUCT_NAME;
        static ::string_literal VIEWER_PRODUCT_NAME;
    };

    class CLASS_DECL_REMOTING RegistryPaths
    {
    public:
        static ::string_literal VIEWER_PATH;
    };

    class CLASS_DECL_REMOTING LogNames
    {
    public:
        static ::string_literal VIEWER_LOG_FILE_STUB_NAME;
        static ::string_literal LOG_DIR_NAME;
    };


    class CLASS_DECL_REMOTING ApplicationNames
    {
    public:
        static ::string_literal WINDOW_CLASS_NAME;
    };

    class CLASS_DECL_REMOTING WindowNames
    {
    public:
        static ::string_literal TVN_WINDOW_CLASS_NAME;
        static ::string_literal TVN_WINDOW_TITLE_NAME;
        static ::string_literal TVN_SUB_WINDOW_TITLE_NAME;
    };
} // namespace remoting_client
