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
#include "framework.h"
#include "ComboBox.h"

#include <Windowsx.h>

ComboBox::ComboBox()
{
}

ComboBox::~ComboBox()
{
}

int ComboBox::addItem(const ::scoped_string & scopedstrText)
{
  return ComboBox_AddString(m_hwnd, ::wstring(scopedstrText).c_str());
}

int ComboBox::addItem(const ::scoped_string & scopedstrText, void *tag)
{
  int index = addItem(scopedstrText);
  setItemData(index, tag);
  return index;
}

void ComboBox::insertItem(int index, const ::scoped_string & scopedstrText)
{
  ComboBox_InsertString(m_hwnd, index, ::wstring(scopedstrText).c_str());
}

void ComboBox::insertItem(int index, const ::scoped_string & scopedstrText, void *tag)
{
  insertItem(index, scopedstrText);
  setItemData(index, tag);
}

int ComboBox::getItemsCount() const
{
  return ComboBox_GetCount(m_hwnd);
}

void ComboBox::setItemData(int index, void *tag)
{
  ComboBox_SetItemData(m_hwnd, index, (LPARAM)tag);
}

void *ComboBox::getItemData(int index) const
{
  return (void *)ComboBox_GetItemData(m_hwnd, index);
}

::string ComboBox::getItemText(int index) const
{
  size_t length = ComboBox_GetLBTextLen(m_hwnd, index);
  ::string str;

   auto p = str.get_buffer(length + 1);
  ComboBox_GetLBText(m_hwnd, index, p);
   str.release_buffer();
  //str = buf.data();]
   return str;
}

int ComboBox::getSelectedItemIndex()
{
  return ComboBox_GetCurSel(m_hwnd);
}

void ComboBox::setSelectedItem(int index)
{
  ComboBox_SetCurSel(m_hwnd, index);
}

void ComboBox::deleteItem(int index)
{
  ComboBox_DeleteString(m_hwnd, index);
}

void ComboBox::removeAllItems()
{
  ComboBox_ResetContent(m_hwnd);
}

