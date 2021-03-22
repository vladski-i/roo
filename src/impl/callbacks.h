#include <jack/types.h>


/*UNROO CALLBACK FUNCTION + HELPERS */
 void 
 unroo_zero_fill(jack_default_audio_sample_t *in1,
				jack_default_audio_sample_t *in2,
				jack_default_audio_sample_t * out,
				jack_nframes_t nframes);

void 
unroo_no_fill(jack_default_audio_sample_t *in1,
				jack_default_audio_sample_t *in2,
				jack_default_audio_sample_t * out,
				jack_nframes_t nframes);
int
unroo_callback (jack_nframes_t nframes, void *arg);



/*ROO CALLBACK FUNCTION + HELPERS*/
void 
zero_fill(jack_default_audio_sample_t *in,
				jack_default_audio_sample_t *out1,
				jack_default_audio_sample_t * out2,
				jack_nframes_t nframes);

void 
no_fill(jack_default_audio_sample_t *in,
				jack_default_audio_sample_t *out1,
				jack_default_audio_sample_t * out2,
				jack_nframes_t nframes);

int
roo_callback (jack_nframes_t nframes, void *arg);