/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: hxshtdn.h,v 1.3 2004/07/09 18:20:48 hubbe Exp $
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

#ifndef _HXSHUT_H_
#define _HXSHUT_H_

/*
 * Forward declarations of some interfaces defined or used here-in.
 */
typedef _INTERFACE	IUnknown		    IUnknown;
typedef _INTERFACE	IHXShutDownEverything	    IHXShutDownEverything;

/****************************************************************************
 * 
 *  Interface:
 *
 *	IHXShutDownEverything
 *
 *  Purpose:
 *
 *	Interface used by the plugin handler to shut down all the plugins
 *	before calling Refresh().
 *
 *  IID_IHXShutDownEverything:
 *
 *	{00000C00-b4c8-11d0-9995-00a0248da5f0}
 *
 */
DEFINE_GUID(IID_IHXShutDownEverything, 0x00000C00, 0xb4c8, 0x11d0, 0x99, 
				0x95, 0x0, 0xa0, 0x24, 0x8d, 0xa5, 0xf0);

#undef  INTERFACE
#define INTERFACE   IHXShutDownEverything

DECLARE_INTERFACE_(IHXShutDownEverything, IUnknown)
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
     * IHXShutDownEverything methods
     */

    /************************************************************************
     *	Method:
     *	    IHXShutDownEverything::ShutDown
     *	Purpose:
     *	    Shutdown all the plugins.
     *
     */
    STDMETHOD(ShutDown)		(THIS) PURE;

    /************************************************************************
     *	Method:
     *	    IHXShutDownEverything::StopAllOtherPlayers
     *	Purpose:
     *	    Stop all other players in other processes that use audio.
     *
     */
    STDMETHOD(StopAllOtherPlayers)  (THIS) PURE;

    /************************************************************************
     *	Method:
     *	    IHXShutDownEverything::AskAllOtherPlayersToUnload
     *	Purpose:
     *	    Ask all other players in other processes to unload their 
     *	    unused DLLs.
     */
    STDMETHOD(AskAllOtherPlayersToUnload)  (THIS) PURE;

    /************************************************************************
     *	Method:
     *	    IHXShutDownEverything::AskAllOtherPlayersToReload
     *	Purpose:
     *	    Ask all other players in other processes to reload their 
     *	    DLLs.
     */
    STDMETHOD(AskAllOtherPlayersToReload)  (THIS) PURE;

};
#endif /*_HXSHUT_H_*/
