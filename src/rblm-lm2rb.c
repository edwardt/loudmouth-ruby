/* GLib to EM Synchronization */

#include "rblm-callback.h"
#include "rblm-synchronizer.h"
#include "rblm-lm2rb.h"

GAsyncQueue* lm2rb_queue = NULL;

/* Initializes GLib to ruby synchronization layer *
 * Runs in ruby thread                            */
void
lm2rb_init()
{
    lm2rb_queue = g_async_queue_new();
}

/* Shutdown GLib to ruby synchronization layer */
void
lm2rb_shutdown()
{
    g_async_queue_unref (lm2rb_queue);
}

/* Notify ruby of message callback:  *
 *    1. Push message to async queue *
 *    2. Notify ruby through pipe    */
static void
notify_ruby (LmAsyncNotification notification,
             VALUE block,
             gpointer data)
{
    gsize written;
    GError* error = NULL;
    LmAsyncCallback* cb = create_async_message (notification, block, data);
    g_async_queue_push (lm2rb_queue, (gpointer)cb);
    if (g_io_channel_write_chars (lm2rb_write, "1", 1, &written, &error) == G_IO_STATUS_ERROR)
    {
        g_error ("Failed to write to Loudmouth to Ruby pipe: %s\n", error->message);
        g_error_free (error);
    }
}

/* Handlers that get called back by Loudmouth in GLib thread */
LmHandlerResult
msg_handler (LmMessageHandler *handler,
             LmConnection     *connection,
             LmMessage        *message,
             gpointer          user_data)
{
    notify_ruby (LM_CB_MSG, (VALUE)user_data, MSG2GPOINTER(message));

    return LM_HANDLER_RESULT_ALLOW_MORE_HANDLERS;
}

LmHandlerResult
reply_handler (LmMessageHandler *handler,
               LmConnection *connection,
               LmMessage *message,
               gpointer *user_data)
{
    notify_ruby (LM_CB_REPLY, (VALUE)user_data, MSG2GPOINTER(message));

    return LM_HANDLER_RESULT_ALLOW_MORE_HANDLERS;
}

void open_handler (LmConnection *conn,
                   gboolean success,
                   gpointer user_data)
{
    notify_ruby (LM_CB_CONN_OPEN, (VALUE)user_data, GBOOL2GPOINTER(success));
}

void auth_handler (LmConnection *conn,
                   gboolean success,
                   gpointer user_data)
{
    notify_ruby (LM_CB_AUTH, (VALUE)user_data, GBOOL2GPOINTER(success));
}

void disconnect_handler (LmConnection *conn, 
                         LmDisconnectReason  reason,
                         gpointer user_data)
{
    notify_ruby (LM_CB_CONN_OPEN, (VALUE)user_data, DISCONNECT2GPOINTER(reason));
}

LmSSLResponse ssl_handler (LmSSL *ssl, LmSSLStatus status, gpointer user_data)
{
    notify_ruby (LM_CB_SSL, (VALUE)user_data, SSLSTATUS2GPOINTER(status));

    /* TODO: We need to get this from ruby, use a blocking pipe read? */
    return LM_SSL_RESPONSE_CONTINUE;
}
