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

#ifndef __GSTREAMER_MESSAGE_H
#define __GSTREAMER_MESSAGE_H

#include <QMetaType>

#include <gst/gst.h>


namespace gstreamer
{

class Message
{
public:
    Message();
    Message(GstMessage* message);
    ~Message();

    GstMessage* rawMessage() const;

private:
    GstMessage* m_message;
};

}   // ns gstreamer

Q_DECLARE_METATYPE(gstreamer::Message);


#endif  // __GSTREAMER_MESSAGE_H

