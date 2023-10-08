#include <stdio.h>
#include <thread.h>
#include "ft2_header.h"
#include "ft2_edit.h"
#include "ft2_config.h"
#include "ft2_gui.h"
#include "ft2_midi.h"
#include "ft2_audio.h"
#include "ft2_mouse.h"
#include "ft2_pattern_ed.h"
#include "ft2_structs.h"
#include "rtmidi/rtmidi_c.h"

static struct RtMidiWrapper notrt;
static RtMidiCCallback callback;
static int mpfd = -1;

unsigned int
rtmidi_get_port_count(RtMidiPtr)
{
	if(access("/srv/midi", AREAD) < 0){
		notrt.ok = false;
		return 0;
	}
	notrt.ok = true;
	return 1;
}

char *
rtmidi_get_port_name(RtMidiPtr, unsigned int)
{
	char *s;

	if((s = strdup("/srv/midi")) == nil)
		sysfatal("strdup: %r");
	return s;
}

void
rtmidi_in_cancel_callback(RtMidiInPtr)
{
}

void rtmidi_close_port(RtMidiPtr)
{
	close(mpfd);
	mpfd = -1;
	callback = nil;
	notrt.ok = false;
}

void rtmidi_in_free(RtMidiInPtr)
{
}

RtMidiInPtr
rtmidi_in_create_default(void)
{
	notrt.ok = true;
	return &notrt;
}

void
midiproc(void *)
{
	int fd, n, k;
	uchar buf[1024];

	while((n = read(mpfd, buf, sizeof buf)) > 0){
		if(n & 3)
			fprint(2, "midiproc: malformed message size %d\n", n);
		for(k=0; k<n; k+=4)
			if(callback != nil)
				callback(.0, buf+k+1, 3, nil);
			else
				fprint(2, "midiproc: discarding message\n");
	}
	fprint(2, "midiproc is off this merry-go-round: %r\n");
	mpfd = -1;
}

void
rtmidi_open_port(RtMidiPtr, unsigned int, char *)
{
	notrt.ok = true;
	if((mpfd = open("/srv/midi", OREAD)) < 0){
		notrt.ok = false;
		return;
	}
	if(procrfork(midiproc, nil, mainstacksize, RFFDG) < 0)
		sysfatal("proccreate: %r");
}

void
rtmidi_in_set_callback(RtMidiInPtr, RtMidiCCallback fn, void *)
{
	callback = fn;
}

void
rtmidi_in_ignore_types(RtMidiInPtr, bool, bool, bool)
{
}
