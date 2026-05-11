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

#include "subsystem/platform/RegistryKey.h"
#include "MsiProperties.h"
//#include "subsystem/platform/::string.h"
#include "subsystem/platform/VncPassCrypt.h"
#include "remoting/remoting/node_config/Configurator.h"
#include "remoting/remoting/config/RegistrySettingsManager.h"
#include "remoting/node_desktop/NamingDefs.h"

// #include aaa_<msiquery.h>
// #include aaa_<shlwapi.h>
//#include aaa_<algorithm>
//// #include aaa_<wcautil.h>

const TCHAR SAS_REG_ENTRY[] = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System";
const TCHAR SAS_REG_KEY[] = "SoftwareSASGeneration";

void allowSas()
{
  RegistryKey regKey(HKEY_LOCAL_MACHINE,
                     SAS_REG_ENTRY,
                     true);
  int sasValue = 0;
  regKey.getValueAsInt32(SAS_REG_KEY, &sasValue);
  sasValue |= 1;
  regKey.setValueAsInt32(SAS_REG_KEY, sasValue);
}

::u32 __stdcall AllowSas(MSIHANDLE hInstall)
{
  allowSas();
  return ERROR_SUCCESS;
}

// FIXME: Code duplication: see the ControlApplication class.
void getCryptedPassword(unsigned char cryptedPass[8], const ::scoped_string & plainPass)
{
  // Get a copy of the password truncated at 8 characters.
  ::string copyOfPlainPass;
  plainPass->getSubstring(&copyOfPlainPass, 0, 7);
  // Convert from TCHAR[] to char[].
  // FIXME: Check exception catching.
  ::string ansiPass(&copyOfPlainPass);

  // Convert to a byte array.
  unsigned char byteArray[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  size_t len = ::minimum(ansiPass.length(), (size_t)8);
  memcpy(byteArray, ansiPass, len);

  // Encrypt with a fixed key.
  VncPassCrypt::getEncryptedPass(cryptedPass, byteArray);
}

void writePasswordToRegistry(MSIHANDLE hInstall,
                             const ::scoped_string & scopedstrRegistryPath,
                             const ::scoped_string & scopedstrEntryName)
{
  MsiProperties msiProp(hInstall);
  try {
    ::string plainPass;
    msiProp.getString("CustomActionData", &plainPass);

    unsigned char cryptedPass[8];
    getCryptedPassword(cryptedPass, &plainPass);

    RegistrySecurityAttributes registrySA;
    SECURITY_ATTRIBUTES *sa = 0;
    sa = registrySA.getServiceSA();
    ::subsystem::registry rootKey = HKEY_LOCAL_MACHINE;
    RegistrySettingsManager sm(rootKey, registryPath, sa);

    if (!sm.setBinaryData(entryName, &cryptedPass[0], 8)) {
      throw ::subsystem::Exception("Can't w_rite to the registry.");
    }

  } catch (::exception &e) {
    ::string ansiStr(&::string(e.get_message()));
    //WcaLog(LOGMSG_STANDARD, ansiStr);
  }
}


::u32 __stdcall SetRfbPassword(MSIHANDLE hInstall)
{
  try {
    writePasswordToRegistry(hInstall,
                            RegistryPaths::SERVER_PATH, "Password");
  } catch (...) {
    return ERROR_INSTALL_FAILURE;
  }
  return ERROR_SUCCESS;
}

::u32 __stdcall SetViewOnlyPassword(MSIHANDLE hInstall)
{
  try {
    writePasswordToRegistry(hInstall,
                            RegistryPaths::SERVER_PATH, "PasswordViewOnly");
  } catch (...) {
    return ERROR_INSTALL_FAILURE;
  }
  return ERROR_SUCCESS;
}

::u32 __stdcall SetControlPassword(MSIHANDLE hInstall)
{
  try {
    writePasswordToRegistry(hInstall,
                            RegistryPaths::SERVER_PATH, "ControlPassword");
  } catch (...) {
    return ERROR_INSTALL_FAILURE;
  }
  return ERROR_SUCCESS;
}

extern "C" bool WINAPI DllMain(
                               __in HINSTANCE hInst,
                               __in ULONG ulReason,
                               __in LPVOID
                               )
{
  return true;
}
