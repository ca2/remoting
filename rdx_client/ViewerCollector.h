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


//#include "subsystem/thread/lockable_critical_section.h"
//#include aaa_<list>
//#include "ViewerInstance.h"


namespace remoting_client
{

    class ViewerInstance;

    using ViewerInstanceList = ::list_base<::pointer < ViewerInstance > > ;

    // Collector instances.
    class ViewerCollector :
        virtual public ::particle
    {
    public:

       mutable lockable_critical_section m_criticalsection;
       ViewerInstanceList m_viewerinstancelist;

       // This variable contain count of instance, when need to reconnect.
       // If this count isn't 0, shutdown application is denied.
       int m_countToReconnect;


        ViewerCollector();
        ~ViewerCollector() override;

        // Adds instance to a self ::list_base.
        void addInstance(ViewerInstance *instance);

        // Forces terminates all instances, waits until they dies and than
        // delete them from memory and thread ::list_base.
        void destroyAllInstances();

        // This function decrease counter of connection, requires reconnect.
        void decreaseToReconnect();

        // Return true, if and only if m_instances is empty()
        bool empty() const;

        // Deletes all stopped instances from memory and removes them from self ::list_base.
        void deleteDeadInstances();

    };


} // namespace remoting_client



