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
#include "CopyRectDecoder.h"

CopyRectDecoder::CopyRectDecoder(::subsystem::LogWriter * plogwriter)
: DecoderOfRectangle(logWriter)
{
  m_encoding = EncodingDefs::COPYRECT;
}

CopyRectDecoder::~CopyRectDecoder()
{
}

void CopyRectDecoder::decode(RfbInputGate *pinput,
                             ::subsystem::FrameBuffer *frameBuffer,
                             const ::int_rectangle &  dstRect)
{
  m_sourcePosition.x = pinput->readInt16();
  m_sourcePosition.y = pinput->readInt16();
}

void CopyRectDecoder::copy(::subsystem::FrameBuffer *dstFrameBuffer,
                           const ::subsystem::FrameBuffer *srcFrameBuffer,
                           const ::int_rectangle &  rect,
                           critical_section *fbLock)
{
  critical_section_lock al(fbLock);
  dstFrameBuffer->move(rect, m_sourcePosition.x, m_sourcePosition.y);
}
