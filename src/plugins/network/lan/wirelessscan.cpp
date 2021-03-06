/****************************************************************************
**
** This file is part of the Qtopia Opensource Edition Package.
**
** Copyright (C) 2008 Trolltech ASA.
**
** Contact: Qt Extended Information (info@qtextended.org)
**
** This file may be used under the terms of the GNU General Public License
** versions 2.0 as published by the Free Software Foundation and appearing
** in the file LICENSE.GPL included in the packaging of this file.
**
** Please review the following information to ensure GNU General Public
** Licensing requirements will be met:
**     http://www.fsf.org/licensing/licenses/info/GPLv2.html.
**
**
****************************************************************************/

#include "wirelessscan.h"

#ifndef NO_WIRELESS_LAN

#include <errno.h>
#include <math.h>
#include <net/ethernet.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <QAction>
#include <QApplication>
#include <QDialog>
#include <QHeaderView>
#include <QLabel>
#include <QListWidget>
#include <QMenu>
#include <QSettings>
#include <QTimer>
#include <QVBoxLayout>
#include <QKeyEvent>

#include <qtopiaapplication.h>
#include <qtopialog.h>
#include <qsoftmenubar.h>
#include <qnetworkdevice.h>


//some wlan driver return "<hidden>" when the essid is hidden other drivers
//just return an empty string. Qtopia assumes that a hidden essid is called "<hidden>"
//change this function if there is yet another way of indicating the essid is hidden.
static QString convertToHidden( const QString& essid )
{
    if ( essid.isEmpty() )
        return QString("<hidden>");
    return essid;
}

WirelessScan::WirelessScan( const QString& ifaceName, QObject* parent )
    : QObject( parent ), iface( ifaceName ), sockfd( -1 )
{
}

WirelessScan::~WirelessScan()
{
}

/*!
    This function obtains the range information of the wireless interface. The value of \a weVersion
    must be checked before using \a range. \a weVersion is 0 if the device is not a wireless device.


    This function
    returns 0 if the interface associated to this object is not a wireless interface. A
    return value >0 is equal to \a weVersion.

    Wireless extensions versioning was introduced by WE v11. If the driver for the wireless
    network device doesn't support versioning ( WE <= 10 ) we do a lucky guess and assume version 9.

    The structure of iw_range has been reshuffled and increased in WE version 16. For
    simplicity this function does not return a valid range parameter if  \a weVersion < 16.
    You must check \a weVersion before \a range is used.
*/
void WirelessScan::rangeInfo(iw_range* range, int* weVersion) const
{
    *weVersion = 0;
    int socket = ::socket( AF_INET, SOCK_DGRAM, 0 );
    if ( socket < 0 )
        return;

    char buffer[sizeof(struct iw_range) * 2];
    memset( buffer, 0, sizeof(buffer) );

    struct iwreq wrq;
    wrq.u.data.flags = 0;
    wrq.u.data.pointer = 0;
    wrq.u.data.length = sizeof(buffer);
    wrq.u.data.pointer = buffer;
    strncpy( wrq.ifr_name, iface.toLatin1().constData(), IFNAMSIZ );

    if ( ioctl( socket, SIOCGIWRANGE, &wrq ) < 0 ) {
        //no Wireless Extension
        ::close( socket );
        return;
    }

    memcpy( range, buffer, sizeof(struct iw_range) );

    // struct iw_range changed but we_version_compiled is still at the same offset. Hence
    // we can access the version no matter what WE version we have
    if ( wrq.u.data.length >= 300 ) {
        *weVersion = range->we_version_compiled;
    } else {
        //everything up to version 10
        ::close( socket );
        *weVersion = 9;
    }

    ::close( socket );
    return;
}

//TODO document WirelessScan::ConnectionState

/*!
    Returns the connectivity state of the wireless interface.
*/
WirelessScan::ConnectionState WirelessScan::deviceState() const
{
    //TODO support for IPv4 only (PF_INET6)
    int inetfd = socket( PF_INET, SOCK_DGRAM, 0 );
    if ( inetfd == -1 )
        return InterfaceUnavailable;

    int flags = 0;
    struct ifreq ifreqst;
    ::strcpy( ifreqst.ifr_name, iface.toLatin1().constData() );
    int ret = ioctl( inetfd, SIOCGIFFLAGS, &ifreqst );
    if ( ret == -1 ) {
        ::close( inetfd );
        return InterfaceUnavailable;
    }

    ::close( inetfd );

    flags = ifreqst.ifr_flags;
    if ( ( flags & IFF_UP ) == IFF_UP  &&
            (flags & IFF_LOOPBACK) != IFF_LOOPBACK &&
            (flags & IFF_BROADCAST) == IFF_BROADCAST ) {
        return Connected;
    }

    return NotConnected;
}

/*!
  Returns the MAC address of the access point we are connected to. If the device is not connected to a
  wireless network the returned string is empty.

  \sa deviceState()
*/
QString WirelessScan::currentAccessPoint() const
{
    QString result;
    if ( deviceState() != Connected )
        return result;

    struct iwreq wrq;

    int fd = socket( AF_INET, SOCK_DGRAM, 0 );
    if ( fd < 0 )
        return result;

    strncpy( wrq.ifr_name, iface.toLatin1().constData(), IFNAMSIZ );
    int retCode = ioctl( fd, SIOCGIWAP, &wrq );
    if ( retCode >= 0 ) {
        struct ether_addr* eth = (struct ether_addr*)(&(wrq.u.ap_addr.sa_data));
                    result.sprintf("%02X:%02X:%02X:%02X:%02X:%02X", eth->ether_addr_octet[0],
                            eth->ether_addr_octet[1], eth->ether_addr_octet[2],
                            eth->ether_addr_octet[3], eth->ether_addr_octet[4],
                            eth->ether_addr_octet[5]);

    }
    ::close( fd );
    return result;
}

/*!
  Returns the ESSID of the access point we are connected to. If the device is not connected to a
  wireless network the returned string is empty.

  \sa deviceState()
*/
QString WirelessScan::currentESSID() const
{
    QString result;
    if ( deviceState() != Connected )
        return result;

    struct iwreq wrq;

    int fd = socket( AF_INET, SOCK_DGRAM, 0 );
    if ( fd < 0 )
        return result;

    char buffer[IW_ESSID_MAX_SIZE+1];

    wrq.u.essid.flags = 0;
    wrq.u.essid.length = IW_ESSID_MAX_SIZE+1;
    wrq.u.essid.pointer = buffer;
    strncpy( wrq.ifr_name, iface.toLatin1().constData(), IFNAMSIZ );
    int retCode = ioctl( fd, SIOCGIWESSID, &wrq );
    if ( retCode >= 0 ) {
        buffer[wrq.u.essid.length] = '\0';
        result = buffer;
    }
    ::close( fd );
    
    //some wlan driver return "<hidden>" when the essid is hidden other drivers
    //just return an empty string. Qtopia assumes that a hidden essid is called "<hidden>"
    result = convertToHidden( result );
    return result;
}

void WirelessScan::ensureScanESSID()
{
    QString currEssid = currentESSID();
    if ( currEssid.isEmpty() )
    {
        //qLog(Network) << "Setting preliminary ESSID for scan";
        char scanEssid[IW_ESSID_MAX_SIZE+1];
        scanEssid[0]='\0';

        struct iwreq rq;
        rq.u.essid.flags = 0;
        rq.u.essid.pointer = (caddr_t) scanEssid;
        rq.u.essid.length = strlen( scanEssid );

        struct iw_range range;
        int weVersion;
        rangeInfo( &range, &weVersion );
        if ( weVersion <= 20 )
            rq.u.essid.length++;

        strncpy( rq.ifr_name, iface.toLatin1().constData(), IFNAMSIZ );
        int fd = socket( AF_INET, SOCK_DGRAM, 0 );
        if ( fd < 0 ) {
            qWarning( "Cannot open socket for set essid: %s", strerror( errno ) );
            return;
        }
        int retCode = ioctl( fd, SIOCSIWESSID, &rq );
        if ( retCode < 0 ) {
            qWarning( "Cannot set essid for scanning: %s %d", strerror( errno ), retCode );
        }

        ::close( fd );
    }
}

const QList<WirelessNetwork> WirelessScan::results() const
{
    return entries;
}

bool WirelessScan::startScanning()
{
    struct iw_range range;
    int weVersion;
    rangeInfo( &range, &weVersion );
    static bool showExtendedLog = true;
    if ( showExtendedLog && weVersion < 14 ) {
        qLog(Network) << "WE version 14+ is required for wireless network scanning on interface" << iface;
        return false;
    }

#if WIRELESS_EXT > 13
    if ( showExtendedLog && qtopiaLogEnabled("Network") ) {
        qLog(Network) << "driver on interface" << iface << "supports WE version" << range.we_version_source;
        qLog(Network) << "compiled with WE version" << range.we_version_compiled;
        showExtendedLog = false;
    }

    if (sockfd > 0) {
        qLog(Network) << "Scanning process active";
        return false; //we are in the process of scanning, dont start yet another scan
    }

    qLog(Network) << "Scanning for wireless networks...";

    //some wlan drivers require an initial set essid before they work
    ensureScanESSID();
    
    //open socket
    sockfd = socket( AF_INET, SOCK_DGRAM, 0 );
    if ( sockfd < 0 )
        return false;

    //initiate scanning for wireless networks
    struct iwreq wrq;
    wrq.u.data.flags = 0;
    wrq.u.data.length = 0;
    wrq.u.data.pointer = 0;
    strncpy( wrq.ifr_name, iface.toLatin1().constData(), IFNAMSIZ );

    if ( ioctl( sockfd, SIOCSIWSCAN, &wrq ) < 0 ) {
        if ( qLogEnabled(Network) )
            perror("wireless scan initiation");
        ::close( sockfd );
        sockfd = -1;
        return false;
    }
    QTimer::singleShot( 300, this, SLOT(checkResults()) );

    return true;
#else
    return false;
#endif
}

void WirelessScan::checkResults()
{
    unsigned char* buffer = 0;
    struct iwreq wrq;

    struct iw_range range;
    int weVersion;
    rangeInfo( &range, &weVersion );
    if ( weVersion < 14 )
        return; //no scan support if WE version <14

#if WIRELESS_EXT > 13
    int blength = IW_SCAN_MAX_DATA;
    do {
        //we may have to allocate more memory later on
        unsigned char* temp = (unsigned char *)realloc( buffer, blength );
        if ( !temp ) {
            if ( buffer )
                free( buffer );
            return;
        }
        buffer = temp;

        wrq.u.data.flags = 0;
        wrq.u.data.length = blength;
        wrq.u.data.pointer = (char*)buffer;
        strncpy( wrq.ifr_name, iface.toLatin1().constData(), IFNAMSIZ );

        if ( ioctl( sockfd, SIOCGIWSCAN, &wrq ) < 0 ) {
            if ( errno == EAGAIN ) {
                // still busy scanning => try again later
                QTimer::singleShot( 200, this, SLOT(checkResults()) );
                free(buffer);
                return;
            } else if ( (weVersion > 16) && (errno == E2BIG) ) {
                //scan results can be quite large
                if ( wrq.u.data.length > blength )
                    blength = wrq.u.data.length;
                else
                    blength = blength + IW_SCAN_MAX_DATA;
                continue;
            } else {
                //hmm don't really know what this could be
                perror( "error:" );
                free( buffer );
                ::close( sockfd );
                sockfd = -1;
                return;
            }
        }
        break;
    } while ( true );

    if ( wrq.u.data.length > 0 ) {
        readData( buffer, wrq.u.data.length, weVersion, &range );
    }

    free( buffer );
    ::close( sockfd );
    sockfd = -1;
    emit scanningFinished();
#else
    Q_UNUSED(buffer);
    Q_UNUSED(wrq);
#endif
}

static const char* operationMode[] =
{
        "Auto",
        "Ad-hoc",
        "Managed",
        "Master",
        "Repeater",
        "Secondary",
        "Monitor"
};

//compatibility with WE 19
#define IW_EV_POINT_OFF (((char *) &(((struct iw_point *) NULL)->length)) - (char *) NULL)

//compatibility with WE 16 and below
//these defines were added in WE 17
#ifndef IW_QUAL_QUAL_INVALID
#define IW_QUAL_QUAL_INVALID    0x10
#endif
#ifndef IW_QUAL_LEVEL_INVALID
#define IW_QUAL_LEVEL_INVALID   0x20
#endif
#ifndef IW_QUAL_NOISE_INVALID
#define IW_QUAL_NOISE_INVALID   0x40
#endif

/*!
  Returns the signal quality as a percentage of the maximum quality.
  This function returns -1 if the signal strength cannot be determined.
  */
int WirelessScan::currentSignalStrength() const
{
#if WIRELESS_EXT > 11
    struct iwreq rq;
    struct iw_statistics iwstats;
    
    rq.u.essid.flags = 0;
    rq.u.essid.pointer = (caddr_t) (&iwstats);
    rq.u.essid.length = sizeof( struct iw_statistics );
    strncpy( rq.ifr_name, iface.toLatin1().constData(), IFNAMSIZ );

    int fd = socket( AF_INET, SOCK_DGRAM, 0 );
    if ( fd < 0 ) {
        qLog(Network) << "Cannot open signal strength socket" << strerror(errno);
        return -1;
    }

    int retCode = ioctl( fd, SIOCGIWSTATS, &rq );
    if ( retCode < 0 ) {
        qLog(Network) << "Cannot obtain wireless statistics" << strerror(errno);
        ::close( fd );
        return -1;
    }
    
    ::close( fd );
    
    //we need the range to determine the upper boundaries 
    struct iw_range range;
    int weVersion;
    rangeInfo( &range, &weVersion );

    if ( !(iwstats.qual.updated & IW_QUAL_QUAL_INVALID) ) {
        return (int) (iwstats.qual.qual*100/range.max_qual.qual);
    } else {
        return -1;
    }
    
    
#else
    //read from /proc/net/wireless?
    qLog(Network) << "Cannot determine signal quality (need WE version 12 or more)";
    return -1;
#endif
}


void WirelessScan::readData( unsigned char* data, int length, int weVersion, iw_range* range )
{
#if WIRELESS_EXT > 13
    unsigned char* dataEnd = data + length;
    entries.clear();

    WirelessNetwork net;
    while ( data + IW_EV_LCP_LEN <= dataEnd ) {
        struct iw_event iwevent;
        char * iwp = (char*) (&iwevent);

        //copy event header
        memcpy( (char*) iwp, data, IW_EV_LCP_LEN );

        //an empty event?
        if ( iwevent.len <= IW_EV_LCP_LEN ) {
            //current event is too small => ignore it and continue with next
            data += iwevent.len;
            continue;
        }

        unsigned char * value = data + IW_EV_LCP_LEN;
        iwp = (char*)iwp + IW_EV_LCP_LEN;
        switch ( iwevent.cmd ) {
            case  SIOCSIWCOMMIT:            /* Commit pending changes to driver */
                break;
            case  SIOCGIWNAME:              /* get name == wireless protocol */
                memcpy( iwp , value, IW_EV_CHAR_LEN - IW_EV_LCP_LEN);
                net.setData( WirelessNetwork::Protocol, QString( iwevent.u.name ) );
                break;
            case  SIOCSIWNWID:              /* set network id (pre-802.11) */
                break;
            case  SIOCGIWNWID:              /* get network id (the cell) */
                memcpy( iwp, value, IW_EV_PARAM_LEN-IW_EV_LCP_LEN );
                if ( iwevent.u.nwid.disabled )
                    net.setData( WirelessNetwork::NWID, tr("off") );
                else
                    net.setData( WirelessNetwork::NWID, QString::number( iwevent.u.nwid.value ) );
                break;
            case  SIOCSIWFREQ:              /* set channel/frequency (Hz) */
                break;
            case  SIOCGIWFREQ:              /* get channel/frequency (Hz) */
                {
                    memcpy( iwp, value, IW_EV_FREQ_LEN-IW_EV_LCP_LEN );
                    double val = ((double)iwevent.u.freq.m) * pow(10, iwevent.u.freq.e);
                    if ( val < 1e3 ) {
                        //we have a channel number
                        net.setData( WirelessNetwork::Channel, QString::number(val) );
                    } else {
                        //TODO match channel against frequency ->check iw_range
                        //we got the frequency
                        net.setData( WirelessNetwork::Frequency, QString::number(val) );
                    }
                }
                break;
            case  SIOCSIWMODE:              /* set operation mode */
                break;
            case  SIOCGIWMODE:              /* get operation mode */
                //IW_EV_UINT_LEN
                memcpy( iwp, value, IW_EV_UINT_LEN - IW_EV_LCP_LEN );
                net.setData( WirelessNetwork::Mode, operationMode[iwevent.u.mode] );
                break;
            //case  SIOCSIWSENS:              /* set sensitivity (dBm) */
            //case  SIOCGIWSENS:              /* get sensitivity (dBm) */
            //case  SIOCSIWRANGE:             /* Unused */
            //case  SIOCGIWRANGE:             /* Get range of parameters */
            //case  SIOCSIWPRIV:              /* Unused */
            //case  SIOCGIWPRIV:              /* get private ioctl interface info */
            //case  SIOCSIWSTATS:             /* Unused */
            //case  SIOCGIWSTATS:             /* Get /proc/net/wireless stats */
            //case  SIOCSIWSPY:               /* set spy addresses */
            //case  SIOCGIWSPY:               /* get spy info (quality of link) */
            //case  SIOCSIWTHRSPY:            /* set spy threshold (spy event) */
            //case  SIOCGIWTHRSPY:            /* get spy threshold */
            //case  SIOCSIWAP:                /* set access point MAC addresses */
            case  SIOCGIWAP:                /* get access point MAC addresses */
                {
                    //AP is the first parameter returned -> from here on we start a new WLAN network
                    if ( net.isValid() )
                        entries.append( net );
                    net = WirelessNetwork();
                    memcpy( iwp, value, IW_EV_ADDR_LEN-IW_EV_LCP_LEN );
                    const struct ether_addr* eth = (const struct ether_addr*)(&iwevent.u.ap_addr.sa_data);
                    QString tmp;
                    tmp.sprintf("%02X:%02X:%02X:%02X:%02X:%02X", eth->ether_addr_octet[0],
                            eth->ether_addr_octet[1], eth->ether_addr_octet[2],
                            eth->ether_addr_octet[3], eth->ether_addr_octet[4],
                            eth->ether_addr_octet[5]);
                    net.setData( WirelessNetwork::AP, tmp );
                }
                break;
            //case  SIOCSIWSCAN:              /* trigger scanning (list cells) */
            //case  SIOCGIWSCAN:              /* get scanning results */
            case  SIOCSIWESSID:             /* set ESSID (network name) */
                break;
            case  SIOCGIWESSID:             /* get ESSID */
                {
                    unsigned char * payload;
                    if ( weVersion > 18 ) {
                        //the pointer in iw_point is omitted
                        memcpy( iwp+IW_EV_POINT_OFF, value, sizeof(struct iw_point)-IW_EV_POINT_OFF );
                        payload = value + sizeof(struct iw_point) - IW_EV_POINT_OFF;
                    } else {
                        memcpy( iwp, value, sizeof(struct iw_point) );
                        payload = value + sizeof(struct iw_point);
                    }
                    if ( data + iwevent.len - payload > 0 && iwevent.u.essid.length  )  { //we have some payload
                        QString ssid;
                        if ( iwevent.u.essid.flags ) {
                            char essid[IW_ESSID_MAX_SIZE+1];
                            memcpy( essid, payload, iwevent.u.essid.length );
                            essid[iwevent.u.essid.length] = '\0';
                            ssid = QString(essid);
                            //some wlan driver return "<hidden>" when the essid is hidden other drivers
                            //just return an empty string. Qtopia assumes that a hidden essid is called "<hidden>"
                            ssid = convertToHidden( ssid );
                            //qLog(Network) << "Discovered network on" << iface << ": " << ssid;
                        } else {
                            ssid = tr("off");
                        }
                        net.setData( WirelessNetwork::ESSID, ssid );
                    } else {
                        qLog(Network) << "ESSID event detected but no payload available";
                    }
                }
                break;
            //case  SIOCSIWNICKN:             /* set node name/nickname */
            //case  SIOCGIWNICKN:             /* get node name/nickname */
            case  SIOCSIWRATE:              /* set default bit rate (bps) */
                break;
            case  SIOCGIWRATE:              /* get default bit rate (bps) */
                {
                    memcpy( iwp, value, IW_EV_PARAM_LEN-IW_EV_LCP_LEN ); //to be consistent
                    struct iw_param param;
                    const unsigned char * const end = (unsigned char*) (data +iwevent.len);
                    int maxTransfer = 0;
                    while ( value + IW_EV_PARAM_LEN-IW_EV_LCP_LEN <= end )
                    {
                        memcpy( &param, value, IW_EV_PARAM_LEN-IW_EV_LCP_LEN );
                        maxTransfer = qMax( maxTransfer, param.value );
                        value += (IW_EV_PARAM_LEN-IW_EV_LCP_LEN);
                    }
                    net.setData( WirelessNetwork::BitRate, maxTransfer );
                }
                break;
            //case  SIOCSIWRTS:               /* set RTS/CTS threshold (bytes) */
            //case  SIOCGIWRTS:               /* get RTS/CTS threshold (bytes) */
            //case  SIOCSIWFRAG:              /* set fragmentation thr (bytes) */
            //case  SIOCGIWFRAG:              /* get fragmentation thr (bytes) */
            //case  SIOCSIWTXPOW:             /* set transmit power (dBm) */
            //case  SIOCGIWTXPOW:             /* get transmit power (dBm) */
            //case  SIOCSIWRETRY:             /* set retry limits and lifetime */
            //case  SIOCGIWRETRY:             /* get retry limits and lifetime */
            case  SIOCSIWENCODE:            /* set encoding token & mode */
                break;
            case  SIOCGIWENCODE:            /* get encoding token & mode */
                {
                    unsigned char * payload;
                    if ( weVersion > 18 ) {
                        //the pointer in iw_point is omitted
                        memcpy( iwp+IW_EV_POINT_OFF, value, sizeof(struct iw_point)-IW_EV_POINT_OFF );
                        payload = value + sizeof(struct iw_point) - IW_EV_POINT_OFF;
                    } else {
                        memcpy( iwp, value, sizeof(struct iw_point) );
                        payload = value + sizeof(struct iw_point);
                    }
                    //evaluate security mode
                    QString tmp;
                    if ( (iwevent.u.data.flags & IW_ENCODE_OPEN) || (iwevent.u.data.flags & IW_ENCODE_RESTRICTED) ) {
                        if ( iwevent.u.data.flags & IW_ENCODE_OPEN )
                            tmp = tr("open", "open security");
                        else
                            tmp = tr("restricted", "restricted security");
                        net.setData( WirelessNetwork::Security, tmp );
                    }
                    if ( iwevent.u.data.flags & IW_ENCODE_DISABLED ) {
                        net.setData( WirelessNetwork::Encoding, tr("off") );
                    } else {
                        if ( iwevent.u.data.flags & IW_ENCODE_NOKEY )
                            net.setData( WirelessNetwork::Encoding, tr("on") );
                        else if ( data + iwevent.len - payload > 0 && iwevent.u.data.length  ) { //we have some payload
                            //TODO we have a key that we could show
                            //r.data = tr("whatever");
                        }
                    }
                }
                break;
            //case  SIOCSIWPOWER:             /* set Power Management settings */
            //case  SIOCGIWPOWER:             /* get Power Management settings */

            // WPA ioctls
            //case  SIOCSIWGENIE:             /* set generic IE */
            //case  SIOCGIWGENIE:             /* get generic IE */
            //case  SIOCSIWMLME:              /* request MLME operation */
            //case  SIOCSIWAUTH:              /* set authentication mode params */
            //case  SIOCGIWAUTH:              /* get authentication mode params */
            //case  SIOCSIWENCODEEXT:         /* set encoding token & mode */
            //case  SIOCGIWENCODEEXT:         /* get encoding token & mode */
            //case  SIOCSIWPMKSA:             /* PMKSA cache operation */

            //wireless events
            //case  IWEVTXDROP:
            case  IWEVQUAL:
                //we need range info WE version >15 to support quality parameter
                if ( weVersion > 15 ) {
                    memcpy( iwp, value, IW_EV_QUAL_LEN-IW_EV_LCP_LEN );
                    struct iw_quality* qual = &iwevent.u.qual;
                    //the quality is always a relative value
                    if ( !(qual->updated & IW_QUAL_QUAL_INVALID) ) {
                        net.setData( WirelessNetwork::Quality,
                                QString().setNum( ((double) qual->qual)/((int)range->max_qual.qual), 'f', 2 ) );

                    }

                    //the quality field is 8 bit integer
                    //we need range to work out whether we have dbm or percent value
                    //by comparing quality.qual with range->max_qual.value
                    //Percent -> use iw_quality.qual as signed integer ( iw_quality.qual < range->max_qual.value )
                    //dbm -> use iw_quality.qual as negative integer ( iw_quality.qual > range->max_qual.value )
                    

                    if ( qual->level <= range->max_qual.level ){
                        //we have relative signal level
                        if ( !(qual->updated & IW_QUAL_LEVEL_INVALID) ) {
                            int range_max = qMax( (int)range->max_qual.level, 255 );
                            net.setData( WirelessNetwork::Signal,
                                    QString().setNum( ((double)qual->level) / range_max ) );
                        }
                        if ( !(qual->updated & IW_QUAL_NOISE_INVALID) ) {
                            int range_max = qMax( (int)range->max_qual.noise, 255 );
                            net.setData( WirelessNetwork::Noise,
                                    QString().setNum( ((double)qual->noise) / range_max, 'f', 2));
                        }
                    } else {
                        if ( !(qual->updated & IW_QUAL_LEVEL_INVALID) ) {
                            int range_max = qMax( (int)range->max_qual.level, 255 );
                            net.setData( WirelessNetwork::Signal,
                                    QString().setNum( ( (double)qual->level - 0x100)/range_max + 1, 'f', 2 ) );
                            //level.data = QString::number(qual->level - 0x100) + QChar(' ') + tr("dBm", "unit for signal strength");
                        }
                        if ( !(qual->updated & IW_QUAL_NOISE_INVALID) ) {
                            int range_max = qMax( (int)range->max_qual.noise, 255 );
                            net.setData( WirelessNetwork::Noise,
                                    QString().setNum( ( (double)qual->noise - 0x100)/range_max +1, 'f', 2 ) );
                            //noise.data = QString::number(qual->noise - 0x100) + QChar(' ') + tr("dBm", "unit for signal strength");
                        }
                    }
                }
                break;
#if WIRELESS_EXT > 14
            case  IWEVCUSTOM:
                {
                    unsigned char * payload;
                    if ( weVersion > 18 ) {
                        //the pointer in iw_point is omitted
                        memcpy( iwp+IW_EV_POINT_OFF, value, sizeof(struct iw_point)-IW_EV_POINT_OFF );
                        payload = value + sizeof(struct iw_point) - IW_EV_POINT_OFF;
                    } else {
                        memcpy( iwp, value, sizeof(struct iw_point) );
                        payload = value + sizeof(struct iw_point);
                    }

                    char buf[IW_CUSTOM_MAX+1];
                    if ( iwevent.u.data.length ) {
                        memcpy( buf, payload, iwevent.u.data.length );
                        buf[ iwevent.u.data.length ] = '\0';
                        net.addCustomData( QString( buf ).trimmed() );
                    }

                }
                break;
#endif
            //case  IWEVREGISTERED:
            //case  IWEVEXPIRED:
#if WIRELESS_EXT > 17
            case  IWEVGENIE:
                //we don't use this data yet (no need) but the package logic is there already.
                {
                    unsigned char * payload;
                    if ( weVersion > 18 ) {
                        //the pointer in iw_point is omitted
                        memcpy( iwp+IW_EV_POINT_OFF, value, sizeof(struct iw_point)-IW_EV_POINT_OFF );
                        payload = value + sizeof(struct iw_point) - IW_EV_POINT_OFF;
                    } else {
                        memcpy( iwp, value, sizeof(struct iw_point) );
                        payload = value + sizeof(struct iw_point);
                    }
                  
                    //payload structure from iwlist.c 
                    //int len = iwevent.u.data.length;
                    /*package structure (each package minimum of 2 bytes): 
                        - payload[0] -> 0xdd (WPA1 or other)
                                        ->
                                     -> 0x30 (WPA2)
                        - payload[1] -> length of subsequent additional data
                      */
                    while ( &(payload[2]) <= data + iwevent.len) 
                    {

                        switch( payload[0] )
                        {
                            case 0xdd:
                                //qDebug() << "WPA1?";
                                //we cannot be sure yet that this is WPA2. for details see wireless tools.
                                break;
                            case 0x30:
                                //qDebug() << "WPA2";
                                break;
                            default:
                                break;
                        }
                        payload += 2 + payload[1];
                    }
                }
                break;
#endif
            //case  IWEVMICHAELMICFAILURE:
            //case  IWEVASSOCREQIE:
            //case  IWEVASSOCRESPIE:
            //case  IWEVPMKIDCAND:
            default:
                qLog(Network) << "Unknown iw_event type" << iwevent.cmd;
            }

        data += iwevent.len;
    }
    if ( net.isValid() )
        entries.append( net );

    if ( qLogEnabled(Network) ) {
        foreach(WirelessNetwork n, entries ) {
            qLog(Network) << "#### Found" << n.data(WirelessNetwork::ESSID) << n.data(WirelessNetwork::AP) << n.data(WirelessNetwork::Security).toString();
            //net.dump();
        }
    }
#else
    Q_UNUSED(data);
    Q_UNUSED(length);
    Q_UNUSED(weVersion);
    Q_UNUSED(range);
    Q_UNUSED(data);
    Q_UNUSED(operationMode);

#endif
}


static const char* itemDescription[] = {
    QT_TRANSLATE_NOOP( "WSearchPage", "Protocol" ), //WRecord::Protocol
    QT_TRANSLATE_NOOP( "WSearchPage", "Access point" ), //WRecord::AP
    QT_TRANSLATE_NOOP( "WSearchPage", "ESSID" ), //WRecord::ESSID
    QT_TRANSLATE_NOOP( "WSearchPage", "Mode" ), //WRecord::Mode
    QT_TRANSLATE_NOOP( "WSearchPage", "Network ID" ), //WRecord::NWID
    QT_TRANSLATE_NOOP( "WSearchPage", "Bit rate" ), //WRecord::BitRate
    QT_TRANSLATE_NOOP( "WSearchPage", "Frequency" ), //WRecord::Frequency
    QT_TRANSLATE_NOOP( "WSearchPage", "Channel" ), //WRecord::Channel
    QT_TRANSLATE_NOOP( "WSearchPage", "Encryption" ), //WRecord::Encoding
    QT_TRANSLATE_NOOP( "WSearchPage", "Security" ), //WRecord::Security
    QT_TRANSLATE_NOOP( "WSearchPage", "Quality" ), //WRecord::Quality
    QT_TRANSLATE_NOOP( "WSearchPage", "Signal level" ), //WRecord::Signal
    QT_TRANSLATE_NOOP( "WSearchPage", "Noise level" ), //WRecord::Noise
    QT_TRANSLATE_NOOP( "WSearchPage", "More info" )  //WRecord::Custom
};

class ChooseNetworkUI : public QDialog {
    Q_OBJECT
public:
    ChooseNetworkUI( QWidget* parent = 0, Qt::WFlags flags = 0 );
    virtual ~ChooseNetworkUI();

    void setScanData( const QList<WirelessNetwork>& list);
    WirelessNetwork selectedWlan() const;

private:
    void init();

private slots:
    void wlanSelected();
    void updateView();

private:
    QListWidget* list;
    QList<WirelessNetwork> nets;

    QAction* filterHidden;
};

/**************************************************************************************/

static const int MacAddressRole = Qt::UserRole;
static const int OnlineStateRole = Qt::UserRole + 1;
static const int BitRateRole = Qt::UserRole + 2;
static const int ESSIDRole = Qt::UserRole + 3;
static const int ChannelRole = Qt::UserRole + 4;
static const int ModeRole = Qt::UserRole + 5;
static const int EncryptionRole = Qt::UserRole + 6;
static const int EncryptKeyLengthRole = Qt::UserRole + 7;
static const int EncryptPassphraseRole = Qt::UserRole + 8;
static const int EncryptKeyRole = Qt::UserRole + 9;
static const int SelectedEncryptKeyRole = Qt::UserRole + 10;
static const int NickNameRole = Qt::UserRole + 11;
static const int UuidRole = Qt::UserRole + 12;
static const int EAPAnonIdentityRole = Qt::UserRole + 13;
static const int EAPAuthenticationRole = Qt::UserRole + 14;
static const int EAPClientCertRole = Qt::UserRole + 15;
static const int EAPClientKeyRole = Qt::UserRole + 16;
static const int EAPClientKeyPasswordRole = Qt::UserRole + 17;
static const int EAPIdentityRole = Qt::UserRole + 18;
static const int EAPIdentityPasswordRole = Qt::UserRole + 19;
static const int EAPServerCertRole = Qt::UserRole + 20;
static const int PSKAlgorithmRole = Qt::UserRole + 21;
static const int WPAEnterpriseRole = Qt::UserRole + 22;

WSearchPage::WSearchPage( const QString& c, QWidget* parent, Qt::WFlags flags )
    : QWidget( parent, flags ), config( c ), scanEngine( 0 ), state( QtopiaNetworkInterface::Unknown ),
        currentSelection( 0 ), isRestart( false )
{
    // itemDescription is used to generate translations
    Q_UNUSED(itemDescription);

    initUI();
    loadKnownNetworks();

    devSpace = new QNetworkDevice( c, this );
    attachToInterface( devSpace->interfaceName() );
    state = devSpace->state();
    stateChanged( state, false );
    connect( devSpace, SIGNAL(stateChanged(QtopiaNetworkInterface::Status,bool)),
           this, SLOT(stateChanged(QtopiaNetworkInterface::Status,bool)) );

    setObjectName( "wireless-scan");
}

WSearchPage::~WSearchPage()
{
}

/*!
  \internal

  Initialise user interface.
  */
void WSearchPage::initUI()
{
    QVBoxLayout* vbox = new QVBoxLayout( this );
    vbox->setMargin( 2 );
    vbox->setSpacing( 2 );

    currentNetwork = new QLabel( this );
    currentNetwork->setWordWrap( true );
    currentNetwork->setTextFormat( Qt::RichText );
    currentNetwork->setText( tr("Connection state:\n<center><b>not connected</b></center>") );
    vbox->addWidget( currentNetwork );

    QFrame *seperator = new QFrame( this );
    seperator->setFrameShape( QFrame::HLine );
    vbox->addWidget( seperator );

    descr = new QLabel( tr("Network priority:"), this );
    descr->setWordWrap( true );
    vbox->addWidget( descr );

    knownNetworks = new QListWidget( this );
    knownNetworks->setAlternatingRowColors( true );
    knownNetworks->setSelectionBehavior( QAbstractItemView::SelectRows );
    knownNetworks->setEditTriggers( QAbstractItemView::NoEditTriggers );
    vbox->addWidget( knownNetworks );

    connect( knownNetworks, SIGNAL(itemActivated(QListWidgetItem*)),
        this, SLOT(changePriority(QListWidgetItem*)) );

    knownNetworks->installEventFilter( this );

    QMenu* menu = QSoftMenuBar::menuFor( this );
    QSoftMenuBar::setHelpEnabled ( this, true );
    scanAction = new QAction( QIcon(":icon/Network/lan/WLAN-demand"), tr("Rescan"), this ) ;
    scanAction->setVisible( false );
    menu->addAction( scanAction );

    environmentAction = new QAction( QIcon(":icon/new"), tr("Add new networks..."), this );
    menu->addAction( environmentAction );
    connect( environmentAction, SIGNAL(triggered()), this, SLOT(showAllNetworks()) );

    connectAction = new QAction( QIcon(":icon/Network/lan/WLAN-online"), tr("Connect"), this );
    menu->addAction( connectAction );
    connectAction->setVisible( false );
    connect( connectAction, SIGNAL(triggered()), this, SLOT(connectToNetwork()) );

    deleteAction = new QAction( QIcon(":icon/trash"), tr("Delete"), this );
    menu->addAction( deleteAction );
    deleteAction->setVisible( false );
    connect( deleteAction, SIGNAL(triggered()), this, SLOT(deleteNetwork()) );
}


/*!
  \internal

  Populates the "know networks" list by loading the details from config file
  */
void WSearchPage::loadKnownNetworks()
{
    knownNetworks->clear();
    QSettings cfg( config, QSettings::IniFormat );
    int size = cfg.beginReadArray( "WirelessNetworks" );

    if ( size <= 0 ) {
        QListWidgetItem* item = new QListWidgetItem( tr("<No known networks>") );
        item->setData( MacAddressRole, "INVALID" );
        item->setTextAlignment( Qt::AlignCenter );
        knownNetworks->addItem( item );
        knownNetworks->setSelectionMode( QAbstractItemView::NoSelection );
        cfg.endArray();
        return;
    }

    knownNetworks->setSelectionMode( QAbstractItemView::SingleSelection );

    QListWidgetItem* item;
    QString speed;
    QString essid;
    for ( int i= 0; i < size; i++ ) {
        cfg.setArrayIndex( i );
        QString essid = cfg.value("ESSID").toString();
        QString speed = cfg.value("BitRate", "0").toString();
        QString itemText = essid;
        if ( speed != "0" )
            itemText += " ("+speed+" "+ tr("Mb/s" , "Megabit per seconds")+")" ;
        item = new QListWidgetItem( itemText, knownNetworks );
        item->setData( MacAddressRole, cfg.value("AccessPoint").toString() );
        item->setIcon( QIcon(":icon/Network/lan/WLAN-notavail") );
        item->setData( OnlineStateRole, false );
        item->setData( BitRateRole, speed );
        item->setData( ESSIDRole, essid );
        item->setData( ChannelRole, cfg.value("CHANNEL", 0 ).toInt() );
        item->setData( EncryptionRole, cfg.value("Encryption", QString("open") ).toString() );
        item->setData( EncryptKeyLengthRole, cfg.value("KeyLength", 128 ).toInt() );
        item->setData( EncryptPassphraseRole, cfg.value("PRIV_GENSTR").toString() );
        item->setData( PSKAlgorithmRole, cfg.value("PSKAlgorithm", "TKIP").toString() );
        item->setData( WPAEnterpriseRole, cfg.value("WPAEnterprise", "TLS").toString() );
        item->setData( EAPAnonIdentityRole, cfg.value("EAPAnonIdentity").toString() );
        item->setData( EAPAuthenticationRole, cfg.value("EAPAuthentication","Any").toString() );
        item->setData( EAPClientCertRole, cfg.value("EAPClientCert").toString() );
        item->setData( EAPClientKeyRole, cfg.value("EAPClientKey").toString() );
        item->setData( EAPClientKeyPasswordRole, cfg.value("EAPClientKeyPassword").toString() );
        item->setData( EAPIdentityRole, cfg.value("EAPIdentity").toString() );
        item->setData( EAPIdentityPasswordRole, cfg.value("EAPIdentityPassword").toString() );
        item->setData( EAPServerCertRole, cfg.value("EAPServerCert").toString() );
        QString key1 = cfg.value( "WirelessKey_1" ).toString();
        QString key2 = cfg.value( "WirelessKey_2" ).toString();
        QString key3 = cfg.value( "WirelessKey_3" ).toString();
        QString key4 = cfg.value( "WirelessKey_4" ).toString();
        item->setData( EncryptKeyRole, QString("%1@%2@%3@%4").arg(key1).arg(key2).arg(key3).arg(key4) );
        item->setData( SelectedEncryptKeyRole, cfg.value("SelectedKey", "PP").toString() );
        item->setData( NickNameRole, cfg.value("Nickname").toString() );
        item->setData( UuidRole, cfg.value("Uuid").toString() );
    }

    cfg.endArray();
}


void WSearchPage::stateChanged(QtopiaNetworkInterface::Status newState, bool /*error*/)
{
    if ( !scanEngine )
        return;
    if ( state != newState 
            && state == QtopiaNetworkInterface::Down 
            && (newState == QtopiaNetworkInterface::Demand || newState == QtopiaNetworkInterface::Pending
                || newState == QtopiaNetworkInterface::Up) )
    {
        QtopiaNetwork::extendInterfaceLifetime( config, true );
    }
    
    state = newState;
    switch (newState) {
        case QtopiaNetworkInterface::Pending:
        case QtopiaNetworkInterface::Demand:
            //don't do anything
            break;
        case QtopiaNetworkInterface::Up:
            {
                if ( scanEngine ) {
                    const QString mac = scanEngine->currentAccessPoint();
                    QString essid;
                    QListWidgetItem* lwi = 0;
                    for (int i = 0; i<knownNetworks->count() && essid.isEmpty(); i++) {
                        QListWidgetItem* item = knownNetworks->item( i );
                        if ( !item )
                            return;
                        if ( item->data( MacAddressRole ).toString() == mac ) {
                            essid = item->data( ESSIDRole ).toString();
                            lwi = item;
                        }
                    }

                    //this could return <hidden> which is not very useful -> hence we do it after camparing MACs
                    const QString curEssid = scanEngine->currentESSID();
                    if ( essid.isEmpty() ) {
                        essid = curEssid;
                    } else if ( curEssid != essid ) {
                        //same macs but different essids
                        //->we have a hidden essid but we are connected to it hence we know the real essid
                        essid = curEssid;
                        lwi->setData( ESSIDRole, essid );
                    }
                    //mask < and > because we are about the show it in richtext
                    essid.replace( QString("<"), QString("&lt;") );
                    essid.replace( QString(">"), QString("&gt;") );

                    currentNetwork->setText( QString(
                                    tr("Connection state:\n<center>Connected to <b>%1</b></center>", "1=network name") )
                            .arg( essid ));
                    updateActions( knownNetworks->currentItem(), 0 ); //update all actions
                }
            }
            break;
        case QtopiaNetworkInterface::Down:
            if ( isRestart ) {
                //we could have a reconnection attempt to a new network
                //if that's the case attempt to connect;
                isRestart = false;
                connectToNetwork();
                break;
            }
        default:
            currentNetwork->setText( tr("Connection state:\n<center><b>not connected</b></center>") );
            return;
    }
}

void WSearchPage::saveScanResults()
{
    saveKnownNetworks();
}

/*!
  Saves all scan results to config file.
  */
void WSearchPage::saveKnownNetworks() {
    QSettings cfg( config, QSettings::IniFormat );
    const QVariant timeout = cfg.value( "WirelessNetworks/Timeout" );
    const QVariant autoConnect = cfg.value( "WirelessNetworks/AutoConnect" );
    cfg.beginGroup( "WirelessNetworks" );
    cfg.remove(""); //delete all "known networks" information
    cfg.endGroup();

    if ( knownNetworks->count() ) {
        QListWidgetItem* item;
        cfg.beginWriteArray( "WirelessNetworks" );
        for ( int i = 0; i<knownNetworks->count(); i++ ) {
            item = knownNetworks->item( i );
            if (!item)
                continue;
            QString mac = item->data( MacAddressRole ).toString();
            if ( mac == "INVALID" ) //don't save the "no known network" item
                continue;
            cfg.setArrayIndex( i );
            cfg.setValue( "AccessPoint", mac );
            cfg.setValue( "ESSID" , item->data( ESSIDRole ).toString() );
            cfg.setValue( "BitRate", item->data( BitRateRole ).toString() );
            int channel = 0;
            QVariant v = item->data( ChannelRole );
            if ( v.isValid() )
                channel = v.toInt();
            cfg.setValue( "CHANNEL", channel );
            v = item->data( ModeRole );
            if ( v.isValid() )
                cfg.setValue("WirelessMode", v.toString());
            else
                cfg.setValue("WirelessMode", "Managed");

            v = item->data( EncryptionRole );
            if ( v.isValid() )
                cfg.setValue("Encryption", item->data( EncryptionRole ).toString() );
            else
                cfg.setValue("Encryption", "open" );

            v = item->data( EncryptKeyLengthRole );
            if ( v.isValid() )
                cfg.setValue("KeyLength", v.toInt() );
            else
                cfg.setValue("KeyLength", 128 );

            cfg.setValue("PRIV_GENSTR", item->data( EncryptPassphraseRole ).toString() );

            v = item->data( SelectedEncryptKeyRole );
            if ( v.isValid() )
                cfg.setValue( "SelectedKey", v.toString() );
            else
                cfg.setValue( "SelectedKey", "PP" ); //use passphrase as default

            v = item->data( EncryptKeyRole );
            QStringList keys;
            if ( v.isValid() )
                keys = item->data( EncryptKeyRole ).toString().split( QChar('@'), QString::KeepEmptyParts );
            else
                keys << "" << "" << "" << ""; //add 4 empty keys
            for (int j = keys.count() -1; j >= 0; j-- )
                cfg.setValue("WirelessKey_"+QString::number(j+1), keys[j] );

            cfg.setValue("Nickname", item->data( NickNameRole ).toString() );
            
            v = item->data( UuidRole );
            if ( v.isValid() ) {
                cfg.setValue( "Uuid", item->data( UuidRole ).toString() );
            } else {
                //new WLAN
                QUuid uid = QUuid::createUuid(); 
                cfg.setValue( "Uuid", uid.toString() );
            }

            cfg.setValue( "EAPAnonIdentity", item->data(EAPAnonIdentityRole).toString() );
            v = item->data( EAPAuthenticationRole );
            cfg.setValue( "EAPAuthentication", v.isValid() ? v : QLatin1String("Any" ) );
            cfg.setValue( "EAPClientCert", item->data( EAPClientCertRole ) );
            cfg.setValue( "EAPClientKey", item->data( EAPClientKeyRole ) );
            cfg.setValue( "EAPClientKeyPassword", item->data( EAPClientKeyPasswordRole ) );
            cfg.setValue( "EAPIdentity", item->data( EAPIdentityRole ) );
            cfg.setValue( "EAPIdentityPassword", item->data( EAPIdentityPasswordRole ) );
            cfg.setValue( "EAPServerCert", item->data( EAPServerCertRole ));
            v = item->data( PSKAlgorithmRole );
            cfg.setValue( "PSKAlgorithm", v.isValid() ? v.toString(): QLatin1String("TKIP" ) );
            v = item->data( WPAEnterpriseRole );
            cfg.setValue( "WPAEnterprise", v.isValid() ? v.toString() : QLatin1String("TLS") );
        }
        cfg.endArray();
    }
    cfg.setValue( "Info/WriteToSystem", true );
    cfg.setValue( "WirelessNetworks/AutoConnect", autoConnect );
    cfg.setValue( "WirelessNetworks/Timeout", timeout );
    cfg.sync();

}

/*!
  The search for networks should be performed on \ifaceName.
  */
void WSearchPage::attachToInterface( const QString& ifaceName )
{
    if ( !scanEngine ) {
        qLog(Network) << "Using network scanner on interface" << ifaceName;
        scanEngine = new WirelessScan( ifaceName, this );
        connect( scanEngine, SIGNAL(scanningFinished()), this, SLOT(updateConnectivity()) );
        connect( scanAction, SIGNAL(triggered()), scanEngine, SLOT(startScanning()) );

        //do we support network scanning?
        struct iw_range range;
        int weVersion = 0;
        scanEngine->rangeInfo( &range, &weVersion);
        qLog(Network) << "Wireless extension version" << weVersion << "detected";
        if ( weVersion >= 14 ) { //WE v14 introduced SIOCSIWSCAN and friends
            scanAction->setVisible( true );
            QTimer::singleShot( 1, scanEngine, SLOT(startScanning()) );
        } else {
            scanAction->setVisible( false );
        }
    }
}

/*!
  \internal

  Update actions.
  */
void WSearchPage::updateActions(QListWidgetItem* cur, QListWidgetItem* /*prev*/)
{
    //update all actions/buttons
    deleteAction->setVisible( cur );

    if ( !cur || !scanEngine)
        return;

    const bool isOnline = cur->data(OnlineStateRole).toBool();

    QString curAP = scanEngine->currentAccessPoint();
    bool connectEnabled = isOnline && !(curAP == cur->data( MacAddressRole ).toString()) ;
    connectAction->setVisible( connectEnabled );
}

/*!
  \internal

  Attempts to connect to the currently selected network in the "known network" list.
  */
void WSearchPage::connectToNetwork()
{
    if ( !scanEngine )
        return;
    QListWidgetItem* item = knownNetworks->currentItem();
    if ( !item )
        return;

    saveKnownNetworks();
    QString newESSID = item->data(ESSIDRole).toString();
    if ( newESSID.isEmpty() )
        return;

    switch (state) {
        case QtopiaNetworkInterface::Down:
            break;
        case QtopiaNetworkInterface::Pending:
        case QtopiaNetworkInterface::Demand:
        case QtopiaNetworkInterface::Up:
            {
                if ( item->data( MacAddressRole ).toString() == scanEngine->currentAccessPoint() ) {
                    qLog(Network) << "Already connected to" << item->data( ESSIDRole );
                    return;
                }
                QString currentESSID = scanEngine->currentESSID();
                qLog(Network) << "Connecting from" << currentESSID << "to"
                    << newESSID;

                //force shutdown. This means the app that loads this plugin
                //needs SXE netsetup privileges
                QtopiaNetwork::privilegedInterfaceStop( config );
                //stopping will take some time. we have to wait till the plugin
                //signals no connection;
                isRestart = true;
                return;
            }
            break;
        default:
            return;
    }

    qLog(Network) << "Connecting to" << newESSID;
    currentNetwork->setText( QString(
                    tr("Connection state:\n<center>Connecting to <b>%1</b></center>", "1=network name") )
            .arg( item->data( ESSIDRole ).toString() ));

    QtopiaNetwork::startInterface( config, QVariant( newESSID ) );
}

/*!
  \internal

  Deletes the selected network.
  */
void WSearchPage::deleteNetwork()
{
    int row = knownNetworks->currentRow();
    if ( row < 0 || row >= knownNetworks->count() )
        return;

    QListWidgetItem* item = knownNetworks->takeItem( row );
    delete item;
}


QString qualityToImage( const QString& quality, bool secure )
{
    bool ok = false;
    float qual = quality.toFloat( &ok );
    if ( !ok )
        return QString();
    if ( qual > 0.75f )
        if ( secure )
            return QString(":image/Network/lan/wlan-excellent-secure");
        else
            return QString(":image/Network/lan/wlan-excellent");
    else if ( qual > 0.4f )
        if ( secure )
            return QString(":image/Network/lan/wlan-avg-secure");
        else
            return QString(":image/Network/lan/wlan-avg");
    else
        if ( secure )
            return QString(":image/Network/lan/wlan-bad-secure");
        else
            return QString(":image/Network/lan/wlan-bad");
    return QString();
}

/*!
  \internal

  This function is a helper function for updateConnectivity(). It updates the information
  of a single item in the known list.

  If \a itemToUpdate is null it is assumed that we want to add a new entry.
  */
void WSearchPage::updateKnownNetworkList( const WirelessNetwork& record, QListWidgetItem* itemToUpdate )
{
    if ( !record.isValid() )
        return;
    QListWidgetItem* item = itemToUpdate;
    if ( !item )
        item = new QListWidgetItem( knownNetworks );
    else
        item->setText("");

    QString enc = record.data(WirelessNetwork::Encoding).toString();
    bool securedNet = false;
    if ( enc !=  WirelessScan::tr("off") )
        securedNet = true;

    const QVariant tmp = record.data( WirelessNetwork::Quality );
    if ( tmp.isValid() )
        item->setIcon( QIcon(qualityToImage( tmp.toString(), securedNet )) );
    else
        item->setIcon( QIcon(qualityToImage( record.data(WirelessNetwork::Signal).toString(), securedNet )) );

    bool ok;
    int rate = record.data(WirelessNetwork::BitRate).toInt( &ok );
    if ( ok ) {
        item->setData( BitRateRole, QString::number(rate/1e6) );
    }

    //don't override existing essid
    //if the essid is hidden we don't want to override the name set by the user
    if ( item->data( ESSIDRole ).toString().isEmpty() )
        item->setData( ESSIDRole, record.data(WirelessNetwork::ESSID) );

    item->setData( MacAddressRole, record.data(WirelessNetwork::AP) );
    if ( record.data(WirelessNetwork::Mode).toString() == "Ad-hoc" )
        item->setData( ModeRole, "Ad-hoc" );
    else
        item->setData( ModeRole, "Managed" );

    item->setData( OnlineStateRole, true );
    if ( rate > 0 )
        item->setText( item->data(ESSIDRole).toString() + " (" +
                item->data(BitRateRole).toString() + " " +
                tr("Mb/s" , "Megabit per seconds")+")" );
    else
        item->setText( item->data(ESSIDRole).toString() );
    knownNetworks->setSelectionMode( QAbstractItemView::SingleSelection );
}

/*!
  \internal

  This function initiates/shows the dialog which presents all WLAN networks
  in the local area. It can be used to add new networks to the "Known" list.
  */
void WSearchPage::showAllNetworks()
{
    if ( !scanEngine )
        return;

    QList<WirelessNetwork> list = scanEngine->results();

    ChooseNetworkUI dlg( this );
    dlg.setScanData( list );
    dlg.showMaximized();
    if ( QtopiaApplication::execDialog( &dlg ) ) {
        WirelessNetwork net = dlg.selectedWlan();
        if ( !net.isValid() )
            return;

        const QString selectedMac = net.data( WirelessNetwork::AP).toString();
        const QString selectedEssid = net.data( WirelessNetwork::ESSID).toString();

        //delete "no known network item"
        if ( knownNetworks->count() == 1 &&
                knownNetworks->item(0) && knownNetworks->item(0)->data( MacAddressRole ).toString() == "INVALID" ) {
            knownNetworks->clear();
        }

        //select current item if it's among known Networks already (matching MAC and ESSID)
        const bool hiddenEssid = (selectedEssid == "<hidden>");
        int matching = -1;
        for (int i = 0; i<knownNetworks->count(); i++) {
            QListWidgetItem* item = knownNetworks->item( i );
            if ( !item )
                continue;
            if ( !hiddenEssid && item->data(ESSIDRole).toString() == selectedEssid ) {
                if ( matching < 0 )
                    matching = i;
                if ( item->data(MacAddressRole).toString() == selectedMac ) { //exact match
                    knownNetworks->setCurrentItem( item );
                    return;
                }
            } else if ( !hiddenEssid ) {
                continue;
            } else if ( item->data(MacAddressRole).toString() == selectedMac ) {
                //we have to deal with a hidden essid -> just compare MACs
                matching = i;
            }
        }
        if ( matching >= 0 ) { //we had at least one network with the same essid
            knownNetworks->setCurrentItem( knownNetworks->item( matching ) );
            return;
        }

        //the selected network is not in our list yet
        updateKnownNetworkList( net );

        int row = knownNetworks->count() - 1;
        if ( row >= 0 )
            knownNetworks->setCurrentRow( row );
    }
}

/*!
  \internal

  Updates the state of the known networks and displays the network we are currently connected to.
  It uses updateKnownNetworkList to update each individual item.
  */
void WSearchPage::updateConnectivity()
{
    if ( !scanEngine )
        return;

    QList<WirelessNetwork> results = scanEngine->results();
    QStringList foundMacs;
    QStringList foundEssids;
    foreach ( WirelessNetwork net, results ) {
        foundMacs.append( net.data( WirelessNetwork::AP ).toString() );
        foundEssids.append( net.data( WirelessNetwork::ESSID ).toString() );
    }

    // network is a match if
    // a) essid is not hidden and mac and essid match or
    // b) essid is not hidden and essid matches or
    // c) essid is hidden and mac matches

    for ( int i = 0; i< knownNetworks->count(); ++i ) {
        QListWidgetItem* item = knownNetworks->item( i );
        if ( !item )
            continue;
        const QString macAddress = item->data(MacAddressRole).toString();
        if ( macAddress == "INVALID" )  //the empty list place holder
            continue;
        const QString essid = item->data(ESSIDRole).toString();

        int idx = -1;
        if ( (idx = foundEssids.indexOf( essid ))>=0  && essid != "<hidden>") {
            updateKnownNetworkList( results.at(idx), item );
        } else if ( (idx=foundMacs.indexOf(macAddress)) >= 0 ) {
            if ( foundEssids.at(idx) == "<hidden>" )
                updateKnownNetworkList( results.at(idx), item );
        } else {
            item->setIcon( QIcon(":icon/Network/lan/WLAN-notavail") );
            item->setData( OnlineStateRole, false );
        }
    }

    updateActions( knownNetworks->currentItem(), 0 ); //update all actions
}

void WSearchPage::changePriority( QListWidgetItem* item )
{
    if ( !item )
        return;
    if ( !currentSelection ) {
        descr->setText( tr("Moving %1", "%1=essid").arg(item->text()) );
        QFont f = item->font();
        f.setBold( true );
        item->setFont( f );
        currentSelection = item;
        QSoftMenuBar::setLabel( knownNetworks, Qt::Key_Back, QSoftMenuBar::NoLabel );
        QSoftMenuBar::setLabel( knownNetworks, Qt::Key_Back, QSoftMenuBar::NoLabel );
    }else if ( currentSelection ) {
        descr->setText( tr("Network priority:") );
        QFont f = currentSelection->font();
        f.setBold( false );
        currentSelection->setFont( f );
        if ( item != currentSelection) {
            int oldRow = knownNetworks->row(currentSelection);
            int newRow = knownNetworks->row(item);
            if (oldRow>newRow) {
                knownNetworks->takeItem(oldRow);
                knownNetworks->insertItem(newRow+1, currentSelection);
                knownNetworks->takeItem(newRow);
                knownNetworks->insertItem(oldRow, item);
            } else {
                knownNetworks->takeItem(oldRow);
                knownNetworks->insertItem(newRow, currentSelection);
                knownNetworks->takeItem(newRow-1);
                knownNetworks->insertItem(oldRow, item);
            }
            knownNetworks->setCurrentRow(newRow);
        }
        currentSelection = 0;
        QSoftMenuBar::setLabel( knownNetworks, Qt::Key_Back, QSoftMenuBar::Back );
    }

    connectAction->setVisible( !currentSelection );
    scanAction->setVisible( !currentSelection );
    environmentAction->setVisible( !currentSelection );
    deleteAction->setVisible( !currentSelection );
}
    
bool WSearchPage::eventFilter( QObject* watched, QEvent* event )
{
    if ( watched == knownNetworks && 
            0 != currentSelection )
    {
        if ( event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease ) {
            QKeyEvent *ke = static_cast<QKeyEvent*>(event);

            if ( event->type() == QEvent::KeyRelease &&  //ignore releases if key is one we watch out for
                    (ke->key() == Qt::Key_Up || ke->key() == Qt::Key_Down || ke->key()==Qt::Key_Back) )
                return true;

            int row = knownNetworks->currentRow();
            if ( ke->key() == Qt::Key_Up ) {
                if ( row > 0 ) //top row cannot move further up
                {
                    knownNetworks->takeItem( row );
                    knownNetworks->insertItem( row-1, currentSelection );
                    knownNetworks->setCurrentRow( row-1 );
                }
                return true;
            } else if ( ke->key() == Qt::Key_Down ) {
                if ( row < knownNetworks->count()-1 ) { //bottom row cannot move further down
                    knownNetworks->takeItem( row );
                    knownNetworks->insertItem( row+1, currentSelection );
                    knownNetworks->setCurrentRow( row+1 );
                    
                }
                return true;
            } else if ( ke->key() == Qt::Key_Back ) {
                return true; //ignore back for as long as we have a selection
            }
        }
    }
    return false;
}
/**************************************************************************************/

ChooseNetworkUI::ChooseNetworkUI( QWidget* parent, Qt::WFlags flags )
    : QDialog( parent, flags )
{
    setModal( true );
    init();
}

ChooseNetworkUI::~ChooseNetworkUI()
{
}

void ChooseNetworkUI::init()
{
    setWindowTitle( tr("New WLAN's") );

    QVBoxLayout* vbox = new QVBoxLayout( this );
    vbox->setMargin( 2 );
    vbox->setSpacing( 2 );

    QLabel* label = new QLabel( tr("The following networks were detected in the local area:"), this );
    label->setWordWrap( true );
    vbox->addWidget( label );

    list = new QListWidget( this );
    list->setAlternatingRowColors( true );
    vbox->addWidget( list );

    QMenu* menu = QSoftMenuBar::menuFor( this );

    filterHidden = new QAction( tr("Show hidden networks"), this );
    filterHidden->setCheckable( true );
    filterHidden->setChecked( false );
    menu->addAction( filterHidden );
    connect( filterHidden, SIGNAL(toggled(bool)), this, SLOT(updateView()) );

    connect( list, SIGNAL(itemActivated(QListWidgetItem*)),
            this, SLOT(wlanSelected()) );
}

WirelessNetwork ChooseNetworkUI::selectedWlan() const
{
    if ( !list->count() || !list->currentItem() )
        return WirelessNetwork();

    const QString mac = list->currentItem()->data( MacAddressRole ).toString();
    const QString essid = list->currentItem()->data( ESSIDRole ).toString();
    for( int i=0; i<nets.count(); ++i) {
        if ( nets[i].data(WirelessNetwork::AP).toString() == mac
            && nets[i].data( WirelessNetwork::ESSID).toString() == essid )
            return nets[i];
    }

    return WirelessNetwork();
}

void ChooseNetworkUI::setScanData( const QList<WirelessNetwork>& records)
{
    nets = records;
    updateView();
}

void ChooseNetworkUI::updateView()
{
    list->clear();

    const bool showHidden = filterHidden->isChecked();

    QListWidgetItem* item;
    if ( !nets.count() )
    {
        item = new QListWidgetItem( list );
        list->setSelectionMode( QAbstractItemView::NoSelection );
        item->setText( tr("<No WLAN found>") );
        item->setTextAlignment( Qt::AlignCenter );
        return;
    }

    QVariant tmp;
    QString essid;
    QHash<QString,int> essidExist;
    foreach( WirelessNetwork net, nets ) {
        essid = net.data(WirelessNetwork::ESSID).toString();

        if ( !showHidden && essid == "<hidden>" )
            continue;

        if ( essid != "<hidden>" ) {
            if ( essidExist[essid] < 1 )
                essidExist[essid]++;
            else
                continue; //don't show several APs with same essid
        }

        item = new QListWidgetItem( list );
        item->setData( ESSIDRole, essid );

        tmp = net.data(WirelessNetwork::Encoding).toString();
        bool securedNet = false;
        if ( tmp.toString() !=  WirelessScan::tr("off") )
            securedNet = true;

        tmp = net.data( WirelessNetwork::Quality );
        if ( tmp.isValid() )
            item->setIcon( QIcon(qualityToImage( tmp.toString(), securedNet )) );
        else
            item->setIcon( QIcon(qualityToImage( net.data(WirelessNetwork::Signal).toString(), securedNet )) );

        bool ok;
        int rate = net.data(WirelessNetwork::BitRate).toInt( &ok );
        if ( ok && rate>0 ) {
            essid += QLatin1String("    ");
            essid += QString::number(rate/1e6) + QLatin1String(" ") + tr("Mb/s" , "Megabit per seconds");
        }
        item->setText( essid );
        item->setData( MacAddressRole, net.data(WirelessNetwork::AP) );

    }

    list->sortItems();
}

void ChooseNetworkUI::wlanSelected()
{
    QDialog::done(1);
}
#include "wirelessscan.moc"

#endif //NO_WIRELESS_LAN
