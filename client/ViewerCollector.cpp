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
#include "ViewerCollector.h"
#include "ViewerInstance.h"
//#include "subsystem/thread/lockable_critical_section.h"

namespace remoting_client
{
    ViewerCollector::ViewerCollector()
    : m_countToReconnect(0)
    {
    }

    ViewerCollector::~ViewerCollector()
    {
        destroyAllInstances();
    }

    void ViewerCollector::addInstance(ViewerInstance *viewerInstance)
    {
        critical_section_lock l(&m_criticalsection);
        m_viewerinstancelist.add(viewerInstance);
    }

    void ViewerCollector::deleteDeadInstances()
    {
        critical_section_lock l(&m_criticalsection);

        auto iter = m_viewerinstancelist.begin();
        while (iter != m_viewerinstancelist.end()) {
            auto it = iter;
            iter++;
            auto pinstance = *it;

            if (pinstance->isStopped())
            {
               if (pinstance->requiresReconnect())
               {
                    ++m_countToReconnect;
                }
                //delete instance;
                m_viewerinstancelist.erase(it);
            } //else {
            //iter++;
            //}
        }
    }

    void ViewerCollector::destroyAllInstances()
    {
        critical_section_lock l(&m_criticalsection);

        //InstanceList::iterator iter;
        for (auto iter = m_viewerinstancelist.begin(); iter != m_viewerinstancelist.end(); iter++) {
            (*iter)->stop();
        }
        for (auto iter = m_viewerinstancelist.begin(); iter != m_viewerinstancelist.end(); iter++) {
            (*iter)->waitViewer();
        }

        deleteDeadInstances();
    }

    void ViewerCollector::decreaseToReconnect()
    {
        critical_section_lock l(&m_criticalsection);

        --m_countToReconnect;
    }

    bool ViewerCollector::empty() const
    {
        critical_section_lock l(&m_criticalsection);

        // If not active instance and count wait to reconnect is 0,
        // then return true and false otherwise.
        return m_viewerinstancelist.empty() && (m_countToReconnect == 0);
    }
} // namespace remoting_client
