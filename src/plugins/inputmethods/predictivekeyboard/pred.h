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

#ifndef _PRED_H_
#define _PRED_H_

#include <QList>
#include <QHash>
#include <QPoint>
#include <QString>
#include <QStringList>

class Word;
class WPWord;
template<class T> class DawgReduction;
class WordPredict
{
public:
    struct Config
    {
        int reallyNoMoveSensitivity;
        int moveSensitivity;
        int excludeDistance;
    };

    WordPredict(const Config &, int max, bool includePrefix = false);
    ~WordPredict();

    void setLetter(QChar, const QPoint &);

    void addLetter(QChar);
    void addTouch(const QPoint &);

    QString prefixedWord() const;
    QStringList words() const;
    QList<qreal> ratings() const;
    QString word() const;

    void reset();
    QList<QPoint> points() const { return m_points; }

    QString movementDesc() const;
private:
    int distanceForPoint(const QPoint &pos, QChar c);
    qreal weightForWord(const Word &word);
    qreal incrWeightForWord(WPWord *w);

    void updateWords();
    QStringList m_words;
    QList<qreal> m_weights;
    QString m_word;

    enum Movement { None = 0x0000,
                    ReallyNone = 0x0010,
                    Up = 0x0001,
                    Down = 0x0002,
                    Left = 0x0004,
                    Right = 0x0008,
                    Perfect = 0x0010 };
    Movement movement(const QPoint &, const QPoint &);

    QList<Movement> m_mPoints;
    QList<QPoint> m_points;
    QHash<QChar, QPoint> m_layout;
    QHash<QChar, int> m_latestDfp;
    int m_max;
    QString m_prefixedWord;
    QString m_alphabet;

    DawgReduction<WPWord> *reduction;

    bool m_includePre;
    Config m_config;
};

#endif // _PRED_H_
