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
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//
#include "framework.h"
#include "CapsContainer.h"

#include "thread/AutoLock.h"

CapsContainer::CapsContainer()
{
}

CapsContainer::~CapsContainer()
{
}

void CapsContainer::add(unsigned int code, const char *vendor, const char *name,
                        const ::string desc)
{
  // Fill in an RfbCapabilityInfo structure and pass it to the overloaded
  // function.
  RfbCapabilityInfo capinfo;
  capinfo.code = code;
  memcpy(capinfo.vendorSignature, vendor, RfbCapabilityInfo::vendorSigSize);
  memcpy(capinfo.nameSignature, name, RfbCapabilityInfo::nameSigSize);
  add(&capinfo, desc);
}

void CapsContainer::add(const RfbCapabilityInfo *capinfo,
                        const ::string desc)
{
  AutoLock al(&m_mapLock);

  infoMap[capinfo->code] = *capinfo;
  enableMap[capinfo->code] = false;
  descMap[capinfo->code] = desc;
}

bool CapsContainer::isAdded(unsigned int code) const
{
  AutoLock al(&m_mapLock);

  return isKnown(code);
}

bool CapsContainer::getInfo(unsigned int code, RfbCapabilityInfo *capinfo)
{
  AutoLock al(&m_mapLock);
  if (isKnown(code)) {
    *capinfo = infoMap[code];
    return true;
  }

  return false;
}

::string CapsContainer::getDescription(unsigned int code) const
{
  AutoLock al(&m_mapLock);
  return (isKnown(code)) ? descMap.find(code)->element2() : ::string();
}

bool CapsContainer::enable(const RfbCapabilityInfo *capinfo)
{
  AutoLock al(&m_mapLock);
  if (!isKnown(capinfo->code)) {
    return false;
  }

  const RfbCapabilityInfo known = infoMap[capinfo->code];
  if (!known.isEqual(capinfo->vendorSignature, capinfo->nameSignature)){
    enableMap[capinfo->code] = false;
    return false;
  }

  enableMap[capinfo->code] = true;
  m_plist.add(capinfo->code);
  return true;
}

bool CapsContainer::isEnabled(unsigned int code) const
{
  AutoLock al(&m_mapLock);
  return (isKnown(code)) ? enableMap.find(code)->element2() : false;
}

size_t CapsContainer::numEnabled() const
{
  AutoLock al(&m_mapLock);
  return m_plist.size();
}

unsigned int CapsContainer::getByOrder(size_t idx)
{
  AutoLock al(&m_mapLock);
  return (idx < m_plist.size()) ? m_plist[idx] : 0;
}

void CapsContainer::getEnabledCapabilities(::array_base<unsigned int> &codes) const
{
  AutoLock al(&m_mapLock);
  codes = m_plist;
}

bool CapsContainer::isKnown(unsigned int code) const
{
  return (descMap.find(code) != descMap.end());
}

bool RfbCapabilityInfo::isEqual(const char *vendor, const char *signature) const
{
  if (memcmp(vendorSignature, vendor, vendorSigSize) != 0 ||
      memcmp(nameSignature, signature, nameSigSize) != 0 ) {
    return false;
  }
  return true;
}
