/*
 * This file Copyright (C) 2012-2014 Mnemosyne LLC
 *
 * It may be used under the GNU Public License v2 or v3 licenses,
 * or any future license endorsed by Mnemosyne LLC.
 *
 * $Id: dbus-adaptor.cc 14225 2014-01-19 01:09:44Z jordan $
 */

#include "add-data.h"
#include "app.h"
#include "dbus-adaptor.h"

TrDBusAdaptor :: TrDBusAdaptor (MyApp* app):
  QDBusAbstractAdaptor (app),
  myApp (app)
{
}

bool
TrDBusAdaptor :: PresentWindow ()
{
  myApp->raise ();
  return true;
}

bool
TrDBusAdaptor :: AddMetainfo (const QString& key)
{
  AddData addme (key);

  if (addme.type != addme.NONE)
    myApp->addTorrent (addme);

  return true;
}
