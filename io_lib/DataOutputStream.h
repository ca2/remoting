// Copyright (C) 2009,2010,2011,2012 GlavSoft LLC.
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

#ifndef _DATA_OUTPUT_STREAM_H_
#define _DATA_OUTPUT_STREAM_H_

#include "util/inttypes.h"
#include "OutputStream.h"
#include "IOException.h"

/**
 * Data output stream (decorator pattern).
 * Adds feature to write typezied data to output stream.
 */
class DataOutputStream : public OutputStream
{
public:
  /**
   * Creates new data output stream.
   * @param outputStream real output stream.
   */
  DataOutputStream(OutputStream *outputStream);
  virtual ~DataOutputStream();

  /**
   * Inherited from superclass.
   * @remark just delegates call to real output stream.
   */
  virtual size_t write(const void *buffer, size_t len);

  /**
   * Writes exacly specified count of bytes to stream.
   * @param buffer source buffer.
   * @param len count of bytes to write.
   * @throws IOException on error.
   * @fixme really it can throw any kind of exception (depends on implementation
   * of output stream passed to costructor of DataOutputStream).
   */
  void writeFully(const void *buffer, size_t len);

  void writeUInt8(unsigned char x);
  void writeUInt16(unsigned short x);
  void writeUInt32(unsigned int x);
  void writeUInt64(unsigned long long x);

  void writeInt8(char x);
  void writeInt16(short x);
  void writeInt32(int x);
  void writeInt64(long long x);

  void writeUTF8(const ::scoped_string & scopedstrstring);

  /**
   * Flushes inner output stream.
   */
  virtual void flush();
protected:
  OutputStream *m_outStream;
};

#endif
