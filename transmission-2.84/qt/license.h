/*
 * This file Copyright (C) 2009-2014 Mnemosyne LLC
 *
 * It may be used under the GNU GPL versions 2 or 3
 * or any future license endorsed by Mnemosyne LLC.
 *
 * $Id: license.h 14241 2014-01-21 03:10:30Z jordan $
 */

#ifndef LICENSE_DIALOG_H
#define LICENSE_DIALOG_H

#include <QDialog>

class LicenseDialog: public QDialog
{
    Q_OBJECT

  public:
    LicenseDialog (QWidget * parent = 0);
    ~LicenseDialog () {}
};

#endif

