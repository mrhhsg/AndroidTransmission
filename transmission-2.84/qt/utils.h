/*
 * This file Copyright (C) 2009-2014 Mnemosyne LLC
 *
 * It may be used under the GNU GPL versions 2 or 3
 * or any future license endorsed by Mnemosyne LLC.
 *
 * $Id: utils.h 14241 2014-01-21 03:10:30Z jordan $
 */

#ifndef QTR_UTILS
#define QTR_UTILS

#include <QString>
#include <QObject>
#include <QIcon>

#include <cctype> // isxdigit()

#include "speed.h"

class Utils: public QObject
{
    Q_OBJECT

  public:
    Utils () {}
    virtual ~Utils () {}

  public:
    static QString remoteFileChooser (QWidget * parent, const QString& title, const QString& myPath, bool dir, bool local);
    static QIcon guessMimeIcon (const QString& filename);
    // Test if string is UTF-8 or not
    static bool isValidUtf8  (const char *s);

    static QString removeTrailingDirSeparator (const QString& path);

    // meh
    static void toStderr (const QString& qstr);

    ///
    /// URLs
    ///

    static bool isMagnetLink (const QString& s)
    {
      return s.startsWith (QString::fromUtf8 ("magnet:?"));
    }

    static bool isHexHashcode (const QString& s)
    {
      if (s.length() != 40)
        return false;
      foreach (QChar ch, s) if (!isxdigit (ch.unicode())) return false;
      return true;
    }

    static bool isUriWithSupportedScheme (const QString& s)
    {
      static const QString ftp = QString::fromUtf8 ("ftp://");
      static const QString http = QString::fromUtf8 ("http://");
      static const QString https = QString::fromUtf8 ("https://");
      return s.startsWith(http) || s.startsWith(https) || s.startsWith(ftp);
    }
};

#endif
