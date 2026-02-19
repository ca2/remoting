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
#include <algorithm>
#include <functional>

#include "DecoderStore.h"

DecoderStore::DecoderStore(LogWriter *logWriter)
: m_logWriter(logWriter),
  m_preferredEncoding(EncodingDefs::TIGHT),
  m_allowCopyRect(true)
{
}

DecoderStore::~DecoderStore()
{
  try {
    for (::map<int, ::pair<int, Decoder *> >::iterator i = m_decoders.begin();
         i != m_decoders.end();
         i++) {
      m_logWriter->debug("Decoder '{}' destroyed", i->m_element2.m_element2->getCode());
      try {
        delete i->m_element2.m_element2;
      } catch (...) {
      }
    }
  } catch (...) {
  }
}

Decoder *DecoderStore::getDecoder(int decoderId)
{
  if (m_decoders.count(decoderId))
    return m_decoders[decoderId].m_element2;
  else
    return 0;
}

::array_base<int> DecoderStore::getDecoderIds()
{
  // this method returned ::list_base of decoders, sorted by priority.
  // in first position is preffered encoding.
  ::array_base<::pair<int, int> > decoders;

  for (::map<int, ::pair <int, Decoder *> >::iterator i = m_decoders.begin();
       i != m_decoders.end();
       i++) {
    // preferred encoding is skipping
    if (i->m_element1 != m_preferredEncoding) {
      // copy rect is allowed?
      if (i->m_element1 != EncodingDefs::COPYRECT || m_allowCopyRect)
        decoders.add({i->m_element2.m_element1, i->m_element1});
    }
  }
  decoders.predicate_sort([]( auto& pair1, auto& pair2)
       {
          auto order =pair1.m_element1 <=> pair2.m_element1;

     if (order != ::std::strong_ordering::equivalent)
     {

        return order > 0;

     }
     return (pair1.m_element2 <=> pair2.m_element2) > 0;

       });
  ::array_base<int> sortedDecoders;
  auto priorityEnc = m_decoders.find(m_preferredEncoding);
  if (priorityEnc != m_decoders.end())
    sortedDecoders.add(priorityEnc->m_element1);
  for (::array_base<::pair<int, int> >::iterator i = decoders.begin();
       i != decoders.end();
       i++) {
    sortedDecoders.add(i->m_element2);
  }
  if (sortedDecoders.empty())
    sortedDecoders.add(EncodingDefs::RAW);
  return sortedDecoders;
}

bool DecoderStore::addDecoder(Decoder *decoder, int priority)
{
  m_logWriter->debug("Decoder {} added", decoder->getCode());
  if (m_decoders.count(decoder->getCode()) == 0) {
    m_decoders[decoder->getCode()] = {priority, decoder};
    return true;
  }
  delete[] decoder;
  return false;
}

bool DecoderStore::removeDecoder(int decoderId)
{
  if (m_decoders.count(decoderId)) {
    m_logWriter->debug("Decoder '{}' destroyed (removed from ::list_base)",
                        m_decoders[decoderId].m_element2->getCode());
    delete m_decoders[decoderId].m_element2;
    m_decoders.erase(decoderId);
    return true;
  }
  return false;
}

void DecoderStore::setPreferredEncoding(int encodingType)
{
  m_logWriter->debug("Decoder store: preferred encoding is \"{}\".", encodingType);
  m_preferredEncoding = encodingType;
}

void DecoderStore::allowCopyRect(bool allow)
{
  if (allow) {
    m_logWriter->debug("Decoder store: enable Copy ::int_rectangle");
  } else {
    m_logWriter->debug("Decoder store: disable Copy ::int_rectangle");
  }
  m_allowCopyRect = allow;
}
