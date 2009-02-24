/* 
 * File:   rblm-lm2rb.h
 * Author: raphael
 *
 * Created on February 19, 2009, 9:58 PM
 */

#ifndef _RBLM_LM2RB_H
#define	_RBLM_LM2RB_H

#include <glib.h>
#include <loudmouth/loudmouth.h>

/* Asynchronous Message queue */
extern GAsyncQueue* lm2rb_queue;

/* Loudmouth event handlers */
LmHandlerResult msg_handler (LmMessageHandler *handler, LmConnection *connection, LmMessage *message, gpointer user_data);
LmHandlerResult reply_handler (LmMessageHandler *handler, LmConnection *connection, LmMessage *message, gpointer *user_data);
void open_handler (LmConnection *conn, gboolean success, gpointer user_data);
void auth_handler (LmConnection *conn, gboolean success, gpointer user_data);
void disconnect_handler (LmConnection *conn, LmDisconnectReason  reason, gpointer user_data);
LmSSLResponse ssl_handler (LmSSL *ssl, LmSSLStatus  status, gpointer user_data);

/* Event packaging */
#define MSG2GPOINTER(m)        ((gpointer)(m))
#define GPOINTER2MSG(p)        ((LmMessage*)(p))
#define GBOOL2GPOINTER(b)      ((gpointer)(b))
#define GPOINTER2GBOOL(p)      ((gboolean)(p))
#define DISCONNECT2GPOINTER(d) ((gpointer)(d))
#define GPOINTER2DISCONNECT(p) ((LmDisconnectReason)(p))
#define SSLSTATUS2GPOINTER(s)  ((gpointer)(s))
#define GPOINTER2SSLSTATUS(p)  ((LmSSLStatus)(p))

#endif	/* _RBLM_LM2RB_H */

