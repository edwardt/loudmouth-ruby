#include "rblm.h"
#include "rblm-callback.h"
#include "rblm-synchronizer.h"

static VALUE lm_cSink;

static void
sink_free (void* _)
{
    rblm_shutdown_sync();
}

static VALUE
sink_allocate (VALUE klass)
{
    return Data_Wrap_Struct (klass, NULL, sink_free, NULL);
}

static VALUE
sink_initialize (int argc, VALUE *argv, VALUE self)
{
    rblm_init_sync();
}

static VALUE
sink_file_descriptor (VALUE self)
{
    return INT2NUM (g_io_channel_unix_get_fd (lm2rb_read));
}

static VALUE
sink_notification (VALUE self)
{
    LmAsyncCallback* cb = (LmAsyncCallback*)g_async_queue_pop(lm2rb_queue);
    if (cb)
        return lm_callback_to_ruby_object (cb);
    else
        return Qnil;
}

void
Init_lm_sink (VALUE lm_mLM)
{
    lm_cSink = rb_define_class_under (lm_mLM, "Sink", rb_cObject);

    rb_define_alloc_func (lm_cSink, sink_allocate);
    rb_define_method (lm_cSink, "initialize", sink_initialize, -1);
    rb_define_method (lm_cSink, "file_descriptor", sink_file_descriptor, 0);
    rb_define_method (lm_cSink, "notification", sink_notification, 0);
}

