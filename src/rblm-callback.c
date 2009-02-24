/*
 * Loudmouth to ruby callback
 * Used by ruby code for dispatching to proper handler
 */

#include "rblm-callback.h"
#include "rblm-private.h"
#include "rblm-lm2rb.h"
#include <ruby.h>

/* Ruby callback classes */
VALUE lm_cCallback;
VALUE lm_cMessageCallback;
VALUE lm_cStatusCallback;
VALUE lm_cDisconnectCallback;
VALUE lm_cSslCallback;

/* Create messages to be posted on async queue */
LmAsyncCallback*
create_async_message (LmAsyncNotification notification, VALUE block, gpointer data)
{
    LmAsyncCallback* cb = ALLOC (LmAsyncCallback);
    cb->notification = notification;
    cb->block = block;
    cb->data = data;
}

/* Ruby instance initialize */
VALUE
callback_initialize (int argc, VALUE *argv, VALUE self)
{
    VALUE target, msg_type;
    rb_scan_args (argc, argv, "22", &target, &msg_type);

    rb_iv_set (self, "@target", target);
    rb_iv_set (self, "@msg_type", msg_type);
}

VALUE
msg_callback_initialize (VALUE self, VALUE conn, VALUE message, VALUE msg_type)
{
    VALUE args[2];
    args[0] = conn;
    args[1] = msg_type;
    rb_call_super (2, args);
    rb_iv_set (self, "@message", message);
}

VALUE
status_callback_initialize (VALUE self, VALUE conn, VALUE success, VALUE msg_type)
{
    VALUE args[2];
    args[0] = conn;
    args[1] = msg_type;
    rb_call_super (2, args);
    rb_iv_set (self, "@success", success);
}

VALUE
disconnect_callback_initialize (VALUE self, VALUE conn, VALUE reason)
{
    VALUE args[2];
    args[0] = conn;
    args[1] = INT2FIX (LM_CB_DISCONNECT);
    rb_call_super (2, args);
    rb_iv_set (self, "@reason", reason);
}

VALUE
ssl_callback_initialize (VALUE self, VALUE ssl, VALUE status)
{
    VALUE args[2];
    args[0] = ssl;
    args[1] = INT2FIX (LM_CB_SSL);
    rb_call_super (2, args);
    rb_iv_set (self, "@status", status);
}

/* Convert Loudmouth callback user data to ruby object */
VALUE
lm_callback_to_ruby_object (LmAsyncCallback* callback)
{
    if (!callback)
        return Qnil;
    VALUE res;
    VALUE msg_type = INT2NUM (callback->notification);
    switch (callback->notification)
    {
        case LM_CB_MSG:
        case LM_CB_REPLY:
        {
            VALUE args[3];
            args[0] = callback->block;
            args[1] = LMMESSAGE2RVAL (GPOINTER2MSG (callback->data));
            args[2] = msg_type;
            res = rb_class_new_instance (3, args, lm_cMessageCallback);
            break;
        }
        case LM_CB_CONN_OPEN:
        case LM_CB_AUTH:
        {
            VALUE args[3];
            args[0] = callback->block;
            args[1] = GBOOL2RVAL (GPOINTER2GBOOL (callback->data));
            args[2] = msg_type;
            res = rb_class_new_instance (3, args, lm_cMessageCallback);
            break;
        }
        case LM_CB_DISCONNECT:
        {
            VALUE args[2];
            args[0] = callback->block;
            args[1] = INT2FIX (GPOINTER2DISCONNECT (callback->data));
            res = rb_class_new_instance (2, args, lm_cMessageCallback);
            break;
        }
        case LM_CB_SSL:
        {
            VALUE args[2];
            args[0] = callback->block;
            args[1] = INT2FIX (GPOINTER2SSLSTATUS (callback->data));
            res = rb_class_new_instance (2, args, lm_cSslCallback);
            break;
        }
        default:
            g_warning ("Unknown callback type '%d'\n", callback->notification);
    }

    return res;
}

void
Init_lm_callback(VALUE lm_mLM)
{
    lm_cCallback = rb_define_class_under (lm_mLM, "Callback", rb_cObject);
    rb_define_method (lm_cCallback, "initialize", callback_initialize, -1);
    rb_define_const (lm_cCallback, "CB_MSG", INT2FIX (LM_CB_MSG));
    rb_define_const (lm_cCallback, "CB_REPLY", INT2FIX (LM_CB_REPLY));
    rb_define_const (lm_cCallback, "CB_CONN_OPEN", INT2FIX (LM_CB_CONN_OPEN));
    rb_define_const (lm_cCallback, "CB_AUTH", INT2FIX (LM_CB_AUTH));
    rb_define_const (lm_cCallback, "CB_DISCONNECT", INT2FIX (LM_CB_DISCONNECT));
    rb_define_const (lm_cCallback, "CB_SSL", INT2FIX (LM_CB_SSL));
    rb_define_attr (lm_cCallback, "target", 1, 0);
    rb_define_attr (lm_cCallback, "msg_type", 1, 0);

    lm_cMessageCallback = rb_define_class_under (lm_mLM, "MessageCallback", lm_cCallback);
    rb_define_method (lm_cMessageCallback, "initialize", msg_callback_initialize, 3);
    rb_define_attr (lm_cMessageCallback, "message", 1, 0);

    lm_cStatusCallback = rb_define_class_under (lm_mLM, "StatusCallback", lm_cCallback);
    rb_define_method (lm_cStatusCallback, "initialize", status_callback_initialize, 3);
    rb_define_attr (lm_cMessageCallback, "success", 1, 0);

    lm_cDisconnectCallback = rb_define_class_under (lm_mLM, "DisconnectCallback", lm_cCallback);
    rb_define_method (lm_cDisconnectCallback, "initialize", disconnect_callback_initialize, 2);
    rb_define_attr (lm_cMessageCallback, "reason", 1, 0);

    lm_cSslCallback = rb_define_class_under (lm_mLM, "SslCallback", lm_cCallback);
    rb_define_method (lm_cSslCallback, "initialize", ssl_callback_initialize, 2);
    rb_define_attr (lm_cSslCallback, "status", 1, 0);
}

