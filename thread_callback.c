//gcc  `pkg-config --libs --cflags glib-2.0 gthread-2.0` gthread_callback.c
#include <stdio.h>
#include <glib.h>


#define THREAD_TIMEOUTS (10)
#define MAIN_TIMEOUS (5)

typedef struct timeout_struct
{
    int i;
    int max;
    GMainLoop *loop;
    char *name;

} TIMEOUT_STRUCT;

gboolean timeout_callback(gpointer data)
{
    TIMEOUT_STRUCT *psTimeout = (TIMEOUT_STRUCT *)data;

	printf ("%d, %s\n", psTimeout->i, psTimeout->name);
    psTimeout->i++;

    if (psTimeout->i == psTimeout->max)
    {
        if (psTimeout->max == THREAD_TIMEOUTS)
        {
            g_main_loop_quit( (GMainLoop*)psTimeout->loop );
        }
        return FALSE;
    }

    return TRUE;
}
void* thread_function(void *data)
{
    GMainContext *ps_context;
    GMainLoop *ps_loop;
    GSource *ps_timer;
    TIMEOUT_STRUCT sTimeout;

    ps_context = g_main_context_new();
    g_main_context_push_thread_default(ps_context);

    ps_loop = g_main_loop_new(ps_context, FALSE);

    sTimeout.i = 0;
    sTimeout.max = THREAD_TIMEOUTS;
    sTimeout.loop = ps_loop;
    sTimeout.name = "thread";
    ps_timer = g_timeout_source_new_seconds(1);
    g_source_set_callback(ps_timer, timeout_callback, &sTimeout, NULL);
    g_source_attach(ps_timer, ps_context);

    g_main_loop_run(ps_loop);

    g_main_loop_quit( (GMainLoop*)data );

    return NULL;

}
/*
 * This main boots a thread, then starts up a GMainLoop.  Then the thread runs
 * a GMainLoop.  The thread sets a timer that fires ten times and the main sets a
 * timer that fires two times. The thread quits and
 * and then the other main l
 *
 *
 * */
int main()
{
    GThread *ps_thread;
    GMainLoop *loop;
    TIMEOUT_STRUCT sTimeout;

    loop = g_main_loop_new ( NULL , FALSE );

    sTimeout.i = 0;
    sTimeout.max = MAIN_TIMEOUS;
    sTimeout.loop = loop;
    sTimeout.name = "main";

    // add source to default context
    g_timeout_add (1 , timeout_callback, &sTimeout);

    ps_thread = g_thread_new("thread", thread_function, loop);

    g_main_loop_run (loop);
    g_main_loop_unref(loop);
}
