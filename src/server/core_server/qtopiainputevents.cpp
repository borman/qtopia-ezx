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

#include "qtopiainputevents.h"

#ifdef Q_WS_QWS

/*!
  \class QtopiaInputEvents
  \ingroup QtopiaServer
  \brief The QtopiaInputEvents class provides functions to simulate and access input events.

  The server calls sendKeyEvent() whenever it wishes to simulate
  a key event.
  
  This class is part of the Qtopia server and cannot be used by other Qtopia applications.
*/

/*!
    Sends the given key event. The key is identified by its \a unicode
    value and the given \a keycode, \a modifiers, \a isPress and \a
    autoRepeat parameters.

    Use this function to send key events generated by "virtual
    keyboards" (note that the processKeyEvent() function is
    impelemented using this function).

    The \a keycode parameter is the Qt keycode value as defined by the
    Qt::Key enum. The \a modifiers is an OR combination of
    Qt::KeyboardModifier values, indicating whether \gui
    Shift/Alt/Ctrl keys are pressed. The \a isPress parameter is true
    if the event is a key press event and \a autoRepeat is true if the
    event is caused by an auto-repeat mechanism and not an actual key
    press.

    \sa processKeyEvent()
*/
void QtopiaInputEvents::sendKeyEvent
    (int unicode, int keycode, Qt::KeyboardModifiers modifiers,
     bool isPress, bool autoRepeat)
{
    QWSServer::sendKeyEvent(unicode, keycode, modifiers, isPress, autoRepeat);
}

/*!
    Processes the given key event. The key is identified by its \a
    unicode value and the given \a keycode, \a modifiers, \a isPress
    and \a autoRepeat parameters.

    The \a keycode parameter is the Qt keycode value as defined by the
    Qt::Key enum. The \a modifiers is an OR combination of
    Qt::KeyboardModifier values, indicating whether \gui
    Shift/Alt/Ctrl keys are pressed. The \a isPress parameter is true
    if the event is a key press event and \a autoRepeat is true if the
    event is caused by an auto-repeat mechanism and not an actual key
    press.

    This function is typically called internally by keyboard drivers.

    \sa sendKeyEvent()
*/
void QtopiaInputEvents::processKeyEvent
    (int unicode, int keycode, Qt::KeyboardModifiers modifiers,
    bool isPress, bool autoRepeat)
{
    QWSServer::processKeyEvent(unicode, keycode, modifiers, isPress, autoRepeat);
}

/*!
    Adds the given keyboard \a filter to key events sent to Qtopia.

    Note the filter is not invoked for keys generated by virtual keyboards,
    but only for the physical keys on the device.

    \sa removeKeyboardFilter()
*/
void QtopiaInputEvents::addKeyboardFilter(QtopiaKeyboardFilter *filter)
{
    QWSServer::addKeyboardFilter(filter);
}

/*!
    Removes and deletes the most recently added filter.

    Note that the programmer is responsible for removing each added
    keyboard filter.
*/
void QtopiaInputEvents::removeKeyboardFilter()
{
    QWSServer::removeKeyboardFilter();
}

/*!
    Suspends pointer handling by deactivating all the mouse drivers
    registered by the QWS_MOUSE_PROTO environment variable.

    \sa resumeMouse()
*/
void QtopiaInputEvents::suspendMouse()
{
    qwsServer->suspendMouse();
}

/*!
    Resumes pointer handling by reactivating all the mouse drivers
    registered by the QWS_MOUSE_PROTO environment variable.

    \sa suspendMouse()
*/
void QtopiaInputEvents::resumeMouse()
{
    qwsServer->resumeMouse();
}

/*!
    Opens the mouse devices specified by the QWS_MOUSE_PROTO
    environment variable.
*/
void QtopiaInputEvents::openMouse()
{
    qwsServer->openMouse();
}

/*!
    Opens the keyboard devices specified by the QWS_KEYBOARD
    environment variable.
*/
void QtopiaInputEvents::openKeyboard()
{
    qwsServer->openKeyboard();
}

#endif // Q_WS_QWS
