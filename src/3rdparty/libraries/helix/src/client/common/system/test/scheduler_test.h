/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: scheduler_test.h,v 1.3 2004/07/09 18:43:05 hubbe Exp $
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

#ifndef SCHEDULER_TEST_H
#define SCHEDULER_TEST_H

#include "hx_cmd_based_test.h"

#include "hxsched.h"

class TestOperation;

class HXSchedulerTest : public HLXCmdBasedTest
{
public:
    HXSchedulerTest();
    virtual ~HXSchedulerTest();

    virtual const char* DefaultCommandLine() const;
    virtual void GetCommandInfo(UTVector<HLXUnitTestCmdInfo*>& cmds);
    virtual HLXCmdBasedTest* Clone() const;

    void OperationFailed();
    bool OnOperation(UINT32 operationID);

    bool RemoveHandle(CallbackHandle handle);
    void StopScheduler();

    ULONG32 GetMSFromStart();

protected:
    bool HandleResetCmd(const UTVector<UTString>& info);
    bool HandleTimeoutCmd(const UTVector<UTString>& info);
    bool HandleShutdownCmd(const UTVector<UTString>& info);
    bool HandleCancelLastCmd(const UTVector<UTString>& info);
    bool HandleRunSchedulerCmd(const UTVector<UTString>& info);

    bool TestRelativeEnter();
    bool TestRemove();

    CallbackHandle AddTimeoutOp(UINT32 operationID, UINT32 timeout);
    CallbackHandle AddShutdownOp(UINT32 operationID, UINT32 timeout);
    CallbackHandle AddRemoveOp(UINT32 operationID, UINT32 timeout, 
		     CallbackHandle handle);
    
    void AddCallback(unsigned int operationID, unsigned int timeout,
		     TestOperation* pOp);
    void CreateScheduler();
    bool RunScheduler();

    void ProcessEvents(); // platform specific function

private:
    HXScheduler* m_pSched;
    bool m_done;
    bool m_failed;
    UINT32 m_nextOpID;
    UINT32 m_finalOpID;
    UINT32 m_highestTimeout;
    HXTimeval m_startTime;
    CallbackHandle m_lastCallbackHandle;
};
#endif /* SCHEDULER_TEST_H */
