/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: getreqhdr.cpp,v 1.2 2005/05/02 23:04:49 ehodge Exp $
 * 
 * Portions Copyright (c) 1995-2005 RealNetworks, Inc. All Rights Reserved.
 * 
 * The contents of this file, and the files included with this file,
 * are subject to the current version of the RealNetworks Public
 * Source License (the "RPSL") available at
 * http://www.helixcommunity.org/content/rpsl unless you have licensed
 * the file under the current version of the RealNetworks Community
 * Source License (the "RCSL") available at
 * http://www.helixcommunity.org/content/rcsl, in which case the RCSL
 * will apply. You may also obtain the license terms directly from
 * RealNetworks.  You may not use this file except in compliance with
 * the RPSL or, if you have a valid RCSL with RealNetworks applicable
 * to this file, the RCSL.  Please see the applicable RPSL or RCSL for
 * the rights, obligations and limitations governing use of the
 * contents of the file.
 * 
 * Alternatively, the contents of this file may be used under the
 * terms of the GNU General Public License Version 2 or later (the
 * "GPL") in which case the provisions of the GPL are applicable
 * instead of those above. If you wish to allow use of your version of
 * this file only under the terms of the GPL, and not to allow others
 * to use your version of this file under the terms of either the RPSL
 * or RCSL, indicate your decision by deleting the provisions above
 * and replace them with the notice and other provisions required by
 * the GPL. If you do not delete the provisions above, a recipient may
 * use your version of this file under the terms of any one of the
 * RPSL, the RCSL or the GPL.
 * 
 * This file is part of the Helix DNA Technology. RealNetworks is the
 * developer of the Original Code and owns the copyrights in the
 * portions it created.
 * 
 * This file, and the files included with this file, is distributed
 * and made available on an 'AS IS' basis, WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESS OR IMPLIED, AND REALNETWORKS HEREBY DISCLAIMS
 * ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET
 * ENJOYMENT OR NON-INFRINGEMENT.
 * 
 * Technology Compatibility Kit Test Suite(s) Location:
 *    http://www.helixcommunity.org/content/tck
 * 
 * Contributor(s):
 * 
 * ***** END LICENSE BLOCK ***** */

#include "hxcom.h"
#include "hxtypes.h"
#include "hxcore.h"
#include "ihxpckts.h"
#include "hxfiles.h"
#include "hxassert.h"

#include "getreqhdr.h"

///////////////////////////////////////////////////////////////////////
// /This function first gets the request headers, if any, from the
// pPlayer's IHXPlayer2.  Then it looks through the Cstring, buffer,
// and UINT32 request headers values and returns the first it finds
// with the given pszName.
//
// The caller should pass in a NULL rpValue and release it after the
// call.  If rpValue is not NULL up front, this function releases it and
// sets it to NULL before attempting to use it.
//
// Returns:
//    - HXR_INVALID_PARAMETER if either of the incoming pointers
//      are NULL or if the pszName is longer than the max
//      allowed (MAX_DISPLAY_NAME).
//    - HXR_NOINTERFACE if pPlayer->QI for IHXPlayer2 fails
//    - HXR_FAIL if no request or RequestHeaders exist
//    - HXR_FAIL if the value is not found in RequestHeaders
//   Note:
//      Under all error conditions, rulValue and rpValue will be set to
//      0 & NULL, respectively.
//
// The caller is responsible for releasing the rpValue when done with it.
//
HX_RESULT
GetRequestHeader(IHXPlayer* pPlayer,
                 const char* pszName,
                 REF(UINT32) rulValue,
                 REF(IHXBuffer*) rpValue)
{
    HX_RESULT hxr = HXR_OK;
    IHXPlayer2* pPlayer2 = NULL;

    rulValue = 0;
    HX_ASSERT(!rpValue);
    HX_RELEASE(rpValue);

    if (!pPlayer  ||  !pszName  ||  (strlen(pszName) > MAX_DISPLAY_NAME) )
    {
        return HXR_INVALID_PARAMETER;
    }

    if (HXR_OK == (hxr =
            pPlayer->QueryInterface(IID_IHXPlayer2,(void**)&pPlayer2) ) )
    {
        IHXRequest* pRequest = NULL;
        if (HXR_OK == pPlayer2->GetRequest(pRequest) )
        {
            IHXValues* pReqHdrs = NULL;
            if (HXR_OK == pRequest->GetRequestHeaders(pReqHdrs))
            {
                // /Look first at ULONG32.  If that fails, then look for CString.
                // If that fails look for Buffer.  If that fails, return FAIL:
                if (HXR_OK !=
                        pReqHdrs->GetPropertyULONG32(pszName, rulValue))
                {
                    if (HXR_OK !=
                            pReqHdrs->GetPropertyCString(pszName, rpValue))
                    {
                        if (HXR_OK !=
                                pReqHdrs->GetPropertyBuffer(pszName, rpValue))
                        {
                            hxr = HXR_FAIL;
                        }
                    }
                }
            }
            HX_RELEASE(pReqHdrs);
        }
        else
        {
            hxr = HXR_FAIL;
        }
        HX_RELEASE(pRequest);
    }
    HX_RELEASE(pPlayer2);

    return hxr;
}

