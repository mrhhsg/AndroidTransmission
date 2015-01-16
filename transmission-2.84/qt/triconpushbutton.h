/*
 * This file Copyright (C) 2009-2014 Mnemosyne LLC
 *
 * It may be used under the GNU GPL versions 2 or 3
 * or any future license endorsed by Mnemosyne LLC.
 *
 * $Id: triconpushbutton.h 14241 2014-01-21 03:10:30Z jordan $
 */

#ifndef QTR_IconPushButton_H
#define QTR_IconPushButton_H

#include <QPushButton>

class QIcon;

class TrIconPushButton: public QPushButton
{
    Q_OBJECT

  public:
    TrIconPushButton (QWidget * parent = 0);
    TrIconPushButton (const QIcon&, QWidget * parent = 0);
    virtual ~TrIconPushButton () {}
    QSize sizeHint () const;

  protected:
    void paintEvent (QPaintEvent * event);
};

#endif // QTR_IconPushButton_H
