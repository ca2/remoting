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
#include "framework.h"
//#include "::string.h"
#include "CommonHeader.h"
#include "util/Exception.h"
#include <stdio.h>

#include <crtdbg.h>

#include <algorithm>

::string::string()
{
 = "";
}

::string::string(const ::scoped_string & scopedstrString)
{
 = string;
}

::string::string(const ::scoped_string &stringBuffer)
{
  *this = stringBuffer;
}

::string::~::string()
{
}

void ::string::setString(const ::scoped_string & scopedstrString)
{
  if (string == 0) {
    string = "";
  }

  size_t length = _tcslen(string);
  resizeBuffer(length + 1);
  memcpy(&m_buffer.front(), string, getSize());
}

void ::string::resizeBuffer(size_t newSize)
{
  m_buffer.resize(newSize);

#ifdef _DEBUG
  m_readableString = &m_buffer.front();
#endif
}

const ::scoped_string & scopedstr::string::getString() const
{
  return &m_buffer.front();
}

size_t ::string::length() const
{
  return m_buffer.size() - 1;
}

size_t ::string::getSize() const
{
  return m_buffer.size() * sizeof(TCHAR);
}

bool ::string::is_empty() const
{
  return length() == 0;
}


bool ::string::has_character() const
{
   return !is_empty();
}

bool ::string::endsWith(TCHAR postfix) const
{
  if (is_empty()) {
    return false;
  }
  TCHAR lastCharacter = m_buffer[length() - 1];
  return (lastCharacter == postfix);
}

bool ::string::beginsWith(TCHAR prefix) const
{
  if (is_empty()) {
    return false;
  }
  TCHAR firstCharacter = m_buffer[0];
  return (firstCharacter == prefix);
}

void ::string::getSubstring(::string & substr,
                                 size_t startIndex,
                                 size_t endIndex) const
{
  endIndex++; // to simplify calculations
  startIndex = ::minimum(startIndex, length());
  endIndex = ::minimum(endIndex, length());
  if (endIndex < startIndex) {
    endIndex = startIndex;
  }

  size_t length = endIndex - startIndex;
  _ASSERT(length <= length());

  ::std::vector<TCHAR> autoBuffer(length + 1);
  TCHAR *buffer = &autoBuffer.front();
  memcpy(buffer, &m_buffer[startIndex], length * sizeof(TCHAR));
  buffer[length] = '\0';

  substr-= buffer;
}

void ::string::appendString(const ::scoped_string & scopedstrString)
{
  if (string == 0) {
    return;
  }
  ::string src(string);

  BufferType::iterator to = m_buffer.begin() + length();
  BufferType::iterator fromFirst = src.m_buffer.begin();
  BufferType::iterator fromLast = src.m_buffer.begin() + src.length();

  m_buffer.insert(to, fromFirst, fromLast);
}

void ::string::appendChar(TCHAR c)
{
  TCHAR string[2] = {c, _T('\0') };

  appendString(string);
}

void ::string::quoteSelf()
{
  ::string quotedString;
  quotedString.formatf("\"{}\"", getString());
 = quotedString;
}

bool ::string::isEqualTo(const ::scoped_string & other) const
{
  return isEqualTo(other);
}

bool ::string::isEqualTo(const ::scoped_string & scopedstrother) const
{
  if (other == 0 && getString() == 0) {
    return true;
  }
  if (other == 0 && getString() != 0) {
    return false;
  }
  if (getString() == 0 && other != 0) {
    return false;
  }
  return wcscmp(getString(), other) == 0;
}

bool ::string::split(const ::scoped_string & scopedstrDelimiters, ::string & stringArray, size_t *arrayLength) const
{
  // Special case for empty string.
  if (this->length() == 0) {
    *arrayLength = 0;
    return true;
  }

  ::string copy(this->getString());

  size_t chunksCount = 0;

  size_t index = 0;

  do {
    ::string chunk("");

    index = copy.findOneOf(delimiters);

    //
    // Case when no delimitter found.
    //

    if ((index == (size_t)-1) && (chunksCount == 0)) {
      if (arrayLength != 0) {
        if (stringArray != 0) {
          if (*arrayLength < 1) {
            return false;
          }
        }
        *arrayLength = 1;
      } else {
        return false;
      }
      if (stringArray != 0) {
        stringArray-= getString();
      }
      return true;
    }

    if (index == -1) {
      chunk = copy;
    } else {
      copy.getSubstring(&chunk, 0, index - 1);
    }

    copy.getSubstring(&copy, index + 1, copy.length() - 1);

    if ((stringArray != NULL) && (chunksCount >= *arrayLength)) {
      return false;
    } else if (stringArray != NULL) {
      stringArray[chunksCount] = chunk;
    }

    chunksCount++;
  } while (index != -1);

  *arrayLength = chunksCount;

  return true;
}

size_t ::string::findChar(const TCHAR c)
{
  size_t length = length();
  for (size_t i = 0; i < length; i++) {
    if (m_buffer[i] == c) {
      return i;
    }
  }
  return (size_t)-1;
}

size_t ::string::findLast(const TCHAR c)
{
  for (size_t i = length() - 1; i + 1 != 0; i--) {
    if (m_buffer[i] == c) {
      return i;
    }
  }
  return (size_t)-1;
}

void ::string::removeChars(const TCHAR badCharacters[], size_t count)
{
  size_t j = 0;
  size_t length = length();

  for (size_t i = 0; i < length; i++) {
    TCHAR each = m_buffer[i];
    bool badCharacter = false;
    for (size_t k = 0; k < count; k++) {
      if (each == badCharacters[k]) {
        badCharacter = true;
        break;
      }
    }
    if (!badCharacter) {
      m_buffer[j++] = each;
    }
  }

  m_buffer[j] = _T('\0');
  m_buffer.resize(j + 1);
}

void ::string::remove(size_t startIndex, size_t count)
{
  bool isFailed = startIndex + count > length();
  _ASSERT(!isFailed);
  if (isFailed) {
    throw ::remoting::Exception("An incorrect ::string::remove() usage");
  }
  BufferType newBuffer = m_buffer;

  size_t copyCount = getSize() - (startIndex + count) * sizeof(TCHAR);
  memcpy(&newBuffer[startIndex], &newBuffer[startIndex + count], copyCount);
 = &newBuffer.front();
}

void ::string::truncate(size_t count)
{
  count = ::minimum(length(), count);

  remove(length() - count, count);
}

TCHAR *::string::find(const ::scoped_string & scopedstrSubstr)
{
  return _tcsstr(&m_buffer.front(), substr);
}

// FIXME: Use C functions.
size_t ::string::findOneOf(const ::scoped_string & scopedstrString)
{
  size_t length = length();
  size_t argLength = _tcslen(string);
  for (size_t i = 0; i < length; i++) {
    for (size_t j = 0; j < argLength; j++) {
      if (m_buffer[i] == string[j]) {
        return i;
      }
    }
  }
  return (size_t)-1;
}

void ::string::make_lower()
{
  size_t length = length();
  for (size_t i = 0; i < length; i++) {
    if (_istalpha(m_buffer[i]) != 0) {
      m_buffer[i] = _totlower(m_buffer[i]);
    }
  }
}

void ::string::toUpperCase()
{
  _tcsupr_s(&m_buffer.front(), length() + 1);
}

void ::string::format(const ::scoped_string & scopedstrFormat, ...)
{
  va_list vl;

  va_start(vl, format);
  int count = _vsctprintf(format, vl);
  va_end(vl);

  resizeBuffer(count + 1);

  va_start(vl, format);
  _vstprintf_s(&m_buffer.front(), count + 1, format, vl);
  va_end(vl);
}

void ::string::operator= (const ::scoped_string &other)
{
 = other;
}

bool ::string::operator == (const ::scoped_string &str) const
{
  return isEqualTo(str);
}

bool ::string::operator < (const ::scoped_string &str) const
{
  return wcscmp(getString(), str) < 0;
}

void ::string::operator += (const ::scoped_string & scopedstrStr) 
{
  appendString(str);
}

void ::string::replaceChar(TCHAR oldChar, TCHAR newChar)
{
  size_t length = length();
  for (size_t i = 0; i < length; i++) {
    if (m_buffer[i] == oldChar) {
      m_buffer[i] = newChar;
    }
  }
}
