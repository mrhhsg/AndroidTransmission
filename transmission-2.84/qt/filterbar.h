/*
 * This file Copyright (C) 2010-2014 Mnemosyne LLC
 *
 * It may be used under the GNU GPL versions 2 or 3
 * or any future license endorsed by Mnemosyne LLC.
 *
 * $Id: filterbar.h 14241 2014-01-21 03:10:30Z jordan $
 */

#ifndef QTR_FILTERBAR_H
#define QTR_FILTERBAR_H

#include <QComboBox>
#include <QItemDelegate>
#include <QWidget>

class QLabel;
class QLineEdit;
class QPaintEvent;
class QStandardItemModel;
class QTimer;

class Prefs;
class TorrentFilter;
class TorrentModel;

class FilterBarComboBoxDelegate: public QItemDelegate
{
    Q_OBJECT

  public:
    FilterBarComboBoxDelegate (QObject * parent, QComboBox * combo);

  public:
    static bool isSeparator (const QModelIndex &index);
    static void setSeparator (QAbstractItemModel * model, const QModelIndex& index);

  protected:
    virtual void paint (QPainter*, const QStyleOptionViewItem&, const QModelIndex&) const;
    virtual QSize sizeHint (const QStyleOptionViewItem&, const QModelIndex&) const;

  private:
    QComboBox * myCombo;

};

class FilterBarComboBox: public QComboBox
{
    Q_OBJECT

  public:
    FilterBarComboBox (QWidget * parent = 0);
    int currentCount () const;

  protected:
    virtual void paintEvent (QPaintEvent * e);
};


class FilterBar: public QWidget
{
    Q_OBJECT

  public:
    FilterBar (Prefs& prefs, TorrentModel& torrents, TorrentFilter& filter, QWidget * parent = 0);
    ~FilterBar ();

  private:
    FilterBarComboBox * createTrackerCombo (QStandardItemModel * );
    FilterBarComboBox * createActivityCombo ();
    void recountSoon ();
    void refreshTrackers ();
    QString getCountString (int n) const;

  private:
    Prefs& myPrefs;
    TorrentModel& myTorrents;
    TorrentFilter& myFilter;
    FilterBarComboBox * myActivityCombo;
    FilterBarComboBox * myTrackerCombo;
    QLabel * myCountLabel;
    QStandardItemModel * myTrackerModel;
    QTimer * myRecountTimer;
    bool myIsBootstrapping;
    QLineEdit * myLineEdit;

  private slots:
    void recount ();
    void refreshPref (int key);
    void refreshCountLabel ();
    void onActivityIndexChanged (int index);
    void onTrackerIndexChanged (int index);
    void onTorrentModelReset ();
    void onTorrentModelRowsInserted (const QModelIndex&, int, int);
    void onTorrentModelRowsRemoved (const QModelIndex&, int, int);
    void onTorrentModelDataChanged (const QModelIndex&, const QModelIndex&);
    void onTextChanged (const QString&);
};

#endif
