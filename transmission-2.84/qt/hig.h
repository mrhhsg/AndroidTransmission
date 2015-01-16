/*
 * This file Copyright (C) 2009-2014 Mnemosyne LLC
 *
 * It may be used under the GNU GPL versions 2 or 3
 * or any future license endorsed by Mnemosyne LLC.
 *
 * $Id: hig.h 14241 2014-01-21 03:10:30Z jordan $
 */


#ifndef QTR_HIG_H
#define QTR_HIG_H

#include <QWidget>

class QCheckBox;
class QLabel;
class QString;
class QGridLayout;
class QLayout;

class HIG: public QWidget
{
    Q_OBJECT

  public:

    enum
    {
      PAD_SMALL = 3,
      PAD = 6,
      PAD_BIG = 12,
      PAD_LARGE = PAD_BIG
    };

  public:

    HIG (QWidget * parent = 0);
    virtual ~HIG ();

  public:

    void addSectionDivider ();
    void addSectionTitle (const QString&);
    void addSectionTitle (QWidget*);
    void addSectionTitle (QLayout*);
    void addWideControl (QLayout *);
    void addWideControl (QWidget *);
    QCheckBox* addWideCheckBox (const QString&, bool isChecked);
    QLabel* addLabel (const QString&);
    QLabel* addTallLabel (const QString&);
    void addLabel (QWidget *);
    void addTallLabel (QWidget *);
    void addControl (QWidget *);
    void addControl (QLayout *);
    QLabel* addRow (const QString & label, QWidget * control, QWidget * buddy=0);
    QLabel* addRow (const QString & label, QLayout * control, QWidget * buddy);
    void addRow (QWidget * label, QWidget * control, QWidget * buddy=0);
    void addRow (QWidget * label, QLayout * control, QWidget * buddy);
    QLabel* addTallRow (const QString & label, QWidget * control, QWidget * buddy=0);
    void finish ();

  private:
    QLayout* addRow (QWidget* w);

  private:
    int myRow;
    bool myHasTall;
    QGridLayout * myGrid;
};

#endif // QTR_HIG_H
