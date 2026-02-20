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


//#include <vector>

#include "remoting/remoting_common/util/Inflater.h"

#include "DecoderOfRectangle.h"
#include "JpegDecompressor.h"

class CLASS_DECL_REMOTING_COMMON TightDecoder : public DecoderOfRectangle
{
public:
  TightDecoder(LogWriter *logWriter);
  virtual ~TightDecoder();

protected:
  virtual void decode(RfbInputGate *input,
                      FrameBuffer *frameBuffer,
                      const ::int_rectangle &  dstRect);

private:
  void reset();
  void resetDecoders(unsigned char compControl);
  unsigned int readTightPixel(RfbInputGate *input, int bytesPerCPixel);
  int readCompactSize(RfbInputGate *input);
  ::array_base<unsigned int> readPalette(RfbInputGate *input,
                          int paletteSize,
                          int bytesPerCPixel);
  void processJpeg(RfbInputGate *input,
                   FrameBuffer *frameBuffer,
                   const ::int_rectangle &  dstRect);
  void processBasicTypes(RfbInputGate *input,
                         FrameBuffer *frameBuffer,
                         const ::int_rectangle &  dstRect,
                         unsigned char compControl);
  void readTightData(RfbInputGate *input,
                     ::array_base<unsigned char> &buffer,
                     size_t expectedLength,
                     const int decoderId);
  void readCompressedData(RfbInputGate *input,
                          ::array_base<unsigned char> &buffer,
                          size_t expectedLength,
                          const int decoderId);
  void drawPalette(FrameBuffer *fb,
                   const ::array_base<unsigned int> &palette,
                   const ::array_base<unsigned char> &pixels,
                   const ::int_rectangle &  dstRect);
  void drawGradient(FrameBuffer *fb,
                    const ::array_base<unsigned char> &pixels,
                    const ::int_rectangle &  dstRect);
  void drawTightBytes(FrameBuffer *fb,
                     const ::array_base<unsigned char> *pixels,
                     const ::int_rectangle &  dstRect);
  void drawJpegBytes(FrameBuffer *fb,
                     const ::array_base<unsigned char> *pixels,
                     const ::int_rectangle &  dstRect);

  unsigned int getRawTightColor(const PixelFormat & pxFormat,
                          const ::array_base<unsigned char> &pixels,
                          size_t offset);
  void fillRawComponents(const PixelFormat & pxFormat,
                         unsigned char components[],
                         const ::array_base<unsigned char> &pixels,
                         size_t pixelOffset);

  unsigned int transformPixelToTight(unsigned int color);
  ::array_base<unsigned char> transformArray(const ::array_base<unsigned char> &buffer);

  ::array_base<Inflater *> m_inflater;
  JpegDecompressor m_jpeg;

  bool m_isCPixel;
private:
  static const int MAX_SUBENCODING = 0x09;
  static const int JPEG_TYPE = 0x09;
  static const int FILL_TYPE = 0x08;

  static const int FILTER_ID_MASK = 0x40;
  // TODO: removed 0x30 constant?
  static const int STREAM_ID_MASK = 0x30;

  static const int COPY_FILTER = 0x00;
  static const int PALETTE_FILTER = 0x01;
  static const int GRADIENT_FILTER = 0x02;

  static const int DECODERS_NUM = 4;

  static const int MIN_SIZE_TO_COMPRESS = 12;
};


