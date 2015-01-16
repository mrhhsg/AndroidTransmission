/*
 * This file Copyright (C) 2009-2014 Mnemosyne LLC
 *
 * It may be used under the GNU GPL versions 2 or 3
 * or any future license endorsed by Mnemosyne LLC.
 *
 * $Id: watchdir.h 14241 2014-01-21 03:10:30Z jordan $
 */

#ifndef QTR_WATCHDIR_H
#define QTR_WATCHDIR_H

#include <QObject>
#include <QSet>
#include <QString>

class TorrentModel;
class QFileSystemWatcher;

class WatchDir: public QObject
{
    Q_OBJECT

  public:
    WatchDir (const TorrentModel&);
    ~WatchDir ();

  public:
    void setPath (const QString& path, bool isEnabled);

  private:
    enum { OK, DUPLICATE, ERROR };
    int metainfoTest (const QString& filename) const;

  signals:
    void torrentFileAdded (QString filename);

  private slots:
    void watcherActivated (const QString& path);
    void onTimeout ();

  private:
    const TorrentModel& myModel;
    QSet<QString> myWatchDirFiles;
    QFileSystemWatcher * myWatcher;
};

#endif
