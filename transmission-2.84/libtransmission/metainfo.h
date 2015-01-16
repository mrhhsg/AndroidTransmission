/*
 * This file Copyright (C) 2005-2014 Mnemosyne LLC
 *
 * It may be used under the GNU GPL versions 2 or 3
 * or any future license endorsed by Mnemosyne LLC.
 *
 * $Id: metainfo.h 14241 2014-01-21 03:10:30Z jordan $
 */

#ifndef __TRANSMISSION__
 #error only libtransmission should #include this header.
#endif

#ifndef TR_METAINFO_H
#define TR_METAINFO_H 1

#include "transmission.h"
#include "variant.h"

bool  tr_metainfoParse (const tr_session  * session,
                        const tr_variant  * variant,
                        tr_info           * setmeInfo,
                        bool              * setmeHasInfoDict,
                        int               * setmeInfoDictLength);

void tr_metainfoRemoveSaved (const tr_session * session,
                             const tr_info    * info);

char* tr_metainfoGetBasename (const tr_info *);


#endif
