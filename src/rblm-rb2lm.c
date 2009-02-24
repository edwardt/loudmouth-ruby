/* Ruby to GLib Synchronization (see rbml-synchronizer.h for details) */

#include "rblm-rb2lm.h"
#include "rblm-synchronizer.h"
#include "rblm-lm2rb.h"

/* Condition variables used to pause/resume GLib event loop */
GCond* loop_paused = NULL;
GMutex* loop_paused_mx = NULL;
GCond* loop_resumed = NULL;
GMutex* loop_resumed_mx = NULL;

/* Handle notification coming from ruby thread */
static gboolean
rb2lm_notify (GIOChannel* source, GIOCondition cond, gpointer data)
{
    if (cond & G_IO_HUP)
        g_warning ("Read end of ruby to GLib pipe broken");

    g_cond_signal (loop_paused);
    g_cond_wait (loop_resumed, loop_resumed_mx);
    g_mutex_unlock (loop_resumed_mx);

    return TRUE;
}

/* Initializes ruby to GLib synchronization layer *
 * Runs in GLib thread                            */
void
rb2lm_init()
{
    loop_paused = g_cond_new();
    loop_paused_mx = g_mutex_new();
    loop_resumed = g_cond_new();
    loop_resumed_mx = g_mutex_new();

    (void)g_async_queue_ref (lm2rb_queue);

    if (!g_io_add_watch (rb2lm_read, G_IO_IN | G_IO_HUP, rb2lm_notify, NULL))
        g_error ("Failed to add watch on IO Channel");
}

/* Shut down GLib to ruby synchronization layer *
 * Runs in GLib thread                          */
void
rb2lm_shutdown()
{
    g_async_queue_unref (lm2rb_queue);
}

void
rb2lm_pause_glib()
{
    gsize written;
    GError* error = NULL;
    if (g_io_channel_write_chars (rb2lm_write, "1", 1, &written, &error) == G_IO_STATUS_ERROR)
    {
      g_warning ("Failed to write into Ruby to Loudmouth pipe: %s\n", error->message);
      g_error_free (error);
    }
    g_cond_wait (loop_paused, loop_paused_mx);
    g_mutex_unlock (loop_paused_mx);
}

void
rb2lm_resume_glib()
{
    g_cond_signal (loop_resumed);
}