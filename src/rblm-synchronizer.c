/* Synchronization between Glib and Ruby threads */

#include "rblm-synchronizer.h"
#include <errno.h>
#include <string.h>

GIOChannel* rb2lm_read = NULL;
GIOChannel* rb2lm_write = NULL;
GIOChannel* lm2rb_read = NULL;
GIOChannel* lm2rb_write = NULL;
GMainContext* main_context = NULL;
static GThread* glib_thread = NULL;

/* GLib main loop */
static GMainLoop* main_loop;

/* Helper method to create pipe IO channels */
void
rblm_create_pipe (GIOChannel** ch_read, GIOChannel** ch_write)
{
    int fd[2], ret;
    ret = pipe (fd);
    if (ret == -1)
    {
        g_warning ("Creating pipe failed: error %s\n", strerror (errno));
        return;
    }
    if (ch_read)
        *ch_read = g_io_channel_unix_new (fd[0]);
    if (ch_write)
        *ch_write = g_io_channel_unix_new (fd[1]);
}

/* GLib event loop thread function */
static gpointer
loop_thread (gpointer _)
{
    rb2lm_init();
    g_main_loop_run (main_loop);
    rb2lm_shutdown();

    return NULL;
}

/* Initialize queues, pipes and the GLib event loop, call from ruby thread */
void
rblm_init_sync()
{
    g_thread_init(NULL);
    main_context = g_main_context_new();

    rblm_create_pipe (&rb2lm_read, &rb2lm_write);
    rblm_create_pipe (&lm2rb_read, &lm2rb_write);

    lm2rb_init();

    main_loop = g_main_loop_new (main_context, FALSE);
    GError* error = NULL;
    glib_thread = g_thread_create ((GThreadFunc)&loop_thread, /* func     */
                                       NULL,                  /* data     */
                                       TRUE,                  /* joinable */
                                       &error);               /* error    */
    if (error)
    {
        g_error ("Could not start GLib thread: %s\n", error->message);
        g_error_free (error);
    }
}

/* Shut down synchronization layer, call from ruby thread */
void
rblm_shutdown_sync()
{
    g_main_loop_quit (main_loop);
    g_thread_join (glib_thread);
    g_io_channel_shutdown (lm2rb_read, FALSE, NULL);
    g_io_channel_shutdown (lm2rb_write, FALSE, NULL);
    g_io_channel_shutdown (rb2lm_read, FALSE, NULL);
    g_io_channel_shutdown (rb2lm_write, FALSE, NULL);
    g_io_channel_unref (lm2rb_read);
    g_io_channel_unref (lm2rb_write);
    g_io_channel_unref (rb2lm_read);
    g_io_channel_unref (rb2lm_write);
    g_main_context_unref (main_context);
}

