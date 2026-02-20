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
#include "framework.h"
#include "RawDecoder.h"

#include <algorithm>

RawDecoder::RawDecoder(LogWriter *logWriter)
: DecoderOfRectangle(logWriter)
{
  m_encoding = EncodingDefs::RAW;
}

RawDecoder::~RawDecoder()
{
}

void RawDecoder::process(RfbInputGate *input,
                         FrameBuffer *frameBuffer,
                         FrameBuffer *secondFrameBuffer,
                         const ::int_rectangle &  rect,
                         LocalMutex *fbLock,
                         FbUpdateNotifier *fbNotifier)
{
  // If area of rectangle is 0, then exit from process: nothing update.
  if (rect.area() == 0) {
    return;
  }

  int width = rect.width();
  int height = rect.height();

  // Division with round to up.
  int deltaHeight = (AREA_OF_ONE_PART + width - 1) / width;

  ::int_rectangle deltaRect(0, 0, width, deltaHeight);
  deltaRect.set_top_left(rect.left, rect.top);

  // Process all rectangle without last part of rectangle or 
  // two last part, if area of last part is less half of AREA_OF_ONE_PART.
  while (deltaRect.bottom + deltaHeight / 2 < rect.bottom) {
    DecoderOfRectangle::process(input,
                                frameBuffer, secondFrameBuffer, deltaRect, fbLock,
                                fbNotifier);

    // Increment position of rectangle.
    deltaRect.offset(0, deltaHeight);
  }

  // And process remainder parts of rectangle.
  deltaRect.top = ::maximum(rect.top, deltaRect.bottom - deltaHeight);
  deltaRect.bottom = rect.bottom;
  DecoderOfRectangle::process(input,
                              frameBuffer, secondFrameBuffer, deltaRect, fbLock,
                              fbNotifier);
}

void RawDecoder::decode(RfbInputGate *pinput,
                     FrameBuffer *frameBuffer,
                     const ::int_rectangle &  rect)
{
  size_t bytesPerPixel = frameBuffer->getPixelFormat().bitsPerPixel / 8;
  size_t bytesPerLine = bytesPerPixel * rect.width();

  if (::int_rectangle(frameBuffer->getDimension()).intersection(rect) != rect)
    throw ::remoting::Exception("Error in protocol: incorrect size of rectangle");
  for (int y = rect.top; y < rect.bottom; y++)
    pinput->readFully(frameBuffer->getBufferPtr(rect.left, y), bytesPerLine);
}
