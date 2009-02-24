/* Synchronizes the GLib and Ruby threads using an async queue and pipes.
 * 
 * Ruby to GLib thread synchronization
 *
 * Calls from the ruby code into Loudmouth must run whille the GLib event loop 
 * is stopped to avoid concurrent calls accessing GLib data structures with no
 * synchronization. This can be done by triggering an event through a pipe. 
 * The event handler should then signal the ruby thread that the event loop is
 * paused. The ruby thread can then proceed with calling the Loudmouth API. 
 * Once the Loudmouth API call returns the ruby thread can signal back the 
 * GLib thread and tell it to resume the event loop.
 * Ruby methods that need synchronization include all calls to Loudmouth
 * functions that access data also accessed by GLib events, including:
 *   - all calls to LM::Connection
 *
 * GLib to Ruby Synchronization
 *
 * The other way around requires an actual context switch: the event triggered
 * by GLib must run in the ruby thread. To achieve this, the GLib event posts a
 * message to an asynchronous queue and writes to a pipe to let the ruby thread
 * know that there is a message for it. The ruby thread then picks up the
 * message and dispatches it to the right event handler.
 * Loudmouth/GLib events that need to be sent to the Ruby thread are:
 *   - new message notifications (msg_handler_cb)
 *   - reply notifications (msg_handler_for_send_cb)
 *   - connection open notifications (open_callback)
 *   - authentication notifications (auth_callback)
 *   - disconnection notifications (disconnect_cb)
 *   - SSL notifications (ssl_function_callback)
 *
 * Packaging
 *
 * The Loudmouth ruby binding supports both blocking and threaded behaviors.
 */

#ifndef _RBLM_SYNCHRONIZER_H
#define	_RBLM_SYNCHRONIZER_H

#include "rblm.h"

/* Pipes used to notify of pending queue items */
extern GIOChannel* rb2lm_read;
extern GIOChannel* rb2lm_write;
extern GIOChannel* lm2rb_read;
extern GIOChannel* lm2rb_write;

/* GLib event loop main context */
extern GMainContext* main_context;

/* Initialize queues, pipes and the GLib event loop, call from ruby thread */
void rblm_init_sync();

/* Shut down synchronization layer, call from ruby thread */
void rblm_shutdown_sync();

/* Helper to create pipe and its channels */
void rblm_create_pipe (GIOChannel** ch_read, GIOChannel** ch_write);

#endif	/* _RBLM_SYNCHRONIZER_H */

