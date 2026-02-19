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

#pragma onceINCLUDED__
INCLUDED__

#include "Encoder.h"
#include "region/Region.h"

class RreEncoder : public Encoder
{
public:
  RreEncoder(PixelConverter *conv, DataOutputStream *output);
  virtual ~RreEncoder();

  virtual int getCode() const;

  virtual void splitRectangle(const ::int_rectangle &  rect,
                              ::array_base<::int_rectangle> *rectList,
                              const FrameBuffer *serverFb,
                              const EncodeOptions *options);

  virtual void sendRectangle(const ::int_rectangle &  rect,
                             const FrameBuffer *serverFb,
                             const EncodeOptions *options);

private:
  template <class PIXEL_T>
    void rreEncode(const ::int_rectangle &  r,
                   const FrameBuffer *frameBuffer);

  // Coordinates of subrectangles.
  ::array_base<::int_rectangle> m_rects;
  
  // All rectangles are devided (in splitRectangle() function)
  // into new rectangles with maximum size == 64.
  // ::int_rectangle size == 64 for a better performance and less memory consumption.
  static const int RECT_SIZE = 64;
};

//// __RFB_RRE_ENCODER_H_INCLUDED__
