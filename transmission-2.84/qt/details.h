/*
 * This file Copyright (C) 2009-2014 Mnemosyne LLC
 *
 * It may be used under the GNU GPL versions 2 or 3
 * or any future license endorsed by Mnemosyne LLC.
 *
 * $Id: details.h 14241 2014-01-21 03:10:30Z jordan $
 */

#ifndef DETAILS_DIALOG_H
#define DETAILS_DIALOG_H

#include <QDialog>
#include <QString>
#include <QMap>
#include <QSet>
#include <QTimer>
#include <QWidgetList>

#include "prefs.h"

class FileTreeView;
class QTreeView;
class QComboBox;
class QCheckBox;
class QDoubleSpinBox;
class QLabel;
class QRadioButton;
class QSpinBox;
class QTextBrowser;
class QTreeWidget;
class QTreeWidgetItem;
class Session;
class Torrent;
class TorrentModel;
class TrackerDelegate;
class TrackerModel;
class TrackerModelFilter;

class Details: public QDialog
{
    Q_OBJECT

  private:
    void getNewData ();

  private slots:
    void onTorrentChanged ();
    void onTimer ();

  public:
    Details (Session&, Prefs&, TorrentModel&, QWidget * parent = 0);
    ~Details ();
    void setIds (const QSet<int>& ids);
    virtual QSize sizeHint () const { return QSize (440, 460); }

  private:
    QWidget * createPeersTab ();
    QWidget * createTrackerTab ();
    QWidget * createInfoTab ();
    QWidget * createFilesTab ();
    QWidget * createOptionsTab ();

  private:
    QIcon getStockIcon (const QString& freedesktop_name, int fallback);
    QString timeToStringRounded (int seconds);
    QString trimToDesiredWidth (const QString& str);
    void enableWhenChecked (QCheckBox *, QWidget *);

  private:
    Session& mySession;
    Prefs& myPrefs;
    TorrentModel& myModel;
    QSet<int> myIds;
    QTimer myTimer;
    bool myChangedTorrents;
    bool myHavePendingRefresh;

    QLabel * myStateLabel;
    QLabel * myHaveLabel;
    QLabel * myAvailabilityLabel;
    QLabel * myDownloadedLabel;
    QLabel * myUploadedLabel;
    QLabel * myErrorLabel;
    QLabel * myRunTimeLabel;
    QLabel * myETALabel;
    QLabel * myLastActivityLabel;

    QCheckBox * mySessionLimitCheck;
    QCheckBox * mySingleDownCheck;
    QCheckBox * mySingleUpCheck;
    QCheckBox * myShowTrackerScrapesCheck;
    QCheckBox * myShowBackupTrackersCheck;
    QPushButton * myAddTrackerButton;
    QPushButton * myEditTrackerButton;
    QPushButton * myRemoveTrackerButton;
    QSpinBox * mySingleDownSpin;
    QSpinBox * mySingleUpSpin;
    QComboBox * myRatioCombo;
    QDoubleSpinBox * myRatioSpin;
    QComboBox * myIdleCombo;
    QSpinBox * myIdleSpin;
    QSpinBox * myPeerLimitSpin;
    QComboBox * myBandwidthPriorityCombo;

    QLabel * mySizeLabel;
    QLabel * myHashLabel;
    QLabel * myPrivacyLabel;
    QLabel * myOriginLabel;
    QLabel * myLocationLabel;
    QTextBrowser * myCommentBrowser;

    QLabel * myTrackerLabel;
    QLabel * myScrapeTimePrevLabel;
    QLabel * myScrapeTimeNextLabel;
    QLabel * myScrapeResponseLabel;
    QLabel * myAnnounceTimePrevLabel;
    QLabel * myAnnounceTimeNextLabel;
    QLabel * myAnnounceResponseLabel;
    QLabel * myAnnounceManualLabel;

    TrackerModel * myTrackerModel;
    TrackerModelFilter * myTrackerFilter;
    TrackerDelegate * myTrackerDelegate;
    QTreeView * myTrackerView;
    //QMap<QString,QTreeWidgetItem*> myTrackerTiers;
    //QMap<QString,QTreeWidgetItem*> myTrackerItems;

    QTreeWidget * myPeerTree;
    QMap<QString,QTreeWidgetItem*> myPeers;

    QWidgetList myWidgets;

    FileTreeView * myFileTreeView;

  private slots:
    void refreshPref (int key);
    void onBandwidthPriorityChanged (int);
    void onFilePriorityChanged (const QSet<int>& fileIndices, int);
    void onFileWantedChanged (const QSet<int>& fileIndices, bool);
    void onPathEdited (const QString& oldpath, const QString& newname);
    void onOpenRequested (const QString& path);
    void onHonorsSessionLimitsToggled (bool);
    void onDownloadLimitedToggled (bool);
    void onSpinBoxEditingFinished ();
    void onUploadLimitedToggled (bool);
    void onRatioModeChanged (int);
    void onIdleModeChanged (int);
    void onShowTrackerScrapesToggled (bool);
    void onShowBackupTrackersToggled (bool);
    void onTrackerSelectionChanged ();
    void onAddTrackerClicked ();
    void onEditTrackerClicked ();
    void onRemoveTrackerClicked ();
    void refresh ();
};

#endif
