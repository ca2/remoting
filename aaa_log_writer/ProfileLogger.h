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


//#include "util/::earth::time.h"
//#include <vector>
//#include <map>
#include "thread/LocalMutex.h"


struct ProcessorTimes {
  double process;
  double kernel;
  ULONG64 cycle;
  ::earth::time wall;
};

// �lass for acquiring processor load metrics.
class ProfileLogWriter
{
public:
  ProfileLogWriter(double rate = 5.)
  : m_dropRate(rate) 
  {
    m_lastDrop = ::earth::time::now();
  };

  ~ProfileLogWriter();
  // returns cycles and times deltas from previouse checkpoint
  ProcessorTimes checkPoint(const ::scoped_string & scopedstrTag);
  ::array_base<::array_base<TCHAR>> dropStat();

private:
  LocalMutex m_mapMut;
  //::map<const ::scoped_string & scopedstr, ::array_base<ProcessorTimes>> m_checkPoints;
   ::string_map <::array_base<ProcessorTimes> > m_checkPoints;
  ProcessorTimes m_last;
  double m_dropRate; // time interval in seconds to log statistics
  ::earth::time m_lastDrop;

};

//// __PROFILELogWriter_H__
