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

#ifndef QMULTIPORTMULTIPLEXER_H
#define QMULTIPORTMULTIPLEXER_H

#include <qserialiodevicemultiplexer.h>

class QMultiPortMultiplexerPrivate;

class QTOPIACOMM_EXPORT QMultiPortMultiplexer : public QSerialIODeviceMultiplexer
{
    Q_OBJECT
public:
    explicit QMultiPortMultiplexer( QSerialIODevice *device, QObject *parent = 0 );
    ~QMultiPortMultiplexer();

    bool addChannel( const QString& name, QSerialIODevice *device );
    QSerialIODevice *channel( const QString& name );

private:
    QMultiPortMultiplexerPrivate *d;
};

#endif /* QMULTIPORTMULTIPLEXER_H */
