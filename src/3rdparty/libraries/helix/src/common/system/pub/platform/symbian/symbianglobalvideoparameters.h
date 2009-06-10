/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: symbianglobalvideoparameters.h,v 1.8 2009/03/30 20:39:53 praveenkumar Exp $
 *
 * Copyright Notices:
 *
 * Portions Copyright (c) 1995-2006 RealNetworks, Inc. All Rights Reserved.
 *
 * Patent Notices: This file may contain technology protected by one or
 * more of the patents listed at www.helixcommunity.org
 *
 * 1.   The contents of this file, and the files included with this file,
 * are protected by copyright controlled by RealNetworks and its
 * licensors, and made available by RealNetworks subject to the current
 * version of the RealNetworks Public Source License (the "RPSL")
 * available at  http://www.helixcommunity.org/content/rpsl unless
 * you have licensed the file under the current version of the
 * RealNetworks Community Source License (the "RCSL") available at
 * http://www.helixcommunity.org/content/rcsl, in which case the RCSL
 * will apply.  You may also obtain the license terms directly from
 * RealNetworks.  You may not use this file except in compliance with
 * the RPSL or, if you have a valid RCSL with RealNetworks applicable
 * to this file, the RCSL.  Please see the applicable RPSL or RCSL for
 * the rights, obligations and limitations governing use of the
 * contents of the file.
 *
 * 2.  Alternatively, the contents of this file may be used under the
 * terms of the GNU General Public License Version 2 (the
 * "GPL") in which case the provisions of the GPL are applicable
 * instead of those above.  Please note that RealNetworks and its
 * licensors disclaim any implied patent license under the GPL.
 * If you wish to allow use of your version of this file only under
 * the terms of the GPL, and not to allow others
 * to use your version of this file under the terms of either the RPSL
 * or RCSL, indicate your decision by deleting Paragraph 1 above
 * and replace them with the notice and other provisions required by
 * the GPL. If you do not delete Paragraph 1 above, a recipient may
 * use your version of this file under the terms of any one of the
 * RPSL, the RCSL or the GPL.
 *
 * This file is part of the Helix DNA Technology.  RealNetworks is the
 * developer of the Original Code and owns the copyrights in the
 * portions it created.   Copying, including reproducing, storing,
 * adapting or translating, any or all of this material other than
 * pursuant to the license terms referred to above requires the prior
 * written consent of RealNetworks and its licensors
 *
 * This file, and the files included with this file, is distributed
 * and made available by RealNetworks on an 'AS IS' basis, WITHOUT
 * WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, AND REALNETWORKS
 * AND ITS LICENSORS HEREBY DISCLAIM  ALL SUCH WARRANTIES, INCLUDING
 * WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 *
 * Technology Compatibility Kit Test Suite(s) Location:
 *    http://www.helixcommunity.org/content/tck
 *
 * Contributors: Nokia Inc
 *
 *
 * ***** END LICENSE BLOCK ***** */

#ifndef SYMBIAN_VIDEO_PARAMETERS_H
#define SYMBIAN_VIDEO_PARAMETERS_H

#include "globals/hxglobals.h"
#include "hxwintyp.h"           //HXxEvent
#include "hxwin.h"              //IHXSite
#include "symbian_gm_inst.h"
#include "MMFScalingCustomCommandConstants.h"
#include "platform.h"



//This class is used to store Symbian video parameters in the global manager.
//This is used by the Symbian MDF & MMF. This is needed beacuse inital video parameters
//are sent down to the MMF before the MDF has been created. The MDF uses these values in
//it's initial setup
class SymbianGlobalVideoParameters
{
private:

    SymbianGlobalVideoParameters();

public:

    static SymbianGlobalVideoParameters*  Instance();
    ~SymbianGlobalVideoParameters();

    HX_RESULT    GetWindowParameters(HXxRect &clipRect, HXxRect &windowRect);
    HX_RESULT    SetWindowParameters(HXxRect clipRect, HXxRect windowRect);
    HX_RESULT    GetDisplayRegion(HXxRegion aRegion);
    HX_RESULT    SetDisplayRegion(HXxRegion Region);
    HX_RESULT    GetScalingParameters(HXFLOAT &fWidthPerc, HXFLOAT &fHeightPerc, HXBOOL &bAntiAlias);
    HX_RESULT    SetScalingParameters(HXFLOAT fWidthPerc, HXFLOAT fHeightPerc, HXBOOL bAntiAlias);
    HX_RESULT    GetDSAStatus(HXBOOL &bValue);
    HX_RESULT    SetDSAStatus(HXBOOL bValue);
#ifdef SYMBIAN_ENABLE_MMF_MULTISCREEN_SUPPORT
    HXBOOL       GetInitScreenNumber(INT32& lScreenNumber) const;
    HX_RESULT    SetInitScreenNumber(INT32 lScreenNumber);
    HX_RESULT    ClearInitScreenNumber();
#endif  //SYMBIAN_ENABLE_MMF_MULTISCREEN_SUPPORT
    HX_RESULT    GetRotationValue(UINT32 &ulValue);
    HX_RESULT    SetRotationValue(UINT32 ulValue);
    HX_RESULT    GetFrameRect(HXxRect &sValue);
    HX_RESULT    SetFrameRect(HXxRect sValue);
    HXBOOL       IsDisplayRegionValid(void);
    HXBOOL       IsFrameRectValid(void);
    HX_RESULT    GetScalingTypeValue(UINT32 &ulValue);
    HX_RESULT    SetScalingTypeValue(UINT32 ulValue);
#if defined(HELIX_FEATURE_SYMBIAN_GRAPHICS_SURFACES)
    HX_RESULT	 SetUseSurfaces();
    HXBOOL		 GetUseSurfaces();

#endif //HELIX_FEATURE_SYMBIAN_GRAPHICS_SURFACES

private:
    HXxRect               m_clipRect;
    HXxRect               m_windowRect;
    HXxRect               m_frameRect;
    HXBOOL                m_bFrameRectValid;
    HXBOOL                m_bAntiAlias;
    RRegion               m_ClipRegion;
    ULONG32               m_rotationValue;
    HXFLOAT               m_fHeightPerc;
    HXFLOAT               m_fWidthPerc;
    HXBOOL                m_bDSAStatus;
    HXBOOL                m_bValidDisplayRegion;
#ifdef SYMBIAN_ENABLE_MMF_MULTISCREEN_SUPPORT
    INT32                 m_lScreenNumber;
    HXBOOL                m_bScreenNumberSet;
#endif  //SYMBIAN_ENABLE_MMF_MULTISCREEN_SUPPORT
    UINT32                m_ulScalingTypeValue;
#if defined(HELIX_FEATURE_SYMBIAN_GRAPHICS_SURFACES)
    HXBOOL 				  m_bUseSurfaces;
	INT32                 m_ulDisplayIDArray[10]; //This will be removed, dont worry about magic number :) or use vector
	INT32				  m_ulDisplayIDArrayIndex;
#endif //HELIX_FEATURE_SYMBIAN_GRAPHICS_SURFACES
};

#endif SYMBIAN_VIDEO_PARAMETERS_H

