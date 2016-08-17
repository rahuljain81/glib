//gcc  `pkg-config --libs --cflags glib-2.0 gthread-2.0` gthread_callback.c

#include <glib.h>

typedef struct 
{
  GMainLoop *loop;
  guint n_run;
} LoopInfo;

gboolean callback (LoopInfo *info)
{
  info->n_run -= 1;

  g_print ("loop %p n_run: %u\n", info->loop, info->n_run);

  if (info->n_run <= 0)
    {
      g_main_loop_quit (info->loop);
      g_print ("loop %p quit\n", info->loop);
      return FALSE;
    }

  return TRUE;
}

gpointer *thread1 (gpointer *data)
{
  GMainLoop *loop = g_main_loop_new (NULL, FALSE);
  GSource *source = g_timeout_source_new (2250);
  LoopInfo info = { .loop = loop, .n_run = 3 };

  g_source_set_callback (source, (GSourceFunc) callback, &info, NULL);
  g_source_attach (source, NULL);
  g_print ("Thread loop %p run\n", loop);
  g_main_loop_run (loop);

  return NULL;
}

gint main(void)
{
  GThread *thread = g_thread_new ("thread1", (GThreadFunc) thread1, NULL);

  //GMainContext *ctx = g_main_context_new ();
  GMainLoop *loop = g_main_loop_new (NULL, FALSE); //ctx, FALSE);
  GSource *source = g_timeout_source_new_seconds (1);
  LoopInfo info = { .loop = loop, .n_run = 5 };

  g_source_set_callback (source, (GSourceFunc) callback, &info, NULL);
  g_source_attach (source, NULL); //ctx);
  g_print ("Main loop %p run\n", loop);
  g_main_loop_run (loop);
  g_thread_join (thread);

  return 0;
}
