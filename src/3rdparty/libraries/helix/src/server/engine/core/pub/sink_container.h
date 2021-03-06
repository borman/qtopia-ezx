/* ***** BEGIN LICENSE BLOCK *****  
 * Source last modified: $Id: sink_container.h,v 1.3 2003/09/04 22:39:08 dcollins Exp $ 
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

#ifndef _SINK_CONTAINER_H_
#define _SINK_CONTAINER_H_

#include "hxcom.h"
#include "ihxpckts.h"
#include "hxfiles.h"
#include "hxsrc.h"
#include "sink.h"

class SinkContainer : public IHXPSinkControl,
                      public IHXPSinkPackets
{
public:
    SinkContainer(IHXRawSinkObject *pSink);
    ~SinkContainer();

	/* IHXUnknown methods */

    STDMETHOD(QueryInterface)		(THIS_ REFIID riid, void** ppvObj);

    STDMETHOD_(ULONG32,AddRef)		(THIS);

    STDMETHOD_(ULONG32,Release)		(THIS);

	/* IHXPSinkControl methods */

    STDMETHOD(InitDone)			(HX_RESULT status);

    STDMETHOD(FileHeaderReady)		(HX_RESULT status,
					IHXValues* pHeader);

    STDMETHOD(StreamHeaderReady)	(HX_RESULT status,
					IHXValues* pHeader);

    STDMETHOD(StreamDone)		(UINT16 unStreamNumber);

    STDMETHOD(SeekDone)			(HX_RESULT status);

	/* IHXPSinkPackets methods */

    STDMETHOD(PacketReady)		(HX_RESULT status,
					IHXPacket* pPacket);

    void Done				();

private:

    LONG32			m_lRefCount;
    IHXRawSinkObject*		m_pSink;
};

#endif // _SINK_CONTAINER_H_
