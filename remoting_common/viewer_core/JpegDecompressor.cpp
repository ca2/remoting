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
#include "JpegDecompressor.h"

#include "acme/prototype/geometry2d/rectangle.h"
//#include "remoting/remoting_common/util/::string.h"
#include "remoting/remoting_common/util/Exception.h"

JpegDecompressor::JpegDecompressor()
{
  init();
}

JpegDecompressor::~JpegDecompressor()
{
  try {
    cleanup();
  } catch (...) {
  }
}

::string JpegDecompressor::get_message(j_common_ptr cinfo)
{
  char buffer[JMSG_LENGTH_MAX];
  // Create the scopedstrMessage
  (*cinfo->err->format_message) (cinfo, buffer);

  ::string errorAnsi(buffer);
  //::string error;
  //errorAnsi.toStringStorage(&error);
  return errorAnsi;
}

void JpegDecompressor::errorExit(j_common_ptr cinfo)
{
  (*cinfo->err->output_message) (cinfo);
  ::string error = get_message(cinfo);
  jpeg_destroy(cinfo);
  throw ::remoting::Exception(error);
}

void JpegDecompressor::outputMessage(j_common_ptr cinfo)
{
  return;
}

void JpegDecompressor::decompress(::array_base<unsigned char> &buffer,
                                  size_t jpegBufLen,
                                  ::array_base<unsigned char> &pixels,
                                  const ::int_rectangle &  dstRect)
{
  if (dstRect.is_empty())
    throw ::remoting::Exception("invalid destination rectangle in jpeg-decompressor");

  if (buffer.size() == 0 || buffer.size() < jpegBufLen)
    throw ::remoting::Exception("incorrect size of buffer in jpeg-decompressor");
  unsigned char *src_buf = buffer.data();
  size_t src_buf_size = jpegBufLen;

  size_t width = dstRect.width();
  size_t height = dstRect.height();
  size_t pixelBufferCount =  width * height * BYTES_PER_PIXEL;
  if (pixels.size() == 0 || pixels.size() < pixelBufferCount)
    throw ::remoting::Exception("incorrect size of pixels-buffer in jpeg-decompressor");
  unsigned char *dst_buf = pixels.data();

  try {
    /* Initialize data source and read the header. */
    jpeg_mem_src(&m_jpeg.cinfo, src_buf, static_cast<unsigned long>(src_buf_size));
    if (jpeg_read_header(&m_jpeg.cinfo, TRUE) != JPEG_HEADER_OK) {
      throw ::remoting::Exception("possible, bad JPEG header");
    }

    JDIMENSION jpegWidth = dstRect.width();
    JDIMENSION jpegHeight = dstRect.height();
    if (m_jpeg.cinfo.image_width != jpegWidth ||
        m_jpeg.cinfo.image_height != jpegHeight) {
      jpeg_abort_decompress(&m_jpeg.cinfo);
      throw ::remoting::Exception("wrong image size");
    }

    /* Configure and start decompression. */
    m_jpeg.cinfo.out_color_space = JCS_RGB;
    jpeg_start_decompress(&m_jpeg.cinfo);
    if (m_jpeg.cinfo.output_width != jpegWidth ||
        m_jpeg.cinfo.output_height != jpegHeight ) {
      jpeg_abort_decompress(&m_jpeg.cinfo);
      throw ::remoting::Exception("something's wrong with the JPEG library");
    }

    /* Consume decompressed data. */
    while (m_jpeg.cinfo.output_scanline < m_jpeg.cinfo.output_height) {
      size_t bufferIndex = m_jpeg.cinfo.output_scanline;
      size_t bufferOffset = bufferIndex * width * BYTES_PER_PIXEL;

      JSAMPROW row_ptr[1];
      row_ptr[0] = &dst_buf[bufferOffset];
      if (jpeg_read_scanlines(&m_jpeg.cinfo, row_ptr, 1) != 1) {
        jpeg_abort_decompress(&m_jpeg.cinfo);
        throw ::remoting::Exception("error decompressing JPEG data");
      }
    }
    /* Cleanup after the decompression. */
    jpeg_finish_decompress(&m_jpeg.cinfo);
  } catch (const ::remoting::Exception &ex) {
    cleanup();
    init();
    throw ex;
  }
}


void JpegDecompressor::init()
{
  jpeg_create_decompress(&m_jpeg.cinfo);
  
  m_jpeg.cinfo.err = jpeg_std_error(&m_jpeg.jerr);
  m_jpeg.cinfo.err->error_exit = errorExit;
  m_jpeg.cinfo.err->output_message = outputMessage;
}

void JpegDecompressor::cleanup()
{
  jpeg_destroy_decompress(&m_jpeg.cinfo);
}
