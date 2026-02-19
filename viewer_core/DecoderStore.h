// Copyright (C) 2011,2012 GlavSoft LLC.
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


//#include <map>
//#include <vector>

#include "Decoder.h"

//#include "log_writer/LogWriter.h"

class DecoderStore
{
public:
  DecoderStore(LogWriter *logWriter);
  ~DecoderStore();

  Decoder *getDecoder(int decoderId);
  ::array_base<int> getDecoderIds();
  
  // return true, if adding is complete
  // return false, if decoder already exist
  bool addDecoder(Decoder *newDecoder, int priority);
  // return true, if deleting is complete
  // return false, if decoder not exist
  bool removeDecoder(int decoderId);

  void setPreferredEncoding(int encodingType);
  void allowCopyRect(bool allow);

private:
  LogWriter *m_logWriter;

  ::map<int, ::pair<int, Decoder*> > m_decoders;
  int m_preferredEncoding;
  bool m_allowCopyRect;
};


