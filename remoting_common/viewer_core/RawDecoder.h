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


#include "DecoderOfRectangle.h"

class CLASS_DECL_REMOTING_COMMON RawDecoder : public DecoderOfRectangle
{
public:
  RawDecoder(LogWriter *logWriter);
  virtual ~RawDecoder();

  //
  // This method is inherited from DecoderOfRectangle.
  //
  virtual void process(RfbInputGate *input,
                       FrameBuffer *frameBuffer,
                       FrameBuffer *secondFrameBuffer,
                       const ::int_rectangle &  rect,
                       LocalMutex *fbLock,
                       FbUpdateNotifier *fbNotifier);

protected:
  virtual void decode(RfbInputGate *input,
                      FrameBuffer *frameBuffer,
                      const ::int_rectangle &  rect);

private:
  static const size_t AREA_OF_ONE_PART = 1024 * 64;
};


