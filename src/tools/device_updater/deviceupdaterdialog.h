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

#ifndef DEVICEUPDATERDIALOG_H
#define DEVICEUPDATERDIALOG_H

#include <QDialog>

class DeviceUpdaterWidget;

class DeviceUpdaterDialog : public QDialog
{
    Q_OBJECT
public:
    DeviceUpdaterDialog( QWidget *parent = 0, Qt::WFlags f = 0 );
    virtual ~DeviceUpdaterDialog();
    DeviceUpdaterWidget *updater() { return ui; }
private:
    DeviceUpdaterWidget *ui;
};

#endif
