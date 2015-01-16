/*
 * This file Copyright (C) 2010-2014 Mnemosyne LLC
 *
 * It may be used under the GNU GPL versions 2 or 3
 * or any future license endorsed by Mnemosyne LLC.
 *
 * $Id: tracker-model.h 14241 2014-01-21 03:10:30Z jordan $
 */

#ifndef QTR_TRACKER_MODEL_H
#define QTR_TRACKER_MODEL_H

#include <QAbstractListModel>
#include <QSet>
#include <QVector>

#include "torrent.h"
#include "torrent-model.h"

struct TrackerInfo
{
    TrackerStat st;
    int torrentId;
};
Q_DECLARE_METATYPE(TrackerInfo)

class TrackerModel: public QAbstractListModel
{
    Q_OBJECT

    typedef QVector<TrackerInfo> rows_t;
    rows_t myRows;

  public:
    void refresh (const TorrentModel&, const QSet<int>& ids);
    int find (int torrentId, const QString& url) const;

  public:
    virtual int rowCount (const QModelIndex& parent = QModelIndex()) const;
    virtual QVariant data (const QModelIndex& index, int role = Qt::DisplayRole) const;
    enum Role { TrackerRole = Qt::UserRole };

  public:
    TrackerModel () {}
    virtual ~TrackerModel () {}
};

#endif
