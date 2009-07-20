/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: rdt_tcp.h,v 1.4 2007/01/06 00:12:04 seansmith Exp $
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

#ifndef _RDT_TCP_H_
#define _RDT_TCP_H_

#include "hxcom.h"
#include "hxcomptr.h"

#include "rdt_base.h"   // For RDTBaseTransport
#include "hxengin.h"    // For IHXWouldBlock

_INTERFACE IHXBuffer;
_INTERFACE IHXSocket;
_INTERFACE IHXServerPacketSource;
_INTERFACE IHXQoSSignalBus;
_INTERFACE IHXRTSPTransportResponse;
_INTERFACE IHXWouldBlockResponse;
_INTERFACE IHXBufferedSocket;

class RDTTransportInfoRequestPacket;
class TNGLatencyReportPacket;
class RDTTransportInfoRequestPacket;
class BasePacket;
class ServerPacket;

#define MAX_BLOCK_QUEUE_SZ 64

class RDTTCPTransport: public RDTBaseTransport,
                       public IHXWouldBlock
{
public:
    RDTTCPTransport                     (BOOL bIsSource,
                                         BOOL bNoLostPackets = FALSE,
                                         UINT32 uFeatureLevel = 0);
    virtual ~RDTTCPTransport            (void);

    STDMETHOD(QueryInterface)           (THIS_ REFIID riid, void** ppvObj);
    STDMETHOD_(ULONG32,AddRef)		(THIS);
    STDMETHOD_(ULONG32,Release)		(THIS);

    // RTSPTransport
    STDMETHOD_(void, Done)              (THIS);
    virtual HX_RESULT sendPacket        (BasePacket* pPacket);
    virtual HX_RESULT handlePacket      (IHXBuffer* pBuffer);
    virtual HX_RESULT streamDone        (UINT16 streamNumber,
                                         UINT32 uReasonCode = 0,
                                         const char* pReasonText = NULL);

    /* IHXQoSSignalSourceResponse */
    STDMETHOD (SignalBusReady)(THIS_ HX_RESULT hResult,
                                IHXQoSSignalBus* pBus,
                               IHXBuffer* pSessionId);


    HX_RESULT init                      (IUnknown* pContext,
                                        IHXSocket* pSocket,
                                        INT8 interleave,
                                        IHXRTSPTransportResponse* pResp);
    HX_RESULT sendPackets               (BasePacket** pPacket);
    HX_RESULT sendPacket                (ServerPacket* pPacket);
    HX_RESULT sendPackets               (ServerPacket** pPacket);
    virtual HX_RESULT sendBWProbingPackets(UINT32 ulCount, UINT32 ulSize, REF(INT32) lSeqNo);
    virtual HX_RESULT startPackets      (UINT16 uStreamNumber);
    virtual HX_RESULT stopPackets       (UINT16 uStreamNumber);

    /* IHXServerPacketSink */
    STDMETHOD(PacketReady) (ServerPacket* pPacket);
    STDMETHOD(SetSource) (IHXServerPacketSource* pSource);
    STDMETHOD(SourceDone) (void);

    /* IHXWouldBlock */
    STDMETHOD(WantWouldBlock)   (THIS_
                                IHXWouldBlockResponse* pResp, UINT32 ulId);

    /* IHXSocketResponse */
    STDMETHOD(EventPending)             (THIS_ UINT32 uEvent, HX_RESULT status);

    STDMETHOD(SetDeliveryBandwidth)(UINT32 ulBackOff, UINT32 ulBandwidth);
    STDMETHOD(SetAggregationLimits) (THIS_   UINT32* pAggregStat,
                                     UINT32 ulAggregateTo, UINT32 ulAggregateHighest);

    void setTransportInterleave         (INT8 interleave);

private:
    BOOL createInfoRequestPacket(RDTTransportInfoRequestPacket* pReq);

    HX_RESULT writePacket               (IHXBuffer* pSendBuffer);

    // Template functions to make common code that differs only in the packet
    // pointer type (BasePacket v. ServerPacket).

    template<typename PacketT>
    HX_RESULT sendPacketImpl            (PacketT* pPacket);

    template<typename PacketT>
    HX_RESULT sendPacketsImpl           (PacketT** pPacket);

    INT8                                m_tcpInterleave;
    BOOL                                m_bNoPacketBuffering;
    BOOL                                m_bPacketsStarted;

    BOOL                                m_bNoLostPackets;

    IHXWouldBlockResponse*              m_pWouldBlockResponse;

    /* Media Delivery Pipeline: */
    UINT32                              m_ulPktsPerBufferRequest;
    UINT32                              m_ulPktCount;
    BOOL                                m_bBlocked;

    /* Block Marker Queue */
    UINT16                              m_unBlockQueueSz;

    UINT16                              m_unBlockQueueWrite;
    UINT16                              m_unBlockQueueRead;
    INT16*                              m_pBlockQueue;

    BOOL*                               m_pBlockMarker;

    friend class CUTRDTTCPTransportTestDriver;    // For unit testing.
};

#endif /* ndef _RDT_TCP_H_ */