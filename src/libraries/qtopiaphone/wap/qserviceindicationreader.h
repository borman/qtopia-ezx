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
#ifndef QSERVICEINDICATIONREADER_H
#define QSERVICEINDICATIONREADER_H

#include <qwbxmlreader.h>

class QTOPIAPHONE_EXPORT QServiceIndicationReader : public QWbXmlReader
{
public:
    QServiceIndicationReader();
    virtual ~QServiceIndicationReader();

protected:
    QString resolveOpaque( const QString& attr, const QByteArray& data );
};

#endif // QSERVICEINDICATIONREADER_H
