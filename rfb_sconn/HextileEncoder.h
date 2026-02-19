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
// with this program; if not, w_rite to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//

#pragma onceINCLUDED__
INCLUDED__

#include "Encoder.h"

class HextileEncoder : public Encoder
{
public:
  HextileEncoder(PixelConverter *conv, DataOutputStream *output);
  virtual ~HextileEncoder();

  virtual int getCode() const;

  virtual void sendRectangle(const ::int_rectangle &  rect,
                             const FrameBuffer *serverFb,
                             const EncodeOptions *options);

private:
  template <class PIXEL_T>
    void hextileFunction(const ::int_rectangle &r,
                         const FrameBuffer *frameBuffer);
};

//// __RFB_HEXTILE_ENCODER_H_INCLUDED__
