/** @file roo.c
 *
 * @brief This simple roo_client demonstrates the basic features of JACK
 * as they would be used by many applications.
 */

#include <jack/types.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <jack/jack.h>
#include "startup.h"
#include "callbacks.h"
#include "config.h"

jack_port_t *roo_out1, *roo_out2, *roo_in;
jack_port_t *unroo_out, *unroo_in1, *unroo_in2;
jack_client_t *roo_client, *unroo_client;
config_t *args;

#ifndef M_PI
#define M_PI (3.14159265)
#endif

/* Handles exit signals; Free resources here*/
static void
signal_handler(int sig)
{
	jack_client_close(roo_client);
	jack_client_close(unroo_client);
	free(args);
	fprintf(stderr, "signal received, exiting ...\n");
	exit(0);
}

/**
 * JACK calls this shutdown_callback if the server ever shuts down or
 * decides to disconnect the roo/unroo_client.
 */
void jack_shutdown(void *arg)
{
	exit(1);
}

int main(int argc, char *argv[])
{
	const char *roo_client_name = "roo";
	const char *unroo_client_name = "unroo";
	const char *server_name = NULL;
	jack_options_t options = JackNullOption;
	jack_status_t status;
	args = malloc(sizeof(config_t));
	int i;
	/* initialize args*/
	args->mode = ZERO_FILL;
	args->window_size = 1;
	/* open a client connection to the JACK server */
	if (create_client(&roo_client, roo_client_name, options, server_name))
	{
		exit(1);
	}

	if (create_client(&unroo_client, unroo_client_name, options, server_name))
	{
		exit(1);
	}

	/* tell the JACK server to call the correct callback for each client whenever
	   there is work to be done.
	*/

	jack_set_process_callback(roo_client, roo_callback, args);
	jack_set_process_callback(unroo_client, unroo_callback, args);

	/* tell the JACK server to call `jack_shutdown()' if
	   it ever shuts down, either entirely, or if it
	   just decides to stop calling us.
	*/

	jack_on_shutdown(roo_client, jack_shutdown, 0);
	jack_on_shutdown(unroo_client, jack_shutdown, 0);

	/* create two ports */

	roo_out1 = jack_port_register(roo_client, "output1",
								  JACK_DEFAULT_AUDIO_TYPE,
								  JackPortIsOutput, 0);

	roo_out2 = jack_port_register(roo_client, "output2",
								  JACK_DEFAULT_AUDIO_TYPE,
								  JackPortIsOutput, 0);

	roo_in = jack_port_register(roo_client, "input",
								JACK_DEFAULT_AUDIO_TYPE,
								JackPortIsInput, 0);

	unroo_out = jack_port_register(unroo_client, "output",
								   JACK_DEFAULT_AUDIO_TYPE,
								   JackPortIsOutput, 0);

	unroo_in1 = jack_port_register(unroo_client, "input1",
								   JACK_DEFAULT_AUDIO_TYPE,
								   JackPortIsInput, 0);

	unroo_in2 = jack_port_register(unroo_client, "input2",
								   JACK_DEFAULT_AUDIO_TYPE,
								   JackPortIsInput, 0);

	if ((roo_out1 == NULL) || (roo_in == NULL))
	{
		fprintf(stderr, "no more JACK ports available\n");
		exit(1);
	}

	/* Tell the JACK server that we are ready to roll.  Our
	 * process() callback will start running now. */

	if (jack_activate(roo_client))
	{
		fprintf(stderr, "cannot activate roo_client");
		exit(1);
	}
	if (jack_activate(unroo_client))
	{
		fprintf(stderr, "cannot activate unroo_client");
		exit(1);
	}
	/* install a signal handler to properly quits jack roo_client */
#ifdef WIN32
	signal(SIGINT, signal_handler);
	signal(SIGABRT, signal_handler);
	signal(SIGTERM, signal_handler);
#else
	signal(SIGQUIT, signal_handler);
	signal(SIGTERM, signal_handler);
	signal(SIGHUP, signal_handler);
	signal(SIGINT, signal_handler);
#endif

	/* keep running until the Ctrl+C */

	while (1)
	{
#ifdef WIN32
		Sleep(1000);
#else
		sleep(1);
#endif
	}

	jack_client_close(roo_client);
	exit(0);
}
