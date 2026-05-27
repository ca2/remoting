// Copyright (C) 2011,2012 GlavSoft LLC.
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


#include "acme/input_output/DataInputStream.h"
#include "acme/input_output/DataOutputStream.h"
#include "remoting/remoting_rfb/rfb/AuthDefs.h"
#include "subsystem/platform/Exception.h"

#include "CapabilitiesManager.h"


namespace remoting_rfb_client
{
    //
    // This exception is raised when connection is valid, but it's invalid
    // from authentication client-logic, or authentication is failure.
    //
    class CLASS_DECL_REMOTING_RDX AuthException : public ::subsystem::Exception
    {
    public:
        AuthException(const ::scoped_string & scopedstrMessage = "Error in authentication");
        virtual ~AuthException();
        ::i32 getAuthCode() const;

    //public:
        static const ::i32 AUTH_ERROR = 1;
        static const ::i32 AUTH_UNKNOWN_TYPE = 2;
        static const ::i32 AUTH_CANCELED = 3;

    //protected:
        ::i32 m_authErrorCode;
    };

    class CLASS_DECL_REMOTING_RDX AuthUnknownException : public AuthException
    {
    public:
        AuthUnknownException(const ::scoped_string & scopedstrMessage = "Error in authentification: "
                                                    "auth is canceled or isn't support");
        virtual ~AuthUnknownException();
    };

    //
    // This exception is raised when process of authentication is canceled by user.
    //
    class CLASS_DECL_REMOTING_RDX AuthCanceledException : public AuthException
    {
    public:
        AuthCanceledException(const ::scoped_string & scopedstrMessage = "Auth is canceled");
        virtual ~AuthCanceledException();
    };

    
    class CLASS_DECL_REMOTING_RDX AuthHandler :
       virtual public ::particle
    {
    public:


       ::i32 m_id;


       AuthHandler(::i32 authType);
       ~AuthHandler() override;

        //
        // This abstract method that performs the authentication.
        // Use "input" and "output" outside authenticate() is prohibited.
        //
        // FIXME: AUTH: document throwing AuthCanceledException
        virtual void authenticate(::DataInputStream * pinput, ::DataOutputStream * pdataoutputstream) = 0;

        //
        // This abstract method that add auth-capability in RemoveViewerCore.
        //
        virtual void addAuthCapability(CapabilitiesManager *capabilitiesManager) = 0;

        //
        // This method return type of authentication (m_id).
        //
        virtual ::i32 getType() const;

    };

} // namespace remoting_rfb_client


