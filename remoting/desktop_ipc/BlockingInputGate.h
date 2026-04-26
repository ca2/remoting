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

#pragma once


#include "acme/input_output/DataInputStream.h"
//#include "subsystem/thread/critical_section.h"

namespace remoting
{

   class BlockingInputGate :
   virtual public critical_section,
   virtual public DataInputStream
   {
   public:
      //BlockingInputGate(InputStream *inputStream);
      BlockingInputGate();
      ~BlockingInputGate() override;


      virtual void initialize_blocking_input_gate(InputStream *inputStream);


   };


} // namespace remoting
