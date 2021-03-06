/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: hxrsdbf.h,v 1.3 2004/10/20 17:43:35 acolwell Exp $
 * 
 * Portions Copyright (c) 1995-2004 RealNetworks, Inc. All Rights Reserved.
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

#ifndef _HXRSDBF_H_
#define _HXRSDBF_H_

typedef _INTERFACE	IHXResendBufferControl	    IHXResendBufferControl;

/****************************************************************************
 * 
 *  Interface:
 *
 *	IHXResendBufferControl
 *
 *  Purpose:
 *
 *	Allows management of the transport resend buffer
 *
 *  IID_IHXResendBufferControl:
 *
 *	{00002B00-b4c8-11d0-9995-00a0248da5f0}
 *
 */
DEFINE_GUID(IID_IHXResendBufferControl, 0x00002B00, 0xb4c8, 0x11d0,
	    0x99, 0x95, 0x0, 0xa0, 0x24, 0x8d, 0xa5, 0xf0);

#undef  INTERFACE
#define INTERFACE   IHXResendBufferControl

DECLARE_INTERFACE_(IHXResendBufferControl, IUnknown)
{
    /*
     *	IUnknown methods
     */
    STDMETHOD(QueryInterface)	(THIS_
				REFIID riid,
				void** ppvObj) PURE;

    STDMETHOD_(ULONG32,AddRef)	(THIS) PURE;

    STDMETHOD_(ULONG32,Release)	(THIS) PURE;

    /*
     *	IHXResendBufferControl methods
     */

    STDMETHOD(SetResendBufferDepth)	(THIS_
					UINT32 uSeconds) PURE;
};

typedef _INTERFACE	IHXResendBufferControl2	    IHXResendBufferControl2;

/****************************************************************************
 * 
 *  Interface:
 *
 *	IHXResendBufferControl2
 *
 *  Purpose:
 *
 *	Allows management of the transport resend buffer
 *
 *  IID_IHXResendBufferControl2:
 *
 *	{D3103F1E-738F-4161-9B39-1DE7BC60E0E3}
 *
 */
DEFINE_GUID(IID_IHXResendBufferControl2, 0xd3103f1e, 0x738f, 0x4161, 
            0x9b, 0x39, 0x1d, 0xe7, 0xbc, 0x60, 0xe0, 0xe3);

#undef  INTERFACE
#define INTERFACE   IHXResendBufferControl2

DECLARE_INTERFACE_(IHXResendBufferControl2, IUnknown)
{
    /*
     *	IUnknown methods
     */
    STDMETHOD(QueryInterface)	(THIS_
				REFIID riid,
				void** ppvObj) PURE;

    STDMETHOD_(ULONG32,AddRef)	(THIS) PURE;

    STDMETHOD_(ULONG32,Release)	(THIS) PURE;

    /*
     *	IHXResendBufferControl2 methods
     */

    STDMETHOD(SetResendBufferParameters) (THIS_
                                          UINT32 uMinimumDelay,/* ms */
                                          UINT32 uMaximumDelay, /* ms */
                                          UINT32 uExtraBufferingDelay/* ms */) PURE;
};

#endif /* _HXRSDBF_H_ */
