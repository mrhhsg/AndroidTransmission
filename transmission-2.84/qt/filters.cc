/*
 * This file Copyright (C) 2009-2014 Mnemosyne LLC
 *
 * It may be used under the GNU Public License v2 or v3 licenses,
 * or any future license endorsed by Mnemosyne LLC.
 *
 * $Id: filters.cc 14225 2014-01-19 01:09:44Z jordan $
 */

#include "filters.h"

const QString FilterMode::names[NUM_MODES] =
{
  "show-all",
  "show-active",
  "show-downloading",
  "show-seeding",
  "show-paused",
  "show-finished",
  "show-verifying",
  "show-error",
};

int
FilterMode :: modeFromName( const QString& name )
{
  for (int i=0; i<NUM_MODES; ++i)
    if( names[i] == name )
      return i;

  return FilterMode().mode(); // use the default value
}

const QString SortMode::names[NUM_MODES] =
{
  "sort-by-activity",
  "sort-by-age",
  "sort-by-eta",
  "sort-by-name",
  "sort-by-progress",
  "sort-by-queue"
  "sort-by-ratio",
  "sort-by-size",
  "sort-by-state",
  "sort-by-id"
};

int
SortMode :: modeFromName (const QString& name)
{
  for (int i=0; i<NUM_MODES; ++i)
    if (names[i] == name)
      return i;

  return SortMode().mode(); // use the default value
}
