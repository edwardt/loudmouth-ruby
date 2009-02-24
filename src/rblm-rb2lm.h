/* 
 * Ruby to GLib thread synchronization layer.
 * Ruby calls to the Loudmouth API that require synchronization cause the
 * GLib event loop to pause while the API runs.
 */

#ifndef _RBLM_RB2LM_H
#define	_RBLM_RB2LM_H

/* Macro used to make synchronized calls to the Loudmouth API */

/* Ruby to GLib thread synchronization setup */
void rb2lm_init();

/* Ruby to GLib thread synchronization shutdown */
void rb2lm_shutdown();

/* 'Pause' GLib */
void rb2lm_pause_glib();

/* 'Resume' GLib */
void rb2lm_resume_glib();

#endif	/* _RBLM_RB2LM_H */

