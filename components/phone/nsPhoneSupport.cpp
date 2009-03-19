/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is Minimo Tel Protocol Handler.
 *
 * The Initial Developer of the Original Code is
 * the Mozilla Foundation <http://www.mozilla.org/>.
 * Portions created by the Initial Developer are Copyright (C) 2008
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *  Doug Turner <dougt@meer.net>
 *  Nino D'Aversa <ninodaversa@gmail.com>
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#include <windows.h>
#include <phone.h>
#include "nsStringAPI.h"
#include "nsIPhoneSupport.h"
#include "nsIGenericFactory.h"

class nsPhoneSupport : public nsIPhoneSupport
{
public:

  NS_DECL_ISUPPORTS
  NS_DECL_NSIPHONESUPPORT

  nsPhoneSupport() {};
  ~nsPhoneSupport(){};

};

NS_IMPL_ISUPPORTS1(nsPhoneSupport, nsIPhoneSupport)

NS_IMETHODIMP
nsPhoneSupport::MakeCall(const PRUnichar *telephoneNumber, const PRUnichar *telephoneDescription, PRBool aPrompt)
{
  long result = -1;

  typedef LONG (*__PhoneMakeCall)(PHONEMAKECALLINFO *ppmci);

  HMODULE hPhoneDLL = LoadLibraryW(L"phone.dll");
  if(hPhoneDLL)
  {
    __PhoneMakeCall MakeCall = (__PhoneMakeCall) GetProcAddress( hPhoneDLL,
                                                                 "PhoneMakeCall");
    if(MakeCall)
    {
      PHONEMAKECALLINFO callInfo;
      callInfo.cbSize          = sizeof(PHONEMAKECALLINFO);
      callInfo.dwFlags         = aPrompt ? PMCF_PROMPTBEFORECALLING : PMCF_DEFAULT;
      callInfo.pszDestAddress  = telephoneNumber;
      callInfo.pszAppName      = nsnull;
      callInfo.pszCalledParty  = telephoneDescription;
      callInfo.pszComment      = nsnull;
      result = MakeCall(&callInfo);
    }
    FreeLibrary(hPhoneDLL);
  }
  return (result == 0) ? NS_OK : NS_ERROR_FAILURE;
}

//------------------------------------------------------------------------------
//  XPCOM REGISTRATION BELOW
//------------------------------------------------------------------------------

#define nsPhoneSupport_CID                          \
{ 0x2a08c9e4, 0xf853, 0x4f02,                       \
{0x88, 0xd8, 0xd6, 0x2f, 0x27, 0xca, 0x06, 0x85} }

#define nsPhoneSupport_ContractID "@mozilla.org/phone/support;1"

NS_GENERIC_FACTORY_CONSTRUCTOR(nsPhoneSupport)

static const nsModuleComponentInfo components[] =
{
  { "Phone Support",
    nsPhoneSupport_CID,
    nsPhoneSupport_ContractID,
    nsPhoneSupportConstructor,
    nsnull,
    nsnull
  }
};

NS_IMPL_NSGETMODULE(nsPhoneSupportModule, components)