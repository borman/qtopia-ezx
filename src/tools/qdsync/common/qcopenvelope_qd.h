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
#ifndef QCOPENVELOPE_QD_H
#define QCOPENVELOPE_QD_H

#include <qdglobal.h>

#include <QDataStream>
#include <QString>

namespace qdsync {

class QD_EXPORT QCopEnvelope : public QDataStream
{
public:
    QCopEnvelope( const QString &channel, const QString &message );
    ~QCopEnvelope();

private:
    QString ch, msg;
};

};

using namespace qdsync;

#endif
