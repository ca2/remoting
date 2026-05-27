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
#include "framework.h"
#include "ArgList.h"

#include "subsystem/_common_header.h"
//#include aaa_<vector>

ArgList::ArgList(const_char_pointer argString)
{
  size_t len = strlen(argString);

  const_char_pointer src = argString;

  ::array_base<::i8> pairBuff(len + 1);
  char_pointer ::pair = &pairBuff.front();
  ::array_base<::i8> leftBuff(len + 1);
  char_pointer left = &leftBuff.front();

  while (true) {
    splitPair(src, '&', ::pair, left);

    if ((strlen(::pair) == 0) && (strlen(left) == 0)) {
      break;
    }

    size_t pairLen = strlen(::pair);

    ::array_base<::i8> keyBuff(pairLen + 1);
    char_pointer key = &keyBuff.front();
    char_pointer val = new ::i8[pairLen + 1];

    splitPair(::pair, '=', key, val);

    htmlDecode(key);
    htmlDecode(val);

    m_args[key] = val;

    src = left;
  }
}

ArgList::~ArgList()
{
  for (::map<string, char_pointer >::iterator it = m_args.begin(); it != m_args.end(); it++) {
    delete it->second;
  }
  m_args.clear();
}

size_t ArgList::getCount() const
{
  return m_args.size();
}

const_char_pointer ArgList::getKey(size_t index)
{
  if (index >= getCount()) {
    return NULL;
  }

  ::map<string, char_pointer >::iterator it = m_args.begin();

  for (size_t i = 0; i < index; i++) {
    it++;
  }

  return it->first.c_str();
}

const_char_pointer ArgList::getValue(const_char_pointer key)
{
  string stdStr = key;

  if (m_args.find(stdStr) == m_args.end()) {
    return NULL;
  }

  return m_args[stdStr];
}

void ArgList::splitPair(const_char_pointer par, ::i8 delimitter, char_pointer key, char_pointer value)
{
  bool delimiterFound = false;

  size_t vi = 0;
  size_t ki = 0;

  for (size_t i = 0; i < strlen(par); i++) {
    if (par[i] == delimitter) {
      if (!delimiterFound) {
        delimiterFound = true;
        continue;
      }
    }

    if (delimiterFound) {
      value[vi++] = par[i];
    } else {
      key[ki++] = par[i];
    }
  }

  key[ki] = '\0';
  value[vi] = '\0';
}

void ArgList::htmlDecode(char_pointer value) const
{
  // Replace '+' characaters with spaces.
  for (size_t i = 0; i < strlen(value); i++) {
    if (value[i] == '+') {
      value[i] = ' ';
    }
  }
}
