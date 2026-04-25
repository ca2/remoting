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
//#include "acme/_operating_system.h"
#include "RfbInitializer.h"
//#include "subsystem/thread/critical_section.h"
#include "remoting/remoting/rfb/VendorDefs.h"
#include "remoting/remoting/rfb/AuthDefs.h"
#include "CapContainer.h"
#include "remoting/remoting/server_config/Configurator.h"
#include "AuthException.h"
#include "subsystem/platform/VncPassCrypt.h"
#include "subsystem/socket/SocketAddressIPv4.h"
#include "subsystem/node/OperatingSystem.h"
//#include "subsystem/platform/::string.h"
//#include "remoting/node_desktop/NamingDefs.h"

// #include aaa_<stdlib.h>
//#include aaa_<time.h>

namespace remoting
{
   // RfbInitializer::RfbInitializer(Channel *stream,
   //                                ClientAuthListener *pclientauthlistener,
   //                                RfbClient *client, bool authAllowed)
   // : m_shared(false),
   //   m_tightEnabled(false),
   //   m_minorVerNum(0),
   //   m_extAuthListener(pclientauthlistener),
   //   m_prfbclient(client),
   //   m_bAuthAllowed(authAllowed),
   //   m_viewOnlyAuth(false)
   // {
   //    m_pdataoutputstream = new DataOutputStream(stream);
   //    m_pdatainputstream = new DataInputStream(stream);
   // }

   RfbInitializer::RfbInitializer()
   : m_shared(false),
     m_tightEnabled(false),
     m_minorVerNum(0),
     //m_pclientauthlistener(pclientauthlistener),
     //m_prfbclient(client),
     m_bAuthAllowed(false),
     m_viewOnlyAuth(false)
   {
   }
   RfbInitializer::~RfbInitializer()
   {
      //delete m_pdataoutputstream;
      //delete m_pdatainputstream;
   }

   void RfbInitializer::initialize_rfb_initializer(Channel * pchannel, ClientAuthListener *pclientauthlistener, RfbClient *prfbclient, bool bAuthAllowed)
   {
      m_pdataoutputstream = allocateø DataOutputStream(pchannel);
      m_pdataoutputstream = allocateø DataInputStream(pchannel);
      m_pclientauthlistener = pclientauthlistener;
      m_prfbclient = prfbclient;
      m_bAuthAllowed = bAuthAllowed;

   }


   void RfbInitializer::authPhase()
   {
      initVersion();
      initAuthenticate();
      readClientInit();
   }

   void RfbInitializer::afterAuthPhase(const CapContainer *srvToClCaps,
                                       const CapContainer *clToSrvCaps,
                                       const CapContainer *encCaps,
                                       const ::int_size & size,
                                       const ::innate_subsystem::PixelFormat & pixelformat)
   {
      sendServerInit(size, pixelformat);
      sendDesktopName();
      if (m_tightEnabled) {
         sendInteractionCaps(srvToClCaps, clToSrvCaps, encCaps);
      }
   }

   void RfbInitializer::initVersion()
   {
      char initVersionMsg[] = "RFB 003.008\n";
      char clientVersionMsg[13];
      size_t msgLen = 12;
      m_pdataoutputstream->write(initVersionMsg, msgLen);
      m_pdatainputstream->readFully(clientVersionMsg, msgLen);
      clientVersionMsg[12] = 0;
      m_minorVerNum = getProtocolMinorVersion(clientVersionMsg);

      try {
         checkForLoopback();
         // Checking for a ban before auth and then after.
         checkForBan();
      } catch (::exception &e) {
         if (m_minorVerNum == 3) {
            m_pdataoutputstream->writeUInt32(0);
         } else {
            m_pdataoutputstream->writeUInt8(0);
         }
         ::string reason(e.get_message());
         unsigned int reasonLen = (unsigned int)reason.length();
         _ASSERT(reasonLen == reason.length());

         m_pdataoutputstream->writeUInt32(reasonLen);
         m_pdataoutputstream->write(reason, reasonLen);

         throw;
      }
   }

   void RfbInitializer::checkForLoopback()
   {
      auto psockAddr = m_prfbclient->getSocketAddr();

      bool isLoopback = psockAddr->isLoopbackAddress();

      ServerConfig *srvConf = m_pconfigurator->getServerConfig();
      if (isLoopback && !srvConf->isLoopbackConnectionsAllowed()) {
         throw ::subsystem::Exception("Sorry, loopback connections are not enabled");
      }
      if (srvConf->isOnlyLoopbackConnectionsAllowed() && !isLoopback) {
         throw ::subsystem::Exception("Your connection has been rejected");
      }
   }

   void RfbInitializer::doTightAuth()
   {
      // Negotiate tunneling.
      m_pdataoutputstream->writeUInt32(0);
      // Negotiate authentication.
      // FIXME: Recognize authentication types.
      if (m_pconfigurator->getServerConfig()->isUsingAuthentication()
          && m_bAuthAllowed) {
         CapContainer authInfo;
         authInfo.addCap(AuthDefs::VNC, VendorDefs::STANDARD, AuthDefs::SIG_VNC);
         m_pdataoutputstream->writeUInt32(authInfo.getCapCount());
         authInfo.sendCaps(m_pdataoutputstream);
         // Read the security type selected by the client.
         unsigned int clientAuthValue = m_pdatainputstream->readUInt32();
         if (!authInfo.includes(clientAuthValue)) {
            throw ::subsystem::Exception("");
         }
         doAuth(clientAuthValue);
          } else {
             m_pdataoutputstream->writeUInt32(0);
             doAuth(AuthDefs::NONE);
          }
   }

   void RfbInitializer::doAuth(unsigned int authType)
   {
      if (authType == AuthDefs::VNC) {
         doVncAuth();
      } else if (authType == AuthDefs::NONE) {
         doAuthNone();
      } else {
         throw ::subsystem::Exception("");
      }
      // Perform additional work via a listener.
      m_pclientauthlistener->onCheckAccessControl(m_prfbclient);
      // Send authentication result.
      if (m_minorVerNum >= 8 || authType != AuthDefs::NONE) {
         m_pdataoutputstream->writeUInt32(0); // FIXME: Use a named constant instead of 0.
      }
   }

   void RfbInitializer::doVncAuth()
   {
      unsigned char challenge[16];
      srand((unsigned)time(0));
      for (int i = 0; i < sizeof(challenge); i++) {
         challenge[i] = rand() & 0xff;
      }

      m_pdataoutputstream->write(challenge, sizeof(challenge));
      unsigned char response[16];
      m_pdatainputstream->readFully(response, sizeof(response));
      // Checking for a ban after auth.
      checkForBan();

      // Comparing the challenge with the response.
      ServerConfig *srvConf = m_pconfigurator->getServerConfig();
      bool hasPrim = srvConf->hasPrimaryPassword();
      bool hasRdly = srvConf->hasReadOnlyPassword();

      if (!hasPrim && !hasRdly) {
         throw AuthException("Server is not configured properly");
      }

      if (hasPrim) {
         unsigned char crypPrimPass[8];
         srvConf->getPrimaryPassword(crypPrimPass);
         ::subsystem::VncPassCrypt passCrypt;
         passCrypt.updatePlain(crypPrimPass);
         if (passCrypt.challengeAndResponseIsValid(challenge, response)) {
            return;
         }
      }
      if (hasRdly) {
         unsigned char crypReadOnlyPass[8];
         srvConf->getReadOnlyPassword(crypReadOnlyPass);
         ::subsystem::VncPassCrypt passCrypt;
         passCrypt.updatePlain(crypReadOnlyPass);
         if (passCrypt.challengeAndResponseIsValid(challenge, response)) {
            m_viewOnlyAuth = true;
            return;
         }
      }
      // At this time we are sure that the client was typed an incorectly password.
      m_pclientauthlistener->onAuthFailed(m_prfbclient);

      ::string clientAddressStorage;
      m_prfbclient->getPeerHost(clientAddressStorage);
      ::string errMess;
      errMess.formatf("Authentication failed from {}", clientAddressStorage);

      throw AuthException(errMess);
   }

   void RfbInitializer::doAuthNone()
   {
   }

   void RfbInitializer::initAuthenticate()
   {
      try {
         // Determine effective security type from the configuration.
         unsigned int primSecType = SecurityDefs::VNC;
         if (!m_pconfigurator->getServerConfig()->isUsingAuthentication()
             || !m_bAuthAllowed) {
            primSecType = SecurityDefs::NONE;
             }
         // Here the protocol varies between versions 3.3 and 3.7+.
         if (m_minorVerNum >= 7) {
            // Send a ::list_base with two security types -- VNC-compatible security type
            // and a special code allowing to enable TightVNC protocol extensions.
            m_pdataoutputstream->writeUInt8(2);
            m_pdataoutputstream->writeUInt8(primSecType);
            m_pdataoutputstream->writeUInt8(SecurityDefs::TIGHT);
            // Read what the client has actually selected.
            unsigned char clientSecType = m_pdatainputstream->readUInt8();
            if (clientSecType == SecurityDefs::TIGHT) {
               m_tightEnabled = true;
               doTightAuth();
            } else {
               if (clientSecType != primSecType) {
                  throw ::subsystem::Exception("Security types do not match");
               }
               doAuth(AuthDefs::convertFromSecurityType(clientSecType));
            }
         } else {
            // Just tell the client we will use the configured security type.
            m_pdataoutputstream->writeUInt32(primSecType);
            doAuth(AuthDefs::convertFromSecurityType(primSecType));
         }
      } catch (AuthException &e) {
         // FIXME: The authentication result must be sent in protocols 3.3 and 3.7
         //        as well, unless the authentication was set to AuthDefs::NONE.
         if (m_minorVerNum >= 8) {
            ::string reason(e.get_message());
            unsigned int reasonLen = (unsigned int)reason.length();
            _ASSERT(reasonLen == reason.length());

            m_pdataoutputstream->writeUInt32(1); // FIXME: Use a named constant instead of 1.
            m_pdataoutputstream->writeUInt32(reasonLen);
            m_pdataoutputstream->write(reason, reasonLen);
         }
         throw;
      }
   }

   void RfbInitializer::readClientInit()
   {
      m_shared = m_pdatainputstream->readUInt8() != 0;
   }

   void RfbInitializer::sendServerInit(const ::int_size & size,
                                       const ::innate_subsystem::PixelFormat & pixelformat)
   {
      m_pdataoutputstream->writeUInt16((unsigned short)size.cx);
      m_pdataoutputstream->writeUInt16((unsigned short)size.cy);
      // Pixel format
      m_pdataoutputstream->writeUInt8((unsigned char)pixelformat.bitsPerPixel);
      m_pdataoutputstream->writeUInt8((unsigned char)pixelformat.colorDepth);
      m_pdataoutputstream->writeUInt8((unsigned char)pixelformat.bigEndian);
      m_pdataoutputstream->writeUInt8(1);
      m_pdataoutputstream->writeUInt16((unsigned short)pixelformat.redMax);
      m_pdataoutputstream->writeUInt16((unsigned short)pixelformat.greenMax);
      m_pdataoutputstream->writeUInt16((unsigned short)pixelformat.blueMax);
      m_pdataoutputstream->writeUInt8((unsigned char)pixelformat.redShift);
      m_pdataoutputstream->writeUInt8((unsigned char)pixelformat.greenShift);
      m_pdataoutputstream->writeUInt8((unsigned char)pixelformat.blueShift);
      // Padding
      m_pdataoutputstream->writeUInt8(0);
      m_pdataoutputstream->writeUInt16(0);
   }

   void RfbInitializer::sendDesktopName()
   {
      ::string deskName;
      try
      {
         deskName = MainSubsystem().OperatingSystem().getComputerName();
      }
      catch (...)
      {
         //deskName= DefaultNames::DEFAULT_COMPUTER_NAME;
         deskName= "Terminal Server";
      }

      //::string ansiName(&deskName);
      unsigned int dnLen = (unsigned int)deskName.length();
      //_ASSERT(dnLen == ansiName.length());

      m_pdataoutputstream->writeUInt32(dnLen);
      m_pdataoutputstream->write(deskName.c_str(), dnLen);
   }

   void RfbInitializer::sendInteractionCaps(const CapContainer *srvToClCaps,
                                            const CapContainer *clToSrvCaps,
                                            const CapContainer *encCaps)
   {
      m_pdataoutputstream->writeUInt16(srvToClCaps->getCapCount());
      m_pdataoutputstream->writeUInt16(clToSrvCaps->getCapCount());
      m_pdataoutputstream->writeUInt16(encCaps->getCapCount());
      m_pdataoutputstream->writeUInt16(0); // Pad

      srvToClCaps->sendCaps(m_pdataoutputstream);
      clToSrvCaps->sendCaps(m_pdataoutputstream);
      encCaps->sendCaps(m_pdataoutputstream);
   }

   unsigned int RfbInitializer::getProtocolMinorVersion(const char str[12])
   {
      if ( str[0] != 'R' || str[1] != 'F' || str[2] != 'B' || str[3] != ' ' ||
           !isdigit(str[4]) || !isdigit(str[5]) || !isdigit(str[6]) ||
           str[7] != '.' ||
           !isdigit(str[8]) || !isdigit(str[9]) || !isdigit(str[10]) ||
           str[11] != '\n' ) {
         throw ::subsystem::Exception("Invalid format of the RFB version scopedstrMessage");
           }

      unsigned int majorVersion =
        (str[4] - '0') * 100 + (str[5] - '0') * 10 + (str[6] - '0');
      if (majorVersion != 3) {
         throw ::subsystem::Exception("Unsupported RFB protocol version requested");
      }

      unsigned int minorVersion =
        (str[8] - '0') * 100 + (str[9] - '0') * 10 + (str[10] - '0');
      return minorVersion;
   }

   void RfbInitializer::checkForBan()
   {
      if (m_pclientauthlistener->onCheckForBan(m_prfbclient)) {
         throw AuthException("Your connection has been rejected");
      }
   }
} // namespace remoting

