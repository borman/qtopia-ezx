/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: ptr_array_test.h,v 1.5 2007/07/06 20:35:03 jfinnecy Exp $
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
 * terms of the GNU General Public License Version 2 (the
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

#ifndef PTR_ARRAY_TEST_H
#define PTR_ARRAY_TEST_H

#include "hx_cmd_based_test.h"

#include "carray.h"

class HLXPtrArrayTest : public HLXCmdBasedTest
{
public:
    HLXPtrArrayTest();

    virtual const char* DefaultCommandLine() const;
    virtual void GetCommandInfo(UTVector<HLXUnitTestCmdInfo*>& cmds);
    virtual HLXCmdBasedTest* Clone() const;

protected:
    bool HLXPtrArrayTest::HandleIsEmptyCmd(const UTVector<UTString>& info);
    bool HLXPtrArrayTest::HandleGetSizeCmd(const UTVector<UTString>& info);
    bool HLXPtrArrayTest::HandleGetUpperBoundCmd(const UTVector<UTString>& info);
    bool HLXPtrArrayTest::HandleSetSizeCmd(const UTVector<UTString>& info);
    bool HLXPtrArrayTest::HandleFreeExtraCmd(const UTVector<UTString>& info);
    bool HLXPtrArrayTest::HandleRemoveAllCmd(const UTVector<UTString>& info);
    bool HLXPtrArrayTest::HandleGetAtCmd(const UTVector<UTString>& info);
    bool HLXPtrArrayTest::HandleSetAtCmd(const UTVector<UTString>& info);
    bool HLXPtrArrayTest::HandleElementAtCmd(const UTVector<UTString>& info);
    bool HLXPtrArrayTest::HandleSetAtGrowCmd(const UTVector<UTString>& info);
    bool HLXPtrArrayTest::HandleAddCmd(const UTVector<UTString>& info);
    bool HLXPtrArrayTest::HandleArrayOpCmd(const UTVector<UTString>& info);
    bool HLXPtrArrayTest::HandleInsertAtCmd(const UTVector<UTString>& info);
    bool HLXPtrArrayTest::HandleRemoveAtCmd(const UTVector<UTString>& info);
    bool HLXPtrArrayTest::HandleInsertArrayAtCmd(const UTVector<UTString>& info);
    bool HLXPtrArrayTest::HandleIsNullCmd(const UTVector<UTString>& info);

private:
    CHXPtrArray m_array;
};

#endif // PTR_ARRAY_TEST_H
