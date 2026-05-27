// Copyright (C) 2009,2010,2011,2012 GlavSoft LLC.
// All rights reserved.
//
//-------------------------------------------------------------------------
// This file is part of the T i g h t V N C software.  Please visit our Web site:
//
//                       http://www.t i g h t v n c.com/
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


//#include aaa_<vector>

#include "innate_subsystem/framebuffer/Framebuffer.h"
#include "acme/exception/io.h"
#include "remoting/remoting_rfb/network/RfbOutputGate.h"
#include "remoting/remoting_rfb/rfb/EncodingDefs.h"
#include "EncodeOptions.h"
#include "remoting/remoting_rfb/rfb/PixelConverter.h"

namespace remoting_rfb
{

   //
   // Encoder is the base class for all RFB encoders.
   // It implements Raw encoder which is used in RFB by default.
   //

   class CLASS_DECL_REMOTING_RFB Encoder :
   virtual public ::particle
   {
   //protected:
   public:

      // PixelConverter is used for converting pixels from the given pframebuffer
      // to some other pixel format (typically, the pixel format using by an RFB
      // client). Encoders may assume it will be properly configured at the moment
      // of calling splitRectangle() or sendRectangle(). The same object may not
      // be used by other threads during the execution the mentioned functions of
      // the Encoder, and it may not be altered between a call to splitRectangle()
      // and the corresponding calls to sendRectangle().
      ::pointer < ::remoting_rfb::PixelConverter > m_ppixelconverter;

      // The output stream to write the encoded data to.
      ::pointer < DataOutputStream > m_pdataoutputstream;

      /// private:
      // Do not allow copying objects.
      // Encoder(const Encoder &other);
      // Encoder &operator=(const Encoder &other);

   //public:

      // The constructor.
      // `conv' specifies a PixelConverter allowing encoders to translate pixels
      // from the given pframebuffer to some other pixel format (typically, the
      // pixel format used by an RFB client). The converter must be maintained by
      // its owner so that it would be relevant at the moment of calling
      // splitRectangle() or sendRectangle(). The same object may not be used by
      // other threads during the execution the mentioned functions of the
      // Encoder, and it may not be altered between a call to splitRectangle() and
      // the corresponding calls to sendRectangle().
      // `output' specifies the output stream to write the encoded data to.
      // Both `conv' and 'output' should point to objects that should remain
      // relevant during the whole life cycle of the Encoder.
      Encoder(::remoting_rfb::PixelConverter * ppixelconverter, DataOutputStream * pdataoutputstream);

      virtual ~Encoder(void);

      // Return the "encoding type" code for this encoder, as defined in the RFB
      // protocol specification.
      virtual ::i32 getCode() const;

      // splitRectangle() is used to let the encoder split one big rectangle to a
      // number of smaller ones for efficient encoding or for satisfying the
      // limitations on maximum rectangle width and height. The resulting ::list_base of
      // rectangles is added to the end of the provided `rectList'.
      //
      // The default implementation of this function provided by the base Encoder
      // class does not split rectangles, it just adds the original rectangle to
      // rectList. If a particular encoder wishes to split rectangles differently,
      // it should implement its own splitRectangle() function.
      //
      // UpdateSender calls splitRectangle() for each rectangle of the update
      // region, then calls sendRectangle() for the same ::list_base of rectangles.
      //
      // The arguments of splitRectangle() are similar to those of
      // sendRectangle(). It's guaranteed that options and serverFb will point to
      // the same data as in the subsequent calls to sendRectanle(). Also, it's
      // guaranteed that the state of m_ppixelconverter will not be changed between
      // splitRectangle() and sendRectangle() calls. splitRectangles() may change
      // the state of PixelConverter that's why it cannot be declared const.
      virtual void splitRectangle(const ::i32_rectangle & rectangle, ::int_rectangle_array_base & rectanglea,
                                  const ::innate_subsystem::Framebuffer *serverFb, const EncodeOptions *options);

      // Encode and send the rectangle. The `serverFb' argument points to a frame
      // buffer that can be in arbitrary pixel format natively used by the RFB
      // server. To get relevant pixels in the destination format (client format),
      // encoders must convert the data from *serverFb explicitly, e.g. by calling
      // m_ppixelconverter->convert().
      virtual void sendRectangle(const ::i32_rectangle & rectangle, const ::innate_subsystem::Framebuffer *serverFb,
                                 const EncodeOptions *options);

   };

 

} // namespace remoting_rfb
 