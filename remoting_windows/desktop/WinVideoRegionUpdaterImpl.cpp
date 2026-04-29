// Copyright (C) 2013 GlavSoft LLC.
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
#include "remoting/remoting_windows/desktop/WinVideoRegionUpdaterImpl.h"
#include "remoting/remoting/node_config/Configurator.h"
#include "innate_subsystem/gui/WindowFinder.h"
#include "remoting/remoting/region/RectSerializer.h"
//#include "subsystem/thread/lockable_critical_section.h"


namespace remoting_windows
{


   //WinVideoRegionUpdaterImpl::WinVideoRegionUpdaterImpl(::subsystem::LogWriter * plogwriter)
   WinVideoRegionUpdaterImpl::WinVideoRegionUpdaterImpl()
   {
      //: m_plogwriter(plogwriter)
      //{
        // resume();


   }

   WinVideoRegionUpdaterImpl::~WinVideoRegionUpdaterImpl()
   {
      terminate();
      wait();
   }


   void WinVideoRegionUpdaterImpl::initialize_screen_driver(::remoting_node::Configurator * pconfigurator, ::remoting::UpdateKeeper * pupdatekeeper, ::remoting::UpdateListener * pupdatelistener,
                                        ::innate_subsystem::Framebuffer *pframebuffer,
                          lockable_critical_section *pcriticalsectionFramebuffer, ::subsystem::LogWriter * plogwriter)
   {
      m_pconfigurator = pconfigurator;
      m_plogwriter = plogwriter;

      resume();

   }


   void WinVideoRegionUpdaterImpl::onTerminate() { m_happeningSleeper.set_happening(); }

   void WinVideoRegionUpdaterImpl::execute()
   {
      while (!isTerminating())
      {
         m_happeningSleeper.wait(getInterval() * 1_ms);
         if (!isTerminating())
         {
            try
            {
               updateVideoRegion();
            }
            catch (...)
            {
            }
         }
      }
   }

   unsigned int WinVideoRegionUpdaterImpl::getInterval()
   {
      ::remoting_node::ServerConfig *pserverconfig = m_pconfigurator->getServerConfig();
      return pserverconfig->getVideoRecognitionInterval();
   }

   ::remoting::Region WinVideoRegionUpdaterImpl::getVideoRegion()
   {
      critical_section_lock al(&m_criticalsectionRegion);
      return m_regionVideo;
   }

   void WinVideoRegionUpdaterImpl::getClassNamesAndRectsFromConfig(::string_array &classNames,
                                                                   ::int_rectangle_array_base &rectanglea)
   {
      ::remoting_node::ServerConfig *pserverconfig = m_pconfigurator->getServerConfig();
      AutoLock al(pserverconfig);
      classNames = *pserverconfig->getVideoClassNames();
      rectanglea = *pserverconfig->getVideoRects();
   }

   void WinVideoRegionUpdaterImpl::updateVideoRegion()
   {
      ::string_array classNames;
      ::int_rectangle_array_base rectanglea;
      getClassNamesAndRectsFromConfig(classNames, rectanglea);
      ::remoting::Region tmpRegion;
      m_plogwriter->debug("WinVideoRegionUpdaterImpl: ClassNames {}, Rects {}", classNames.size(),
                          m_regionVideo.getCount());
      if (!classNames.empty())
      {
         auto timeStart = ::time::now();
         tmpRegion.add(getRectsByClass(classNames));
         unsigned int millis = timeStart.elapsed().integral_millisecond();
         m_plogwriter->debug("WinVideoRegionUpdaterImpl::getRectsByClass call took {} ms", millis);
      }
      if (!rectanglea.empty())
      {
         tmpRegion.add(getRectsByCoords(rectanglea));
      }
      m_plogwriter->debug("WinVideoRegionUpdaterImpl: copy data");
      {
         critical_section_lock al(&m_criticalsectionRegion);
         m_regionVideo = tmpRegion;
      }
      m_plogwriter->debug("WinVideoRegionUpdaterImpl: exit updateVideoRegion()");
   }

   ::remoting::Region WinVideoRegionUpdaterImpl::getRectsByClass(::string_array classNames)
   {
      //::array_base<HWND> hwndVector;
      //::array_base<HWND>::iterator hwndIter;
      ::remoting::Region vidRegion;

      for (int i = 0; i < classNames.size(); ++i)
      {
         m_plogwriter->debug("WinVideoRegionUpdaterImpl: getRectsByClass : classname: {} ", classNames[i]);
      }

      auto operatingsystemwindowa = ::windows::findWindowsByClass(classNames);

      m_plogwriter->debug("WinVideoRegionUpdaterImpl: getRectsByClass : %u windows found", operatingsystemwindowa.size());

      //for (hwndIter = hwndVector.begin(); hwndIter != hwndVector.end(); hwndIter++)
      for (auto operatingsystemwindowVideo : operatingsystemwindowa)
      {
         //HWND videoHWND = *hwndIter;
         if (operatingsystemwindowVideo.is_set())
         {
            HWND hwndVideo = ::as_HWND(operatingsystemwindowVideo);
            WINDOWINFO wi;
            wi.cbSize = sizeof(WINDOWINFO);
            if (GetWindowInfo(hwndVideo, &wi))
            {
               ::int_rectangle rectangleVideo(wi.rcClient.left, wi.rcClient.top, wi.rcClient.right, wi.rcClient.bottom);
               if (rectangleVideo.is_set())
               {
                  rectangleVideo.offset(-GetSystemMetrics(SM_XVIRTUALSCREEN), -GetSystemMetrics(SM_YVIRTUALSCREEN));
                  vidRegion.addRect(rectangleVideo);
               }
            }
         }
      }
      return vidRegion;
   }

   ::remoting::Region WinVideoRegionUpdaterImpl::getRectsByCoords(::int_rectangle_array_base &rectanglea)
   {
      ::int_rectangle_array_base::iterator rIter;
      ::int_rectangle rectangleVideo;
      ::remoting::Region vidRegion;
      for (rIter = rectanglea.begin(); rIter != rectanglea.end(); rIter++)
      {
         rectangleVideo = *rIter;
         if (rectangleVideo.is_set())
         {
            vidRegion.addRect(rectangleVideo);
         }
      }
      return vidRegion;
   }


} // namespace remoting_windows
