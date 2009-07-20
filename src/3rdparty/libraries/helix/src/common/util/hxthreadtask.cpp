/* ***** BEGIN LICENSE BLOCK ***** 
 * Version: RCSL 1.0/RPSL 1.0 
 *  
 * Portions Copyright (c) 1995-2002 RealNetworks, Inc. All Rights Reserved. 
 *      
 * The contents of this file, and the files included with this file, are 
 * subject to the current version of the RealNetworks Public Source License 
 * Version 1.0 (the "RPSL") available at 
 * http://www.helixcommunity.org/content/rpsl unless you have licensed 
 * the file under the RealNetworks Community Source License Version 1.0 
 * (the "RCSL") available at http://www.helixcommunity.org/content/rcsl, 
 * in which case the RCSL will apply. You may also obtain the license terms 
 * directly from RealNetworks.  You may not use this file except in 
 * compliance with the RPSL or, if you have a valid RCSL with RealNetworks 
 * applicable to this file, the RCSL.  Please see the applicable RPSL or 
 * RCSL for the rights, obligations and limitations governing use of the 
 * contents of the file.  
 *  
 * This file is part of the Helix DNA Technology. RealNetworks is the 
 * developer of the Original Code and owns the copyrights in the portions 
 * it created. 
 *  
 * This file, and the files included with this file, is distributed and made 
 * available on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER 
 * EXPRESS OR IMPLIED, AND REALNETWORKS HEREBY DISCLAIMS ALL SUCH WARRANTIES, 
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS 
 * FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT. 
 * 
 * Technology Compatibility Kit Test Suite(s) Location: 
 *    http://www.helixcommunity.org/content/tck 
 * 
 * Contributor(s): 
 *  
 * ***** END LICENSE BLOCK ***** */ 

//
// Thread message loop
//
//

#include "hxtypes.h"
#include "hxthread.h"
#include "hxthreadtask.h"
#include "pckunpck.h"

#include "hxassert.h"
#include "debug.h"
#include "hxheap.h"
#ifdef _DEBUG
#undef HX_THIS_FILE		
static const char HX_THIS_FILE[] = __FILE__;
#endif

HXThreadTask::~HXThreadTask()
{
    HX_RELEASE(m_pEvent);
}

 
HX_RESULT HXThreadTask::EnableWait(IUnknown* pContext)
{
    return CreateEventCCF((void**)&m_pEvent, pContext, NULL, FALSE);
}

HX_RESULT HXThreadTask::Complete(HX_RESULT result)
{
    m_result = result;

    HX_RESULT hr = HXR_OK;
    if( m_pEvent)
    {
        hr = m_pEvent->SignalEvent();
    }
    return hr;
}

HX_RESULT HXThreadTask::Wait()
{
    HX_RESULT hr = HXR_OK;
    if( m_pEvent )
    {
        hr = m_pEvent->Wait(ALLFS /*INFINITE*/);
    }
    return hr;
}




