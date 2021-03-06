/* ***** BEGIN LICENSE BLOCK *****  
 * Source last modified: $Id: sysinfo.h,v 1.6 2005/04/20 14:05:31 dcollins Exp $ 
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
#ifndef _SYSINFO_H_
#define _SYSINFO_H_

#ifdef _SOLARIS
#include <kstat.h>
#endif

#ifdef _WINDOWS
// We must 8-byte-pack these declarations because some of the structures
// (for example, PDH_FMT_COUNTERVALUE) are assumed to be 8-byte-packed
// by the Windows PDH library - DPS
#pragma pack(8)
#include <pdh.h>
#pragma pack()

// Work with all versions of pdh.dll
#ifdef PdhOpenQuery
#undef PdhOpenQuery
extern "C" long __stdcall 
PdhOpenQuery (LPCSTR  szD, DWORD  dw, HQUERY  *phQ);
#endif /* PdhOpenQuery */
#endif /* _WINDOWS */

class CSysInfo
{
public:
    CSysInfo();
    virtual ~CSysInfo();

    virtual UINT32 GetNumberofCPU       (void);
    virtual HX_RESULT GetCPUUsage       (REF(INT32) ulUserUsage, REF(INT32) ulKernUsage, REF(INT32) ulAggUsage);
    virtual HX_RESULT GetMemUsage       (REF(UINT32) ulMemUsage);
    virtual HX_RESULT InitCPUCalc       (void);

protected:
    virtual HX_RESULT InitProcUsage     (void);
    virtual HX_RESULT GetTotalProcUsage (UINT32* plUserUsage, UINT32* plKernUsage);
    virtual HX_RESULT GetProcUsageById  (UINT32 id, UINT32* plUserUsage, UINT32* plKernUsage);
    virtual HX_RESULT GetTotalCPUUsage  (UINT32* plUsage);
    char* SkipToken (const char* p);

    struct timeval      m_StartTime;
    UINT32              m_ulLastUserCPUUsage;
    UINT32              m_ulLastKernCPUUsage;
    UINT32              m_ulLastAggCPUUsage;
};


#if defined(_WINDOWS)
class CWindowsSysInfo : public CSysInfo
{
public:
    CWindowsSysInfo();
    ~CWindowsSysInfo();

    HX_RESULT GetCPUUsage       (REF(INT32) ulUserUsage, REF(INT32) ulKernUsage, REF(INT32) ulAggUsage);
    UINT32 GetNumberofCPU       (void);
    HX_RESULT InitCPUCalc       (void);
    HX_RESULT GetMemUsage       (REF(UINT32) ulMemUsage);

private:
    HX_RESULT ClosePerformanceQuery (void);

    HQUERY	        m_hPerfCPUUsageQuery;
    HQUERY              m_hPerfMemUsageQuery;
    HCOUNTER	        m_pUserCPUCounter;
    HCOUNTER	        m_pKernCPUCounter;
    HCOUNTER            m_pAggCPUCounter;
    HCOUNTER            m_pMemCounter;
};


#elif defined(_SOLARIS)
class CSolarisSysInfo : public CSysInfo
{
public:
    CSolarisSysInfo();
    ~CSolarisSysInfo();

    UINT32 GetNumberofCPU       (void);
    HX_RESULT InitCPUCalc       (void);

private:
    HX_RESULT GetTotalProcUsage (UINT32* plUserUsage, UINT32* plKernUsage);
    HX_RESULT GetProcUsageById  (UINT32 id, UINT32* plUserUsage, UINT32* plKernUsage);
    HX_RESULT GetTotalCPUUsage  (UINT32* plUsage);

    kstat_ctl_t*        m_pkc;
};


#elif defined(_LINUX)
class CLinuxSysInfo : public CSysInfo
{
public:
    CLinuxSysInfo();
    ~CLinuxSysInfo();

    UINT32 GetNumberofCPU       (void);

private:
    HX_RESULT GetTotalProcUsage (UINT32* plUserUsage, UINT32* plKernUsage);
    HX_RESULT GetProcUsageById  (UINT32 id, UINT32* plUserUsage, UINT32* plKernUsage);
    HX_RESULT GetTotalCPUUsage  (UINT32* plUsage);
};


#elif defined(_FREEBSD) || defined(_OPENBSD) || defined(_NETBSD)
class CBsdSysInfo : public CSysInfo
{
public:
    CBsdSysInfo();
    ~CBsdSysInfo();

private:
    HX_RESULT GetTotalProcUsage (UINT32* plUserUsage, ULONG32* plKernUsage);
    HX_RESULT GetProcUsageById  (UINT32 id, UINT32* plUserUsage, ULONG32* plKernUsage);
    HX_RESULT GetTotalCPUUsage  (UINT32* pUsage);
};


#elif defined (_HPUX)
class CHPSysInfo : public CSysInfo
{
public:
    CHPSysInfo();
    ~CHPSysInfo();
 
    UINT32 GetNumberofCPU       (void);
    HX_RESULT GetCPUUsage       (REF(INT32) ulServerUsage, REF(INT32) ulAggUsage);

private:
    HX_RESULT GetProcUsageById  (UINT32 id, UINT32* plUserUsage, ULONG32* plKernUsage);
    HX_RESULT GetTotalCPUUsage  (UINT32* plUsage);
    HX_RESULT GetProcUsage      (UINT32 id, float* pfUsage);
};


#elif defined(_AIX)
class CAIXSysInfo : public CSysInfo
{
public:
    CHPSysInfo();
    ~CAIXSysInfo();

    UINT32 GetNumberofCPU (void);


private:
    HX_RESULT GetProcUsageById  (UINT32 id, UINT32* plUserUsage, ULONG32* plKernUsage)
    HX_RESULT GetTotalCPUUsage  (UINT32* plUsage)
    HX_RESULT GetKernelValue    (unsigned long ulOffset, caddr_t pAddr, int nSize);

    unsigned long       m_ulSysinfoOffset;
    int                 m_kmemfd;
};


#elif defined(_OSF1)
class COSF1SysInfo: public CSysInfo
{
public:
    COSF1SysInfo();
    ~COSF1SysInfo();

    GetNumberofCPU(void)

private:
    HX_RESULT GetProcUsageById  (UINT32 id, UINT32* plUserUsage, ULONG32* plKernUsage);
    HX_RESULT GetTotalCPUUsage  (UINT32* pulUsage);

};
#endif

#endif //_SYSINFO_H_
