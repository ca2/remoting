// Copyright (C) 2010,2011,2012 GlavSoft LLC.
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


#include "subsystem/_common_header.h"

#include "subsystem/node/LocalOperatingSystemApplication.h"


namespace remoting_node_desktop
{
   /**
    * Windows application that used to do additional actions:
    *
    * Solves a couple of problems:
    *  1) Lock workstation (-lockworkstation keys).
    *  2) Log out interactive user (-logout keys).
    *
    * @fixme stub class.
    */
   class CLASS_DECL_REMOTING_NODE_DESKTOP AdditionalActionApplication : public ::subsystem::LocalOperatingSystemApplication
   {
   public:
      /**
       * Key identifying lock workstation action.
       */
      static ::string_literal LOCK_WORKSTATION_KEY;
      /**
       * Key identifying logout action.
       */
      static ::string_literal LOGOUT_KEY;

   public:
      /**
       * Creates new instance.
       * @param hInstance application instance.
       * @param commandLine command line.
       * @throws SystemException on fail(inherited from superclass).
       */
      //AdditionalActionApplication(::hinstance hInstance,
      //                          const ::scoped_string & scopedstrwindowClassName,
      //                        const ::scoped_string & scopedstrCommandLine);
      //AdditionalActionApplication(const ::scoped_string & scopedstrCommandLine);
      AdditionalActionApplication();
      /**
      * Destructor.
      */
      ~AdditionalActionApplication() override;

      void initialize_additional_action_application(const ::scoped_string &scopedstrCommandLine);
      /**
       * Runs application.
       * @return 0 on success, non-zero on error (Windows error code).
       */
      virtual void run();

   private:
      /**
       * Application command line arguments.
       */
      ::string m_strCommandLine;
   };

} // namespace remoting_node_desktop