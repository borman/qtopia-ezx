/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: hxpsink.h,v 1.2 2004/07/09 18:42:47 hubbe Exp $
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

#ifndef _HXPSINK_H_
#define _HXPSINK_H_

/*
 * Forward declarations of some interfaces defined here-in.
 */
typedef _INTERFACE       IHXPlayer		    IHXPlayer;
typedef _INTERFACE       IHXPlayerSinkControl	    IHXPlayerSinkControl;
typedef _INTERFACE       IHXPlayerSinkControl	    IHXPlayerSinkControl;


/****************************************************************************
 * 
 *  Interface:
 * 
 *	IHXPlayerCreationSink
 * 
 *  Purpose:
 * 
 *	PlayerCreation Sink Interface
 * 
 *  IID_IHXPlayerCreationSink:
 * 
 *	{00002100-0901-11d1-8B06-00A024406D59}
 * 
 */
DEFINE_GUID(IID_IHXPlayerCreationSink,	0x00002100, 0x901, 0x11d1, 0x8b, 
				0x6, 0x0, 0xa0, 0x24, 0x40, 0x6d, 0x59);
#undef  INTERFACE
#define INTERFACE   IHXPlayerCreationSink

DECLARE_INTERFACE_(IHXPlayerCreationSink, IUnknown)
{
    /*
     * IUnknown methods
     */
    STDMETHOD(QueryInterface)	(THIS_
				REFIID riid,
				void** ppvObj) PURE;

    STDMETHOD_(ULONG32,AddRef)	(THIS) PURE;

    STDMETHOD_(ULONG32,Release)	(THIS) PURE;

    /*
     * IHXPlayerCreationSink Methods
     */

    /************************************************************************
     *	Method:
     *	    IHXPlayerCreationSink::PlayerCreated
     *	Purpose:
     *	    Notification when a new player is created
     *
     */
    STDMETHOD(PlayerCreated)		(THIS_
					IHXPlayer* pPlayer) PURE;
    
    /************************************************************************
     *	Method:
     *	    IHXPlayerCreationSink::PlayerClosed
     *	Purpose:
     *	    Notification when an exisitng player is closed
     *
     */
    STDMETHOD(PlayerClosed)		(THIS_
					IHXPlayer* pPlayer) PURE;
};


/****************************************************************************
 * 
 *  Interface:
 * 
 *	IHXPlayerSinkControl
 * 
 *  Purpose:
 * 
 *	Player SinkControl Interface
 * 
 *  IID_IHXPlayerSinkControl:
 * 
 *	{00002101-0901-11d1-8B06-00A024406D59}
 * 
 */

DEFINE_GUID(IID_IHXPlayerSinkControl, 0x00002101, 0x901, 0x11d1, 0x8b, 
				    0x6, 0x0, 0xa0, 0x24, 0x40, 0x6d, 0x59);
#undef  INTERFACE
#define INTERFACE   IHXPlayerSinkControl

DECLARE_INTERFACE_(IHXPlayerSinkControl, IUnknown)
{
    /*
     * IUnknown methods
     */
    STDMETHOD(QueryInterface)	(THIS_
				REFIID riid,
				void** ppvObj) PURE;

    STDMETHOD_(ULONG32,AddRef)	(THIS) PURE;

    STDMETHOD_(ULONG32,Release)	(THIS) PURE;

    /*
     * IHXPlayerSinkControl Methods
     */
    
    /************************************************************************
     *	Method:
     *	    IHXPlayerSinkControl::AddSink
     *	Purpose:
     *	    Add a new sink
     *
     */
    STDMETHOD(AddSink)			(THIS_
					IHXPlayerCreationSink* pSink) PURE;
    
    /************************************************************************
     *	Method:
     *	    IHXPlayerSinkControl::RemoveSink
     *	Purpose:
     *	    Remove an exisitng sink
     *
     */
    STDMETHOD(RemoveSink)		(THIS_
					IHXPlayerCreationSink* pSink) PURE;
};

#endif /* _HXPSINK_H_ */
