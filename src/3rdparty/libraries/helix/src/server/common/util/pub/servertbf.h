/* ***** BEGIN LICENSE BLOCK *****  
 * Source last modified: $Id: servertbf.h,v 1.4 2005/09/12 18:41:59 jc Exp $ 
 *   
 * Portions Copyright (c) 1995-2003 RealNetworks, Inc. All Rights Reserved.  
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

#ifndef SERVERTBF_H
#define SERVERTBF_H
#include "hxtypes.h"
#include "hxengin.h"

class CServerTBF: public IHXTokenBufferFilter
{
public:
    CServerTBF();
    virtual ~CServerTBF();
    /*
     *	IUnknown methods
     */
    STDMETHOD(QueryInterface)	(THIS_
				REFIID riid,
				void** ppvObj);

    STDMETHOD_(ULONG32,AddRef)	(THIS);

    STDMETHOD_(ULONG32,Release)	(THIS);

    //IHXTokenBufferFilter interface
    STDMETHOD(Init)             (THIS_
                                 IUnknown* pContext);

    STDMETHOD(AddTokens)        (THIS_
                                 UINT32 ulTokens);

    STDMETHOD(RemoveTokens)     (THIS_
                                 UINT32 ulTokens);

    STDMETHOD_(INT32,GetTokenCount) (THIS);

    STDMETHOD(SetMinTokenCeiling) (THIS_
                                 UINT32 ulMinCeiling);

    STDMETHOD(SetBandwidth)     (THIS_
                                 UINT32 bandwidth);

    STDMETHOD(UpdateTokens)     (THIS);

private:

    INT32 m_lTokens;
    INT32 m_lCeiling;
    UINT32 m_ulMinCeiling;
    UINT32 m_ulRefCount;
    UINT32 m_ulBandwidth;
    UINT32 m_LastTimeStamp;
    IHXScheduler* m_pScheduler;
};

#endif //SERVERTBF_H
