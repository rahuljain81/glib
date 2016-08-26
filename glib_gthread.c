#include <glib.h>
#include <stdio.h>
#include <unistd.h>

int fd[2];

void *thread_function(void *data)
{
	// call sleep so that the main loop source is not ready immediately
	sleep(10);

	write(fd[1],"GIOChannel main loop example\n",31);
	return NULL;
}

gboolean my_callback(GIOChannel *source,GIOCondition condition,gpointer data)
{
	char buf[100];	

	switch (condition)
	{
		case G_IO_IN:
			{
				printf ("GIOCondition %d\n", condition);
				read(fd[0],buf,sizeof(buf));
				g_print("%s",buf);
			}
			break;
		default:
			printf ("no data\n");
			break;
	}

	getchar();

	g_main_loop_quit((GMainLoop *)data);

	return FALSE;
}

int main()
{
	GMainLoop *loop;
	GIOChannel *channel;

	pipe(fd);

	channel = g_io_channel_unix_new(fd[0]);

	g_thread_new("Rahul", thread_function,NULL);

	loop = g_main_loop_new(NULL,FALSE);
	g_io_add_watch(channel,G_IO_IN | G_IO_HUP | G_IO_ERR,(GIOFunc)my_callback,loop);
	g_main_loop_run(loop);

	printf ("Exiting\n");

	g_io_channel_shutdown(channel,TRUE,NULL);

	return 0;
}

