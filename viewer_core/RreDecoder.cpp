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
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//
#include "framework.h"
#include "RreDecoder.h"

RreDecoder::RreDecoder(LogWriter *logWriter)
: DecoderOfRectangle(logWriter)
{
  m_encoding = EncodingDefs::RRE;
}

RreDecoder::~RreDecoder()
{
}

void RreDecoder::decode(RfbInputGate *pinput,
                        FrameBuffer *frameBuffer,
                        const ::int_rectangle &  dstRect)
{
  unsigned int numberRectangle = pinput->readUInt32();
  size_t bytesPerPixel = frameBuffer->getBytesPerPixel();

  unsigned int backgroundColor;
  pinput->readFully(&backgroundColor, bytesPerPixel);
  frameBuffer->fillRect(dstRect, backgroundColor);

  while (numberRectangle--) {
    unsigned int color;
    pinput->readFully(&color, bytesPerPixel);
    unsigned int x = pinput->readUInt16();
    unsigned int y = pinput->readUInt16();
    unsigned int w = pinput->readUInt16();
    unsigned int h = pinput->readUInt16();

    ::int_rectangle rect(x, y, x + w, y + h);
    rect.offset(dstRect.left, dstRect.top);
    frameBuffer->fillRect(rect, color);
  }
}
