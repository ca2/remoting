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

#pragma once


#include "subsystem/platform/CommandLine.h"
#include "subsystem/platform/Exception.h"


namespace remoting_rfb_node_desktop
{
   /**
    * Service control application command line parser.
    * Supported keys are:
    *  -install, -reinstall, -remove, -start, -stop.
    * @remark only one allowed key can be specified.
    * @fixme no test for this class.
    */
   class CLASS_DECL_REMOTING_NODE_DESKTOP ServiceControlCommandLine :
      virtual public ::subsystem::CommandLine
   {
   public:
      static string_literal APPLICATION_KEY;
      static string_literal INSTALL_SERVICE;
      static string_literal REMOVE_SERVICE;
      static string_literal REINSTALL_SERVICE;
      static string_literal START_SERVICE;
      static string_literal STOP_SERVICE;
      static string_literal SILENT;
      static string_literal DONT_ELEVATE;

   public:
      /**
       * Default constructor.
       */
      ServiceControlCommandLine();
      /**
       * Destructor.
       */
      virtual ~ServiceControlCommandLine();

      /**
       * Parses command line.
       * @param commandLine command line string to parse.
       * @throws ::subsystem::Exception on parse error.
       */
      void parse(const ::subsystem::CommandLineArguments *cmdArgs);

      /**
       * Checks if key is specified.
       * @param key key to check.
       * @return true if key is specified, false otherwise.
       * @remark must be called only after call of parse method.
       */
      bool keySpecified(const ::scoped_string & scopedstrKey) const;

      bool installationRequested() const;
      bool removalRequested() const;
      bool reinstallRequested() const;
      bool startRequested() const;
      bool stopRequested() const;

      bool beSilent() const;
      bool dontElevate() const;
   };


} // namespace remoting_rfb_node_desktop




