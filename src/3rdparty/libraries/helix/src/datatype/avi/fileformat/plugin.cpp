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

#include "hxtypes.h"

#define INITGUID
#include "hxcom.h"
#include "hxbuffer.h"
#include "hxplugn.h"
#include "hxcomm.h"
#include "hxerror.h"
#include "hxfiles.h"
#include "hxformt.h"
#include "hxpends.h"
#include "hxcore.h"
#include "ihxfgbuf.h"
#include "ihxtlogsystem.h"
#include "ihxtlogsystemcontext.h"
#include "hxdllaccess.h"
#include "baseobj.h"

#include "riff.h"
#include "riffres.h"
#include "fragment.h"
#include "chxxtype.h"

#include "aviffpln.h"

#ifdef _AIX
#include "dllpath.h"
ENABLE_MULTILOAD_DLLACCESS_PATHS(Avifformat);
#endif

STDAPI ENTRYPOINTCALLTYPE ENTRYPOINT(HXCREATEINSTANCE)(IUnknown** ppIUnknown)
{
    return CAVIFileFormat::HXCreateInstance(ppIUnknown);
}

STDAPI ENTRYPOINTCALLTYPE ENTRYPOINT(CanUnload2)(void)
{
    return (CHXBaseCountingObject::ObjectsActive() > 0 ? HXR_FAIL : HXR_OK);
}

STDAPI ENTRYPOINTCALLTYPE ENTRYPOINT(CanUnload)(void)
{
    return ENTRYPOINT(CanUnload2)();
}
