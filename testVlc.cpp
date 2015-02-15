#include <vlc/libvlc.h>
#include <vlc/vlc.h>
#include <unistd.h>
#include <stddef.h>

int main(int argc, char **argv)
{
	libvlc_instance_t *vlc;
	const char *url;
	const char *sout = "#transcode{acodec=mp3,ab=128,channels=2," \
			    "samplerate=44100}:http{dst=:8090/go.mp3}";
	const char *media_name = "Foo";

	if (argc != 2) {
		return 1;
	}
	url = argv[1];

	vlc = libvlc_new(0, NULL);
	libvlc_vlm_add_broadcast(vlc, media_name, url, sout, 0, NULL, true, false);
	libvlc_vlm_play_media(vlc, media_name);

	sleep(60); /* Let it play for a minute */

	libvlc_vlm_stop_media(vlc, media_name);
	libvlc_vlm_release(vlc);
	return 0;
}
