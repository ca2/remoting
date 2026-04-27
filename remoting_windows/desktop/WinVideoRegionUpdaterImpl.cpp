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
#include "remoting/remoting/server_config/Configurator.h"
#include "innate_subsystem/gui/WindowFinder.h"
#include "remoting/remoting/region/RectSerializer.h"
//#include "subsystem/thread/lockable_critical_section.h"


namespace remoting
{


   WinVideoRegionUpdaterImpl::WinVideoRegionUpdaterImpl(::subsystem::LogWriter * plogwriter)
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


   void WinVideoRegionUpdaterImpl::initialize_screen_driver(::subsystem::LogWriter * plogwriter)
   {

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
      ServerConfig *pserverconfig = m_pconfigurator->getServerConfig();
      return pserverconfig->getVideoRecognitionInterval();
   }

   Region WinVideoRegionUpdaterImpl::getVideoRegion()
   {
      critical_section_lock al(&m_criticalsectionRegion);
      return m_regionVideo;
   }

   void WinVideoRegionUpdaterImpl::getClassNamesAndRectsFromConfig(::string_array &classNames,
                                                                   ::int_rectangle_array_base &rectanglea)
   {
      ServerConfig *pserverconfig = m_pconfigurator->getServerConfig();
      critical_section_lock al(pserverconfig);
      classNames = *pserverconfig->getVideoClassNames();
      rectanglea = *pserverconfig->getVideoRects();
   }

   void WinVideoRegionUpdaterImpl::updateVideoRegion()
   {
      ::string_array classNames;
      ::int_rectangle_array_base rectanglea;
      getClassNamesAndRectsFromConfig(classNames, rectanglea);
      Region tmpRegion;
      m_plogwriter->debug(L"WinVideoRegionUpdaterImpl: ClassNames {}, Rects {}", classNames.size(),
                          m_regionVideo.getCount());
      if (!classNames.empty())
      {
         class ::time startTime = class ::time::now();
         tmpRegion.add(getRectsByClass(classNames));
         unsigned int millis = (class ::time::now() - startTime).getTime();
         m_plogwriter->debug(L"WinVideoRegionUpdaterImpl::getRectsByClass call took {} ms", millis);
      }
      if (!rectanglea.empty())
      {
         tmpRegion.add(getRectsByCoords(rectanglea));
      }
      m_plogwriter->debug(L"WinVideoRegionUpdaterImpl: copy data");
      {
         critical_section_lock al(&m_criticalsectionRegion);
         m_regionVideo = tmpRegion;
      }
      m_plogwriter->debug(L"WinVideoRegionUpdaterImpl: exit updateVideoRegion()");
   }

   Region WinVideoRegionUpdaterImpl::getRectsByClass(::string_array classNames)
   {
      ::array_base<HWND> hwndVector;
      ::array_base<HWND>::iterator hwndIter;
      Region vidRegion;

      for (int i = 0; i < classNames.size(); ++i)
      {
         m_plogwriter->debug(L"WinVideoRegionUpdaterImpl: getRectsByClass : classname: {} ", classNames[i]);
      }
      hwndVector = WindowFinder::findWindowsByClass(classNames);

      m_plogwriter->debug(L"WinVideoRegionUpdaterImpl: getRectsByClass : %u windows found", hwndVector.size());

      for (hwndIter = hwndVector.begin(); hwndIter != hwndVector.end(); hwndIter++)
      {
         HWND videoHWND = *hwndIter;
         if (videoHWND != 0)
         {
            WINDOWINFO wi;
            wi.cbSize = sizeof(WINDOWINFO);
            if (GetWindowInfo(videoHWND, &wi))
            {
               ::int_rectangle videoRect(wi.rcClient.left, wi.rcClient.top, wi.rcClient.right, wi.rcClient.bottom);
               if (videoRect.isValid())
               {
                  videoRect.move(-GetSystemMetrics(SM_XVIRTUALSCREEN), -GetSystemMetrics(SM_YVIRTUALSCREEN));
                  vidRegion.addRect(&videoRect);
               }
            }
         }
      }
      return vidRegion;
   }

   Region WinVideoRegionUpdaterImpl::getRectsByCoords(::int_rectangle_array_base &rectanglea)
   {
      ::int_rectangle_array_base::iterator rIter;
      ::int_rectangle videoRect;
      Region vidRegion;
      for (rIter = rectanglea.begin(); rIter != rectanglea.end(); rIter++)
      {
         videoRect = *rIter;
         if (videoRect.isValid())
         {
            vidRegion.addRect(&videoRect);
         }
      }
      return vidRegion;
   }


} // namespace remoting
