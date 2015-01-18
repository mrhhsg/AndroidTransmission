/*
 * This file Copyright (C) 2008-2014 Mnemosyne LLC
 *
 * It may be used under the GNU GPL versions 2 or 3
 * or any future license endorsed by Mnemosyne LLC.
 *
 * $Id: rpcimpl.h 14241 2014-01-21 03:10:30Z jordan $
 */

#ifndef TR_RPC_H
#define TR_RPC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "transmission.h"
#include "variant.h"

/***
****  RPC processing
***/

struct evbuffer;

typedef void (*tr_rpc_response_func)(tr_session      * session,
                                     struct evbuffer * response,
                                     void            * user_data);
/* http://www.json.org/ */
void tr_rpc_request_exec_json (tr_session            * session,
                               const void            * request_json,
                               int                     request_len,
                               tr_rpc_response_func    callback,
                               void                  * callback_user_data);

/* see the RPC spec's "Request URI Notation" section */
void tr_rpc_request_exec_uri (tr_session           * session,
                              const void           * request_uri,
                              int                    request_len,
                              tr_rpc_response_func   callback,
                              void                 * callback_user_data);

void tr_rpc_parse_list_str (tr_variant  * setme,
                            const char  * list_str,
                            int           list_str_len);

#ifdef __cplusplus
}
#endif

#endif /* TR_RPC_H */
