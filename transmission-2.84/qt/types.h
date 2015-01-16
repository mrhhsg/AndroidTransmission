/*
 * This file Copyright (C) 2012-2014 Mnemosyne LLC
 *
 * It may be used under the GNU GPL versions 2 or 3
 * or any future license endorsed by Mnemosyne LLC.
 *
 * $Id: types.h 14241 2014-01-21 03:10:30Z jordan $
 */

#ifndef QTR_TYPES_H
#define QTR_TYPES_H

#include <QVariant>

class TrTypes
{
  public:

    enum
    {
      TrackerStatsList = QVariant::UserType,
      PeerList = QVariant::UserType,
      FileList,
      FilterModeType,
      SortModeType
    };
};

#endif
