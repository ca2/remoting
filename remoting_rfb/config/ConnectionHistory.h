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


//#include "subsystem/platform/RegistryKey.h"

//#include aaa_<vector>

namespace remoting_rfb
{
   /**
    * History of tcp connections.
    *
    * Solves problem with saving and restoring history of connections
    * for user (UI).
    *
    */
   class CLASS_DECL_REMOTING_RFB ConnectionHistory
   {
   public:
      /**
       * Creates new empty connection history.
       * @param key registry key identifing root storage for connection history.
       * @param limit maximum count of connection history entries.
       */
      //ConnectionHistory(RegistryKey *key, size_t limit);
      ConnectionHistory();
      /**
       * Destructor.
       */
      virtual ~ConnectionHistory();

      virtual void initialize_connection_history(const ::scoped_string & scopedstr, size_t limit);

      /**
       * Sets connection history entries limit and truncates history
       * if needed.
       */
      void setLimit(size_t limit);

      /**
       * Returns connection history entries limit.
       */
      size_t getLimit() const;

      /**
       * Loads connection history from registry.
       */
      void load();

      /**
       * Saves connection history to registry.
       */
      void save();

      /**
       * Truncates connection history (memory and registry both) to max limit.
       */
      void truncate();

      /**
       * Clears connection history and removed it from registry.
       */
      void clear();

      /**
       * Adds host to connection history.
       * @param scopedstrHost host to add to history.
       * @remark: if host is already in connection history,
         then it's just moves it up to first pointPosition.
       */
      void addHost(const ::scoped_string & scopedstrHost);

      /**
       * Return hosts count.
       * @return hosts count.
       */
      size_t getHostCount() const;

      /**
       * Return host with specified index.
       * @param i index of history entry.
       * @return host string.
       */
      ::string getHost(size_t i) const;

      //protected:
      void releaseHosts();
      void removeHost(const ::scoped_string & scopedstrHost);

      //protected:
      /**
       * Array of history enries.
       */
      ::string_array m_hosts;

      /**
       * Maximum count of history entries.
       */
      size_t m_limit;

      /**
       * Registry key for saving and loading connection history data.
       */
      ::pointer < ::subsystem::RegistryKey > m_pregistrykey;
   };
} // namespace remoting_rfb
