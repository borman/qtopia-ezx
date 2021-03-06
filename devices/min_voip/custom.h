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

#ifndef QT_QWS_GREENPHONE
#define QT_QWS_GREENPHONE
#endif

#if defined(__GNUC__) && (__GNUC__ > 2)
#define QPE_USE_MALLOC_FOR_NEW
#endif

#define QPE_DEFAULT_TODAY_MODE "Daily"

#define QPE_NEED_CALIBRATION

//#define NO_WIRELESS_LAN

// The serial device for AT command access to the phone
// hardware:
//
#define QTOPIA_PHONE_DEVICE "/dev/ttyS0"

// Sets the phone vendor plugin to load (normally all plugins are loaded).
#define QTOPIA_PHONE_VENDOR "greenphone"

// Define this to use the "advanced" GSM 07.10 CMUX mode instead of "basic".
//#define QTOPIA_ADVANCED_CMUX

// Define these to use direct serial ports for multiplexing channels.
// The primary AT command channel is defined by "QTOPIA_PHONE_DEVICE".
// Channels may be omitted if they aren't appropriate.
//
// This enables the use of CSMI on the TI device, instead of GMS 07.10.
//#define QTOPIA_MUX_SECONDARY_DEVICE "/dev/csmi/6"
//#define QTOPIA_MUX_DATA_DEVICE "/dev/csmi/8"
//#define QTOPIA_MUX_SPARE_DEVICE "/dev/ttyXX"
//#define QTOPIA_MUX_RAW_DEVICE "/dev/ttyXX"

// Define this if setup commands for GPRS data sessions should be sent
// on the primary command channel instead of the data channel.
//#define QTOPIA_MUX_DATA_SETUP_ON_COMMAND_CHANNEL

#define QTOPIA_ENABLE_EXPORTED_BACKGROUNDS
#define QTOPIA_ENABLE_GLOBAL_BACKGROUNDS

// Disable media player visualization
#define NO_VISUALIZATION

// Define the devices whose packages are compatible with this device,
// by convention the first device listed is this device.
#define QTOPIA_COMPATIBLE_DEVICES "Greenphone"

// Define the name of the Video4Linux device to use for the camera.
#ifndef V4L_VIDEO_DEVICE
#define V4L_VIDEO_DEVICE            "/dev/video"
#endif
