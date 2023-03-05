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
static int mpid = -1;
static char *epfile, *eptab[MAX_MIDI_DEVICES];
static int disable[MAX_MIDI_DEVICES];
static int neps;

static int
scaneps(void)
{
	int fd, i, n, m;
	char *s, **t, **e, buf[512], *fl[32];
	Dir *d;

	e = eptab + nelem(eptab);
	for(t=eptab; t<e; t++){
		free(*t);
		*t = nil;
	}
	neps = 0;
	t = eptab;
	/* special case for plugging in any non endpoint file */
	if((s = getenv("midikbd")) != nil){
		*t++ = s;
		neps++;
	}
	if((fd = open("/dev/usb", OREAD)) < 0){
		fprint(2, "scaneps: %r\n");
		return neps;
	}
	n = dirreadall(fd, &d);
	close(fd);
	if(n < 0){
		fprint(2, "scaneps: %r\n");
		return neps;
	}
	for(i=0; i<n; i++){
		snprint(buf, sizeof buf, "/dev/usb/%s/ctl", d[i].name);
		if(epfile != nil && (s = strrchr(epfile, '/')) != nil){
			if(strncmp(buf, epfile, s - epfile) == 0)
				goto gotit;
		}
		if((fd = open(buf, OREAD)) < 0)
			continue;
		if((m = pread(fd, buf, sizeof buf, 0)) <= 0)
			continue;
		close(fd);
		buf[m-1] = 0;
		if(getfields(buf, fl, nelem(fl), 0, " ") < 26)
			continue;
		if(strcmp(fl[0], "enabled") != 0
		|| strcmp(fl[2], "r") != 0 && strcmp(fl[2], "rw") != 0
		|| strcmp(fl[25], "idle") != 0)
			continue;
	gotit:
		if((*t++ = smprint("/dev/usb/%s/data", d[i].name)) == nil)
			sysfatal("smprint: %r\n");
		neps++;
		if(t >= e)
			break;
	}
	free(d);
	return neps;
}

unsigned int
rtmidi_get_port_count(RtMidiPtr)
{
	notrt.ok = true;
	return scaneps();
}

char *
rtmidi_get_port_name(RtMidiPtr, unsigned int i)
{
	char *s;

	if(i >= neps)
		return NULL;
	if((s = strdup(eptab[i])) == nil)
		sysfatal("strdup: %r");
	return s;
}

void
rtmidi_in_cancel_callback(RtMidiInPtr)
{
}

void rtmidi_close_port(RtMidiPtr)
{
	threadkill(mpid);
	mpid = -1;
	callback = nil;
	notrt.ok = false;
	epfile = nil;
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

	if((fd = open(epfile, OREAD)) < 0){
		fprint(2, "midiproc: could not open stream: %r; exiting");
		goto end;
	}
	while((n = read(fd, buf, sizeof buf)) > 0){
		if(n & 3)
			fprint(2, "midiproc: malformed message size %d\n", n);
		for(k=0; k<n; k+=4)
			if(callback != nil)
				callback(.0, buf+k+1, 3, nil);
			else
				fprint(2, "midiproc: discarding message\n");
	}
	fprint(2, "midiproc is off this merry-go-round: %r\n");
end:
	epfile = nil;
	notrt.ok = false;
	mpid = -1;
}

void
rtmidi_open_port(RtMidiPtr, unsigned int i, char *)
{
	assert(mpid < 0);
	if(i >= neps)
		return;
	notrt.ok = true;
	epfile = eptab[i];
	if((mpid = proccreate(midiproc, nil, mainstacksize)) < 0)
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
