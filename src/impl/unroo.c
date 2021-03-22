/** @file simple_client.c
 *
 * @brief This simple client demonstrates the basic features of JACK
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

jack_port_t *output_port, *input_port1, *input_port2;
jack_client_t *client;

#ifndef M_PI
#define M_PI  (3.14159265)
#endif


static void signal_handler(int sig)
{
	jack_client_close(client);
	fprintf(stderr, "signal received, exiting ...\n");
	exit(0);
}

/**
 * The process callback for this JACK application is called in a
 * special realtime thread once for each audio cycle.
 *
 * This client follows a simple rule: when the JACK transport is
 * running, copy the input port to the output.  When it stops, exit.
 */
 void zero_fill(jack_default_audio_sample_t *in1,
				jack_default_audio_sample_t *in2,
				jack_default_audio_sample_t * out,
				jack_nframes_t nframes){
	for (int i = 0; i < nframes; i ++){
		out[i] = in1[i] + in2[i];
	}

}

void no_fill(jack_default_audio_sample_t *in1,
				jack_default_audio_sample_t *in2,
				jack_default_audio_sample_t * out,
				jack_nframes_t nframes){
	static int roo = 0;
	int init_roo = roo;
	for (int i = 0; i < nframes; i ++){
		if(roo % 2){
			out[i] = in1[(i + init_roo)/ 2];
		}
		else{
			out[i] = in2[(i + init_roo)/ 2];
		}
		roo++;
		roo = roo % 2;
	}

}
int
process_callback (jack_nframes_t nframes, void *arg)
{
	jack_default_audio_sample_t *out, *in1, *in2;
	int i;
	
	out = (jack_default_audio_sample_t*)jack_port_get_buffer (output_port, nframes);
	in1 = (jack_default_audio_sample_t*)jack_port_get_buffer (input_port1, nframes);
	in2 = (jack_default_audio_sample_t*)jack_port_get_buffer (input_port2, nframes);

    no_fill(in1, in2, out, nframes);
	return 0;      
}

/**
 * JACK calls this shutdown_callback if the server ever shuts down or
 * decides to disconnect the client.
 */
void
jack_shutdown (void *arg)
{
	exit (1);
}

int
main (int argc, char *argv[])
{
	const char **ports;
	const char *client_name;
	const char *server_name = NULL;
	jack_options_t options = JackNullOption;
	jack_status_t status;
	int i;

	if (argc >= 2) {		/* client name specified? */
		client_name = argv[1];
		if (argc >= 3) {	/* server name specified? */
			server_name = argv[2];
			int my_option = JackNullOption | JackServerName;
			options = (jack_options_t)my_option;
		}
	} else {			/* use basename of argv[0] */
		client_name = strrchr(argv[0], '/');
		if (client_name == 0) {
			client_name = argv[0];
		} else {
			client_name++;
		}
	}
  

	/* open a client connection to the JACK server */

	client = jack_client_open (client_name, options, &status, server_name);
	if (client == NULL) {
		fprintf (stderr, "jack_client_open() failed, "
			 "status = 0x%2.0x\n", status);
		if (status & JackServerFailed) {
			fprintf (stderr, "Unable to connect to JACK server\n");
		}
		exit (1);
	}
	if (status & JackServerStarted) {
		fprintf (stderr, "JACK server started\n");
	}
	if (status & JackNameNotUnique) {
		client_name = jack_get_client_name(client);
		fprintf (stderr, "unique name `%s' assigned\n", client_name);
	}

	/* tell the JACK server to call `process()' whenever
	   there is work to be done.
	*/

	jack_set_process_callback (client, process_callback, NULL);

	/* tell the JACK server to call `jack_shutdown()' if
	   it ever shuts down, either entirely, or if it
	   just decides to stop calling us.
	*/

	jack_on_shutdown (client, jack_shutdown, 0);

	/* create two ports */

	output_port = jack_port_register (client, "output",
					  JACK_DEFAULT_AUDIO_TYPE,
					  JackPortIsOutput, 0);

	input_port1 = jack_port_register (client, "input1",
					  JACK_DEFAULT_AUDIO_TYPE,
					  JackPortIsInput, 0);

	input_port2 = jack_port_register (client, "input2",
					  JACK_DEFAULT_AUDIO_TYPE,
					  JackPortIsInput, 0);

	if ((output_port == NULL) || (input_port1 == NULL)) {
		fprintf(stderr, "no more JACK ports available\n");
		exit (1);
	}

	/* Tell the JACK server that we are ready to roll.  Our
	 * process() callback will start running now. */

	if (jack_activate (client)) {
		fprintf (stderr, "cannot activate client");
		exit (1);
	}

	/* Connect the ports.  You can't do this before the client is
	 * activated, because we can't make connections to clients
	 * that aren't running.  Note the confusing (but necessary)
	 * orientation of the driver backend ports: playback ports are
	 * "input" to the backend, and capture ports are "output" from
	 * it.
	 */
 	
	ports = jack_get_ports (client, NULL, NULL,
				JackPortIsPhysical|JackPortIsInput);
	if (ports == NULL) {
		fprintf(stderr, "no physical playback ports\n");
		exit (1);
	}

	if (jack_connect (client, jack_port_name (output_port), ports[0])) {
		fprintf (stderr, "cannot connect output ports\n");
	}

	jack_free (ports);
    
    /* install a signal handler to properly quits jack client */
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

	while (1) {
	#ifdef WIN32 
		Sleep(1000);
	#else
		sleep (1);
	#endif
	}

	jack_client_close (client);
	exit (0);
}
