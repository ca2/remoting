// Copyright (C) 2010,2011,2012 GlavSoft LLC.
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
#include "framework.h"
#include "EncoderStore.h"

#include "RreEncoder.h"
#include "HextileEncoder.h"
#include "ZrleEncoder.h"
#include "TightEncoder.h"


namespace remoting_rfb
{


   // EncoderStore::EncoderStore(PixelConverter * ppixelconverter, DataOutputStream * pdataoutputstream)
   // : m_pencoder(0),
   //   m_pjpegencoder(0),
   // {
   // }
   EncoderStore::EncoderStore()
   {
   }

   EncoderStore::~EncoderStore()
   {
      // Remove JpegEncoder which is not in m_map.
      //if (m_pjpegencoder != 0) {
        // delete m_pjpegencoder;
      //}
      m_pjpegencoder.defer_destroy_and_release();
      // Remove all allocated encoders referenced in m_map.
      //::map<::i32, Encoder *>::iterator it;
      //for (it = m_map.begin(); it != m_map.end(); it++) {
        // delete it->second;
      //}
      m_map.defer_destroy_and_release();
   }

   void EncoderStore::initialize_encoder_store(::remoting_rfb::PixelConverter * ppixelconverter, DataOutputStream * pdataoutputstream)
   {

      m_ppixelconverter = ppixelconverter;
      m_pdataoutputstream = pdataoutputstream;

   }

   Encoder *EncoderStore::getEncoder() const
   {
      return m_pencoder;
   }

   JpegEncoder *EncoderStore::getJpegEncoder() const
   {
      return m_pjpegencoder;
   }

   void EncoderStore::selectEncoder(::i32 encType)
   {
      m_pencoder = validateEncoder(encType);
   }

   void EncoderStore::validateJpegEncoder()
   {
      if (m_pjpegencoder == 0) {
         TightEncoder *tight = (TightEncoder *)validateEncoder(EncodingDefs::TIGHT);
         m_pjpegencoder = new JpegEncoder(tight);
      }
   }

   //---------------------------- Internal methods ----------------------------//

   Encoder *EncoderStore::validateEncoder(::i32 encType)
   {
      // Use Raw encoding instead of unknown codes.
      if (!encodingSupported(encType)) {
         encType = EncodingDefs::RAW;
      }
      // If that encoder is already allocated, return a pointer to it.
      auto & pencoder = m_map[encType];
      if (pencoder) {
         return pencoder;
      }
      // Otherwise, allocate it, store it in m_map and return the pointer to it.
      Encoder *newEncoder = allocateEncoder(encType);
      try {
         pencoder = newEncoder;
      } catch (...) {
         //delete newEncoder;
         pencoder.release();
         throw;
      }
      return pencoder;
   }

   // FIXME: Instead of the two functions below, create a separate EncoderFactory
   //        which would know if a given encoding code is supported and would be
   //        able to create encoder instances corresponding to the given codes.
   //        Such EncoderFactory would be useful for EncodeOptions as well.

   bool EncoderStore::encodingSupported(::i32 encType)
   {
      return (encType == EncodingDefs::RAW ||
              encType == EncodingDefs::RRE ||
              encType == EncodingDefs::HEXTILE ||
              encType == EncodingDefs::ZRLE ||
              encType == EncodingDefs::TIGHT);
   }

   Encoder *EncoderStore::allocateEncoder(::i32 encType) const
   {
      switch (encType) {
         case EncodingDefs::TIGHT:
            return new TightEncoder(m_ppixelconverter, m_pdataoutputstream);
         case EncodingDefs::ZRLE:
            return new ZrleEncoder(m_ppixelconverter, m_pdataoutputstream);
         case EncodingDefs::HEXTILE:
            return new HextileEncoder(m_ppixelconverter, m_pdataoutputstream);
         case EncodingDefs::RRE:
            return new RreEncoder(m_ppixelconverter, m_pdataoutputstream);
         case EncodingDefs::RAW:
            return new Encoder(m_ppixelconverter, m_pdataoutputstream);
         default:
            throw ::subsystem::Exception("Cannot create encoder of the specified type");
      }
   }
} // namespace remoting_rfb


